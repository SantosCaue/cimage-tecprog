#include <stdio.h>
#include "../include/image_transform.h"
#include "../include/image.h"

void test_cortar_valor_valido (){
    printf("Teste: Cortar - entrada válida\n");
    Image *img = img_criar_imagem(100, 100, 3);
    ImgStatus status = IMG_SUCESSO;

    Image *img_cortada = cortar(img, 10, 10, 40, 60, &status);
    if(status == IMG_SUCESSO && img_cortada != NULL && img_consul_altura(img_cortada) == 40 && img_consul_largura(img_cortada) == 60)
        printf("Sucesso\n");
    else
        printf("Erro\n");
    
    if (img_cortada) img_liberar_imagem(img_cortada);
    img_liberar_imagem(img);
}

void test_cortar_valor_invalido (){
    printf("Teste: Cortar - entrada inválida\n");

    Image *img = img_criar_imagem(100, 100, 3);
    ImgStatus status = IMG_SUCESSO;

    Image *img_cortada = cortar(img, 90, 90, 40, 60, &status);
    if(status == IMG_FORA_DA_IMAGEM && img_cortada == NULL)
        printf("Sucesso\n");
    else
        printf("Erro\n");

    if (img_cortada) img_liberar_imagem(img_cortada);
    img_liberar_imagem(img);

    printf("Teste: Cortar - valor nulo\n");
    Image *corte_nulo = cortar(NULL, 0, 0, 30, 20, &status);
    if(status == IMG_PONTEIRO_NULL && corte_nulo == NULL)
        printf("Sucesso\n");
    else
        printf("Erro\n");
}

void test_cortar_valor_limite(){
    printf("Teste: Cortar - valor limite\n");
  
    Image *img = img_criar_imagem(100, 100, 3);
    ImgStatus status = IMG_SUCESSO;

    Image *img_cortada = cortar(img, 0, 0, 100, 100, &status);

    if(status == IMG_SUCESSO && img_cortada != NULL && img_consul_altura(img_cortada) == 100 && img_consul_largura(img_cortada) == 100)
        printf("Sucesso\n");
    else
        printf("Erro\n");

    if (img_cortada) img_liberar_imagem(img_cortada);
    img_liberar_imagem(img);
}

void test_espelhar_vertical_entrada_valida(){
    printf("Teste: espelhar_vertical - entrada válida\n");

    Image *img = img_criar_imagem(100, 100, 3);
    ImgStatus status = IMG_SUCESSO;

    Image *img_espelhada = espelhar_vertical(img, &status);

    if(status == IMG_SUCESSO && img_espelhada != NULL)
        printf("Sucesso\n");
    else
        printf("Erro\n");

    if (img_espelhada) img_liberar_imagem(img_espelhada);
    img_liberar_imagem(img);
}

void test_espelhar_vertical_entrada_invalida(){
    printf("Teste: espelhar_vertical - entrada nula\n");

    ImgStatus status = IMG_SUCESSO;

    Image *img_espelhada = espelhar_vertical(NULL, &status);

    if(status == IMG_PONTEIRO_NULL && img_espelhada == NULL)
        printf("Sucesso\n");
    else
        printf("Erro\n");

    if (img_espelhada) img_liberar_imagem(img_espelhada);
}

void test_espelhar_horizontal_entrada_valida(){
    printf("Teste: espelhar_horizontal - entrada válida\n");

    Image *img = img_criar_imagem(100, 100, 3);
    ImgStatus status = IMG_SUCESSO;

    Image *img_espelhada = espelhar_horizontal(img, &status);

    if(status == IMG_SUCESSO && img_espelhada != NULL)
        printf("Sucesso\n");
    else
        printf("Erro\n");

    if (img_espelhada) img_liberar_imagem(img_espelhada);
    img_liberar_imagem(img);
}

void test_espelhar_horizontal_entrada_invalida(){
    printf("Teste: espelhar_horizontal - entrada nula\n");

    ImgStatus status = IMG_SUCESSO;

    Image *img_espelhada = espelhar_horizontal(NULL, &status);

    if(status == IMG_PONTEIRO_NULL && img_espelhada == NULL)
        printf("Sucesso\n");
    else
        printf("Erro\n");

    if (img_espelhada) img_liberar_imagem(img_espelhada);
}

void test_rotacionar_entrada_valida(){
    printf("Teste: rotacionar - entrada válida\n");

    Image *img = img_criar_imagem(100, 100, 3);
    ImgStatus status = IMG_SUCESSO;

    Image *img_espelhada = rotacionar(img, &status);

    if(status == IMG_SUCESSO && img_espelhada != NULL)
        printf("Sucesso\n");
    else
        printf("Erro\n");

    if (img_espelhada) img_liberar_imagem(img_espelhada);
    img_liberar_imagem(img);
}

void test_rotacionar_entrada_invalida(){
    printf("Teste: rotacionar - entrada nula\n");

    ImgStatus status = IMG_SUCESSO;

    Image *img_espelhada = rotacionar(NULL, &status);

    if(status == IMG_PONTEIRO_NULL && img_espelhada == NULL)
        printf("Sucesso\n");
    else
        printf("Erro\n");

    if (img_espelhada) img_liberar_imagem(img_espelhada);
}

int main(){

    test_cortar_valor_valido();
    test_cortar_valor_invalido();
    test_cortar_valor_limite();
    
    test_espelhar_vertical_entrada_valida();
    test_espelhar_vertical_entrada_invalida();

    test_espelhar_horizontal_entrada_valida();
    test_espelhar_horizontal_entrada_invalida();

    test_rotacionar_entrada_valida();
    test_rotacionar_entrada_invalida();

    return 0;
}