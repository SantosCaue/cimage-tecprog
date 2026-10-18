#include <stddef.h>
#include <math.h>
#include "filtros.h"

/* ---------- auxiliares internas ---------- */

/* Grava o status só se o ponteiro não for NULL. */
static void definir_status(FilStatus *status, FilStatus valor) {
    if (status != NULL) {
        *status = valor;
    }
}

/* Prende x em [0, max]: repete o pixel da borda. */
static int limitar(int x, int max) {
    if (x < 0) return 0;
    if (x > max) return max;
    return x;
}

/* ---------- convolução ---------- */

Matriz *fil_convolucao(const Matriz *src, const Matriz *kernel, FilStatus *status) {
    if (src == NULL || kernel == NULL) {
        definir_status(status, FIL_PONTEIRO_NULL);
        return NULL;
    }

    int kl = mat_linhas(kernel);
    int kc = mat_colunas(kernel);
    if (kl % 2 == 0 || kc % 2 == 0) {      /* kernel precisa de centro */
        definir_status(status, FIL_KERNEL_INVALIDO);
        return NULL;
    }

    int linhas = mat_linhas(src);
    int colunas = mat_colunas(src);
    Matriz *dst = mat_criar(linhas, colunas);
    if (dst == NULL) {
        definir_status(status, FIL_SEM_MEMORIA);
        return NULL;
    }

    int cl = kl / 2;                       /* centro do kernel */
    int cc = kc / 2;

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            double acum = 0.0;
            for (int u = 0; u < kl; u++) {
                for (int v = 0; v < kc; v++) {
                    /* vizinho correspondente (kernel espelhado), preso à borda */
                    int si = limitar(i + cl - u, linhas - 1);
                    int sj = limitar(j + cc - v, colunas - 1);
                    double k, s;
                    mat_get(kernel, u, v, &k);   /* índices sempre válidos */
                    mat_get(src, si, sj, &s);
                    acum += k * s;
                }
            }
            mat_set(dst, i, j, acum);
        }
    }

    definir_status(status, FIL_SUCESSO);
    return dst;
}
