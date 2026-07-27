#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "raylib.h"

#include "arquivos.h"
#include "dialogo.h"
#include "editor.h"
#include "jogo.h"
#include "menu.h"
#include "ritual.h"
#include "rsa.h"

static Font carregarFontePortugues(
    const char *caminho,
    int tamanho
) {
    enum {
        PRIMEIRO_CODEPOINT = 32,
        ULTIMO_CODEPOINT = 255,
        QUANTIDADE =
            ULTIMO_CODEPOINT -
            PRIMEIRO_CODEPOINT + 1
    };

    int codepoints[QUANTIDADE];

    for (int i = 0; i < QUANTIDADE; i++) {
        codepoints[i] = PRIMEIRO_CODEPOINT + i;
    }

    return LoadFontEx(
        caminho,
        tamanho,
        codepoints,
        QUANTIDADE
    );
}

static int encerrarComErro(
    const char *mensagem,
    Font fonte,
    bool fonteCarregada,
    Texture2D mago,
    Texture2D pergaminhoAberto,
    Texture2D pergaminhoFechado,
    Texture2D runaA,
    Texture2D runaB,
    Texture2D runaC
) {
    fprintf(stderr, "%s\n", mensagem);

    if (runaC.id != 0) UnloadTexture(runaC);
    if (runaB.id != 0) UnloadTexture(runaB);
    if (runaA.id != 0) UnloadTexture(runaA);
    if (pergaminhoFechado.id != 0) UnloadTexture(pergaminhoFechado);
    if (pergaminhoAberto.id != 0) UnloadTexture(pergaminhoAberto);
    if (mago.id != 0) UnloadTexture(mago);
    if (fonteCarregada) UnloadFont(fonte);

    CloseWindow();
    return 1;
}

