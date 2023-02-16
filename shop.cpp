#include "aero.h"

void pick(MetPros thing)            //向背包中加入某物
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

bool lose(MetPros thing)        //从背包中取出某物
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

void buy(int number, bool flag1)            //购买物品
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
    if (lose(buying_thing))
        pick(get_thing);
    return;
}