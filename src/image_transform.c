#include "image_transform.h"
#include <stdlib.h>

Image *cortar (const Image *img, int x_inicial, int y_inicial, int altura, int largura, ImgStatus *status){
    if (img == NULL) {
        if (status != NULL) *status = IMG_PONTEIRO_NULL; //[cite: 9]
        return NULL;
    }
    
    if (x_inicial < 0 || y_inicial < 0 || largura <= 0 || altura <= 0 || x_inicial + largura > img_consul_largura(img) || y_inicial + altura > img_consul_altura(img)){
        if (status != NULL) *status = IMG_FORA_DA_IMAGEM;
        return NULL;      
    }
    Image *img_cortada = img_criar_imagem(largura, altura, img_consul_canal(img));

    if (img_cortada == NULL) {
        if(status != NULL)  *status = IMG_SEM_MEMORIA;
        return NULL;
    }
    
    unsigned char bytes [3];

    for(int i = 0; i < altura; i++)
        for(int j = 0; j < largura; j++){

            img_ler_pixel(img, y_inicial + i, x_inicial + j, bytes);
            img_escrever_pixel(img_cortada, i, j, bytes);
        }

    if(status != NULL) *status = IMG_SUCESSO;
    return img_cortada;
}

Image *espelhar_vertical (const Image *img, ImgStatus *status){
    if (img == NULL) {
        if (status != NULL) *status = IMG_PONTEIRO_NULL; //[cite: 9]
        return NULL;
    }
    int largura = img_consul_largura(img);
    int altura = img_consul_altura(img);
    int canal = img_consul_canal(img);

    Image *img_espelhada = img_criar_imagem(largura, altura, canal);

    if(img_espelhada == NULL){
        if(status != NULL)  *status = IMG_SEM_MEMORIA;
        return NULL; 
    }
    unsigned char bytes[3];

    for(int i = 0; i < altura; i++)
        for(int j = 0; j < largura; j++){
            img_ler_pixel(img, altura - i - 1, j, bytes);
            img_escrever_pixel(img_espelhada, i, j, bytes);
        }

    if(status != NULL) *status = IMG_SUCESSO;
    return img_espelhada;
        
}

Image *espelhar_horizontal (const Image *img, ImgStatus *status){
    if (img == NULL) {
        if (status != NULL) *status = IMG_PONTEIRO_NULL; //[cite: 9]
        return NULL;
    }
    int largura = img_consul_largura(img);
    int altura = img_consul_altura(img);
    int canal = img_consul_canal(img);

    Image *img_espelhada = img_criar_imagem(largura, altura, canal);

    if(img_espelhada == NULL){
        if(status != NULL)  *status = IMG_SEM_MEMORIA;
        return NULL; 
    }
    unsigned char bytes[3];

    for(int i = 0; i < altura; i++)
        for(int j = 0; j < largura; j++){
            img_ler_pixel(img, i, largura - j - 1, bytes);
            img_escrever_pixel(img_espelhada, i, j, bytes);
        }

    if(status != NULL) *status = IMG_SUCESSO;
    return img_espelhada;
    
}

Image *rotacionar (const Image *img, ImgStatus *status){
    if (img == NULL) {
        if (status != NULL) *status = IMG_PONTEIRO_NULL; //[cite: 9]
        return NULL;
    }

    int largura = img_consul_largura(img);
    int altura = img_consul_altura(img);
    int canal = img_consul_canal(img);

    Image *img_rotacionada = img_criar_imagem(altura, largura, canal);

    if(img_rotacionada == NULL){
        if(status != NULL)  *status = IMG_SEM_MEMORIA;
        return NULL; 
    }
    unsigned char bytes[3];

    for(int i = 0; i < largura; i++)
        for(int j = 0; j < altura; j++){
            img_ler_pixel(img, altura - j - 1, i, bytes);
            img_escrever_pixel(img_rotacionada, i, j, bytes);
        }
    
        if(status != NULL) *status = IMG_SUCESSO;
        return img_rotacionada;
}