#include "glu.h"

#include <SFML/Graphics.hpp>

#include "resources.h"
#include "menu.h"
#include "controller.h"

Menu::Menu(sf::RenderTarget &tar, Controller &ctrl) {
    controller = &ctrl;
    target = &tar;
    isRunning = false;
    current = 0;
    selectedColor = sf::Color(107,196,227);

    layout.push_back(Controller::MoveCharUp);
    layout.push_back(Controller::MoveCharDown);
    layout.push_back(Controller::MoveCharLeft);
    layout.push_back(Controller::MoveCharRight);
    layout.push_back(Controller::AlternateCamera);
    layout.push_back(Controller::CameraZoomIn);
    layout.push_back(Controller::CameraZoomOut);

    for (unsigned i = 0; i < layout.size(); i++) {
        descs.push_back(new sf::Text("", gbRes.font()));
        descs.back()->move(50, 30 + 60*i);
        keys.push_back(new sf::Text("", gbRes.font()));
        keys.back()->move(320, 30 + 60*i);
    }

    descs[0]->setString("Move Up: ");
    descs[1]->setString("Move Down: ");
    descs[2]->setString("Move Left: ");
    descs[3]->setString("Move Right: ");
    descs[4]->setString("Alternate Camera: ");
    descs[5]->setString("Camera Zoom In: ");
    descs[6]->setString("Camera Zoom Out: ");
}

void Menu::loadText() {

    for (unsigned i = 0; i < layout.size(); i++) {
        keys[i]->setString(Controller::getKeyName(controller->getKey(layout[i])));
    }
}

template<class T>
void deleteOp(T *stuff) {
    delete stuff;
}

Menu::~Menu() {
    std::for_each(descs.begin(), descs.end(), deleteOp<sf::Text>);
    std::for_each(keys.begin(), keys.end(), deleteOp<sf::Text>);
    descs.clear();
    keys.clear();
}

bool Menu::running() {
    return isRunning;
}

void Menu::start() {
    isRunning = true;
    current = 0;
    resetColors();
    loadText();
}

void Menu::handleKeyPress(sf::Event::KeyEvent &e) {
    if (e.code == sf::Keyboard::Escape) {
        controller->loadControls();
        isRunning = false;
        return;
    }
    if (e.code == sf::Keyboard::Return) {
        controller->saveControls();
        isRunning = false;
        return;
    }
    if (e.code == sf::Keyboard::Up) {
        decreaseCurrent();
        return;
    }
    if (e.code == sf::Keyboard::Down) {
        increaseCurrent();
        return;
    }
    controller->setControl(layout[current], e.code);
    keys[current]->setString(Controller::getKeyName(controller->getKey(layout[current])));
}

void Menu::draw()
{
    /* Switch to 2D Mode */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,800,0,600);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Draw the gray transparent zone over the game */
    glColor4f(0.5, 0.5, 0.5, 0.5);
    glBegin(GL_QUADS);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 600, 0);
    glVertex3f(800, 600, 0);
    glVertex3f(800, 0, 0);
    glEnd();

    target->pushGLStates();
    /* Draw the menu stuff */
    for (unsigned i = 0; i < layout.size(); i++) {
        target->draw(*descs[i]);
        target->draw(*keys[i]);
    }
    target->popGLStates();

    /* Switch back to 3D Mode */
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Menu::increaseCurrent() {
    current = (current + 1) % layout.size();

    resetColors();
}

void Menu::decreaseCurrent() {
    current = (current + layout.size()-1) % layout.size();

    resetColors();
}

void Menu::resetColors() {
    for (unsigned i = 0; i < descs.size(); i++) {
        if (current != i) {
            descs[i]->setColor(sf::Color(255,255,255));
        } else {
            descs[i]->setColor(selectedColor);
        }
    }
}
