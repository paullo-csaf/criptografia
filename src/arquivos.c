#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "arquivos.h"

int salvarTexto(
    const char *caminho,
    const char *texto
) {
    if (
        caminho == NULL ||
        texto == NULL
    ) {
        return 0;
    }

    FILE *arquivo = fopen(caminho, "w");

    if (arquivo == NULL) {
        return 0;
    }

    if (fputs(texto, arquivo) == EOF) {
        fclose(arquivo);
        return 0;
    }

    if (fclose(arquivo) != 0) {
        return 0;
    }

    return 1;
}

int lerTexto(
    const char *caminho,
    char *texto,
    size_t capacidade
) {
    if (
        caminho == NULL ||
        texto == NULL ||
        capacidade == 0
    ) {
        return 0;
    }

    FILE *arquivo = fopen(caminho, "r");

    if (arquivo == NULL) {
        return 0;
    }

    size_t quantidadeLida = fread(
        texto,
        sizeof(char),
        capacidade - 1,
        arquivo
    );

    texto[quantidadeLida] = '\0';

    if (ferror(arquivo)) {
        fclose(arquivo);
        return 0;
    }

    if (fclose(arquivo) != 0) {
        return 0;
    }

    return 1;
}

int salvarCifra(
    const char *caminho,
    const u64 *cifra,
    size_t quantidade
) {
    if (
        caminho == NULL ||
        cifra == NULL ||
        quantidade == 0
    ) {
        return 0;
    }

    FILE *arquivo = fopen(caminho, "w");

    if (arquivo == NULL) {
        return 0;
    }

    for (size_t i = 0; i < quantidade; i++) {
        if (
            fprintf(
                arquivo,
                "%" PRIu64,
                cifra[i]
            ) < 0
        ) {
            fclose(arquivo);
            return 0;
        }

        if (i + 1 < quantidade) {
            if (fputc(' ', arquivo) == EOF) {
                fclose(arquivo);
                return 0;
            }
        }
    }

    if (fputc('\n', arquivo) == EOF) {
        fclose(arquivo);
        return 0;
    }

    if (fclose(arquivo) != 0) {
        return 0;
    }

    return 1;
}

int lerCifra(
    const char *caminho,
    u64 *cifra,
    size_t capacidade
) {
    if (
        caminho == NULL ||
        cifra == NULL ||
        capacidade == 0
    ) {
        return -1;
    }

    FILE *arquivo = fopen(caminho, "r");

    if (arquivo == NULL) {
        return -1;
    }

    size_t quantidade = 0;

    while (quantidade < capacidade) {
        int resultado = fscanf(
            arquivo,
            "%" SCNu64,
            &cifra[quantidade]
        );

        if (resultado == 1) {
            quantidade++;
            continue;
        }

        if (resultado == EOF) {
            break;
        }

        /*
         * Há algum conteúdo inválido no arquivo.
         */
        fclose(arquivo);
        return -1;
    }

    /*
     * Se ainda existir um número após atingir
     * a capacidade, o arquivo é grande demais.
     */
    if (quantidade == capacidade) {
        u64 excedente;

        if (
            fscanf(
                arquivo,
                "%" SCNu64,
                &excedente
            ) == 1
        ) {
            fclose(arquivo);
            return -1;
        }
    }

    if (ferror(arquivo)) {
        fclose(arquivo);
        return -1;
    }

    if (fclose(arquivo) != 0) {
        return -1;
    }

    return (int)quantidade;
}

int salvarTextoComoBytes(
    const char *caminho,
    const char *texto
) {
    if (
        caminho == NULL ||
        texto == NULL
    ) {
        return 0;
    }

    FILE *arquivo = fopen(caminho, "w");

    if (arquivo == NULL) {
        return 0;
    }

    const unsigned char *bytes =
        (const unsigned char *)texto;

    for (size_t i = 0; bytes[i] != '\0'; i++) {
        if (
            fprintf(
                arquivo,
                "%u",
                (unsigned int)bytes[i]
            ) < 0
        ) {
            fclose(arquivo);
            return 0;
        }

        if (bytes[i + 1] != '\0') {
            if (fputc(' ', arquivo) == EOF) {
                fclose(arquivo);
                return 0;
            }
        }
    }

    if (fputc('\n', arquivo) == EOF) {
        fclose(arquivo);
        return 0;
    }

    return fclose(arquivo) == 0;
}

