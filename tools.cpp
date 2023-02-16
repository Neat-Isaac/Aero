#include "aero.h"

void showMap()
{
    system("cls");
    for (int i = 0; i < (U_BORDER - D_BORDER) / SEP; i++)
    {
        for (int j = 0; j < (R_BORDER - L_BORDER) / SEP; j++)
        {
            //printf("%d %d\n", (player_x - L_BORDER) / SEP, (player_y - D_BORDER) / SEP);
            if (int((playership.pos.x - L_BORDER) / SEP) == j && int((playership.pos.y - D_BORDER) / SEP) == i)
                printf("O ");
            else if (pused[i][j])
                printf("* ");
            else
                printf(". ");
        }
        printf("\n");
    }
}
