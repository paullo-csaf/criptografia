#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "arquivos.h"
#include "editor.h"
#include "ritual.h"

/*
 * Caminhos usados pelo beta.
 *
 * As pastas entrada/ e saida/ devem existir.
 */
#define CAMINHO_MENSAGEM_ORIGINAL \
    "saida/mensagem_original.txt"

#define CAMINHO_MENSAGEM_ORIGINAL_ASCII \
    "saida/mensagem_original_ascii.txt"

#define CAMINHO_CIFRA \
    "saida/criptografado_ascii.txt"

#define CAMINHO_MENSAGEM_RECUPERADA \
    "saida/descriptografado.txt"

#define CAMINHO_MENSAGEM_RECUPERADA_ASCII \
    "saida/descriptografado_ascii.txt"

/*
 * Copia uma mensagem para o aviso do jogo
 * garantindo o '\0' final.
 */
static void definirAviso(
    Jogo *jogo,
    const char *texto,
    float duracao
) {
    if (
        jogo == NULL ||
        texto == NULL
    ) {
        return;
    }

    strncpy(
        jogo->aviso,
        texto,
        sizeof(jogo->aviso) - 1
    );

    jogo->aviso[
        sizeof(jogo->aviso) - 1
    ] = '\0';

    jogo->tempoAviso = duracao;
}

/*
 * Reinicia os dados temporais do ritual.
 *
 * Use esta função antes de mudar para uma
 * cena de ritual.
 */
void ritualReiniciar(Jogo *jogo) {
    if (jogo == NULL) {
        return;
    }

    jogo->tempoRitual = 0.0f;
    jogo->etapaRitual = 0;

    jogo->processamentoExecutado = false;
    jogo->operacaoConcluida = false;
}

/*
 * Converte uma sequência como:
 *
 * 231 45 318 92
 *
 * para um vetor de u64.
 *
 * Aceita espaços, tabs, vírgulas, ponto e vírgula
 * e quebras de linha como separadores.
 */
static int converterTextoParaCifra(
    const char *texto,
    u64 *cifra,
    size_t capacidade
) {
    if (
        texto == NULL ||
        cifra == NULL ||
        capacidade == 0
    ) {
        return -1;
    }

    size_t quantidade = 0;
    const char *cursor = texto;

    while (*cursor != '\0') {
        /*
         * Ignora separadores.
         */
        while (
            *cursor == ' ' ||
            *cursor == '\t' ||
            *cursor == '\n' ||
            *cursor == '\r' ||
            *cursor == ',' ||
            *cursor == ';'
        ) {
            cursor++;
        }

        if (*cursor == '\0') {
            break;
        }

        /*
         * A cifra deve conter apenas números
         * positivos separados.
         */
        if (
            *cursor < '0' ||
            *cursor > '9'
        ) {
            return -1;
        }

        char *fim = NULL;

        unsigned long long valor =
            strtoull(
                cursor,
                &fim,
                10
            );

        if (
            fim == cursor ||
            quantidade >= capacidade
        ) {
            return -1;
        }

        cifra[quantidade] = (u64)valor;
        quantidade++;

        cursor = fim;

        /*
         * Se, após o número, houver um caractere que
         * não seja separador, a entrada é inválida.
         */
        if (
            *cursor != '\0' &&
            *cursor != ' ' &&
            *cursor != '\t' &&
            *cursor != '\n' &&
            *cursor != '\r' &&
            *cursor != ',' &&
            *cursor != ';'
        ) {
            return -1;
        }
    }

    if (quantidade == 0) {
        return -1;
    }

    return (int)quantidade;
}

/*
 * Executa a criptografia apenas uma vez durante
 * a animação.
 */