int salvarParametrosRSA(
    const char *caminho,
    const Params *params,
    const PublicKey *pub,
    const PrivKey *priv
) {
    if (
        caminho == NULL ||
        params == NULL ||
        pub == NULL ||
        priv == NULL
    ) {
        return 0;
    }

    FILE *arquivo = fopen(caminho, "w");

    if (arquivo == NULL) {
        return 0;
    }

    int erro = 0;

    if (
        fprintf(
            arquivo,
            "Parametro,Valor\n"
        ) < 0
    ) {
        erro = 1;
    }

    if (
        !erro &&
        fprintf(
            arquivo,
            "p,%" PRIu64 "\n",
            params->p
        ) < 0
    ) {
        erro = 1;
    }

    if (
        !erro &&
        fprintf(
            arquivo,
            "q,%" PRIu64 "\n",
            params->q
        ) < 0
    ) {
        erro = 1;
    }

    if (
        !erro &&
        fprintf(
            arquivo,
            "n,%" PRIu64 "\n",
            pub->n
        ) < 0
    ) {
        erro = 1;
    }

    if (
        !erro &&
        fprintf(
            arquivo,
            "phi,%" PRIu64 "\n",
            params->phi
        ) < 0
    ) {
        erro = 1;
    }

    if (
        !erro &&
        fprintf(
            arquivo,
            "e,%" PRIu64 "\n",
            pub->e
        ) < 0
    ) {
        erro = 1;
    }

    if (
        !erro &&
        fprintf(
            arquivo,
            "d,%" PRIu64 "\n",
            priv->d
        ) < 0
    ) {
        erro = 1;
    }

    if (
        !erro &&
        fprintf(
            arquivo,
            "dp,%" PRIu64 "\n",
            priv->dp
        ) < 0
    ) {
        erro = 1;
    }

    if (
        !erro &&
        fprintf(
            arquivo,
            "dq,%" PRIu64 "\n",
            priv->dq
        ) < 0
    ) {
        erro = 1;
    }

    if (
        !erro &&
        fprintf(
            arquivo,
            "q_inv,%" PRIu64 "\n",
            priv->q_inv
        ) < 0
    ) {
        erro = 1;
    }

    if (fclose(arquivo) != 0) {
        erro = 1;
    }

    return erro ? 0 : 1;
}

int lerParametrosRSA(
    const char *caminho,
    Params *params
) {
    if (
        caminho == NULL ||
        params == NULL
    ) {
        return 0;
    }

    FILE *arquivo = fopen(caminho, "r");

    if (arquivo == NULL) {
        return 0;
    }

    /*
     * Inicializa os campos antes da leitura.
     */
    params->p = 0;
    params->q = 0;
    params->e = 0;
    params->n = 0;
    params->phi = 0;

    char linha[128];

    /*
     * Ignora o cabeçalho:
     *
     * Parametro,Valor
     */
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        fclose(arquivo);
        return 0;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char nome[32];
        u64 valor;

        int resultado = sscanf(
            linha,
            " %31[^,],%" SCNu64,
            nome,
            &valor
        );

        if (resultado != 2) {
            fclose(arquivo);
            return 0;
        }

        if (strcmp(nome, "p") == 0) {
            params->p = valor;
        } else if (strcmp(nome, "q") == 0) {
            params->q = valor;
        } else if (strcmp(nome, "e") == 0) {
            params->e = valor;
        }
    }

    if (ferror(arquivo)) {
        fclose(arquivo);
        return 0;
    }

    if (fclose(arquivo) != 0) {
        return 0;
    }

    /*
     * Os três parâmetros essenciais precisam existir.
     */
    if (
        params->p == 0 ||
        params->q == 0 ||
        params->e == 0
    ) {
        return 0;
    }

    return 1;
}