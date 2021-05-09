#include <iostream>
#include <fstream>
#include <chrono>
#include <future>
#include <thread>
#include <ctime>
#include <mach-o/dyld.h>

#include <SFML/Graphics.hpp>

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>

#include "AdvancedMouse.hpp"
#include "RectangleBounds.hpp"
#include "RectangleButton.hpp"
#include "ImprovedText.hpp"
#include "ResourcePath.hpp"
#include "Canvas.hpp"

const int MIN_TIMER_VALUE = 100;
const int MAX_TIMER_VALUE = 1000;

int _fieldWidth = 50, _fieldHeight = 36;
std::vector< std::vector<bool> > _lastMatrix(_fieldWidth, std::vector<bool>(_fieldHeight, false));
std::queue< std::vector< std::vector<bool> > > _nextTurns;

std::future<void> _calculationHandle;
bool _exceptionBool = false;

enum GameStage {
    DrawChoice,
    DrawField,
    Game
};

// <Helper functions>
void clearQueue(std::queue< std::vector< std::vector<bool> > > &q) {
   std::queue< std::vector< std::vector<bool> > > empty;
   std::swap(q, empty);
}

std::string cropZeroes(std::string s) {
    for (int i = s.size() - 1; i >= 0; i--) {
        if (s[i] == '0' || s[i] == '.') s.pop_back();
        else return s;
    }
    return s;
}

std::string doubleToString(double d) { return cropZeroes(std::to_string(d)); }
// </Helper functions>

// <Game logic>
void tileSetToMatrix(Canvas& tileSet, std::vector< std::vector<bool> >& matrix) {
    for (int i = 0; i < _fieldWidth; i++)
        for (int j = 0; j < _fieldHeight; j++)
            matrix[i][j] = (tileSet.drawingCanvas[i][j].getFillColor() == sf::Color::White);
}

void matrixToTileSet(Canvas& tileSet, std::vector< std::vector<bool> >& matrix) {
    for (int i = 0; i < _fieldWidth; i++)
        for (int j = 0; j < _fieldHeight; j++)
            tileSet.drawingCanvas[i][j].setFillColor(matrix[i][j] ? sf::Color::White : sf::Color::Black);
}

bool cellIsLucky() {
    srand(time(0) * rand());
    return (rand() % 100) > 70;
}

void placeRandomCells(std::vector< std::vector<bool> >& matrix) {
    for (int i = 0; i < _fieldWidth; i++)
        for (int j = 0; j < _fieldHeight; j++)
            matrix[i][j] = cellIsLucky();
}

int amountOfNeighbors(int i, int j, std::vector< std::vector<bool> >& matrix) {
    int counter = 0;
    for (int currentI = i - 1; currentI <= i + 1; currentI++)
        for (int currentJ = j - 1; currentJ <= j + 1; currentJ++)
            if (!(currentI == i && currentJ == j))
                if (currentI >= 0 && currentI < _fieldWidth && currentJ >= 0 && currentJ < _fieldHeight)
                    if (matrix[currentI][currentJ]) counter++;
    return counter;
}

void calculateNextTurn() {
    int temp;
    std::vector< std::vector<bool> > newMatrix(_fieldWidth, std::vector<bool>(_fieldHeight));
    
    for (int i = 0; i < _fieldWidth; i++) {
        for (int j = 0; j < _fieldHeight; j++) {
            temp = amountOfNeighbors(i, j, _lastMatrix);
            
            if (_lastMatrix[i][j]) newMatrix[i][j] = (temp == 2 || temp == 3);
            else newMatrix[i][j] = (temp == 3);
        }
    }
    
    _nextTurns.push(newMatrix);
    _lastMatrix = newMatrix;
}

void setNextTurn(Canvas& tileSet, unsigned int& amountOfTurns, ImprovedText& amountOfTurnsText) {
    if (_nextTurns.size() > 0) {
        matrixToTileSet(tileSet, _nextTurns.front());
        amountOfTurns++;
        amountOfTurnsText.setText(std::to_string(amountOfTurns));
        _nextTurns.pop();
    }
}

void calculateNextTurnsSecondThread() {
    while (true) {
        if (_exceptionBool) throw std::runtime_error("stop"); // cause exception to stop function
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        if (_nextTurns.size() < 10) calculateNextTurn();
    }
}
// </Game logic>

void windowStart(sf::RenderWindow& window) {
    window.setFramerateLimit(120);
    window.clear(sf::Color::Black);
    window.display();
}

// <DrawChoice functions>
void enableDrawChoice(RectangleButton& randomFieldFill, RectangleButton& drawField) {
    randomFieldFill.interactable = true;
    drawField.interactable = true;
}

void displayDrawChoice(sf::RenderWindow& window, RectangleButton& randomFieldFill, ImprovedText& randomFieldFillText,
                       RectangleButton& drawField, ImprovedText& drawFieldText) {
    window.clear(sf::Color::Black);
    
    window.draw(randomFieldFill);
    window.draw(randomFieldFillText);
    
    window.draw(drawField);
    window.draw(drawFieldText);
    
    window.display();
}

void updateDrawChoice(sf::RenderWindow& window, AdvancedMouse& mouse,
                      RectangleButton& randomFieldFill, RectangleButton& drawField) {
    mouse.updateCondition(window);
    randomFieldFill.updateButtonCondition(mouse, window);
    drawField.updateButtonCondition(mouse, window);
}

void disableDrawChoice(sf::RenderWindow& window, RectangleButton& randomFieldFill, RectangleButton& drawField) {
    randomFieldFill.interactable = false;
    drawField.interactable = false;
}
// </DrawChoice functions>

