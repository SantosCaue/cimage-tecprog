#ifndef IMAGE_TRANSFORM_H
#define IMAGE_TRANSFORM_H

#include "image.h"
/* Finalidade: Faz cópia de uma região específica da imagem original.
*  Parâmetros: 
*   - img: ponteiro para imagem original.
*   - linha_inicial: linha do ponto mais a esquerda e mais acima no corte.
*   - coluna_inicial: coluna do ponto mais a esquerda e mais acima no corte
*   - altura: dimensão vertica do corte.
*   - largura: dimensão horizontal do corte. 
*   - status: Ponteiro para armazenar código de erro.
*  Retorno: Ponteiro para nova imagem contendo o corte da imagem original, ou NULL no caso de erro.
*  Condições de erro:
*   - IMG_PONTEIRO_NULL se a imagem recebida for nula.
*   - IMG_FORA_DA_IMAGEM se o corte estiver fora das dimensões da imagem.
*   - IMG_SEM_MEMORIA se houver falha na alocação dinâmica de memória.
*/
Image *cortar (const Image *img, int linha_inicial, int coluna_inicial, int altura, int largura, ImgStatus *status);

/* Finalidade: Faz cópia da imagem original espelhada verticalmente.
*  Parâmetros: 
*   - img: ponteiro para imagem original.
*   - status: Ponteiro para armazenar código de erro.
*  Retorno: Ponteiro para nova imagem da imagem original espelhada verticalmente, ou NULL no caso de erro.
*  Condições de erro:
*   - IMG_PONTEIRO_NULL se a imagem recebida for nula.
*   - IMG_SEM_MEMORIA se houver falha na alocação dinâmica de memória.
*/
Image *espelhar_vertical (const Image *img, ImgStatus *status);

/* Finalidade: Faz cópia da imagem original espelhada horizontalmente.
*  Parâmetros: 
*   - img: ponteiro para imagem original.
*   - status: Ponteiro para armazenar código de erro.
*  Retorno: Ponteiro para nova imagem da imagem original espelhada horizontalmente, ou NULL no caso de erro.
*  Condições de erro:
*   - IMG_PONTEIRO_NULL se a imagem recebida for nula.
*   - IMG_SEM_MEMORIA se houver falha na alocação dinâmica de memória.
*/
Image *espelhar_horizontal (const Image *img, ImgStatus *status);

/* Finalidade: Faz cópia da imagem original rotacionada em 90 graus no sentido horário.
*  Parâmetros: 
*   - img: ponteiro para imagem original.
*   - status: Ponteiro para armazenar código de erro.
*  Retorno: Ponteiro para nova imagem da imagem original rotacionada, ou NULL no caso de erro.
*  Condições de erro:
*   - IMG_PONTEIRO_NULL se a imagem recebida for nula.
*   - IMG_SEM_MEMORIA se houver falha na alocação dinâmica de memória.
*/
Image *rotacionar (const Image *img, ImgStatus *status);

#endif