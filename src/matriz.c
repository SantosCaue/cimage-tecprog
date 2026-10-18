/**
 * @file matriz.c
 * @brief Implementação do módulo de matriz de doubles.
 *
 * Os elementos ficam num único vetor, em ordem de linhas: o elemento
 * (linha, coluna) está na posição linha * colunas + coluna.
 */

#include <stdlib.h>
#include "matriz.h"

/* Definição da struct, visível só neste arquivo: o header matriz.h expõe
 * apenas o tipo opaco, então quem usa a biblioteca não vê estes campos. */
struct Matriz{
    int linhas;
    int colunas;
    double *lista;
};

/* Cria uma matriz zerada. Devolve NULL se as dimensões forem inválidas ou
 * se faltar memória (liberando o que já tinha alocado). */
Matriz *mat_criar(int linhas,int colunas){
    if((linhas<=0)||(colunas<=0)){
        return NULL;
    }
    Matriz *pont=malloc(sizeof *pont);
    if(pont==NULL){
        return NULL;
    }

    /* cast para size_t evita overflow de int no produto */
    size_t quant=(size_t)colunas*linhas;
    /* calloc zera o vetor: todos os elementos começam com 0.0 */
    pont->lista=calloc(quant,sizeof *pont->lista);
    if(pont->lista == NULL){
        free(pont);
        return NULL;
    }

    pont->linhas=linhas;
    pont->colunas=colunas;

    return pont;
}

/* Libera o vetor de elementos e depois a struct. Aceita NULL. */
void mat_liberar(Matriz *matriz){
    if(matriz==NULL){
        return;
    }
    free(matriz->lista);
    free(matriz);
}

/* Consultas de dimensão: devolvem 0 para matriz NULL (valor que nunca ocorre
 * numa matriz válida, já que mat_criar exige dimensões positivas). */
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

/* Grava 'valor' no elemento (linha, coluna). A matriz não é alterada se os
 * argumentos forem inválidos. */
MatStatus mat_set(Matriz *matriz,int linha, int coluna,double valor){
    if(matriz==NULL){
        return MAT_PONTEIRO_NULL;
    }
    /* índices precisam estar em [0, linhas) e [0, colunas) */
    if((linha<0)||(coluna<0)||(linha>=matriz->linhas)||(coluna>=matriz->colunas)){
        return MAT_FORA_DA_MATRIZ;
    }
    /* posição do elemento no vetor (armazenamento por linhas) */
    size_t posi=((size_t)linha *matriz->colunas + coluna);

    matriz->lista[posi]=valor;
    return MAT_SUCESSO;

}

/* Lê o elemento (linha, coluna) para *valor. 'valor' só é modificado em
 * caso de sucesso. */
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




