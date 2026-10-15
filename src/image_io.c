#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_io.h"
#include "image_internal.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"//biblioteca pública, salvar JPG/PNG

#define STB_IMAGE_WRITE_IMPLEMENTATION//biblioteca pública, escrever JPG/PNG
#include "stb_image_write.h"


Image *img_carregar_imagem(const char *arquivo, ImgStatus *status){
   
    if(status==NULL){
        return NULL;    
    }
   
    if(arquivo==NULL){
        *status=IMG_PONTEIRO_NULL;  
        return NULL;
    }

    FILE *f = fopen(arquivo,"rb");//Abrir o arquivo primeiro e verificar se ele existe
    if(f==NULL){
        *status=IMG_ERRO_DE_ARQUIVO;
        return NULL;
    }
    fclose(f);

    int largura,altura,canal;
    unsigned char* dados;

    dados=stbi_load(arquivo,&largura,&altura,&canal,3);
    if(dados==NULL){
        *status=IMG_FORMATO_INVALIDO;
        return NULL;
    }   
    
    Image *rec=img_criar_imagem(largura,altura,3);
    if(rec==NULL){
        stbi_image_free(dados);
        *status=IMG_SEM_MEMORIA;
        return NULL;
    }

    memcpy(rec->lista,dados,(size_t)largura*altura*3);
    stbi_image_free(dados);
    *status=IMG_SUCESSO;
    return rec;
}

ImgStatus img_salvar_imagem(const Image *img,const char *arquivo){
    if((img==NULL)||(arquivo==NULL)){
        return IMG_PONTEIRO_NULL;
    }

    int escr=stbi_write_png(arquivo,img->largura,img->altura,img->canal,img->lista,img->largura * img->canal);//grava sempre PNG

    if(escr==0){
        return IMG_ERRO_DE_ARQUIVO;
    }
    return IMG_SUCESSO;
}

