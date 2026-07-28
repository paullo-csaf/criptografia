#include <stddef.h>
#include <string.h>

#include "arquivos.h"
#include "dialogo.h"
#include "ritual.h"


/*
 * Atualiza tanto a aparição gradual do mago
 * quanto a tela de diálogo.
 */
void dialogoAtualizar(
    Jogo *jogo,
    float delta
) {
    if (jogo == NULL) {
        return;
    }

    switch (jogo->cena) {
        case CENA_MAGO_APARECENDO:
            jogo->alphaMago += 110.0f * delta;

            if (jogo->alphaMago >= 255.0f) {
                jogo->alphaMago = 255.0f;
                jogo->cena = CENA_DIALOGO;
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                jogo->cena = CENA_MENU;
            }

            break;

        case CENA_DIALOGO:
            if (IsKeyPressed(KEY_ENTER)) {
                /*
                 * Criptografia:
                 * segue para o editor de mensagem normal.
                 */
                if (jogo->acao == ACAO_CRIPTOGRAFAR) {
                    if (
                    jogo->modoCriptografia ==
                    CRIPTOGRAFAR_DIGITADA
                ) {
                    jogo->mensagem[0] = '\0';
                    jogo->tamanhoMensagemBytes = 0;

                    jogo->cena = CENA_EDITOR;
                } else {
                    if (
                        !lerTexto(
                            "entrada/mensagem.txt",
                            jogo->mensagem,
                            sizeof(jogo->mensagem)
                        )
                    ) {
                        strncpy(
                            jogo->aviso,
                            "Não foi possível ler entrada/mensagem.txt.",
                            sizeof(jogo->aviso) - 1
                        );

                        jogo->aviso[
                            sizeof(jogo->aviso) - 1
                        ] = '\0';

                        jogo->tempoAviso = 3.0f;
                        jogo->cena = CENA_MENU;
                        break;
                    }

                    jogo->tamanhoMensagemBytes =
                        (int)strlen(jogo->mensagem);

                    if (jogo->tamanhoMensagemBytes == 0) {
                        strncpy(
                            jogo->aviso,
                            "O arquivo entrada/mensagem.txt está vazio.",
                            sizeof(jogo->aviso) - 1
                        );

                        jogo->aviso[
                            sizeof(jogo->aviso) - 1
                        ] = '\0';

                        jogo->tempoAviso = 3.0f;
                        jogo->cena = CENA_MENU;
                        break;
                    }

                    ritualReiniciar(jogo);

                    jogo->cena =
                        CENA_RITUAL_CRIPTOGRAFIA;
                }
                }

                /*
                 * Descriptografia:
                 * depende do modo selecionado no submenu.
                 */
                else {
                    if (
                        jogo->modoDescriptografia ==
                        DESCRIPTOGRAFAR_DIGITADA
                    ) {
                        jogo->mensagem[0] = '\0';
                        jogo->tamanhoMensagemBytes = 0;

                        jogo->cena = CENA_EDITOR;
                    } else {
                        ritualReiniciar(jogo);

                        jogo->cena = CENA_RITUAL_DESCRIPTOGRAFIA;
                    }
                }
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                jogo->cena = CENA_MENU;
            }

            break;

        default:
            break;
    }
}

/*
 * Desenha o mago surgindo e a caixa de diálogo.
 */
void dialogoDesenhar(
    const Jogo *jogo,
    Font fonte,
    Texture2D mago
) {
    if (jogo == NULL) {
        return;
    }

    Rectangle origemMago = {
        0.0f,
        0.0f,
        (float)mago.width,
        (float)mago.height
    };

    Rectangle destinoMago = {
        288.0f,
        5.0f,
        384.0f,
        384.0f
    };

    Color corMago = {
        255,
        255,
        255,
        (unsigned char)jogo->alphaMago
    };

    DrawTexturePro(
        mago,
        origemMago,
        destinoMago,
        (Vector2){0.0f, 0.0f},
        0.0f,
        corMago
    );

    /*
     * Durante CENA_MAGO_APARECENDO,
     * só desenha o mago.
     */
    if (jogo->cena != CENA_DIALOGO) {
        return;
    }

    Rectangle caixaDialogo = {
        90.0f,
        370.0f,
        780.0f,
        150.0f
    };

    DrawRectangleRounded(
        caixaDialogo,
        0.08f,
        8,
        (Color){10, 12, 20, 245}
    );

    DrawRectangleRoundedLinesEx(
        caixaDialogo,
        0.08f,
        8,
        2.0f,
        SKYBLUE
    );

    DrawTextEx(
        fonte,
        "Olá, viajante...",
        (Vector2){120.0f, 392.0f},
        30.0f,
        1.0f,
        RAYWHITE
    );

    const char *fala = NULL;

    if (jogo->acao == ACAO_CRIPTOGRAFAR) {
    if (
        jogo->modoCriptografia ==
        CRIPTOGRAFAR_ARQUIVO
    ) {
        fala =
            "Trarei as palavras guardadas no arquivo.";
    } else {
        fala =
            "Entregue-me as palavras que deseja ocultar.";
    }
    } else if (
        jogo->modoDescriptografia ==
        DESCRIPTOGRAFAR_ULTIMA
    ) {
        fala =
            "Trarei à luz o último pergaminho selado.";
    } else {
        fala =
            "Mostre-me os símbolos que deseja revelar.";
    }

    DrawTextEx(
        fonte,
        fala,
        (Vector2){120.0f, 442.0f},
        25.0f,
        1.0f,
        LIGHTGRAY
    );

    DrawTextEx(
        fonte,
        "ENTER",
        (Vector2){770.0f, 485.0f},
        17.0f,
        1.0f,
        SKYBLUE
    );
}