#ifndef RITUAL_H
#define RITUAL_H

#include "raylib.h"
#include "jogo.h"

typedef struct {
    Texture2D mago;
    Texture2D pergaminhoAberto;
    Texture2D pergaminhoFechado;
    Texture2D runas[3];
    Font fonte;
} RecursosRitual;

void ritualAtualizar(
    Jogo *jogo,
    float delta
);

void ritualReiniciar(
    Jogo *jogo
);

void ritualDesenhar(
    const Jogo *jogo,
    const RecursosRitual *recursos
);

#endif