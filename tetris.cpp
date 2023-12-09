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

enum COLORS { BLUE = 1, GREEN = 2, CYAN = 3, RED = 4, PURPLE = 5, YELLOW = 6, WHITE = 15 };
enum OPERATIONS {TO_RIGHT, TO_LEFT, FLIP};
enum BOUNDS { RIGHT_BOUND, LEFT_BOUND, TOP_BOUND, BOTTOM_BOUND };

vector<vector<char>> LineBlockStructure { {BLOCK_MOVING, BLOCK_MOVING, BLOCK_MOVING, BLOCK_MOVING} };
vector<vector<char>> LBlockStructure{ {BLOCK_MOVING, BLOCK_MOVING, BLOCK_MOVING},
                              {BLOCK_MOVING, MAP_VOID, MAP_VOID} };
vector<vector<char>> JBlockStructure{ {BLOCK_MOVING, MAP_VOID, MAP_VOID},
                              {BLOCK_MOVING, BLOCK_MOVING, BLOCK_MOVING} };
vector<vector<char>> TBlockStructure{ {MAP_VOID, BLOCK_MOVING, MAP_VOID},
                              {BLOCK_MOVING, BLOCK_MOVING, BLOCK_MOVING} };
vector<vector<char>> ZBlockStructure{ {BLOCK_MOVING, BLOCK_MOVING, MAP_VOID},
                              {MAP_VOID, BLOCK_MOVING, BLOCK_MOVING} };
vector<vector<char>> SBlockStructure{ {MAP_VOID, BLOCK_MOVING, BLOCK_MOVING},
                              {BLOCK_MOVING, BLOCK_MOVING, MAP_VOID} };
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

    void flip() {
        vector<vector<char>> newStructure;
        newStructure.resize(getWidth());

        for (int row = 0; row < getWidth(); ++row) {
            vector<char> newColumns;
            newColumns.resize(getHeigth());
            newStructure[row] = newColumns;
        }

        for (int row = 0; row < getHeigth(); ++row) {
            for (int column = 0; column < getWidth(); ++column) {
                newStructure[column][row] = structure[row][column];
            }
        }

        structure = newStructure;
    }

};


class Map {
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

    Map() {
    }

