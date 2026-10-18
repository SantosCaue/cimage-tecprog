/**
 * @file image_convert.c
 * @brief Implementação da conversão entre imagens e matrizes.
 *
 * Este módulo usa apenas as APIs públicas de image.h e matriz.h (não inclui
 * image_internal.h), ou seja, funciona sem conhecer a representação interna
 * de Image nem de Matriz.
 */

#include "image_convert.h"
#include "image.h"
#include "matriz.h"
#include <stdlib.h>

/* Copia um canal da imagem para uma matriz de double (mesmas dimensões).*/
Matriz *img_para_matriz(const Image *img,int indice_canal,ImgStatus *status){
    /* sem 'status' não há como informar o motivo da falha, então recusa */
    if(status==NULL){
        return NULL;
    }

    if(img==NULL){
        *status=IMG_PONTEIRO_NULL;
        return NULL;
    }

    /* canal válido: de 0 até (nº de canais da imagem - 1). Assim, numa
     * imagem de 1 canal só o canal 0 é aceito. */
    if((indice_canal<0)||(indice_canal>=img_consul_canal(img))){
        *status=IMG_CANAL_INVALIDO;
        return NULL;
    }
    /* matriz: linhas = altura da imagem, colunas = largura da imagem */
    Matriz *matriz=mat_criar(img_consul_altura(img),img_consul_largura(img));
    if(matriz==NULL){
        *status=IMG_SEM_MEMORIA;
        return NULL;
    }
    /* buffer com espaço para o maior número de canais possível (3) */
    unsigned char bytes[3];
    for(int i=0;i<img_consul_altura(img);i++){
        for(int j=0;j<img_consul_largura(img);j++){
            /* os retornos de img_ler_pixel e mat_set não são checados porque
             * i e j percorrem exatamente os limites da imagem e da matriz,
             * então nenhum erro pode ocorrer aqui */
            img_ler_pixel(img,i,j ,bytes);
            mat_set(matriz,i,j,(double)bytes[indice_canal]);
        }
    }

    *status=IMG_SUCESSO;
    return matriz;
}

/* Converte um valor double qualquer em um componente de pixel (0 a 255):
 * limita ao intervalo válido e arredonda para o inteiro mais próximo
 * (somar 0.5 antes do cast para unsigned char). Função auxiliar interna
 * (static): não faz parte da API pública. */
static unsigned char para_nota(double x) {
    if (x < 0.0) return 0;
    if (x > 255.0) return 255;
    return (unsigned char)(x + 0.5);
}

/* Combina três matrizes (R, G, B) em uma imagem de 3 canais. */
Image *matrizes_para_img(const Matriz *r,const Matriz *g,const Matriz *b,ImgStatus *status){  
    /* sem 'status' não há como informar o motivo da falha, então recusa */
    if(status==NULL){
        return NULL;
    }

    if((r==NULL)||(g==NULL)||(b==NULL)){
        *status=IMG_PONTEIRO_NULL;
        return NULL;
    }
    /* as três matrizes precisam ter exatamente as mesmas dimensões;
     * comparar g e b com r basta para garantir que as três são iguais */
    if((mat_linhas(r) != mat_linhas(g))||(mat_linhas(r) != mat_linhas(b))||(mat_colunas(r) != mat_colunas(g))||
    mat_colunas(r) != mat_colunas(b)){
        *status=IMG_DIMENSAO_INCOMPATIVEL;
        return NULL;
    }

    int linhas=mat_linhas(r);
    int colunas=mat_colunas(r);
    /* img_criar_imagem recebe (largura, altura, canal): largura = colunas,
     * altura = linhas */
    Image *aux=img_criar_imagem(colunas,linhas,3);
    if(aux==NULL){
        *status=IMG_SEM_MEMORIA;
        return NULL;
    }
    for(int i=0;i<linhas;i++){
        for(int j=0;j<colunas;j++){
            double vr,vg,vb;
            /* i e j estão dentro dos limites das três matrizes (iguais),
             * então os retornos de mat_get não precisam ser checados */
            mat_get(r,i,j,&vr);
            mat_get(g,i,j,&vg);
            mat_get(b,i,j,&vb);

            /* ordem R, G, B, a mesma convenção de img_ler_pixel */
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