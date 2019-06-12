#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <set>

#define FIELD_WIDTH 20
#define FIELD_HEIGHT 20
#define FIELD_DRAW_WIDTH 30
#define FIELD_DRAW_HEIGHT 30
#define SPAWN_COOLDOWN 2000
#define WAY_DELAY 200

using namespace std;

map<string, sf::Texture> textures;
int error = 0;
vector<tuple<int, int>> optimal_way;

int field[FIELD_WIDTH][FIELD_HEIGHT];
sf::Sprite drawedField[FIELD_WIDTH][FIELD_HEIGHT];

void LoadTextureFromFile(string path, string name)
{
    sf::Texture texture;
    if (!texture.loadFromFile(path))
        error = EXIT_FAILURE;
    textures[name] = texture;
}

void ReadMapFromFile(string path)
{
    cout << "reading from file" << endl;
    ifstream file(path);
    if(file.is_open())
    {
        cout << "file opened" << endl;
        char c;
        int coord = 0;
        while(file.get(c))
        {
            int value = -1;
            switch(c)
            {
                case '*': //nothing
                    value = 0;
                    break;
                case 'o': //path
                    value = 1;
                    break;
                case 'A': //base
                    value = 2;
                    break;
                case 'X': //Start point
                    value = 3;
                    break;
                case 'T': //tower place
                    value = 4;
                    break;
            }
            if(value >= 0)
            {
                field[coord%FIELD_WIDTH][coord/FIELD_WIDTH] = value;
                coord++;
            }
        }
        file.close();
    }
    cout << "reading from file successfully" << endl;
}

void WriteMapToFile(string path)
{
    cout << "writing to file" << endl;
    ofstream file(path);
    if(file.is_open())
    {
        for(int x = 0; x < FIELD_WIDTH; x++)
        {
            for(int y = 0; y < FIELD_WIDTH; y++)
            {
                char c;
                switch(field[x][y])
                {
                case 0: //nothing
                    c = '*';
                    break;
                case 1: //path
                    c = 'o';
                    break;
                case 2: //base
                    c = 'A';
                    break;
                case 3: //Start point
                    c = 'X';
                    break;
                case 4: //tower place
                    c = 'T';
                    break;
                }
                file << c << " ";
            }
            file << endl;
        }
    }
    cout << "writing to file successful" << endl;
}

const vector<int> emptyVector = {};

map<int, int> EnemiesTarget;
map<int, sf::Sprite> EnemiesSprites;
map<int, int> ticks;
int spawnX = 0;
int spawnY = 0;
set<int> ids;
int lastId = 0;
int maxTicks = 0;
sf::Font font;

void SpawnEnemy()
{
    int id = ++lastId;
    ids.insert(id);
    EnemiesTarget[id] = 0;
    ticks[id] = 0;
    sf::Sprite sprite(textures["enemy"]);
    sprite.setPosition(spawnX, spawnY);
    EnemiesSprites[id] = sprite;
}

void RemoveEnemy(int id)
{
    ids.erase(id);
}

int spawnNew = 0;

void DrawEnemy(sf::RenderWindow &app)
{
    if(spawnNew%SPAWN_COOLDOWN==0)
        SpawnEnemy();
    spawnNew++;
    for(auto id = ids.begin(); id != ids.end(); ++id)
    {
        if(ticks[*id]/WAY_DELAY >= maxTicks)
        {
            RemoveEnemy(*id);
            continue;
        }
        auto point = optimal_way[ticks[*id]/WAY_DELAY];
        EnemiesSprites[*id].setPosition(get<0>(point) * FIELD_DRAW_WIDTH, get<1>(point) * FIELD_DRAW_HEIGHT);
        EnemiesSprites[*id].setScale(FIELD_DRAW_WIDTH * 1.0 / 50, FIELD_DRAW_HEIGHT * 1.0 / 50);
        ticks[*id]++;
        app.draw(EnemiesSprites[*id]);
    }
}

vector<int> Way(int x, int y, set<int> visited)
{
    if(x < 0 || x >= FIELD_WIDTH || y < 0 || y >= FIELD_HEIGHT)
        return emptyVector;
    if(field[x][y] == 2) // base cell
    {
        vector<int> way(visited.rbegin(), visited.rend());
        way.push_back(x+y*FIELD_HEIGHT);
        return way;
    }
    if(visited.find(x+y*FIELD_HEIGHT) != visited.end())
        return emptyVector;
    if(field[x][y] != 1 && field[x][y] != 3) //1 - path cell, 3 - start point
    {
        return emptyVector;
    }
    visited.insert(x+y*FIELD_HEIGHT);
    vector<int> up = Way(x, y - 1, visited);
    vector<int> down = Way(x, y + 1, visited);
    vector<int> left = Way(x - 1, y, visited);
    vector<int> right = Way(x + 1, y, visited);

    int sizes[4] = {0, 0, 0, 0};
    if(!up.empty())
        sizes[0] = up.size();
    if(!down.empty())
        sizes[1] = down.size();
    if(!left.empty())
        sizes[2] = left.size();
    if(!right.empty())
        sizes[3] = right.size();

    int chosen = -1;
    for(int i = 0; i < 4; i++)
    {
        if(sizes[i] > 0 && (chosen < 0 || sizes[i] < sizes[chosen]))
            chosen = i;
    }

    switch(chosen)
    {
        case 0:
            return up;
        break;
        case 1:
            return down;
        break;
        case 2:
            return left;
        break;
        case 3:
            return right;
        break;
    }
    return emptyVector;
}

