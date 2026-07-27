#include <stdbool.h>
#include <string.h>

#include "editor.h"
#include "ritual.h"

/*
 * Remove o último caractere UTF-8 completo.
 *
 * Caracteres acentuados podem ocupar mais de um byte,
 * então não podemos simplesmente remover o último byte.
 */
void editorApagarUltimoUtf8(
    char *texto,
    int *tamanhoBytes
) {
    if (
        texto == NULL ||
        tamanhoBytes == NULL ||
        *tamanhoBytes <= 0
    ) {
        return;
    }

    (*tamanhoBytes)--;

    /*
     * Em UTF-8, bytes de continuação possuem o formato:
     *
     * 10xxxxxx
     */
    while (
        *tamanhoBytes > 0 &&
        (
            (unsigned char)texto[*tamanhoBytes] &
            0xC0
        ) == 0x80
    ) {
        (*tamanhoBytes)--;
    }

    texto[*tamanhoBytes] = '\0';
}

/*
 * Converte um codepoint Unicode para UTF-8
 * e o adiciona ao texto.
 */
bool editorAdicionarCodepoint(
    char *texto,
    int *tamanhoBytes,
    int capacidade,
    int codepoint
) {
    if (
        texto == NULL ||
        tamanhoBytes == NULL ||
        capacidade <= 0
    ) {
        return false;
    }

    int quantidadeBytes = 0;

    const char *utf8 = CodepointToUTF8(
        codepoint,
        &quantidadeBytes
    );

    if (
        utf8 == NULL ||
        quantidadeBytes <= 0
    ) {
        return false;
    }

    /*
     * Reserva um byte para o '\0'.
     */
    if (
        *tamanhoBytes + quantidadeBytes >=
        capacidade
    ) {
        return false;
    }

    memcpy(
        texto + *tamanhoBytes,
        utf8,
        (size_t)quantidadeBytes
    );

    *tamanhoBytes += quantidadeBytes;
    texto[*tamanhoBytes] = '\0';

    return true;
}

/*
 * Insere uma quebra de linha manual.
 */
bool editorAdicionarNovaLinha(
    char *texto,
    int *tamanhoBytes,
    int capacidade
) {
    if (
        texto == NULL ||
        tamanhoBytes == NULL ||
        *tamanhoBytes + 1 >= capacidade
    ) {
        return false;
    }

    texto[*tamanhoBytes] = '\n';
    (*tamanhoBytes)++;
    texto[*tamanhoBytes] = '\0';

    return true;
}

/*
 * Desenha texto UTF-8 com quebra automática.
 *
 * Retorna a posição final do cursor.
 */
Vector2 editorDesenharTextoComQuebra(
    Font fonte,
    const char *texto,
    Rectangle area,
    float tamanhoFonte,
    float espacamento,
    float alturaLinha,
    Color cor
) {
    Vector2 posicao = {
        area.x,
        area.y
    };

    if (texto == NULL) {
        return posicao;
    }

    const char *cursorTexto = texto;

    while (*cursorTexto != '\0') {
        int bytesLidos = 0;

        int codepoint = GetCodepoint(
            cursorTexto,
            &bytesLidos
        );

        /*
         * Proteção contra UTF-8 inválido.
         */
        if (bytesLidos <= 0) {
            bytesLidos = 1;
            codepoint = '?';
        }

        /*
         * Quebra de linha inserida pelo usuário.
         */
        if (codepoint == '\n') {
            posicao.x = area.x;
            posicao.y += alturaLinha;

            cursorTexto += bytesLidos;
            continue;
        }

        int tamanhoUtf8 = 0;

        const char *caractereUtf8 =
            CodepointToUTF8(
                codepoint,
                &tamanhoUtf8
            );

        Vector2 medida = MeasureTextEx(
            fonte,
            caractereUtf8,
            tamanhoFonte,
            espacamento
        );

        /*
         * Quebra automática ao alcançar a borda direita.
         */
        if (
            posicao.x + medida.x >
            area.x + area.width
        ) {
            posicao.x = area.x;
            posicao.y += alturaLinha;
        }

        /*
         * Interrompe o desenho ao chegar ao limite inferior.
         */
        if (
            posicao.y + alturaLinha >
            area.y + area.height
        ) {
            break;
        }

        DrawTextCodepoint(
            fonte,
            codepoint,
            posicao,
            tamanhoFonte,
            cor
        );

        posicao.x += medida.x;
        cursorTexto += bytesLidos;
    }

    return posicao;
}

/*
 * Atualiza a entrada de texto do editor.
 */
void editorAtualizar(Jogo *jogo) {
    if (jogo == NULL) {
        return;
    }

    int codepoint = GetCharPressed();

    /*
     * Pode haver mais de um caractere pendente
     * no mesmo quadro.
     */
    while (codepoint > 0) {
        /*
         * Ignora caracteres de controle.
         * O Enter é tratado separadamente.
         */
        if (
            codepoint >= 32 &&
            codepoint != 127
        ) {
            editorAdicionarCodepoint(
                jogo->mensagem,
                &jogo->tamanhoMensagemBytes,
                MAX_MENSAGEM,
                codepoint
            );
        }

        codepoint = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        editorApagarUltimoUtf8(
            jogo->mensagem,
            &jogo->tamanhoMensagemBytes
        );
    }

    bool ctrlPressionado =
        IsKeyDown(KEY_LEFT_CONTROL) ||
        IsKeyDown(KEY_RIGHT_CONTROL);

    /*
     * Ctrl + Enter confirma.
     */
    if (
    IsKeyPressed(KEY_ENTER) &&
    ctrlPressionado
) {
    if (jogo->tamanhoMensagemBytes > 0) {
        ritualReiniciar(jogo);

        if (
            jogo->acao ==
            ACAO_CRIPTOGRAFAR
        ) {
            jogo->cena =
                CENA_RITUAL_CRIPTOGRAFIA;
        } else {
            jogo->cena =
                CENA_RITUAL_DESCRIPTOGRAFIA;
        }
    }
}

    /*
     * Enter sem Ctrl cria uma nova linha.
     */
    else if (IsKeyPressed(KEY_ENTER)) {
        editorAdicionarNovaLinha(
            jogo->mensagem,
            &jogo->tamanhoMensagemBytes,
            MAX_MENSAGEM
        );
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        jogo->cena = CENA_MENU;
    }
}

