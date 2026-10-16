#include "image_convert.h"
#include "image.h"
#include "matriz.h"
#include <stdlib.h>

Matriz *img_para_matriz(const Image *img,int indice_canal,ImgStatus *status){
    if(img==NULL){
        *status=IMG_PONTEIRO_NULL;
        return NULL;
    }

    if((indice_canal<0)||(indice_canal>=img_consul_canal(img))){
        *status=IMG_CANAL_INVALIDO;
        return NULL;
    }
    Matriz *matriz=mat_criar(img_consul_altura(img),img_consul_largura(img));
    if(matriz==NULL){
        *status=IMG_SEM_MEMORIA;
        return NULL;
    }
    unsigned char bytes[3];
    for(int i=0;i<img_consul_altura(img);i++){
        for(int j=0;j<img_consul_largura(img);j++){
            img_ler_pixel(img,i,j ,bytes);
            mat_set(matriz,i,j,(double)bytes[indice_canal]);
        }
    }

    *status=IMG_SUCESSO;
    return matriz;
}

static unsigned char para_nota(double x) {
    if (x < 0.0) return 0;
    if (x > 255.0) return 255;
    return (unsigned char)(x + 0.5);
}

Image *matrizes_para_img(const Matriz *r,const Matriz *g,const Matriz *b,ImgStatus *status){
    if((r==NULL)||(g==NULL)||(b==NULL)){
        *status=IMG_PONTEIRO_NULL;
        return NULL;
    }
    if((mat_linhas(r) != mat_linhas(g))||(mat_linhas(r) != mat_linhas(b))||(mat_colunas(r) != mat_colunas(g))||
    mat_colunas(r) != mat_colunas(b)){
        *status=IMG_DIMENSAO_INCOMPATIVEL;
        return NULL;
    }

    int linhas=mat_linhas(r);
    int colunas=mat_colunas(r);
    Image *aux=img_criar_imagem(colunas,linhas,3);
    if(aux==NULL){
        *status=IMG_SEM_MEMORIA;
        return NULL;
    }
    for(int i=0;i<linhas;i++){
        for(int j=0;j<colunas;j++){
            double vr,vg,vb;
            mat_get(r,i,j,&vr);
            mat_get(g,i,j,&vg);
            mat_get(b,i,j,&vb);

            unsigned char bytes[3];
            bytes[0] = para_nota(vr);
            bytes[1]=para_nota(vg);
            bytes[2]=para_nota(vb);

            img_escrever_pixel(aux,i,j,bytes);

        }
    }
    *status=IMG_SUCESSO;
    return aux;
}