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

int img_consul_largura(const Image *img);

int img_consul_altura(const Image *img);

int img_consul_canal(const Image *img);

Image *img_carregar_imagem(const char *arquivo, ImgStatus *status);

void img_liberar_imagem(Image *img);

ImgStatus img_ler_pixel(const Image *img,int x, int y,unsigned char bytes[]);

ImgStatus img_escrever_pixel(Image *img,int x,int y,const unsigned char bytes[]);



#endif
