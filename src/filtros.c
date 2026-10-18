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

/* ---------- blur ---------- */

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

    double peso = 1.0 / ((double)lado * lado);   /* soma dos pesos = 1 */
    for (int i = 0; i < lado; i++) {
        for (int j = 0; j < lado; j++) {
            mat_set(kernel, i, j, peso);
        }
    }

    Matriz *resultado = fil_convolucao(src, kernel, status);
    mat_liberar(kernel);
    return resultado;
}

/* ---------- sharpen ---------- */

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

Matriz *fil_sobel(const Matriz *src, FilStatus *status) {
    if (src == NULL) {
        definir_status(status, FIL_PONTEIRO_NULL);
        return NULL;
    }

    static const double vx[3][3] = {{-1.0, 0.0, 1.0}, {-2.0, 0.0, 2.0}, {-1.0, 0.0, 1.0}};
    static const double vy[3][3] = {{-1.0, -2.0, -1.0}, {0.0, 0.0, 0.0}, {1.0, 2.0, 1.0}};

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

    /* uma única saída: cada etapa só roda se a anterior deu certo,
     * e a limpeza no final libera tudo que foi criado */
    FilStatus st = FIL_SUCESSO;
    Matriz *gx = fil_convolucao(src, kx, &st);
    Matriz *gy = NULL;
    Matriz *dst = NULL;
    if (gx != NULL) {
        gy = fil_convolucao(src, ky, &st);
    }
    if (gy != NULL) {
        dst = mat_criar(mat_linhas(src), mat_colunas(src));
        if (dst == NULL) {
            st = FIL_SEM_MEMORIA;
        }
    }
    if (dst != NULL) {
        for (int i = 0; i < mat_linhas(src); i++) {
            for (int j = 0; j < mat_colunas(src); j++) {
                double a, b;
                mat_get(gx, i, j, &a);
                mat_get(gy, i, j, &b);
                mat_set(dst, i, j, sqrt(a * a + b * b));
            }
        }
    }

    mat_liberar(kx);
    mat_liberar(ky);
    mat_liberar(gx);
    mat_liberar(gy);
    definir_status(status, st);
    return dst;
}

/* ---------- limiar ---------- */

Matriz *fil_limiar(const Matriz *src, double limiar, FilStatus *status) {
    if (src == NULL) {
        definir_status(status, FIL_PONTEIRO_NULL);
        return NULL;
    }

    int linhas = mat_linhas(src);
    int colunas = mat_colunas(src);
    Matriz *dst = mat_criar(linhas, colunas);
    if (dst == NULL) {
        definir_status(status, FIL_SEM_MEMORIA);
        return NULL;
    }

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            double valor;
            mat_get(src, i, j, &valor);
            mat_set(dst, i, j, valor >= limiar ? 255.0 : 0.0);
        }
    }

    definir_status(status, FIL_SUCESSO);
    return dst;
}

