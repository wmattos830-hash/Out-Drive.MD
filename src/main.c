/*
 * OUT DRIVE - Mega Drive Port
 * v0.6 - controle manual + direcao responsiva
 */
#include <genesis.h>

#define VPC(r,g,b) (u16)((((b)>>5)<<9)|(((g)>>5)<<5)|(((r)>>5)<<1))

static const u16 pal0[16] = {
    VPC(0,0,0), VPC(224,32,32), VPC(128,16,16), VPC(40,60,120),
    VPC(10,10,10), VPC(255,208,0)
};
static const u16 pal1[16] = {
    VPC(0,0,0), VPC(16,160,64), VPC(0,120,48), VPC(160,160,160),
    VPC(130,130,130), VPC(224,32,32), VPC(240,240,240), VPC(80,160,240), VPC(40,100,200)
};

#define T8(V) (V),(V),(V),(V),(V),(V),(V),(V)
static const u32 roadTiles[8*8] = {
    T8(0x11111111), T8(0x22222222), T8(0x33333333), T8(0x44444444),
    T8(0x55555555), T8(0x66666666), T8(0x77777777), T8(0x88888888)
};

#define R8(a,b,c,d,e,f,g,h) (((a)<<28)|((b)<<24)|((c)<<20)|((d)<<16)|((e)<<12)|((f)<<8)|((g)<<4)|(h))
static const u32 carTiles[8*8] = {
    R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,2), R8(0,0,0,0,0,0,0,2), R8(0,0,0,0,0,0,0,2),
    R8(0,0,1,1,1,1,1,1), R8(0,0,1,1,1,1,1,1), R8(0,0,5,5,1,1,1,1), R8(0,0,1,1,1,1,1,1),
    R8(2,2,2,2,2,2,2,2), R8(3,3,3,3,3,3,3,3), R8(3,3,3,3,3,3,3,3), R8(2,2,2,2,2,2,2,2),
    R8(1,1,1,1,1,1,1,1), R8(1,1,1,1,1,1,1,1), R8(1,1,1,1,1,1,1,1), R8(1,1,1,1,1,1,1,1),
    R8(2,2,2,2,2,2,2,2), R8(3,3,3,3,3,3,3,3), R8(3,3,3,3,3,3,3,3), R8(2,2,2,2,2,2,2,2),
    R8(1,1,1,1,1,1,1,1), R8(1,1,1,1,1,1,1,1), R8(1,1,1,1,1,1,1,1), R8(1,1,1,1,1,1,1,1),
    R8(0,0,0,0,0,0,0,0), R8(2,0,0,0,0,0,0,0), R8(2,0,0,0,0,0,0,0), R8(2,0,0,0,0,0,0,0),
    R8(1,1,1,1,1,1,0,0), R8(1,1,1,1,1,1,0,0), R8(1,1,1,1,5,5,0,0), R8(1,1,1,1,1,1,0,0),
    R8(4,4,4,4,0,0,2,2), R8(4,4,4,4,0,0,2,2), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    R8(2,2,2,2,2,2,2,2), R8(2,2,2,2,2,2,2,2), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    R8(2,2,2,2,2,2,2,2), R8(2,2,2,2,2,2,2,2), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    R8(2,2,0,0,4,4,4,4), R8(2,2,0,0,4,4,4,4), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0)
};

#define TA(pal,tile) TILE_ATTR_FULL((pal), FALSE, FALSE, FALSE, (tile))
#define HORIZON 10

static s16 trackCurve(s32 seg)
{
    static const s16 tab[8] = {0, 2, 0, -3, 0, 3, -2, 0};
    return tab[(seg >> 6) & 7];
}

