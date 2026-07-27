#ifndef EDITOR_H
#define EDITOR_H

#include <stdbool.h>
#include "jogo.h"

bool editorAdicionarCodepoint(
    char *texto,
    int *tamanhoBytes,
    int capacidade,
    int codepoint
);

bool editorAdicionarNovaLinha(
    char *texto,
    int *tamanhoBytes,
    int capacidade
);

void editorApagarUltimoUtf8(
    char *texto,
    int *tamanhoBytes
);

Vector2 editorDesenharTextoComQuebra(
    Font fonte,
    const char *texto,
    Rectangle area,
    float tamanhoFonte,
    float espacamento,
    float alturaLinha,
    Color cor
);

void editorAtualizar(Jogo *jogo);
void editorDesenhar(const Jogo *jogo, Font fonte);

#endif
