#ifndef IMAGE_TRANSFORM_H
#define IMAGE_TRANSFORM_H

/**
 * @file image_transform.h
 * @brief Transformações geométricas e espaciais sobre imagens.
 *
 * Fornece operações de recorte (corte), espelhamento vertical/horizontal
 * e rotação em 90 graus no sentido horário.
 */

#include "image.h"

/**
 * @brief Faz uma cópia de uma região retangular específica da imagem original.
 *
 * @param img            Ponteiro para a imagem original.
 * @param linha_inicial  Linha do canto superior esquerdo do corte.
 * @param coluna_inicial Coluna do canto superior esquerdo do corte.
 * @param altura         Dimensão vertical do corte (número de linhas).
 * @param largura        Dimensão horizontal do corte (número de colunas).
 * @param status         Ponteiro opcional para armazenar o código de status:
 *                       - IMG_SUCESSO em caso de sucesso;
 *                       - IMG_PONTEIRO_NULL se img for NULL;
 *                       - IMG_FORA_DA_IMAGEM se o corte exceder as dimensões da imagem
 *                         ou se as coordenadas/dimensões forem inválidas;
 *                       - IMG_SEM_MEMORIA se falhar a alocação de memória.
 * @return Ponteiro para a nova imagem contendo o recorte, ou NULL em caso de erro.
 * @note A imagem retornada deve ser liberada com img_liberar_imagem().
 */
Image *img_cortar(const Image *img, int linha_inicial, int coluna_inicial,
                  int altura, int largura, ImgStatus *status);

/**
 * @brief Faz uma cópia da imagem original espelhada verticalmente.
 *
 * @param img    Ponteiro para a imagem original.
 * @param status Ponteiro opcional para armazenar o código de status:
 *               - IMG_SUCESSO em caso de sucesso;
 *               - IMG_PONTEIRO_NULL se img for NULL;
 *               - IMG_SEM_MEMORIA se falhar a alocação de memória.
 * @return Ponteiro para a nova imagem espelhada verticalmente, ou NULL em caso de erro.
 * @note A imagem retornada deve ser liberada com img_liberar_imagem().
 */
Image *img_espelhar_vertical(const Image *img, ImgStatus *status);

/**
 * @brief Faz uma cópia da imagem original espelhada horizontalmente.
 *
 * @param img    Ponteiro para a imagem original.
 * @param status Ponteiro opcional para armazenar o código de status:
 *               - IMG_SUCESSO em caso de sucesso;
 *               - IMG_PONTEIRO_NULL se img for NULL;
 *               - IMG_SEM_MEMORIA se falhar a alocação de memória.
 * @return Ponteiro para a nova imagem espelhada horizontalmente, ou NULL em caso de erro.
 * @note A imagem retornada deve ser liberada com img_liberar_imagem().
 */
Image *img_espelhar_horizontal(const Image *img, ImgStatus *status);

/**
 * @brief Faz uma cópia da imagem original rotacionada em 90 graus no sentido horário.
 *
 * @param img    Ponteiro para a imagem original.
 * @param status Ponteiro opcional para armazenar o código de status:
 *               - IMG_SUCESSO em caso de sucesso;
 *               - IMG_PONTEIRO_NULL se img for NULL;
 *               - IMG_SEM_MEMORIA se falhar a alocação de memória.
 * @return Ponteiro para a nova imagem rotacionada, ou NULL em caso de erro.
 * @note A imagem retornada deve ser liberada com img_liberar_imagem().
 */
Image *img_rotacionar(const Image *img, ImgStatus *status);

/* Aliases para compatibilidade retroativa com código existente */
#define cortar img_cortar
#define espelhar_vertical img_espelhar_vertical
#define espelhar_horizontal img_espelhar_horizontal
#define rotacionar img_rotacionar

#endif