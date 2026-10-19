#ifndef MATRIZ_H
#define MATRIZ_H

/**
 * @file matriz.h
 * @brief Tipo opaco Matriz de valores double.
 *
 * Linhas e colunas começam em 0.
 */

/** Tipo opaco: a estrutura é definida apenas em matriz.c. */
typedef struct Matriz Matriz;

/** Códigos de status devolvidos pelas funções do módulo de matriz. */
typedef enum{
    MAT_SUCESSO,           /**< Operação concluída sem erros. */
    MAT_PONTEIRO_NULL,     /**< Um ponteiro obrigatório recebido é NULL. */
    MAT_FORA_DA_MATRIZ,    /**< Linha ou coluna fora dos limites da matriz. */
    MAT_SEM_MEMORIA,       /**< Falha de alocação de memória. */
    MAT_DIMENSAO_INVALIDA, /**< Dimensões incompatíveis para a operação. */
    MAT_NAO_QUADRADA       /**< Operação exige matriz quadrada. */
}MatStatus;

/**
 * @brief Cria uma matriz com o número de linhas e colunas indicado.
 *
 * Todos os elementos começam com valor 0.0.
 *
 * @param linhas  Número de linhas (deve ser maior que 0).
 * @param colunas Número de colunas (deve ser maior que 0).
 * @return Ponteiro para a nova matriz, ou NULL se linhas ou colunas forem
 *         menores ou iguais a 0, ou se faltar memória.
 * @note Deve ser liberada com mat_liberar().
 */
Matriz *mat_criar(int linhas,int colunas);

/**
 * @brief Libera a memória de uma matriz.
 *
 * @param matriz Matriz a liberar. Se for NULL, nada acontece.
 */
void mat_liberar(Matriz *matriz);

/**
 * @brief Consulta o número de linhas da matriz.
 *
 * @param matriz Matriz a consultar.
 * @return Número de linhas, ou 0 se matriz for NULL.
 */
int mat_linhas(const Matriz *matriz);

/**
 * @brief Consulta o número de colunas da matriz.
 *
 * @param matriz Matriz a consultar.
 * @return Número de colunas, ou 0 se matriz for NULL.
 */
int mat_colunas(const Matriz *matriz);

/**
 * @brief Lê o valor de um elemento da matriz.
 *
 * @param matriz Matriz de origem.
 * @param linha  Linha do elemento (de 0 a linhas - 1).
 * @param coluna Coluna do elemento (de 0 a colunas - 1).
 * @param valor  Recebe o valor lido.
 * @return MAT_SUCESSO em caso de sucesso;
 *         MAT_PONTEIRO_NULL se matriz ou valor forem NULL;
 *         MAT_FORA_DA_MATRIZ se linha ou coluna estiverem fora dos limites.
 *         Em caso de erro, valor não é modificado.
 */
MatStatus mat_get(const Matriz *matriz, int linha, int coluna, double *valor);

/**
 * @brief Escreve um valor em um elemento da matriz.
 *
 * @param matriz Matriz de destino.
 * @param linha  Linha do elemento (de 0 a linhas - 1).
 * @param coluna Coluna do elemento (de 0 a colunas - 1).
 * @param valor  Valor a gravar.
 * @return MAT_SUCESSO em caso de sucesso;
 *         MAT_PONTEIRO_NULL se matriz for NULL;
 *         MAT_FORA_DA_MATRIZ se linha ou coluna estiverem fora dos limites.
 *         Em caso de erro, a matriz não é modificada.
 */
MatStatus mat_set(Matriz *matriz, int linha, int coluna, double valor);

/**
 * @brief Multiplica todos os elementos da matriz por um escalar (in-place).
 *
 * @param matriz  Matriz a escalar. É modificada no próprio objeto.
 * @param escalar Valor pelo qual cada elemento será multiplicado.
 * @return MAT_SUCESSO em caso de sucesso;
 *         MAT_PONTEIRO_NULL se matriz for NULL.
 */
MatStatus mat_mult_escalar(Matriz *matriz, double escalar);

/**
 * @brief Multiplica a matriz por um vetor-coluna (M × v).
 *
 * A matriz deve ter número de colunas igual a @p n.
 * O array @p resultado deve ter pelo menos mat_linhas(matriz) posições,
 * alocadas pelo chamador.
 *
 * @param matriz    Matriz de dimensão m×n.
 * @param vetor     Vetor de entrada com n elementos.
 * @param n         Comprimento do vetor (deve ser igual a mat_colunas(matriz)).
 * @param resultado Array de saída com m elementos, alocado pelo chamador.
 * @return MAT_SUCESSO em caso de sucesso;
 *         MAT_PONTEIRO_NULL se algum ponteiro for NULL;
 *         MAT_DIMENSAO_INVALIDA se n != mat_colunas(matriz).
 */
MatStatus mat_mult_vetor(const Matriz *matriz, const double *vetor, int n,
                         double *resultado);

/**
 * @brief Multiplica duas matrizes (A × B) e devolve uma nova matriz.
 *
 * O número de colunas de A deve ser igual ao número de linhas de B.
 * A matriz devolvida tem dimensão linhas(A) × colunas(B).
 *
 * @param a Primeira matriz (m×k).
 * @param b Segunda matriz (k×n).
 * @return Nova matriz resultado (m×n), ou NULL em caso de erro
 *         (ponteiro NULL, dimensões incompatíveis ou falta de memória).
 * @note Deve ser liberada com mat_liberar().
 */
Matriz *mat_mult_matriz(const Matriz *a, const Matriz *b);

/**
 * @brief Soma duas matrizes (A + B) e devolve uma nova matriz.
 *
 * As duas matrizes devem ter exatamente as mesmas dimensões.
 *
 * @param a Primeira matriz.
 * @param b Segunda matriz.
 * @return Nova matriz resultado, ou NULL em caso de erro
 *         (ponteiro NULL, dimensões diferentes ou falta de memória).
 * @note Deve ser liberada com mat_liberar().
 */
Matriz *mat_somar(const Matriz *a, const Matriz *b);

/**
 * @brief Calcula a transposta de uma matriz e devolve uma nova matriz.
 *
 * Se a original é m×n, a transposta é n×m, onde resultado[j][i] = original[i][j].
 *
 * @param matriz Matriz a transpor.
 * @return Nova matriz transposta, ou NULL se matriz for NULL ou faltar memória.
 * @note Deve ser liberada com mat_liberar().
 */
Matriz *mat_transpor(const Matriz *matriz);

#endif