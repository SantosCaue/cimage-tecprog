#ifndef IMAGE_TRANSFORM_H
#define IMAGE_TRANSFORM_H

#include "image.h"

Image *cortar (const image *img, int x_inicial, int y_inicial, int altura, int largura, ImgStatus *status);

Image *esperlhar_vertical (const Image *img, ImgStatus *status);

Image *espelhar_horizontal (const Image *img, ImgStatus *status);

Image *rotacionar (const Image *img, ImgStatus *status);

#endif