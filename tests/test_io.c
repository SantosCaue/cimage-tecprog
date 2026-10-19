#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "image_io.h"

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

static void teste_salvar_e_carregar_rgb(void) {
    printf("img_salvar_imagem e img_carregar_imagem (RGB)\n");

    const char *arquivo_temp = "build/teste_io_rgb.png";

    /* Cria imagem 3x2 RGB com pixels conhecidos */
    Image *orig = img_criar_imagem(3, 2, 3);
    CHECK(orig != NULL);

    unsigned char p[3];
    p[0] = 255; p[1] = 0;   p[2] = 0;   img_escrever_pixel(orig, 0, 0, p); /* Vermelho */
    p[0] = 0;   p[1] = 255; p[2] = 0;   img_escrever_pixel(orig, 0, 1, p); /* Verde */
    p[0] = 0;   p[1] = 0;   p[2] = 255; img_escrever_pixel(orig, 0, 2, p); /* Azul */
    p[0] = 128; p[1] = 64;  p[2] = 32;  img_escrever_pixel(orig, 1, 0, p);
    p[0] = 200; p[1] = 200; p[2] = 200; img_escrever_pixel(orig, 1, 1, p);
    p[0] = 10;  p[1] = 20;  p[2] = 30;  img_escrever_pixel(orig, 1, 2, p);

    /* Salva em PNG */
    CHECK(img_salvar_imagem(orig, arquivo_temp) == IMG_SUCESSO);

    /* Carrega de volta */
    ImgStatus st = IMG_SEM_MEMORIA;
    Image *carregada = img_carregar_imagem(arquivo_temp, &st);
    CHECK(carregada != NULL && st == IMG_SUCESSO);

    /* Valida dimensões e canais */
    CHECK(img_consul_largura(carregada) == 3);
    CHECK(img_consul_altura(carregada) == 2);
    CHECK(img_consul_canal(carregada) == 3);

    /* Valida valores exatos dos pixels */
    unsigned char lido[3];
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            img_ler_pixel(orig, i, j, p);
            img_ler_pixel(carregada, i, j, lido);
            CHECK(p[0] == lido[0] && p[1] == lido[1] && p[2] == lido[2]);
        }
    }

    img_liberar_imagem(orig);
    img_liberar_imagem(carregada);
    remove(arquivo_temp);
}

static void teste_salvar_e_carregar_cinza(void) {
    printf("img_salvar_imagem e img_carregar_imagem (tons de cinza)\n");

    const char *arquivo_temp = "build/teste_io_cinza.png";

    /* Cria imagem 2x2 com 1 canal */
    Image *orig = img_criar_imagem(2, 2, 1);
    CHECK(orig != NULL);

    unsigned char p[1];
    p[0] = 50;  img_escrever_pixel(orig, 0, 0, p);
    p[0] = 100; img_escrever_pixel(orig, 0, 1, p);
    p[0] = 150; img_escrever_pixel(orig, 1, 0, p);
    p[0] = 200; img_escrever_pixel(orig, 1, 1, p);

    CHECK(img_salvar_imagem(orig, arquivo_temp) == IMG_SUCESSO);

    /* Carrega de volta: conforme image_io.h, a imagem carregada sempre tem 3 canais RGB */
    ImgStatus st = IMG_SEM_MEMORIA;
    Image *carregada = img_carregar_imagem(arquivo_temp, &st);
    CHECK(carregada != NULL && st == IMG_SUCESSO);
    CHECK(img_consul_largura(carregada) == 2);
    CHECK(img_consul_altura(carregada) == 2);
    CHECK(img_consul_canal(carregada) == 3);

    unsigned char lido[3];
    img_ler_pixel(carregada, 0, 0, lido);
    CHECK(lido[0] == 50 && lido[1] == 50 && lido[2] == 50);

    img_ler_pixel(carregada, 0, 1, lido);
    CHECK(lido[0] == 100 && lido[1] == 100 && lido[2] == 100);

    img_ler_pixel(carregada, 1, 0, lido);
    CHECK(lido[0] == 150 && lido[1] == 150 && lido[2] == 150);

    img_ler_pixel(carregada, 1, 1, lido);
    CHECK(lido[0] == 200 && lido[1] == 200 && lido[2] == 200);

    img_liberar_imagem(orig);
    img_liberar_imagem(carregada);
    remove(arquivo_temp);
}

static void teste_io_erros(void) {
    printf("img_carregar_imagem e img_salvar_imagem (tratamento de erros)\n");

    ImgStatus st = IMG_SUCESSO;

    /* Arquivo inexistente */
    CHECK(img_carregar_imagem("build/arquivo_que_nao_existe.png", &st) == NULL);
    CHECK(st == IMG_ERRO_DE_ARQUIVO);

    /* Arquivo com formato inválido (não é imagem) */
    const char *arq_invalido = "build/teste_invalido.txt";
    FILE *f = fopen(arq_invalido, "w");
    if (f != NULL) {
        fprintf(f, "Este arquivo nao e uma imagem valida.\n");
        fclose(f);

        st = IMG_SUCESSO;
        CHECK(img_carregar_imagem(arq_invalido, &st) == NULL);
        CHECK(st == IMG_FORMATO_INVALIDO);

        remove(arq_invalido);
    }

    /* Falha ao salvar em caminho inexistente */
    Image *img = img_criar_imagem(2, 2, 3);
    CHECK(img_salvar_imagem(img, "caminho_inexistente/subpasta/arquivo.png") == IMG_ERRO_DE_ARQUIVO);

    /* Ponteiros NULL */
    st = IMG_SUCESSO;
    CHECK(img_carregar_imagem(NULL, &st) == NULL && st == IMG_PONTEIRO_NULL);
    CHECK(img_carregar_imagem("qualquer.png", NULL) == NULL);

    CHECK(img_salvar_imagem(NULL, "qualquer.png") == IMG_PONTEIRO_NULL);
    CHECK(img_salvar_imagem(img, NULL) == IMG_PONTEIRO_NULL);

    img_liberar_imagem(img);
}

/* ---------- main ---------- */

int main(void) {
    teste_salvar_e_carregar_rgb();
    teste_salvar_e_carregar_cinza();
    teste_io_erros();

    printf("\n%d verificacoes, %d falhas\n", total, falhas);
    if (falhas == 0) {
        printf("RESULTADO: APROVADO\n");
        return 0;
    }
    printf("RESULTADO: FALHOU\n");
    return 1;
}