// <DrawField functions>
void enableDrawField(RectangleButton& colorSwitcher, RectangleButton& gridSwitcher, RectangleButton& minusSpeedButton, RectangleButton& plusSpeedButton,
                     RectangleButton& eraseAllButton, RectangleButton& moveToGameStageButton, Canvas& tileSet) {
    colorSwitcher.interactable = true;
    gridSwitcher.interactable = true;
    minusSpeedButton.interactable = true;
    plusSpeedButton.interactable = true;
    eraseAllButton.interactable = true;
    moveToGameStageButton.interactable = true;
    tileSet.isActive = true;
}

void displayDrawField(sf::RenderWindow& window, sf::RectangleShape& divisorRect,
                      RectangleButton& colorSwitcher,            ImprovedText& colorSwitcherText,
                      RectangleButton& gridSwitcher,             ImprovedText& gridSwitcherText,
                      sf::RectangleShape& speedSwitchBackground, ImprovedText& currentSpeedText,
                      RectangleButton& minusSpeedButton,         ImprovedText& minusSpeedButtonText,
                      RectangleButton& plusSpeedButton,          ImprovedText& plusSpeedButtonText,
                      RectangleButton& eraseAllButton,           ImprovedText& eraseAllButtonText,
                      RectangleButton& moveToGameStageButton,    ImprovedText& moveToGameStageButtonText,
                      std::vector<sf::RectangleShape>& grid, Canvas& tileSet) {
    window.clear(sf::Color::Black);
    
    window.draw(colorSwitcher);
    window.draw(colorSwitcherText);
    
    window.draw(gridSwitcher);
    window.draw(gridSwitcherText);
    
    window.draw(speedSwitchBackground);
    window.draw(currentSpeedText);
    window.draw(minusSpeedButton);
    window.draw(minusSpeedButtonText);
    window.draw(plusSpeedButton);
    window.draw(plusSpeedButtonText);
    
    window.draw(eraseAllButton);
    window.draw(eraseAllButtonText);
    
    window.draw(moveToGameStageButton);
    window.draw(moveToGameStageButtonText);
    
    window.draw(divisorRect);
    
    window.draw(tileSet);
    
    for (int i = 0; i < grid.size(); i++) window.draw(grid[i]);
    
    window.display();
}

void updateDrawField(sf::RenderWindow& window, AdvancedMouse& mouse,
                     RectangleButton& colorSwitcher,    RectangleButton& gridSwitcher,
                     RectangleButton& minusSpeedButton, RectangleButton& plusSpeedButton,
                     RectangleButton& eraseAllButton,   RectangleButton& moveToGameStageButton,
                     Canvas& tileSet) {
    mouse.updateCondition(window);
    colorSwitcher.updateButtonCondition(mouse, window);
    gridSwitcher.updateButtonCondition(mouse, window);
    minusSpeedButton.updateButtonCondition(mouse, window);
    plusSpeedButton.updateButtonCondition(mouse, window);
    eraseAllButton.updateButtonCondition(mouse, window);
    moveToGameStageButton.updateButtonCondition(mouse, window);
    tileSet.updateCondition(mouse, window);
}

void disableDrawField(RectangleButton& colorSwitcher, RectangleButton& moveToGameStageButton,
                      Canvas& tileSet,
                      ImprovedText& colorSwitcherText, sf::Color& drawColor) {
    colorSwitcher.interactable = false;
    moveToGameStageButton.interactable = false;
    tileSet.isActive = false;
    
    colorSwitcherText.setText("White");
    drawColor = sf::Color::White;
}
// </DrawField functions>

// <Game functions>
void enableGame(RectangleButton& gridSwitcher, RectangleButton& minusSpeedButton, RectangleButton& plusSpeedButton,
                RectangleButton& newGameButton, RectangleButton& pauseButton) {
    gridSwitcher.interactable = true;
    minusSpeedButton.interactable = true;
    plusSpeedButton.interactable = true;
    newGameButton.interactable = true;
    pauseButton.interactable = true;
}

void initializeGame(Canvas& tileSet, bool isRandom, std::chrono::steady_clock::time_point& timePoint) {
    if (isRandom) {
        placeRandomCells(_lastMatrix);
        matrixToTileSet(tileSet, _lastMatrix);
    } else {
        tileSetToMatrix(tileSet, _lastMatrix);
    }
    
    _exceptionBool = false;
    if (_nextTurns.size() != 0) clearQueue(_nextTurns);
    _calculationHandle = std::async(calculateNextTurnsSecondThread);
    
    timePoint = std::chrono::steady_clock::now();
}

