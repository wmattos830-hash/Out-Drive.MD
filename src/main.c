/*
 * OUT DRIVE - Mega Drive Port
 * v0.2 - pista pseudo-3D + carro + controles
 */
#include <genesis.h>
#include <stdio.h>

/* ---------- cores VDP (3 bits por canal) ---------- */
#define VPC(r,g,b) (u16)((((b)>>5)<<9)|(((g)>>5)<<5)|(((r)>>5)<<1))

static const u16 pal0[16] = {
    VPC(0,0,0),       /* 0 transparente */
    VPC(224,32,32),   /* 1 vermelho     */
    VPC(128,16,16),   /* 2 verm. escuro */
    VPC(40,60,120),   /* 3 vidro        */
    VPC(10,10,10),    /* 4 preto        */
    VPC(255,208,0),   /* 5 amarelo      */
};
static const u16 pal1[16] = {
    VPC(0,0,0),
    VPC(16,160,64),   /* 1 grama clara  */
    VPC(0,120,48),    /* 2 grama escura */
    VPC(160,160,160), /* 3 pista clara  */
    VPC(130,130,130), /* 4 pista escura */
    VPC(224,32,32),   /* 5 rumble ver.  */
    VPC(240,240,240), /* 6 rumble br.   */
    VPC(80,160,240),  /* 7 ceu          */
    VPC(40,100,200),  /* 8 ceu escuro   */
};

/* ---------- tiles solidos da pista ---------- */
#define T8(V) (V),(V),(V),(V),(V),(V),(V),(V)
static const u32 roadTiles[8*8] = {
    T8(0x11111111), /* 128 grama clara  */
    T8(0x22222222), /* 129 grama escura */
    T8(0x33333333), /* 130 pista clara  */
    T8(0x44444444), /* 131 pista escura */
    T8(0x55555555), /* 132 rumble ver.  */
    T8(0x66666666), /* 133 rumble br.   */
    T8(0x77777777), /* 134 ceu          */
    T8(0x88888888), /* 135 ceu escuro   */
};

/* ---------- tiles do carro (vista traseira 32x16) ---------- */
#define R8(a,b,c,d,e,f,g,h) (((a)<<28)|((b)<<24)|((c)<<20)|((d)<<16)|((e)<<12)|((f)<<8)|((g)<<4)|(h))
static const u32 carTiles[8*8] = {
    /* col0 topo */
    R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,2), R8(0,0,0,0,0,0,0,2), R8(0,0,0,0,0,0,0,2),
    R8(0,0,1,1,1,1,1,1), R8(0,0,1,1,1,1,1,1), R8(0,0,5,5,1,1,1,1), R8(0,0,1,1,1,1,1,1),
    /* col1 topo */
    R8(2,2,2,2,2,2,2,2), R8(3,3,3,3,3,3,3,3), R8(3,3,3,3,3,3,3,3), R8(2,2,2,2,2,2,2,2),
    R8(1,1,1,1,1,1,1,1), R8(1,1,1,1,1,1,1,1), R8(1,1,1,1,1,1,1,1), R8(1,1,1,1,1,1,1,1),
    /* col2 topo */
    R8(2,2,2,2,2,2,2,2), R8(3,3,3,3,3,3,3,3), R8(3,3,3,3,3,3,3,3), R8(2,2,2,2,2,2,2,2),
    R8(1,1,1,1,1,1,1,1), R8(1,1,1,1,1,1,1,1), R8(1,1,1,1,1,1,1,1), R8(1,1,1,1,1,1,1,1),
    /* col3 topo */
    R8(0,0,0,0,0,0,0,0), R8(2,0,0,0,0,0,0,0), R8(2,0,0,0,0,0,0,0), R8(2,0,0,0,0,0,0,0),
    R8(1,1,1,1,1,1,0,0), R8(1,1,1,1,1,1,0,0), R8(1,1,1,1,5,5,0,0), R8(1,1,1,1,1,1,0,0),
    /* col0 baixo */
    R8(4,4,4,4,0,0,2,2), R8(4,4,4,4,0,0,2,2), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    /* col1 baixo */
    R8(2,2,2,2,2,2,2,2), R8(2,2,2,2,2,2,2,2), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    /* col2 baixo */
    R8(2,2,2,2,2,2,2,2), R8(2,2,2,2,2,2,2,2), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    /* col3 baixo */
    R8(2,2,0,0,4,4,4,4), R8(2,2,0,0,4,4,4,4), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
    R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0), R8(0,0,0,0,0,0,0,0),
};