static int executarCriptografia(
    Jogo *jogo
) {
    if (jogo == NULL) {
        return 0;
    }

    if (jogo->mensagem[0] == '\0') {
        definirAviso(
            jogo,
            "A mensagem está vazia.",
            3.0f
        );

        return 0;
    }

    /*
     * Salva o texto original.
     */
    if (
        !salvarTexto(
            CAMINHO_MENSAGEM_ORIGINAL,
            jogo->mensagem
        )
    ) {
        definirAviso(
            jogo,
            "Erro ao salvar a mensagem original.",
            3.0f
        );

        return 0;
    }

    if (
        !salvarTextoComoBytes(
            CAMINHO_MENSAGEM_ORIGINAL_ASCII,
            jogo->mensagem
        )
    ) {
        definirAviso(
            jogo,
            "Erro ao salvar os bytes da mensagem original.",
            3.0f
        );

        return 0;
    }

    int quantidade = cifrarTexto(
        jogo->mensagem,
        jogo->cifra,
        MAX_MENSAGEM,
        &jogo->chavePublica
    );

    if (quantidade <= 0) {
        definirAviso(
            jogo,
            "Erro ao criptografar a mensagem.",
            3.0f
        );

        return 0;
    }

    jogo->tamanhoCifra =
        (size_t)quantidade;

    if (
        !salvarCifra(
            CAMINHO_CIFRA,
            jogo->cifra,
            jogo->tamanhoCifra
        )
    ) {
        definirAviso(
            jogo,
            "Erro ao salvar a mensagem criptografada.",
            3.0f
        );

        return 0;
    }

    jogo->operacaoConcluida = true;

    return 1;
}

/*
 * Carrega ou interpreta a cifra e executa
 * a descriptografia.
 */
static int executarDescriptografia(
    Jogo *jogo
) {
    if (jogo == NULL) {
        return 0;
    }

    int quantidade = 0;

    if (
        jogo->modoDescriptografia ==
        DESCRIPTOGRAFAR_ULTIMA
    ) {
        quantidade = lerCifra(
            CAMINHO_CIFRA,
            jogo->cifra,
            MAX_MENSAGEM
        );

        if (quantidade <= 0) {
            definirAviso(
                jogo,
                "Nenhuma mensagem criptografada foi encontrada.",
                3.0f
            );

            return 0;
        }
    } else {
        quantidade = converterTextoParaCifra(
            jogo->mensagem,
            jogo->cifra,
            MAX_MENSAGEM
        );

        if (quantidade <= 0) {
            definirAviso(
                jogo,
                "A cifra digitada é inválida.",
                3.0f
            );

            return 0;
        }
    }

    jogo->tamanhoCifra =
        (size_t)quantidade;

    /*
     * Cada bloco RSA deve estar no intervalo [0, n).
     */
    for (
        size_t i = 0;
        i < jogo->tamanhoCifra;
        i++
    ) {
        if (
            jogo->cifra[i] >=
            jogo->chavePrivada.n
        ) {
            definirAviso(
                jogo,
                "A cifra possui um bloco maior ou igual a n.",
                3.0f
            );

            return 0;
        }
    }

    if (
        !decifrarTexto(
            jogo->cifra,
            jogo->mensagemRecuperada,
            jogo->tamanhoCifra,
            sizeof(jogo->mensagemRecuperada),
            &jogo->chavePrivada
        )
    ) {
        definirAviso(
            jogo,
            "Erro ao descriptografar a mensagem.",
            3.0f
        );

        return 0;
    }

    if (
        !salvarTexto(
            CAMINHO_MENSAGEM_RECUPERADA,
            jogo->mensagemRecuperada
        )
    ) {
        definirAviso(
            jogo,
            "A mensagem foi revelada, mas não pôde ser salva.",
            3.0f
        );

        /*
         * A descriptografia funcionou, portanto ainda
         * consideramos a operação concluída.
         */
    }

    if (
    !salvarTexto(
        CAMINHO_MENSAGEM_RECUPERADA,
        jogo->mensagemRecuperada
    )
) {
    definirAviso(
        jogo,
        "Erro ao salvar a mensagem descriptografada.",
        3.0f
    );

    return 0;
}

if (
    !salvarTextoComoBytes(
        CAMINHO_MENSAGEM_RECUPERADA_ASCII,
        jogo->mensagemRecuperada
    )
) {
    definirAviso(
        jogo,
        "Erro ao salvar os bytes da mensagem descriptografada.",
        3.0f
    );

    return 0;
}

    jogo->operacaoConcluida = true;

    return 1;
}

