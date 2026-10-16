#include <stdlib.h>
#include "matriz.h"

struct Matriz{
    int linhas;
    int colunas;
    double *lista;
};

Matriz *mat_criar(int linhas,int colunas){
    if((linhas<=0)||(colunas<=0)){
        return NULL;
    }
    Matriz *pont=malloc(sizeof *pont);
    if(pont==NULL){
        return NULL;
    }

    size_t quant=(size_t)colunas*linhas;
    pont->lista=calloc(quant,sizeof *pont->lista);
    if(pont->lista == NULL){
        free(pont);
        return NULL;
    }

    pont->linhas=linhas;
    pont->colunas=colunas;

    return pont;
}

void mat_liberar(Matriz *matriz){
    if(matriz==NULL){
        return;
    }
    free(matriz->lista);
    free(matriz);
}

int mat_linhas(const Matriz *matriz){
    if(matriz==NULL){
        return 0;
    }
    return matriz->linhas;
}

int mat_colunas(const Matriz *matriz){
    if(matriz==NULL){
        return 0;
    }
    return matriz->colunas;
}

MatStatus mat_set(Matriz *matriz,int linha, int coluna,double valor){
    if(matriz==NULL){
        return MAT_PONTEIRO_NULL;
    }
    if((linha<0)||(coluna<0)||(linha>=matriz->linhas)||(coluna>=matriz->colunas)){
        return MAT_FORA_DA_MATRIZ;
    }
    size_t posi=((size_t)linha *matriz->colunas + coluna);

    matriz->lista[posi]=valor;
    return MAT_SUCESSO;

}

MatStatus mat_get(const Matriz* matriz,int linha, int coluna,double *valor){
     if((matriz==NULL)||(valor==NULL)){
        return MAT_PONTEIRO_NULL;
    }
    if((linha<0)||(coluna<0)||(linha>=matriz->linhas)||(coluna>=matriz->colunas)){
        return MAT_FORA_DA_MATRIZ;
    }
    size_t posi=((size_t)linha *matriz->colunas + coluna);
    
    *valor=matriz->lista[posi];
    return MAT_SUCESSO;
}




