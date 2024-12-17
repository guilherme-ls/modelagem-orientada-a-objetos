#include<vector>
#include<cstdlib>
#include<cstdio>
#include<array>
#include<time.h>

class Board {
    public:
        int players;
        int player_turn;
        int my_player;
        std::vector<bool> active_player;
        std::vector<std::array<std::array<int, 4>, 4>> positions;
        std::vector<std::array<std::array<bool, 4>, 4>> highlights;
        int matchID;

        Board() {
            players = 3;
            player_turn = 0;
            my_player = 0;
            matchID = time(NULL);
        }

        void openDB();
        void saveData();
        void closeDB();

        void clear();

    private:
        FILE *database = NULL;
};