/*
 * Desenha toda a tela do editor.
 */
void editorDesenhar(
    const Jogo *jogo,
    Font fonte
) {
    if (jogo == NULL) {
        return;
    }

    Rectangle caixaEditor = {
        110.0f,
        95.0f,
        740.0f,
        345.0f
    };

    DrawRectangleRounded(
        caixaEditor,
        0.04f,
        8,
        (Color){19, 16, 15, 255}
    );

    DrawRectangleRoundedLinesEx(
        caixaEditor,
        0.04f,
        8,
        3.0f,
        (Color){145, 105, 65, 255}
    );

    const char *titulo =
        jogo->acao == ACAO_CRIPTOGRAFAR
            ? "Escreva a mensagem que deseja selar"
            : "Insira os blocos da mensagem cifrada";

    Vector2 medidaTitulo = MeasureTextEx(
        fonte,
        titulo,
        31.0f,
        1.0f
    );

    DrawTextEx(
        fonte,
        titulo,
        (Vector2){
            (
                LARGURA_JANELA -
                medidaTitulo.x
            ) / 2.0f,
            38.0f
        },
        31.0f,
        1.0f,
        RAYWHITE
    );

    Rectangle areaTexto = {
        caixaEditor.x + 28.0f,
        caixaEditor.y + 28.0f,
        caixaEditor.width - 56.0f,
        caixaEditor.height - 56.0f
    };

    /*
     * Impede que o texto seja desenhado fora da caixa.
     */
    BeginScissorMode(
        (int)areaTexto.x,
        (int)areaTexto.y,
        (int)areaTexto.width,
        (int)areaTexto.height
    );

    Vector2 cursor = editorDesenharTextoComQuebra(
        fonte,
        jogo->mensagem,
        areaTexto,
        28.0f,
        1.0f,
        37.0f,
        (Color){224, 207, 177, 255}
    );

    /*
     * Cursor piscando.
     */
    if (
        ((int)(GetTime() * 2.0)) % 2 == 0
    ) {
        DrawRectangle(
            (int)cursor.x + 1,
            (int)cursor.y,
            2,
            30,
            SKYBLUE
        );
    }

    EndScissorMode();

    DrawTextEx(
        fonte,
        "ENTER: nova linha",
        (Vector2){79.0f, 475.0f},
        24.0f,
        1.0f,
        BLACK
    );

    DrawTextEx(
        fonte,
        "ENTER: nova linha",
        (Vector2){81.0f, 475.0f},
        24.0f,
        1.0f,
        BLACK
    );

    DrawTextEx(
        fonte,
        "ENTER: nova linha",
        (Vector2){80.0f, 474.0f},
        24.0f,
        1.0f,
        BLACK
    );

    DrawTextEx(
        fonte,
        "ENTER: nova linha",
        (Vector2){80.0f, 476.0f},
        24.0f,
        1.0f,
        BLACK
    );

    DrawTextEx(
        fonte,
        "ENTER: nova linha",
        (Vector2){80.0f, 475.0f},
        24.0f,
        1.0f,
        GRAY
    );

    DrawTextEx(
        fonte,
        "ESC: voltar",
        (Vector2){389.0f, 475.0f},
        24.0f,
        1.0f,
        BLACK
    );

    DrawTextEx(
        fonte,
        "ESC: voltar",
        (Vector2){391.0f, 475.0f},
        24.0f,
        1.0f,
        BLACK
    );

    DrawTextEx(
        fonte,
        "ESC: voltar",
        (Vector2){390.0f, 474.0f},
        24.0f,
        1.0f,
        BLACK
    );

    DrawTextEx(
        fonte,
        "ESC: voltar",
        (Vector2){390.0f, 476.0f},
        24.0f,
        1.0f,
        BLACK
    );

    DrawTextEx(
        fonte,
        "ESC: voltar",
        (Vector2){390.0f, 475.0f},
        24.0f,
        1.0f,
        GRAY
    );

    DrawTextEx(
        fonte,
        "CTRL + ENTER: confirmar",
        (Vector2){589.0f, 475.0f},
        24.0f,
        1.0f,
        BLACK
    );

    DrawTextEx(
        fonte,
        "CTRL + ENTER: confirmar",
        (Vector2){591.0f, 475.0f},
        24.0f,
        1.0f,
        BLACK
    );

    DrawTextEx(
        fonte,
        "CTRL + ENTER: confirmar",
        (Vector2){590.0f, 474.0f},
        24.0f,
        1.0f,
        BLACK
    );

    DrawTextEx(
        fonte,
        "CTRL + ENTER: confirmar",
        (Vector2){590.0f, 476.0f},
        24.0f,
        1.0f,
        BLACK
    );

    DrawTextEx(
        fonte,
        "CTRL + ENTER: confirmar",
        (Vector2){590.0f, 475.0f},
        24.0f,
        1.0f,
        SKYBLUE
    );
}