#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SFML/Window.hpp>
#include <map>

class Controller {
public:
    Controller();
    void loadControls();
    void saveControls();
    sf::Key::Code getKey(int function);
    int getFunction(int key);
    void setControl(int function, int key);

    static const char* getKeyName(int code);

    enum {
        MoveCharUp,
        MoveCharDown,
        MoveCharLeft,
        MoveCharRight,
        AlternateCamera,
        LastFunction
    };

private:
    static std::map<int, const char*> keyNames;

    class KeyCodeInitializer {
    public:
        KeyCodeInitializer();
    };

    static KeyCodeInitializer initializer;

    std::vector<int> keyCodes;
    std::map<int, int> keyFunctions;

    static const char * ident[LastFunction];
    static int defVal[LastFunction];
};

#endif // CONTROLLER_H
