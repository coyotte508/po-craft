#ifndef RESOURCES_H
#define RESOURCES_H

#include <SFML/Graphics/Font.hpp>
#include "macro.h"

class Resources
{
    PROPERTY(sf::Font, font)
public:
    Resources();

    static Resources & getSingleton();
private:
    static Resources *singleton;
};

#define gbRes Resources::getSingleton()

#endif // RESOURCES_H