int main(void) {
    InitWindow(
        LARGURA_JANELA,
        ALTURA_JANELA,
        "O Escriba Arcano"
    );

    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    Texture2D mago = LoadTexture(
        "assets/sprites/mago.png"
    );

    Texture2D pergaminhoAberto = LoadTexture(
        "assets/sprites/pergaminho_aberto.png"
    );

    Texture2D pergaminhoFechado = LoadTexture(
        "assets/sprites/pergaminho_fechado.png"
    );

    Texture2D runaA = LoadTexture(
        "assets/sprites/runas/runa_a.png"
    );

    Texture2D runaB = LoadTexture(
        "assets/sprites/runas/runa_b.png"
    );

    Texture2D runaC = LoadTexture(
        "assets/sprites/runas/runa_c.png"
    );

    Font fonte = carregarFontePortugues(
        "assets/fontes/fonte.ttf",
        48
    );

    bool fonteCarregada = fonte.texture.id != 0;

    if (mago.id == 0) {
        return encerrarComErro(
            "Erro: não foi possível carregar assets/sprites/mago.png",
            fonte,
            fonteCarregada,
            mago,
            pergaminhoAberto,
            pergaminhoFechado,
            runaA,
            runaB,
            runaC
        );
    }

    if (pergaminhoAberto.id == 0) {
        return encerrarComErro(
            "Erro: não foi possível carregar assets/sprites/pergaminho_aberto.png",
            fonte,
            fonteCarregada,
            mago,
            pergaminhoAberto,
            pergaminhoFechado,
            runaA,
            runaB,
            runaC
        );
    }

    if (pergaminhoFechado.id == 0) {
        return encerrarComErro(
            "Erro: não foi possível carregar assets/sprites/pergaminho_fechado.png",
            fonte,
            fonteCarregada,
            mago,
            pergaminhoAberto,
            pergaminhoFechado,
            runaA,
            runaB,
            runaC
        );
    }

    if (runaA.id == 0 || runaB.id == 0 || runaC.id == 0) {
        return encerrarComErro(
            "Erro: não foi possível carregar uma ou mais runas.",
            fonte,
            fonteCarregada,
            mago,
            pergaminhoAberto,
            pergaminhoFechado,
            runaA,
            runaB,
            runaC
        );
    }

    if (!fonteCarregada) {
        return encerrarComErro(
            "Erro: não foi possível carregar assets/fontes/fonte.ttf",
            fonte,
            false,
            mago,
            pergaminhoAberto,
            pergaminhoFechado,
            runaA,
            runaB,
            runaC
        );
    }

    SetTextureFilter(mago, TEXTURE_FILTER_POINT);
    SetTextureFilter(pergaminhoAberto, TEXTURE_FILTER_POINT);
    SetTextureFilter(pergaminhoFechado, TEXTURE_FILTER_POINT);
    SetTextureFilter(runaA, TEXTURE_FILTER_POINT);
    SetTextureFilter(runaB, TEXTURE_FILTER_POINT);
    SetTextureFilter(runaC, TEXTURE_FILTER_POINT);
    SetTextureFilter(fonte.texture, TEXTURE_FILTER_BILINEAR);

Params params = {0};

if (
    !lerParametrosRSA(
        "dados/parametros.csv",
        &params
    )
) {
    return encerrarComErro(
        "Erro: não foi possível ler dados/parametros.csv",
        fonte,
        true,
        mago,
        pergaminhoAberto,
        pergaminhoFechado,
        runaA,
        runaB,
        runaC
    );
}

    if (!genParams(&params)) {
        return encerrarComErro(
            "Erro: parâmetros RSA inválidos.",
            fonte,
            true,
            mago,
            pergaminhoAberto,
            pergaminhoFechado,
            runaA,
            runaB,
            runaC
        );
    }

    PublicKey chavePublica = genPubk(&params);
    PrivKey chavePrivada = genPrivk(&params);

    if (
    !salvarParametrosRSA(
        "dados/parametros.csv",
        &params,
        &chavePublica,
        &chavePrivada
    )
) {
    return encerrarComErro(
        "Erro ao salvar dados/parametros.csv",
        fonte,
        true,
        mago,
        pergaminhoAberto,
        pergaminhoFechado,
        runaA,
        runaB,
        runaC
    );
}

    Jogo jogo = {
        .cena = CENA_MENU,
        .acao = ACAO_CRIPTOGRAFAR,
        .modoDescriptografia = DESCRIPTOGRAFAR_ULTIMA,
        .executando = true,
        .opcaoMenu = 0,
        .opcaoDescriptografar = 0,
        .alphaMago = 0.0f,
        .mensagem = {0},
        .tamanhoMensagemBytes = 0,
        .cifra = {0},
        .tamanhoCifra = 0,
        .mensagemRecuperada = {0},
        .chavePublica = chavePublica,
        .chavePrivada = chavePrivada,
        .aviso = {0},
        .tempoAviso = 0.0f,
        .tempoRitual = 0.0f,
        .etapaRitual = 0,
        .processamentoExecutado = false,
        .operacaoConcluida = false,
        .modoCriptografia = CRIPTOGRAFAR_DIGITADA,
        .opcaoCriptografar = 0,
    };

    RecursosRitual recursosRitual = {
        .mago = mago,
        .pergaminhoAberto = pergaminhoAberto,
        .pergaminhoFechado = pergaminhoFechado,
        .runas = {runaA, runaB, runaC},
        .fonte = fonte
    };

    while (jogo.executando && !WindowShouldClose()) {
        float delta = GetFrameTime();

        if (jogo.tempoAviso > 0.0f) {
            jogo.tempoAviso -= delta;
            if (jogo.tempoAviso < 0.0f) jogo.tempoAviso = 0.0f;
        }

        switch (jogo.cena) {
            case CENA_MENU:
                menuAtualizar(&jogo);
                break;

            case CENA_MENU_CRIPTOGRAFAR:
                submenuCriptografiaAtualizar(
                    &jogo
                );
                break;

            case CENA_MENU_DESCRIPTOGRAFAR:
                submenuDescriptografiaAtualizar(&jogo);
                break;

            case CENA_MAGO_APARECENDO:
            case CENA_DIALOGO:
                dialogoAtualizar(&jogo, delta);
                break;

            case CENA_EDITOR:
                editorAtualizar(&jogo);
                break;

            case CENA_RITUAL_CRIPTOGRAFIA:
            case CENA_RITUAL_DESCRIPTOGRAFIA:
            case CENA_RESULTADO:
                ritualAtualizar(&jogo, delta);
                break;

            default:
                jogo.cena = CENA_MENU;
                break;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        switch (jogo.cena) {
            case CENA_MENU:
                menuDesenhar(&jogo, fonte);
                break;

            case CENA_MENU_CRIPTOGRAFAR:
                submenuCriptografiaDesenhar(
                    &jogo,
                    fonte
                );
                break;

            case CENA_MENU_DESCRIPTOGRAFAR:
                submenuDescriptografiaDesenhar(&jogo, fonte);
                break;

            case CENA_MAGO_APARECENDO:
            case CENA_DIALOGO:
                dialogoDesenhar(&jogo, fonte, mago);
                break;

            case CENA_EDITOR:
                editorDesenhar(&jogo, fonte);
                break;

            case CENA_RITUAL_CRIPTOGRAFIA:
            case CENA_RITUAL_DESCRIPTOGRAFIA:
            case CENA_RESULTADO:
                ritualDesenhar(&jogo, &recursosRitual);
                break;

            default:
                break;
        }

        if (jogo.tempoAviso > 0.0f && jogo.aviso[0] != '\0') {
            Rectangle caixaAviso = {
                190.0f,
                474.0f,
                580.0f,
                48.0f
            };

            DrawRectangleRounded(
                caixaAviso,
                0.18f,
                8,
                (Color){15, 30, 45, 245}
            );

            DrawRectangleRoundedLinesEx(
                caixaAviso,
                0.18f,
                8,
                2.0f,
                SKYBLUE
            );

            Vector2 medida = MeasureTextEx(
                fonte,
                jogo.aviso,
                19.0f,
                1.0f
            );

            DrawTextEx(
                fonte,
                jogo.aviso,
                (Vector2){
                    (LARGURA_JANELA - medida.x) / 2.0f,
                    486.0f
                },
                19.0f,
                1.0f,
                SKYBLUE
            );
        }

        EndDrawing();
    }

    UnloadTexture(runaC);
    UnloadTexture(runaB);
    UnloadTexture(runaA);
    UnloadTexture(pergaminhoFechado);
    UnloadTexture(pergaminhoAberto);
    UnloadTexture(mago);
    UnloadFont(fonte);

    CloseWindow();
    return 0;
}
