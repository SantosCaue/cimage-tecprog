#ifndef IMAGE_CONVERT_H
#define IMAGE_CONVERT_H

#include "image.h"
#include "matriz.h"

Matriz *img_para_matriz(const Image *img,int indice_canal,ImgStatus *status);//mesma justificativa do segundo, mas entrar em acordo com img_carregar tbm

Image *matrizes_para_img(const Matriz *r,const Matriz *g,const Matriz *b,ImgStatus *status);//aceitar NULL(para quem nn tem interesse no motivo do erro)
//canal pode ser 0(r),1(g) ou 2(b)

#endif
