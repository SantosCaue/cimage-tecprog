#include <stdlib.h>
#include "image.h"
#include "image_internal.h"

Image *img_criar_imagem(int largura,int altura,int canal){
    if((largura<=0)||(altura<=0)){
        return NULL;
    }
    if((canal != 1)&&(canal != 3)){
        return NULL;
    }
    Image *pont=malloc(sizeof *pont);
    if(pont==NULL){
        return NULL;
    }
    size_t quant=(size_t)altura*largura*canal;
    pont->lista=calloc(quant,sizeof *pont->lista);

    if(pont->lista == NULL){
        free(pont);
        return NULL;
    }

    pont->altura=altura;
    pont->largura=largura;
    pont->canal=canal;


    return pont;
}

void img_liberar_imagem(Image *img){
    if(img==NULL){
        return;
    }
    free(img->lista);
    free(img);
}

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

ImgStatus img_escrever_pixel(Image *img,int linha,int coluna,const unsigned char bytes[]){
    if((img==NULL)||(bytes==NULL)){
        return IMG_PONTEIRO_NULL;
    }
    if((coluna>=img->largura||linha>=img->altura)||(linha<0 || coluna<0)){
        return IMG_FORA_DA_IMAGEM;
    }
    size_t posi = ((size_t)linha * img->largura + coluna) * img->canal;

    for(int i=0;i<img->canal;i++){
        img->lista[i+posi]=bytes[i];

    }
    return IMG_SUCESSO;
}

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
