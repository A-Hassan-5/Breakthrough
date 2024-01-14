#include <math.h>
#include <raylib.h>  //including the raylib library
#include <stdio.h>

// Blocks
typedef struct block {  // data type blocks ( represents the blocks in the game)
    Rectangle possize;  // position and size of blocks
    bool state;         // state of the rectangle
    Color color;        // color of block
} block;

// Balls movement with its speed movements
typedef struct gameball {
    Rectangle ball_possize;  // poition and size of ball
    float x_ball;            // speed of ball in x axis direction
    float y_ball;            // speed of ball in y axis
} gameball;

// Collision between the blocks , ball and the pad operated by the player
typedef enum collision {
    no_collision = 0,  // if no collision(default condition)
    top_collision,
    bottom_collision,
    left_collision,
    right_collision,
} typeCollision;  // data type

enum Gamestate  // Gamestate data enumerator
{
    Startscreen,
    Playing,
    Gameover
};

Rectangle rec =
    {
        // rectangle
        .width = 139,
        .height = 30,
        .y = 10  // staring point in y axis
};

Rectangle pad =
{  // pad operated by the player
        .width = 180,
        .height = 30,
        .y = 850,
        .x = 180
};

Rectangle ballmeasure =  // ball
{
        .width = 16,
        .height = 16,
        .y = 500,
        .x = 500
};


// Game Window resolution
const int height = 900;
const int width = 1500;

// Variables for user movement and scores
const int pad_speed = 25;  // speed by which the pad on the bottom moves
int score = 0;             // speed of user attained by collision of ball with the blocks
int x_movement = 9;        // Speed of ball in x axis
int y_movement = -9;       // Speed of ball at y axis
int start = 1;             // Varible to manage the starting of the game
unsigned char action = 0;  // Tracks the no. of canceled actions in the game

block blocks[5][10];  // defining a array of blocks of struct 'block'  ( used to store the collision blocks )

enum Gamestate gamestate = Startscreen;  // set the gamestate variable at the start screen

// USER DEFINED FUNCTIONS
void make_blocks(int* n);
void physics(Rectangle pad, gameball* ball, block blocks[5][10], int* lifes);
void reset_ball(gameball* ball);
typeCollision colide(Rectangle rect1, Rectangle rect2);
void reset_life(int* lifes);
void reset_score(int* score);
int score_maker(int y);
void startpage();

