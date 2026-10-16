#ifndef IMAGE_H
#define IMAGE_H

typedef struct Image Image;

typedef enum{
    IMG_SUCESSO,
    IMG_PONTEIRO_NULL,
    IMG_FORA_DA_IMAGEM,
    IMG_SEM_MEMORIA,
    IMG_ERRO_DE_ARQUIVO,
    IMG_FORMATO_INVALIDO
}ImgStatus;

Image *img_criar_imagem(int largura,int altura,int canal);

int img_consul_largura(const Image *img);//receber valor 0 faz com que a imagem nn entra em img_criar_imagem

int img_consul_altura(const Image *img);//receber valor 0 faz com que a imagem nn entra em img_criar_imagem

int img_consul_canal(const Image *img);//receber qualquer valor diferente de 1 ou 3 a imagem nn entra em img_criar_imagem

void img_liberar_imagem(Image *img);

ImgStatus img_ler_pixel(const Image *img,int linha, int coluna,unsigned char bytes[]);

ImgStatus img_escrever_pixel(Image *img,int linha,int coluna,const unsigned char bytes[]);



#endif
