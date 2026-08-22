/*
 * Super Racing MD
 * Gerado por Mega Drive Racing Game Maker
 */

#include <genesis.h>
#include "config.h"
#include "resources.h"

// Estado do jogo
typedef struct {
    fix32 position;
    fix32 speed;
    fix32 playerX;
    u16 currentLap;
    u16 totalTime;
} GameState;

GameState gameState;

void initGame() {
    // Inicializa vídeo
    VDP_setScreenWidth320();
    
    // Carrega paleta
    PAL_setPalette(PAL0, player_pal.data, CPU);
    
    // Inicializa estado
    gameState.position = FIX32(0);
    gameState.speed = FIX32(0);
    gameState.playerX = FIX32(0);
    gameState.currentLap = 1;
    gameState.totalTime = 0;
}

void handleInput() {
    u16 joy = JOY_readJoypad(JOY_1);
    
    // Acelerar
    if (joy & BUTTON_A) {
        gameState.speed = fix32Add(gameState.speed, FIX32(0.5));
        if (gameState.speed > FIX32(370)) {
            gameState.speed = FIX32(370);
        }
    }
    
    // Frear
    if (joy & BUTTON_B) {
        gameState.speed = fix32Sub(gameState.speed, FIX32(0.14));
        if (gameState.speed < FIX32(0)) {
            gameState.speed = FIX32(0);
        }
    }
    
    // Virar
    if (joy & BUTTON_LEFT) {
        gameState.playerX = fix32Sub(gameState.playerX, FIX32(0.3));
    }
    if (joy & BUTTON_RIGHT) {
        gameState.playerX = fix32Add(gameState.playerX, FIX32(0.3));
    }
    
    // Limites da pista
    if (gameState.playerX < FIX32(-1)) gameState.playerX = FIX32(-1);
    if (gameState.playerX > FIX32(1)) gameState.playerX = FIX32(1);
}

void updateGame() {
    // Atualiza posição
    gameState.position = fix32Add(gameState.position, gameState.speed);
    
    // Verifica volta completa
    if (gameState.position >= TRACK_LENGTH) {
        gameState.position = FIX32(0);
        gameState.currentLap++;
        
        if (gameState.currentLap > 10) {
            // Fim de jogo
            VDP_drawText("FIM DE JOGO!", 12, 12);
        }
    }
}

void renderGame() {
    // Renderiza pista pseudo-3D
    renderTrack(gameState.position, gameState.playerX);
    
    // HUD
    char buffer[32];
    sprintf(buffer, "VOLTA: %d/%d", gameState.currentLap, 10);
    VDP_drawText(buffer, 2, 2);
    
    sprintf(buffer, "VEL: %d km/h", fix32ToInt(gameState.speed));
    VDP_drawText(buffer, 2, 3);
}

int main(bool hardReset) {
    initGame();
    
    while(1) {
        handleInput();
        updateGame();
        renderGame();
        
        SPR_update();
        SYS_doVBlankProcess();
    }
    
    return 0;
}