/*
 * Atualização da animação e do processamento.
 */
void ritualAtualizar(
    Jogo *jogo,
    float delta
) {
    if (jogo == NULL) {
        return;
    }

    /*
     * Tela de resultado.
     */
    if (jogo->cena == CENA_RESULTADO) {
        if (
            IsKeyPressed(KEY_ENTER) ||
            IsKeyPressed(KEY_ESCAPE)
        ) {
            jogo->cena = CENA_MENU;

            jogo->tempoRitual = 0.0f;
            jogo->etapaRitual = 0;

            jogo->processamentoExecutado = false;
            jogo->operacaoConcluida = false;
        }

        return;
    }

    /*
     * Permite cancelar antes do processamento.
     */
    if (
        IsKeyPressed(KEY_ESCAPE) &&
        !jogo->processamentoExecutado
    ) {
        jogo->cena = CENA_MENU;
        ritualReiniciar(jogo);

        return;
    }

    jogo->tempoRitual += delta;

    /*
     * Etapa 0:
     * mago e pergaminho aparecem.
     */
    if (jogo->tempoRitual < 1.5f) {
        jogo->etapaRitual = 0;
    }

    /*
     * Etapa 1:
     * runas começam a orbitar.
     */
    else if (jogo->tempoRitual < 3.0f) {
        jogo->etapaRitual = 1;
    }

    /*
     * Etapa 2:
     * executa RSA uma única vez.
     */
    else if (jogo->tempoRitual < 4.5f) {
        jogo->etapaRitual = 2;

        if (!jogo->processamentoExecutado) {
            jogo->processamentoExecutado = true;

            int sucesso = 0;

            if (
                jogo->cena ==
                CENA_RITUAL_CRIPTOGRAFIA
            ) {
                sucesso =
                    executarCriptografia(jogo);
            } else {
                sucesso =
                    executarDescriptografia(jogo);
            }

            if (!sucesso) {
                /*
                 * Mantém o aviso visível e volta ao menu
                 * após um pequeno intervalo.
                 */
                jogo->operacaoConcluida = false;
            }
        }
    }

    /*
     * Etapa 3:
     * finalização visual.
     */
    else {
        jogo->etapaRitual = 3;

        if (
            jogo->operacaoConcluida &&
            jogo->tempoRitual >= 5.7f
        ) {
            jogo->cena = CENA_RESULTADO;
        } else if (
            !jogo->operacaoConcluida &&
            jogo->tempoRitual >= 5.7f
        ) {
            jogo->cena = CENA_MENU;
            ritualReiniciar(jogo);
        }
    }
}

/*
 * Desenha uma textura centralizada usando
 * um tamanho final definido.
 */
static void desenharTexturaCentralizada(
    Texture2D textura,
    float centroX,
    float centroY,
    float largura,
    float altura,
    Color cor
) {
    Rectangle origem = {
        0.0f,
        0.0f,
        (float)textura.width,
        (float)textura.height
    };

    Rectangle destino = {
        centroX,
        centroY,
        largura,
        altura
    };

    Vector2 origemDesenho = {
        largura / 2.0f,
        altura / 2.0f
    };

    DrawTexturePro(
        textura,
        origem,
        destino,
        origemDesenho,
        0.0f,
        cor
    );
}

/*
 * Desenha as três runas em órbita.
 */
