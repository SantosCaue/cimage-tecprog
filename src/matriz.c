/**
 * @file matriz.c
 * @brief Implementação do módulo de matriz de doubles.
 *
 * Os elementos ficam num único vetor, em ordem de linhas: o elemento
 * (linha, coluna) está na posição linha * colunas + coluna.
 */

#include <stdlib.h>
#include <math.h>
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

/**
 * @brief Multiplica todos os elementos da matriz por um escalar (in-place).
 *
 * @details Percorre o vetor interno de elementos em ordem linear e multiplica
 * cada posição por @p escalar. A própria matriz é modificada; nenhuma cópia
 * é criada.
 *
 * @param matriz  Matriz a escalar.
 * @param escalar Fator de multiplicação.
 * @return MAT_SUCESSO em caso de sucesso;
 *         MAT_PONTEIRO_NULL se @p matriz for NULL.
 */
MatStatus mat_mult_escalar(Matriz *matriz, double escalar){
    if(matriz == NULL){
        return MAT_PONTEIRO_NULL;
    }
    size_t total = (size_t)matriz->linhas * matriz->colunas;
    for(size_t i = 0; i < total; i++){
        matriz->lista[i] *= escalar;
    }
    return MAT_SUCESSO;
}

/**
 * @brief Multiplica a matriz por um vetor-coluna (M × v).
 *
 * @details Para cada linha @c i da matriz, calcula o produto escalar da linha
 * com @p vetor e armazena o resultado em @p resultado[i]. O array de saída
 * deve ser alocado pelo chamador com pelo menos mat_linhas(@p matriz) posições.
 *
 * @param matriz    Matriz de dimensão m×n.
 * @param vetor     Vetor de entrada com @p n elementos.
 * @param n         Comprimento do vetor (deve ser igual a mat_colunas(@p matriz)).
 * @param resultado Array de saída com m elementos, alocado pelo chamador.
 * @return MAT_SUCESSO em caso de sucesso;
 *         MAT_PONTEIRO_NULL se @p matriz, @p vetor ou @p resultado forem NULL;
 *         MAT_DIMENSAO_INVALIDA se @p n != mat_colunas(@p matriz).
 */
MatStatus mat_mult_vetor(const Matriz *matriz, const double *vetor, int n,
                         double *resultado){
    if((matriz == NULL) || (vetor == NULL) || (resultado == NULL)){
        return MAT_PONTEIRO_NULL;
    }
    /* n deve ser igual ao número de colunas para o produto ser definido */
    if(n != matriz->colunas){
        return MAT_DIMENSAO_INVALIDA;
    }
    for(int i = 0; i < matriz->linhas; i++){
        double soma = 0.0;
        for(int j = 0; j < matriz->colunas; j++){
            soma += matriz->lista[(size_t)i * matriz->colunas + j] * vetor[j];
        }
        resultado[i] = soma;
    }
    return MAT_SUCESSO;
}

/**
 * @brief Multiplica duas matrizes (A × B) e devolve uma nova matriz resultado.
 *
 * @details Implementa o produto matricial padrão: res[i][j] = Σ A[i][p]*B[p][j].
 * @p a deve ter dimensão m×k e @p b deve ter dimensão k×n; o resultado tem
 * dimensão m×n. Devolve NULL se algum ponteiro for NULL, se as dimensões
 * forem incompatíveis (colunas(A) ≠ linhas(B)) ou se faltar memória.
 *
 * @param a Primeira matriz (m×k).
 * @param b Segunda matriz (k×n).
 * @return Nova matriz resultado (m×n), ou NULL em caso de erro.
 * @note O chamador é responsável por liberar a matriz devolvida com mat_liberar().
 */
Matriz *mat_mult_matriz(const Matriz *a, const Matriz *b){
    if((a == NULL) || (b == NULL)){
        return NULL;
    }
    /* número de colunas de A deve ser igual ao número de linhas de B */
    if(a->colunas != b->linhas){
        return NULL;
    }
    int m = a->linhas;
    int k = a->colunas;  /* = b->linhas */
    int n = b->colunas;

    Matriz *res = mat_criar(m, n);
    if(res == NULL){
        return NULL;
    }
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            double soma = 0.0;
            for(int p = 0; p < k; p++){
                soma += a->lista[(size_t)i * k + p]
                      * b->lista[(size_t)p * n + j];
            }
            res->lista[(size_t)i * n + j] = soma;
        }
    }
    return res;
}

/**
 * @brief Soma duas matrizes (A + B) e devolve uma nova matriz resultado.
 *
 * @details Calcula res[i][j] = A[i][j] + B[i][j] para todos os índices.
 * As dimensões de @p a e @p b devem ser idênticas. Devolve NULL se algum
 * ponteiro for NULL, se as dimensões diferirem ou se faltar memória.
 *
 * @param a Primeira matriz.
 * @param b Segunda matriz.
 * @return Nova matriz resultado, ou NULL em caso de erro.
 * @note O chamador é responsável por liberar a matriz devolvida com mat_liberar().
 */
Matriz *mat_somar(const Matriz *a, const Matriz *b){
    if((a == NULL) || (b == NULL)){
        return NULL;
    }
    if((a->linhas != b->linhas) || (a->colunas != b->colunas)){
        return NULL;
    }
    Matriz *res = mat_criar(a->linhas, a->colunas);
    if(res == NULL){
        return NULL;
    }
    size_t total = (size_t)a->linhas * a->colunas;
    for(size_t i = 0; i < total; i++){
        res->lista[i] = a->lista[i] + b->lista[i];
    }
    return res;
}

/**
 * @brief Calcula a transposta de uma matriz e devolve uma nova matriz.
 *
 * @details Cria uma nova matriz de dimensão colunas(@p matriz) × linhas(@p matriz)
 * onde res[j][i] = original[i][j]. A matriz original não é modificada.
 *
 * @param matriz Matriz a transpor.
 * @return Nova matriz transposta, ou NULL se @p matriz for NULL ou faltar memória.
 * @note O chamador é responsável por liberar a matriz devolvida com mat_liberar().
 */
Matriz *mat_transpor(const Matriz *matriz){
    if(matriz == NULL){
        return NULL;
    }
    Matriz *res = mat_criar(matriz->colunas, matriz->linhas);
    if(res == NULL){
        return NULL;
    }
    for(int i = 0; i < matriz->linhas; i++){
        for(int j = 0; j < matriz->colunas; j++){
            res->lista[(size_t)j * matriz->linhas + i] =
                matriz->lista[(size_t)i * matriz->colunas + j];
        }
    }
    return res;
}

