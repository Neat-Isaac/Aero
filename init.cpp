#include "aero.h"

void readData()                         //读入文件设置各项数据
{
    int arguments[ARGS] = { 0 };
    std::ifstream fin;
    fin.open("config.in", std::ios::in);
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
    WCHART = arguments[16];
    CLF = arguments[17];
}

void init()     //各项数值的初始化，SFML各对象初始化没写在这里是因为不会
{
    readData();
    playership.direction = 0;
    visiting_planet = -1;
    srand(time(NULL)+clock());
    velocity = 1;
    playership.pos.x = 0;
    playership.pos.y = 0;
    playership.home = -1;
    playership.index = -1;
    playership.level = 3;
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
        t.has_aero = 0;
        t.direction = rand() % 2;
        t.pos = p;
        t.visited = 0;
        int lvl = rand() % 42;
        if (lvl >= 0 && lvl <= 11)
            t.level = 0;
        else if (lvl >= 12 && lvl <= 17)
            t.level = 1;
        else if (lvl >= 18 && lvl <= 27)
            t.level = 2;
        else if (lvl >= 28 && lvl <= 35)
            t.level = 3;
        else if (lvl >= 36 && lvl <= 39)
            t.level = 4;
        else
            t.level = 5;
        if (t.level)
            t.favour = rand() % 3;
        else
            t.favour = 1;
        t.left = t.level;
        t.name[0] = rand() % 26 + 'A';
        for (int j = 1; j < 5; j++)
            t.name[j] = rand() % 26 + 'a';
        for (int j = 0; j < t.level; j++)
        {
            t.product[j].name[0] = rand() % 26 + 'A';
            for (int k = 1; k < 5; k++)
                t.product[j].name[k] = rand() % 26 + 'a';
            t.product[j].num = 0;
        }
        pdata.push_back(t);
    }
    setCharts();
    for (int i = 0; i < N; i++)
    {
        std::vector<MetPros> templist;
        for (int j = 0; j < pdata[i].level; j++)
        {
            templist.push_back(pdata[i].product[j]);
            for (int k = 0; k < pdata[pdata[i].lines[j]].level; k++)
                templist.push_back(pdata[pdata[i].lines[j]].product[k]);
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

void setCharts()
{
    for (int i = 0; i < N; i++)         //生成星图
    {
        for (int j = 0; j < pdata[i].left; j++)
        {
            int mindis1 = calcDistance(R_BORDER, U_BORDER, L_BORDER, D_BORDER);
            int minpla1 = 0;
            bool isexist2 = 0;
            for (int k = 0; k < N; k++)
            {
                if (k == i || pdata[k].left == 0)
                    continue;
                bool isexist1 = 0;
                for (int q = 0; q < pdata[i].level; q++)
                {
                    if (pdata[i].lines[q] == k)
                    {
                        isexist1 = 1;
                        break;
                    }
                }
                if (isexist1)
                    continue;
                if (calcDistance(pdata[k].pos.x, pdata[k].pos.y, pdata[i].pos.x, pdata[i].pos.y) <= mindis1)
                {
                    isexist2 = 1;
                    mindis1 = calcDistance(pdata[k].pos.x, pdata[k].pos.y, pdata[i].pos.x, pdata[i].pos.y);
                    minpla1 = k;
                }
            }
            if (!isexist2)
            {
                pdata[i].left = 0;
                break;
            }
            pdata[minpla1].left--;
            pdata[minpla1].lines[pdata[minpla1].left] = i;
            pdata[i].lines[j] = minpla1;
        }
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < pdata[i].level; j++)
        {
            if (pdata[i].lines[j] < i)
                continue;
            Chart v;
            v.sp = i;
            v.start = pdata[i].pos;
            v.ep = pdata[i].lines[j];
            v.end = pdata[v.ep].pos;
            charts.push_back(v);
        }
    }
    /*for (int i = 0; i < charts.size(); i++)
    {
        if (i && charts[i].sp != charts[i - 1].sp)
            printf("\n");
        printf("%d-%d ", charts[i].sp, charts[i].ep);
    }*/
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
