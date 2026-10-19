#include <stdio.h>
#include <stdlib.h>
#include "image_transform.h"
#include "image.h"

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

static int pixel_igual(const unsigned char a[3], const unsigned char b[3], int canais) {
    for (int c = 0; c < canais; c++) {
        if (a[c] != b[c]) return 0;
    }
    return 1;
}

/* ---------- testes unitários ---------- */

static void teste_cortar(void) {
    printf("img_cortar\n");

    /* Imagem 4x4 onde pixel (i, j) = { i * 10 + j, 0, 0 } */
    Image *img = img_criar_imagem(4, 4, 3);
    CHECK(img != NULL);

    unsigned char p[3] = {0, 0, 0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            p[0] = (unsigned char)(i * 10 + j);
            img_escrever_pixel(img, i, j, p);
        }
    }

    /* Corte 2x2 a partir da posição (1, 1) */
    ImgStatus status = IMG_SEM_MEMORIA;
    Image *corte = img_cortar(img, 1, 1, 2, 2, &status);
    CHECK(corte != NULL && status == IMG_SUCESSO);
    CHECK(img_consul_altura(corte) == 2);
    CHECK(img_consul_largura(corte) == 2);
    CHECK(img_consul_canal(corte) == 3);

    /* Validação dos pixels cortados */
    unsigned char esperado[3] = {0, 0, 0};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            img_ler_pixel(corte, i, j, p);
            esperado[0] = (unsigned char)((1 + i) * 10 + (1 + j));
            CHECK(pixel_igual(p, esperado, 3));
        }
    }
    img_liberar_imagem(corte);

    /* Corte exato de toda a imagem (valor limite) */
    corte = img_cortar(img, 0, 0, 4, 4, &status);
    CHECK(corte != NULL && status == IMG_SUCESSO);
    CHECK(img_consul_altura(corte) == 4 && img_consul_largura(corte) == 4);
    img_liberar_imagem(corte);

    /* Casos inválidos: corte que ultrapassa limites */
    status = IMG_SUCESSO;
    CHECK(img_cortar(img, 3, 3, 2, 2, &status) == NULL && status == IMG_FORA_DA_IMAGEM);
    status = IMG_SUCESSO;
    CHECK(img_cortar(img, -1, 0, 2, 2, &status) == NULL && status == IMG_FORA_DA_IMAGEM);
    status = IMG_SUCESSO;
    CHECK(img_cortar(img, 0, 0, 0, 2, &status) == NULL && status == IMG_FORA_DA_IMAGEM);

    /* Ponteiro NULL */
    status = IMG_SUCESSO;
    CHECK(img_cortar(NULL, 0, 0, 2, 2, &status) == NULL && status == IMG_PONTEIRO_NULL);
    CHECK(img_cortar(NULL, 0, 0, 2, 2, NULL) == NULL);

    img_liberar_imagem(img);
}

static void teste_espelhar_vertical(void) {
    printf("img_espelhar_vertical\n");

    /* Imagem 3x2:
     * Linha 0: (10, 10, 10)
     * Linha 1: (20, 20, 20)
     * Linha 2: (30, 30, 30)
     */
    Image *img = img_criar_imagem(2, 3, 3);
    CHECK(img != NULL);

    unsigned char p[3];
    for (int j = 0; j < 2; j++) {
        p[0] = p[1] = p[2] = 10; img_escrever_pixel(img, 0, j, p);
        p[0] = p[1] = p[2] = 20; img_escrever_pixel(img, 1, j, p);
        p[0] = p[1] = p[2] = 30; img_escrever_pixel(img, 2, j, p);
    }

    ImgStatus status = IMG_SEM_MEMORIA;
    Image *esp = img_espelhar_vertical(img, &status);
    CHECK(esp != NULL && status == IMG_SUCESSO);
    CHECK(img_consul_largura(esp) == 2);
    CHECK(img_consul_altura(esp) == 3);

    /* Linha 0 deve ter valor 30, linha 1 valor 20, linha 2 valor 10 */
    for (int j = 0; j < 2; j++) {
        img_ler_pixel(esp, 0, j, p); CHECK(p[0] == 30 && p[1] == 30 && p[2] == 30);
        img_ler_pixel(esp, 1, j, p); CHECK(p[0] == 20 && p[1] == 20 && p[2] == 20);
        img_ler_pixel(esp, 2, j, p); CHECK(p[0] == 10 && p[1] == 10 && p[2] == 10);
    }
    img_liberar_imagem(esp);

    /* Ponteiro NULL */
    status = IMG_SUCESSO;
    CHECK(img_espelhar_vertical(NULL, &status) == NULL && status == IMG_PONTEIRO_NULL);
    CHECK(img_espelhar_vertical(NULL, NULL) == NULL);

    img_liberar_imagem(img);
}

