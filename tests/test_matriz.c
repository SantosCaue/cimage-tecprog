#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

/* ---------- testes unitários ---------- */

static void teste_criar_liberar_consultas(void) {
    printf("mat_criar, mat_liberar e consultas\n");

    Matriz *m = mat_criar(3, 4);
    CHECK(m != NULL);
    CHECK(mat_linhas(m) == 3);
    CHECK(mat_colunas(m) == 4);

    /* Todos os elementos devem iniciar zerados */
    int todos_zeros = 1;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            if (!igual(valor(m, i, j), 0.0)) {
                todos_zeros = 0;
            }
        }
    }
    CHECK(todos_zeros);
    mat_liberar(m);

    /* Dimensões inválidas */
    CHECK(mat_criar(0, 5) == NULL);
    CHECK(mat_criar(5, 0) == NULL);
    CHECK(mat_criar(-1, 5) == NULL);
    CHECK(mat_criar(5, -2) == NULL);

    /* Consultas com matriz NULL */
    CHECK(mat_linhas(NULL) == 0);
    CHECK(mat_colunas(NULL) == 0);

    /* Liberar NULL não deve falhar */
    mat_liberar(NULL);
}

static void teste_get_set(void) {
    printf("mat_get e mat_set\n");

    Matriz *m = mat_criar(2, 3);
    CHECK(m != NULL);

    /* Gravação e leitura de valores válidos */
    CHECK(mat_set(m, 0, 0, 1.5) == MAT_SUCESSO);
    CHECK(mat_set(m, 0, 2, -3.25) == MAT_SUCESSO);
    CHECK(mat_set(m, 1, 1, 100.0) == MAT_SUCESSO);

    double v = 0.0;
    CHECK(mat_get(m, 0, 0, &v) == MAT_SUCESSO && igual(v, 1.5));
    CHECK(mat_get(m, 0, 2, &v) == MAT_SUCESSO && igual(v, -3.25));
    CHECK(mat_get(m, 1, 1, &v) == MAT_SUCESSO && igual(v, 100.0));

    /* Limites fora da matriz */
    CHECK(mat_set(m, -1, 0, 1.0) == MAT_FORA_DA_MATRIZ);
    CHECK(mat_set(m, 0, -1, 1.0) == MAT_FORA_DA_MATRIZ);
    CHECK(mat_set(m, 2, 0, 1.0) == MAT_FORA_DA_MATRIZ);
    CHECK(mat_set(m, 0, 3, 1.0) == MAT_FORA_DA_MATRIZ);

    v = 999.0;
    CHECK(mat_get(m, 2, 1, &v) == MAT_FORA_DA_MATRIZ);
    CHECK(igual(v, 999.0)); /* Não deve alterar valor em caso de erro */

    CHECK(mat_get(m, -1, 0, &v) == MAT_FORA_DA_MATRIZ);
    CHECK(mat_get(m, 0, 3, &v) == MAT_FORA_DA_MATRIZ);

    /* Ponteiros NULL */
    CHECK(mat_set(NULL, 0, 0, 1.0) == MAT_PONTEIRO_NULL);
    CHECK(mat_get(NULL, 0, 0, &v) == MAT_PONTEIRO_NULL);
    CHECK(mat_get(m, 0, 0, NULL) == MAT_PONTEIRO_NULL);

    mat_liberar(m);
}

static void teste_mult_escalar(void) {
    printf("mat_mult_escalar\n");

    Matriz *m = mat_criar(2, 2);
    mat_set(m, 0, 0, 1.0);
    mat_set(m, 0, 1, -2.0);
    mat_set(m, 1, 0, 3.5);
    mat_set(m, 1, 1, 0.0);

    /* Multiplicação por escalar positivo */
    CHECK(mat_mult_escalar(m, 2.0) == MAT_SUCESSO);
    CHECK(igual(valor(m, 0, 0), 2.0));
    CHECK(igual(valor(m, 0, 1), -4.0));
    CHECK(igual(valor(m, 1, 0), 7.0));
    CHECK(igual(valor(m, 1, 1), 0.0));

    /* Multiplicação por 0.0 */
    CHECK(mat_mult_escalar(m, 0.0) == MAT_SUCESSO);
    CHECK(igual(valor(m, 0, 0), 0.0));
    CHECK(igual(valor(m, 0, 1), 0.0));
    CHECK(igual(valor(m, 1, 0), 0.0));
    CHECK(igual(valor(m, 1, 1), 0.0));

    /* Ponteiro NULL */
    CHECK(mat_mult_escalar(NULL, 5.0) == MAT_PONTEIRO_NULL);

    mat_liberar(m);
}

