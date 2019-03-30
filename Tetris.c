
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>
#include <unistd.h>

int windowWidth = 80;
int windowHeight = 24;

int gamex = 1;
int gamey = 1;

char screenBuffer[1921];
char blocks[201];
int lastkey = 0;

char tetrominos[7][17];

int getScreenIndex(int x, int y);
int rotate(int x, int y, int a);
int spaceAvaliable(int tetronimo, int px, int py, int a);
int getKey();
void arrayMove(char array[], int oldindex, int newindex);

int main() {
    int gameWidth = 10;
    int gameHeight = 20;

    int currentTetromino = 0;
    int currentx = 4;
    int currenty = 1;
    int currentRotation = 0;

    int frameCount;

    for(int i = 0; i < windowWidth*windowHeight; i++) {
        strcat(screenBuffer, " ");
    }

    for(int i = 0; i < 200; i++) {
        strcat(blocks, " ");
    }

    // Set random seed
     srand(time(NULL)); 
     currentTetromino = rand() % 7;


    // Fill an array with all seven tetrominos
    strcpy(tetrominos[0], "  O   O   O   O ");
    strcpy(tetrominos[1], "     O   OOO    ");
    strcpy(tetrominos[2], "       O OOO    ");
    strcpy(tetrominos[3], "     OO  OO     ");
    strcpy(tetrominos[4], "     OO OO      ");
    strcpy(tetrominos[5], " O   OO  O      ");
    strcpy(tetrominos[6], "    OO   OO     ");


    printf("\033[8;24;80t"); // Resize screen
    printf("\e[1;1H\e[2J");  // clear screen
    clear();

    WINDOW *w = initscr();
    cbreak();
    nodelay(w, TRUE);
    noecho();
    clear();
    refresh();

    int gameOver = 0;
    while (!gameOver) {
        refresh();
        usleep(20000);
        frameCount++;
        printf("\e[1;1H\e[2J");


        // -- Game Logic --
        if(frameCount % 25 == 0 || lastkey == 2) {
            if(spaceAvaliable(currentTetromino, currentx, currenty+1, currentRotation)) {
                lastkey = 0;
                currenty++;
                frameCount = 0;
            } else {
                // Turn piece into X's, and make another random piece at the top, also, check if a line was made
                for(int x = 0; x < 4; x++) {
                 for(int y = 0; y < 4;  y++) {
                        char c = tetrominos[currentTetromino][rotate(x, y, currentRotation)];
                
                        if(c == 'O') {
                            blocks[(currenty+y-1)*10+(currentx+x-1)] = 'X';
                        }
                    }
                }

                currentTetromino = rand() % 7;
                currenty = 0;
                currentx = 4;
            }
        }

        if(getKey() != 0) {
            if(lastkey == 4) {
                if(spaceAvaliable(currentTetromino, currentx-1, currenty, currentRotation)) {
                    lastkey = 0;
                    currentx--;
                }
            } 

            if(lastkey == 3) {
                if(spaceAvaliable(currentTetromino, currentx+1, currenty, currentRotation)) {
                    lastkey = 0;
                    currentx++;
                }
            }

            if(lastkey == 1) {
                if(spaceAvaliable(currentTetromino, currentx, currenty, currentRotation+1)) {
                    lastkey = 0;
                    currentRotation++;
                }
            }

        }

        for(int y = 0; y < 20; y++) {
            char row[11];
            for(int x = 0; x < 10; x++) {
                row[x] = blocks[y*10+x];
            }

            if(strcmp(row, "XXXXXXXXXX") == 0) {
                for(int x = 0; x < 10; x++) {
                   blocks[y*10+x] = ' ';
                }

                for(int n = 0; n < 10; n++) {
                    char temp = blocks[((y+1)*10)-1];
                    for(int i = ((y+1)*10)-1;i>0;i--) {
                        blocks[i] = blocks[i-1];
                    }
                    blocks[0] = temp;
                }
            }
        }

        // -- Render --

        // Game Walls
        for(int x = gamex; x < 13; x++) {
            for(int y = gamey; y < 23; y++) {
                if(x == 1 || y == 1 || x == 12 || y == 22) {
                    screenBuffer[getScreenIndex(x,y)] = '*';
                } else {
                    screenBuffer[getScreenIndex(x,y)] = ' ';
                }
            }
        }

        // Draw the blocks
        for(int x = 0; x < 10; x++) {
            for(int y = 0; y < 20; y++) {
                char c = blocks[y*10+x];
                
                if(c == '.')
                    c = screenBuffer[getScreenIndex(2+x, 2+y)];

                screenBuffer[getScreenIndex(2+x, 2+y)] = c;
            }
        }

        // Current tetromino
        for(int x = 0; x < 4; x++) {
            for(int y = 0; y < 4;  y++) {
                char c = tetrominos[currentTetromino][rotate(x, y, currentRotation)];   
                if(screenBuffer[getScreenIndex(currentx+x+gamex, currenty+y+gamey)] == ' ') {
                    screenBuffer[getScreenIndex(currentx+x+gamex, currenty+y+gamey)] = c;
                }
            }
        }

        printf("%s\n", screenBuffer);
    }

    return 0;
}

int getScreenIndex(int x, int y) {
     return y * windowWidth + x;
}

int rotate(int x, int y, int a) {
    int pi = 0;
    switch(a%4) {
        case 0: 
            pi = y * 4 + x;
            break;

        case 1:
            pi = 12 + y - (x * 4);
            break;
        
        case 2:
            pi = 15 - (y * 4) - x;
            break;

        case 3:
            pi = 3 - y + (x * 4);
            break;
        	
    }

    return pi;
}

int spaceAvaliable(int tetromino, int px, int py, int a) {
    for (int x = 0; x < 4; x++) {
        for(int y = 0; y < 4; y++) {
            char c = tetrominos[tetromino][rotate(x, y, a)];

            int solidPiece = 0;

            if (screenBuffer[getScreenIndex(px+x+gamex, py+y+gamey)] == 'X') {
                solidPiece = 1;
            } else if (screenBuffer[getScreenIndex(px+x+gamex, py+y+gamey)] == '*') {
                solidPiece = 1;
            }

            if(solidPiece && c == 'O') {
                return 0;
            }

        }
    }

    return 1;
}

int getKey() {
    if(getch() == '\033') {
        getch();
        switch(getch()) { 
        case 'A':
            lastkey = 1;
            return 1;
            break;
        case 'B':
            lastkey = 2;
            return 2;
            break;
        case 'C':
            lastkey = 3;        
            return 3;
            break;
        case 'D':
            lastkey = 4;
            return 4;
            break;
    }
    }

    return 0;
}
