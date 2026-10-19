#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "image.h"
#include "matriz.h"
#include "image_convert.h"

static int total = 0;
static int falhas = 0;

#define CHECK(cond) do { \
    total++; \
    if (!(cond)) { \
        falhas++; \
        printf("  FALHOU: %s (linha %d)\n", #cond, __LINE__); \
    } \
} while (0)

static int igual(double a, double b) {
    return fabs(a - b) < 1e-9;
}

static double mat_val(const Matriz *m, int i, int j) {
    double v = 0.0;
    mat_get(m, i, j, &v);
    return v;
}

/* ---------- testes unitários ---------- */

static void teste_img_para_matriz_cinza(void) {
    printf("img_para_matriz (tons de cinza)\n");

    /* Imagem 2x3 com 1 canal */
    Image *img = img_criar_imagem(3, 2, 1);
    CHECK(img != NULL);

    unsigned char p[1];
    p[0] = 10; img_escrever_pixel(img, 0, 0, p);
    p[0] = 20; img_escrever_pixel(img, 0, 1, p);
    p[0] = 30; img_escrever_pixel(img, 0, 2, p);
    p[0] = 40; img_escrever_pixel(img, 1, 0, p);
    p[0] = 50; img_escrever_pixel(img, 1, 1, p);
    p[0] = 60; img_escrever_pixel(img, 1, 2, p);

    ImgStatus st = IMG_SEM_MEMORIA;
    Matriz *m = img_para_matriz(img, 0, &st);
    CHECK(m != NULL && st == IMG_SUCESSO);
    CHECK(mat_linhas(m) == 2);
    CHECK(mat_colunas(m) == 3);
    CHECK(igual(mat_val(m, 0, 0), 10.0));
    CHECK(igual(mat_val(m, 0, 1), 20.0));
    CHECK(igual(mat_val(m, 0, 2), 30.0));
    CHECK(igual(mat_val(m, 1, 0), 40.0));
    CHECK(igual(mat_val(m, 1, 1), 50.0));
    CHECK(igual(mat_val(m, 1, 2), 60.0));
    mat_liberar(m);

    /* Canal inválido (imagem de 1 canal só aceita canal 0) */
    st = IMG_SUCESSO;
    CHECK(img_para_matriz(img, 1, &st) == NULL && st == IMG_CANAL_INVALIDO);
    st = IMG_SUCESSO;
    CHECK(img_para_matriz(img, -1, &st) == NULL && st == IMG_CANAL_INVALIDO);

    /* Ponteiros NULL */
    st = IMG_SUCESSO;
    CHECK(img_para_matriz(NULL, 0, &st) == NULL && st == IMG_PONTEIRO_NULL);
    CHECK(img_para_matriz(img, 0, NULL) == NULL);

    img_liberar_imagem(img);
}

static void teste_img_para_matriz_rgb(void) {
    printf("img_para_matriz (RGB)\n");

    /* Imagem 2x2 com 3 canais */
    Image *img = img_criar_imagem(2, 2, 3);
    CHECK(img != NULL);

    unsigned char p[3];
    p[0] = 11; p[1] = 22; p[2] = 33; img_escrever_pixel(img, 0, 0, p);
    p[0] = 44; p[1] = 55; p[2] = 66; img_escrever_pixel(img, 0, 1, p);
    p[0] = 77; p[1] = 88; p[2] = 99; img_escrever_pixel(img, 1, 0, p);
    p[0] = 12; p[1] = 34; p[2] = 56; img_escrever_pixel(img, 1, 1, p);

    ImgStatus st;
    Matriz *r = img_para_matriz(img, 0, &st);
    Matriz *g = img_para_matriz(img, 1, &st);
    Matriz *b = img_para_matriz(img, 2, &st);

    CHECK(r != NULL && g != NULL && b != NULL);
    CHECK(igual(mat_val(r, 0, 0), 11.0) && igual(mat_val(g, 0, 0), 22.0) && igual(mat_val(b, 0, 0), 33.0));
    CHECK(igual(mat_val(r, 0, 1), 44.0) && igual(mat_val(g, 0, 1), 55.0) && igual(mat_val(b, 0, 1), 66.0));
    CHECK(igual(mat_val(r, 1, 0), 77.0) && igual(mat_val(g, 1, 0), 88.0) && igual(mat_val(b, 1, 0), 99.0));
    CHECK(igual(mat_val(r, 1, 1), 12.0) && igual(mat_val(g, 1, 1), 34.0) && igual(mat_val(b, 1, 1), 56.0));

    mat_liberar(r);
    mat_liberar(g);
    mat_liberar(b);

    /* Canal 3 em imagem de 3 canais é inválido (canais válidos: 0, 1, 2) */
    st = IMG_SUCESSO;
    CHECK(img_para_matriz(img, 3, &st) == NULL && st == IMG_CANAL_INVALIDO);

    img_liberar_imagem(img);
}

