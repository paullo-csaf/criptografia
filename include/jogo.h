#ifndef JOGO_H
#define JOGO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "rsa.h"
#include "raylib.h"

#define LARGURA_JANELA 960
#define ALTURA_JANELA 540
#define MAX_MENSAGEM 16384

typedef enum {
    CENA_MENU,
    CENA_MENU_CRIPTOGRAFAR,
    CENA_MENU_DESCRIPTOGRAFAR,
    CENA_MAGO_APARECENDO,
    CENA_DIALOGO,
    CENA_EDITOR,
    CENA_RITUAL_CRIPTOGRAFIA,
    CENA_RITUAL_DESCRIPTOGRAFIA,
    CENA_RESULTADO
} Cena;

typedef enum {
    CRIPTOGRAFAR_DIGITADA,
    CRIPTOGRAFAR_ARQUIVO
} ModoCriptografia;

typedef enum {
    ACAO_CRIPTOGRAFAR,
    ACAO_DESCRIPTOGRAFAR
} Acao;

typedef enum {
    DESCRIPTOGRAFAR_ULTIMA,
    DESCRIPTOGRAFAR_DIGITADA
} ModoDescriptografia;

typedef struct {
    Cena cena;
    Acao acao;
    ModoDescriptografia modoDescriptografia;

    bool executando;

    int opcaoMenu;
    int opcaoDescriptografar;

    float alphaMago;

    char mensagem[MAX_MENSAGEM];
    int tamanhoMensagemBytes;

    char aviso[128];
    float tempoAviso;

    u64 cifra[MAX_MENSAGEM];
    size_t tamanhoCifra;

    char mensagemRecuperada[MAX_MENSAGEM];

    PublicKey chavePublica;
    PrivKey chavePrivada;

    float tempoRitual;
    int etapaRitual;
    bool processamentoExecutado;
    bool operacaoConcluida;

    ModoCriptografia modoCriptografia;
    int opcaoCriptografar;
    } Jogo;

#endif
