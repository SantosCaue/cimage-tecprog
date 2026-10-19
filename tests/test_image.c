#include <stdio.h>
#include <stdlib.h>
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

/* ---------- testes unitários ---------- */

static void teste_criar_e_consultas(void) {
    printf("img_criar_imagem e consultas\n");

    /* RGB: dimensões e canais corretos, pixels começam em 0 (preto) */
    Image *rgb = img_criar_imagem(4, 3, 3);
    CHECK(rgb != NULL);
    CHECK(img_consul_largura(rgb) == 4);
    CHECK(img_consul_altura(rgb) == 3);
    CHECK(img_consul_canal(rgb) == 3);

    unsigned char p[3] = {9, 9, 9};
    CHECK(img_ler_pixel(rgb, 2, 3, p) == IMG_SUCESSO);
    CHECK(p[0] == 0 && p[1] == 0 && p[2] == 0);
    img_liberar_imagem(rgb);

    /* Cinza: 1 canal */
    Image *cinza = img_criar_imagem(2, 5, 1);
    CHECK(cinza != NULL);
    CHECK(img_consul_largura(cinza) == 2);
    CHECK(img_consul_altura(cinza) == 5);
    CHECK(img_consul_canal(cinza) == 1);
    img_liberar_imagem(cinza);

    /* Imagem 1x1 (valor limite) */
    Image *um = img_criar_imagem(1, 1, 3);
    CHECK(um != NULL);
    img_liberar_imagem(um);

    /* Dimensões inválidas */
    CHECK(img_criar_imagem(0, 3, 3) == NULL);
    CHECK(img_criar_imagem(3, 0, 3) == NULL);
    CHECK(img_criar_imagem(-1, 3, 3) == NULL);
    CHECK(img_criar_imagem(3, -1, 3) == NULL);

    /* Número de canais inválido (só 1 ou 3 são aceitos) */
    CHECK(img_criar_imagem(3, 3, 0) == NULL);
    CHECK(img_criar_imagem(3, 3, 2) == NULL);
    CHECK(img_criar_imagem(3, 3, 4) == NULL);

    /* Consultas com NULL devolvem 0; liberar NULL não falha */
    CHECK(img_consul_largura(NULL) == 0);
    CHECK(img_consul_altura(NULL) == 0);
    CHECK(img_consul_canal(NULL) == 0);
    img_liberar_imagem(NULL);
}

static void teste_ler_escrever_rgb(void) {
    printf("img_escrever_pixel e img_ler_pixel (RGB)\n");

    Image *img = img_criar_imagem(3, 2, 3);
    CHECK(img != NULL);

    unsigned char esc[3] = {10, 20, 30};
    unsigned char lido[3] = {0, 0, 0};

    CHECK(img_escrever_pixel(img, 1, 2, esc) == IMG_SUCESSO);
    CHECK(img_ler_pixel(img, 1, 2, lido) == IMG_SUCESSO);
    CHECK(lido[0] == 10 && lido[1] == 20 && lido[2] == 30);

    /* Escrever em um pixel não altera os vizinhos */
    CHECK(img_ler_pixel(img, 1, 1, lido) == IMG_SUCESSO);
    CHECK(lido[0] == 0 && lido[1] == 0 && lido[2] == 0);
    CHECK(img_ler_pixel(img, 0, 2, lido) == IMG_SUCESSO);
    CHECK(lido[0] == 0 && lido[1] == 0 && lido[2] == 0);

    /* Extremos 0 e 255 */
    unsigned char ext[3] = {0, 255, 128};
    CHECK(img_escrever_pixel(img, 0, 0, ext) == IMG_SUCESSO);
    CHECK(img_ler_pixel(img, 0, 0, lido) == IMG_SUCESSO);
    CHECK(lido[0] == 0 && lido[1] == 255 && lido[2] == 128);

    img_liberar_imagem(img);
}

static void teste_ler_escrever_cinza(void) {
    printf("img_escrever_pixel e img_ler_pixel (cinza)\n");

    Image *img = img_criar_imagem(2, 2, 1);
    CHECK(img != NULL);

    unsigned char esc[1] = {200};
    unsigned char lido[1] = {0};
    CHECK(img_escrever_pixel(img, 1, 0, esc) == IMG_SUCESSO);
    CHECK(img_ler_pixel(img, 1, 0, lido) == IMG_SUCESSO);
    CHECK(lido[0] == 200);

    img_liberar_imagem(img);
}

static void teste_erros_de_pixel(void) {
    printf("img_ler_pixel e img_escrever_pixel (erros)\n");

    Image *img = img_criar_imagem(3, 2, 3);
    CHECK(img != NULL);

    unsigned char esc[3] = {1, 2, 3};
    unsigned char lido[3] = {7, 7, 7};

    /* Fora da imagem: linha e coluna negativas, e iguais ao tamanho */
    CHECK(img_escrever_pixel(img, -1, 0, esc) == IMG_FORA_DA_IMAGEM);
    CHECK(img_escrever_pixel(img, 0, -1, esc) == IMG_FORA_DA_IMAGEM);
    CHECK(img_escrever_pixel(img, 2, 0, esc) == IMG_FORA_DA_IMAGEM);  /* altura = 2 */
    CHECK(img_escrever_pixel(img, 0, 3, esc) == IMG_FORA_DA_IMAGEM);  /* largura = 3 */

    CHECK(img_ler_pixel(img, -1, 0, lido) == IMG_FORA_DA_IMAGEM);
    CHECK(img_ler_pixel(img, 0, -1, lido) == IMG_FORA_DA_IMAGEM);
    CHECK(img_ler_pixel(img, 2, 0, lido) == IMG_FORA_DA_IMAGEM);
    CHECK(img_ler_pixel(img, 0, 3, lido) == IMG_FORA_DA_IMAGEM);

    /* Em caso de erro, bytes não é modificado */
    CHECK(lido[0] == 7 && lido[1] == 7 && lido[2] == 7);

    /* Em caso de erro, a imagem não é modificada */
    unsigned char conf[3] = {9, 9, 9};
    CHECK(img_ler_pixel(img, 1, 2, conf) == IMG_SUCESSO);
    CHECK(conf[0] == 0 && conf[1] == 0 && conf[2] == 0);

    /* Ponteiros NULL */
    CHECK(img_escrever_pixel(NULL, 0, 0, esc) == IMG_PONTEIRO_NULL);
    CHECK(img_escrever_pixel(img, 0, 0, NULL) == IMG_PONTEIRO_NULL);
    CHECK(img_ler_pixel(NULL, 0, 0, lido) == IMG_PONTEIRO_NULL);
    CHECK(img_ler_pixel(img, 0, 0, NULL) == IMG_PONTEIRO_NULL);

    img_liberar_imagem(img);
}

/* ---------- main ---------- */

int main(void) {
    teste_criar_e_consultas();
    teste_ler_escrever_rgb();
    teste_ler_escrever_cinza();
    teste_erros_de_pixel();

    printf("\n%d verificacoes, %d falhas\n", total, falhas);
    if (falhas == 0) {
        printf("RESULTADO: APROVADO\n");
        return 0;
    }
    printf("RESULTADO: FALHOU\n");
    return 1;
}
