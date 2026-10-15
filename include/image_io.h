#ifndef IMAGE_IO_H
#define IMAGE_IO_H
#include "image.h"



Image *img_carregar_imagem(const char *arquivo, ImgStatus *status);//imagem devolvida tem sempre 1 canal

ImgStatus img_salvar_imagem(const Image *img,const char *arquivo);



#endif