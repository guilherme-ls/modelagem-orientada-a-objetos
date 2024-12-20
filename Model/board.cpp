#include "board.hpp"

/**
 * @brief opens database (file)
 */
void Board::openDB() {
    database = fopen("database.txt", "w+");
}

/**
 * @brief closes database (file)
 */
void Board::closeDB() {
    fclose(database);
    database = NULL;
}

/**
 * @brief saves board data when called
 */
void Board::saveData() {
    if(database == NULL)
        return;

    fprintf(database, "%d, %d, %d", matchID, players, player_turn);
    for(int i = 0; i < positions.size(); i++) {
        for(int j = 0; j < 4; j++) {
            for(int k = 0; k < 4; k++) {
                fprintf(database, ", %d", positions[i][j][k]);
            }
        }
    }
    fprintf(database, "\n");
}

/* gets and sets */

int Board::getPieceAt(vector3 pos) {
    return positions[pos.i][pos.j][pos.k];
}

void Board::setPositions(std::vector<std::array<std::array<int, 4>, 4>> pos) {
    positions = pos;
}

std::vector<std::array<std::array<int, 4>, 4>> Board::getPositions() {
    return positions;
}
std::vector<std::array<std::array<bool, 4>, 4>> Board::getHighlights() {
    return highlights;
}

void Board::setPlayers(int play) {
    players = play;
}

int Board::getPlayers() {
    return players;
}

void Board::setPlayerTurn(int turn) {
    player_turn = turn;
}

int Board::getPlayerTurn() {
    return player_turn;
}

void Board::setMyPlayer(int player) {
    my_player = player;
}

int Board::getMyPlayer() {
    return my_player;
}

/* general logic */

/**
 * @brief checks which players have already lost the game and organizes active_player list
 * @param positions piece position matrix
 * @param active active board.players vector
 */
void Board::check_lost() {
    active_player.clear();
    for(int play = 0; play < positions.size()/2; play++) {
        bool active = false;
        for(int i = 0; i < positions.size(); i++) {
            for(int j = 0; j < 4; j++) {
                for(int k = 0; k < 4; k++) {
                    int piece = positions[i][j][k];
                    if(piece != 0 && (piece - piece % 10) / 10 == play) {
                        active = true;
                        break;
                    }
                }
                if(active)
                    break;
            }
            if(active)
                break;
        }
        active_player.emplace_back(active);
    }
}

bool Board::isPieceValid(vector3 src) {
    return (positions[src.i][src.j][src.k] - positions[src.i][src.j][src.k] % 10) / 10 == player_turn;
}

bool Board::isHighlighted(vector3 src){
    return highlights[src.i][src.j][src.k];
}

void Board::initializePositions() {
    for(int i = 0; i < players * 2; i++) {
        std::array<std::array<int, 4>, 4> temp;
        if(i % 2 == 1) {
            temp = {(std::array<int, 4>){0, 0, 6, 2}, (std::array<int, 4>){0, 0, 6, 4}, (std::array<int, 4>){0, 0, 6, 5}, (std::array<int, 4>){0, 0, 6, 3}};
            for(int j = 0; j < 4; j++) {
                for(int k = 2; k < 4; k++) {
                    temp[j][k] += 10 * (i - i % 2) / 2;
                }
            }
        }
        else {
            temp = {(std::array<int, 4>){0, 0, 0, 0}, (std::array<int, 4>){0, 0, 0, 0}, (std::array<int, 4>){6, 6, 6, 6}, (std::array<int, 4>){1, 4, 5, 3}};
            for(int j = 2; j < 4; j++) {
                for(int k = 0; k < 4; k++) {
                    temp[j][k] += 10 * (i - i % 2) / 2;
                }
            }
        }
        positions.emplace_back(temp);
    }
}

/**
 * @brief moves pieces in the board
 * @param src source position
 * @param dst destination position
 */
void Board::move(vector3 src, vector3 dst) {
    // destroys oponent if the king is captured
    ifKingDestroy(dst);

    // executes movement
    positions[dst.i][dst.j][dst.k] = positions[src.i][src.j][src.k];
    positions[src.i][src.j][src.k] = 0;

    // passes turn after movement
    passTurn();

    // saves in database
    saveData();
}

/**
 * @brief destroys player if king is captured
 * @param pos position to check if it is king
 */
void Board::ifKingDestroy(vector3 pos) {
    // if the king is destroyed, player loses
    int piece = positions[pos.i][pos.j][pos.k];
    if(piece % 10 == 1) {
        int player = (piece - piece % 10) / 10;
        active_player[player] = false;
        for(int i = 0; i < players * 2; i++)
            for(int j = 0; j < 4; j++)
                for(int k = 0; k < 4; k++)
                    if((positions[i][j][k] - positions[i][j][k] % 10) / 10 == player)
                        positions[i][j][k] = 0;
    }
}

