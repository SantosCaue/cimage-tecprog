/**
 * @file image_transform.c
 * @brief Implementação das transformações geométricas de imagem.
 *
 * Cada transformação geométrica é expressa como uma operação sobre matrizes:
 *
 *  - Espelhar vertical   → P_lin × canal       (P_lin inverte as linhas)
 *  - Espelhar horizontal → canal × P_col        (P_col inverte as colunas)
 *  - Rotacionar 90° CW  → transposta(canal) × P_col
 *                          (equivale a transpor e depois espelhar horizontalmente)
 *
 * Para cada canal da imagem (1 ou 3) são realizadas as seguintes etapas:
 *   1. A imagem é copiada pixel a pixel para uma Matriz de doubles (altura×largura).
 *   2. A operação matricial é aplicada com as funções de matriz.h.
 *   3. O resultado (Matriz de doubles) é escrito de volta como canal da imagem destino.
 *
 * O corte (crop) não tem representação matricial direta com a API atual e
 * continua sendo feito pixel a pixel.
 */

#include "image_transform.h"
#include "matriz.h"
#include <stdlib.h>

/* -----------------------------------------------------------------------
 * Funções auxiliares internas
 * --------------------------------------------------------------------- */

/**
 * @brief Cria uma Matriz de permutação anti-diagonal de ordem n.
 *
 * @details A matriz de permutação reversa J de ordem n é a identidade com
 * as colunas na ordem inversa: J[i][j] = 1 se i+j == n-1, 0 caso contrário.
 * Multiplicar à esquerda (J × A) inverte as linhas de A.
 * Multiplicar à direita (A × J) inverte as colunas de A.
 *
 * @param n Ordem da matriz.
 * @return Nova Matriz de permutação, ou NULL se faltar memória.
 * @note Deve ser liberada com mat_liberar().
 */
static Matriz *criar_permutacao(int n) {
    Matriz *p = mat_criar(n, n);
    if (p == NULL) return NULL;
    for (int i = 0; i < n; i++) {
        mat_set(p, i, n - 1 - i, 1.0);
    }
    return p;
}

/**
 * @brief Copia um canal de uma Image para uma Matriz de doubles.
 *
 * @details Itera sobre todos os pixels e extrai o byte correspondente ao
 * canal @p c, convertendo para double. A Matriz devolvida tem dimensão
 * altura × largura.
 *
 * @param img Imagem fonte.
 * @param c   Índice do canal a extrair (0=R, 1=G, 2=B, ou 0 em imagem cinza).
 * @return Nova Matriz com os valores do canal, ou NULL se faltar memória.
 * @note Deve ser liberada com mat_liberar().
 */
static Matriz *canal_para_matriz(const Image *img, int c) {
    int alt = img_consul_altura(img);
    int lar = img_consul_largura(img);
    Matriz *m = mat_criar(alt, lar);
    if (m == NULL) return NULL;
    unsigned char bytes[3];
    for (int i = 0; i < alt; i++) {
        for (int j = 0; j < lar; j++) {
            img_ler_pixel(img, i, j, bytes);
            mat_set(m, i, j, (double)bytes[c]);
        }
    }
    return m;
}

/**
 * @brief Escreve uma Matriz de doubles de volta como canal de uma Image.
 *
 * @details Para cada posição (i,j), lê o pixel atual da imagem destino,
 * substitui o byte do canal @p c pelo valor da Matriz (clampado a [0,255])
 * e reescreve o pixel.
 *
 * @param img  Imagem destino.
 * @param m    Matriz com os novos valores do canal.
 * @param c    Índice do canal a escrever.
 */
static void matriz_para_canal(Image *img, const Matriz *m, int c) {
    int alt = img_consul_altura(img);
    int lar = img_consul_largura(img);
    unsigned char bytes[3] = {0, 0, 0};
    for (int i = 0; i < alt; i++) {
        for (int j = 0; j < lar; j++) {
            double val = 0.0;
            mat_get(m, i, j, &val);
            /* clamp para [0, 255] antes de converter para unsigned char */
            if (val < 0.0)   val = 0.0;
            if (val > 255.0) val = 255.0;
            img_ler_pixel(img, i, j, bytes);
            bytes[c] = (unsigned char)val;
            img_escrever_pixel(img, i, j, bytes);
        }
    }
}

/* -----------------------------------------------------------------------
 * Cortar
 * --------------------------------------------------------------------- */

/**
 * @brief Recorta uma região retangular da imagem.
 *
 * @details Copia pixel a pixel a sub-região definida por
 * (linha_inicial, coluna_inicial) com dimensões altura × largura.
 * Não há representação desta operação como produto matricial simples
 * com a API atual, portanto é mantida a implementação direta.
 *
 * @param img            Imagem fonte.
 * @param linha_inicial  Linha do canto superior esquerdo do corte.
 * @param coluna_inicial Coluna do canto superior esquerdo do corte.
 * @param altura         Altura do corte em pixels.
 * @param largura        Largura do corte em pixels.
 * @param status         Código de erro opcional (pode ser NULL).
 * @return Nova imagem com o corte, ou NULL em caso de erro.
 */
