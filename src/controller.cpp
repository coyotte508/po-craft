#include <QSettings>
#include "controller.h"

std::map<int, const char*> Controller::keyNames;
const char * Controller::ident[LastFunction] = {
    "MoveCharUp",
    "MoveCharDown",
    "MoveCharLeft",
    "MoveCharRight",
    "CameraZoomIn",
    "CameraZoomOut",
    "AlternateCamera"
};
int Controller::defVal[LastFunction] = {
    sf::Key::W,
    sf::Key::S,
    sf::Key::A,
    sf::Key::D,
    sf::Key::R,
    sf::Key::E,
    sf::Key::Q
};
Controller::KeyCodeInitializer Controller::initializer;

Controller::KeyCodeInitializer::KeyCodeInitializer() {
#define AutoAttribName(x) Controller::keyNames[sf::Key::x]=#x
    AutoAttribName(A);
    AutoAttribName(B);
    AutoAttribName(C);
    AutoAttribName(D);
    AutoAttribName(E);
    AutoAttribName(F);
    AutoAttribName(G);
    AutoAttribName(H);
    AutoAttribName(I);
    AutoAttribName(J);
    AutoAttribName(K);
    AutoAttribName(L);
    AutoAttribName(M);
    AutoAttribName(N);
    AutoAttribName(O);
    AutoAttribName(P);
    AutoAttribName(Q);
    AutoAttribName(R);
    AutoAttribName(S);
    AutoAttribName(T);
    AutoAttribName(U);
    AutoAttribName(V);
    AutoAttribName(W);
    AutoAttribName(X);
    AutoAttribName(Y);
    AutoAttribName(Z);
    AutoAttribName(Num0);
    AutoAttribName(Num1);
    AutoAttribName(Num2);
    AutoAttribName(Num3);
    AutoAttribName(Num4);
    AutoAttribName(Num5);
    AutoAttribName(Num6);
    AutoAttribName(Num7);
    AutoAttribName(Num8);
    AutoAttribName(Num9);
    AutoAttribName(Escape);
    AutoAttribName(Space);
#undef AutoAttribName
}

const char * Controller::getKeyName(int code) {
    if (keyNames.count(code) > 0) {
        return keyNames[code];
    } else {
        return "unknown";
    }
}

Controller::Controller() {
    keyCodes.resize(LastFunction);
    loadControls();
}

void Controller::loadControls() {
    QSettings settings("keyboard.ini", QSettings::IniFormat);
    keyFunctions.clear();
    for (int i = 0; i < LastFunction; i++) {
        keyCodes[i] = settings.value(ident[i], defVal[i]).toInt();
        keyFunctions[keyCodes[i]] = i;
    }
}

void Controller::saveControls() {
    QSettings settings("keyboard.ini", QSettings::IniFormat);
    for (int i = 0; i < LastFunction; i++) {
        settings.setValue(ident[i], keyCodes[i]);
    }
}

sf::Key::Code Controller::getKey(int function) {
    return (sf::Key::Code)keyCodes[function];
}

void Controller::setControl(int function, int key) {
    keyFunctions.erase(getKey(function));
    keyCodes[function] = key;
    keyFunctions[key] = function;
}
