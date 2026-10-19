/**
 * @file image.c
 * @brief Implementação do módulo de imagem (criação, consulta, pixels).
 *
 * Os pixels ficam num único vetor de bytes (campo lista), em ordem de linhas:
 * o pixel (linha, coluna) começa na posição
 *     (linha * largura + coluna) * canal
 * e ocupa 'canal' bytes consecutivos (R, G, B quando canal = 3).
 */

#include <stdlib.h>
#include "image.h"
#include "image_internal.h"

/**
 * @brief Cria uma imagem com as dimensões e número de canais indicados.
 *
 * @details Valida dimensões positivas e canais (apenas 1 ou 3).
 * Inicializa todos os bytes dos pixels com zero (imagem preta).
 *
 * @param largura Largura em pixels (deve ser > 0).
 * @param altura  Altura em pixels (deve ser > 0).
 * @param canal   Número de canais (1 para tons de cinza, 3 para RGB).
 * @return Ponteiro para a nova Image, ou NULL se argumentos inválidos ou falta de memória.
 */
Image *img_criar_imagem(int largura, int altura, int canal) {
    /* dimensões precisam ser positivas */
    if ((largura <= 0) || (altura <= 0)) {
        return NULL;
    }

    /* só são aceitas imagens em tons de cinza (1) ou RGB (3) */
    if ((canal != 1) && (canal != 3)) {
        return NULL;
    }

    Image *pont = malloc(sizeof *pont);
    if (pont == NULL) {
        return NULL;
    }

    /* o cast para size_t evita overflow de int em imagens grandes */
    size_t quant = (size_t)altura * largura * canal;
    /* calloc zera os bytes: a imagem nova começa toda preta */
    pont->lista = calloc(quant, sizeof *pont->lista);

    /* se o vetor de pixels falhar, libera a struct para não vazar memória */
    if (pont->lista == NULL) {
        free(pont);
        return NULL;
    }

    pont->altura = altura;
    pont->largura = largura;
    pont->canal = canal;

    return pont;
}

/**
 * @brief Libera toda a memória associada a uma Image.
 *
 * @details Libera o vetor de pixels e em seguida a estrutura. Aceita ponteiro NULL.
 *
 * @param img Ponteiro para a imagem a ser liberada.
 */
void img_liberar_imagem(Image *img) {
    if (img == NULL) {
        return;
    }
    free(img->lista);
    free(img);
}

/**
 * @brief Consulta a largura (número de colunas) da imagem.
 *
 * @param img Imagem a consultar.
 * @return Largura em pixels, ou 0 se img for NULL.
 */
int img_consul_largura(const Image *img) {
    if (img == NULL) {
        return 0;
    }
    return img->largura;
}

/**
 * @brief Consulta a altura (número de linhas) da imagem.
 *
 * @param img Imagem a consultar.
 * @return Altura em pixels, ou 0 se img for NULL.
 */
int img_consul_altura(const Image *img) {
    if (img == NULL) {
        return 0;
    }
    return img->altura;
}

/**
 * @brief Consulta o número de canais da imagem.
 *
 * @param img Imagem a consultar.
 * @return 1 (cinza) ou 3 (RGB), ou 0 se img for NULL.
 */
int img_consul_canal(const Image *img) {
    if (img == NULL) {
        return 0;
    }
    return img->canal;
}

/**
 * @brief Escreve os bytes de componente de um pixel na imagem.
 *
 * @details Copia 'canal' bytes do vetor @p bytes para o pixel em (linha, coluna).
 *
 * @param img    Imagem de destino.
 * @param linha  Índice da linha (0 a altura - 1).
 * @param coluna Índice da coluna (0 a largura - 1).
 * @param bytes  Vetor contendo os componentes do pixel (tamanho >= canal).
 * @return IMG_SUCESSO em caso de sucesso;
 *         IMG_PONTEIRO_NULL se img ou bytes forem NULL;
 *         IMG_FORA_DA_IMAGEM se coordenadas estiverem fora dos limites.
 */
ImgStatus img_escrever_pixel(Image *img, int linha, int coluna, const unsigned char bytes[]) {
    if ((img == NULL) || (bytes == NULL)) {
        return IMG_PONTEIRO_NULL;
    }

    /* posição precisa estar dentro de [0, altura) x [0, largura) */
    if ((coluna >= img->largura || linha >= img->altura) || (linha < 0 || coluna < 0)) {
        return IMG_FORA_DA_IMAGEM;
    }

    /* índice do primeiro byte do pixel dentro do vetor 'lista' */
    size_t posi = ((size_t)linha * img->largura + coluna) * img->canal;

    for (int i = 0; i < img->canal; i++) {
        img->lista[i + posi] = bytes[i];
    }
    return IMG_SUCESSO;
}

/**
 * @brief Lê os bytes de componente de um pixel da imagem.
 *
 * @details Copia 'canal' bytes do pixel em (linha, coluna) para o vetor @p bytes.
 *
 * @param img    Imagem de origem.
 * @param linha  Índice da linha (0 a altura - 1).
 * @param coluna Índice da coluna (0 a largura - 1).
 * @param bytes  Vetor de saída para receber os componentes (tamanho >= canal).
 * @return IMG_SUCESSO em caso de sucesso;
 *         IMG_PONTEIRO_NULL se img ou bytes forem NULL;
 *         IMG_FORA_DA_IMAGEM se coordenadas estiverem fora dos limites.
 */
ImgStatus img_ler_pixel(const Image *img, int linha, int coluna, unsigned char bytes[]) {
    if ((img == NULL) || (bytes == NULL)) {
        return IMG_PONTEIRO_NULL;
    }
    if ((coluna >= img->largura || linha >= img->altura) || (linha < 0 || coluna < 0)) {
        return IMG_FORA_DA_IMAGEM;
    }
    size_t posi = ((size_t)linha * img->largura + coluna) * img->canal;
    for (int i = 0; i < img->canal; i++) {
        bytes[i] = img->lista[posi + i];
    }
    return IMG_SUCESSO;
}