    void init(int width, int heigth){
        this->width = width;
        this->heigth = heigth;

        field.resize(heigth);

        for (int row = 0; row < heigth; ++row) {
            vector<coloredChar> columns;
            columns.resize(width);
            field[row] = columns;
            for (int column = 0; column < width; ++column) {
                field[row][column] = coloredChar{ MAP_VOID, WHITE };
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
                coloredChar letter;
                char blockLetter = MAP_VOID;

                if ((row >= startY && row <= endY) && (column >= startX && column <= endX)) {
                    blockLetter = block.structure[row - block.originY][column - block.originX];
                }
                if (blockLetter == MAP_VOID) {
                    letter = field[row][column];
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
        int endY = block.getHeigth();

        for (int row = 0; row < endY; ++row) {
            for (int column = 0; column < endX; ++column) {
                char letterBlock = block.structure[row][column];
                if (letterBlock == BLOCK_MOVING) {
                    int newFieldRow = row + block.originY + 1;
                    int fieldColumn = column + block.originX;

                    char cell = field[newFieldRow][fieldColumn].letter;

                    if (cell == BLOCK_STILL) {
                        return true;
                    }
                }
            }
        }
    }

    void placeBlock(Block block) {
        int endX = block.getWidth();
        int endY = block.getHeigth();

        for (int row = 0; row < endY; ++row) {
            for (int column = 0; column < endX; ++column) {
                char letterBlock = block.structure[row][column];
                if (letterBlock == BLOCK_MOVING) {
                    int fieldRow = row + block.originY;
                    int fieldColumn = column + block.originX;
                    
                    field[fieldRow][fieldColumn] = coloredChar{ BLOCK_STILL, block.color };
                }
            }
        }
    }

    bool isBoundsToMove(Block block, int operation) {
        char letterBlock;
        char cell;

        // todo если флип будет центрироваться, то чтобы еще по горизонтали была проверка
        if (operation == FLIP) {
            int newY = block.getBound(RIGHT_BOUND);
            int newX = block.getBound(BOTTOM_BOUND);

            if (newY >= heigth || newX >= width) {
                return true;
            } else {
                // todo проверить соприкасание с другими блоками
                return false;
            }
        }

        if (operation == TO_RIGHT) {
            int newX = block.getBound(RIGHT_BOUND) + 1;
            if (newX >= width) {
                return true;
            }

            letterBlock = block.structure[block.getHeigth() - 1][block.getWidth() - 1];
            cell = field[block.getBound(BOTTOM_BOUND)][newX].letter;
        }

        if (operation == TO_LEFT) {
            int newX = block.getBound(LEFT_BOUND) - 1;
            if (newX < 0) {
                return true;
            }

            letterBlock = block.structure[block.getHeigth() - 1][0];
            cell = field[block.getBound(BOTTOM_BOUND)][newX].letter;
        }

        return letterBlock == BLOCK_MOVING && cell == BLOCK_STILL;
    }

    void checkAllLayers(Block block, int* gmScore) {
        int startY = block.getBound(TOP_BOUND);
        int endY = block.getBound(BOTTOM_BOUND);

        int row = startY;

        while (row <= endY) {
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
            char cell = field[row][column].letter;

            if (cell == MAP_VOID) {
                return false;
            }
        }

        return true;
    }

    bool anyStillBlockOnTop() {
        for (int column = 0; column < width; ++column) {
            char cell = field[0][column].letter;

            if (cell == BLOCK_STILL) {
                return true;
            }
        }

        return false;
    }

};


class GameManager {
    int score = 0;
public:
    Map gameMap;
    Block currentBlock;

    int getRandomNumber(int start, int end) {
        random_device rd;
        mt19937 generator(rd());
        uniform_int_distribution<> distribution(start, end);

        return distribution(generator);
    }

    void chooseBlock() {
        currentBlock = Block();
        currentBlock.color = getRandomNumber(1, 14);

        int typeOfBlock = getRandomNumber(0, 6);
        switch (typeOfBlock)
        {
        case 0:
            currentBlock.structure = LineBlockStructure;
            break;
        case 1:
            currentBlock.structure = OBlockStructure;
            break;
        case 2:
            currentBlock.structure = LBlockStructure;
            break;
        case 3:
            currentBlock.structure = JBlockStructure;
            break;
        case 4:
            currentBlock.structure = ZBlockStructure;
            break;
        case 5:
            currentBlock.structure = TBlockStructure;
            break;
        case 6:
            currentBlock.structure = SBlockStructure;
            break;
        }
        currentBlock.originY = 0;
        currentBlock.originX = (gameMap.getWidth() / 2) - (currentBlock.getWidth() / 2);
    }

    GameManager(int width, int height) {
        gameMap.init(width, height);
        chooseBlock();
    }

    void drawMap() {
        gameMap.draw(currentBlock);
    }

    bool isLose() {
        bool anyBlock = gameMap.anyStillBlockOnTop();
        return anyBlock;
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
    string score = "Очки: " + to_string(gm.getScore()) + '\n';
    coutTextWithColor(score, GREEN);
    
    if (gm.isLose()) {
        coutTextWithColor("ВЫ ПРОИГРАЛИ!\n", RED);
    }
}

void tick(float sec) {
    Sleep(sec * 1000);
}

int main()
{   
    setlocale(LC_ALL, "Russian");
    int width = 10;
    int heigth = 15;
    GameManager gm = GameManager(width, heigth);

    while (true) {
        int operation = handleInput();

        if (!gm.isLose()) {
            gm.gameCycle(operation);
        }
        system("cls");
        gm.drawMap();
        drawUi(gm);
        tick(0.5);
    }
}

