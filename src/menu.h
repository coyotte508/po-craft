#ifndef MENU_H
#define MENU_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class Controller;

class Menu {
public:
    Menu(sf::RenderTarget &tar, Controller &ctr);
    ~Menu();

    void draw();
    void start();
    void handleKeyPress(sf::Event::KeyEvent &e);
    bool running();
private:
    bool isRunning;
    unsigned current; /* Current item in the menu layout */

    void increaseCurrent();
    void decreaseCurrent();
    void resetColors();
    void loadText();

    std::vector<sf::Text*> descs;
    std::vector<sf::Text*> keys;
    sf::Font font;
    Controller *controller;
    sf::RenderTarget *target;
    sf::Color selectedColor;
    std::vector<int> layout;
};

#endif // MENU_H
