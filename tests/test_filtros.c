#include <stdio.h>
#include <math.h>
#include "filtros.h"
#include "matriz.h"

static int total = 0;
static int falhas = 0;

#define CHECK(cond) do { \
    total++; \
    if (!(cond)) { \
        falhas++; \
        printf("  FALHOU: %s (linha %d)\n", #cond, __LINE__); \
    } \
} while (0)

/* ---------- auxiliares dos testes ---------- */

static int igual(double a, double b) {
    return fabs(a - b) < 1e-9;
}

static double valor(const Matriz *m, int i, int j) {
    double v = 0.0;
    mat_get(m, i, j, &v);
    return v;
}

/* matriz com 1, 2, 3, ... preenchida linha a linha */
static Matriz *sequencia(int linhas, int colunas) {
    Matriz *m = mat_criar(linhas, colunas);
    int n = 1;
    for (int i = 0; i < linhas; i++)
        for (int j = 0; j < colunas; j++)
            mat_set(m, i, j, n++);
    return m;
}

static Matriz *constante(int linhas, int colunas, double v) {
    Matriz *m = mat_criar(linhas, colunas);
    for (int i = 0; i < linhas; i++)
        for (int j = 0; j < colunas; j++)
            mat_set(m, i, j, v);
    return m;
}

/* kernel lado x lado com o mesmo valor em todas as posições */
static Matriz *kernel_uniforme(int lado, double peso) {
    return constante(lado, lado, peso);
}

/* ---------- fil_convolucao ---------- */

static void teste_convolucao(void) {
    printf("fil_convolucao\n");
    FilStatus st;
    Matriz *img = sequencia(3, 3);

    /* kernel identidade devolve a mesma matriz */
    Matriz *id = mat_criar(3, 3);
    mat_set(id, 1, 1, 1.0);
    st = FIL_SEM_MEMORIA;
    Matriz *r = fil_convolucao(img, id, &st);
    CHECK(r != NULL && st == FIL_SUCESSO);
    int iguais = 1;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (!igual(valor(r, i, j), valor(img, i, j))) iguais = 0;
    CHECK(iguais);
    mat_liberar(r);

    /* soma dos 9 vizinhos no centro: 1+...+9 = 45 */
    Matriz *soma = kernel_uniforme(3, 1.0);
    r = fil_convolucao(img, soma, &st);
    CHECK(r != NULL && igual(valor(r, 1, 1), 45.0));
    mat_liberar(r);
    mat_liberar(soma);

    /* kernel maior que a imagem: 1x1 de valor 7 com kernel 5x5 */
    Matriz *um = constante(1, 1, 7.0);
    Matriz *k5 = kernel_uniforme(5, 1.0 / 25.0);
    r = fil_convolucao(um, k5, &st);
    CHECK(r != NULL && st == FIL_SUCESSO && igual(valor(r, 0, 0), 7.0));
    mat_liberar(r);

    /* kernel com dimensão par */
    Matriz *par = kernel_uniforme(2, 1.0);
    st = FIL_SUCESSO;
    r = fil_convolucao(img, par, &st);
    CHECK(r == NULL && st == FIL_KERNEL_INVALIDO);

    /* ponteiros NULL */
    st = FIL_SUCESSO;
    CHECK(fil_convolucao(NULL, id, &st) == NULL && st == FIL_PONTEIRO_NULL);
    st = FIL_SUCESSO;
    CHECK(fil_convolucao(img, NULL, &st) == NULL && st == FIL_PONTEIRO_NULL);
    CHECK(fil_convolucao(NULL, NULL, NULL) == NULL);  /* status NULL é aceito */

    mat_liberar(img);
    mat_liberar(id);
    mat_liberar(um);
    mat_liberar(k5);
    mat_liberar(par);
}

/* ---------- fil_blur ---------- */

static void teste_blur(void) {
    printf("fil_blur\n");
    FilStatus st;
    Matriz *img = sequencia(3, 3);

    /* centro = média de 1..9 = 5; canto (0,0) com borda replicada = 21/9 */
    st = FIL_SEM_MEMORIA;
    Matriz *r = fil_blur(img, 1, &st);
    CHECK(r != NULL && st == FIL_SUCESSO);
    CHECK(igual(valor(r, 1, 1), 5.0));
    CHECK(igual(valor(r, 0, 0), 21.0 / 9.0));
    CHECK(igual(valor(r, 2, 2), 69.0 / 9.0));  /* canto oposto: 5,6,6,8,9,9,8,9,9 */
    mat_liberar(r);

    /* imagem constante continua constante */
    Matriz *c = constante(4, 5, 7.0);
    r = fil_blur(c, 2, &st);
    int ok = (r != NULL);
    for (int i = 0; ok && i < 4; i++)
        for (int j = 0; j < 5; j++)
            if (!igual(valor(r, i, j), 7.0)) ok = 0;
    CHECK(ok);
    mat_liberar(r);
    mat_liberar(c);

    /* raio 0 devolve cópia */
    r = fil_blur(img, 0, &st);
    CHECK(r != NULL && igual(valor(r, 2, 2), 9.0) && igual(valor(r, 0, 1), 2.0));
    mat_liberar(r);

    /* imagem 1x1 */
    Matriz *um = constante(1, 1, 7.0);
    r = fil_blur(um, 5, &st);
    CHECK(r != NULL && igual(valor(r, 0, 0), 7.0));
    mat_liberar(r);
    mat_liberar(um);

    /* a entrada não é modificada */
    r = fil_blur(img, 1, &st);
    CHECK(igual(valor(img, 0, 0), 1.0) && igual(valor(img, 1, 1), 5.0));
    mat_liberar(r);

    /* erros */
    st = FIL_SUCESSO;
    CHECK(fil_blur(img, -1, &st) == NULL && st == FIL_PARAMETRO_INVALIDO);
    st = FIL_SUCESSO;
    CHECK(fil_blur(NULL, 1, &st) == NULL && st == FIL_PONTEIRO_NULL);
    CHECK(fil_blur(NULL, 1, NULL) == NULL);

    mat_liberar(img);
}