static void desenharRunas(
    const RecursosRitual *recursos,
    float tempo,
    float alpha,
    bool sentidoInverso
) {
    if (recursos == NULL) {
        return;
    }

    const float centroX = 480.0f;
    const float centroY = 320.0f;
    const float raioX = 195.0f;
    const float raioY = 105.0f;

    float sentido =
        sentidoInverso
            ? -1.0f
            : 1.0f;

    for (int i = 0; i < 3; i++) {
        float deslocamento =
            (2.0f * PI / 3.0f) *
            (float)i;

        float angulo =
            tempo * 1.8f * sentido +
            deslocamento;

        float x =
            centroX +
            cosf(angulo) * raioX;

        float y =
            centroY +
            sinf(angulo) * raioY;

        /*
         * A runa parece ligeiramente menor
         * quando passa "atrás" do pergaminho.
         */
        float profundidade =
            (sinf(angulo) + 1.0f) / 2.0f;

        float tamanho =
            62.0f +
            profundidade * 24.0f;

        unsigned char canalAlpha =
            (unsigned char)alpha;

        Color cor = {
            255,
            255,
            255,
            canalAlpha
        };

        desenharTexturaCentralizada(
            recursos->runas[i],
            x,
            y,
            tamanho,
            tamanho,
            cor
        );
    }
}

/*
 * Desenha a animação do ritual.
 */
static void desenharCenaRitual(
    const Jogo *jogo,
    const RecursosRitual *recursos
) {
    float alphaMago =
        jogo->tempoRitual * 170.0f;

    if (alphaMago > 255.0f) {
        alphaMago = 255.0f;
    }

    Color corMago = {
        255,
        255,
        255,
        (unsigned char)alphaMago
    };

    desenharTexturaCentralizada(
        recursos->mago,
        480.0f,
        170.0f,
        360.0f,
        360.0f,
        corMago
    );

    bool criptografando =
        jogo->cena ==
        CENA_RITUAL_CRIPTOGRAFIA;

    /*
     * Na criptografia, o pergaminho começa aberto
     * e depois fecha.
     *
     * Na descriptografia, começa fechado e depois abre.
     */
    bool usarFechado;

    if (criptografando) {
        usarFechado =
            jogo->etapaRitual >= 2;
    } else {
        usarFechado =
            jogo->etapaRitual < 3;
    }

    Texture2D pergaminho =
        usarFechado
            ? recursos->pergaminhoFechado
            : recursos->pergaminhoAberto;

    float flutuacao =
        sinf((float)GetTime() * 2.4f) *
        5.0f;

    desenharTexturaCentralizada(
        pergaminho,
        480.0f,
        335.0f + flutuacao,
        260.0f,
        260.0f,
        WHITE
    );

    if (jogo->etapaRitual >= 1) {
        float alphaRunas = 255.0f;

        if (jogo->etapaRitual == 1) {
            float progresso =
                (
                    jogo->tempoRitual -
                    1.5f
                ) / 1.5f;

            if (progresso < 0.0f) {
                progresso = 0.0f;
            }

            if (progresso > 1.0f) {
                progresso = 1.0f;
            }

            alphaRunas =
                progresso * 255.0f;
        }

        if (jogo->etapaRitual == 3) {
            float progresso =
                (
                    jogo->tempoRitual -
                    4.5f
                ) / 1.2f;

            alphaRunas =
                255.0f -
                progresso * 255.0f;

            if (alphaRunas < 0.0f) {
                alphaRunas = 0.0f;
            }
        }

        desenharRunas(
            recursos,
            jogo->tempoRitual,
            alphaRunas,
            !criptografando
        );
    }

    const char *texto = NULL;

    if (criptografando) {
        switch (jogo->etapaRitual) {
            case 0:
                texto =
                    "O pergaminho aguarda o selo...";
                break;

            case 1:
                texto =
                    "As runas despertam.";
                break;

            case 2:
                texto =
                    "Ocultando as palavras...";
                break;

            default:
                texto =
                    "O selo foi concluído.";
                break;
        }
    } else {
        switch (jogo->etapaRitual) {
            case 0:
                texto =
                    "O pergaminho permanece selado...";
                break;

            case 1:
                texto =
                    "As runas recuam.";
                break;

            case 2:
                texto =
                    "Revelando as palavras...";
                break;

            default:
                texto =
                    "O selo foi desfeito.";
                break;
        }
    }

    Vector2 medida = MeasureTextEx(
        recursos->fonte,
        texto,
        24.0f,
        1.0f
    );

    DrawTextEx(
        recursos->fonte,
        texto,
        (Vector2){
            (
                LARGURA_JANELA -
                medida.x
            ) / 2.0f,
            480.0f
        },
        24.0f,
        1.0f,
        RAYWHITE
    );
}

