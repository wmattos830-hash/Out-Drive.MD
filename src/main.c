/*
 * OUT DRIVE - Mega Drive Port
 * v0.1 - Prototipo inicial
 */
#include <genesis.h>

int main()
{
    /* Modo 320x224 (nativo do Mega Drive) */
    VDP_setScreenWidth320();

    /* Tela de titulo */
    VDP_drawText("OUT DRIVE", 13, 4);
    VDP_drawText("================", 12, 6);
    VDP_drawText("MEGA DRIVE PORT", 10, 9);
    VDP_drawText("v0.1 - Prototipo", 9, 13);
    VDP_drawText("Praia > Campo > Cidade", 7, 18);
    VDP_drawText("> Montanha > Por do Sol", 6, 20);

    /* Controles */
    JOY_init();

    u16 frame = 0;

    /* Loop principal */
    while (TRUE)
    {
        frame++;

        u32 joy = JOY_readJoypad(JOY_1);

        /* "Pressione START" piscando */
        if ((frame & 31) < 16)
            VDP_drawText("Pressione START", 10, 24);
        else
            VDP_clearTextArea(10, 24, 15, 1);

        /* START = placeholder da corrida */
        if (joy & BUTTON_START)
            VDP_drawText("INICIANDO CORRIDA...", 8, 14);

        /* 60 fps */
        VDP_waitVSync();
    }

    return 0;
}
