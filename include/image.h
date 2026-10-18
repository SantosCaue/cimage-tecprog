#ifndef IMAGE_H
#define IMAGE_H

/**
 * @file image.h
 * @brief Tipo opaco Image e operações básicas sobre imagens.
 *
 * Uma imagem é uma grade de pixels com 1 canal (tons de cinza) ou
 * 3 canais (RGB). Cada componente de um pixel é um unsigned char (0 a 255).
 * Em imagens de 3 canais, a ordem dos componentes é R (0), G (1), B (2).
 * Linhas e colunas começam em 0.
 */

/** Tipo opaco: a estrutura é definida apenas em image_internal.h. */
typedef struct Image Image;

/** Códigos de status devolvidos pelas funções do módulo de imagem. */
typedef enum{
    IMG_SUCESSO,               /**< Operação concluída sem erros. */
    IMG_PONTEIRO_NULL,         /**< Um ponteiro obrigatório recebido é NULL. */
    IMG_FORA_DA_IMAGEM,        /**< Linha ou coluna fora dos limites da imagem. */
    IMG_SEM_MEMORIA,           /**< Falha de alocação de memória. */
    IMG_ERRO_DE_ARQUIVO,       /**< Falha ao abrir, ler ou escrever um arquivo. */
    IMG_FORMATO_INVALIDO,      /**< Conteúdo do arquivo não é uma imagem em formato suportado. */
    IMG_CANAL_INVALIDO,        /**< Índice de canal negativo ou inexistente na imagem. */
    IMG_DIMENSAO_INCOMPATIVEL, /**< As matrizes R, G e B não têm o mesmo número de linhas e/ou colunas. */
}ImgStatus;

/**
 * @brief Cria uma imagem com as dimensões e o número de canais indicados.
 *
 * Todos os componentes de todos os pixels começam com valor 0 (imagem preta).
 *
 * @param largura Número de colunas (deve ser maior que 0).
 * @param altura  Número de linhas (deve ser maior que 0).
 * @param canal   Número de canais: 1 (cinza) ou 3 (RGB).
 * @return Ponteiro para a nova imagem, ou NULL se largura ou altura forem
 *         menores ou iguais a 0, se canal for diferente de 1 e de 3, ou se
 *         faltar memória.
 * @note A imagem devolvida deve ser liberada com img_liberar_imagem().
 */
Image *img_criar_imagem(int largura,int altura,int canal);

/**
 * @brief Consulta a largura (número de colunas) da imagem.
 *
 * @param img Imagem a consultar.
 * @return Largura em pixels, ou 0 se img for NULL.
 */
int img_consul_largura(const Image *img);//receber valor 0 faz com que a imagem nn entra em img_criar_imagem

/**
 * @brief Consulta a altura (número de linhas) da imagem.
 *
 * @param img Imagem a consultar.
 * @return Altura em pixels, ou 0 se img for NULL.
 */
int img_consul_altura(const Image *img);//receber valor 0 faz com que a imagem nn entra em img_criar_imagem

/**
 * @brief Consulta o número de canais da imagem.
 *
 * @param img Imagem a consultar.
 * @return 1 (tons de cinza) ou 3 (RGB), ou 0 se img for NULL.
 */
int img_consul_canal(const Image *img);//receber qualquer valor diferente de 1 ou 3 a imagem nn entra em img_criar_imagem

/**
 * @brief Libera a memória de uma imagem.
 *
 * @param img Imagem a liberar. Se for NULL, nada acontece.
 * @note Após a chamada, o ponteiro não deve mais ser usado.
 */
void img_liberar_imagem(Image *img);

/**
 * @brief Lê os componentes de um pixel.
 *
 * @param img    Imagem de origem.
 * @param linha  Linha do pixel (de 0 a altura - 1).
 * @param coluna Coluna do pixel (de 0 a largura - 1).
 * @param bytes  Vetor de saída, com pelo menos img_consul_canal(img) posições.
 *               Em imagens RGB: bytes[0] = R, bytes[1] = G, bytes[2] = B.
 * @return IMG_SUCESSO em caso de sucesso;
 *         IMG_PONTEIRO_NULL se img ou bytes forem NULL;
 *         IMG_FORA_DA_IMAGEM se linha ou coluna estiverem fora dos limites.
 *         Em caso de erro, bytes não é modificado.
 */
ImgStatus img_ler_pixel(const Image *img,int linha, int coluna,unsigned char bytes[]);

/**
 * @brief Escreve os componentes de um pixel.
 *
 * @param img    Imagem de destino.
 * @param linha  Linha do pixel (de 0 a altura - 1).
 * @param coluna Coluna do pixel (de 0 a largura - 1).
 * @param bytes  Vetor com pelo menos img_consul_canal(img) valores
 *               (em imagens RGB: R, G, B).
 * @return IMG_SUCESSO em caso de sucesso;
 *         IMG_PONTEIRO_NULL se img ou bytes forem NULL;
 *         IMG_FORA_DA_IMAGEM se linha ou coluna estiverem fora dos limites.
 *         Em caso de erro, a imagem não é modificada.
 */
ImgStatus img_escrever_pixel(Image *img,int linha,int coluna,const unsigned char bytes[]);


#endif
