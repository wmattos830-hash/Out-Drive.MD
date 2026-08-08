/*
 * OUT DRIVE - Mega Drive Port
 * Prototipo inicial
 */
#include <genesis.h>

int main()
{
    // Inicializa video (320x224, modo nativo MD)
    VDP_setScreenWidth320();
    
    // Paleta preta inicial
    PAL_setColors(0, palette_black, 64, CPU);
    
    // Titulo do jogo
    VDP_drawText("OUT DRIVE", 13, 4);
    VDP_drawText("================", 12, 6);
    VDP_drawText("MEGA DRIVE PORT", 10, 9);
    VDP_drawText("v0.1 - Prototipo", 9, 13);
    VDP_drawText("", 10, 17);
    VDP_drawText("Praia > Campo > Cidade", 7, 18);
    VDP_drawText("> Montanha > Por do Sol", 6, 20);
    VDP_drawText("", 10, 23);
    VDP_drawText("Pressione START", 10, 24);
    
    // Inicializa controles
    JOY_init();
    
    // Variaveis de tempo
    u16 frame = 0;
    char timeStr[20];
    
    // Loop principal
    while (TRUE)
    {
        frame++;
        
        // Le controle
        u16 joy = JOY_readJoypad(JOY_1);
        
        // Blink do texto "Pressione START" a cada 30 frames
        if (frame % 60 < 30) {
            VDP_clearTextArea(10, 24, 20, 1);
            VDP_drawText("Pressione START", 10, 24);
        } else {
            VDP_clearTextArea(10, 24, 20, 1);
        }
        
        // Se apertou START, limpa tela (placeholder)
        if (joy & BUTTON_START) {
            VDP_drawTextEx(BG_A, "INICIANDO CORRIDA...", 
                           TILE_ATTR(PAL0, FALSE, FALSE, FALSE),
                           8, 14, DMA);
        }
        
        // Contador de frames no canto
        sprintf(timeStr, "F:%d", frame);
        VDP_clearTextArea(35, 26, 6, 1);
        VDP_drawText(timeStr, 35, 26);
        
        // Espera proximo frame (60fps)
        VDP_waitVSync();
    }
    
    return 0;
}