void ChooseOptimalWay()
{
    int x = -1;
    int y = -1;
    for(int xi = 0; xi < FIELD_WIDTH; xi++)
    {
        for(int yi = 0; yi < FIELD_HEIGHT; yi++)
        {
            if(field[xi][yi] == 3) // 3 = start point
            {
                x = xi;
                y = yi;
            }
        }
    }

    spawnX = x;
    spawnY = y;

    set<int> visited;

    auto way = Way(x, y, visited);
    for (auto point = way.begin(); point != way.end(); ++point)
    {
        int x = *point%FIELD_HEIGHT;
        int y = *point/FIELD_HEIGHT;
        tuple<int,int> pointxy(x, y);
        optimal_way.push_back(pointxy);
        maxTicks++;
    }
}

void LoadTextures()
{
    LoadTextureFromFile("textures/black.bmp", "default");
    LoadTextureFromFile("textures/red.bmp", "spawn");
    LoadTextureFromFile("textures/enemy.bmp", "enemy");
    LoadTextureFromFile("textures/white.bmp", "way");
    LoadTextureFromFile("textures/tower_place.bmp", "tower_place");
    LoadTextureFromFile("textures/base.bmp", "base");

    if (!font.loadFromFile("textures/font.ttf"))
    {
        error = EXIT_FAILURE;
    }
}

void CategorizeTowers(sf::RenderWindow &app)
{
    for(int x = 0; x < FIELD_WIDTH; x++)
    {
        for(int y = 0; y < FIELD_HEIGHT; y++)
        {
            if(field[x][y] == 4)
            {
                int rang = 0;
                for(int sx = x - 1; sx <= x + 1; sx++)
                {
                    for(int sy = y - 1; sy <= y + 1; sy++)
                    {
                        if(sx > 0 && sy > 0 && sx < FIELD_WIDTH && sy < FIELD_HEIGHT && field[sx][sy] == 1)
                        {
                            rang++;
                        }
                    }
                }
                sf::Text label;
                label.setFont(font);
                label.setPosition(x * FIELD_DRAW_WIDTH, y * FIELD_DRAW_HEIGHT);
                label.setString("" + to_string(rang));
                label.setCharacterSize(25);
                label.setFillColor(sf::Color(0,128,0));
                app.draw(label);
            }
        }
    }
}

int main()
{
    // Create the main window
    ReadMapFromFile("sample.txt");
    ChooseOptimalWay();

    sf::RenderWindow app(sf::VideoMode(800, 800), "SFML window");
    LoadTextures();
    if(error)
        return error;

    for(int x = 0; x < FIELD_WIDTH; x++)
    {
        for(int y = 0; y < FIELD_HEIGHT; y++)
        {
            string textureName;
            switch(field[x][y])
            {
                case 0: //nothing
                    textureName = "default";
                    break;
                case 1: //path
                    textureName = "way";
                    break;
                case 2: //base
                    textureName = "base";
                    break;
                case 3: //Start point
                    textureName = "spawn";
                    break;
                case 4: //tower place
                    textureName = "tower_place";
                    break;
            }
            sf::Sprite sprite(textures[textureName]);
            sprite.setPosition(x * FIELD_DRAW_WIDTH, y * FIELD_DRAW_HEIGHT);
            sprite.setScale(FIELD_DRAW_WIDTH * 1.0 / 50, FIELD_DRAW_HEIGHT * 1.0 / 50);
            drawedField[x][y] = sprite;
        }
    }

	// Start the game loop
    while (app.isOpen())
    {
        // Process events
        sf::Event event;
        while (app.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                app.close();
        }

        // Clear screen
        app.clear();

        // Draw the sprite
        for(int x = 0; x < FIELD_WIDTH; x++)
        {
            for(int y = 0; y < FIELD_HEIGHT; y++)
            {
                app.draw(drawedField[x][y]);
            }
        }

        DrawEnemy(app);
        CategorizeTowers(app);

        // Update the window
        app.display();
    }

    return EXIT_SUCCESS;
}