#define TA(pal,tile) TILE_ATTR_FULL((pal), FALSE, FALSE, FALSE, (tile))
#define HORIZON 10

static u16 screenBuf[28*40];

/* curva do trecho atual (-4..4) */
static s16 trackCurve(s32 seg)
{
    static const s16 tab[8] = {0, 2, 0, -3, 0, 3, -2, 0};
    return tab[(seg >> 6) & 7];
}

/* ---------- desenha pista linha a linha ---------- */
static void drawRoad(s32 position, s16 curve, s16 playerX)
{
    /* ceu */
    for (u16 y = 0; y < HORIZON; y++)
    {
        u16 t = (y < 4) ? TA(PAL1,135) : TA(PAL1,134);
        for (u16 x = 0; x < 40; x++) screenBuf[y*40 + x] = t;
    }

    s16 ddx = 0, acc = 0;

    /* de baixo (perto) ate o horizonte (longe) */
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

        u16 *row = &screenBuf[y*40];
        for (s16 x = 0; x < 40; x++)
        {
            if (x < left - rum || x >= right + rum) row[x] = gT;
            else if (x < left || x >= right)        row[x] = bT;
            else                                    row[x] = rT;
        }

        ddx += curve;
        acc += ddx >> 5;
    }

    VDP_setTileMapXY(BG_B, screenBuf, 0, 0, 40, 28);
}

/* ---------- desenha o carro ---------- */
static void drawCar(s16 carX)
{
    static u16 carBuf[8];
    VDP_clearTileMapRect(BG_A, 0, 22, 40, 6);
    for (u16 i = 0; i < 4; i++)
    {
        carBuf[i]   = TA(PAL0, 160 + i);
        carBuf[4+i] = TA(PAL0, 164 + i);
    }
    VDP_setTileMapXY(BG_A, carBuf, carX, 23, 4, 2);
}

int main()
{
    VDP_setScreenWidth320();
    PAL_setColors(0,  (u16*)pal0, 16, DMA);
    PAL_setColors(16, (u16*)pal1, 16, DMA);
    VDP_loadTileData(roadTiles, 128, 8, DMA);
    VDP_loadTileData(carTiles, 160, 8, DMA);
    JOY_init();

    s32 position = 0;
    s32 speed    = 0;
    s16 playerX  = 0;   /* -100..100 */
    char txt[8];

    while (TRUE)
    {
        u32 joy = JOY_readJoypad(JOY_1);

        /* acelerar / frear */
        if (joy & BUTTON_UP)   speed += 300;
        else                   speed -= 100;
        if (joy & BUTTON_DOWN) speed -= 500;
        if (speed < 0)     speed = 0;
        if (speed > 12000) speed = 12000;

        /* direcao */
        s16 steer = 0;
        if (joy & BUTTON_LEFT)  steer = -1;
        if (joy & BUTTON_RIGHT) steer = +1;
        playerX += steer * (speed / 3000 + 1);

        /* curva + centrifuga */
        s16 curve = trackCurve(position >> 8);
        playerX -= curve * (speed / 4000);
        if (playerX < -120) playerX = -120;
        if (playerX >  120) playerX =  120;

        /* fora da pista perde velocidade */
        if ((playerX < -95 || playerX > 95) && speed > 4000) speed -= 400;

        position += speed >> 5;

        drawRoad(position, curve, playerX);
        drawCar(18 + playerX / 12);

        /* HUD */
        sprintf(txt, "%d", (int)(speed / 100));
        VDP_drawText("VEL", 2, 0);
        VDP_drawText(txt, 6, 0);

        VDP_waitVSync();
    }
    return 0;
}
