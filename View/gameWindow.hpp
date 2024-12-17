#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <array>
#include <iostream>
#include "map.hpp"

class GameWindow {
    private:
        unsigned int screenWidth;
        unsigned int midScreenWidth;
        unsigned int screenHeight;
        unsigned int midScreenHeight;
        unsigned int fps_target;
        bool fps_cap;
        
        Rectangle drawButton(std::string text, Vector2 position, float scale, bool draw_rec);
        
    public:
        Font standard_font = LoadFont("resources/fonts/pixantiqua.png");

        Camera2D camera;

        GameWindow(unsigned int screenWidth, unsigned int screenHeight, unsigned int fps_target, bool fps_cap) {
            GameWindow::screenWidth = screenWidth;
            GameWindow::midScreenWidth = screenWidth/2;
            GameWindow::screenHeight = screenHeight;
            GameWindow::midScreenHeight= screenHeight/2;
            GameWindow::fps_target = fps_target;
            GameWindow::fps_cap = fps_cap;
        }

        std::vector<Rectangle> menuDrawLoop(int player_number, Map* decorativeMap);

        int gameDrawLoop(int my_player, int player_turn, int players, std::vector<std::array<std::array<bool, 4>, 4>> highlights, std::vector<std::array<std::array<int, 4>, 4>> positions, Map* gameMap);

        void initialize();

        void generalDraw();

        void finalize();

        void cameraUpdate();

        bool checkMapInput(int i, int j, int k, Map *gameMap);

        int checkMenuClicks(std::vector<Rectangle> buttons, int players);
};