#include <stdint.h>
#include <string.h>
#include "rsa.h"

typedef __uint128_t u128;

/*
 * Multiplicação modular segura.
 *
 * Função interna ao rsa.c, por isso é static.
 */
static inline u64 mulmod(u64 a, u64 b, u64 modulo) {
    return (u64)(((u128)a * b) % modulo);
}

/*
 * Exponenciação modular rápida.
 *
 * Calcula:
 * base^expoente mod modulo
 */
u64 expmod(u64 base, u64 expoente, u64 modulo) {
    if (modulo == 0 || modulo == 1) {
        return 0;
    }

    u64 resultado = 1;
    base %= modulo;

    while (expoente > 0) {
        /*
         * Se o expoente for ímpar.
         */
        if (expoente & 1ULL) {
            resultado = mulmod(resultado, base, modulo);
        }

        expoente >>= 1;

        /*
         * Evita calcular um último quadrado desnecessário.
         */
        if (expoente > 0) {
            base = mulmod(base, base, modulo);
        }
    }

    return resultado;
}

/*
 * Algoritmo de Euclides.
 */
u64 mdc(u64 a, u64 b) {
    while (b != 0) {
        u64 resto = a % b;
        a = b;
        b = resto;
    }

    return a;
}

/*
 * Algoritmo de Euclides estendido.
 *
 * Função auxiliar usada para calcular o inverso modular.
 */
static int64_t mdc_estendido(
    int64_t a,
    int64_t b,
    int64_t *x,
    int64_t *y
) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }

    int64_t x1;
    int64_t y1;

    int64_t divisor = mdc_estendido(
        b,
        a % b,
        &x1,
        &y1
    );

    *x = y1;
    *y = x1 - (a / b) * y1;

    return divisor;
}

/*
 * Calcula:
 *
 * a^(-1) mod modulo
 */
u64 invmod(u64 a, u64 modulo) {
    if (modulo == 0) {
        return 0;
    }

    int64_t x;
    int64_t y;

    int64_t divisor = mdc_estendido(
        (int64_t)a,
        (int64_t)modulo,
        &x,
        &y
    );

    if (divisor != 1) {
        return 0;
    }

    int64_t inverso = x % (int64_t)modulo;

    if (inverso < 0) {
        inverso += (int64_t)modulo;
    }

    return (u64)inverso;
}

/*
 * Calcula e valida os parâmetros RSA.
 */
int genParams(Params *pa) {
    if (pa == NULL) {
        return 0;
    }

    if (pa->p < 2 || pa->q < 2) {
        return 0;
    }

    if (pa->p == pa->q) {
        return 0;
    }

    pa->n = pa->p * pa->q;
    pa->phi = (pa->p - 1) * (pa->q - 1);

    /*
     * O expoente público deve satisfazer:
     *
     * 1 < e < phi
     * mdc(e, phi) = 1
     */
    if (pa->e <= 1 || pa->e >= pa->phi) {
        return 0;
    }

    if (mdc(pa->e, pa->phi) != 1) {
        return 0;
    }

    return 1;
}

/*
 * Gera a chave pública.
 */
PublicKey genPubk(const Params *pa) {
    PublicKey pub = {0};

    if (pa == NULL) {
        return pub;
    }

    pub.n = pa->n;
    pub.e = pa->e;

    return pub;
}

/*
 * Gera a chave privada e pré-calcula os parâmetros do CRT.
 */
PrivKey genPrivk(const Params *pa) {
    PrivKey priv = {0};

    if (pa == NULL) {
        return priv;
    }

    /*
     * d é o inverso de e módulo phi.
     */
    u64 d = invmod(pa->e, pa->phi);

    if (d == 0) {
        return priv;
    }

    priv.n = pa->n;
    priv.d = d;

    priv.p = pa->p;
    priv.q = pa->q;

    /*
     * Redução dos expoentes usando Fermat.
     */
    priv.dp = d % (pa->p - 1);
    priv.dq = d % (pa->q - 1);

    /*
     * q_inv = q^(-1) mod p
     */
    priv.q_inv = invmod(pa->q, pa->p);

    return priv;
}

/*
 * Criptografia RSA.
 *
 * cifra = mensagem^e mod n
 */
u64 criptar(u64 mensagem, const PublicKey *pub) {
    if (pub == NULL) {
        return 0;
    }

    if (mensagem >= pub->n) {
        return 0;
    }

    return expmod(
        mensagem,
        pub->e,
        pub->n
    );
}

/*
 * Descriptografia RSA usando CRT.
 */
u64 decriptar(u64 cifra, const PrivKey *priv) {
    if (priv == NULL) {
        return 0;
    }

    if (
        priv->n == 0 ||
        priv->p == 0 ||
        priv->q == 0 ||
        priv->q_inv == 0
    ) {
        return 0;
    }

    /*
     * m1 = cifra^dp mod p
     * m2 = cifra^dq mod q
     */
    u64 m1 = expmod(
        cifra,
        priv->dp,
        priv->p
    );

    u64 m2 = expmod(
        cifra,
        priv->dq,
        priv->q
    );

    /*
     * diferenca = (m1 - m2) mod p
     */
    u64 m2_mod_p = m2 % priv->p;
    u64 diferenca;

    if (m1 >= m2_mod_p) {
        diferenca = m1 - m2_mod_p;
    } else {
        diferenca = priv->p - (m2_mod_p - m1);
    }

    /*
     * h = q_inv * diferenca mod p
     */
    u64 h = mulmod(
        priv->q_inv,
        diferenca,
        priv->p
    );

    /*
     * Reconstrução pelo Teorema Chinês do Resto:
     *
     * mensagem = m2 + q * h
     */
    u128 mensagem = (u128)m2 + (u128)priv->q * h;

    return (u64)(mensagem % priv->n);
}

/*
 * Criptografa uma string byte a byte.
 */
int cifrarTexto(
    const char *msg,
    u64 *cifra,
    size_t capacidade,
    const PublicKey *pub
) {
    if (msg == NULL || cifra == NULL || pub == NULL) {
        return -1;
    }

    size_t tamanho = strlen(msg);

    if (tamanho > capacidade) {
        return -1;
    }

    for (size_t i = 0; i < tamanho; i++) {
        /*
         * O cast para unsigned char evita valores negativos.
         */
        u64 byte = (unsigned char)msg[i];

        if (byte >= pub->n) {
            return -1;
        }

        cifra[i] = criptar(byte, pub);
    }

    return (int)tamanho;
}

/*
 * Descriptografa os blocos e reconstrói a string.
 */
int decifrarTexto(
    const u64 *cifra,
    char *decifra,
    size_t tamanho,
    size_t capacidade,
    const PrivKey *priv
) {
    if (
        cifra == NULL ||
        decifra == NULL ||
        priv == NULL
    ) {
        return 0;
    }

    /*
     * Precisa haver espaço para o terminador '\0'.
     */
    if (capacidade <= tamanho) {
        return 0;
    }

    for (size_t i = 0; i < tamanho; i++) {
        u64 byte = decriptar(cifra[i], priv);

        if (byte > 255) {
            return 0;
        }

        decifra[i] = (char)(unsigned char)byte;
    }

    decifra[tamanho] = '\0';

    return 1;
}