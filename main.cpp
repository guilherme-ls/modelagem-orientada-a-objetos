// include other parts
#include "controller.hpp"

int main() {
    // initializes window and main components
    Controller* gameController = new Controller();

    gameController->gameLoop();

    // correct execution
    return 0;
}