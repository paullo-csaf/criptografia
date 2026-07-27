#include <stddef.h>
#include "menu.h"

/*
 * Desenha um texto centralizado horizontalmente.
 */
static void desenharTextoCentralizado(
    Font fonte,
    const char *texto,
    float y,
    float tamanho,
    Color cor
) {
    Vector2 medida = MeasureTextEx(
        fonte,
        texto,
        tamanho,
        1.0f
    );

    DrawTextEx(
        fonte,
        texto,
        (Vector2){
            (
                LARGURA_JANELA -
                medida.x
            ) / 2.0f,
            y
        },
        tamanho,
        1.0f,
        cor
    );
}

/*
 * Atualiza o menu principal.
 */
void menuAtualizar(Jogo *jogo) {
    
    if (jogo == NULL) {
        return;
    }

    if (IsKeyPressed(KEY_DOWN)) {
        jogo->opcaoMenu++;

        if (jogo->opcaoMenu > 2) {
            jogo->opcaoMenu = 0;
        }
    }

    if (IsKeyPressed(KEY_UP)) {
        jogo->opcaoMenu--;

        if (jogo->opcaoMenu < 0) {
            jogo->opcaoMenu = 2;
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        switch (jogo->opcaoMenu) {
            case 0:
                /*
                 * Criptografar mensagem.
                 */
                /*jogo->acao = ACAO_CRIPTOGRAFAR;
                jogo->alphaMago = 0.0f;
                jogo->cena = CENA_MAGO_APARECENDO;
                break;*/

                jogo->opcaoCriptografar = 0;
                jogo->cena = CENA_MENU_CRIPTOGRAFAR;
                break;

            case 1:
                /*
                 * Abre o submenu da descriptografia.
                 */
                jogo->opcaoDescriptografar = 0;
                jogo->cena =
                    CENA_MENU_DESCRIPTOGRAFAR;
                break;

            case 2:
                /*
                 * Encerra o programa.
                 */
                jogo->executando = false;
                break;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        jogo->executando = false;
    }
}

/*
 * Desenha o menu principal.
 */
void menuDesenhar(
    const Jogo *jogo,
    Font fonte
) {
    if (jogo == NULL) {
        return;
    }

    const char *opcoes[] = {
        "Criptografar mensagem",
        "Descriptografar mensagem",
        "Sair"
    };

    desenharTextoCentralizado(
        fonte,
        "O ESCRIBA ARCANO",
        80.0f,
        44.0f,
        RAYWHITE
    );

    desenharTextoCentralizado(
        fonte,
        "Escolha o ritual",
        145.0f,
        25.0f,
        GRAY
    );

    for (int i = 0; i < 3; i++) {
        const float tamanhoFonte = 30.0f;

        Vector2 medida = MeasureTextEx(
            fonte,
            opcoes[i],
            tamanhoFonte,
            1.0f
        );

        float x =
            (
                LARGURA_JANELA -
                medida.x
            ) / 2.0f;

        float y =
            245.0f +
            i * 62.0f;

        Color cor =
            i == jogo->opcaoMenu
                ? SKYBLUE
                : RAYWHITE;

        if (i == jogo->opcaoMenu) {
            DrawTextEx(
                fonte,
                ">",
                (Vector2){
                    x - 42.0f,
                    y
                },
                tamanhoFonte,
                1.0f,
                SKYBLUE
            );
        }

        DrawTextEx(
            fonte,
            opcoes[i],
            (Vector2){x, y},
            tamanhoFonte,
            1.0f,
            cor
        );
    }

    desenharTextoCentralizado(
        fonte,
        "Use as setas e pressione ENTER",
        470.0f,
        19.0f,
        DARKGRAY
    );
}

/*
 * Atualiza o submenu da descriptografia.
 */
void submenuDescriptografiaAtualizar(
    Jogo *jogo
) {
    if (jogo == NULL) {
        return;
    }

    if (IsKeyPressed(KEY_DOWN)) {
        jogo->opcaoDescriptografar++;

        if (
            jogo->opcaoDescriptografar >
            2
        ) {
            jogo->opcaoDescriptografar = 0;
        }
    }

    if (IsKeyPressed(KEY_UP)) {
        jogo->opcaoDescriptografar--;

        if (
            jogo->opcaoDescriptografar <
            0
        ) {
            jogo->opcaoDescriptografar = 2;
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        switch (
            jogo->opcaoDescriptografar
        ) {
            case 0:
                /*
                 * Usar a última cifra salva.
                 */
                jogo->acao =
                    ACAO_DESCRIPTOGRAFAR;

                jogo->modoDescriptografia =
                    DESCRIPTOGRAFAR_ULTIMA;

                jogo->alphaMago = 0.0f;

                jogo->cena =
                    CENA_MAGO_APARECENDO;

                break;

            case 1:
                /*
                 * Digitar a cifra manualmente.
                 */
                jogo->acao =
                    ACAO_DESCRIPTOGRAFAR;

                jogo->modoDescriptografia =
                    DESCRIPTOGRAFAR_DIGITADA;

                jogo->mensagem[0] = '\0';
                jogo->tamanhoMensagemBytes = 0;

                jogo->alphaMago = 0.0f;

                jogo->cena =
                    CENA_MAGO_APARECENDO;

                break;

            case 2:
                /*
                 * Retorna ao menu principal.
                 */
                jogo->cena = CENA_MENU;
                break;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        jogo->cena = CENA_MENU;
    }
}

/*
 * Desenha o submenu da descriptografia.
 */
void submenuDescriptografiaDesenhar(
    const Jogo *jogo,
    Font fonte
) {
    if (jogo == NULL) {
        return;
    }

    const char *opcoes[] = {
        "Usar a última mensagem criptografada",
        "Digitar mensagem criptografada",
        "Voltar"
    };

    desenharTextoCentralizado(
        fonte,
        "DESCRIPTOGRAFAR",
        75.0f,
        42.0f,
        RAYWHITE
    );

    desenharTextoCentralizado(
        fonte,
        "Escolha a origem da mensagem",
        140.0f,
        24.0f,
        GRAY
    );

    for (int i = 0; i < 3; i++) {
        const float tamanhoFonte = 27.0f;

        Vector2 medida = MeasureTextEx(
            fonte,
            opcoes[i],
            tamanhoFonte,
            1.0f
        );

        float x =
            (
                LARGURA_JANELA -
                medida.x
            ) / 2.0f;

        float y =
            235.0f +
            i * 65.0f;

        Color cor =
            i ==
            jogo->opcaoDescriptografar
                ? SKYBLUE
                : RAYWHITE;

        if (
            i ==
            jogo->opcaoDescriptografar
        ) {
            DrawTextEx(
                fonte,
                ">",
                (Vector2){
                    x - 40.0f,
                    y
                },
                tamanhoFonte,
                1.0f,
                SKYBLUE
            );
        }

        DrawTextEx(
            fonte,
            opcoes[i],
            (Vector2){x, y},
            tamanhoFonte,
            1.0f,
            cor
        );
    }

    desenharTextoCentralizado(
        fonte,
        "ESC: voltar",
        470.0f,
        19.0f,
        DARKGRAY
    );
}

void submenuCriptografiaAtualizar(
    Jogo *jogo
) {
    if (jogo == NULL) {
        return;
    }

    if (IsKeyPressed(KEY_DOWN)) {
        jogo->opcaoCriptografar++;

        if (jogo->opcaoCriptografar > 2) {
            jogo->opcaoCriptografar = 0;
        }
    }

    if (IsKeyPressed(KEY_UP)) {
        jogo->opcaoCriptografar--;

        if (jogo->opcaoCriptografar < 0) {
            jogo->opcaoCriptografar = 2;
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        switch (jogo->opcaoCriptografar) {
            case 0:
                jogo->acao = ACAO_CRIPTOGRAFAR;
                jogo->modoCriptografia =
                    CRIPTOGRAFAR_DIGITADA;

                jogo->alphaMago = 0.0f;
                jogo->cena =
                    CENA_MAGO_APARECENDO;
                break;

            case 1:
                jogo->acao = ACAO_CRIPTOGRAFAR;
                jogo->modoCriptografia =
                    CRIPTOGRAFAR_ARQUIVO;

                jogo->alphaMago = 0.0f;
                jogo->cena =
                    CENA_MAGO_APARECENDO;
                break;

            case 2:
                jogo->cena = CENA_MENU;
                break;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        jogo->cena = CENA_MENU;
    }
}

void submenuCriptografiaDesenhar(
    const Jogo *jogo,
    Font fonte
) {
    if (jogo == NULL) {
        return;
    }

    const char *opcoes[] = {
        "Digitar mensagem",
        "Usar entrada/mensagem.txt",
        "Voltar"
    };

    desenharTextoCentralizado(
        fonte,
        "CRIPTOGRAFAR",
        75.0f,
        42.0f,
        RAYWHITE
    );

    desenharTextoCentralizado(
        fonte,
        "Escolha a origem da mensagem",
        140.0f,
        24.0f,
        GRAY
    );

    for (int i = 0; i < 3; i++) {
        const float tamanhoFonte = 29.0f;

        Vector2 medida = MeasureTextEx(
            fonte,
            opcoes[i],
            tamanhoFonte,
            1.0f
        );

        float x =
            (
                LARGURA_JANELA -
                medida.x
            ) / 2.0f;

        float y =
            235.0f +
            i * 65.0f;

        Color cor =
            i == jogo->opcaoCriptografar
                ? SKYBLUE
                : RAYWHITE;

        if (
            i ==
            jogo->opcaoCriptografar
        ) {
            DrawTextEx(
                fonte,
                ">",
                (Vector2){
                    x - 40.0f,
                    y
                },
                tamanhoFonte,
                1.0f,
                SKYBLUE
            );
        }

        DrawTextEx(
            fonte,
            opcoes[i],
            (Vector2){x, y},
            tamanhoFonte,
            1.0f,
            cor
        );
    }

    desenharTextoCentralizado(
        fonte,
        "ESC: voltar",
        470.0f,
        19.0f,
        DARKGRAY
    );
}