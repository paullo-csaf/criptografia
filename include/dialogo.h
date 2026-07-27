#ifndef DIALOGO_H
#define DIALOGO_H

#include "jogo.h"

void dialogoAtualizar(
    Jogo *jogo,
    float delta
);

void dialogoDesenhar(
    const Jogo *jogo,
    Font fonte,
    Texture2D mago
);

#endif
