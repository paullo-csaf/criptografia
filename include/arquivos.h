#ifndef ARQUIVOS_H
#define ARQUIVOS_H

#include <stddef.h>

#include "rsa.h"

int salvarTexto(
    const char *caminho,
    const char *texto
);

int lerTexto(
    const char *caminho,
    char *texto,
    size_t capacidade
);

int salvarTextoComoBytes(
    const char *caminho,
    const char *texto
);

int salvarCifra(
    const char *caminho,
    const u64 *cifra,
    size_t quantidade
);

int lerCifra(
    const char *caminho,
    u64 *cifra,
    size_t capacidade
);

int salvarParametrosRSA(
    const char *caminho,
    const Params *params,
    const PublicKey *pub,
    const PrivKey *priv
);

int lerParametrosRSA(
    const char *caminho,
    Params *params
);

#endif