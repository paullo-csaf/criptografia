#ifndef MENU_H
#define MENU_H

#include "jogo.h"

void menuAtualizar(Jogo *jogo);
void menuDesenhar(const Jogo *jogo, Font fonte);

void submenuDescriptografiaAtualizar(Jogo *jogo);

void submenuDescriptografiaDesenhar(
    const Jogo *jogo,
    Font fonte
);

void submenuCriptografiaAtualizar(
    Jogo *jogo
);

void submenuCriptografiaDesenhar(
    const Jogo *jogo,
    Font fonte
);

#endif
