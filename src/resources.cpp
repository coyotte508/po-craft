#include "resources.h"

Resources *Resources::singleton = nullptr;

Resources::Resources()
{
    font().loadFromFile("db/charset/YourHandwriting.ttf");
}

Resources &Resources::getSingleton()
{
    if (!singleton) {
        singleton = new Resources();
    }
    return *singleton;
}
