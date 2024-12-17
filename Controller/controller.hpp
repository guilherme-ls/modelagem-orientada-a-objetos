#include <cstdlib>
#include <vector>
#include <array>
#include <mutex>
#include <sstream>
#include <thread>
#include "board.hpp"
#include "gameWindow.hpp"
#include "sockets.hpp"

typedef struct {
    int i, j, k;
} vector3;

class Controller {
    public:
        void gameLoop();

        Controller() {
            // initializes view
            window->initialize();
            decorativeMap->initializeMap(6);
            sock->configureMessaging(&mutex_alter_inbound_messages, &mutex_alter_outbound_messages, &inbound_messages, &outbound_messages, &(board.player_turn), &(board.positions));

            // loads game textures
            gameMap->loadTextures();
        }

    private:
        std::mutex mutex_alter_inbound_messages;
        std::mutex mutex_alter_outbound_messages;
        std::vector<std::string> inbound_messages;
        std::vector<std::string> outbound_messages;

        vector3 previous_piece = {10, 10, 10};

        // view standard objects
        GameWindow* window = new GameWindow(1200, 780, 60, true);
        Map* gameMap = new Map(200);
        Map* decorativeMap = new Map(150);
        Sockets* sock = new Sockets("127.0.0.1", 128, 4277);
        std::thread network_thread;
        Board board;

        void initialize_positions();

        void receiveMessages();

        void move(vector3 src, vector3 dst);

        void calculateMoves(vector3 src);

        void clearController();

        void fillFalseMatrix();

        void menuLoop(int *gameState);

        int checkMenuClicks(Vector2 mouse, std::vector<Rectangle> buttons);
        void checkMapClicks(Vector2 mouse);

        bool checkBlock(int player, vector3 dest);
        void moveDirection(bool recursive, int player, std::pair<int, int> direction, vector3 src);

        void calculatePawn(vector3 src);
        void calculateRook(vector3 src);
        void calculateHorse(vector3 src);
        void calculateBishop(vector3 src);
        void calculateQueen(vector3 src);
        void calculateKing(vector3 src);

        void executeMessage(std::string msg);
        void sendMessages(std::string msg);
};