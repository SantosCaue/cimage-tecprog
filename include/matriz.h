#ifndef MATRIZ_H
#define MATRIZ_H

typedef struct Matriz Matriz;
typedef enum{
    MAT_SUCESSO,
    MAT_PONTEIRO_NULL,
    MAT_FORA_DA_MATRIZ,
    MAT_SEM_MEMORIA
}MatStatus;


Matriz *mat_criar(int linhas,int colunas);

void mat_liberar(Matriz *matriz);

int mat_linhas(const Matriz *matriz);

int mat_colunas(const Matriz *matriz);

MatStatus mat_get(const Matriz* matriz,int linha, int coluna,double *valor);

MatStatus mat_set(Matriz *matriz,int linha, int coluna,double valor);



#endif  