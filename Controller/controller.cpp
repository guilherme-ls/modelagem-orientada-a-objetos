#include "controller.hpp"

void Controller::menuLoop(int *gameState) {
    std::vector<Rectangle> buttons = window->menuDrawLoop(board.getPlayers(), decorativeMap);
    *gameState = window->checkMenuClicks(buttons, board.getPlayers()) + 1;
    // start game
    if(*gameState > 0 && *gameState < 3) {
        clearController();
        sock->clearSocket();
        gameMap->initializeMap(board.getPlayers() * 2);

        // begin game as server
        if(*gameState == 1) {
            if(sock->startServer() == -1) {
                printf("Error starting server\n");
                *gameState = 0;
            }
            else {
                network_thread = std::thread(&Sockets::realServerLoop, sock);
            }
        }
        // begin game as client
        else {
            if(sock->startClient() == -1) {
                printf("Error starting client\n");
                *gameState = 0;
            }
            else {
                network_thread = std::thread(&Sockets::realClientLoop, sock);
            }
        }
    }
    // changes number of board.players
    if(*gameState > 2) {
        board.setPlayers(board.getPlayers() + (*gameState * 2 - 7));
        *gameState = 0;
    }
}

void Controller::gameLoop() {
    // var to determine if the system is in the menu or in game
    int gameState = 0;
    
    // opens model database
    board.openDB();

    // loop while window is not closed
    while(!WindowShouldClose()) {
        // menu loop
        if(gameState == 0) {
            menuLoop(&gameState);
        }
        // game loop
        else {
            // updates gameWindow camera
            window->cameraUpdate();

            // receives messages from conected peers
            receiveMessages();

            // deals with mouse clicks
            checkMapClicks();

            // draws window (and returns to menu if "<" button is pressed)
            if(window->gameDrawLoop(board.getMyPlayer(), board.getPlayerTurn(), board.getPlayers(), board.getHighlights(), board.getPositions(), gameMap) == 1) {
                printf("Returning to menu\n");
                sock->endConnection(&network_thread);
                gameState = 0;
            }

            // game execution needs to be halted
            else if(sock->halt_loop) {
                printf("Connection error, returning to menu\n");
                sock->endConnection(&network_thread);
                gameState = 0;
            }
        }
    }

    // stop game
    gameMap->unloadTextures();
    window->finalize();
    if(!sock->halt_loop && gameState != 0)
        sock->endConnection(&network_thread);

    // closes model database
    board.closeDB();
}

/**
 * @brief clear all controller variables
 */
void Controller::clearController() {
    board.clear();
    inbound_messages.clear();
    outbound_messages.clear();
    board.fillFalseHighlights();
    board.initializePositions();
    board.saveData();
}

/**
 * @brief deals with any input in the map
 */
void Controller::checkMapClicks() {
    // checks if it is the correct turn
    if(board.getPlayerTurn() != board.getMyPlayer())
        return;

    // loops over each fraction of the map
    for(int i = 0; i < gameMap->fractions; i++) {
        // checks collision with each point
        for(int j = 0; j < 4; j++) {
            for(int k = 0; k < 4; k++) {
                // checks inputs in View
                bool input = window->checkMapInput(i, j, k, gameMap);

                // deals with inputs (if detected)
                if(input) {
                    // move already selected piece to new position
                    if(board.isHighlighted((vector3){i, j, k})) {
                        board.fillFalseHighlights();
                        board.move(Controller::previous_piece, (vector3){i, j, k});
                        std::string msg = "mov " + std::to_string(previous_piece.i) + " " + std::to_string(previous_piece.j) + " " + std::to_string(previous_piece.k) + " " + std::to_string(i) + " " +std::to_string(j) + " " + std::to_string(k) + "\n";
                        sendMessages(msg);
                    }
                    // select piece
                    else if(board.isPieceValid((vector3){i, j, k})) {
                        board.fillFalseHighlights();
                        previous_piece = {i, j, k};
                        board.calculateMoves((vector3){i, j, k});
                    }
                    return;
                }
            }
        }
    }
}


void Controller::receiveMessages() {
    // deals with received messages
    mutex_alter_inbound_messages.lock();
    int size = inbound_messages.size();
    mutex_alter_inbound_messages.unlock();

    for(int i = 0; i < size; i++) {
        std::string msg;
        mutex_alter_inbound_messages.lock();
        auto iter = inbound_messages.cbegin();
        msg = *iter;
        inbound_messages.erase(iter);
        mutex_alter_inbound_messages.unlock();
        
        // deal with msg
        executeMessage(msg);
        std::cout << msg << std::endl;
    }
}

std::vector<std::array<std::array<int, 4>, 4>> Controller::desserialize(std::stringstream* stream) {
    std::vector<std::array<std::array<int, 4>, 4>> output;
    int fractions, turn;
    (*stream) >> turn >> fractions;
    board.setPlayerTurn(turn);
    for(int i = 0; i < fractions; i++) {
        std::array<std::array<int, 4>, 4> temp;
        for(int j = 0; j < 4; j++) {
            for(int k = 0; k < 4; k++) {
                (*stream) >> temp[j][k];
            }
        }
        output.emplace_back(temp);
    }

    return output;
}

/**
 * @brief updates game state according to message received through the network
 * @param msg message received
 */
void Controller::executeMessage(std::string msg) {
    if(msg.substr(0,3) == "mov") {
        std::stringstream stream;
        stream << msg.substr(3, msg.size() - 3);
        vector3 src, dst;
        stream >> src.i >> src.j >> src.k >> dst.i >> dst.j >> dst.k;
        board.move(src, dst);
    }
    else if(msg.substr(0,3) == "num") {
        std::stringstream stream;
        stream << msg.substr(3, msg.size() - 3);
        int my_player;
        stream >> my_player;
        board.setMyPlayer(my_player);
    }
    else if(msg.substr(0, 3) == "brd") {
        std::stringstream stream;
        stream << msg.substr(3, msg.size() - 3);
        std::vector<std::array<std::array<int, 4>, 4>> pos = desserialize(&stream);
        board.setPositions(pos);
        board.setPlayers(pos.size() / 2);
        gameMap->initializeMap(board.getPlayers() * 2);
        board.fillFalseHighlights();
        board.check_lost();
    }
    else if(msg.substr(0, 3) == "inf") {
        sendMessages(serialize());
    }
}

/**
 * @brief sends messages to ohter connections
 * @param msg string to be sent
 */
void Controller::sendMessages(std::string msg) {
    mutex_alter_outbound_messages.lock();
    outbound_messages.emplace_back(msg);
    mutex_alter_outbound_messages.unlock();
}

/**
 * @brief Serializes data in a string
 * @param board data to be serialized
 * @return serialized data in a string
 */
std::string Controller::serialize() {
    std::string output = "brd " + std::to_string(board.getPlayerTurn()) + " ";
    int fractions = board.getPlayers() * 2;
    output += std::to_string(fractions) + " ";
    for(int i = 0; i < fractions; i++) {
        for(int j = 0; j < 4; j++) {
            for(int k = 0; k < 4; k++) {
                output += std::to_string(board.getPieceAt((vector3){i, j, k})) + " ";
            }
        }
    }
    output += "\n";

    return output;
}