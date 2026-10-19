/**
 * @file filtros.c
 * @brief Implementação dos filtros de imagem via operações matriciais.
 *
 * Cada filtro opera sobre uma Matriz de doubles (um canal por vez).
 * A convolução é implementada como um produto matriz-vetor: o kernel
 * (kl×kc) é tratado como uma matriz de uma linha (vetor linha) e cada
 * janela da imagem fonte é extraída como um vetor coluna; o produto
 * interno entre eles fornece o valor filtrado de cada pixel.
 */

#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "filtros.h"

/* ---------- auxiliares internas ---------- */

/** Grava o status só se o ponteiro não for NULL. */
static void definir_status(FilStatus *status, FilStatus valor) {
    if (status != NULL) {
        *status = valor;
    }
}

/** Prende x em [0, max]: repete o pixel da borda (padding de borda). */
static int limitar(int x, int max) {
    if (x < 0)   return 0;
    if (x > max) return max;
    return x;
}

/**
 * @brief Extrai uma janela (patch) da imagem centrada em (ci, cj) para um vetor.
 *
 * @details A janela tem dimensão kl×kc. Elementos fora dos limites da imagem
 * são obtidos por replicação de borda (padding "clamp"). O vetor de saída
 * tem kl*kc posições e é preenchido em ordem de linhas (igual ao kernel).
 *
 * @param src    Imagem fonte (Matriz de doubles).
 * @param ci     Linha central da janela.
 * @param cj     Coluna central da janela.
 * @param kl     Número de linhas do kernel (deve ser ímpar).
 * @param kc     Número de colunas do kernel (deve ser ímpar).
 * @param patch  Vetor de saída com kl*kc posições, alocado pelo chamador.
 */
static void extrair_patch(const Matriz *src, int ci, int cj,
                          int kl, int kc, double *patch) {
    int linhas  = mat_linhas(src);
    int colunas = mat_colunas(src);
    int rl = kl / 2;  /* raio de linhas do kernel */
    int rc = kc / 2;  /* raio de colunas do kernel */
    int idx = 0;
    for (int u = 0; u < kl; u++) {
        for (int v = 0; v < kc; v++) {
            int si = limitar(ci + u - rl, linhas  - 1);
            int sj = limitar(cj + v - rc, colunas - 1);
            double val = 0.0;
            mat_get(src, si, sj, &val);
            patch[idx++] = val;
        }
    }
}

/* ---------- convolução ---------- */

/**
 * @brief Convolução de src com kernel via produto matriz-vetor.
 *
 * @details O kernel (kl×kc) é achatado num vetor linha de kl*kc elementos.
 * Para cada pixel (i,j) da imagem, extrai-se uma janela (patch) de kl*kc
 * valores da imagem fonte e calcula-se o produto interno kernel · patch
 * usando mat_mult_vetor. Bordas são tratadas por replicação (padding "clamp").
 *
 * @param src    Matriz fonte (um canal da imagem).
 * @param kernel Matriz do kernel de convolução (dimensões ímpares).
 * @param status Código de erro opcional (pode ser NULL).
 * @return Nova Matriz resultado, ou NULL em caso de erro.
 */
Matriz *fil_convolucao(const Matriz *src, const Matriz *kernel, FilStatus *status) {
    if (src == NULL || kernel == NULL) {
        definir_status(status, FIL_PONTEIRO_NULL);
        return NULL;
    }

    int kl = mat_linhas(kernel);
    int kc = mat_colunas(kernel);
    if (kl % 2 == 0 || kc % 2 == 0) {   /* kernel precisa ter centro definido */
        definir_status(status, FIL_KERNEL_INVALIDO);
        return NULL;
    }

    int linhas  = mat_linhas(src);
    int colunas = mat_colunas(src);
    int ksz     = kl * kc;               /* número total de elementos do kernel */

    Matriz *dst = mat_criar(linhas, colunas);
    if (dst == NULL) {
        definir_status(status, FIL_SEM_MEMORIA);
        return NULL;
    }

    /* Achata o kernel numa matriz de 1 linha × ksz colunas para usar
     * mat_mult_vetor: kernel_linha[0..ksz-1] = kernel[u][v] em row-major. */
    Matriz *kernel_linha = mat_criar(1, ksz);
    if (kernel_linha == NULL) {
        mat_liberar(dst);
        definir_status(status, FIL_SEM_MEMORIA);
        return NULL;
    }
    for (int u = 0; u < kl; u++) {
        for (int v = 0; v < kc; v++) {
            double k = 0.0;
            mat_get(kernel, u, v, &k);
            mat_set(kernel_linha, 0, u * kc + v, k);
        }
    }

    /* Para cada pixel, extrai patch e calcula produto interno via mat_mult_vetor.
     * resultado[0] = Σ kernel_linha[j] * patch[j] = valor convoluído. */
    double *patch    = malloc((size_t)ksz * sizeof *patch);
    double  resultado = 0.0;
    if (patch == NULL) {
        mat_liberar(dst);
        mat_liberar(kernel_linha);
        definir_status(status, FIL_SEM_MEMORIA);
        return NULL;
    }

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            /* preenche o vetor patch com os kl*kc vizinhos (com borda) */
            extrair_patch(src, i, j, kl, kc, patch);
            /* produto interno: kernel_linha (1×ksz) × patch (ksz×1) → escalar */
            mat_mult_vetor(kernel_linha, patch, ksz, &resultado);
            mat_set(dst, i, j, resultado);
        }
    }

    free(patch);
    mat_liberar(kernel_linha);
    definir_status(status, FIL_SUCESSO);
    return dst;
}