Image *cortar(const Image *img, int linha_inicial, int coluna_inicial,
              int altura, int largura, ImgStatus *status) {
    if (img == NULL) {
        if (status != NULL) *status = IMG_PONTEIRO_NULL;
        return NULL;
    }
    if (linha_inicial < 0 || coluna_inicial < 0 ||
        largura <= 0 || altura <= 0 ||
        linha_inicial + altura > img_consul_altura(img) ||
        coluna_inicial + largura > img_consul_largura(img)) {
        if (status != NULL) *status = IMG_FORA_DA_IMAGEM;
        return NULL;
    }

    Image *img_cortada = img_criar_imagem(largura, altura, img_consul_canal(img));
    if (img_cortada == NULL) {
        if (status != NULL) *status = IMG_SEM_MEMORIA;
        return NULL;
    }

    unsigned char bytes[3];
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            img_ler_pixel(img, linha_inicial + i, coluna_inicial + j, bytes);
            img_escrever_pixel(img_cortada, i, j, bytes);
        }
    }

    if (status != NULL) *status = IMG_SUCESSO;
    return img_cortada;
}

/* -----------------------------------------------------------------------
 * Espelhar vertical  →  P × canal  (inverte linhas)
 * --------------------------------------------------------------------- */

/**
 * @brief Espelha a imagem verticalmente (inverte a ordem das linhas).
 *
 * @details Para cada canal c, a operação é realizada como um produto
 * matricial:
 *
 *     canal_dst = P_linhas × canal_src
 *
 * onde P_linhas é a matriz de permutação anti-diagonal de ordem `altura`.
 * Multiplicar pela esquerda pela matriz de permutação inverte as linhas
 * da imagem, o que é exatamente o espelhamento vertical.
 *
 * @param img    Imagem fonte.
 * @param status Código de erro opcional (pode ser NULL).
 * @return Nova imagem espelhada verticalmente, ou NULL em caso de erro.
 */
Image *espelhar_vertical(const Image *img, ImgStatus *status) {
    if (img == NULL) {
        if (status != NULL) *status = IMG_PONTEIRO_NULL;
        return NULL;
    }

    int largura = img_consul_largura(img);
    int altura  = img_consul_altura(img);
    int canal   = img_consul_canal(img);

    Image *dst = img_criar_imagem(largura, altura, canal);
    if (dst == NULL) {
        if (status != NULL) *status = IMG_SEM_MEMORIA;
        return NULL;
    }

    /* P inverte linhas: P[i][j] = 1 se i+j == altura-1 */
    Matriz *P = criar_permutacao(altura);
    if (P == NULL) {
        img_liberar_imagem(dst);
        if (status != NULL) *status = IMG_SEM_MEMORIA;
        return NULL;
    }

    for (int c = 0; c < canal; c++) {
        /* extrai canal da imagem fonte como Matriz altura×largura */
        Matriz *m = canal_para_matriz(img, c);
        if (m == NULL) {
            mat_liberar(P);
            img_liberar_imagem(dst);
            if (status != NULL) *status = IMG_SEM_MEMORIA;
            return NULL;
        }

        /* P (altura×altura) × m (altura×largura) → resultado (altura×largura)
         * com as linhas em ordem invertida */
        Matriz *r = mat_mult_matriz(P, m);
        mat_liberar(m);
        if (r == NULL) {
            mat_liberar(P);
            img_liberar_imagem(dst);
            if (status != NULL) *status = IMG_SEM_MEMORIA;
            return NULL;
        }

        /* grava o canal resultante de volta na imagem destino */
        matriz_para_canal(dst, r, c);
        mat_liberar(r);
    }

    mat_liberar(P);
    if (status != NULL) *status = IMG_SUCESSO;
    return dst;
}

/* -----------------------------------------------------------------------
 * Espelhar horizontal  →  canal × P  (inverte colunas)
 * --------------------------------------------------------------------- */

/**
 * @brief Espelha a imagem horizontalmente (inverte a ordem das colunas).
 *
 * @details Para cada canal c, a operação é realizada como um produto
 * matricial:
 *
 *     canal_dst = canal_src × P_colunas
 *
 * onde P_colunas é a matriz de permutação anti-diagonal de ordem `largura`.
 * Multiplicar pela direita pela matriz de permutação inverte as colunas
 * da imagem, produzindo o espelhamento horizontal.
 *
 * @param img    Imagem fonte.
 * @param status Código de erro opcional (pode ser NULL).
 * @return Nova imagem espelhada horizontalmente, ou NULL em caso de erro.
 */
