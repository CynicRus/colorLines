
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

#include <SFML/Graphics.hpp>

// Here is a small helper for you! Have a look.
#include "ResourcePath.hpp"

struct Cell {
    float x,y;
    int state = 0;
};

enum GameState {g_playing, g_endgame};

Cell map[9][9];
int path[9][9];
int wavemap[9][9];

int ci = -1, cj = -1;
int ci1 = -1, cj1 = -1;
bool clickb = false;

int numlines = 0, score = 0;
bool RoadFlag = false, DestroyFlag = false;

GameState state = g_playing;

//build the game grid
void buildField(int col, int row, sf::Sprite& s){
    for (int i = 0; i<=col-1;i++){
        for (int j = 0; j<=row-1;j++){
            sf::FloatRect bounds = s.getGlobalBounds();
            map[i][j].x = i * bounds.width;
            map[i][j].y = j * bounds.height;
            map[i][j].state = 0;
        }
    }
    
}

//get count of empty cells
int emptyCellsCount()
{
    int e = 0;
    for (int i = 0; i<=8; i++){
        for (int j = 0; j<=8; j++){
            if (map[i][j].state == 0){
                e++;
            }
        }
    }
    return e;
}

//check the grid borders

bool checkGridBounds(int X, int Y){
    bool r = false;
    if ((X>=0) && (X<=8) && (Y>=0) &&(Y<=8)) r = !r;
    return r;
    
}


//place balls
void placeBalls(int quantity){
    int i = 0;
    for (int stop = quantity, i = 1; i<=stop;i++ )
    {
        int x = rand()%9;
        int y = rand()%9;
        int b = 1+rand()%6;
        if (map[x][y].state == 0){
            map[x][y].state = b;
        }
    }
    
}

//the pathfinding algorithm

void makeWave(int x1, int y1, int x2, int y2){
    int k = 1;
    bool flag = true;
    for (int i = 0; i<=8; i++){
        for (int j = 0; j<=8 ;j++){
            if (map[i][j].state > 0)
                wavemap[i][j] = -1;
            else
                wavemap[i][j] = 0;
        }
    }
    wavemap[x1][y1] = k;
    while (flag){
        for (int i = 0; i<=8; i++){
            for (int j = 0; j<=8 ;j++){
                if (wavemap[i][j] == k) {
                    if ((wavemap[i - 1][j] == 0) && (checkGridBounds(i-1, j))){
                        wavemap[i - 1][j] = k + 1;
                        flag = true;
                    }
                    if ((wavemap[i + 1][j] == 0) && (checkGridBounds(i+1, j))){
                        wavemap[i + 1][j] = k + 1;
                        flag = true;
                    }
                    if ((wavemap[i][j - 1] == 0) && (checkGridBounds(i, j-1))){
                        wavemap[i][j - 1] = k + 1;
                        flag = true;
                    }
                    if ((wavemap[i][j + 1] == 0) && (checkGridBounds(i, j + 1))){
                        wavemap[i][j + 1] = k + 1;
                        flag = true;
                    }
                    
                }
            }
        }
        if (wavemap[x2][y2]>0)
            flag = false;
        else
            k+=1;
    }
}

void way(int x1, int y1, int x2, int y2){
    int k = wavemap[x2][y2];
    path[x2][y2] = k - wavemap[x1][y1];
    if ((checkGridBounds(x2-1, y2))&&(wavemap[x2 - 1][y2]==k-1)){
        way(x1, y1, x2 - 1, y2);
    }
        else if ((checkGridBounds(x2+1, y2))&&(wavemap[x2 + 1][y2]==k-1)){
            way(x1, y1, x2 + 1, y2);
    }
        else if ((checkGridBounds(x2, y2 - 1))&&(wavemap[x2][y2-1]==k-1)){
            way(x1, y1, x2, y2 - 1);
    }
        else if ((checkGridBounds(x2, y2 + 1))&&(wavemap[x2][y2 + 1]==k-1)){
            way(x1, y1, x2, y2 + 1);
    }
}

void updateInfo (int sc, int linesCount)
{
    numlines += linesCount;
    score += sc;
}

//destroy balls
void destroyBalls(int X, int Y, int k, int I, int J){
    int n = 0, dx = 0, dy = 0;
    dx=X;
    dy=Y;
    while (n !=k)
    {
        map[dx][dy].state = 0;
        n++;
        dx+=I;
        dy+=J;
    }
    //k is score for destroyed balls
    updateInfo(k, 1);
    DestroyFlag = true;
}

int checkLine(int X, int Y, int I, int J){
    int dx = 0, dy = 0, k = 0;
    dx = X;
    dy = Y;
    
    while (map[X][Y].state == map[dx][dy].state)
    {
        if (checkGridBounds(dx, dy)){
            dx += I;
            dy += J;
            k++;
        } else
            break;
    }
    return k;
}

void destroyLines(){
    DestroyFlag = false;
    for (int i = 0; i<=8; i++){
        for (int j = 0; j <= 8; j++){
            if (map[i][j].state != 0){
                if (checkLine(i, j, 1, 0) > 4)
                    destroyBalls(i, j, checkLine(i, j, 1, 0), 1, 0);
                else if (checkLine(i, j, 1, 1) > 4)
                    destroyBalls(i, j, checkLine(i, j, 1, 1), 1, 1);
                else if (checkLine(i, j, 0, 1) > 4)
                    destroyBalls(i, j, checkLine(i, j, 0, 1), 0, 1);
                else if (checkLine(i, j, -1, 1) > 4)
                    destroyBalls(i, j, checkLine(i, j, -1, 1), -1, 1);
                    
            }
        }
    }
}



