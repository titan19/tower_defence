#ifndef WORLD_H
#define WORLD_H

#include "Entity.h"

class World
{
    public:
        World(std::map<std::string, sf::Texture> textures, int width, int height);
        virtual ~World();
        virtual void DrawWorld(sf::RenderWindow &app);
        virtual void AddEntity(Entity entity);

    protected:
        std::vector<Entity> m_entities;
        std::map<std::string, sf::Texture> m_textures;

    private:
};

#endif // WORLD_H
