#include "aero.h"

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
        playership.pos.x += calcPos(velocity, playership.direction).x;
        playership.pos.y += calcPos(velocity, playership.direction).y;
        break;
    case sf::Keyboard::S:
        player_direction = 1;
        if (clock() - c_moving <= DELAY && velocity < SPEED)
            velocity++;
        c_moving = clock();
        is_moving = 1;
        playership.pos.x += calcPos(-velocity, playership.direction).x;
        playership.pos.y += calcPos(-velocity, playership.direction).y;
        break;
    case sf::Keyboard::A:       //A，D：左右旋转
        playership.direction--;
        break;
    case sf::Keyboard::D:
        playership.direction++;
        break;
    case sf::Keyboard::K:       //开火
        is_shooting = 1;
        c_shooting = clock();
        fire(playership);
        break;
    case sf::Keyboard::F:       //显示菜单
        printf("%d:%d\n", visiting_planet, visiting_planet > -1 ? pdata[visiting_planet].favour : 0);
        
        if (visiting_planet >= 0)
        {
            if (pdata[visiting_planet].favour)
            {
                for (int q = 0; q < charts.size(); q++)
                {
                    if (charts[q].sp == visiting_planet || charts[q].ep == visiting_planet)     //如果该星图有任意端点为此星球则绘制
                        printf("%d-%d ", charts[q].sp, charts[q].ep);
                }
                pdata[visiting_planet].visited = !pdata[visiting_planet].visited;
            }
        }
        else if (visiting_planet == -1)
            visiting_home = !visiting_home;
        break;
    case sf::Keyboard::E:
        show_bag = !show_bag;
        break;
    case sf::Keyboard::J:
        pressed_J = 1;
        c_pj = clock();
        break;
    case sf::Keyboard::Num1:
        buy(1, 0);
        break;
    case sf::Keyboard::Num2:
        buy(1, 1);
        break;
    case sf::Keyboard::Num3:
        buy(2, 0);
        break;
    case sf::Keyboard::Num4:
        buy(2, 1);
        break;
    case sf::Keyboard::Num5:
        buy(3, 0);
        break;
    case sf::Keyboard::Num6:
        buy(3, 1);
        break;
    case sf::Keyboard::Num7:
        buy(4, 0);
        break;
    case sf::Keyboard::Num8:
        buy(4, 1);
        break;
    case sf::Keyboard::Num9:
        buy(5, 0);
        break;
    case sf::Keyboard::Num0:
        buy(5, 1);
        break;
    default:
        break;
    }
    if (playership.direction < 0)           //控制角度在0~360之间
        playership.direction += 360;
    if (playership.direction >= 360)
        playership.direction -= 360;
}

void slide()                        //松开按键后短暂滑行再停止
{
    if (clock() - c_moving > DELAY)
    {
        is_moving = 0;
        if (clock() - c_slide > DELAY)
        {
            velocity -= 4;
            if (velocity < 1)
            {
                velocity = 1;
                player_direction = -1;
            }
            c_slide = clock();
            playership.pos.x += calcPos((player_direction ? -1 : 1) * velocity, playership.direction).x;
            playership.pos.y += calcPos((player_direction ? -1 : 1) * velocity, playership.direction).y;
        }
    }
}
void fire(Aero ship)
{
    Bullet bul;
    bul.shooter = ship.index;
    bul.direction = ship.direction;
    bul.pos = ship.pos;
    bul.speed = ship.level * 4;
    bul.distance = ship.level * 256;
    bullets.push_back(bul);
}
void moveBullets()
{
    for (int i = 0; i < bullets.size(); i++)
    {
        bullets[i].pos.x += calcPos(bullets[i].speed, bullets[i].direction).x;
        bullets[i].pos.y += calcPos(bullets[i].speed, bullets[i].direction).y;
        bullets[i].distance -= bullets[i].speed;
    }
}
void newAero(int home)
{
    Aero tempship;
    tempship.home = home;
    tempship.pos = pdata[home].pos;
    tempship.direction = 0;
    tempship.level = pdata[home].level;
    tempship.speed = tempship.level * 0.1;
    tempship.shoot_timer = clock();
    tempship.index = aeroes.size();
    aeroes.push_back(tempship);
}
void moveAeroes()
{
    for (int i = 0; i < aeroes.size(); i++)
    {
        aeroes[i].direction = calcAngle(playership.pos.x - aeroes[i].pos.x, playership.pos.y - aeroes[i].pos.y)+0.5;
        //if (rand() % 2)
            //aeroes[i].direction--;
        //else
            //aeroes[i].direction++;
        ;if (aeroes[i].direction < 0)
            aeroes[i].direction += 360;
        if (aeroes[i].direction >= 360)
            aeroes[i].direction -= 360;
        aeroes[i].pos.x += calcPos(aeroes[i].speed, aeroes[i].direction).x;
        aeroes[i].pos.y += calcPos(aeroes[i].speed, aeroes[i].direction).y;
        if (clock() - aeroes[i].shoot_timer > (6 - aeroes[i].level) * 100)
        {
            aeroes[i].shoot_timer = clock();
            int exact = (6 - aeroes[i].level) * 5;
            aeroes[i].direction = calcAngle(playership.pos.x - aeroes[i].pos.x, playership.pos.y - aeroes[i].pos.y) + (rand()%(exact*2)+exact);
            fire(aeroes[i]);
            aeroes[i].direction = calcAngle(playership.pos.x - aeroes[i].pos.x, playership.pos.y - aeroes[i].pos.y) + 0.5;
        }
        printf("%d:Level:%d Pos:%f,%f\n", i, aeroes[i].level,aeroes[i].pos.x, aeroes[i].pos.y);
    }
}