static void teste_espelhar_horizontal(void) {
    printf("img_espelhar_horizontal\n");

    /* Imagem 2x3:
     * Coluna 0: (1, 1, 1), Coluna 1: (2, 2, 2), Coluna 2: (3, 3, 3)
     */
    Image *img = img_criar_imagem(3, 2, 3);
    CHECK(img != NULL);

    unsigned char p[3];
    for (int i = 0; i < 2; i++) {
        p[0] = p[1] = p[2] = 1; img_escrever_pixel(img, i, 0, p);
        p[0] = p[1] = p[2] = 2; img_escrever_pixel(img, i, 1, p);
        p[0] = p[1] = p[2] = 3; img_escrever_pixel(img, i, 2, p);
    }

    ImgStatus status = IMG_SEM_MEMORIA;
    Image *esp = img_espelhar_horizontal(img, &status);
    CHECK(esp != NULL && status == IMG_SUCESSO);
    CHECK(img_consul_largura(esp) == 3);
    CHECK(img_consul_altura(esp) == 2);

    /* Coluna 0 deve ter valor 3, coluna 1 valor 2, coluna 2 valor 1 */
    for (int i = 0; i < 2; i++) {
        img_ler_pixel(esp, i, 0, p); CHECK(p[0] == 3 && p[1] == 3 && p[2] == 3);
        img_ler_pixel(esp, i, 1, p); CHECK(p[0] == 2 && p[1] == 2 && p[2] == 2);
        img_ler_pixel(esp, i, 2, p); CHECK(p[0] == 1 && p[1] == 1 && p[2] == 1);
    }
    img_liberar_imagem(esp);

    /* Ponteiro NULL */
    status = IMG_SUCESSO;
    CHECK(img_espelhar_horizontal(NULL, &status) == NULL && status == IMG_PONTEIRO_NULL);
    CHECK(img_espelhar_horizontal(NULL, NULL) == NULL);

    img_liberar_imagem(img);
}

static void teste_rotacionar(void) {
    printf("img_rotacionar\n");

    /* Imagem original 3x2 (largura 3, altura 2):
     * [ (10,0,0) (20,0,0) (30,0,0) ]
     * [ (40,0,0) (50,0,0) (60,0,0) ]
     *
     * Rotacionada 90° CW (largura 2, altura 3):
     * [ (40,0,0) (10,0,0) ]
     * [ (50,0,0) (20,0,0) ]
     * [ (60,0,0) (30,0,0) ]
     */
    Image *img = img_criar_imagem(3, 2, 3);
    CHECK(img != NULL);

    unsigned char p[3] = {0, 0, 0};
    p[0] = 10; img_escrever_pixel(img, 0, 0, p);
    p[0] = 20; img_escrever_pixel(img, 0, 1, p);
    p[0] = 30; img_escrever_pixel(img, 0, 2, p);
    p[0] = 40; img_escrever_pixel(img, 1, 0, p);
    p[0] = 50; img_escrever_pixel(img, 1, 1, p);
    p[0] = 60; img_escrever_pixel(img, 1, 2, p);

    ImgStatus status = IMG_SEM_MEMORIA;
    Image *rot = img_rotacionar(img, &status);
    CHECK(rot != NULL && status == IMG_SUCESSO);
    CHECK(img_consul_largura(rot) == 2);
    CHECK(img_consul_altura(rot) == 3);

    /* Valida cada pixel na nova posição rotacionada */
    img_ler_pixel(rot, 0, 0, p); CHECK(p[0] == 40);
    img_ler_pixel(rot, 0, 1, p); CHECK(p[0] == 10);
    img_ler_pixel(rot, 1, 0, p); CHECK(p[0] == 50);
    img_ler_pixel(rot, 1, 1, p); CHECK(p[0] == 20);
    img_ler_pixel(rot, 2, 0, p); CHECK(p[0] == 60);
    img_ler_pixel(rot, 2, 1, p); CHECK(p[0] == 30);

    img_liberar_imagem(rot);

    /* Ponteiro NULL */
    status = IMG_SUCESSO;
    CHECK(img_rotacionar(NULL, &status) == NULL && status == IMG_PONTEIRO_NULL);
    CHECK(img_rotacionar(NULL, NULL) == NULL);

    img_liberar_imagem(img);
}

/* ---------- main ---------- */

int main(void) {
    teste_cortar();
    teste_espelhar_vertical();
    teste_espelhar_horizontal();
    teste_rotacionar();

    printf("\n%d verificacoes, %d falhas\n", total, falhas);
    if (falhas == 0) {
        printf("RESULTADO: APROVADO\n");
        return 0;
    }
    printf("RESULTADO: FALHOU\n");
    return 1;
}