void Board::passTurn() {
    // passes turn
    player_turn = (player_turn + 1) % players;
    // if next player already lost, goes to the next
    while(!active_player[player_turn])
        player_turn = (player_turn + 1) % players;
}

/**
 * @brief clears model attributes
 */
void Board::clear() {
    my_player = 0;
    player_turn = 0;
    positions.clear();
    highlights.clear();
    active_player.clear();
    for(int i = 0; i < players; i++)
        active_player.emplace_back(true);
    matchID = time(NULL);
}

/**
 * @brief fills highlights matrix with false
 */
void Board::fillFalseHighlights() {
    highlights.clear();
    std::array<std::array<bool, 4>, 4> false_matrix_fraction = {(std::array<bool, 4>){0, 0, 0, 0}, (std::array<bool, 4>){0, 0, 0, 0}, (std::array<bool, 4>){0, 0, 0, 0}, (std::array<bool, 4>){0, 0, 0, 0}};
    // fills everything with false
    for(int k = 0; k < players * 2; k++) {
        highlights.emplace_back(false_matrix_fraction);
    }
}

/*
=============================================================
Starting from here, everything is used to calculate movements
=============================================================
*/

void Board::calculateMoves(vector3 src) {
    switch(Board::positions[src.i][src.j][src.k] % 10) {
        case 1:
            calculateKing(src);
            break;
        case 2:
            calculateQueen(src);
            break;
        case 3:
            calculateRook(src);
            break;
        case 4:
            calculateBishop(src);
            break;
        case 5:
            calculateHorse(src);
            break;
        case 6:
            calculatePawn(src);
            break;
        default:
            break;
    }
}

/**
 * @brief calculates direction of the "top"
 * @param i fraction of the board the piece is in
 * @param piece code of the piece
 * @return direction in a <x, y> pair
 */
std::pair<int, int> calculateDirection(int i, int piece) {
    if(i % 2 == 0) {
        if((i - i % 2) / 2 == (piece - piece % 10) / 10)
            return std::make_pair(-1, 0);
        else
            return std::make_pair(1, 0);
    }
    else {
        if((i - i % 2) / 2 == (piece - piece % 10) / 10)
            return std::make_pair(0, -1);
        else
            return std::make_pair(0, 1);
    }
}
// j = x, k = y.

bool Board::checkBlock(int player, vector3 dest) {
    int piece = positions[dest.i][dest.j][dest.k];
    // checks if the way is blocked
    if(piece != 0) {
        // reaches piece of that can be eliminated
        if(player != (piece - piece % 10) / 10) {
            highlights[dest.i][dest.j][dest.k] = true;
        }
        return true;
    }
    return false;
}

void Board::moveDirection(bool recursive, int player, std::pair<int, int> direction, vector3 src) {
    // updates position
    vector3 dest = {src.i, src.j + direction.first, src.k + direction.second};
    // checks if position is valid
    if(dest.j > 3 || dest.k > 3)
        return;
    // special case for diagonal moves
    if (src.j == 0 && src.k == 0 && dest.j == -1 && dest.k == -1) {
        direction = std::make_pair(1, 1);
        for(int pos = src.i % 2; pos < players * 2; pos += 2) {
            if(pos == src.i)
                continue;

            dest = {pos, 0, 0};
            if(checkBlock(player, dest))
                continue;

            if(positions[src.i][src.j][src.k] % 10 != 6)
                highlights[pos][0][0] = true;
            if(recursive)
                moveDirection(recursive, player, direction, dest);
        }

        return;
    }
    // horse movement
    if(dest.j < 0 && dest.k < 0) {
        dest = {(src.i + players * 2 - 2) % (players * 2), -dest.j - 1, -dest.k - 1};
        if(!checkBlock(player, dest))
            highlights[dest.i][dest.j][dest.k] = true;
        
        dest = {(src.i + 2) % (players * 2), dest.j, dest.k};
        if(!checkBlock(player, dest))
            highlights[dest.i][dest.j][dest.k] = true;
        
        return;
    }
    // general cases
    if(dest.j < 0) {
        dest = {(src.i + players * 2 - 1) % (players * 2), dest.k, -dest.j - 1};
        direction = std::make_pair(direction.second, -direction.first);
    }
    else if(dest.k < 0) {
        dest = {(src.i + 1) % (players * 2), -dest.k - 1, dest.j};
        direction = std::make_pair(-direction.second, direction.first);
    }
    
    // checks if another piece blocks the path
    if(checkBlock(player, dest)) {
        // overwrite collision if pawn
        if(positions[src.i][src.j][src.k] % 10 == 6 && abs(direction.first) != abs(direction.second))
            highlights[dest.i][dest.j][dest.k] = false;
        return;
    }
    
    // last update to position
    if(positions[src.i][src.j][src.k] % 10 != 6 || abs(direction.first) != abs(direction.second))
        highlights[dest.i][dest.j][dest.k] = true;
    // continues moving if recursive
    if(recursive) {
        moveDirection(recursive, player, direction, dest);
    }
}

