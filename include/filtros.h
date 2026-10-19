#ifndef FILTROS_H
#define FILTROS_H

/**
 * @file filtros.h
 * @brief Filtros espaciais e operações de processamento sobre Matriz.
 *
 * Cada filtro opera sobre uma Matriz (um canal por vez). A matriz de entrada
 * não é modificada; uma nova Matriz é alocada e retornada, devendo ser
 * liberada posteriormente com mat_liberar(). O parâmetro @p status é opcional
 * e pode ser NULL caso o chamador não deseje verificar o código de status.
 */

#include "matriz.h"

/** Códigos de status retornados pelas funções do módulo de filtros. */
typedef enum {
    FIL_SUCESSO,            /**< Operação concluída sem erros. */
    FIL_PONTEIRO_NULL,      /**< Ponteiro obrigatório recebido é NULL. */
    FIL_KERNEL_INVALIDO,    /**< Kernel com dimensões pares ou inválidas. */
    FIL_PARAMETRO_INVALIDO, /**< Parâmetro numérico fora dos limites permitidos. */
    FIL_SEM_MEMORIA         /**< Falha de alocação de memória dinâmica. */
} FilStatus;

/**
 * @brief Aplica convolução bidimensional da matriz com um kernel.
 *
 * @details O kernel deve possuir dimensões ímpares (com centro bem definido).
 * Elementos fora das bordas da matriz são tratados replicando-se o pixel da borda.
 *
 * @param src    Matriz de entrada (um canal).
 * @param kernel Matriz contendo o kernel de convolução (dimensões ímpares).
 * @param status Ponteiro opcional para receber o código de status:
 *               - FIL_SUCESSO em caso de sucesso;
 *               - FIL_PONTEIRO_NULL se src ou kernel forem NULL;
 *               - FIL_KERNEL_INVALIDO se as dimensões do kernel forem pares;
 *               - FIL_SEM_MEMORIA se falhar a alocação de memória.
 * @return Nova Matriz com o resultado da convolução, ou NULL em caso de erro.
 * @note Deve ser liberada com mat_liberar().
 */
Matriz *fil_convolucao(const Matriz *src, const Matriz *kernel, FilStatus *status);

/**
 * @brief Aplica desfoque (blur) por média móvel numa janela de lado 2*raio+1.
 *
 * @param src    Matriz de entrada.
 * @param raio   Raio da janela de desfoque (deve ser >= 0).
 * @param status Ponteiro opcional para receber o código de status:
 *               - FIL_SUCESSO em caso de sucesso;
 *               - FIL_PONTEIRO_NULL se src for NULL;
 *               - FIL_PARAMETRO_INVALIDO se raio < 0;
 *               - FIL_SEM_MEMORIA se falhar a alocação de memória.
 * @return Nova Matriz desfocada, ou NULL em caso de erro.
 * @note Deve ser liberada com mat_liberar().
 */
Matriz *fil_blur(const Matriz *src, int raio, FilStatus *status);

/**
 * @brief Aplica filtro de nitidez (sharpen) para realçar bordas e detalhes.
 *
 * @param src    Matriz de entrada.
 * @param status Ponteiro opcional para receber o código de status:
 *               - FIL_SUCESSO em caso de sucesso;
 *               - FIL_PONTEIRO_NULL se src for NULL;
 *               - FIL_SEM_MEMORIA se falhar a alocação de memória.
 * @return Nova Matriz com detalhes realçados, ou NULL em caso de erro.
 * @note Deve ser liberada com mat_liberar().
 */
Matriz *fil_sharpen(const Matriz *src, FilStatus *status);

/**
 * @brief Detecta bordas através do operador de Sobel (magnitude dos gradientes).
 *
 * @details Calcula os gradientes horizontal (Gx) e vertical (Gy) através
 * de convoluções com kernels de Sobel 3x3 e combina-os como sqrt(Gx^2 + Gy^2).
 * Os valores resultantes podem ultrapassar 255.0.
 *
 * @param src    Matriz de entrada.
 * @param status Ponteiro opcional para receber o código de status:
 *               - FIL_SUCESSO em caso de sucesso;
 *               - FIL_PONTEIRO_NULL se src for NULL;
 *               - FIL_SEM_MEMORIA se falhar a alocação de memória.
 * @return Nova Matriz com a magnitude das bordas, ou NULL em caso de erro.
 * @note Deve ser liberada com mat_liberar().
 */
Matriz *fil_sobel(const Matriz *src, FilStatus *status);

/**
 * @brief Binariza a matriz com base em um limiar (thresholding).
 *
 * @details Elementos com valor maior ou igual a @p limiar tornam-se 255.0;
 * elementos com valor estritamente menor tornam-se 0.0.
 *
 * @param src    Matriz de entrada.
 * @param limiar Valor de corte para a binarização.
 * @param status Ponteiro opcional para receber o código de status:
 *               - FIL_SUCESSO em caso de sucesso;
 *               - FIL_PONTEIRO_NULL se src for NULL;
 *               - FIL_SEM_MEMORIA se falhar a alocação de memória.
 * @return Nova Matriz binarizada (valores 0.0 ou 255.0), ou NULL em caso de erro.
 * @note Deve ser liberada com mat_liberar().
 */
Matriz *fil_limiar(const Matriz *src, double limiar, FilStatus *status);

#endif
