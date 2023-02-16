#include "aero.h"

int N, DELAY, SPEED, L_BORDER, R_BORDER, U_BORDER, D_BORDER, SEP, SD, M, WCHART, CLF;    //前几个原来是常量，2023.02.05改成了文件读入
int BG_R, BG_G, BG_B, CT_R, CT_G, CT_B;
int player_direction, visiting_planet;             //player_direction是移动的前后方向，player_angle是朝向角度
int c_moving, c_shooting, c_rotate, c_slide,c_pj;                    //计时器
int velocity;
bool is_moving, is_shooting, visiting_home, show_bag, pressed_J;
bool pused[1000][1000];

std::vector<MetPros> bag;
std::vector<Chart> charts;
std::vector<Planet> pdata;      //存储各星球数据的vector
std::vector<Aero> aeroes;
std::vector<Bullet> bullets;
Aero playership;

void game()
{
    init();
    sf::Texture idle, moving, fire, planet1, planet2, home, meteor, guipng,bulpng;     //各元素图片
    sf::Font font;
    sf::Sprite player, home_planet, guide;
    std::vector<sf::Sprite> planets;                            //存储各星球精灵图的vector
    std::vector<sf::Sprite> meteors;
    std::vector<sf::Sprite> aeroshapes;
    std::vector<sf::Sprite> bulshapes;
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
    idle.loadFromFile("src/aeroes.png");
    moving.loadFromFile("src/moving.png");
    fire.loadFromFile("src/fire.png");
    planet1.loadFromFile("src/planet1.png");
    planet2.loadFromFile("src/planet2.png");
    home.loadFromFile("src/home.png");
    meteor.loadFromFile("src/meteor.png");
    guipng.loadFromFile("src/guide.png");
    bulpng.loadFromFile("src/bullet.png");
    status.setFont(font);
    status.setFillColor(sf::Color(255, 255, 255));
    std::stringstream sstr;     //屏幕显示的字符串
    home_planet.setTexture(home);
    home_planet.setOrigin(sf::Vector2f(64.f, 64.f));
    home_planet.setPosition(sf::Vector2f(0.f, 0.f));
    guide.setTexture(guipng);
    guide.setOrigin(sf::Vector2f(64.f, 64.f));
    int r_l = R_BORDER - L_BORDER;
    int u_d = U_BORDER - D_BORDER;
    double min_distance = calcDistance(R_BORDER, U_BORDER, L_BORDER, D_BORDER);//最远距离为地图最远两个顶点间距离
    int min_planet = 0;
    for (int i = 0; i < N; i++)     //初始化各星球
    {
        sf::Sprite temp;
        temp.setOrigin(sf::Vector2f(64.f, 64.f));
        temp.setTexture(rand() % 2 ? planet1 : planet2);
        double dist = calcDistance(pdata[i].pos.x, pdata[i].pos.y, 0, 0);
        if (dist < min_distance && pdata[i].level)
        {
            min_distance = dist;
            min_planet = i;
        }
        temp.setPosition(sf::Vector2f(pdata[i].pos.x, pdata[i].pos.y));
        if (CLF)
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
            if (calcDistance(pdata[j].pos.x, pdata[j].pos.y, mx, my) < min_dis && pdata[j].level > 0)
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
    sf::RectangleShape homechart(sf::Vector2f(min_distance, WCHART));
    homechart.setFillColor(sf::Color::Color(CT_R, CT_G, CT_B));
    homechart.setRotation(calcAngle(pdata[min_planet].pos.x, pdata[min_planet].pos.y));
    homechart.setPosition(sf::Vector2f(0, 0));
    for (int i = 0; i < charts.size(); i++)               //初始化星图
    {
        int tempdis = calcDistance(charts[i].start.x, charts[i].start.y, charts[i].end.x, charts[i].end.y);
        pdata[i].distance = tempdis;
        sf::RectangleShape templine(sf::Vector2f(tempdis, WCHART));
        templine.setFillColor(sf::Color::Color(CT_R, CT_G, CT_B));
        templine.setRotation(calcAngle(charts[i].end.x - charts[i].start.x, charts[i].end.y - charts[i].start.y));
        //printf("%f\n", templine.getRotation());
        templine.setPosition(sf::Vector2f(charts[i].start.x, charts[i].start.y));
        chart.push_back(templine);
    }
    player.setOrigin(sf::Vector2f(16.f, 16.f));
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
                camera.setSize(event.size.width, event.size.height);
        }
        if (clock() - c_pj > DELAY)
            pressed_J = 0;
        if (velocity > 1)
            slide();
        if (playership.pos.x < L_BORDER)        //控制坐标在边界内
            playership.pos.x = L_BORDER;
        if (playership.pos.x > R_BORDER)
            playership.pos.x = R_BORDER;
        if (playership.pos.y > U_BORDER)
            playership.pos.y = U_BORDER;
        if (playership.pos.y < D_BORDER)
            playership.pos.y = D_BORDER;
        if (clock() - c_shooting > DELAY)
            is_shooting = 0;
        if (is_moving)                  //切换精灵图片
            player.setTexture(moving);
        else if (is_shooting)
            player.setTexture(fire);
        else
            player.setTexture(idle);
        if (bullets.size()>bulshapes.size())
        {
            sf::Sprite tempbul;
            tempbul.setTexture(bulpng);
            tempbul.setOrigin(sf::Vector2f(2.f, 4.f));
            bulshapes.push_back(tempbul);
        }
        if (aeroes.size() > aeroshapes.size())
        {
            sf::Sprite tempaero;
            tempaero.setTexture(idle);
            tempaero.setColor(planets[aeroes[aeroshapes.size()].home].getColor());
            tempaero.setOrigin(sf::Vector2f(16.f, 16.f));
            aeroshapes.push_back(tempaero);
        }
        moveBullets();
        moveAeroes();
        sf::FloatRect player_collision = player.getGlobalBounds();
        for (int i = 0; i < aeroshapes.size(); i++)
        {
            aeroshapes[i].setPosition(sf::Vector2f(aeroes[i].pos.x, aeroes[i].pos.y));
            aeroshapes[i].setRotation(aeroes[i].direction);
            if (player_collision.intersects(aeroshapes[i].getGlobalBounds()))
            {
                gameOver(2);
                return;
            }
            for (int j = 0; j < aeroshapes.size(); j++)
            {
                if (i == j)
                    continue;
                if (aeroshapes[i].getGlobalBounds().intersects(aeroshapes[j].getGlobalBounds()))
                {
                    pdata[aeroes[i].home].favour = 1;
                    pdata[aeroes[j].home].favour = 1;
                    if (i < j)
                    {
                        aeroshapes.erase(aeroshapes.begin() + j);
                        aeroes.erase(aeroes.begin() + j);
                        aeroshapes.erase(aeroshapes.begin() + i);
                        aeroes.erase(aeroes.begin() + i);
                    }
                    else
                    {
                        aeroshapes.erase(aeroshapes.begin() + i);
                        aeroes.erase(aeroes.begin() + i);
                        aeroshapes.erase(aeroshapes.begin() + j);
                        aeroes.erase(aeroes.begin() + j);
                    }
                    break;
                }
            }
        }
        for (int i = 0; i < bulshapes.size(); i++)
        {
            bulshapes[i].setPosition(sf::Vector2f(bullets[i].pos.x, bullets[i].pos.y));
            bulshapes[i].setRotation(bullets[i].direction);
            if (bullets[i].distance <= 0)
            {
                bullets.erase(bullets.begin() + i);
                bulshapes.erase(bulshapes.begin() + i);
            }
            if (player_collision.intersects(bulshapes[i].getGlobalBounds()) && bullets[i].shooter != -1)
            {
                gameOver(1);
                return;
            }
            for (int j = 0; j < aeroshapes.size(); j++)
            {
                if (j == bullets[i].shooter)
                    continue;
                if (aeroshapes[j].getGlobalBounds().intersects(bulshapes[i].getGlobalBounds()))
                {
                    pdata[aeroes[j].home].favour = 1;
                    aeroshapes.erase(aeroshapes.begin() + j);
                    aeroes.erase(aeroes.begin() + j);
                    bulshapes.erase(bulshapes.begin() + i);
                    bullets.erase(bullets.begin() + i);
                    break;
                }
            }
        }
        if (pressed_J)
        {
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
        }
        bool flag = 0;
        for (int i = 0; i < N; i++)     //判断是否绘制星图
        {
            if (player.getGlobalBounds().intersects(planets[i].getGlobalBounds()))
            {
                flag = 1;
                visiting_planet = i;
            }
            if (!pdata[i].favour && calcDistance(playership.pos.x, playership.pos.y, pdata[i].pos.x, pdata[i].pos.y) <= SD)
            {
                if (!pdata[i].level)
                    continue;
                if (!pdata[i].has_aero)
                {
                    pdata[i].has_aero = 1;
                    newAero(i);
                }
            }
        }
        bool draw_home = player.getGlobalBounds().intersects(home_planet.getGlobalBounds());
        if (flag == 0)
        {
            if (draw_home)
                visiting_planet = -1;
            else
                visiting_planet = -2;
        }
        //printf("x:%d y:%d moving count:%d shooting count:%d speed:%d\n", player_x, player_y, c_moving, c_shooting, velocity + SPEED);
        sstr.str("");
        int sx = playership.pos.x, sy = playership.pos.y;
        sstr << "X: " << sx << " Y: " << sy << std::endl << "Speed: " << velocity << std::endl;      //屏幕输出数据
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
            if (pdata[visiting_planet].shopsize)
            {
                pstr << std::endl << "Shop:" << std::endl;
                for (int i = 0; i < pdata[visiting_planet].shopsize; i++)
                {
                    pstr << (i * 2 + 1) % 10 << ": ";
                    pstr << pdata[visiting_planet].sell[i][0].num << " ";
                    for (int j = 0; j < 5; j++)
                        pstr << pdata[visiting_planet].sell[i][0].name[j];
                    pstr << " = " << pdata[visiting_planet].sell[i][1].num << " ";
                    for (int j = 0; j < 5; j++)
                        pstr << pdata[visiting_planet].sell[i][1].name[j];
                    pstr << std::endl;
                    pstr << (i * 2 + 2) % 10 << ": ";
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
        player.setPosition(sf::Vector2f(playership.pos.x, playership.pos.y));
        float sizex = (float)window.getSize().x / 2;
        float sizey = (float)window.getSize().y / 2;
        float s_dis = calcDistance(sizex, sizey, 0, 0);
        //int s_dis = 500;
        double l = asin(sizey / s_dis) * 180 / acos(-1);                          //反余弦不行，必须是反正弦
        float status_x = calcPos(s_dis - 16, playership.direction - 90 + l).x;                //计算屏幕输出数据的位置，因为相机随玩家旋转而旋转
        float status_y = calcPos(s_dis - 16, playership.direction - 90 + l).y;
        //status.setPosition(sf::Vector2f(player_x - window.getSize().x / 2, player_y - window.getSize().y / 2));
        status.setPosition(sf::Vector2f(playership.pos.x + status_x, playership.pos.y + status_y));
        float guide_x = calcPos(s_dis - 192, playership.direction + 90 - l).x;                //计算屏幕输出数据的位置，因为相机随玩家旋转而旋转
        float guide_y = calcPos(s_dis - 192, playership.direction + 90 - l).y;
        guide.setPosition(sf::Vector2f(playership.pos.x + guide_x, playership.pos.y + guide_y));
        dot.setPosition(guide.getPosition());
        player.setRotation(playership.direction * 1.0);
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
            {
                for (int q = 0; q < chart.size(); q++)
                {
                    if (charts[q].sp == i || charts[q].ep == i)     //如果该星图有任意端点为此星球则绘制
                        window.draw(chart[q]);
                }
            }
        }
        for (int i = 0; i < bulshapes.size(); i++)
            window.draw(bulshapes[i]);
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
        for (int i = 0; i < aeroshapes.size(); i++)
            window.draw(aeroshapes[i]);
        window.draw(player);
        if (visiting_planet >= 0 && pdata[visiting_planet].visited)
            window.draw(pinfo);
        window.draw(status);
        window.draw(guide);
        //window.draw(dot);
        window.display();
    }
}
void gameOver(int kind)
{
    if (kind == 1)
        printf("Bullet hit.\n");
    else if (kind == 2)
        printf("Aero hit.\n");
    printf("Game Over.\n");
    Sleep(1000);
    return;
}
int main()
{
    game();
    return 0;
}
