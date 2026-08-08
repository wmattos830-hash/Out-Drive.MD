#include <genesis.h>

int main(void)
{
    VDP_setScreenWidth320();

    VDP_drawText("OUT DRIVE", 13, 8);
    VDP_drawText("BUILD OK!", 14, 12);
    VDP_drawText("v0.1 - teste", 12, 16);

    while (1)
    {
        VDP_waitVSync();
    }

    return 0;
}