void displayGame(sf::RenderWindow& window, sf::RectangleShape& divisorRect,
                 sf::RectangleShape& speedSwitchBackground, ImprovedText& currentSpeedText,
                 sf::RectangleShape& amountOfTurnsRect, ImprovedText& amountOfTurnsText,
                 RectangleButton&    gridSwitcher,      ImprovedText& gridSwitcherText,
                 RectangleButton&    minusSpeedButton,  ImprovedText& minusSpeedButtonText,
                 RectangleButton&    plusSpeedButton,   ImprovedText& plusSpeedButtonText,
                 RectangleButton&    newGameButton,     ImprovedText& newGameButtonText,
                 RectangleButton&    pauseButton,       ImprovedText& pauseButtonText,
                 std::vector<sf::RectangleShape>& grid, Canvas& tileSet,
                 sf::RectangleShape& aysBackground, ImprovedText& aysText,
                 RectangleButton&    aysYes,        ImprovedText& aysYesText,
                 RectangleButton&    aysNo,         ImprovedText& aysNoText,
                 std::vector<sf::RectangleShape>& aysBackgroundBorders, bool aysMode) {
    window.clear(sf::Color::Black);
    
    // <Standart display>
    window.draw(divisorRect);
    
    window.draw(tileSet);
    
    window.draw(amountOfTurnsRect);
    window.draw(gridSwitcher);
    window.draw(speedSwitchBackground);
    window.draw(minusSpeedButton);
    window.draw(plusSpeedButton);
    window.draw(newGameButton);
    window.draw(pauseButton);
    
    for (int i = 0; i < grid.size(); i++) window.draw(grid[i]);
    // </Standart display>
    
    if (aysMode) {
        window.draw(aysBackground);
        for (int i = 0; i < 4; i++) window.draw(aysBackgroundBorders[i]);
        window.draw(aysText);
        
        window.draw(aysYes);
        window.draw(aysYesText);
        
        window.draw(aysNo);
        window.draw(aysNoText);
    } else {
        window.draw(amountOfTurnsText);
        window.draw(gridSwitcherText);
        window.draw(currentSpeedText);
        window.draw(minusSpeedButtonText);
        window.draw(plusSpeedButtonText);
        window.draw(newGameButtonText);
        window.draw(pauseButtonText);
    }
    
    window.display();
}

void updateGame(sf::RenderWindow& window, AdvancedMouse& mouse,
                RectangleButton& gridSwitcher, RectangleButton& minusSpeedButton, RectangleButton& plusSpeedButton,
                RectangleButton& newGameButton, RectangleButton& pauseButton,
                RectangleButton& aysYes, RectangleButton& aysNo, bool aysMode) {
    mouse.updateCondition(window);
    if (aysMode) {
        aysYes.updateButtonCondition(mouse, window);
        aysNo.updateButtonCondition(mouse, window);
    } else {
        gridSwitcher.updateButtonCondition(mouse, window);
        minusSpeedButton.updateButtonCondition(mouse, window);
        plusSpeedButton.updateButtonCondition(mouse, window);
        newGameButton.updateButtonCondition(mouse, window);
        pauseButton.updateButtonCondition(mouse, window);
    }
}

void disableGame(unsigned int& amountOfTurns,
                 std::vector<sf::RectangleShape>& grid, Canvas& tileSet,
                 RectangleButton& gridSwitcher, RectangleButton& minusSpeedButton, RectangleButton& plusSpeedButton,
                 RectangleButton& newGameButton, RectangleButton& pauseButton,
                 ImprovedText& amountOfTurnsText, ImprovedText& gridSwitcherText, ImprovedText& pauseButtonText) {
    amountOfTurns = 0;
    
    for (int i = 0; i < grid.size(); i++) grid[i].setFillColor(sf::Color::Black);
    
    tileSet.isActive = false;
    for (int i = 0; i < _fieldWidth; i++)
        for (int j = 0; j < _fieldHeight; j++)
            tileSet.drawingCanvas[i][j].setFillColor(sf::Color::Black);
    
    gridSwitcher.interactable = false;
    minusSpeedButton.interactable = false;
    plusSpeedButton.interactable = false;
    newGameButton.interactable = false;
    pauseButton.interactable = false;
    
    amountOfTurnsText.setText("0");
    gridSwitcherText.setText("Grid: OFF");
    pauseButtonText.setText("Pause");
    
    _exceptionBool = true;
}

void darkenGame(sf::RectangleShape& divisorRect,
                Canvas& tileSet, std::vector<sf::RectangleShape>& grid,
                sf::RectangleShape& amountOfTurnsRect, sf::RectangleShape& speedSwitchBackground,
                RectangleButton& gridSwitcher,  RectangleButton& minusSpeedButton, RectangleButton& plusSpeedButton,
                RectangleButton& newGameButton, RectangleButton& pauseButton,
                sf::Color& darkGrid, sf::Color& darkTileSet) {
    divisorRect.setFillColor(sf::Color(128, 128, 128, 255));
    
    amountOfTurnsRect.setFillColor(sf::Color(128, 128, 128, 255));
    gridSwitcher.setFillColor(sf::Color(128, 128, 128, 255));
    speedSwitchBackground.setFillColor(sf::Color(128, 128, 128, 255));
    minusSpeedButton.setFillColor(sf::Color(128, 128, 128, 255));
    plusSpeedButton.setFillColor(sf::Color(128, 128, 128, 255));
    newGameButton.setFillColor(sf::Color(128, 128, 128, 255));
    pauseButton.setFillColor(sf::Color(128, 128, 128, 255));
    
    if (grid[0].getFillColor() == sf::Color::Red)
        for (int i = 0; i < grid.size(); i++)
            grid[i].setFillColor(darkGrid);
    
    for (int i = 0; i < _fieldWidth; i++)
        for (int j = 0; j < _fieldHeight; j++)
            if (tileSet.drawingCanvas[i][j].getFillColor() == sf::Color::White)
                tileSet.drawingCanvas[i][j].setFillColor(darkTileSet);
}