/* ---------- fil_sharpen ---------- */

static void teste_sharpen(void) {
    printf("fil_sharpen\n");
    FilStatus st;
    Matriz *img = sequencia(3, 3);

    /* centro: 5*5 - (2+4+6+8) = 5; canto: 5*1 - (1+4+1+2) = -3 */
    st = FIL_SEM_MEMORIA;
    Matriz *r = fil_sharpen(img, &st);
    CHECK(r != NULL && st == FIL_SUCESSO);
    CHECK(igual(valor(r, 1, 1), 5.0));
    CHECK(igual(valor(r, 0, 0), -3.0));
    mat_liberar(r);

    /* imagem constante continua constante */
    Matriz *c = constante(3, 4, 7.0);
    r = fil_sharpen(c, &st);
    int ok = (r != NULL);
    for (int i = 0; ok && i < 3; i++)
        for (int j = 0; j < 4; j++)
            if (!igual(valor(r, i, j), 7.0)) ok = 0;
    CHECK(ok);
    mat_liberar(r);
    mat_liberar(c);

    st = FIL_SUCESSO;
    CHECK(fil_sharpen(NULL, &st) == NULL && st == FIL_PONTEIRO_NULL);
    CHECK(fil_sharpen(NULL, NULL) == NULL);

    mat_liberar(img);
}

/* ---------- fil_sobel ---------- */

static void teste_sobel(void) {
    printf("fil_sobel\n");
    FilStatus st;
    Matriz *img = sequencia(3, 3);

    /* centro: gx=8, gy=24 -> sqrt(640); canto: gx=4, gy=12 -> sqrt(160) */
    st = FIL_SEM_MEMORIA;
    Matriz *r = fil_sobel(img, &st);
    CHECK(r != NULL && st == FIL_SUCESSO);
    CHECK(igual(valor(r, 1, 1), sqrt(640.0)));
    CHECK(igual(valor(r, 0, 0), sqrt(160.0)));
    mat_liberar(r);

    /* imagem constante não tem bordas */
    Matriz *c = constante(4, 4, 7.0);
    r = fil_sobel(c, &st);
    int ok = (r != NULL);
    for (int i = 0; ok && i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (!igual(valor(r, i, j), 0.0)) ok = 0;
    CHECK(ok);
    mat_liberar(r);
    mat_liberar(c);

    /* imagem 1x1 */
    Matriz *um = constante(1, 1, 5.0);
    r = fil_sobel(um, &st);
    CHECK(r != NULL && igual(valor(r, 0, 0), 0.0));
    mat_liberar(r);
    mat_liberar(um);

    st = FIL_SUCESSO;
    CHECK(fil_sobel(NULL, &st) == NULL && st == FIL_PONTEIRO_NULL);
    CHECK(fil_sobel(NULL, NULL) == NULL);

    mat_liberar(img);
}

/* ---------- fil_limiar ---------- */

static void teste_limiar(void) {
    printf("fil_limiar\n");
    FilStatus st;
    Matriz *img = sequencia(3, 3);

    /* limiar 5: valores >= 5 viram 255, os outros 0 (5 == limiar vira 255) */
    st = FIL_SEM_MEMORIA;
    Matriz *r = fil_limiar(img, 5.0, &st);
    CHECK(r != NULL && st == FIL_SUCESSO);
    CHECK(igual(valor(r, 0, 0), 0.0));
    CHECK(igual(valor(r, 1, 0), 0.0));   /* 4 */
    CHECK(igual(valor(r, 1, 1), 255.0)); /* 5, igual ao limiar */
    CHECK(igual(valor(r, 2, 2), 255.0));
    mat_liberar(r);

    /* limiar muito baixo: tudo 255; muito alto: tudo 0 */
    r = fil_limiar(img, -1.0, &st);
    CHECK(r != NULL && igual(valor(r, 0, 0), 255.0));
    mat_liberar(r);
    r = fil_limiar(img, 1000.0, &st);
    CHECK(r != NULL && igual(valor(r, 2, 2), 0.0));
    mat_liberar(r);

    st = FIL_SUCESSO;
    CHECK(fil_limiar(NULL, 5.0, &st) == NULL && st == FIL_PONTEIRO_NULL);
    CHECK(fil_limiar(NULL, 5.0, NULL) == NULL);

    mat_liberar(img);
}

/* ---------- main ---------- */

int main(void) {
    teste_convolucao();
    teste_blur();
    teste_sharpen();
    teste_sobel();
    teste_limiar();

    printf("\n%d verificacoes, %d falhas\n", total, falhas);
    if (falhas == 0) {
        printf("RESULTADO: APROVADO\n");
        return 0;
    }
    printf("RESULTADO: FALHOU\n");
    return 1;
}