//move balls
void ballMove(int x1, int y1, int x2, int y2){
    int X = 0; int Y = 0;
    for (int i = 0; i<=8; i++){
        for (int j = 0; j <= 8; j++){
            /*clean paths array*/
            path[i][j] = 0;
            wavemap[i][j] = 0;
        }
    }
    RoadFlag = true;
    makeWave(x1, y1, x2, y2);
    if (wavemap[x2][y2] > 0){
        way(x1, y1, x2, y2);
        X = x1;
        Y = y1;
        do{
            if (path[X-1][Y] - path[X][Y]  == 1){
                X-=1;
            }
            else if (path[X+1][Y] - path[X][Y] == 1){
                X+=1;
            }
            else if (path[X][Y-1] - path[X][Y] == 1){
                Y-=1;
            }
            else if (path[X][Y+1] - path[X][Y] == 1){
                Y+=1;
            }
            
        } while ((!(X==x2)&&!(Y==y2)));
        
    }
    
    map[x2][y2].state = map[x1][y1].state;
    map[x1][y1].state = 0;
    
}



void handleMove(){
    if (ci!=-1 && cj!=-1 && ci1!=-1 && cj1!=-1){
        if ((map[ci1][cj1].state == 0) && (clickb)){
            ballMove(ci, cj, ci1, cj1);
            clickb = false;
            if (RoadFlag){
                destroyLines();
                if (!DestroyFlag){
                    placeBalls(3);
                    destroyLines();
                }
            }
            RoadFlag = false;
            ci = -1; cj = -1; ci1 = -1; cj1 = -1;
            
        }
    } else if ((map[ci1][cj1].state!=0) && (clickb)){
        clickb = !clickb;
        ci = -1; cj = -1; ci1 = -1; cj1 = -1;
    }
}

void updateText(sf::Text& scoreTxt, sf::Text& linesTxt, sf::RenderWindow& window){
    scoreTxt.setString("Score: " + std::to_string(score));
    scoreTxt.setCharacterSize(34);
    scoreTxt.setFillColor(sf::Color::White);
    scoreTxt.setPosition(593, 24);
    linesTxt.setString("Lines: " + std::to_string(numlines));
    linesTxt.setCharacterSize(34);
    linesTxt.setFillColor(sf::Color::White);
    linesTxt.setPosition(593, 64);
    window.draw(scoreTxt);
    window.draw(linesTxt);
}


int main(int, char const**)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Color Lines Game");

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    sf::Texture fl,b1,b2,b3,b4,b5,b6;
    fl.loadFromFile(resourcePath()+"floor_t.png");
    b1.loadFromFile(resourcePath()+"B1.png");
    b2.loadFromFile(resourcePath()+"B2.png");
    b3.loadFromFile(resourcePath()+"B3.png");
    b4.loadFromFile(resourcePath()+"B4.png");
    b5.loadFromFile(resourcePath()+"B5.png");
    b6.loadFromFile(resourcePath()+"B6.png");
    
    sf::Sprite Floor(fl), ball1(b1), ball2(b2), ball3(b3), ball4(b4), ball5(b5), ball6(b6);
    
    sf::Font f;
    f.loadFromFile(resourcePath()+"XpressiveBlack Regular.ttf");
    
    sf::Text linesText, scoreText, endGameText;
    
    linesText.setFont(f);
    scoreText.setFont(f);
    
    buildField(9, 9, Floor);
    placeBalls(5);

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            
            //handle mouse click
            if (event.type == sf::Event::MouseButtonPressed){
                sf::Vector2f mouse = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x,event.mouseButton.y));
                if (event.mouseButton.button == sf::Mouse::Left){
                    for (int i = 0; i<=8; i++){
                        for (int j = 0; j<=8; j++){
                            //check the tile click
                            if ((mouse.x >=map[i][j].x) && (mouse.x <= map[i][j].x + 64/*floor tile size*/)
                                && (mouse.y >= map[i][j].y) && (mouse.y <= map[i][j].y + 64)){
                                if ((map[i][j].state != 0) && (!clickb)) {
                                    ci = i; cj = j;
                                    clickb = !clickb;
                                    break;
                                } else if ((map[i][j].state == 0) && (clickb)){
                                    ci1 = i; cj1 = j;
                                    break;
                                }
                            }
                        }
                    }
                    
                } else if (event.mouseButton.button == sf::Mouse::Right){
                    ci = -1; cj = -1;
                    if (clickb)
                        clickb = !clickb;
                    
                }
            }
        }

        // Clear screen
        window.clear();
        
        //draw game grid
        for (int i = 0; i<=8; i++){
            for (int j = 0; j<=8; j++){
                Floor.setPosition(map[i][j].x, map[i][j].y);
                window.draw(Floor);
            }
        }
        
        //draw balls
        for (int i = 0; i<=8; i++){
            for (int j = 0; j<=8; j++){
                sf::Sprite ball;
                switch (map[i][j].state) {
                    case 1:
                        ball = ball1;
                        break;
                    case 2:
                        ball = ball2;
                        break;
                    case 3:
                        ball = ball3;
                        break;
                    case 4:
                        ball = ball4;
                        break;
                    case 5:
                        ball = ball5;
                        break;
                    case 6:
                        ball = ball6;
                        break;
                    default:
                        break;
                }
                ball.setPosition(map[i][j].x + 16/*sprite width /2*/, map[i][j].y + 16/*height / 2*/);
                window.draw(ball);
            }
        }
        handleMove();
        updateText(scoreText, linesText, window);
        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
