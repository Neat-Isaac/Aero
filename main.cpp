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

#define ARGS 11
int N, DELAY, SPEED, L_BORDER, R_BORDER, U_BORDER, D_BORDER,BG_R,BG_G,BG_B;    //这几个原来是常量，2023.02.05改成了文件读入

double player_x, player_y;
int player_direction, player_angle;                             //player_direction是移动的前后方向，player_angle是朝向角度
int c_moving, c_shooting, c_rotate, c_slide;                    //计时器
int velocity;
bool is_moving,is_shooting;
//std::vector<bool> planet_direction;     //这里和下面用vector是因为变量不能当数组大小
//std::vector<int> planet_distance;
struct Point                            //别问，问就是Vector2f不会用也不敢用
{
    int x, y;
};
struct Planet
{
    bool direction;
    int distance;
    Point pos;
};
std::vector<Planet> pdata;
//std::vector<Point> planet_pos;
void readData()                         //读入文件设置各项数据
{
    int arguments[ARGS] = { 0 };
    std::ifstream fin;
    fin.open("config.in",std::ios::in);
    std::string data[ARGS];
    for (int i = 0; std::getline(fin, data[i]); i++)
    {
        bool flag = 0;
        bool symbol = 0;
        int count = 0;
        for (int j = 0; j < data[i].size(); j++)
        {
            if (flag)
            {
                count++;
                if (data[i][j] == '-')
                    symbol = 1;
                else
                    arguments[i] = arguments[i] * 10 + data[i][j] - '0';
            }
            if (data[i].c_str()[j] == '=')
                flag = 1;
        }
        if (symbol)
            arguments[i] *= -1;
    }
    N = arguments[0];
    DELAY = arguments[1];
    SPEED = arguments[2];
    L_BORDER = arguments[3];
    R_BORDER = arguments[4];
    U_BORDER = arguments[5];
    D_BORDER = arguments[6];
    BG_R = arguments[7];
    BG_G = arguments[8];
    BG_B = arguments[9];
}
void init()     //各项数值的初始化，SFML各对象初始化没写在这里是因为不会
{
    readData();
    player_angle = 0;
    srand(time(NULL));
    velocity = 0;
    player_x = 0;
    player_y = 0;
    is_moving = 0;
    is_shooting = 0;
    c_moving = 0;
    c_shooting = 0;
    player_direction = -1;
    c_rotate = 0;
    c_slide = 0;
}
double radian(int x)    //角度转弧度
{
    return x * 3.1415 / 180;
}
Point calcPos(int distance,int p_angle)     //给定移动方向和距离，计算目的地与出发地的坐标差
{
    int retx = 0, rety = 0;
    int angle = p_angle % 90;
    int kind = p_angle / 90;
    int vel = distance;
    switch (kind)
    {
        case 0:
            retx += sin(radian(angle)) * vel;
            rety -= cos(radian(angle)) * vel;
            break;
        case 1:
            retx += cos(radian(angle)) * vel;
            rety += sin(radian(angle)) * vel;
            break;
        case 2:
            retx -= sin(radian(angle)) * vel;
            rety += cos(radian(angle)) * vel;
            break;
        case 3:
            retx -= cos(radian(angle)) * vel;
            rety -= sin(radian(angle)) * vel;
            break;
    }
    Point ret;
    ret.x = retx;
    ret.y = rety;
    return ret;
}
void control(sf::Event event)           //控制角色移动
{
    switch (event.key.code)
    {
        case sf::Keyboard::W:       //W，S：前后移动
            player_direction = 0;
            if (clock() - c_moving <= DELAY && velocity < SPEED)
                velocity++;
            c_moving = clock();
            is_moving = 1;
            player_x += calcPos(velocity, player_angle).x;
            player_y += calcPos(velocity, player_angle).y;
            break;
        case sf::Keyboard::S:
            player_direction = 1;
            if (clock() - c_moving <= DELAY && velocity < SPEED)
                velocity++;
            c_moving = clock();
            is_moving = 1;
            player_x += calcPos(-velocity, player_angle).x;
            player_y += calcPos(-velocity, player_angle).y;
            break;
        case sf::Keyboard::A:       //A，D：左右旋转
            player_angle--;
            break;
        case sf::Keyboard::D:
            player_angle++;
            break;
        case sf::Keyboard::K:       //开火
            is_shooting = 1;
            c_shooting = clock();
            break;
        default:
            break;
    }
    if (player_angle < 0)           //控制角度在0~360之间
        player_angle += 360;
    if (player_angle >= 360)
        player_angle -= 360;
}
void slide()                        //松开按键后短暂滑行再停止
{
    if (clock() - c_moving > DELAY)
    {
        is_moving = 0;
        if (clock() - c_slide > DELAY)
        {
            velocity -= 4;
            if (velocity < 0)
            {
                velocity = 0;
                player_direction = -1;
            }
            c_slide = clock();
            player_x += calcPos((player_direction ? -1 : 1) * velocity, player_angle).x;
            player_y += calcPos((player_direction ? -1 : 1) * velocity, player_angle).y;
        }
    }
}
int main()
{
    init();
    sf::Texture idle, moving, fire, planet1, planet2, home;     //各元素图片
    sf::Font font;
    sf::Sprite player, home_planet;
    std::vector<sf::Sprite> planets;        //存储各星球的vector
    sf::RenderWindow window(sf::VideoMode(800, 600), "Aero");
    sf::View camera(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    sf::Text status;        //绘制坐标等数据
    sf::CircleShape dot(10.f);      //测试用
    dot.setFillColor(sf::Color::White);
    font.loadFromFile("src/font.ttf");
    idle.loadFromFile("src/idle64.png");
    moving.loadFromFile("src/moving64.png");
    fire.loadFromFile("src/fire64.png");
    planet1.loadFromFile("src/planet1.png");
    planet2.loadFromFile("src/planet2.png");
    home.loadFromFile("src/home.png");
    status.setFont(font);
    status.setFillColor(sf::Color(255, 255, 255));
    std::stringstream sstr;     //屏幕显示的字符串
    home_planet.setTexture(home);
    home_planet.setOrigin(sf::Vector2f(64.f, 64.f));
    home_planet.setPosition(sf::Vector2f(0.f, 0.f));
    double min_distance = sqrt(R_BORDER > -L_BORDER ? R_BORDER * R_BORDER : L_BORDER * L_BORDER + U_BORDER > D_BORDER ? U_BORDER * U_BORDER : D_BORDER * D_BORDER);//最远距离为地图最远顶点到原点
    int min_planet;
    for (int i = 0; i < N; i++)     //初始化各星球
    {
        sf::Sprite temp;
        temp.setOrigin(sf::Vector2f(64.f, 64.f));
        temp.setTexture(rand() % 2 ? planet1 : planet2);
        Point p;
        p.x = (rand() % (R_BORDER - L_BORDER)) + L_BORDER;
        p.y = (rand() % (U_BORDER - D_BORDER)) + D_BORDER;
        double dist = sqrt(p.x * p.x + p.y * p.y);
        if (dist < min_distance)
        {
            min_distance = dist;
            min_planet = i;
        }
        temp.setPosition(sf::Vector2f(p.x,p.y));
        temp.setColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
        temp.setRotation(rand() % 360);
        Planet t;
        t.direction = rand() % 2;
        t.pos = p;
        pdata.push_back(t);
        planets.push_back(temp);
    }
    for (int i = 0; i < N-1; i++)
    {
        int tempx = pdata[i].pos.x - pdata[i + 1].pos.x;
        int tempy = pdata[i].pos.y - pdata[i + 1].pos.y;
        int tempdis = sqrt(tempx * tempx + tempy * tempy);
        pdata[i].distance = tempdis;
    }
    player.setOrigin(sf::Vector2f(32.f, 32.f));
    player.setPosition(sf::Vector2f(400.f, 450.f));
    while (window.isOpen())     //主循环
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
                control(event);
            if (event.type == sf::Event::Resized)
                camera.setSize(event.size.width,event.size.height);
        }
        slide();
        if (player_x < L_BORDER)        //控制坐标在边界内
            player_x = L_BORDER;
        if (player_x > R_BORDER)
            player_x = R_BORDER;
        if (player_y > U_BORDER)
            player_y = U_BORDER;
        if (player_y < D_BORDER)
            player_y = D_BORDER;
        if (clock() - c_shooting > DELAY)
            is_shooting = 0;
        if (is_moving)                  //切换精灵图片
            player.setTexture(moving);
        else if (is_shooting)
            player.setTexture(fire);
        else
            player.setTexture(idle);
        //printf("x:%d y:%d moving count:%d shooting count:%d speed:%d\n", player_x, player_y, c_moving, c_shooting, velocity + SPEED);
        sstr.str("");
        int sx = player_x, sy = player_y;
        sstr << "X: " << sx << " Y: " << sy << std::endl << "SPEED: " << velocity;      //屏幕输出数据
        //std::cout << sstr.str() << std::endl;
        status.setString(sstr.str());
        player.setPosition(sf::Vector2f(player_x, player_y));
        double sizex = window.getSize().x/2;
        double sizey = window.getSize().y/2;
        double s_dis = sqrt(sizex * sizex + sizey * sizey);
        //int s_dis = 500;
        double l = asin(sizey/s_dis) * 180 / acos(-1);                          //反余弦不行，必须是反正弦
        int status_x = calcPos(s_dis-16, player_angle - 90+l).x;                //计算屏幕输出数据的位置，因为相机随玩家旋转而旋转
        int status_y = calcPos(s_dis-16, player_angle - 90+l).y;
        //status.setPosition(sf::Vector2f(player_x - window.getSize().x / 2, player_y - window.getSize().y / 2));
        status.setPosition(sf::Vector2f(player_x + status_x, player_y + status_y));
        dot.setPosition(status.getPosition());
        player.setRotation(player_angle * 1.0);
        camera.setCenter(player.getPosition());
        camera.setRotation(player.getRotation());
        status.setRotation(player.getRotation());
        window.setView(camera);
        //window.clear(sf::Color::Color(0, 0, 40));         //深蓝色，但是看起来有点不舒服
        //window.clear(sf::Color::Color(0, 0, 60));         //浅了一点，但是不像星空
        //window.clear(sf::Color::Color(0, 15, 43));          //深蓝色，混了一点绿色
        window.clear(sf::Color::Color(BG_R, BG_G, BG_B));
        bool can_rotate;
        if (clock() - c_rotate > DELAY)
        {
            c_rotate = clock();
            can_rotate = 1;
            home_planet.rotate(0.5);
        }
        else
            can_rotate = 0;
        for (int i = 0; i < N; i++)     //旋转各星球
        {
            if (can_rotate)
                planets[i].rotate(pdata[i].direction ? 0.5 : -0.5);
            window.draw(planets[i]);
        }
        window.draw(home_planet);
        window.draw(player);
        window.draw(status);
        //window.draw(dot);
        window.display();
    }
    return 0;
}