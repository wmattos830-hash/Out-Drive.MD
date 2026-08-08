/*
 * OUT DRIVE - Mega Drive Port
 * Prototipo v0.1
 */
#include <genesis.h>

int main()
{
    // 320x224, modo nativo do Mega Drive
    VDP_setScreenWidth320();

    // Titulo
    VDP_drawText("OUT DRIVE", 13, 4);
    VDP_drawText("================", 12, 6);
    VDP_drawText("MEGA DRIVE PORT", 10, 9);
    VDP_drawText("v0.1 - Prototipo", 9, 13);
    VDP_drawText("Praia > Campo > Cidade", 7, 18);
    VDP_drawText("> Montanha > Por do Sol", 6, 20);

    // Controles
    JOY_init();

    u16 frame = 0;

    // Loop principal
    while (TRUE)
    {
        frame++;

        u32 joy = JOY_readJoypad(JOY_1);

        // Texto piscando
        if ((frame & 31) < 16)
            VDP_drawText("Pressione START", 10, 24);
        else
            VDP_clearTextArea(10, 24, 15, 1);

        // START = placeholder da corrida
        if (joy & BUTTON_START)
            VDP_drawText("INICIANDO CORRIDA...", 8, 14);

        // 60 fps
        VDP_waitVSync();
    }

    return 0;
}