void lightenGame(sf::RectangleShape& divisorRect,
                 Canvas& tileSet, std::vector<sf::RectangleShape>& grid,
                 sf::RectangleShape& amountOfTurnsRect, sf::RectangleShape& speedSwitchBackground,
                 RectangleButton& gridSwitcher,  RectangleButton& minusSpeedButton, RectangleButton& plusSpeedButton,
                 RectangleButton& newGameButton, RectangleButton& pauseButton,
                 sf::Color& darkGrid, sf::Color& darkTileSet) {
    divisorRect.setFillColor(sf::Color::White);
    
    amountOfTurnsRect.setFillColor(sf::Color::White);
    gridSwitcher.setFillColor(sf::Color::White);
    speedSwitchBackground.setFillColor(sf::Color::White);
    minusSpeedButton.setFillColor(sf::Color::White);
    plusSpeedButton.setFillColor(sf::Color::White);
    newGameButton.setFillColor(sf::Color::White);
    pauseButton.setFillColor(sf::Color::White);
    
    if (grid[0].getFillColor() == darkGrid)
        for (int i = 0; i < grid.size(); i++)
            grid[i].setFillColor(sf::Color::Red);
    
    for (int i = 0; i < _fieldWidth; i++)
        for (int j = 0; j < _fieldHeight; j++)
            if (tileSet.drawingCanvas[i][j].getFillColor() == darkTileSet)
                tileSet.drawingCanvas[i][j].setFillColor(sf::Color::White);
}

// </Game functions>

