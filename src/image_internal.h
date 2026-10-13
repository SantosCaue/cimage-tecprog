#ifndef IMAGE_INTERNAL_H
#define IMAGE_INTERNAL_H


struct Image{
    int largura;
    int altura;
    int canal;
    unsigned char* lista;
};
#endif 