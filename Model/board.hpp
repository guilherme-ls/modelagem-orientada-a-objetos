#include<vector>
#include<cstdlib>
#include<cstdio>
#include<array>
#include<time.h>

typedef struct {
    int i, j, k;
} vector3;

class Board {
    public:
        std::vector<bool> active_player;

        Board() {
            players = 3;
            player_turn = 0;
            my_player = 0;
            matchID = time(NULL);
        }

        int getPieceAt(vector3 pos);
        void setPositions(std::vector<std::array<std::array<int, 4>, 4>> pos);
        std::vector<std::array<std::array<int, 4>, 4>> getPositions();
        std::vector<std::array<std::array<bool, 4>, 4>> getHighlights();
        void setPlayers(int players);
        int getPlayers();
        void setPlayerTurn(int player_turn);
        int getPlayerTurn();
        void setMyPlayer(int my_player);
        int getMyPlayer();

        void openDB();
        void saveData();
        void check_lost();
        void initializePositions();
        void calculateMoves(vector3 src);
        void move(vector3 src, vector3 dst);
        bool isPieceValid(vector3 src);
        bool isHighlighted(vector3 src);
        void fillFalseHighlights();
        void closeDB();
        void clear();

    private:
        int players;
        int player_turn;
        int my_player;
        std::vector<std::array<std::array<int, 4>, 4>> positions;
        std::vector<std::array<std::array<bool, 4>, 4>> highlights;
        int matchID;

        void passTurn();
        void ifKingDestroy(vector3 pos);
        FILE *database = NULL;

        bool checkBlock(int player, vector3 dest);
        void moveDirection(bool recursive, int player, std::pair<int, int> direction, vector3 src);

        void calculatePawn(vector3 src);
        void calculateRook(vector3 src);
        void calculateHorse(vector3 src);
        void calculateBishop(vector3 src);
        void calculateQueen(vector3 src);
        void calculateKing(vector3 src);
};