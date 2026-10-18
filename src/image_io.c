/**
 * @file image_io.c
 * @brief Implementação da leitura e gravação de imagens em arquivo.
 *
 * Usa as bibliotecas públicas de header único stb_image.h (leitura de
 * PNG/JPG/BMP etc.) e stb_image_write.h (escrita de PNG). A definição
 * STB_..._IMPLEMENTATION deve aparecer em exatamente UM arquivo .c do
 * projeto, pois é ela que gera o código das funções das bibliotecas stb.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_io.h"
#include "image_internal.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"//biblioteca pública, salvar JPG/PNG

#define STB_IMAGE_WRITE_IMPLEMENTATION//biblioteca pública, escrever JPG/PNG
#include "stb_image_write.h"

/* Carrega um arquivo de imagem e o converte para uma Image RGB (3 canais).
 * Etapas: valida argumentos -> confirma que o arquivo abre -> decodifica com
 * stb_image -> copia os pixels para uma Image própria da biblioteca. */
Image *img_carregar_imagem(const char *arquivo, ImgStatus *status){
   
    /* sem 'status' não há como informar o motivo da falha, então recusa */
    if(status==NULL){
       return NULL;    
    }
   
    if(arquivo==NULL){
       *status=IMG_PONTEIRO_NULL;  
       return NULL;
    }

    FILE *f = fopen(arquivo,"rb");//Abrir o arquivo primeiro e verificar se ele existe
    if(f==NULL){
        /* arquivo inexistente ou sem permissão de leitura */
        *status=IMG_ERRO_DE_ARQUIVO;
        return NULL;
    }
    /* o teste de abertura acima serve só para distinguir "arquivo não existe"
     * de "formato inválido"; a leitura real é feita pelo stbi_load */
    fclose(f);

    int largura,altura,canal;
    unsigned char* dados;

    /* o último argumento (3) força a conversão para RGB, qualquer que seja o
     * formato original (cinza, RGBA etc.). 'canal' recebe o nº de canais do
     * arquivo original e não é usado depois. */
    dados=stbi_load(arquivo,&largura,&altura,&canal,3);
    if(dados==NULL){
        /* arquivo existe, mas não foi reconhecido como imagem */
        *status=IMG_FORMATO_INVALIDO;
        return NULL;
    }   
    
    Image *rec=img_criar_imagem(largura,altura,3);
    if(rec==NULL){
        /* libera o buffer da stb antes de sair para não vazar memória */
        stbi_image_free(dados);
        *status=IMG_SEM_MEMORIA;
        return NULL;
    }

    /* o layout da stb (linha a linha, R G B intercalados) é o mesmo de
     * Image.lista, então basta copiar o bloco inteiro de uma vez */
    memcpy(rec->lista,dados,(size_t)largura*altura*3);
    stbi_image_free(dados);
    *status=IMG_SUCESSO;
    return rec;
}

/* Grava a imagem como PNG. O formato é sempre PNG, independentemente da
 * extensão do nome do arquivo. */
ImgStatus img_salvar_imagem(const Image *img,const char *arquivo){
    if((img==NULL)||(arquivo==NULL)){
        return IMG_PONTEIRO_NULL;
    }

    /* último argumento = bytes por linha (stride) = largura * canais.
     * stbi_write_png devolve 0 em caso de falha (ex.: não conseguiu criar
     * o arquivo). */
    int escr=stbi_write_png(arquivo,img->largura,img->altura,img->canal,img->lista,img->largura * img->canal);//grava sempre PNG

    if(escr==0){
        return IMG_ERRO_DE_ARQUIVO;
    }
    return IMG_SUCESSO;
}