/*
 * Desenha a tela final.
 */
static void desenharResultado(
    const Jogo *jogo,
    const RecursosRitual *recursos
) {
    bool foiCriptografia =
        jogo->acao ==
        ACAO_CRIPTOGRAFAR;

    const char *titulo =
        foiCriptografia
            ? "PERGAMINHO SELADO"
            : "MENSAGEM REVELADA";

    Vector2 medidaTitulo = MeasureTextEx(
        recursos->fonte,
        titulo,
        38.0f,
        1.0f
    );

    DrawTextEx(
        recursos->fonte,
        titulo,
        (Vector2){
            (
                LARGURA_JANELA -
                medidaTitulo.x
            ) / 2.0f,
            52.0f
        },
        38.0f,
        1.0f,
        SKYBLUE
    );

    if (foiCriptografia) {
        desenharTexturaCentralizada(
            recursos->pergaminhoFechado,
            480.0f,
            270.0f,
            280.0f,
            280.0f,
            WHITE
        );

        const char *mensagem =
            "A última cifra foi salva em "
            "saida/criptografado.txt";

        Vector2 medida = MeasureTextEx(
            recursos->fonte,
            mensagem,
            22.0f,
            1.0f
        );

        DrawTextEx(
            recursos->fonte,
            mensagem,
            (Vector2){
                (
                    LARGURA_JANELA -
                    medida.x
                ) / 2.0f,
                420.0f
            },
            22.0f,
            1.0f,
            LIGHTGRAY
        );
    } else {
        Rectangle caixa = {
            110.0f,
            125.0f,
            740.0f,
            295.0f
        };

        DrawRectangleRounded(
            caixa,
            0.04f,
            8,
            (Color){19, 16, 15, 255}
        );

        DrawRectangleRoundedLinesEx(
            caixa,
            0.04f,
            8,
            3.0f,
            (Color){145, 105, 65, 255}
        );

        Rectangle areaTexto = {
            caixa.x + 30.0f,
            caixa.y + 30.0f,
            caixa.width - 60.0f,
            caixa.height - 60.0f
        };

        BeginScissorMode(
            (int)areaTexto.x,
            (int)areaTexto.y,
            (int)areaTexto.width,
            (int)areaTexto.height
        );

        editorDesenharTextoComQuebra(
            recursos->fonte,
            jogo->mensagemRecuperada,
            areaTexto,
            27.0f,
            1.0f,
            37.0f,
            (Color){224, 207, 177, 255}
        );

        EndScissorMode();
    }

    const char *voltar =
        "ENTER ou ESC: voltar ao menu";

    Vector2 medidaVoltar = MeasureTextEx(
        recursos->fonte,
        voltar,
        19.0f,
        1.0f
    );

    DrawTextEx(
        recursos->fonte,
        voltar,
        (Vector2){
            (
                LARGURA_JANELA -
                medidaVoltar.x
            ) / 2.0f,
            485.0f
        },
        19.0f,
        1.0f,
        GRAY
    );
}

void ritualDesenhar(
    const Jogo *jogo,
    const RecursosRitual *recursos
) {
    if (
        jogo == NULL ||
        recursos == NULL
    ) {
        return;
    }

    if (jogo->cena == CENA_RESULTADO) {
        desenharResultado(
            jogo,
            recursos
        );

        return;
    }

    desenharCenaRitual(
        jogo,
        recursos
    );
}