int main(int, char const**) {
    // <MainInitialization>
    sf::RenderWindow mainWindow(sf::VideoMode(1280, 720), "Conway's Game of Life", sf::Style::Titlebar | sf::Style::Close);
    
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png")) std::cout << "hell no" << std::endl;
    mainWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    windowStart(mainWindow);
    
    AdvancedMouse mouse;
    
    sf::Font font;
    if (!font.loadFromFile(resourcePath() + "SanFrancisco.ttf")) std::cout << "hell no" << std::endl;
    // </MainInitialization>
    
    // <DrawChoiceInitialization>
    RectangleButton randomFieldFill(sf::Vector2f(320.f, 150.f), sf::Vector2f(640.f, 150.f), sf::Color::White);
    randomFieldFill.buttonType = ButtonType::ClickOnRelease;
    ImprovedText randomFieldFillText("Use random field", font, 80, sf::Color::Black, randomFieldFill.getGlobalBounds());
    
    RectangleButton drawField(sf::Vector2f(320.f, 420.f), sf::Vector2f(640.f, 150.f), sf::Color::White);
    drawField.buttonType = ButtonType::ClickOnRelease;
    ImprovedText drawFieldText("Draw field", font, 80, sf::Color::Black, drawField.getGlobalBounds());
    // </DrawChoiceInitialization>
    
    // <DrawFieldInitialization>
    sf::RectangleShape divisorRect;
    divisorRect.setPosition(sf::Vector2f(274.f, 0.f));
    divisorRect.setSize(sf::Vector2f(5.f, 720.f));
    divisorRect.setFillColor(sf::Color::White);
    
    RectangleButton colorSwitcher(sf::Vector2f(50.f, 50.f), sf::Vector2f(174.f, 75.f), sf::Color::White);
    colorSwitcher.buttonType = ButtonType::ClickOnRelease;
    ImprovedText colorSwitcherText("White", font, 50, sf::Color::Black, colorSwitcher.getGlobalBounds());
    
    RectangleButton gridSwitcher(sf::Vector2f(50.f, 175.f), sf::Vector2f(174.f, 75.f), sf::Color::White);
    gridSwitcher.buttonType = ButtonType::ClickOnRelease;
    ImprovedText gridSwitcherText("Grid: OFF", font, 40, sf::Color::Black, gridSwitcher.getGlobalBounds());
    
    _fieldWidth--;
    _fieldHeight--;
    std::vector<sf::RectangleShape> grid(_fieldWidth + _fieldHeight + 4);
    float posX = 298.f, posY = 0.f;
    int gridPlaceCounter = 0;
    for ( ; gridPlaceCounter < _fieldWidth; gridPlaceCounter++) {
        grid[gridPlaceCounter].setPosition(sf::Vector2f(posX, posY));
        grid[gridPlaceCounter].setSize(sf::Vector2f(2.f, 720.f));
        grid[gridPlaceCounter].setFillColor(sf::Color::Black);
        posX += 20.f;
    }
    posX = 279.f;
    posY = 19.f;
    for ( ; gridPlaceCounter < (grid.size() - 4); gridPlaceCounter++) {
        grid[gridPlaceCounter].setPosition(sf::Vector2f(posX, posY));
        grid[gridPlaceCounter].setSize(sf::Vector2f(1000.f, 2.f));
        grid[gridPlaceCounter].setFillColor(sf::Color::Black);
        posY += 20.f;
    }
    
    grid[grid.size() - 4].setPosition(sf::Vector2f(279.f, 0.f));
    grid[grid.size() - 4].setSize(sf::Vector2f(1000.f, 1.f));
    grid[grid.size() - 4].setFillColor(sf::Color::Black);
    
    grid[grid.size() - 3].setPosition(sf::Vector2f(279.f, 719.f));
    grid[grid.size() - 3].setSize(sf::Vector2f(1000.f, 1.f));
    grid[grid.size() - 3].setFillColor(sf::Color::Black);
    
    grid[grid.size() - 2].setPosition(sf::Vector2f(279.f, 0.f));
    grid[grid.size() - 2].setSize(sf::Vector2f(1.f, 720.f));
    grid[grid.size() - 2].setFillColor(sf::Color::Black);
    
    grid[grid.size() - 1].setPosition(sf::Vector2f(1278.f, 0.f));
    grid[grid.size() - 1].setSize(sf::Vector2f(1.f, 720.f));
    grid[grid.size() - 1].setFillColor(sf::Color::Black);
    
    sf::Color darkGrid = sf::Color(128, 0, 0, 255);
    _fieldWidth++;
    _fieldHeight++;
    
    
    int nextTurnTimer = 400;
    double textTurnTimer = 0.4;
    
    sf::RectangleShape speedSwitchBackground;
    speedSwitchBackground.setPosition(sf::Vector2f(50.f, 300.f));
    speedSwitchBackground.setSize(sf::Vector2f(174.f, 60.f));
    speedSwitchBackground.setFillColor(sf::Color::White);
    ImprovedText currentSpeedText("1/" + doubleToString(textTurnTimer) + "s", font, 30, sf::Color::Black, speedSwitchBackground.getGlobalBounds());
    
    RectangleButton minusSpeedButton(sf::Vector2f(50.f, 362.f), sf::Vector2f(86.f, 58.f), sf::Color::White);
    minusSpeedButton.buttonType = ButtonType::ClickWithHold;
    ImprovedText minusSpeedButtonText("-", font, 40, sf::Color::Black, minusSpeedButton.getGlobalBounds());
    
    RectangleButton plusSpeedButton(sf::Vector2f(138.f, 362.f), sf::Vector2f(86.f, 58.f), sf::Color::White);
    plusSpeedButton.buttonType = ButtonType::ClickWithHold;
    ImprovedText plusSpeedButtonText("+", font, 40, sf::Color::Black, plusSpeedButton.getGlobalBounds());
    
    RectangleButton moveToGameStageButton(sf::Vector2f(50.f, 595.f), sf::Vector2f(174.f, 75.f), sf::Color::White);
    moveToGameStageButton.buttonType = ButtonType::ClickOnRelease;
    ImprovedText moveToGameStageButtonText("Start", font, 50, sf::Color::Black, moveToGameStageButton.getGlobalBounds());
    
    RectangleButton eraseAllButton(sf::Vector2f(50.f, 470.f), sf::Vector2f(174.f, 75.f), sf::Color::White);
    eraseAllButton.buttonType = ButtonType::ClickOnRelease;
    ImprovedText eraseAllButtonText("Erase all", font, 40, sf::Color::Black, eraseAllButton.getGlobalBounds());
    
    sf::Color drawColor = sf::Color::White;

    Canvas tileSet(279, 0, 1000, 720, (1000 / _fieldWidth), (720 / _fieldHeight), mouse, mainWindow);
    sf::Color darkTileSet = sf::Color(128, 128, 128, 255);
    
    std::chrono::steady_clock::time_point timePoint = std::chrono::steady_clock::now();
    int timer;
    // </DrawFieldInitialization>
    
    
    
    // <GameInitialization>
    unsigned int amountOfTurns = 0;
    
    sf::RectangleShape amountOfTurnsRect;
    amountOfTurnsRect.setPosition(sf::Vector2f(50.f, 50.f));
    amountOfTurnsRect.setSize(sf::Vector2f(174.f, 75.f));
    amountOfTurnsRect.setFillColor(sf::Color::White);
    ImprovedText amountOfTurnsText("0", font, 50, sf::Color::Black, amountOfTurnsRect.getGlobalBounds());
    
    RectangleButton newGameButton(sf::Vector2f(50.f, 470.f), sf::Vector2f(174.f, 75.f), sf::Color::White);
    newGameButton.buttonType = ButtonType::ClickOnRelease;
    ImprovedText newGameButtonText("New Game", font, 35, sf::Color::Black, newGameButton.getGlobalBounds());
    
    
    bool gameIsPaused = false;
    bool gameWasPaused = false;
    
    RectangleButton pauseButton(sf::Vector2f(50.f, 595.f), sf::Vector2f(174.f, 75.f), sf::Color::White);
    pauseButton.buttonType = ButtonType::ClickOnRelease;
    
    ImprovedText pauseButtonText("Pause", font, 50, sf::Color::Black, pauseButton.getGlobalBounds());
    
    
    bool aysMode = false;
    
    sf::RectangleShape aysBackground;
    aysBackground.setPosition(sf::Vector2f(320.f, 140.f));
    aysBackground.setSize(sf::Vector2f(640.f, 440.f));
    aysBackground.setFillColor(sf::Color::Black);
    
    ImprovedText aysText("Are you sure?", font, 90, sf::Color::White, sf::Rect<float>(320.f, 140.f, 640.f, 220.f));
    
    std::vector<sf::RectangleShape> aysBackgroundBorders(4);
    
    aysBackgroundBorders[0].setPosition(sf::Vector2f(320.f, 140.f));
    aysBackgroundBorders[0].setSize(sf::Vector2f(2.f, 440.f));
    aysBackgroundBorders[0].setFillColor(sf::Color::White);
    
    aysBackgroundBorders[1].setPosition(sf::Vector2f(958.f, 140.f));
    aysBackgroundBorders[1].setSize(sf::Vector2f(2.f, 440.f));
    aysBackgroundBorders[1].setFillColor(sf::Color::White);
    
    aysBackgroundBorders[2].setPosition(sf::Vector2f(320.f, 140.f));
    aysBackgroundBorders[2].setSize(sf::Vector2f(640.f, 2.f));
    aysBackgroundBorders[2].setFillColor(sf::Color::White);
    
    aysBackgroundBorders[3].setPosition(sf::Vector2f(320.f, 578.f));
    aysBackgroundBorders[3].setSize(sf::Vector2f(640.f, 2.f));
    aysBackgroundBorders[3].setFillColor(sf::Color::White);
    
    RectangleButton aysYes(sf::Vector2f(360.f, 400.f), sf::Vector2f(260.f, 140.f), sf::Color::White);
    aysYes.buttonType = ButtonType::ClickOnRelease;
    
    ImprovedText aysYesText("Yes", font, 80, sf::Color::Black, aysYes.getGlobalBounds());
    
    RectangleButton aysNo(sf::Vector2f(660.f, 400.f), sf::Vector2f(260.f, 140.f), sf::Color::White);
    aysNo.buttonType = ButtonType::ClickOnRelease;
    
    ImprovedText aysNoText("No", font, 80, sf::Color::Black, aysNo.getGlobalBounds());
    
    
    // </GameInitialization>
    
    GameStage gameStage = GameStage::DrawChoice;
    enableDrawChoice(randomFieldFill, drawField);
    
    while (mainWindow.isOpen()) {
        switch (gameStage) {
            case GameStage::DrawChoice:
                displayDrawChoice(mainWindow, randomFieldFill, randomFieldFillText, drawField, drawFieldText);
                updateDrawChoice(mainWindow, mouse, randomFieldFill, drawField);
                
                if (randomFieldFill.click) {
                    disableDrawChoice(mainWindow, randomFieldFill, drawField);
                    initializeGame(tileSet, true, timePoint);
                    enableGame(gridSwitcher, minusSpeedButton, plusSpeedButton, newGameButton, pauseButton);
                    gameStage = GameStage::Game;
                }
                
                if (drawField.click) {
                    gameStage = GameStage::DrawField;
                    disableDrawChoice(mainWindow, randomFieldFill, drawField);
                    enableDrawField(colorSwitcher, gridSwitcher, minusSpeedButton, plusSpeedButton, eraseAllButton, moveToGameStageButton, tileSet);
                }
                
                if (randomFieldFill.isPressed) {
                    randomFieldFill.setFillColor(sf::Color::Blue);
                    randomFieldFillText.setFillColor(sf::Color::White);
                } else {
                    randomFieldFill.setFillColor(sf::Color::White);
                    randomFieldFillText.setFillColor(sf::Color::Black);
                }
                
                if (drawField.isPressed) {
                    drawField.setFillColor(sf::Color::Blue);
                    drawFieldText.setFillColor(sf::Color::White);
                } else {
                    drawField.setFillColor(sf::Color::White);
                    drawFieldText.setFillColor(sf::Color::Black);
                }
                break; // DrawChoice
                
            case GameStage::DrawField:
                displayDrawField(mainWindow, divisorRect, colorSwitcher, colorSwitcherText, gridSwitcher, gridSwitcherText,
                                 speedSwitchBackground, currentSpeedText, minusSpeedButton, minusSpeedButtonText, plusSpeedButton, plusSpeedButtonText,
                                 eraseAllButton, eraseAllButtonText, moveToGameStageButton, moveToGameStageButtonText,
                                 grid, tileSet);
                updateDrawField(mainWindow, mouse, colorSwitcher, gridSwitcher, minusSpeedButton, plusSpeedButton, eraseAllButton, moveToGameStageButton, tileSet);
                
                if (colorSwitcher.click) {
                    if (drawColor == sf::Color::White) {
                        drawColor = sf::Color::Black;
                        colorSwitcherText.setText("Black");
                    } else {
                        drawColor = sf::Color::White;
                        colorSwitcherText.setText("White");
                    }
                }

                if (gridSwitcher.click) {
                    if (grid[0].getFillColor() == sf::Color::Black) {
                        for (int i = 0; i < grid.size(); i++) grid[i].setFillColor(sf::Color::Red);
                        gridSwitcherText.setText("Grid: ON");
                    } else {
                        for (int i = 0; i < grid.size(); i++) grid[i].setFillColor(sf::Color::Black);
                        gridSwitcherText.setText("Grid: OFF");
                    }
                }
                
                if (minusSpeedButton.click) {
                    if (nextTurnTimer < MAX_TIMER_VALUE) {
                        nextTurnTimer += 10;
                        textTurnTimer = nextTurnTimer / 1000.0;
                        currentSpeedText.setText("1/" + doubleToString(textTurnTimer) + "s");
                    }
                }
                
                if (plusSpeedButton.click) {
                    if (nextTurnTimer > MIN_TIMER_VALUE) {
                        nextTurnTimer -= 10;
                        textTurnTimer = nextTurnTimer / 1000.0;
                        currentSpeedText.setText("1/" + doubleToString(textTurnTimer) + "s");
                    }
                }
                
                if (eraseAllButton.click)
                    for (int i = 0; i < _fieldWidth; i++)
                        for (int j = 0; j < _fieldHeight; j++)
                            tileSet.drawingCanvas[i][j].setFillColor(sf::Color::Black);
                
                if (moveToGameStageButton.click) {
                    disableDrawField(colorSwitcher, moveToGameStageButton, tileSet, colorSwitcherText, drawColor);
                    initializeGame(tileSet, false, timePoint);
                    enableGame(gridSwitcher, minusSpeedButton, plusSpeedButton, newGameButton, pauseButton);
                    gameStage = GameStage::Game;
                }
                
                for (std::pair<int, int> p : tileSet.changedUnits)
                    tileSet.drawingCanvas[p.first][p.second].setFillColor(drawColor);

                if (colorSwitcher.isPressed) {
                    colorSwitcher.setFillColor(sf::Color::Blue);
                    colorSwitcherText.setFillColor(sf::Color::White);
                } else {
                    colorSwitcher.setFillColor(sf::Color::White);
                    colorSwitcherText.setFillColor(sf::Color::Black);
                }

                if (gridSwitcher.isPressed) {
                    gridSwitcher.setFillColor(sf::Color::Blue);
                    gridSwitcherText.setFillColor(sf::Color::White);
                } else {
                    gridSwitcher.setFillColor(sf::Color::White);
                    gridSwitcherText.setFillColor(sf::Color::Black);
                }
                
                if (minusSpeedButton.isPressed) {
                    minusSpeedButton.setFillColor(sf::Color::Blue);
                    minusSpeedButtonText.setFillColor(sf::Color::White);
                } else {
                    minusSpeedButton.setFillColor(sf::Color::White);
                    minusSpeedButtonText.setFillColor(sf::Color::Black);
                }
                
                if (plusSpeedButton.isPressed) {
                    plusSpeedButton.setFillColor(sf::Color::Blue);
                    plusSpeedButtonText.setFillColor(sf::Color::White);
                } else {
                    plusSpeedButton.setFillColor(sf::Color::White);
                    plusSpeedButtonText.setFillColor(sf::Color::Black);
                }
                
                if (eraseAllButton.isPressed) {
                    eraseAllButton.setFillColor(sf::Color::Blue);
                    eraseAllButtonText.setFillColor(sf::Color::White);
                } else {
                    eraseAllButton.setFillColor(sf::Color::White);
                    eraseAllButtonText.setFillColor(sf::Color::Black);
                }
                
                if (moveToGameStageButton.isPressed) {
                    moveToGameStageButton.setFillColor(sf::Color::Blue);
                    moveToGameStageButtonText.setFillColor(sf::Color::White);
                } else {
                    moveToGameStageButton.setFillColor(sf::Color::White);
                    moveToGameStageButtonText.setFillColor(sf::Color::Black);
                }
                break; // DrawField
                
            case GameStage::Game:
                displayGame(mainWindow, divisorRect, speedSwitchBackground, currentSpeedText,
                            amountOfTurnsRect, amountOfTurnsText, gridSwitcher, gridSwitcherText,
                            minusSpeedButton, minusSpeedButtonText, plusSpeedButton, plusSpeedButtonText,
                            newGameButton, newGameButtonText, pauseButton, pauseButtonText, grid, tileSet,
                            aysBackground, aysText, aysYes, aysYesText, aysNo, aysNoText, aysBackgroundBorders, aysMode);
                updateGame(mainWindow, mouse, gridSwitcher, minusSpeedButton, plusSpeedButton, newGameButton, pauseButton, aysYes, aysNo, aysMode);
                
                if (gameIsPaused) {
                    timePoint = std::chrono::steady_clock::now() - std::chrono::milliseconds(timer);
                } else {
                    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - timePoint).count() > nextTurnTimer) {
                        timePoint = std::chrono::steady_clock::now();
                        setNextTurn(tileSet, amountOfTurns, amountOfTurnsText);
                    }
                }
                
                if (gridSwitcher.click) {
                    if (grid[0].getFillColor() == sf::Color::Black) {
                        for (int i = 0; i < grid.size(); i++) grid[i].setFillColor(sf::Color::Red);
                        gridSwitcherText.setText("Grid: ON");
                    } else {
                        for (int i = 0; i < grid.size(); i++) grid[i].setFillColor(sf::Color::Black);
                        gridSwitcherText.setText("Grid: OFF");
                    }
                }
                
                if (minusSpeedButton.click) {
                    if (nextTurnTimer < MAX_TIMER_VALUE) {
                        nextTurnTimer += 10;
                        textTurnTimer = nextTurnTimer / 1000.0;
                        currentSpeedText.setText("1/" + doubleToString(textTurnTimer) + "s");
                    }
                }
                
                if (plusSpeedButton.click) {
                    if (nextTurnTimer > MIN_TIMER_VALUE) {
                        nextTurnTimer -= 10;
                        textTurnTimer = nextTurnTimer / 1000.0;
                        currentSpeedText.setText("1/" + doubleToString(textTurnTimer) + "s");
                    }
                }
                
                if (newGameButton.click) {
                    gameWasPaused = gameIsPaused;
                    gameIsPaused = true;
                    timer = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - timePoint).count();
                    
                    aysMode = true;
                    darkenGame(divisorRect, tileSet, grid, amountOfTurnsRect, speedSwitchBackground,
                               gridSwitcher, minusSpeedButton, plusSpeedButton, newGameButton, pauseButton, darkGrid, darkTileSet);
                    
                    gridSwitcher.interactable = false;
                    newGameButton.interactable = false;
                    pauseButton.interactable = false;
                    
                    aysYes.interactable = true;
                    aysNo.interactable = true;
                    newGameButton.click = false;
                }
                
                if (pauseButton.click) {
                    gameIsPaused = !gameIsPaused;
                    if (gameIsPaused) {
                        pauseButtonText.setText("Play");
                        timer = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - timePoint).count();
                    } else {
                        pauseButtonText.setText("Pause");
                    }
                }
                
                if (aysYes.click) {
                    lightenGame(divisorRect, tileSet, grid, amountOfTurnsRect, speedSwitchBackground,
                                gridSwitcher, minusSpeedButton, plusSpeedButton, newGameButton, pauseButton, darkGrid, darkTileSet);
                    disableGame(amountOfTurns, grid, tileSet, gridSwitcher, minusSpeedButton, plusSpeedButton,
                                newGameButton, pauseButton, amountOfTurnsText, gridSwitcherText, pauseButtonText);
                    aysMode = false;
                    gameIsPaused = false;
                    
                    gridSwitcher.interactable = false;
                    newGameButton.interactable = false;
                    pauseButton.interactable = false;
                    
                    aysYes.interactable = false;
                    aysNo.interactable = false;
                    
                    gameStage = GameStage::DrawChoice;
                    enableDrawChoice(randomFieldFill, drawField);
                    
                    aysYes.click = false;
                    aysYes.isPressed = false;
                }
                
                if (aysNo.click) {
                    lightenGame(divisorRect, tileSet, grid, amountOfTurnsRect, speedSwitchBackground,
                                gridSwitcher, minusSpeedButton, plusSpeedButton, newGameButton, pauseButton, darkGrid, darkTileSet);
                    aysMode = false;
                    gameIsPaused = gameWasPaused;
                    
                    gridSwitcher.interactable = true;
                    newGameButton.interactable = true;
                    pauseButton.interactable = true;
                    
                    aysYes.interactable = false;
                    aysNo.interactable = false;
                    
                    aysNo.click = false;
                    aysNo.isPressed = false;
                }
                
                
                if (aysMode) {
                    if (aysYes.isPressed) {
                        aysYes.setFillColor(sf::Color::Blue);
                        aysYesText.setFillColor(sf::Color::White);
                    } else {
                        aysYes.setFillColor(sf::Color::White);
                        aysYesText.setFillColor(sf::Color::Black);
                    }
                    
                    if (aysNo.isPressed) {
                        aysNo.setFillColor(sf::Color::Blue);
                        aysNoText.setFillColor(sf::Color::White);
                    } else {
                        aysNo.setFillColor(sf::Color::White);
                        aysNoText.setFillColor(sf::Color::Black);
                    }
                } else {
                    if (gridSwitcher.isPressed) {
                        gridSwitcher.setFillColor(sf::Color::Blue);
                        gridSwitcherText.setFillColor(sf::Color::White);
                    } else {
                        gridSwitcher.setFillColor(sf::Color::White);
                        gridSwitcherText.setFillColor(sf::Color::Black);
                    }
                    
                    if (minusSpeedButton.isPressed) {
                        minusSpeedButton.setFillColor(sf::Color::Blue);
                        minusSpeedButtonText.setFillColor(sf::Color::White);
                    } else {
                        minusSpeedButton.setFillColor(sf::Color::White);
                        minusSpeedButtonText.setFillColor(sf::Color::Black);
                    }
                    
                    if (plusSpeedButton.isPressed) {
                        plusSpeedButton.setFillColor(sf::Color::Blue);
                        plusSpeedButtonText.setFillColor(sf::Color::White);
                    } else {
                        plusSpeedButton.setFillColor(sf::Color::White);
                        plusSpeedButtonText.setFillColor(sf::Color::Black);
                    }
                    
                    if (newGameButton.isPressed) {
                        newGameButton.setFillColor(sf::Color::Blue);
                        newGameButtonText.setFillColor(sf::Color::White);
                    } else {
                        newGameButton.setFillColor(sf::Color::White);
                        newGameButtonText.setFillColor(sf::Color::Black);
                    }
                    
                    if (pauseButton.isPressed) {
                        pauseButton.setFillColor(sf::Color::Blue);
                        pauseButtonText.setFillColor(sf::Color::White);
                    } else {
                        pauseButton.setFillColor(sf::Color::White);
                        pauseButtonText.setFillColor(sf::Color::Black);
                    }
                }
                break; // Game
        }
        
        sf::Event event;
        while (mainWindow.pollEvent(event)) {
            switch(event.type) {
                case sf::Event::Closed:
                    _exceptionBool = true;
                    mainWindow.close();
                    break;
                    
                case sf::Event::LostFocus:
                    switch(gameStage) {
                        case GameStage::DrawChoice:
                            randomFieldFill.interactable = false;
                            drawField.interactable = false;
                            break; // DrawChoice
                            
                        case GameStage::DrawField:
                            colorSwitcher.interactable = false;
                            gridSwitcher.interactable = false;
                            for (int i = 0; i < _fieldWidth; i++)
                                for (int j = 0; j < _fieldHeight; j++)
                                    tileSet.isActive = false;
                            break; // DrawField
                            
                        case GameStage::Game:
                            if (aysMode) {
                                aysYes.interactable = false;
                                aysNo.interactable = false;
                            } else {
                                gridSwitcher.interactable = false;
                                newGameButton.interactable = false;
                                pauseButton.interactable = false;
                            }
                            break; // Game
                    }
                    break;
                    
                case sf::Event::GainedFocus:
                    switch(gameStage) {
                        case GameStage::DrawChoice:
                            randomFieldFill.interactable = true;
                            drawField.interactable = true;
                            break; // DrawChoice
                            
                        case GameStage::DrawField:
                            colorSwitcher.interactable = true;
                            gridSwitcher.interactable = true;
                            for (int i = 0; i < _fieldWidth; i++)
                                for (int j = 0; j < _fieldHeight; j++)
                                    tileSet.isActive = true;
                            break; // DrawField
                            
                        case GameStage::Game:
                            if (aysMode) {
                                aysYes.interactable = true;
                                aysNo.interactable = true;
                            } else {
                                gridSwitcher.interactable = true;
                                newGameButton.interactable = true;
                                pauseButton.interactable = true;
                            }
                            break; // Game
                    }
                    
                default:
                    break;
            }
        }
    }
}
