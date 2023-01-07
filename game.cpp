#include "raylib.h"
#include <stdlib.h>
// file
#include <fstream>

# define STORAGE_DATA_FILE "storage.data"

#define MAX_TILES 398 // the amount of squares that will be generated

static const int screenWidth = 800;
static const int screenHeight = 800;
static int frameCounter = 0; // deltaTime but using variables
static bool gameOver = false;
static int deltaTime = 0;

// scores
static int score = 0;
static int highScore = 0;

// screen manager
typedef enum GameScreen { TITLE = 0, GAMEPLAY} GameScreen;

class Tiles {
    public:
        Rectangle rec;
        bool isActive;
        bool active;

        void Draw() {
            if (isActive == true) {
                if (active == true) {
                    DrawRectangle(rec.x, rec.y, rec.width, rec.width, DARKPURPLE);
                } else if (active == false) {
                    DrawRectangle(rec.x, rec.y, rec.width, rec.height, PURPLE);
                }
            }
        }

        void UpdatePos() {
            rec.x = GetRandomValue(0, 19) * 40;
            rec.y = GetRandomValue(0, 19) * 40;
        }        
};

class Player {
    public:
        // making attributes
        Rectangle rec;
        int speed;

        void Draw() {
            DrawRectangle(rec.x, rec.y, rec.width, rec.height, BLACK);
        }

        void Movement() {
            if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) rec.y -= speed;

            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) rec.x -= speed;

            if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) rec.y += speed;

            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) rec.x += speed;

            Borders();
        }

        void Borders() {
            if (rec.x <= 0) {
                rec.x = 0;
            } else if (rec.x >= screenWidth - rec.width) {
                rec.x = screenWidth - rec.width;
            }

            if (rec.y <= 0) {
                rec.y = 0;
            } else if (rec.y >= screenHeight - rec.height) {
                rec.y = screenHeight - rec.height;
            }
        }
};

// "entities" involved
static Tiles set[MAX_TILES] = { 0 };
Player player;

bool CheckOverlap(int index, int x, int y)
{
    // index : index for the tile
    // x,y : x y coordinate
    
    // return : true if there is an overlap. 

    bool isOverlapped=false;

    for(int i = 0;i < index; i++) {
        if (set[i].rec.x == x && set[i].rec.y == y) isOverlapped = true;
    }

    return isOverlapped;
}



// resetting the variables
void resetGame() {
    gameOver = false;
    frameCounter = 0;
    deltaTime = 0;

    // reset position
    player.rec.x = screenWidth / 2; 
    player.rec.y = screenHeight / 2;

    // randomize empty spots(but no overlap)
    for (int i = 0; i < MAX_TILES; i++) {
        
        set[i].rec.x = GetRandomValue(0, 19) * 40;
        set[i].rec.y = GetRandomValue(0, 19) * 40;
        while(CheckOverlap(i,set[i].rec.x,set[i].rec.y)){
            set[i].rec.x = GetRandomValue(0, 19) * 40;
            set[i].rec.y = GetRandomValue(0, 19) * 40;
        }
        set[i].rec.width = 40; set[i].rec.height = 40;
        set[i].isActive = false;
        set[i].active = false;
    }
}

// initialize the game
void InitGame() {
    // player initializing the attributes
    player.rec.width = 40; 
    player.rec.height = 40;
    player.rec.x = screenWidth / 2; 
    player.rec.y = screenHeight / 2;
    player.speed = 40;

    // void tiles initialization
    for (int i = 0; i < MAX_TILES; i++) {
        
        set[i].rec.x = GetRandomValue(0, 19) * 40;
        set[i].rec.y = GetRandomValue(0, 19) * 40;
        while(CheckOverlap(i,set[i].rec.x,set[i].rec.y)){
            set[i].rec.x = GetRandomValue(0, 19) * 40;
            set[i].rec.y = GetRandomValue(0, 19) * 40;
        }
        set[i].rec.width = 40; set[i].rec.height = 40;
        set[i].isActive = false;
        set[i].active = false;
    }
    // highscore
    std::ifstream fi;
    fi.open(STORAGE_DATA_FILE);
    fi >> highScore;
    fi.close();
}

void activationSequence(int i, bool setIsActive, bool setActive) {
    set[i].isActive = setIsActive;
    set[i].active = setActive;
}

// update
void UpdateGame() {
    if (!gameOver) {
        frameCounter += 1; // timer
        deltaTime = (int)(frameCounter/60);

        // player movement
        player.Movement();

        // enemy placement
        if (deltaTime % 7 < 2) { // grace period
            for (int i = 0; i < MAX_TILES; i++) {
                activationSequence(i, true, false); // areas determined to be activated, not killing though
            }
        } 
        else if (deltaTime % 7 >= 2 && deltaTime % 7 < 5) {
            for (int i = 0; i < MAX_TILES; i++) { 
                activationSequence(i, true, true); // TOXICITY HAS BEEN INCREASED, kill the player
                if (CheckCollisionRecs(player.rec, set[i].rec)) {
                    gameOver = true;
                }
            }
        }
        else if (deltaTime % 7 == 5) { // turn end
            for (int i = 0; i < MAX_TILES; i++) {
                activationSequence(i, false, false); // set up for the next turn
                set[i].UpdatePos();
                while(CheckOverlap(i,set[i].rec.x,set[i].rec.y)){
                    set[i].UpdatePos();
                }
            }
        }
    }
}

// draw
void DrawGame() {
    if (!gameOver) {
        
        for (int i = 0; i < MAX_TILES; i++) {
            set[i].Draw();
        }

        player.Draw();

        DrawText(TextFormat("%i",deltaTime), 10, 10, 60, DARKGRAY);        
    }
}

// game mainframe(at this point, fake names)
int main() {
    InitWindow(screenWidth, screenHeight, "Two Seconds");

    SetTargetFPS(60);

    // initializing
    InitGame();
    GameScreen currentScreen = TITLE;

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // update
        switch (currentScreen) {
            case TITLE:
            {
                resetGame();
                if (IsKeyPressed(KEY_ENTER)) {
                    currentScreen = GAMEPLAY;
                }
            }

            case GAMEPLAY:
            {
                UpdateGame();
                if (gameOver == true) {
                    currentScreen = TITLE;
                    score = deltaTime;
                    // highscore system
                    if (score > highScore) {
                        highScore = score;
                        std::ofstream fo; // output stream
                        fo.open(STORAGE_DATA_FILE);
                        fo << highScore;
                        fo.close();

                    }
                }
            }
            default: break;
        }

        // draw
        switch (currentScreen) {
            case TITLE:
            {
                DrawText("Two Seconds", screenWidth / 2 - 270, 50, 80, BLACK);
                DrawText("Highscore", screenWidth / 2 - 125, 120, 50, GRAY);
                DrawText(TextFormat("%i",highScore), screenWidth / 2 - 5, 170, 50, GRAY);
                DrawText("[Press Enter to Play]", screenWidth / 2 - 270, screenHeight - 100, 50, LIGHTGRAY);
                
            } break;

            case GAMEPLAY:
            {
                DrawGame();
            } break;
            default: break;
        } 

        EndDrawing();
    }

    CloseWindow();
    return 0;
}