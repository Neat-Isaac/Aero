#pragma once
#include <SFML/Graphics.hpp>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>

#define ARGS 19

extern int N, DELAY, SPEED, L_BORDER, R_BORDER, U_BORDER, D_BORDER, SEP, SD, M, WCHART, CLF;
extern int BG_R, BG_G, BG_B, CT_R, CT_G, CT_B;
extern int player_direction, visiting_planet;
extern int c_moving, c_shooting, c_rotate, c_slide,c_pj;
extern int velocity;
extern bool is_moving, is_shooting, visiting_home, show_bag, pressed_J;
extern bool pused[1000][1000];

struct Point                            //别问，问就是Vector2f不会用也不敢用
{
    float x, y;
};
struct MetPros
{
    char name[5];
    int num;
};
struct Chart
{
    int sp, ep;
    Point start;
    Point end;
};
struct Planet
{
    int level;
    char name[5];
    int lines[5];
    MetPros product[6];
    MetPros sell[1000][2];      //数组开小导致了一系列bug，解决留念2023.02.09
    int left;
    int shopsize;
    bool visited;
    bool direction;
    bool favour;
    bool has_aero;
    int distance;
    Point pos;
};
struct Aero
{
    int index;
    int home;
    Point pos;
    int shoot_timer;
    float speed;
    float direction;
    int level;
};
struct Bullet
{
    int shooter;
    Point pos;
    float direction;
    int speed;
    int distance;
};

extern std::vector<MetPros> bag;
extern std::vector<Chart> charts;
extern std::vector<Planet> pdata;      //存储各星球数据的vector
extern std::vector<Aero> aeroes;
extern std::vector<Bullet> bullets;
extern Aero playership;

void readData();
void init();
void setCharts();
bool isSpaceEmpty(int x, int y);
void showMap();
double radian(int x);
float calcDistance(float x2, float y2, float x1, float y1);
Point calcPos(float distance, float p_angle);
float calcAngle(int x, int y);
void pick(MetPros thing);
bool lose(MetPros thing);
void buy(int number, bool flag1);
void control(sf::Event event);
void slide();
void fire(Aero ship);
void newAero(int home);
void moveBullets();
void moveAeroes();
void game();
void gameOver(int kind);