// MAIN FUNCTION
int main() {
    int lifes = 7;  // player lives
    int level = 1;  // level of the game

    float transparency = 1.0f;             // transparency level  (max at 1.0)
    int colorcode = GetRandomValue(0, 5);  // used when restarting the blocks to get a random color code each time from 0 to 5



    gameball ball =
        {
            // ball having
            .ball_possize = ballmeasure,  // takes the psotion and size of ball from the ball measures in the structure
            .x_ball = x_movement,         // takes x movement of ball from the global x movement variable
            .y_ball = y_movement          // takes y movement from the y movement global variable
        };



    Color color_life = GREEN;  // Set color of lifes displayed on the screen
    bool pause = false;        // unpause the game from the start

    InitWindow(width, height, "Breakthrough");  // initalize the main game window
    InitAudioDevice();                          // set up the audio system for the application
    SetTargetFPS(60);                           // set up the targets for the frames per second in the game

    Music music = LoadMusicStream("./resources/music.mp3");  // loads the music from the given file
    music.looping = true;                                    // loops the music ( turns it on after it ends )
    PlayMusicStream(music);

    make_blocks(&colorcode);  // calls the function to restart/make the blocks ( it is the first attempt to make the blocks )

    Rectangle playButton = {GetScreenWidth() / 2 - 100, GetScreenHeight() / 2, 200, 50};
    Rectangle exitButton = {GetScreenWidth() / 2 - 100, GetScreenHeight() / 2 + 80, 200, 50};

    // GAME LOOP
    while (!WindowShouldClose())  // checks if user has attempted to close the window or no? ( via esc etc )
    {
        switch (gamestate)  // CASE 1
        {
            case Startscreen:
                startpage();
                if (CheckCollisionPointRec(GetMousePosition(), playButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    gamestate = Playing;  // takes gamestate into playing ( continuation)

                // Check if the mouse is over the "EXIT" button and clicked
                if (CheckCollisionPointRec(GetMousePosition(), exitButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    CloseWindow();  // Exit the game
                break;

            case Playing:  // CASE 2( main game logic )

                UpdateMusicStream(music);  // it updates the music stream to the saem music set before

                if (start > 0) {
                    if (IsKeyPressed(KEY_P)) {  // if we press 'p'
                        if (pause) {
                            ResumeMusicStream(music);  // if the game is paused it resumes the music and sets the pause value to false
                            pause = false;
                        } else {  // if we press p and the gae is running , it pauses the music and gives pause value to true
                            PauseMusicStream(music);
                            pause = true;
                        }
                    }
                }

                // RUNNING GAME
                if (!pause) {                    // if the game is running
                    if (IsKeyDown(KEY_A))        // if key 'a' is pressed
                        if (pad.x > 0)           // if the x coordinate of the player is more than 0 ( inside the screen)
                            pad.x -= pad_speed;  // decreases the x value of player by the pad speed at each point

                    if (IsKeyDown(KEY_D))               // if 'd' is pressed
                        if (pad.x + pad.width < width)  // if the x coordinate of the player and the width of the pad is not going out of the window
                            pad.x += pad_speed;

                    if (lifes > 0) {  // if lives are remaining and game is started and no. of hitted blocks is not 50
                        if (start == 1) {
                            if (action != 50)
                                physics(pad, &ball, blocks, &lifes);  // implement game physics
                            else {                                    // if we have hitted 50 blocks
                                colorcode = GetRandomValue(0, 5);     // again get a color code and make the blocks again and restart the ball from its true location
                                make_blocks(&colorcode);
                                reset_ball(&ball);
                                lifes += 2;  // also each stage gives you 2 new lives
                                ++level;
                            }
                        }
                    }
                }
                // LIFE COLOR
                if (lifes >= 5)
                    color_life = GREEN;
                else if (lifes >= 3)
                    color_life = YELLOW;
                else if (lifes < 3)
                    color_life = RED;

                if (lifes == 0) gamestate = Gameover;  // If lives get to zero ( we die )

                break;

            case Gameover:  // CASE # ( When user dies )

                if (IsKeyPressed(KEY_R))  //&& we press r ( retry)
                {
                    make_blocks(&colorcode);
                    reset_life(&lifes);
                    reset_ball(&ball);
                    reset_score(&score);  // Then reset the score , the ball and the no. of lives andf again make the blocks with the old colorcode
                    gamestate = Playing;  // again set the gamestate to Playing
                }

        }  // Switch close

        // STARTING TO DRAW THE OBJECTS AND TEXT
        BeginDrawing();          // start rendering ( mark start of drawing )
        ClearBackground(BLACK);  // clears the background with a black image and sets that black image

        if (start == 1)  // if game has started
        {
            if (lifes > 0)  // and the game is not ended ( not died yet )
            {
                for (int y = 0; y < 5; ++y)  // Outer -> Rows  , Inner -> Collumns
                {
                    for (int x = 0; x < 10; x++) {
                        if (blocks[y][x].state == true)  // checks if the blocks are at true state ( existent )
                            DrawRectangle(blocks[y][x].possize.x, blocks[y][x].possize.y, blocks[y][x].possize.width, blocks[y][x].possize.height, blocks[y][x].color);
                    }  // DrawRectangle takes the psoition and size of its x , y , width , height and its coor and draws all blocks
                }

                DrawText(TextFormat("Score : %d", score), 1200, 800, 40, RAYWHITE);  // Displays score
                DrawText(TextFormat("Lives : %d", lifes), 50, 800, 40, color_life);  // Displays lives left
                DrawText(TextFormat("Level : %d", level), 50, 850, 30, RAYWHITE);    // DIsplays the level

                DrawRectangle(ball.ball_possize.x, ball.ball_possize.y, ball.ball_possize.width, ball.ball_possize.height, BLUE);  // draw ball
                DrawRectangle(pad.x, pad.y, pad.width, pad.height, RED);                                                           // draw the player pad

                if (pause) {
                    DrawText("PAUSED", 700, 450, 40, WHITE);  // display text when paused
                }
            } else  // if the gamer is over
            {
                DrawText("Game Over", 580, 450, 80, RED);                          // display message
                DrawText(TextFormat("Score: %d", score), 680, 600, 40, RAYWHITE);  // gives end score
                DrawText("Press r to restart !", 640, 700, 30, RAYWHITE);          // restart message
                DrawText("Press ESC to leave !", 640, 750, 30, RAYWHITE);          // Leaving message
            }
        }

        EndDrawing();  // mark end of drawing
    }

    UnloadMusicStream(music);  // unloads and ends the music
    CloseAudioDevice();        // closes the device for audio and its stream
    CloseWindow();
}

void make_blocks(int* colorcode)  // make the game blocks
{
    action = 0;  // resets the actions variable to 0 ( it is a counter basically)
    rec.y = 10;  // starting y axis position of rectangle

    Color ranges[6][10] =
        {// color levels of blocks
         {MAGENTA, BLUE, YELLOW, GREEN, GREEN, BLUE, ORANGE, RAYWHITE, BLUE, GREEN},
         {ORANGE, BLUE, PINK, BLUE, BLUE, GREEN, BLUE, ORANGE, GREEN, YELLOW},
         {BLUE, PINK, BLUE, PINK, GREEN, PINK, PINK, PINK, MAGENTA, GREEN},
         {GREEN, YELLOW, GREEN, YELLOW, BLUE, GREEN, GREEN, BLUE, ORANGE, PINK},
         {GREEN, BLUE, MAGENTA, YELLOW, ORANGE, BLUE, ORANGE, GREEN, BLUE, GREEN},
         {BLUE, PINK, ORANGE, BLUE, MAGENTA, GREEN, BLUE, ORANGE, GREEN, YELLOW}

        };

    for (int y = 0; y < 5; ++y)  // Outer -> Rows , Inner -> collumns
    {
        for (int x = 0; x < 10; x++)  // here we iterate from 5 rows and 10 cullumns of blocks in the game
        {
            switch (y) {
                case 0:
                    blocks[y][x].color = ranges[*colorcode][0];
                    break;
                case 1:
                    blocks[y][x].color = ranges[*colorcode][1];
                    break;
                case 2:
                    blocks[y][x].color = ranges[*colorcode][2];
                    break;
                case 3:
                    blocks[y][x].color = ranges[*colorcode][3];
                    break;
                case 4:
                    blocks[y][x].color = ranges[*colorcode][4];  // the colorcode iterate between 0 to 4
                    break;
                default:
                    break;
            }

            if (x == 0) {  // for the first block in each row , set the rectangle inital y position to 10
                rec.x = 10;
            } else {
                rec.x += rec.width + 10;  // and for the next ones , increment it with 10
            }

            blocks[y][x].possize = rec;  // make each blocks size equal to the standard rectangle size and psoition constants
            blocks[y][x].state = true;   // take all the blocks ( total 50 ) to be true b y each iteration
        }
        rec.y += rec.height + 10;  // after 1 row is completed , add in 10 to the rectangle height to make the range lower
    }
}

void reset_ball(gameball* ball)  // restarts the ball(due to game end , going out of bounds , new blocks)
{
    ball->ball_possize.x = GetRandomValue(50, 1480);  // ball position is randomized between 50 and 1780 on x axis
    ball->ball_possize.y = 750;                       // ball position wrt y axis is set to 750
    ball->x_ball = x_movement;                        // ball x axis speed set to 10 again
    ball->y_ball = y_movement;                        // ball y axis speed set to -10 again
}


void physics(Rectangle player, gameball* ball, block blocks[5][10], int* lifes)  // Implements physics in the game
{
    // CHECKING BOUNDARIES
    if (ball->ball_possize.y <= 0)     // y axis
        ball->y_ball = -ball->y_ball;  // shifts ball speed to opposite if ball crosses or touches the top

    if (ball->ball_possize.x + ball->ball_possize.width >= width || ball->ball_possize.x <= 0)  // x axis
        ball->x_ball = -ball->x_ball;                                                           // shifts ball speed to opposite if ball crosses or touches the sides

    // POSITION CHANGING / INCREMENTING
    ball->ball_possize.x += ball->x_ball;  // x axis
    ball->ball_possize.y += ball->y_ball;  // y axis

    // if ball goes below the pad , deduct 1 life and restart the ball
    if (ball->ball_possize.y > height) {
        *lifes -= 1;
        reset_ball(ball);
    }

    // COLLISION OF BALL WITH THE PAD
    typeCollision playerCollision = colide(ball->ball_possize, pad);                   // finding collision
    if (playerCollision == right_collision || playerCollision == left_collision)       // if collision on right or left
        ball->x_ball = -ball->x_ball;                                                  // oppose the x speed movement of the ball
    else if (playerCollision == top_collision || playerCollision == bottom_collision)  // if collision on top or bottom
        ball->y_ball = -ball->y_ball;                                                  // oppose the y speed movement of the ball

    // COLLISIONS WITH BLOCKS AND THE BALL
    for (int y = 0; y < 5; ++y)  // Outer -> Rows , Inner -> collumns
    {
        for (int x = 0; x < 10; ++x) {
            if (blocks[y][x].state == true)  // checks state of blocks ( all 50 will be true with complete make)
            {
                typeCollision blockCollision = colide(ball->ball_possize, blocks[y][x].possize);

                // If colliding from the top or bottom
                if (blockCollision == top_collision || blockCollision == bottom_collision) {
                    action++;                      // add in an action
                    blocks[y][x].state = false;    // give state of that block to zero and eliminate it
                    ball->y_ball = -ball->y_ball;  // oppose the y speed direction of the ball vertically
                    score += score_maker(y);       // adds score depending on the row being hit
                }

                // if colliding from the right or left
                else if (blockCollision == left_collision || blockCollision == right_collision) {
                    blocks[y][x].state = false;    // again state the block state as false
                    action++;                      // add in a action
                    ball->x_ball = -ball->x_ball;  // reverse the  speed direction of the ball horizontally
                    score += score_maker(y);       // adds score depending on the row being hit
                }
            }
        }
    }
}

typeCollision colide(Rectangle rect1, Rectangle rect2)  // checks if there is a collision(if any)
{
    // CHECKING FOR COLLISION ON THE RIGHT
    if (rect1.x + rect1.width >= rect2.x)
        if (rect1.x < rect2.x && rect1.y + rect1.height > rect2.y && rect1.y < rect2.y + rect2.height)
            return right_collision;

    // CHECKING FOR COLLISION ON THE LEFT
    if (rect1.x <= rect2.x + rect2.width)
        if (rect1.x + rect1.width > rect2.x + rect2.width && rect1.y + rect1.height > rect2.y && rect1.y < rect2.y + rect2.height)
            return left_collision;

    // CHECKING FOR COLLISION ON THE BOTTOM
    if (rect1.y + rect1.height >= rect2.y)
        if (rect1.x + rect1.width >= rect2.x && rect1.x <= rect2.x + rect2.width && rect1.y < rect2.y)
            return bottom_collision;

    // CHECKING FOR COLLISION ON THE TOP
    if (rect1.y <= rect2.y + rect2.height)
        if (rect1.x + rect1.width >= rect2.x && rect1.x <= rect2.x + rect2.width && rect1.y + rect1.height > rect2.y + rect2.height)
            return top_collision;

    return no_collision;  // if no collision is found
}

void reset_life(int* lifes)  // resets the lives
{
    *lifes = 7;
}

void reset_score(int* score)  // reset the scores
{
    *score = 0;
}

int score_maker(int y)  // calculate score
{
    switch (y) {
        case 0:
            return 50;
            break;
        case 1:
            return 40;
            break;
        case 2:
            return 30;
            break;
        case 3:
            return 20;
            break;
        default:
            return 10;
    }
}

void startpage() {
    float oscillation = 10.0f * sin(GetTime() * 5.0f);  // Adjust the frequency with the multiplier

    DrawText("Breakthrough", GetScreenWidth() / 2 - MeasureText("Breakthrough", 40) / 2 + oscillation, GetScreenHeight() / 3, 40, RAYWHITE);  // Draw game logo in the center

    // DRAW THE PLAY BUTTON
    Rectangle playButton = {GetScreenWidth() / 2 - 100, GetScreenHeight() / 2, 200, 50};
    DrawRectangleRec(playButton, RAYWHITE);
    DrawText("PLAY", playButton.x + 50 - MeasureText("PLAY", 20) / 2, playButton.y + 15 - 10, 20, BLACK);

    // DRAW THE EXIT BUTTON
    Rectangle exitButton = {GetScreenWidth() / 2 - 100, GetScreenHeight() / 2 + 80, 200, 50};
    DrawRectangleRec(exitButton, RAYWHITE);
    DrawText("EXIT", exitButton.x + 50 - MeasureText("EXIT", 20) / 2, exitButton.y + 15 - 10, 20, BLACK);
}