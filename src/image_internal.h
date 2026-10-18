#ifndef IMAGE_INTERNAL_H
#define IMAGE_INTERNAL_H

/**
 * @file image_internal.h
 * @brief Definição interna da struct Image (NÃO faz parte da API pública).
 *
 * Este header é incluído apenas por image.c e image_io.c, que precisam
 * acessar os campos da struct. A aplicação cliente e os demais módulos só
 * enxergam o tipo opaco declarado em image.h, o que permite mudar a
 * representação interna sem alterar quem usa a biblioteca.
 */

struct Image{
    int largura;          /* número de colunas (> 0) */
    int altura;           /* número de linhas (> 0) */
    int canal;            /* número de canais: 1 (cinza) ou 3 (RGB) */
    unsigned char* lista; /* pixels em ordem linha a linha, com os canais de
                             cada pixel lado a lado (ex.: R G B R G B ...).
                             Tamanho total: altura * largura * canal bytes. */
};
#endif