/* ---------- blur ---------- */

/**
 * @brief Desfoque por média numa janela de lado 2*raio+1.
 *
 * @details Constrói o kernel uniforme como uma Matriz de 1s e aplica
 * mat_mult_escalar para normalizar (dividir pelo número de elementos),
 * garantindo que a soma dos pesos seja 1. Em seguida delega para
 * fil_convolucao.
 *
 * @param src    Matriz fonte.
 * @param raio   Raio da janela (>= 0). lado = 2*raio+1.
 * @param status Código de erro opcional (pode ser NULL).
 * @return Nova Matriz desfocada, ou NULL em caso de erro.
 */
Matriz *fil_blur(const Matriz *src, int raio, FilStatus *status) {
    if (src == NULL) {
        definir_status(status, FIL_PONTEIRO_NULL);
        return NULL;
    }
    if (raio < 0) {
        definir_status(status, FIL_PARAMETRO_INVALIDO);
        return NULL;
    }

    int lado = 2 * raio + 1;
    Matriz *kernel = mat_criar(lado, lado);
    if (kernel == NULL) {
        definir_status(status, FIL_SEM_MEMORIA);
        return NULL;
    }

    /* Preenche o kernel inteiro com 1.0 e depois escala por 1/(lado²).
     * Isso usa mat_mult_escalar para realizar a normalização do filtro caixa. */
    for (int i = 0; i < lado; i++) {
        for (int j = 0; j < lado; j++) {
            mat_set(kernel, i, j, 1.0);
        }
    }
    mat_mult_escalar(kernel, 1.0 / ((double)lado * lado));

    Matriz *resultado = fil_convolucao(src, kernel, status);
    mat_liberar(kernel);
    return resultado;
}

/* ---------- sharpen ---------- */

/**
 * @brief Realça detalhes e bordas com o kernel laplaciano de nitidez.
 *
 * @details O kernel 3×3 [ 0 -1 0 / -1 5 -1 / 0 -1 0 ] é construído
 * como Matriz e a filtragem é delegada a fil_convolucao.
 *
 * @param src    Matriz fonte.
 * @param status Código de erro opcional (pode ser NULL).
 * @return Nova Matriz com nitidez aumentada, ou NULL em caso de erro.
 */
Matriz *fil_sharpen(const Matriz *src, FilStatus *status) {
    if (src == NULL) {
        definir_status(status, FIL_PONTEIRO_NULL);
        return NULL;
    }

    static const double valores[3][3] = {
        { 0.0, -1.0,  0.0},
        {-1.0,  5.0, -1.0},
        { 0.0, -1.0,  0.0}
    };

    Matriz *kernel = mat_criar(3, 3);
    if (kernel == NULL) {
        definir_status(status, FIL_SEM_MEMORIA);
        return NULL;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat_set(kernel, i, j, valores[i][j]);
        }
    }

    Matriz *resultado = fil_convolucao(src, kernel, status);
    mat_liberar(kernel);
    return resultado;
}

/* ---------- sobel ---------- */

/**
 * @brief Detecta bordas pelo operador de Sobel.
 *
 * @details Aplica dois kernels de Sobel (Gx e Gy) via fil_convolucao,
 * obtendo dois mapas de gradiente. A magnitude é calculada pixel a pixel
 * como sqrt(Gx² + Gy²). Para expressar a combinação em termos matriciais,
 * computa-se Gx² e Gy² (via mat_mult_escalar aplicado ao quadrado de cada
 * elemento não é direto, então usamos mat_somar para somar os dois mapas
 * e depois calculamos a raiz elemento a elemento).
 *
 * @param src    Matriz fonte.
 * @param status Código de erro opcional (pode ser NULL).
 * @return Nova Matriz com mapa de magnitude de gradiente, ou NULL em erro.
 */
