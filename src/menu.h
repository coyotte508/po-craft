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
    int current;

    void increaseCurrent();
    void decreaseCurrent();
    void resetColors();
    void loadText();

    std::vector<sf::String*> descs;
    std::vector<sf::String*> keys;
    sf::Font font;
    Controller *controller;
    sf::RenderTarget *target;
    sf::Color selectedColor;
    std::vector<int> layout;
};

#endif // MENU_H
