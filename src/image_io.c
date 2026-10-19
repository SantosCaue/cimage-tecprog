/**
 * @file image_io.c
 * @brief Implementação da leitura e gravação de imagens em arquivo.
 *
 * Usa as bibliotecas públicas de header único stb_image.h (leitura de
 * PNG/JPG/BMP etc.) e stb_image_write.h (escrita de PNG). A definição
 * STB_..._IMPLEMENTATION deve aparecer em exatamente UM arquivo .c do
 * projeto, pois é ela que gera o código das funções das bibliotecas stb.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_io.h"
#include "image_internal.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // biblioteca pública, leitura JPG/PNG/BMP

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" // biblioteca pública, escrita PNG

/**
 * @brief Carrega uma imagem a partir de um arquivo e converte para RGB (3 canais).
 *
 * @details Realiza os seguintes passos:
 *  1. Valida ponteiros de entrada.
 *  2. Verifica existência e permissão de leitura do arquivo.
 *  3. Decodifica os dados através da biblioteca stb_image forçando 3 canais.
 *  4. Aloca uma Image e copia o buffer decodificado.
 *
 * @param arquivo Caminho para o arquivo de imagem no disco.
 * @param status  Ponteiro obrigatório para receber o status (não pode ser NULL).
 * @return Ponteiro para a nova Image de 3 canais, ou NULL em caso de erro.
 * @note A imagem retornada deve ser liberada com img_liberar_imagem().
 */
Image *img_carregar_imagem(const char *arquivo, ImgStatus *status) {
    /* sem 'status' não há como informar o motivo da falha, então recusa */
    if (status == NULL) {
        return NULL;
    }

    if (arquivo == NULL) {
        *status = IMG_PONTEIRO_NULL;
        return NULL;
    }

    FILE *f = fopen(arquivo, "rb"); // Abrir o arquivo primeiro e verificar se ele existe
    if (f == NULL) {
        /* arquivo inexistente ou sem permissão de leitura */
        *status = IMG_ERRO_DE_ARQUIVO;
        return NULL;
    }
    /* o teste de abertura acima serve só para distinguir "arquivo não existe"
     * de "formato inválido"; a leitura real é feita pelo stbi_load */
    fclose(f);

    int largura, altura, canal;
    unsigned char *dados;

    /* o último argumento (3) força a conversão para RGB, qualquer que seja o
     * formato original (cinza, RGBA etc.). 'canal' recebe o nº de canais do
     * arquivo original e não é usado depois. */
    dados = stbi_load(arquivo, &largura, &altura, &canal, 3);
    if (dados == NULL) {
        /* arquivo existe, mas não foi reconhecido como imagem */
        *status = IMG_FORMATO_INVALIDO;
        return NULL;
    }

    Image *rec = img_criar_imagem(largura, altura, 3);
    if (rec == NULL) {
        /* libera o buffer da stb antes de sair para não vazar memória */
        stbi_image_free(dados);
        *status = IMG_SEM_MEMORIA;
        return NULL;
    }

    /* o layout da stb (linha a linha, R G B intercalados) é o mesmo de
     * Image.lista, então basta copiar o bloco inteiro de uma vez */
    memcpy(rec->lista, dados, (size_t)largura * altura * 3);
    stbi_image_free(dados);
    *status = IMG_SUCESSO;
    return rec;
}

/**
 * @brief Grava a imagem em arquivo no formato PNG.
 *
 * @details Utiliza a biblioteca stb_image_write para codificar os pixels no
 * formato PNG. O arquivo é sempre salvo como PNG, independentemente de sua extensão.
 *
 * @param img     Ponteiro para a imagem a ser salva.
 * @param arquivo Caminho de destino para gravação.
 * @return IMG_SUCESSO em caso de sucesso;
 *         IMG_PONTEIRO_NULL se img ou arquivo forem NULL;
 *         IMG_ERRO_DE_ARQUIVO em caso de erro na gravação.
 */
ImgStatus img_salvar_imagem(const Image *img, const char *arquivo) {
    if ((img == NULL) || (arquivo == NULL)) {
        return IMG_PONTEIRO_NULL;
    }

    /* último argumento = bytes por linha (stride) = largura * canais.
     * stbi_write_png devolve 0 em caso de falha (ex.: não conseguiu criar
     * o arquivo). */
    int escr = stbi_write_png(arquivo, img->largura, img->altura, img->canal,
                              img->lista, img->largura * img->canal);

    if (escr == 0) {
        return IMG_ERRO_DE_ARQUIVO;
    }
    return IMG_SUCESSO;
}