static void teste_mult_vetor(void) {
    printf("mat_mult_vetor\n");

    /* Matriz M (2x3):
     * [ 1.0, 2.0, 3.0 ]
     * [ 4.0, 5.0, 6.0 ]
     * Vetor v (3): [ 1.0, 2.0, 3.0 ]
     * M x v = [ 1*1 + 2*2 + 3*3, 4*1 + 5*2 + 6*3 ] = [ 14.0, 32.0 ]
     */
    Matriz *m = mat_criar(2, 3);
    mat_set(m, 0, 0, 1.0);
    mat_set(m, 0, 1, 2.0);
    mat_set(m, 0, 2, 3.0);
    mat_set(m, 1, 0, 4.0);
    mat_set(m, 1, 1, 5.0);
    mat_set(m, 1, 2, 6.0);

    double v[3] = {1.0, 2.0, 3.0};
    double res[2] = {0.0, 0.0};

    CHECK(mat_mult_vetor(m, v, 3, res) == MAT_SUCESSO);
    CHECK(igual(res[0], 14.0));
    CHECK(igual(res[1], 32.0));

    /* Dimensão incompatível */
    CHECK(mat_mult_vetor(m, v, 2, res) == MAT_DIMENSAO_INVALIDA);
    CHECK(mat_mult_vetor(m, v, 4, res) == MAT_DIMENSAO_INVALIDA);

    /* Ponteiros NULL */
    CHECK(mat_mult_vetor(NULL, v, 3, res) == MAT_PONTEIRO_NULL);
    CHECK(mat_mult_vetor(m, NULL, 3, res) == MAT_PONTEIRO_NULL);
    CHECK(mat_mult_vetor(m, v, 3, NULL) == MAT_PONTEIRO_NULL);

    mat_liberar(m);
}

static void teste_mult_matriz(void) {
    printf("mat_mult_matriz\n");

    /* A (2x3) x B (3x2):
     * A = [ 1 2 3 ]
     *     [ 4 5 6 ]
     * B = [ 7 8 ]
     *     [ 9 1 ]
     *     [ 2 3 ]
     * A x B (2x2):
     * [ 1*7 + 2*9 + 3*2 , 1*8 + 2*1 + 3*3 ] = [ 31, 19 ]
     * [ 4*7 + 5*9 + 6*2 , 4*8 + 5*1 + 6*3 ] = [ 85, 55 ]
     */
    Matriz *a = mat_criar(2, 3);
    mat_set(a, 0, 0, 1.0); mat_set(a, 0, 1, 2.0); mat_set(a, 0, 2, 3.0);
    mat_set(a, 1, 0, 4.0); mat_set(a, 1, 1, 5.0); mat_set(a, 1, 2, 6.0);

    Matriz *b = mat_criar(3, 2);
    mat_set(b, 0, 0, 7.0); mat_set(b, 0, 1, 8.0);
    mat_set(b, 1, 0, 9.0); mat_set(b, 1, 1, 1.0);
    mat_set(b, 2, 0, 2.0); mat_set(b, 2, 1, 3.0);

    Matriz *c = mat_mult_matriz(a, b);
    CHECK(c != NULL);
    CHECK(mat_linhas(c) == 2);
    CHECK(mat_colunas(c) == 2);
    CHECK(igual(valor(c, 0, 0), 31.0));
    CHECK(igual(valor(c, 0, 1), 19.0));
    CHECK(igual(valor(c, 1, 0), 85.0));
    CHECK(igual(valor(c, 1, 1), 55.0));
    mat_liberar(c);

    /* Dimensões incompatíveis: (2x3) x (2x3) */
    Matriz *incompativel = mat_criar(2, 3);
    CHECK(mat_mult_matriz(a, incompativel) == NULL);
    mat_liberar(incompativel);

    /* Ponteiros NULL */
    CHECK(mat_mult_matriz(NULL, b) == NULL);
    CHECK(mat_mult_matriz(a, NULL) == NULL);

    mat_liberar(a);
    mat_liberar(b);
}