static void drawRoad(s32 position, s16 curve, s16 playerX)
{
    VDP_fillTileMapRect(BG_B, TA(PAL1,135), 0, 0, 40, 4);
    VDP_fillTileMapRect(BG_B, TA(PAL1,134), 0, 4, 40, HORIZON - 4);

    s16 ddx = 0, acc = 0;

    for (s16 y = 27; y >= HORIZON; y--)
    {
        s16 dy  = y - HORIZON + 1;
        s32 z   = 1800 / dy;
        s32 seg = (position + z) >> 8;
        u16 alt = (seg >> 2) & 1;

        s16 halfW = (dy * 13) >> 4; if (halfW < 1) halfW = 1;
        s16 rum   = halfW >> 2;     if (rum   < 1) rum   = 1;

        s16 center = 20 - (s16)(((s32)playerX * halfW) / 100) + acc;
        s16 left   = center - halfW;
        s16 right  = center + halfW;

        u16 gT = alt ? TA(PAL1,128) : TA(PAL1,129);
        u16 rT = alt ? TA(PAL1,130) : TA(PAL1,131);
        u16 bT = alt ? TA(PAL1,132) : TA(PAL1,133);

        VDP_fillTileMapRect(BG_B, gT, 0, y, 40, 1);

        s16 lx = left - rum; if (lx < 0) lx = 0;
        s16 lw = left - lx;  if (lw > 0) VDP_fillTileMapRect(BG_B, bT, lx, y, lw, 1);

        s16 rx = left;  if (rx < 0) rx = 0;
        s16 rw = right - rx;
        if (rw > 0) { if (rx + rw > 40) rw = 40 - rx; if (rw > 0) VDP_fillTileMapRect(BG_B, rT, rx, y, rw, 1); }

        s16 bx = right; if (bx < 0) bx = 0;
        s16 bw = (right + rum) - bx; if (bx + bw > 40) bw = 40 - bx;
        if (bw > 0) VDP_fillTileMapRect(BG_B, bT, bx, y, bw, 1);

        ddx += curve;
        acc += ddx >> 5;
    }
}

static void drawCar(s16 carX)
{
    VDP_clearTileMapRect(BG_A, 0, 22, 40, 6);
    for (u16 i = 0; i < 8; i++)
    {
        u16 tx = carX + (i & 3);
        u16 ty = 23 + (i >> 2);
        VDP_setTileMapXY(BG_A, TA(PAL0, 160 + i), tx, ty);
    }
}

int main(void)
{
    VDP_setScreenWidth320();
    PAL_setColors(0,  (u16*)pal0, 16, DMA);
    PAL_setColors(16, (u16*)pal1, 16, DMA);
    VDP_loadTileData(roadTiles, 128, 8, DMA);
    VDP_loadTileData(carTiles, 160, 8, DMA);
    JOY_init();

    s32 position = 0;
    s32 speed    = 0;
    s16 playerX  = 0;
    char txt[8];

    while (1)
    {
        u32 joy = JOY_readJoypad(JOY_1);

        /* CONTROLE MANUAL: CIMA acelera, BAIXO freia, solta = coast */
        if (joy & BUTTON_UP)        speed += 300;
        else if (joy & BUTTON_DOWN) speed -= 600;
        else                        speed -= 80;

        if (speed < 0)     speed = 0;
        if (speed > 12000) speed = 12000;

        /* DIRECAO RESPONDENDO MESMO PARADO */
        s16 steer = 0;
        if (joy & BUTTON_LEFT)  steer = -1;
        if (joy & BUTTON_RIGHT) steer = +1;
        playerX += steer * 4;

        s16 curve = trackCurve(position >> 8);
        playerX -= curve * (speed / 4000);
        if (playerX < -120) playerX = -120;
        if (playerX >  120) playerX =  120;
        if ((playerX < -95 || playerX > 95) && speed > 4000) speed -= 400;

        position += speed >> 5;

        drawRoad(position, curve, playerX);
        drawCar(18 + playerX / 12);

        sprintf(txt, "%d", (int)(speed / 100));
        VDP_drawText("VEL", 2, 0);
        VDP_drawText(txt, 6, 0);

        VDP_waitVSync();
    }
    return 0;
}
