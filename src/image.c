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

/* Cria uma imagem validando dimensões e número de canais.
 * Devolve NULL em qualquer falha, liberando o que já tinha alocado. */
Image *img_criar_imagem(int largura,int altura,int canal){
     /* dimensões precisam ser positivas */
    if((largura<=0)||(altura<=0)){
        return NULL;
    }

    /* só são aceitas imagens em tons de cinza (1) ou RGB (3) */
    if((canal != 1)&&(canal != 3)){
        return NULL;
    }

    Image *pont=malloc(sizeof *pont);
    if(pont==NULL){
        return NULL;
    }

    /* o cast para size_t evita overflow de int em imagens grandes */
    size_t quant=(size_t)altura*largura*canal;
     /* calloc zera os bytes: a imagem nova começa toda preta */
    pont->lista=calloc(quant,sizeof *pont->lista);
   
    /* se o vetor de pixels falhar, libera a struct para não vazar memória */
    if(pont->lista == NULL){
        free(pont);
        return NULL;
    }

    pont->altura=altura;
    pont->largura=largura;
    pont->canal=canal;

    return pont;
}

/* Libera primeiro o vetor de pixels e depois a struct (ordem inversa da
 * alocação). Aceita NULL para facilitar a limpeza em caminhos de erro. */
void img_liberar_imagem(Image *img){
    if(img==NULL){
        return;
    }
    free(img->lista);
    free(img);
}

/* Funções de consulta: devolvem 0 para imagem NULL, valor que nunca ocorre
 * numa imagem válida (todas as dimensões e canais são > 0). */
int img_consul_largura(const Image *img){
    if(img==NULL){
        return 0;
    }
    return img->largura;
}

int img_consul_altura(const Image *img){
    if(img==NULL){
        return 0;
    }
    return img->altura;
}

int img_consul_canal(const Image *img){
    if(img==NULL){
        return 0;
    }
    return img->canal;
}

/* Copia os 'canal' bytes de 'bytes' para o pixel (linha, coluna).
 * Nada é alterado se algum argumento for inválido. */
ImgStatus img_escrever_pixel(Image *img,int linha,int coluna,const unsigned char bytes[]){
    if((img==NULL)||(bytes==NULL)){
        return IMG_PONTEIRO_NULL;
    }

     /* posição precisa estar dentro de [0, altura) x [0, largura) */
    if((coluna>=img->largura||linha>=img->altura)||(linha<0 || coluna<0)){
        return IMG_FORA_DA_IMAGEM;
    }

     /* índice do primeiro byte do pixel dentro do vetor 'lista' */
    size_t posi = ((size_t)linha * img->largura + coluna) * img->canal;

    for(int i=0;i<img->canal;i++){
        img->lista[i+posi]=bytes[i];

    }
    return IMG_SUCESSO;
}

/* Operação inversa de img_escrever_pixel: copia os 'canal' bytes do pixel
 * (linha, coluna) para 'bytes'. 'bytes' só é modificado em caso de sucesso. */
ImgStatus img_ler_pixel(const Image *img,int linha, int coluna ,unsigned char bytes[]){
    if((img==NULL)||(bytes==NULL)){
        return IMG_PONTEIRO_NULL;
    }
    if((coluna>=img->largura||linha>=img->altura)||(linha<0 || coluna<0)){
        return IMG_FORA_DA_IMAGEM;
    }
    size_t posi = ((size_t)linha * img->largura + coluna) * img->canal;
    for(int i=0;i<img->canal;i++){
        bytes[i]=img->lista[posi+i];
    }
    return IMG_SUCESSO;
}
