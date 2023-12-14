using namespace std;

#include <random>
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <Windows.h>

#define setLocale setlocale
#define consoleSleep Sleep
#define toLower tolower
#define toUpper toupper


static char BLOCK_MOVING = '#';
static char BLOCK_FIXED = '%';
static char MAP_EMPTY_SPACE = '-';
static char MAP_WALL = '|';
static char MAP_FLOOR = '=';

static char GAME_KEY_RIGHT = 'd';
static char GAME_KEY_RIGHT_ALT = 'в';
static char GAME_KEY_LEFT = 'a';
static char GAME_KEY_LEFT_ALT = 'ф';
static char GAME_KEY_FLIP = 'f';
static char GAME_KEY_FLIP_ALT = 'f';
static char GAME_KEY_FALL = 's';
static char GAME_KEY_FALL_ALT = 'ы';

float NORMAL_VELOCITY = 0.5;
float FAST_VELOCITY = 0.1;

enum COLORS { BLUE = 1, GREEN = 2, CYAN = 3, RED = 4, PURPLE = 5, YELLOW = 6, WHITE = 15 };
enum OPERATIONS {TO_RIGHT, TO_LEFT, FLIP};
enum BOUNDS { RIGHT_BOUND, LEFT_BOUND, TOP_BOUND, BOTTOM_BOUND };

vector<vector<char>> LineBlockStructure { {BLOCK_MOVING, BLOCK_MOVING, BLOCK_MOVING, BLOCK_MOVING} };
vector<vector<char>> LBlockStructure{ {BLOCK_MOVING, BLOCK_MOVING, BLOCK_MOVING},
                              {BLOCK_MOVING, MAP_EMPTY_SPACE, MAP_EMPTY_SPACE} };
vector<vector<char>> JBlockStructure{ {BLOCK_MOVING, MAP_EMPTY_SPACE, MAP_EMPTY_SPACE},
                              {BLOCK_MOVING, BLOCK_MOVING, BLOCK_MOVING} };
vector<vector<char>> TBlockStructure{ {MAP_EMPTY_SPACE, BLOCK_MOVING, MAP_EMPTY_SPACE},
                              {BLOCK_MOVING, BLOCK_MOVING, BLOCK_MOVING} };
vector<vector<char>> ZBlockStructure{ {BLOCK_MOVING, BLOCK_MOVING, MAP_EMPTY_SPACE},
                              {MAP_EMPTY_SPACE, BLOCK_MOVING, BLOCK_MOVING} };
vector<vector<char>> SBlockStructure{ {MAP_EMPTY_SPACE, BLOCK_MOVING, BLOCK_MOVING},
                              {BLOCK_MOVING, BLOCK_MOVING, MAP_EMPTY_SPACE} };
vector<vector<char>> OBlockStructure{ {BLOCK_MOVING, BLOCK_MOVING},
                              {BLOCK_MOVING, BLOCK_MOVING} };

struct coloredChar {
    char letter;
    int color;
};

void coutTextWithColor(string text, int color, bool withSpacebar=false) {
    HANDLE console_color;
    console_color = GetStdHandle(STD_OUTPUT_HANDLE);
    
    SetConsoleTextAttribute(console_color, color);

    if (withSpacebar) {
        cout << text << " ";
    } else {
        cout << text;
    }

    SetConsoleTextAttribute(console_color, WHITE);
}

class Block {
public:
    int originY;
    int originX;
    int color;

    vector<vector<char>> structure;

    int getWidth() {
        return structure[0].size();
    }

    int getHeigth() {
        return structure.size();
    }