static void teste_matrizes_para_img(void) {
    printf("matrizes_para_img\n");

    Matriz *r = mat_criar(2, 2);
    Matriz *g = mat_criar(2, 2);
    Matriz *b = mat_criar(2, 2);

    /* Valores com clamp e arredondamento:
     * - valor negativo -> clamp para 0
     * - valor > 255 -> clamp para 255
     * - arredondamento: 127.4 -> 127, 127.6 -> 128
     */
    mat_set(r, 0, 0, 100.0);  mat_set(g, 0, 0, 150.0);  mat_set(b, 0, 0, 200.0);
    mat_set(r, 0, 1, -25.0);   mat_set(g, 0, 1, 300.0);  mat_set(b, 0, 1, 0.0);
    mat_set(r, 1, 0, 127.4);  mat_set(g, 1, 0, 127.6);  mat_set(b, 1, 0, 255.0);
    mat_set(r, 1, 1, 50.0);   mat_set(g, 1, 1, 60.0);   mat_set(b, 1, 1, 70.0);

    ImgStatus st = IMG_SEM_MEMORIA;
    Image *img = matrizes_para_img(r, g, b, &st);
    CHECK(img != NULL && st == IMG_SUCESSO);
    CHECK(img_consul_largura(img) == 2);
    CHECK(img_consul_altura(img) == 2);
    CHECK(img_consul_canal(img) == 3);

    unsigned char p[3];
    /* Pixel (0, 0) */
    img_ler_pixel(img, 0, 0, p);
    CHECK(p[0] == 100 && p[1] == 150 && p[2] == 200);

    /* Pixel (0, 1): -25 clamped to 0, 300 clamped to 255 */
    img_ler_pixel(img, 0, 1, p);
    CHECK(p[0] == 0 && p[1] == 255 && p[2] == 0);

    /* Pixel (1, 0): 127.4 arredonda para 127, 127.6 para 128 */
    img_ler_pixel(img, 1, 0, p);
    CHECK(p[0] == 127 && p[1] == 128 && p[2] == 255);

    /* Pixel (1, 1) */
    img_ler_pixel(img, 1, 1, p);
    CHECK(p[0] == 50 && p[1] == 60 && p[2] == 70);

    img_liberar_imagem(img);

    /* Dimensões incompatíveis */
    Matriz *g_incomp = mat_criar(2, 3);
    st = IMG_SUCESSO;
    CHECK(matrizes_para_img(r, g_incomp, b, &st) == NULL && st == IMG_DIMENSAO_INCOMPATIVEL);
    mat_liberar(g_incomp);

    /* Ponteiros NULL */
    st = IMG_SUCESSO;
    CHECK(matrizes_para_img(NULL, g, b, &st) == NULL && st == IMG_PONTEIRO_NULL);
    st = IMG_SUCESSO;
    CHECK(matrizes_para_img(r, NULL, b, &st) == NULL && st == IMG_PONTEIRO_NULL);
    st = IMG_SUCESSO;
    CHECK(matrizes_para_img(r, g, NULL, &st) == NULL && st == IMG_PONTEIRO_NULL);
    CHECK(matrizes_para_img(r, g, b, NULL) == NULL);

    mat_liberar(r);
    mat_liberar(g);
    mat_liberar(b);
}

/* ---------- main ---------- */

int main(void) {
    teste_img_para_matriz_cinza();
    teste_img_para_matriz_rgb();
    teste_matrizes_para_img();

    printf("\n%d verificacoes, %d falhas\n", total, falhas);
    if (falhas == 0) {
        printf("RESULTADO: APROVADO\n");
        return 0;
    }
    printf("RESULTADO: FALHOU\n");
    return 1;
}