Matriz *fil_sobel(const Matriz *src, FilStatus *status) {
    if (src == NULL) {
        definir_status(status, FIL_PONTEIRO_NULL);
        return NULL;
    }

    static const double vx[3][3] = {{-1.0, 0.0, 1.0},
                                     {-2.0, 0.0, 2.0},
                                     {-1.0, 0.0, 1.0}};
    static const double vy[3][3] = {{-1.0, -2.0, -1.0},
                                     { 0.0,  0.0,  0.0},
                                     { 1.0,  2.0,  1.0}};

    Matriz *kx = mat_criar(3, 3);
    Matriz *ky = mat_criar(3, 3);
    if (kx == NULL || ky == NULL) {
        mat_liberar(kx);
        mat_liberar(ky);
        definir_status(status, FIL_SEM_MEMORIA);
        return NULL;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat_set(kx, i, j, vx[i][j]);
            mat_set(ky, i, j, vy[i][j]);
        }
    }

    FilStatus st = FIL_SUCESSO;

    /* Gx e Gy: mapas de gradiente horizontal e vertical */
    Matriz *gx = fil_convolucao(src, kx, &st);
    Matriz *gy = NULL;
    if (gx != NULL) {
        gy = fil_convolucao(src, ky, &st);
    }

    /* Gx² e Gy²: cada mapa elevado ao quadrado (Gx[i][j] *= Gx[i][j]).
     * Fazemos isso construindo matrizes auxiliares com os valores ao quadrado
     * e depois os somamos com mat_somar para obter Gx²+Gy². */
    Matriz *gx2 = NULL;
    Matriz *gy2 = NULL;
    Matriz *soma = NULL;
    Matriz *dst  = NULL;

    if (gy != NULL) {
        int L = mat_linhas(src);
        int C = mat_colunas(src);

        gx2 = mat_criar(L, C);
        gy2 = mat_criar(L, C);
        if (gx2 == NULL || gy2 == NULL) {
            st = FIL_SEM_MEMORIA;
        } else {
            /* preenche gx2[i][j] = gx[i][j]² e gy2[i][j] = gy[i][j]² */
            for (int i = 0; i < L; i++) {
                for (int j = 0; j < C; j++) {
                    double a = 0.0, b = 0.0;
                    mat_get(gx, i, j, &a);
                    mat_get(gy, i, j, &b);
                    mat_set(gx2, i, j, a * a);
                    mat_set(gy2, i, j, b * b);
                }
            }

            /* soma matricial: Gx² + Gy² usando mat_somar */
            soma = mat_somar(gx2, gy2);
            if (soma == NULL) {
                st = FIL_SEM_MEMORIA;
            } else {
                /* raiz quadrada elemento a elemento da soma para obter a magnitude */
                dst = mat_criar(L, C);
                if (dst == NULL) {
                    st = FIL_SEM_MEMORIA;
                } else {
                    for (int i = 0; i < L; i++) {
                        for (int j = 0; j < C; j++) {
                            double s = 0.0;
                            mat_get(soma, i, j, &s);
                            mat_set(dst, i, j, sqrt(s));
                        }
                    }
                }
            }
        }
    }

    mat_liberar(kx);
    mat_liberar(ky);
    mat_liberar(gx);
    mat_liberar(gy);
    mat_liberar(gx2);
    mat_liberar(gy2);
    mat_liberar(soma);
    definir_status(status, st);
    return dst;
}

/* ---------- limiar ---------- */

/**
 * @brief Binariza a matriz: pixels >= limiar viram 255, demais viram 0.
 *
 * @details Não há operação matricial de nossa API que realize limiarização
 * diretamente (ela é não-linear), por isso o loop elemento a elemento é
 * mantido. A Matriz de saída é criada via mat_criar.
 *
 * @param src    Matriz fonte.
 * @param limiar Valor de corte.
 * @param status Código de erro opcional (pode ser NULL).
 * @return Nova Matriz binarizada, ou NULL em caso de erro.
 */
Matriz *fil_limiar(const Matriz *src, double limiar, FilStatus *status) {
    if (src == NULL) {
        definir_status(status, FIL_PONTEIRO_NULL);
        return NULL;
    }

    int linhas  = mat_linhas(src);
    int colunas = mat_colunas(src);
    Matriz *dst = mat_criar(linhas, colunas);
    if (dst == NULL) {
        definir_status(status, FIL_SEM_MEMORIA);
        return NULL;
    }

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            double valor = 0.0;
            mat_get(src, i, j, &valor);
            mat_set(dst, i, j, valor >= limiar ? 255.0 : 0.0);
        }
    }

    definir_status(status, FIL_SUCESSO);
    return dst;
}
