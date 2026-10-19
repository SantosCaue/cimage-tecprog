#ifndef IMAGE_CONVERT_H
#define IMAGE_CONVERT_H

#include "image.h"
#include "matriz.h"

/**
 * @file image_convert.h
 * @brief Conversão entre imagens e matrizes.
 *
 * Os canais são numerados 0 (R), 1 (G) e 2 (B).
 */

/**
 * @brief Extrai um canal de uma imagem como uma matriz de double.
 *
 * O elemento (i, j) da matriz recebe o valor (0 a 255) do canal indicado
 * no pixel da linha i e coluna j.
 *
 * @param img          Imagem de origem.
 * @param indice_canal Canal a extrair: de 0 até img_consul_canal(img) - 1
 *                     (0 = R, 1 = G, 2 = B em imagens RGB).
 * @param status       Recebe o código de resultado. Obrigatório (não pode ser NULL): se for NULL, a função
 *                     devolve NULL sem executar nenhuma operação.
 *                     Valores possíveis: IMG_SUCESSO, IMG_PONTEIRO_NULL (img NULL),
 *                     IMG_CANAL_INVALIDO (indice_canal negativo ou inexistente na
 *                     imagem) e IMG_SEM_MEMORIA.
 * @return Nova matriz com altura(img) linhas e largura(img) colunas,
 *         ou NULL em caso de erro.
 * @note A matriz devolvida deve ser liberada com mat_liberar().
 */
Matriz *img_para_matriz(const Image *img, int indice_canal, ImgStatus *status);

/**
 * @brief Monta uma imagem RGB (3 canais) a partir de três matrizes.
 *
 * Os valores das matrizes são arredondados para o inteiro mais próximo e
 * limitados ao intervalo [0, 255]: valores menores que 0 viram 0 e valores
 * maiores que 255 viram 255.
 *
 * @param r      Matriz do canal vermelho.
 * @param g      Matriz do canal verde.
 * @param b      Matriz do canal azul.
 * @param status Recebe o código de resultado. Obrigatório (não pode ser NULL): se for NULL, a função
 *               devolve NULL sem executar nenhuma operação.
 *               Valores possíveis: IMG_SUCESSO, IMG_PONTEIRO_NULL (alguma matriz NULL),
 *               IMG_DIMENSAO_INCOMPATIVEL (matrizes com número de linhas e/ou
 *               colunas diferentes) e IMG_SEM_MEMORIA.
 * @return Nova imagem de 3 canais com largura = colunas e altura = linhas das
 *         matrizes, ou NULL em caso de erro.
 * @note A imagem devolvida deve ser liberada com img_liberar_imagem().
 *       As matrizes de entrada não são modificadas nem liberadas.
 */
Image *matrizes_para_img(const Matriz *r, const Matriz *g, const Matriz *b, ImgStatus *status);

/* Alias para padronização de nomenclatura com prefixo img_ */
#define img_de_matrizes matrizes_para_img

#endif
