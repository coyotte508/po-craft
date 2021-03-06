#include <QSettings>
#include "controller.h"

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
    sf::Keyboard::W,
    sf::Keyboard::S,
    sf::Keyboard::A,
    sf::Keyboard::D,
    sf::Keyboard::R,
    sf::Keyboard::E,
    sf::Keyboard::Q
};

const char * Controller::getKeyName(int code) {
    std::map<int, const char*> keyNames;

    if (keyNames.size() == 0) {
#define AutoAttribName(x) keyNames[sf::Keyboard::x]=#x
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

sf::Keyboard::Key Controller::getKey(int function) {
    return (sf::Keyboard::Key)keyCodes[function];
}

void Controller::setControl(int function, int key) {
    keyFunctions.erase(getKey(function));
    keyCodes[function] = key;
    keyFunctions[key] = function;
}
