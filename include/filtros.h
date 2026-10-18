#ifndef FILTROS_H
#define FILTROS_H

#include "matriz.h"

/*
 * Filtros sobre Matriz (um canal por vez).
 * A entrada não é alterada: cada filtro devolve uma Matriz nova,
 * que deve ser liberada com mat_liberar. Em erro, devolve NULL.
 * "status" pode ser NULL.
 */

typedef enum {
    FIL_SUCESSO,
    FIL_PONTEIRO_NULL,
    FIL_KERNEL_INVALIDO,
    FIL_PARAMETRO_INVALIDO,
    FIL_SEM_MEMORIA
} FilStatus;

/* Convolução de src com kernel (bordas: repete o pixel da borda).
 * kernel: linhas e colunas ímpares.
 * Erros: PONTEIRO_NULL, KERNEL_INVALIDO, SEM_MEMORIA. */
Matriz *fil_convolucao(const Matriz *src, const Matriz *kernel, FilStatus *status);

/* Desfoque por média numa janela de lado 2*raio+1. raio >= 0.
 * Erros: PONTEIRO_NULL, PARAMETRO_INVALIDO (raio < 0), SEM_MEMORIA. */
Matriz *fil_blur(const Matriz *src, int raio, FilStatus *status);

/* Realça detalhes e bordas.
 * Erros: PONTEIRO_NULL, SEM_MEMORIA. */
Matriz *fil_sharpen(const Matriz *src, FilStatus *status);

/* Detecta bordas (filtro de Sobel). Valores podem passar de 255.
 * Erros: PONTEIRO_NULL, SEM_MEMORIA. */
Matriz *fil_sobel(const Matriz *src, FilStatus *status);

/* Binariza: elemento >= limiar vira 255, senão 0.
 * Erros: PONTEIRO_NULL, SEM_MEMORIA. */
Matriz *fil_limiar(const Matriz *src, double limiar, FilStatus *status);

#endif
