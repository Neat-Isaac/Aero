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
int N, DELAY, SPEED, L_BORDER, R_BORDER, U_BORDER, D_BORDER,BG_R,BG_G,BG_B;    //�⼸��ԭ���ǳ�����2023.02.05�ĳ����ļ�����

double player_x, player_y;
int player_direction, player_angle;                             //player_direction���ƶ���ǰ����player_angle�ǳ���Ƕ�
int c_moving, c_shooting, c_rotate, c_slide;                    //��ʱ��
int velocity;
bool is_moving,is_shooting;
//std::vector<bool> planet_direction;     //�����������vector����Ϊ�������ܵ������С
//std::vector<int> planet_distance;
struct Point                            //���ʣ��ʾ���Vector2f������Ҳ������
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
void readData()                         //�����ļ����ø�������
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
void init()     //������ֵ�ĳ�ʼ����SFML�������ʼ��ûд����������Ϊ����
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
double radian(int x)    //�Ƕ�ת����
{
    return x * 3.1415 / 180;
}
Point calcPos(int distance,int p_angle)     //�����ƶ�����;��룬����Ŀ�ĵ�������ص������
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
void control(sf::Event event)           //���ƽ�ɫ�ƶ�
{
    switch (event.key.code)
    {
        case sf::Keyboard::W:       //W��S��ǰ���ƶ�
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
        case sf::Keyboard::A:       //A��D��������ת
            player_angle--;
            break;
        case sf::Keyboard::D:
            player_angle++;
            break;
        case sf::Keyboard::K:       //����
            is_shooting = 1;
            c_shooting = clock();
            break;
        default:
            break;
    }
    if (player_angle < 0)           //���ƽǶ���0~360֮��
        player_angle += 360;
    if (player_angle >= 360)
        player_angle -= 360;
}
void slide()                        //�ɿ���������ݻ�����ֹͣ
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
    sf::Texture idle, moving, fire, planet1, planet2, home;     //��Ԫ��ͼƬ
    sf::Font font;
    sf::Sprite player, home_planet;
    std::vector<sf::Sprite> planets;        //�洢�������vector
    sf::RenderWindow window(sf::VideoMode(800, 600), "Aero");
    sf::View camera(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    sf::Text status;        //�������������
    sf::CircleShape dot(10.f);      //������
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
    std::stringstream sstr;     //��Ļ��ʾ���ַ���
    home_planet.setTexture(home);
    home_planet.setOrigin(sf::Vector2f(64.f, 64.f));
    home_planet.setPosition(sf::Vector2f(0.f, 0.f));
    double min_distance = sqrt(R_BORDER > -L_BORDER ? R_BORDER * R_BORDER : L_BORDER * L_BORDER + U_BORDER > D_BORDER ? U_BORDER * U_BORDER : D_BORDER * D_BORDER);//��Զ����Ϊ��ͼ��Զ���㵽ԭ��
    int min_planet;
    for (int i = 0; i < N; i++)     //��ʼ��������
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
    while (window.isOpen())     //��ѭ��
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
        if (player_x < L_BORDER)        //���������ڱ߽���
            player_x = L_BORDER;
        if (player_x > R_BORDER)
            player_x = R_BORDER;
        if (player_y > U_BORDER)
            player_y = U_BORDER;
        if (player_y < D_BORDER)
            player_y = D_BORDER;
        if (clock() - c_shooting > DELAY)
            is_shooting = 0;
        if (is_moving)                  //�л�����ͼƬ
            player.setTexture(moving);
        else if (is_shooting)
            player.setTexture(fire);
        else
            player.setTexture(idle);
        //printf("x:%d y:%d moving count:%d shooting count:%d speed:%d\n", player_x, player_y, c_moving, c_shooting, velocity + SPEED);
        sstr.str("");
        int sx = player_x, sy = player_y;
        sstr << "X: " << sx << " Y: " << sy << std::endl << "SPEED: " << velocity;      //��Ļ�������
        //std::cout << sstr.str() << std::endl;
        status.setString(sstr.str());
        player.setPosition(sf::Vector2f(player_x, player_y));
        double sizex = window.getSize().x/2;
        double sizey = window.getSize().y/2;
        double s_dis = sqrt(sizex * sizex + sizey * sizey);
        //int s_dis = 500;
        double l = asin(sizey/s_dis) * 180 / acos(-1);                          //�����Ҳ��У������Ƿ�����
        int status_x = calcPos(s_dis-16, player_angle - 90+l).x;                //������Ļ������ݵ�λ�ã���Ϊ����������ת����ת
        int status_y = calcPos(s_dis-16, player_angle - 90+l).y;
        //status.setPosition(sf::Vector2f(player_x - window.getSize().x / 2, player_y - window.getSize().y / 2));
        status.setPosition(sf::Vector2f(player_x + status_x, player_y + status_y));
        dot.setPosition(status.getPosition());
        player.setRotation(player_angle * 1.0);
        camera.setCenter(player.getPosition());
        camera.setRotation(player.getRotation());
        status.setRotation(player.getRotation());
        window.setView(camera);
        //window.clear(sf::Color::Color(0, 0, 40));         //����ɫ�����ǿ������е㲻���
        //window.clear(sf::Color::Color(0, 0, 60));         //ǳ��һ�㣬���ǲ����ǿ�
        //window.clear(sf::Color::Color(0, 15, 43));          //����ɫ������һ����ɫ
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
        for (int i = 0; i < N; i++)     //��ת������
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