void Board::calculatePawn(vector3 src) {
    int piece = positions[src.i][src.j][src.k];
    int player = (piece - piece % 10) / 10;
    std::pair<int, int> direction = calculateDirection(src.i, piece);
    std::pair<int, int> direction_double = direction;
    std::pair<int, int> direction_eat1 = direction;
    std::pair<int, int> direction_eat2 = direction;
    
    // normal movement
    moveDirection(false, player, direction, src);

    
    // double movement
    if(src.j == 2 && direction.first == -1) {
        direction_double.first = -2;
    }
    else if(src.k == 2 && direction.second == -1) {
        direction_double.second = -2;
    }
    moveDirection(false, player, direction_double, src);
    
    // eat piece movement
    if(direction.first != 0) {
        direction_eat1.second = 1;
        direction_eat2.second = -1;
    }
    if(direction.second != 0) {
        direction_eat1.first = 1;
        direction_eat2.first = -1;
    }
    moveDirection(false, player, direction_eat1, src);
    moveDirection(false, player, direction_eat2, src);
}

void Board::calculateRook(vector3 src) {
    int piece = positions[src.i][src.j][src.k];
    int player = (piece - piece % 10) / 10;
    moveDirection(true, player, std::make_pair(1, 0), src);
    moveDirection(true, player, std::make_pair(-1, 0), src);
    moveDirection(true, player, std::make_pair(0, 1), src);
    moveDirection(true, player, std::make_pair(0, -1), src);
}

void Board::calculateHorse(vector3 src) {
    int piece = positions[src.i][src.j][src.k];
    int player = (piece - piece % 10) / 10;
    moveDirection(false, player, std::make_pair(2, 1), src);
    moveDirection(false, player, std::make_pair(2, -1), src);
    moveDirection(false, player, std::make_pair(-2, 1), src);
    moveDirection(false, player, std::make_pair(-2, -1), src);
    moveDirection(false, player, std::make_pair(1, 2), src);
    moveDirection(false, player, std::make_pair(-1, 2), src);
    moveDirection(false, player, std::make_pair(1, -2), src);
    moveDirection(false, player, std::make_pair(-1, -2), src);
}

void Board::calculateBishop(vector3 src) {
    int piece = positions[src.i][src.j][src.k];
    int player = (piece - piece % 10) / 10;
    moveDirection(true, player, std::make_pair(1, 1), src);
    moveDirection(true, player, std::make_pair(-1, 1), src);
    moveDirection(true, player, std::make_pair(1, -1), src);
    moveDirection(true, player, std::make_pair(-1, -1), src);
}

void Board::calculateQueen(vector3 src) {    
    int piece = positions[src.i][src.j][src.k];
    int player = (piece - piece % 10) / 10;
    moveDirection(true, player, std::make_pair(1, 0), src);
    moveDirection(true, player, std::make_pair(-1, 0), src);
    moveDirection(true, player, std::make_pair(0, 1), src);
    moveDirection(true, player, std::make_pair(0, -1), src);
    moveDirection(true, player, std::make_pair(1, 1), src);
    moveDirection(true, player, std::make_pair(-1, 1), src);
    moveDirection(true, player, std::make_pair(1, -1), src);
    moveDirection(true, player, std::make_pair(-1, -1), src);
}

void Board::calculateKing(vector3 src) {
    int piece = positions[src.i][src.j][src.k];
    int player = (piece - piece % 10) / 10;
    moveDirection(false, player, std::make_pair(1, 0), src);
    moveDirection(false, player, std::make_pair(-1, 0), src);
    moveDirection(false, player, std::make_pair(0, 1), src);
    moveDirection(false, player, std::make_pair(0, -1), src);
    moveDirection(false, player, std::make_pair(1, 1), src);
    moveDirection(false, player, std::make_pair(-1, 1), src);
    moveDirection(false, player, std::make_pair(1, -1), src);
    moveDirection(false, player, std::make_pair(-1, -1), src);
}