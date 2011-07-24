#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "menu.h"
#include "controller.h"

Menu::Menu(sf::RenderTarget &tar, Controller &ctrl) {
    controller = &ctrl;
    target = &tar;
    isRunning = false;
    current = 0;
    selectedColor = sf::Color(107,196,227);
    for (int i = 0; i < 5; i++) {
        descs.push_back(new sf::String());
        descs.back()->Move(50, 30 + 60*i);
        keys.push_back(new sf::String());
        keys.back()->Move(300, 30 + 60*i);
    }

    layout.push_back(Controller::MoveCharUp);
    layout.push_back(Controller::MoveCharDown);
    layout.push_back(Controller::MoveCharLeft);
    layout.push_back(Controller::MoveCharRight);
    layout.push_back(Controller::AlternateCamera);

    descs[0]->SetText("Move Up: ");
    descs[1]->SetText("Move Down: ");
    descs[2]->SetText("Move Left: ");
    descs[3]->SetText("Move Right: ");
    descs[4]->SetText("Alternate Camera: ");
}

void Menu::loadText() {

    for (int i = 0; i < 5; i++) {
        keys[i]->SetText(Controller::getKeyName(controller->getKey(layout[i])));
    }
}

template<class T>
void deleteOp(T *stuff) {
    delete stuff;
}

Menu::~Menu() {
    std::for_each(descs.begin(), descs.end(), deleteOp<sf::String>);
    std::for_each(keys.begin(), keys.end(), deleteOp<sf::String>);
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
    if (e.Code == sf::Key::Escape) {
        controller->loadControls();
        isRunning = false;
        return;
    }
    if (e.Code == sf::Key::Return) {
        controller->saveControls();
        isRunning = false;
        return;
    }
    if (e.Code == sf::Key::Up) {
        decreaseCurrent();
        return;
    }
    if (e.Code == sf::Key::Down) {
        increaseCurrent();
        return;
    }
    controller->setControl(layout[current], e.Code);
    keys[current]->SetText(Controller::getKeyName(controller->getKey(layout[current])));
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

    target->PreserveOpenGLStates(true);
    /* Draw the menu stuff */
    for (int i = 0; i < 5; i++) {
        target->Draw(*descs[i]);
        target->Draw(*keys[i]);
    }

    /* Switch back to 3D Mode */
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Menu::increaseCurrent() {
    current = (current + 1) %5;

    resetColors();
}

void Menu::decreaseCurrent() {
    current = (current + 4) %5;

    resetColors();
}

void Menu::resetColors() {
    for (int i = 0; i < descs.size(); i++) {
        if (current != i) {
            descs[i]->SetColor(sf::Color(255,255,255));
        } else {
            descs[i]->SetColor(selectedColor);
        }
    }
}