    int getBound(int bound) {
        if (bound == RIGHT_BOUND) {
            int value = originX + getWidth() - 1;
            return value;
        } else if (bound == LEFT_BOUND) {
            return originX;
        } else if (bound == TOP_BOUND) {
            return originY;
        } else if (bound == BOTTOM_BOUND) {
            int value = originY + getHeigth() - 1;
            return value;
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

    void shiftForFlip() {
        int width = getWidth();
        int heigth = getHeigth();
        int shift;

        if (width == heigth) {
            return;
        } else if (width < heigth) {
            int shift = heigth / 2;
            originX += shift;
        } else {
            int shift = width / 2;
            originX -= shift;
        }
    }

    void flip() {
        vector<vector<char>> newStructure;
        newStructure.resize(getWidth());

        for (int rowIndex = 0; rowIndex < getWidth(); ++rowIndex) {
            vector<char> newColumns;
            newColumns.resize(getHeigth());
            newStructure[rowIndex] = newColumns;
        }

        for (int rowIndex = 0; rowIndex < getHeigth(); ++rowIndex) {
            for (int columnIndex = 0; columnIndex < getWidth(); ++columnIndex) {
                newStructure[columnIndex][rowIndex] = structure[rowIndex][columnIndex];
            }
        }

        structure = newStructure;
        shiftForFlip();
    }

};


class GameMap {
    int width;
    int heigth;

public:
    int getWidth() {
        return width;
    }

    int getHeight() {
        return heigth;
    }

    vector<vector<coloredChar>> field;

    GameMap() {
    }

    void init(int width, int heigth){
        this->width = width;
        this->heigth = heigth;

        field.resize(heigth);

        for (int rowIndex = 0; rowIndex < heigth; ++rowIndex) {
            vector<coloredChar> columnIndexs;
            columnIndexs.resize(width);
            field[rowIndex] = columnIndexs;
            for (int columnIndex = 0; columnIndex < width; ++columnIndex) {
                field[rowIndex][columnIndex] = coloredChar{ MAP_EMPTY_SPACE, WHITE };
            }
        }
    }

    void draw(Block block) {
        int startX = block.getBound(LEFT_BOUND);
        int endX = block.getBound(RIGHT_BOUND);
        int startY = block.getBound(TOP_BOUND);
        int endY = block.getBound(BOTTOM_BOUND);

        for (int rowIndex = 0; rowIndex < heigth; ++rowIndex) {
            cout << MAP_WALL;
            for (int columnIndex = 0; columnIndex < width; ++columnIndex) {
                coloredChar letter;
                char blockLetter = MAP_EMPTY_SPACE;

                if ((rowIndex >= startY && rowIndex <= endY) && (columnIndex >= startX && columnIndex <= endX)) {
                    blockLetter = block.structure[rowIndex - block.originY][columnIndex - block.originX];
                }
                if (blockLetter == MAP_EMPTY_SPACE) {
                    letter = field[rowIndex][columnIndex];
                } else {
                    letter = coloredChar { blockLetter, block.color};
                }
                string text { letter.letter };
                coutTextWithColor(text, letter.color, true);

            }
            cout << MAP_WALL << "\n";
        }
        drawFloor();
    }

    void drawFloor() {
        cout << MAP_WALL;
        for (int columnIndex = 0; columnIndex < width; ++columnIndex) {
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
        int endY = block.getHeigth();

        for (int rowIndex = 0; rowIndex < endY; ++rowIndex) {
            for (int columnIndex = 0; columnIndex < endX; ++columnIndex) {
                char letterOfBlock = block.structure[rowIndex][columnIndex];
                if (letterOfBlock == BLOCK_MOVING) {
                    int newFieldRow = rowIndex + block.originY + 1;
                    int fieldColumn = columnIndex + block.originX;

                    char cell = field[newFieldRow][fieldColumn].letter;

                    if (cell == BLOCK_FIXED) {
                        return true;
                    }
                }
            }
        }
    }

    void placeBlock(Block block) {
        int endX = block.getWidth();
        int endY = block.getHeigth();

        for (int rowIndex = 0; rowIndex < endY; ++rowIndex) {
            for (int columnIndex = 0; columnIndex < endX; ++columnIndex) {
                char letterOfBlock = block.structure[rowIndex][columnIndex];
                if (letterOfBlock == BLOCK_MOVING) {
                    int fieldRow = rowIndex + block.originY;
                    int fieldColumn = columnIndex + block.originX;
                    
                    field[fieldRow][fieldColumn] = coloredChar{ BLOCK_FIXED, block.color };
                }
            }
        }
    }

    bool hasBoundInRight(Block block, int shift=1) {
        int newX = block.getBound(RIGHT_BOUND) + shift;
        return newX >= width;
    }

    bool hasBoundInLeft(Block block, int shift=1) {
        int newX = block.getBound(LEFT_BOUND) - shift;
        return newX < 0;
    }

    bool hasCollisionInRightBottom(Block block, int shift=1) {
        int newX = block.getBound(RIGHT_BOUND) + shift;

        char letterOfBlock = block.structure[block.getHeigth() - 1][block.getWidth() - 1];
        char cell = field[block.getBound(BOTTOM_BOUND)][newX].letter;

        return letterOfBlock == BLOCK_MOVING && cell == BLOCK_FIXED;
    }

    bool hasCollisionInLeftBottom(Block block, int shift=1) {
        int newX = block.getBound(LEFT_BOUND) - shift;

        char letterOfBlock = block.structure[block.getHeigth() - 1][0];
        char cell = field[block.getBound(BOTTOM_BOUND)][newX].letter;

        return letterOfBlock == BLOCK_MOVING && cell == BLOCK_FIXED;
    }

    bool hasBoundOrCollisionForFlip(Block block) {
        if (block.getWidth() == block.getHeigth()) {
            return false;
        }

        block.flip();
        int blockWidth = block.getWidth();
        int blockHeight = block.getHeigth();
        int newX = block.getBound(RIGHT_BOUND);
        int newY = block.getBound(BOTTOM_BOUND);

        // проверка на выход за границы
        if (newY >= heigth || hasBoundInLeft(block, 0) || hasBoundInRight(block, 0) ) {
            return true;
        } else {
            return hasCollisionInLeftBottom(block, 0) || hasCollisionInRightBottom(block, 0);
        }
    }

    bool hasBoundOrCollisionForOperation(Block block, int operation) {
        char letterOfBlock;
        char cell;
        int newX;

        if (operation == FLIP) {
            return hasBoundOrCollisionForFlip(block);
        }
        if (hasBoundInRight(block) || hasBoundInLeft(block)) {
            return true;
        } else {
            if (operation == TO_RIGHT) {
                return hasCollisionInRightBottom(block);
            }

            if (operation == TO_LEFT) {
                return hasCollisionInLeftBottom(block);
            }
        }
        
    }

    void checkAllLayers(Block block, int* gameManagerScore) {
        int startY = block.getBound(TOP_BOUND);
        int endY = block.getBound(BOTTOM_BOUND);

        int rowIndex = startY;

        while (rowIndex <= endY) {
            if (isLayerFull(rowIndex)) {
                *gameManagerScore += width;
                removeLayer(rowIndex);
            }

            rowIndex += 1;
        }
    }

    void removeLayer(int rowIndex) {
        while (rowIndex > 0) {
            for (int columnIndex = 0; columnIndex < width; ++columnIndex) {
                field[rowIndex][columnIndex] = field[rowIndex - 1][columnIndex];
            }

            rowIndex -= 1;
        }
    }

    bool isLayerFull(int rowIndex) {
        for (int columnIndex = 0; columnIndex < width; ++columnIndex) {
            char cell = field[rowIndex][columnIndex].letter;

            if (cell == MAP_EMPTY_SPACE) {
                return false;
            }
        }

        return true;
    }

    bool anyStillBlockOnTop() {
        for (int columnIndex = 0; columnIndex < width; ++columnIndex) {
            char cell = field[0][columnIndex].letter;

            if (cell == BLOCK_FIXED) {
                return true;
            }
        }

        return false;
    }

};


class GameManager {
    int score = 0;
public:
    GameMap gameMap;
    Block fallingBlock;

    int getRandomNumber(int start, int end) {
        random_device rd;
        mt19937 generator(rd());
        uniform_int_distribution<> distribution(start, end);

        return distribution(generator);
    }

    void chooseBlock() {
        fallingBlock = Block();
        fallingBlock.color = getRandomNumber(1, 14);

        int typeOfBlock = getRandomNumber(0, 6);
        switch (typeOfBlock) {
            case 0:
                fallingBlock.structure = LineBlockStructure;
                break;
            case 1:
                fallingBlock.structure = OBlockStructure;
                break;
            case 2:
                fallingBlock.structure = LBlockStructure;
                break;
            case 3:
                fallingBlock.structure = JBlockStructure;
                break;
            case 4:
                fallingBlock.structure = ZBlockStructure;
                break;
            case 5:
                fallingBlock.structure = TBlockStructure;
                break;
            case 6:
                fallingBlock.structure = SBlockStructure;
                break;
        }
        fallingBlock.originY = 0;
        fallingBlock.originX = (gameMap.getWidth() / 2) - (fallingBlock.getWidth() / 2);
    }

    GameManager(int width, int height) {
        gameMap.init(width, height);
        chooseBlock();
    }

    void drawGameMap() {
        gameMap.draw(fallingBlock);
    }

    bool isOver() {
        bool anyBlock = gameMap.anyStillBlockOnTop();
        return anyBlock;
    }

    int getScore() {
        return score;
    }

    void gameCycle(int operation) {
        if (gameMap.isCollideWithFloor(fallingBlock)) {
            gameMap.placeBlock(fallingBlock);
            gameMap.checkAllLayers(fallingBlock, &score);
            chooseBlock();
        } else {
            fallingBlock.fall();

            switch (operation) {
                case TO_RIGHT:
                    if (!gameMap.hasBoundOrCollisionForOperation(fallingBlock, operation)) {
                        fallingBlock.moveToRight(true);
                    }
                    break;
                case TO_LEFT:
                    if (!gameMap.hasBoundOrCollisionForOperation(fallingBlock, operation)) {
                        fallingBlock.moveToRight(false);
                    }
                    break;
                case FLIP:
                    if (!gameMap.hasBoundOrCollisionForOperation(fallingBlock, operation)) {
                        fallingBlock.flip();
                    }
                    break;
            }
        }
    }
};


int handleInput(float* tickSpeed) {
    char letter = ';';
    
    if (_kbhit()) {
        letter = _getch();
        letter = toLower(letter);
    }

    if (letter == GAME_KEY_RIGHT || letter == GAME_KEY_RIGHT_ALT) {
        return TO_RIGHT;
    } else if (letter == GAME_KEY_LEFT || letter == GAME_KEY_LEFT_ALT) {
        return TO_LEFT;
    } else if (letter == GAME_KEY_FLIP || letter == GAME_KEY_FLIP_ALT) {
        return FLIP;
    }

    if (letter == GAME_KEY_FALL || letter == GAME_KEY_FALL_ALT) {
        *tickSpeed = FAST_VELOCITY;
    } else {
        *tickSpeed = NORMAL_VELOCITY;
    }
}

string characterToUpperString(char character) {
    character = toUpper(character);
    return string { character };
};


void drawKeys() {
    string rightHint = "Вправо: " + characterToUpperString(GAME_KEY_RIGHT) + '\n';
    string leftHint = "Влево: " + characterToUpperString(GAME_KEY_LEFT) + '\n';
    string fallHint = "Ускоренное падение: " + characterToUpperString(GAME_KEY_FALL) + '\n';
    string flipHint = "Поворот (180 градусов): " + characterToUpperString(GAME_KEY_FLIP) + '\n';
    string allHints = rightHint + leftHint + fallHint + flipHint;
    coutTextWithColor(allHints, YELLOW);
}

void drawUi(GameManager gameManager) {
    string score = "Очки: " + to_string(gameManager.getScore()) + '\n';
    coutTextWithColor(score, GREEN);
    
    if (gameManager.isOver()) {
        coutTextWithColor("ВЫ ПРОИГРАЛИ!\n", RED);
    } else {
        drawKeys();
    }
}

void tick(float sec) {
    consoleSleep(sec * 1000);
}

int main()
{   
    system("chcp 1251");
    setLocale(LC_ALL, "UTF-8");
    int width = 10;
    int heigth = 15;
    float tickSpeed = NORMAL_VELOCITY;
    GameManager gameManager = GameManager(width, heigth);

    while (!gameManager.isOver()) {
        int operation = handleInput(&tickSpeed);
        gameManager.gameCycle(operation);        
        system("cls");
        gameManager.drawGameMap();
        drawUi(gameManager);
        tick(tickSpeed);
    }
}

