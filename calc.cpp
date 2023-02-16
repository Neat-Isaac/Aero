#include "aero.h"

double radian(int x)    //�Ƕ�ת����
{
    return x * 3.1415 / 180;
}

float calcDistance(float x2, float y2, float x1, float y1)  //�������ؾ���
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

Point calcPos(float distance, float p_angle)     //�����ƶ�����;��룬����Ŀ�ĵ�������ص������
{
    if (p_angle < 0)
        p_angle += 360;
    if (p_angle >= 360)
        p_angle -= 360;
    float retx = 0, rety = 0;
    int angle = (int)p_angle % 90;
    int kind = p_angle / 90;
    float vel = distance;
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

float calcAngle(int x, int y)           //������������������ת�Ƕ�
{
    float ret;
    float z = sqrt(x * x + y * y);
    if (x > 0 && y > 0)
        ret = asin((float)y / z) * 180 / acos(-1);
    else if (x <= 0 && y > 0)
    {
        x = -x;
        ret = asin((float)x / z) * 180 / acos(-1);
        ret += 90;
    }
    else if (x < 0 && y <= 0)
    {
        x = -x;
        y = -y;
        ret = asin((float)y / z) * 180 / acos(-1);
        ret += 180;
    }
    else
    {
        y = -y;
        ret = asin((float)x / z) * 180 / acos(-1);
        ret += 270;
    }
    return ret;
}