#ifndef FIELD_H
#define FIELD_H

#include <SFML/Graphics.hpp>

class Entity
{
    public:
        Entity(sf::Texture texture) : Entity(sf::Sprite(texture))
        {
        }
        Entity(sf::Sprite sprite);
        virtual ~Entity();
        virtual sf::Sprite* GetSprite()
        {
            return &Sprite;
        }
        int X;
        int Y;

    protected:
        sf::Sprite Sprite;

    private:
};

#endif // FIELD_H
