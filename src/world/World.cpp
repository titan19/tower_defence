#include "World.h"
#include <stdlib.h>

World::World(std::map<std::string, sf::Texture> textures, int cellWidth, int cellHeight)
{
    m_textures = textures;
    Entity entity(textures["enemy"]);
    AddEntity(entity);
}

World::~World()
{
}

void World::DrawWorld(sf::RenderWindow &app)
{
    for (auto &entity: m_entities)
    {
        auto sprite = entity.GetSprite();
        app.draw(*sprite);
    }
}

void World::AddEntity(Entity entity)
{
    m_entities.push_back(entity);
}


