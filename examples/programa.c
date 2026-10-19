/* Uso: build/bin/programa entrada.png saida.png <filtro> [parametro]
 *
 * Filtros disponiveis:
 *   blur   [raio]     desfoque            (raio padrao: 2)
 *   sharpen           realce de detalhes
 *   sobel             deteccao de bordas
 *   limiar [valor]    preto e branco      (valor padrao: 128)
 *
 * Exemplos:
 *   build/bin/programa foto.png saida.png blur
 *   build/bin/programa foto.png saida.png blur 5
 *   build/bin/programa foto.png saida.png limiar 100
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "image_io.h"
#include "image_convert.h"
#include "matriz.h"
#include "filtros.h"

static void mostrar_uso(const char *prog) {
    printf("Uso: %s entrada.png saida.png <filtro> [parametro]\n", prog);
    printf("Filtros: blur [raio] | sharpen | sobel | limiar [valor]\n");
}

/* Aplica o filtro escolhido (pelo nome) em UMA matriz (um canal).
 * Retorna a nova matriz, ou NULL se o nome do filtro for invalido ou der erro. */
static Matriz *aplicar_filtro(const Matriz *canal, const char *filtro, double param) {
    FilStatus st;

    if (strcmp(filtro, "blur") == 0)    return fil_blur(canal, (int)param, &st);
    if (strcmp(filtro, "sharpen") == 0) return fil_sharpen(canal, &st);
    if (strcmp(filtro, "sobel") == 0)   return fil_sobel(canal, &st);
    if (strcmp(filtro, "limiar") == 0)  return fil_limiar(canal, param, &st);

    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 4 || argc > 5) {
        mostrar_uso(argv[0]);
        return 1;
    }

    const char *entrada = argv[1];
    const char *saida   = argv[2];
    const char *filtro  = argv[3];

    /* Parametro opcional (se nao passar, usa o padrao de cada filtro) */
    double param;
    if (argc == 5) {
        param = atof(argv[4]);
    } else if (strcmp(filtro, "limiar") == 0) {
        param = 128;
    } else {
        param = 2; /* raio do blur */
    }

    /* Valida o nome do filtro antes de abrir a imagem */
    if (strcmp(filtro, "blur") != 0 && strcmp(filtro, "sharpen") != 0 &&
        strcmp(filtro, "sobel") != 0 && strcmp(filtro, "limiar") != 0) {
        printf("Filtro desconhecido: '%s'\n", filtro);
        mostrar_uso(argv[0]);
        return 1;
    }

    ImgStatus st;

    /* 1. Carregar a imagem */
    Image *img = img_carregar_imagem(entrada, &st);
    if (img == NULL) {
        printf("Erro ao carregar '%s' (codigo %d)\n", entrada, st);
        return 1;
    }

    /* 2. Separar os canais R, G, B em matrizes */
    Matriz *canais[3];
    Matriz *filtrados[3];
    for (int i = 0; i < 3; i++) {
        canais[i] = img_para_matriz(img, i, &st);
        filtrados[i] = NULL;
    }

    /* 3. Aplicar o filtro escolhido em cada canal */
    int ok = 1;
    for (int i = 0; i < 3; i++) {
        if (canais[i] == NULL) { ok = 0; break; }
        filtrados[i] = aplicar_filtro(canais[i], filtro, param);
        if (filtrados[i] == NULL) { ok = 0; break; }
    }

    /* 4. Juntar os canais e salvar */
    Image *resultado = NULL;
    if (ok) {
        resultado = img_de_matrizes(filtrados[0], filtrados[1], filtrados[2], &st);
    }
    if (resultado != NULL && img_salvar_imagem(resultado, saida) == IMG_SUCESSO) {
        printf("Filtro '%s' aplicado. Salvo em %s\n", filtro, saida);
    } else {
        printf("Erro ao aplicar o filtro ou salvar a imagem\n");
        ok = 0;
    }

    /* 5. Liberar tudo */
    img_liberar_imagem(img);
    img_liberar_imagem(resultado);
    for (int i = 0; i < 3; i++) {
        mat_liberar(canais[i]);
        mat_liberar(filtrados[i]);
    }
    return ok ? 0 : 1;
}