#include "gameWindow.hpp"

void GameWindow::initialize() {
    // sets antialiasing flag
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    // initializes window with target resolution
    InitWindow(GameWindow::screenWidth, GameWindow::screenHeight, "Great Chess Game!");
    
    // sets target fps if option is selected
    if(GameWindow::fps_cap)
        SetTargetFPS(GameWindow::fps_target);
    
    // initializes camera
    GameWindow::camera = { 0 };
    GameWindow::camera.target = (Vector2){0, 0};
    GameWindow::camera.offset = (Vector2){600, 390};
    GameWindow::camera.rotation = 0.0f;
    GameWindow::camera.zoom = 1.0f;
}

void GameWindow::finalize() {
    CloseWindow();
}

/**
 * @brief Draws button with the given specifications
 */
Rectangle GameWindow::drawButton(std::string text, Vector2 position, float scale, bool draw_rec) {
    Vector2 offset = MeasureTextEx(GetFontDefault(), text.c_str(), scale, scale / 8);
    position = Vector2Subtract(position, Vector2Scale(offset, 0.5));
    Rectangle rec = {position.x - offset.x * 0.1f, position.y - offset.y * 0.1f, offset.x * 1.2f, offset.y * 1.2f};
    if(draw_rec)
        DrawRectangleLinesEx(rec, scale / 8, BLACK);
    DrawTextEx(standard_font, text.c_str(), position, scale, scale / 8, BLACK);

    return rec;
}

std::vector<Rectangle> GameWindow::menuDrawLoop(int player_number, Map* decorativeMap) {
    // starts drawing call
    BeginDrawing();
    generalDraw();

    camera.target = (Vector2){0, 0};
    camera.zoom = 1.0f;
    camera.offset = (Vector2){600, 180};
    // begins part affected by camera
    BeginMode2D(camera);

    // draws decorative map on top of the menu
    decorativeMap->drawMap();
    drawButton("MULTICHESS", {0, 0}, (float)(screenWidth / 11), false);

    // end part affected by camera
    EndMode2D();
    camera.offset = (Vector2){600, 390};

    // Draws on-screen buttons
    std::vector<Rectangle> buttons;
    buttons.emplace_back(drawButton("Host Game", {(float)(screenWidth / 2), (float)(screenHeight) / 1.538f}, (float)(screenWidth / 20), true));
    buttons.emplace_back(drawButton("Join Game", {(float)(screenWidth / 2), (float)(screenHeight) / 1.25f}, (float)(screenWidth / 20), true));
    if(player_number > 2)
        buttons.emplace_back(drawButton("<", {(float)(screenWidth) / 3.33f, (float)(screenHeight) / 2.0f}, (float)(screenWidth / 20), false));
    if(player_number < 6)
        buttons.emplace_back(drawButton(">", {(float)(screenWidth) / 1.428f, (float)(screenHeight) / 2.0f}, (float)(screenWidth / 20), false));
    std::string draw = std::to_string(player_number) + " players";
    drawButton(draw, {(float)(screenWidth) / 2.0f, (float)(screenHeight) / 2.0f}, (float)(screenWidth / 20), false);

    // ends drawing call
    EndDrawing();

    return buttons;
}

int GameWindow::gameDrawLoop(int my_player, int player_turn, int players, std::vector<std::array<std::array<bool, 4>, 4>> highlights, std::vector<std::array<std::array<int, 4>, 4>> positions, Map* gameMap) {
    // starts drawing call
    BeginDrawing();
    generalDraw();
    
    // begins part affected by camera
    BeginMode2D(camera);

        // draws game map
        gameMap->drawMap();

        // draws position of each player in the board
        gameMap->drawPlayers(player_turn);

        // draws highlighted areas of the board
        gameMap->drawHighlights(highlights);

        // draw pieces of the board
        gameMap->drawPieces(positions, 1.0 / players);

    // end part affected by camera
    EndMode2D();

    // draws return button
    Rectangle ret = drawButton("<", {(float)(screenWidth) / 10.0f, (float)(screenHeight) / 10.0f}, (float)(screenWidth / 20), false);

    // draws which player you are
    drawButton("You are player " + std::to_string(my_player + 1), {(float)(screenWidth) / 1.2f, (float)(screenHeight) / 1.05f}, (float)(screenWidth / 40), false);

    // ends drawing call
    EndDrawing();

    // checks if return to menu button is pressed
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), ret))
        return 1;
    return 0;
}

/**
 * @brief draws background
 */
void GameWindow::generalDraw() {
    ClearBackground(RAYWHITE);
}

/**
 * @brief updates camera position according to inputs
 */
void GameWindow::cameraUpdate() {
    GameWindow::camera.zoom += ((float)GetMouseWheelMove() * 0.2f);

    if (IsKeyDown(KEY_RIGHT)) GameWindow::camera.target.x += 3;
    else if (IsKeyDown(KEY_LEFT)) GameWindow::camera.target.x -= 3;
    else if (IsKeyDown(KEY_UP)) GameWindow::camera.target.y -= 3;
    else if (IsKeyDown(KEY_DOWN)) GameWindow::camera.target.y += 3;

    if (GameWindow::camera.zoom > 10.0f) GameWindow::camera.zoom = 10.0f;
    else if (GameWindow::camera.zoom < 0.1f) GameWindow::camera.zoom = 0.1f;
}

/**
 * @brief determines if there is a click in a certain region of the map
 * @param i position in i dimension
 * @param j position in j dimension
 * @param k position in k dimension
 * @param gameMap map to be used
 * @return true if there is input, false otherwise
 */
bool GameWindow::checkMapInput(int i, int j, int k, Map *gameMap) {
    // checks if mouse button is pressed
    if(!IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        return false;

    // if it is pressed, check the position
    return gameMap->checkMapCollision(i, j, k, GetScreenToWorld2D(GetMousePosition(), camera));
}

/**
 * @brief checks menu clicks and return interpretation
 * @param buttons buttons in the menu
 * @param players number of players selected
 * @return 
 */
int GameWindow::checkMenuClicks(std::vector<Rectangle> buttons, int players) {
    // checks clicks
    int event = -1;
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for(int i = 0; i < buttons.size(); i++) {
            if(CheckCollisionPointRec(GetMousePosition(), buttons[i])) {
                if(players < 3 && i == 2)
                    i = 3;
                event = i;
            }
        }
    }

    return event;
}