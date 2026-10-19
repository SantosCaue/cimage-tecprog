#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include "image.h"

/**
 * @file image_io.h
 * @brief Leitura e gravação de imagens em arquivo.
 *
 * Usa as bibliotecas públicas stb_image (leitura) e stb_image_write (escrita).
 */

/**
 * @brief Carrega uma imagem a partir de um arquivo.
 *
 * Os formatos aceitos são os suportados pela stb_image (por exemplo PNG, JPG,
 * BMP, TGA). Independentemente do formato original, a imagem é sempre
 * convertida para RGB, ou seja, a imagem devolvida tem sempre 3 canais.
 *
 * @param arquivo Caminho do arquivo a ser lido.
 * @param status  Recebe o código de resultado. Obrigatório (não pode ser NULL;
 *                se for, a função devolve NULL sem executar nenhuma operação.).
 *                Valores possíveis: IMG_SUCESSO, IMG_PONTEIRO_NULL (arquivo NULL),
 *                IMG_ERRO_DE_ARQUIVO (arquivo inexistente ou sem permissão de
 *                leitura), IMG_FORMATO_INVALIDO (conteúdo não reconhecido como
 *                imagem) e IMG_SEM_MEMORIA.
 * @return Ponteiro para a imagem carregada (3 canais), ou NULL em caso de erro.
 * @note A imagem devolvida deve ser liberada com img_liberar_imagem().
 */
Image *img_carregar_imagem(const char *arquivo, ImgStatus *status);

/**
 * @brief Salva uma imagem em arquivo no formato PNG.
 *
 * O arquivo é sempre gravado como PNG, qualquer que seja a extensão do nome.
 * Se já existir, é sobrescrito.
 *
 * @param img     Imagem a ser salva (1 ou 3 canais).
 * @param arquivo Caminho do arquivo de destino.
 * @return IMG_SUCESSO em caso de sucesso;
 *         IMG_PONTEIRO_NULL se img ou arquivo forem NULL;
 *         IMG_ERRO_DE_ARQUIVO se não for possível criar ou escrever o arquivo.
 */
ImgStatus img_salvar_imagem(const Image *img, const char *arquivo);

#endif