Image *espelhar_horizontal(const Image *img, ImgStatus *status) {
    if (img == NULL) {
        if (status != NULL) *status = IMG_PONTEIRO_NULL;
        return NULL;
    }

    int largura = img_consul_largura(img);
    int altura  = img_consul_altura(img);
    int canal   = img_consul_canal(img);

    Image *dst = img_criar_imagem(largura, altura, canal);
    if (dst == NULL) {
        if (status != NULL) *status = IMG_SEM_MEMORIA;
        return NULL;
    }

    /* P inverte colunas: P[i][j] = 1 se i+j == largura-1 */
    Matriz *P = criar_permutacao(largura);
    if (P == NULL) {
        img_liberar_imagem(dst);
        if (status != NULL) *status = IMG_SEM_MEMORIA;
        return NULL;
    }

    for (int c = 0; c < canal; c++) {
        /* extrai canal da imagem fonte como Matriz altura×largura */
        Matriz *m = canal_para_matriz(img, c);
        if (m == NULL) {
            mat_liberar(P);
            img_liberar_imagem(dst);
            if (status != NULL) *status = IMG_SEM_MEMORIA;
            return NULL;
        }

        /* m (altura×largura) × P (largura×largura) → resultado (altura×largura)
         * com as colunas em ordem invertida */
        Matriz *r = mat_mult_matriz(m, P);
        mat_liberar(m);
        if (r == NULL) {
            mat_liberar(P);
            img_liberar_imagem(dst);
            if (status != NULL) *status = IMG_SEM_MEMORIA;
            return NULL;
        }

        matriz_para_canal(dst, r, c);
        mat_liberar(r);
    }

    mat_liberar(P);
    if (status != NULL) *status = IMG_SUCESSO;
    return dst;
}

/* -----------------------------------------------------------------------
 * Rotacionar 90° horário  →  transposta(canal) × P_col
 * --------------------------------------------------------------------- */

/**
 * @brief Rotaciona a imagem 90 graus no sentido horário.
 *
 * @details A rotação 90° CW de uma matriz M de dimensão alt×lar é
 * equivalente a transpor M (obtendo lar×alt) e depois espelhar
 * horizontalmente o resultado (inverter colunas). Usando nossas funções:
 *
 *     transposta = mat_transpor(canal_src)          → lar×alt
 *     canal_dst  = mat_mult_matriz(transposta, P)   → lar×alt  (P de ordem alt)
 *
 * A imagem destino tem dimensões trocadas: largura_dst = altura_src,
 * altura_dst = largura_src.
 *
 * @param img    Imagem fonte.
 * @param status Código de erro opcional (pode ser NULL).
 * @return Nova imagem rotacionada 90° CW, ou NULL em caso de erro.
 */
Image *rotacionar(const Image *img, ImgStatus *status) {
    if (img == NULL) {
        if (status != NULL) *status = IMG_PONTEIRO_NULL;
        return NULL;
    }

    int largura = img_consul_largura(img);
    int altura  = img_consul_altura(img);
    int canal   = img_consul_canal(img);

    /* após rotação 90° CW: nova largura = altura original, nova altura = largura original */
    Image *dst = img_criar_imagem(altura, largura, canal);
    if (dst == NULL) {
        if (status != NULL) *status = IMG_SEM_MEMORIA;
        return NULL;
    }

    /* P de ordem `altura` para espelhar horizontalmente a transposta (lar×alt) × P(alt×alt) */
    Matriz *P = criar_permutacao(altura);
    if (P == NULL) {
        img_liberar_imagem(dst);
        if (status != NULL) *status = IMG_SEM_MEMORIA;
        return NULL;
    }

    for (int c = 0; c < canal; c++) {
        /* passo 1: extrai canal como Matriz alt×lar */
        Matriz *m = canal_para_matriz(img, c);
        if (m == NULL) {
            mat_liberar(P);
            img_liberar_imagem(dst);
            if (status != NULL) *status = IMG_SEM_MEMORIA;
            return NULL;
        }

        /* passo 2: transposta → Matriz lar×alt */
        Matriz *t = mat_transpor(m);
        mat_liberar(m);
        if (t == NULL) {
            mat_liberar(P);
            img_liberar_imagem(dst);
            if (status != NULL) *status = IMG_SEM_MEMORIA;
            return NULL;
        }

        /* passo 3: t (lar×alt) × P (alt×alt) → r (lar×alt) com colunas invertidas
         * isso equivale a um espelhamento horizontal da transposta, completando
         * a rotação de 90° no sentido horário */
        Matriz *r = mat_mult_matriz(t, P);
        mat_liberar(t);
        if (r == NULL) {
            mat_liberar(P);
            img_liberar_imagem(dst);
            if (status != NULL) *status = IMG_SEM_MEMORIA;
            return NULL;
        }

        matriz_para_canal(dst, r, c);
        mat_liberar(r);
    }

    mat_liberar(P);
    if (status != NULL) *status = IMG_SUCESSO;
    return dst;
}