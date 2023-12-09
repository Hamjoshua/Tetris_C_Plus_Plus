using namespace std;

#include <random>
#include <conio.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>

static char BLOCK_MOVING = '#';
static char BLOCK_STILL = '%';
static char MAP_VOID = '-';
static char MAP_WALL = '|';
static char MAP_FLOOR = '=';

enum OPERATIONS {TO_RIGHT, TO_LEFT, FLIP};
enum BOUNDS { RIGHT_BOUND, LEFT_BOUND, TOP_BOUND, BOTTOM_BOUND };

vector<vector<char>> structure { {BLOCK_MOVING, BLOCK_MOVING, BLOCK_MOVING, BLOCK_MOVING} };

class Block {
public:
    int originY;
    int originX;

    vector<vector<char>> structure;

    int getWidth() {
        return structure[0].size();
    }

    int getHeight() {
        return structure.size();
    }

    int getBound(int bound) {
        switch (bound) {
            RIGHT_BOUND:
                return originX + getWidth() - 1;
                break;
            LEFT_BOUND:
                return originX;
            TOP_BOUND:
                return originY;
                break;
            BOTTOM_BOUND:
                return originY + getHeight() - 1;
                break;
        }
    }

    void fall() {
        originY += 1;
    }

    void moveToRight(bool toRight) {
        if (toRight) {
            originX += 1;
        } else {
            originX -= 1;
        }
    }

    void flip() {

    }

};


class Map {
    int width;
    int heigth;

public:
    vector<vector<char>> field;

    Map() {
    }

    void init(int width, int heigth){
        field.resize(heigth);

        for (int row = 0; row < heigth; ++row) {
            vector<char> columns;
            columns.resize(width);
            field[row] = columns;
            for (int column = 0; column < width; ++column) {
                field[row][column] = MAP_VOID;
            }
        }
    }

    void draw(Block block) {
        int startX = block.getBound(LEFT_BOUND);
        int endX = block.getBound(RIGHT_BOUND);
        int startY = block.getBound(TOP_BOUND);
        int endY = block.getBound(BOTTOM_BOUND);

        for (int row = 0; row < heigth; ++row) {
            cout << MAP_WALL;
            for (int column = 0; column < width; ++column) {
                char letter;

                if ((row >= startY && row <= endY) && (column >= startX && column <= endX)) {
                    char blockLetter = block.structure[row - block.originY][column - block.originY];
                    if (blockLetter == BLOCK_MOVING) {
                        letter = BLOCK_MOVING;
                        continue;
                    }
                }                
                letter = field[row][column];
                
                cout << letter << " ";
            }
            cout << MAP_WALL << "\n";
        }
        drawFloor();
    }

    void drawFloor() {
        cout << MAP_WALL;
        for (int column = 0; column < width; ++column) {
            cout << MAP_FLOOR << " ";
        }
        cout << MAP_WALL << "\n";
    }

    bool isCollideWithFloor(Block block) {
        int bottom = block.getBound(BOTTOM_BOUND);

        if (bottom == heigth - 1) {
            return true;
        }

        int endX = block.getWidth();
        int endY = block.getHeight();

        for (int row = 0; row < endY; ++row) {
            for (int column = 0; column < endX; ++column) {
                char letterBlock = block.structure[row][column];
                if (letterBlock == BLOCK_MOVING) {
                    int newFieldRow = row + block.originY + 1;
                    int fieldColumn = column + block.originX;

                    char cell = field[newFieldRow][fieldColumn];

                    if (cell == BLOCK_STILL) {
                        return true;
                    }
                }
            }
        }
    }

    void placeBlock(Block block) {
        int endX = block.getWidth();
        int endY = block.getHeight();

        for (int row = 0; row < endY; ++row) {
            for (int column = 0; column < endX; ++column) {
                char letterBlock = block.structure[row][column];
                if (letterBlock == BLOCK_MOVING) {
                    int fieldRow = row + block.originY;
                    int fieldColumn = column + block.originX;
                    
                    field[fieldRow][fieldColumn] = BLOCK_STILL;
                }
            }
        }
    }

    bool isBoundsToMove(Block block, int operation) {
        char letterBlock;
        char cell;
    }

    void checkAllLayers(Block block, int* gmScore) {
        int startY = block.getBound(TOP_BOUND);
        int endY = block.getBound(BOTTOM_BOUND);

        int row = startY;

        while (row < endY) {
            if (isLayerFull(row)) {
                *gmScore += width;
                removeLayer(row);
            }

            row += 1;
        }
    }

    void removeLayer(int row) {
        while (row > 0) {
            for (int column = 0; column < width; ++column) {
                field[row][column] = field[row - 1][column];
            }

            row -= 1;
        }
    }

    bool isLayerFull(int row) {
        for (int column = 0; column < width; ++column) {
            char cell = field[row][column];

            if (cell == MAP_VOID) {
                return false;
            }
        }
    }

    bool anyStillBlockOnTop() {
        for (int column = 0; column < width; ++column) {
            char cell = field[0][column];

            if (cell == BLOCK_STILL) {
                return true;
            }
        }
    }

};


class GameManager {
    int score = 0;
public:
    Map gameMap;
    Block currentBlock;

    void chooseBlock() {

    }

    GameManager(int width, int height) {
        gameMap.init(width, height);
        chooseBlock();
    }

    void drawMap() {
        gameMap.draw(currentBlock);
    }

    bool isLose() {
        return gameMap.anyStillBlockOnTop();
    }

    int getScore() {
        return score;
    }

    void gameCycle(int operation) {
        if (gameMap.isCollideWithFloor(currentBlock)) {
            gameMap.placeBlock(currentBlock);
            gameMap.checkAllLayers(currentBlock, &score);
            chooseBlock();
        }
        else {
            currentBlock.fall();

            switch (operation)
            {
            case TO_RIGHT:
                if (!gameMap.isBoundsToMove(currentBlock, operation)) {
                    currentBlock.moveToRight(true);
                }
                break;
            case TO_LEFT:
                if (!gameMap.isBoundsToMove(currentBlock, operation)) {
                    currentBlock.moveToRight(false);
                }
                break;
            case FLIP:
                if (!gameMap.isBoundsToMove(currentBlock, operation)) {
                    currentBlock.flip();
                }
                break;

            }
        }
    }
};


int handleInput() {
    char letter = ';';
    
    if (_kbhit()) {
        letter = _getch();
        letter = tolower(letter);
    }

    if (letter == 'd' || letter == 'в') {
        return TO_RIGHT;
    } else if (letter == 'a' || letter == 'ф') {
        return TO_LEFT;
    } else if (letter == 'f' || letter == 'а') {
        return FLIP;
    }
}

void drawUi(GameManager gm) {
    cout << "Очки:" << gm.getScore() << '\n';
    
    if (gm.isLose()) {
        cout << "ВЫ ПРОИГРАЛИ:" << '\n';
    }
}

void tick(float sec) {
    Sleep(sec * 1000);
}

int main()
{   
    int width = 10;
    int heigth = 15;
    GameManager gm = GameManager(width, heigth);

    while (true) {
        int operation = handleInput();

        if (!gm.isLose()) {
            gm.gameCycle(operation);
        }
        system("clear");
        gm.drawMap();
        drawUi(gm);
        tick(0.5);
    }

    std::cout << "Hello World!\n";
}

