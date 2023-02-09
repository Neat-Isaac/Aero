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

#define ARGS 17
int N, DELAY, SPEED, L_BORDER, R_BORDER, U_BORDER, D_BORDER, SEP, SD, M;    //前几个原来是常量，2023.02.05改成了文件读入
int BG_R, BG_G, BG_B, CT_R, CT_G, CT_B;
double player_x, player_y;
int player_direction, player_angle, visiting_planet;             //player_direction是移动的前后方向，player_angle是朝向角度
int c_moving, c_shooting, c_rotate, c_slide;                    //计时器
int velocity;
bool is_moving, is_shooting, visiting_home,show_bag;
bool pused[1000][1000];
struct Point                            //别问，问就是Vector2f不会用也不敢用
{
    int x, y;
};
struct MetPros
{
    char name[5];
    int num;
};
std::vector<MetPros> bag;
struct Planet
{
    int level;
    char name[5];
    MetPros product[6];
    MetPros sell[1000][2];      //数组开小导致了一系列bug，解决留念2023.02.09
    int shopsize;
    bool visited;
    bool direction;
    int distance;
    Point pos;
};
std::vector<Planet> pdata;      //存储各星球数据的vector
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
    CT_R = arguments[10];
    CT_G = arguments[11];
    CT_B = arguments[12];
    SEP = arguments[13];
    SD = arguments[14];
    M = arguments[15];
    
}
bool isSpaceEmpty(int x, int y)                             //检查区块是否没有星球
{
    int px = (x - L_BORDER) / SEP;
    int py = (y - D_BORDER) / SEP;
    for (int i = px - 1; i < px + 1; i++)
    {
        for (int j = py - 1; j < py + 1; j++)
        {
            if (pused[i][j])
                return 0;
        }
    }
    pused[px][py] = 1;
    return 1;
}
void init()     //各项数值的初始化，SFML各对象初始化没写在这里是因为不会
{
    readData();
    player_angle = 0;
    visiting_planet = -1;
    srand(time(NULL));
    velocity = 0;
    player_x = 0;
    player_y = 0;
    is_moving = 0;
    is_shooting = 0;
    show_bag = 0;
    c_moving = 0;
    c_shooting = 0;
    player_direction = -1;
    c_rotate = 0;
    c_slide = 0;
    pused[-L_BORDER / SEP][-D_BORDER] = 1;
    for (int i = 0; i < N; i++)
    {
        Point p;
        p.x = 0; p.y = 0;
        while ((!p.x && !p.y) || !isSpaceEmpty(p.x, p.y))
        {
            p.x = (rand() % (R_BORDER - L_BORDER)) + L_BORDER;
            p.y = (rand() % (U_BORDER - D_BORDER)) + D_BORDER;
        }
        Planet t;
        t.direction = rand() % 2;
        t.pos = p;
        t.visited = 0;
        t.level = rand() % 6;
        t.name[0] = rand() % 26 + 'A';
        for (int i = 1; i < 5; i++)
            t.name[i] = rand() % 26 + 'a';
        for (int i = 0; i < t.level; i++)
        {
            t.product[i].name[0] = rand() % 26 + 'A';
            for (int j = 1; j < 5; j++)
                t.product[i].name[j] = rand() % 26 + 'a';
            t.product[i].num = 0;
        }
        pdata.push_back(t);
    }
    for (int i = 0; i < N; i++)
    {
        std::vector<MetPros> templist;
        for (int j = ((i-pdata[i].level) >0 ? i-pdata[i].level:0);j<((i + pdata[i].level)<N?i+pdata[i].level:N-1); j++)
        {
            if (pdata[j].level == 0)
                continue;
            for (int k = 0; k < pdata[j].level; k++)
                templist.push_back(pdata[j].product[k]);
        }
        pdata[i].shopsize = templist.size() / 2 < pdata[i].level ? templist.size() / 2 : pdata[i].level;
        if (templist.size() < 2)
            continue;
        for (int j = 0; j < templist.size() / 2; j++)
        {
            pdata[i].sell[j][0] = templist[j * 2];
            pdata[i].sell[j][0].num = rand() % 5 + 1;
            pdata[i].sell[j][1] = templist[j * 2 + 1];
            pdata[i].sell[j][1].num = rand() % 5 + 1;/*
            printf("%d ", pdata[i].sell[j][0].num);
            for (int x = 0; x < 5; x++)
                printf("%c", pdata[i].sell[j][0].name[x]);
            printf(" = %d ", pdata[i].sell[j][0].num);
            for (int x = 0; x < 5; x++)
                printf("%c", pdata[i].sell[j][0].name[x]);
            printf("\n");*/
        }
    }
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
float calcAngle(int x, int y)           //给定两地坐标差，计算旋转角度
{
    float ret;
    float z = sqrt(x * x + y * y);
    if (x > 0 && y > 0)
    {
        //printf("1");
        ret = asin((float)y / z) * 180 / acos(-1);
    }
    else if (x <= 0 && y > 0)
    {
        x = -x;
        ret = asin((float)x / z) * 180 / acos(-1);
        ret += 90;
        //printf("2");
    }
    else if (x < 0 && y <= 0)
    {
        x = -x;
        y = -y;
        //printf("3");
        ret = asin((float)y / z) * 180 / acos(-1);
        ret += 180;
    }
    else
    {
        //printf("4");
        y = -y;
        ret = asin((float)x / z) * 180 / acos(-1);
        ret += 270;
    }
    //printf("\n%f", ret);
    return ret;
}
float calcDistance(float x2, float y2, float x1, float y1)  //计算两地距离
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}
void pick(MetPros thing)
{
    bool flagtemp1 = 0;
    for (int j = 0; j < bag.size(); j++)
    {
        bool tempname1 = 0;
        for (int k = 0; k < 5; k++)
        {
            if (bag[j].name[k] != thing.name[k])
            {
                tempname1 = 1;
                break;
            }
        }
        if (!tempname1)
        {
            flagtemp1 = 1;
            bag[j].num += thing.num;;
            break;
        }
    }
    if (!flagtemp1)
        bag.push_back(thing);
}
bool lose(MetPros thing)
{
    bool falg1 = 0;
    for (int j = 0; j < bag.size(); j++)
    {
        bool tempname1 = 0;
        for (int k = 0; k < 5; k++)
        {
            if (bag[j].name[k] != thing.name[k])
            {
                tempname1 = 1;
                break;
            }
        }
        if (!tempname1)
        {
            falg1 = 1;
            if (bag[j].num >= thing.num)
            {
                bag[j].num -= thing.num;
                if (bag[j].num == 0)
                    bag.erase(bag.begin() + j);
                return 1;
            }
            else
                return 0;
        }
    }
    if (!falg1)
        return 0;
}
void buy(int number,bool flag1)
{
    if (pdata[visiting_planet].shopsize < number)
        return;
    bool is_thing_exist = 0;
    MetPros buying_thing, get_thing;
    int bag_index = 0;
    for (int i = 0; i < bag.size(); i++)
    {
        bool comparename = 0;
        for (int j = 0; j < 5; j++)
        {
            if (bag[i].name[j] != pdata[visiting_planet].sell[number - 1][flag1].name[j])
                comparename = 1;
        }
        if (!comparename)
        {
            is_thing_exist = 1;
            bag_index = i;
            buying_thing = pdata[visiting_planet].sell[number - 1][flag1];
            get_thing = pdata[visiting_planet].sell[number - 1][!flag1];
        }
        else
            continue;
    }
    if (!is_thing_exist)
        return;
    if(lose(buying_thing))
        pick(get_thing);
    return;
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
        case sf::Keyboard::F:       //显示菜单
            //printf("%d\n", pdata[visiting_planet].shopsize);
            if (visiting_planet >= 0)
                pdata[visiting_planet].visited = !pdata[visiting_planet].visited;
            else if (visiting_planet == -1)
                visiting_home = !visiting_home;
            break;
        case sf::Keyboard::E:
            show_bag = !show_bag;
            break;
        case sf::Keyboard::Num1:
            buy(1,0);
            break;
        case sf::Keyboard::Num2:
            buy(1,1);
            break;
        case sf::Keyboard::Num3:
            buy(2,0);
            break;
        case sf::Keyboard::Num4:
            buy(2,1);
            break;
        case sf::Keyboard::Num5:
            buy(3,0);
            break;
        case sf::Keyboard::Num6:
            buy(3,1);
            break;
        case sf::Keyboard::Num7:
            buy(4,0);
            break;
        case sf::Keyboard::Num8:
            buy(4,1);
            break;
        case sf::Keyboard::Num9:
            buy(5,0);
            break;
        case sf::Keyboard::Num0:
            buy(5,1);
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
    sf::Texture idle, moving, fire, planet1, planet2, home,meteor;     //各元素图片
    sf::Font font;
    sf::Sprite player, home_planet;
    std::vector<sf::Sprite> planets;                            //存储各星球精灵图的vector
    std::vector<sf::Sprite> meteors;
    std::vector<sf::RectangleShape> chart;
    std::vector<sf::FloatRect> collision;
    std::vector<sf::FloatRect> met_col;
    std::vector<MetPros> metpro;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Aero");
    sf::View camera(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    sf::Text status;        //绘制坐标等数据
    sf::CircleShape dot(10.f);      //测试用
    player.setRotation(0);
    dot.setFillColor(sf::Color::White);
    font.loadFromFile("src/font.ttf");
    idle.loadFromFile("src/idle64.png");
    moving.loadFromFile("src/moving64.png");
    fire.loadFromFile("src/fire64.png");
    planet1.loadFromFile("src/planet1.png");
    planet2.loadFromFile("src/planet2.png");
    home.loadFromFile("src/home.png");
    meteor.loadFromFile("src/meteor.png");
    status.setFont(font);
    status.setFillColor(sf::Color(255, 255, 255));
    std::stringstream sstr;     //屏幕显示的字符串
    home_planet.setTexture(home);
    home_planet.setOrigin(sf::Vector2f(64.f, 64.f));
    home_planet.setPosition(sf::Vector2f(0.f, 0.f));
    int r_l = R_BORDER - L_BORDER;
    int u_d = U_BORDER - D_BORDER;
    double min_distance = calcDistance(R_BORDER,U_BORDER,L_BORDER,D_BORDER);//最远距离为地图最远两个顶点间距离
    int min_planet = 0;
    for (int i = 0; i < N; i++)     //初始化各星球
    {
        sf::Sprite temp;
        temp.setOrigin(sf::Vector2f(64.f, 64.f));
        temp.setTexture(rand() % 2 ? planet1 : planet2);
        double dist = calcDistance(pdata[i].pos.x, pdata[i].pos.y, 0, 0);
        if (dist < min_distance)
        {
            min_distance = dist;
            min_planet = i;
        }
        temp.setPosition(sf::Vector2f(pdata[i].pos.x, pdata[i].pos.y));
        temp.setColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
        temp.setRotation(rand() % 360);
        
        planets.push_back(temp);
        collision.push_back(planets[i].getGlobalBounds());
    }
    for (int i = 0; i < M; i++)     //初始化陨石
    {
        sf::Sprite temp;
        temp.setOrigin(sf::Vector2f(48.f, 48.f));
        temp.setTexture(meteor);
        float mx, my;
        mx = (rand() % (R_BORDER - L_BORDER)) + L_BORDER;
        my = (rand() % (U_BORDER - D_BORDER)) + D_BORDER;
        temp.setPosition(sf::Vector2f(mx, my));
        float min_dis = calcDistance(R_BORDER - L_BORDER, U_BORDER - D_BORDER, 0, 0);
        int min_pnet = 0;
        for (int j = 0; j < N; j++)
        {
            if (calcDistance(pdata[j].pos.x, pdata[j].pos.y, mx, my) < min_dis&&pdata[j].level>0)
            {
                min_dis = calcDistance(pdata[j].pos.x, pdata[j].pos.y, mx, my);
                min_pnet = j;
            }
        }
        int pro = rand() % (pdata[min_pnet].level);
        MetPros tx;
        for (int j = 0; j < 5; j++)
            tx.name[j] = pdata[min_pnet].product[pro].name[j];
        tx.num = rand() % 3 + 1;
        met_col.push_back(temp.getGlobalBounds());
        meteors.push_back(temp);
        metpro.push_back(tx);
    }
    sf::RectangleShape homechart(sf::Vector2f(min_distance, 8.f));
    homechart.setFillColor(sf::Color::Color(CT_R, CT_G, CT_B));
    homechart.setRotation(calcAngle(pdata[min_planet].pos.x, pdata[min_planet].pos.y));
    homechart.setPosition(sf::Vector2f(0, 0));
    for (int i = 0; i < N-1; i++)               //初始化星图
    {
        int tempx = pdata[i].pos.x - pdata[i + 1].pos.x;
        int tempy = pdata[i].pos.y - pdata[i + 1].pos.y;
        int tempdis = calcDistance(tempx,tempy,0,0);
        pdata[i].distance = tempdis;
        sf::RectangleShape templine(sf::Vector2f(tempdis,8.f));
        templine.setFillColor(sf::Color::Color(CT_R, CT_G, CT_B));
        templine.setRotation(calcAngle(-tempx, -tempy));
        //printf("%f\n", templine.getRotation());
        templine.setPosition(planets[i].getPosition());
        chart.push_back(templine);
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
        sf::FloatRect player_collision = player.getGlobalBounds();
        for (int i = 0; i < M; i++)
        {
            if (player_collision.intersects(met_col[i]))
            {
                pick(metpro[i]);
                met_col.erase(met_col.begin() + i);
                meteors.erase(meteors.begin() + i);
                metpro.erase(metpro.begin() + i);
            }
        }
        bool flag = 0;
        for (int i = 0; i < N; i++)     //判断是否绘制星图
        {
            if (calcDistance(pdata[i].pos.x, pdata[i].pos.y, player_x, player_y) <= SD)
            {
                flag = 1;
                visiting_planet = i;
            }
        }
        bool draw_home = calcDistance(player_x, player_y, 0, 0) <= SD;
        if (flag == 0)
        {
            if (draw_home)
                visiting_planet = -1;
            else
                visiting_planet = -2;
        }
        //printf("x:%d y:%d moving count:%d shooting count:%d speed:%d\n", player_x, player_y, c_moving, c_shooting, velocity + SPEED);
        sstr.str("");
        int sx = player_x, sy = player_y;
        sstr << "X: " << sx << " Y: " << sy << std::endl << "Speed: " << velocity<<std::endl;      //屏幕输出数据
        if (show_bag)
        {
            sstr << "Bag:" << std::endl;
            for (int i = 0; i < bag.size(); i++)
            {
                for (int j = 0; j < 5; j++)
                    sstr << bag[i].name[j];
                sstr << ' ' << bag[i].num;
                sstr << std::endl;
            }
        }
        //std::cout << sstr.str() << std::endl;
        sf::Text pinfo;
        if (visiting_planet >= 0)               //输出星球菜单
        {
            pinfo.setFont(font);
            pinfo.setFillColor(sf::Color::White);
            pinfo.setPosition(planets[visiting_planet].getPosition());
            pinfo.setRotation(player.getRotation());
            std::stringstream pstr;
            pstr << "Name: ";
            for (int i = 0; i < 5; i++)
                pstr << pdata[visiting_planet].name[i];
            pstr << std::endl << "Level: " << pdata[visiting_planet].level << std::endl;
            if (pdata[visiting_planet].level)
            {
                pstr << "Product: " << std::endl;
                for (int i = 0; i < pdata[visiting_planet].level; i++)
                {
                    for (int j = 0; j < 5; j++)
                        pstr << pdata[visiting_planet].product[i].name[j];
                    pstr << ' ';
                }
            }
            if(pdata[visiting_planet].shopsize)
            {
                pstr << std::endl << "Shop:" << std::endl;
                for (int i = 0; i < pdata[visiting_planet].shopsize; i++)
                {
                    pstr << (i*2 + 1)%10 << ": ";
                    pstr << pdata[visiting_planet].sell[i][0].num<<" ";
                    for (int j = 0; j < 5; j++)
                        pstr << pdata[visiting_planet].sell[i][0].name[j];
                    pstr << " = " << pdata[visiting_planet].sell[i][1].num << " ";
                    for (int j = 0; j < 5; j++)
                        pstr << pdata[visiting_planet].sell[i][1].name[j];
                    pstr << std::endl;
                    pstr << (i*2 + 2)%10 << ": ";
                    pstr << pdata[visiting_planet].sell[i][1].num << " ";
                    for (int j = 0; j < 5; j++)
                        pstr << pdata[visiting_planet].sell[i][1].name[j];
                    pstr << " = " << pdata[visiting_planet].sell[i][0].num << " ";
                    for (int j = 0; j < 5; j++)
                        pstr << pdata[visiting_planet].sell[i][0].name[j];
                    pstr << std::endl;
                }
            }
            pinfo.setString(pstr.str());
        }
        status.setString(sstr.str());
        player.setPosition(sf::Vector2f(player_x, player_y));
        double sizex = (double)window.getSize().x/2;
        double sizey = (double)window.getSize().y/2;
        double s_dis = calcDistance(sizex,sizey,0,0);
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
        for (int i = 0; i < N - 1; i++)
        {
            if (pdata[i].visited)
                window.draw(chart[i]);
        }
        if (visiting_home)
            window.draw(homechart);
        for (int i = 0; i < N; i++)     //旋转各星球
        {
            if (can_rotate)
                planets[i].rotate(pdata[i].direction ? 0.5 : -0.5);
            window.draw(planets[i]);
        }
        for (int i = 0; i < meteors.size(); i++)
            window.draw(meteors[i]);
        window.draw(home_planet);
        window.draw(player);
        if (visiting_planet >= 0 && pdata[visiting_planet].visited)
            window.draw(pinfo);
        window.draw(status);
        //window.draw(dot);
        window.display();
    }
    return 0;
}
