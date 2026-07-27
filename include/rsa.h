#ifndef RSA_H
#define RSA_H

#include <stddef.h>
#include <stdint.h>

typedef uint64_t u64;

/*
 * Chave pública:
 *
 * n = p * q
 * e = expoente público
 */
typedef struct {
    u64 n;
    u64 e;
} PublicKey;

/*
 * Chave privada com parâmetros pré-calculados para CRT.
 *
 * d     = expoente privado
 * dp    = d mod (p - 1)
 * dq    = d mod (q - 1)
 * q_inv = inverso de q mod p
 */
typedef struct {
    u64 n;
    u64 d;

    u64 p;
    u64 q;

    u64 dp;
    u64 dq;
    u64 q_inv;
} PrivKey;

/*
 * Parâmetros usados durante a geração das chaves.
 */
typedef struct {
    u64 p;
    u64 q;
    u64 e;

    u64 n;
    u64 phi;
} Params;

/*
 * Calcula:
 *
 * base^expoente mod modulo
 *
 * usando exponenciação modular rápida.
 */
u64 expmod(u64 base, u64 expoente, u64 modulo);

/*
 * Máximo divisor comum.
 */
u64 mdc(u64 a, u64 b);

/*
 * Calcula o inverso modular:
 *
 * a^(-1) mod modulo
 *
 * Retorna 0 caso o inverso não exista.
 */
u64 invmod(u64 a, u64 modulo);

/*
 * Inicializa e valida os parâmetros RSA.
 *
 * Calcula:
 *
 * n   = p * q
 * phi = (p - 1) * (q - 1)
 *
 * Retorna:
 *  1 em caso de sucesso
 *  0 em caso de erro
 */
int genParams(Params *pa);

/*
 * Gera a chave pública.
 */
PublicKey genPubk(const Params *pa);

/*
 * Gera a chave privada e os parâmetros do CRT.
 */
PrivKey genPrivk(const Params *pa);

/*
 * Encripta um bloco numérico:
 *
 * cifra = mensagem^e mod n
 */
u64 criptar(u64 mensagem, const PublicKey *pub);

/*
 * Descriptografa usando o Teorema Chinês do Resto:
 *
 * mensagem = cifra^d mod n
 */
u64 decriptar(u64 cifra, const PrivKey *priv);

/*
 * Converte e encripta cada byte da string.
 *
 * Retorna a quantidade de elementos encriptados.
 * Retorna -1 em caso de erro.
 */
int cifrarTexto(
    const char *msg,
    u64 *cifra,
    size_t capacidade,
    const PublicKey *pub
);

/*
 * Descriptografa os elementos e reconstrói a string.
 *
 * Retorna:
 *  1 em caso de sucesso
 *  0 em caso de erro
 */
int decifrarTexto(
    const u64 *cifra,
    char *decifra,
    size_t tamanho,
    size_t capacidade,
    const PrivKey *priv
);

#endif