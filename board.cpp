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

/**
 * @brief clears model attributes
 */
void Board::clear() {
    my_player = 0;
    player_turn = 0;
    positions.clear();
    highlights.clear();
    active_player.clear();
    matchID = time(NULL);
}