static void teste_somar(void) {
    printf("mat_somar\n");

    Matriz *a = mat_criar(2, 2);
    mat_set(a, 0, 0, 1.0); mat_set(a, 0, 1, 2.0);
    mat_set(a, 1, 0, 3.0); mat_set(a, 1, 1, 4.0);

    Matriz *b = mat_criar(2, 2);
    mat_set(b, 0, 0, 10.0); mat_set(b, 0, 1, 20.0);
    mat_set(b, 1, 0, 30.0); mat_set(b, 1, 1, 40.0);

    Matriz *s = mat_somar(a, b);
    CHECK(s != NULL);
    CHECK(mat_linhas(s) == 2);
    CHECK(mat_colunas(s) == 2);
    CHECK(igual(valor(s, 0, 0), 11.0));
    CHECK(igual(valor(s, 0, 1), 22.0));
    CHECK(igual(valor(s, 1, 0), 33.0));
    CHECK(igual(valor(s, 1, 1), 44.0));
    mat_liberar(s);

    /* Dimensões diferentes */
    Matriz *dif = mat_criar(2, 3);
    CHECK(mat_somar(a, dif) == NULL);
    mat_liberar(dif);

    /* Ponteiros NULL */
    CHECK(mat_somar(NULL, b) == NULL);
    CHECK(mat_somar(a, NULL) == NULL);

    mat_liberar(a);
    mat_liberar(b);
}

static void teste_transpor(void) {
    printf("mat_transpor\n");

    /* Matriz M (2x3):
     * [ 1 2 3 ]
     * [ 4 5 6 ]
     * M^T (3x2):
     * [ 1 4 ]
     * [ 2 5 ]
     * [ 3 6 ]
     */
    Matriz *m = mat_criar(2, 3);
    mat_set(m, 0, 0, 1.0); mat_set(m, 0, 1, 2.0); mat_set(m, 0, 2, 3.0);
    mat_set(m, 1, 0, 4.0); mat_set(m, 1, 1, 5.0); mat_set(m, 1, 2, 6.0);

    Matriz *t = mat_transpor(m);
    CHECK(t != NULL);
    CHECK(mat_linhas(t) == 3);
    CHECK(mat_colunas(t) == 2);
    CHECK(igual(valor(t, 0, 0), 1.0));
    CHECK(igual(valor(t, 0, 1), 4.0));
    CHECK(igual(valor(t, 1, 0), 2.0));
    CHECK(igual(valor(t, 1, 1), 5.0));
    CHECK(igual(valor(t, 2, 0), 3.0));
    CHECK(igual(valor(t, 2, 1), 6.0));
    mat_liberar(t);

    /* Transposta de matriz 1x1 */
    Matriz *um = mat_criar(1, 1);
    mat_set(um, 0, 0, 42.0);
    Matriz *um_t = mat_transpor(um);
    CHECK(um_t != NULL && igual(valor(um_t, 0, 0), 42.0));
    mat_liberar(um_t);
    mat_liberar(um);

    /* Ponteiro NULL */
    CHECK(mat_transpor(NULL) == NULL);

    mat_liberar(m);
}

/* ---------- main ---------- */

int main(void) {
    teste_criar_liberar_consultas();
    teste_get_set();
    teste_mult_escalar();
    teste_mult_vetor();
    teste_mult_matriz();
    teste_somar();
    teste_transpor();

    printf("\n%d verificacoes, %d falhas\n", total, falhas);
    if (falhas == 0) {
        printf("RESULTADO: APROVADO\n");
        return 0;
    }
    printf("RESULTADO: FALHOU\n");
    return 1;
}
