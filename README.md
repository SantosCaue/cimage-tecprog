## cimage-tecprog

Biblioteca modular em C para representação, transformação, processamento e álgebra matricial sobre imagens digitais (em tons de cinza ou RGB).

Desenvolvido no contexto da disciplina **MAC0216 - Técnicas de Programação** (IME-USP).

---

## Sumário

- [Visão Geral](#-visão-geral)
- [Estrutura do Repositório](#-estrutura-do-repositório)
- [Módulos da Biblioteca](#-módulos-da-biblioteca)
- [Compilação e Uso](#-compilação-e-uso)
- [Testes](#-testes)
- [Documentação (Doxygen)](#-documentação-doxygen)
- [Padrões de Nomenclatura](#-padrões-de-nomenclatura)

---

## Visão Geral

A biblioteca adota tipos de dados opacos (`Image` e `Matriz`) e tratamento explícito de erros via enums de status, promovendo encapsulamento e robustez.

As imagens podem ter 1 canal (tons de cinza) ou 3 canais (RGB). Cada componente de pixel é armazenado em `unsigned char` (0 a 255). No módulo de matrizes, os elementos são números em ponto flutuante de precisão dupla (`double`), permitindo que filtros de convolução e operações algébricas preservem precisão numérica.

---

## Estrutura do Repositório

```text
.
├── include/              # Cabeçalhos públicos (.h)
│   ├── filtros.h         # Filtros espaciais (convolução, blur, sobel, etc.)
│   ├── image.h           # Tipo Image e manipulação básica de pixels
│   ├── image_convert.h   # Conversão entre Image e Matriz
│   ├── image_io.h        # Carregamento e salvamento de arquivos
│   ├── image_transform.h # Transformações espaciais (corte, espelho, rotação)
│   └── matriz.h          # Tipo Matriz e operações algébricas lineares
├── src/                  # Implementações em C (.c)
│   ├── filtros.c
│   ├── image.c
│   ├── image_convert.c
│   ├── image_internal.h  # Struct Image concreta (visível apenas para o núcleo)
│   ├── image_io.c
│   ├── image_transform.c
│   └── matriz.c
├── public_library/       # Bibliotecas públicas de terceiros (stb_image)
│   ├── stb_image.h       # Decodificação de arquivos de imagem (PNG, JPG, BMP)
│   └── stb_image_write.h # Codificação e escrita em PNG
├── tests/                # Suíte de testes automatizados
│   ├── test_filtros.c    # Testes unitários do módulo de filtros
│   └── test_transform.c  # Testes unitários do módulo de transformações
├── Doxyfile              # Configuração do Doxygen para geração de docs
├── Makefile              # Automação de compilação, testes e instalação
└── README.md             # Documentação principal do projeto
```

---

## Módulos da Biblioteca

### 1. `Image` (`image.h`)
Gerencia o ciclo de vida e o acesso a pixels de uma imagem.
- `img_criar_imagem`: aloca uma nova imagem preta com largura, altura e número de canais (1 ou 3).
- `img_liberar_imagem`: libera a memória ocupada.
- `img_consul_largura`, `img_consul_altura`, `img_consul_canal`: consultas de propriedades.
- `img_ler_pixel`, `img_escrever_pixel`: acesso e modificação dos canais de um pixel específico.

### 2. `Matriz` (`matriz.h`)
Estrutura algébrica genérica de números `double`.
- **Básicos**: `mat_criar`, `mat_liberar`, `mat_linhas`, `mat_colunas`, `mat_get`, `mat_set`.
- **Operações**:
  - `mat_mult_escalar`: multiplicação in-place por um valor escalar.
  - `mat_mult_vetor`: produto matriz-vetor ($M \times v$).
  - `mat_mult_matriz`: produto matricial clássico ($A \times B$).
  - `mat_somar`: soma elemento a elemento ($A + B$).
  - `mat_transpor`: cálculo da matriz transposta ($M^T$).

### 3. Transformações Espaciais (`image_transform.h`)
Operações geométricas sobre imagens, modeladas internamente via permutações e operações matriciais:
- `img_cortar`: recorte retangular de uma sub-região da imagem.
- `img_espelhar_vertical`: inversão da ordem das linhas ($P_{\text{lin}} \times \text{canal}$).
- `img_espelhar_horizontal`: inversão da ordem das colunas ($\text{canal} \times P_{\text{col}}$).
- `img_rotacionar`: rotação de 90° no sentido horário ($\text{canal}^T \times P$).

### 4. Filtros Digitais (`filtros.h`)
Processamento sobre matrizes de canal único:
- `fil_convolucao`: convolução bidimensional genérica com kernel ímpar (tratamento de bordas por clamp/replicação).
- `fil_blur`: filtro de desfoque por média móvel numa janela $(2r+1) \times (2r+1)$.
- `fil_sharpen`: realce laplaciano de bordas e detalhes.
- `fil_sobel`: detecção de bordas via magnitude de gradientes horizontais e verticais ($\sqrt{G_x^2 + G_y^2}$).
- `fil_limiar`: binarização baseada em valor de corte.

### 5. Conversão e E/S (`image_convert.h`, `image_io.h`)
- `img_para_matriz` e `matrizes_para_img` (`img_de_matrizes`): conversão bidirecional entre canais `unsigned char` e matrizes `double`.
- `img_carregar_imagem`: carrega formatos comuns (PNG, JPG, BMP) convertendo para 3 canais RGB.
- `img_salvar_imagem`: codifica e salva a imagem em arquivo formato PNG.

---

## Compilação e Uso

O projeto requer um compilador com suporte a **C11** (como `gcc` ou `clang`) e o utilitário `make`.

### Gerar a biblioteca estática (`libcimage.a`)
```bash
make
# ou:
make static
```
A biblioteca será gerada em `build/libcimage.a`.

### Gerar a biblioteca dinâmica (`libcimage.so`)
```bash
make shared
```
A biblioteca compartilhada será gerada em `build/libcimage.so`.

### Modo de depuração (Debug com Sanitizers)
Compila com `-g -O0` e habilita AddressSanitizer e UndefinedBehaviorSanitizer:
```bash
make debug
```

### Limpeza de artefatos
```bash
make clean
```

---

## Testes

Para compilar e rodar a suíte de testes automatizados:

```bash
make test
```

Os binários dos testes são gerados em `build/bin/` e executados em sequência.

---

## Documentação (Doxygen)

O projeto está integralmente documentado no formato Doxygen. Para gerar o relatório em HTML:

1. Certifique-se de ter o `doxygen` instalado (`sudo apt install doxygen` ou equivalente).
2. Execute:
   ```bash
   make doc
   ```
3. Abra a documentação no navegador:
   ```bash
   xdg-open docs/html/index.html
   ```

---

## Padrões de Nomenclatura

A base de código segue as convenções estabelecidas:

| Elemento | Convenção | Exemplo |
|---|---|---|
| Tipos opacos | `PascalCase` | `Image`, `Matriz` |
| Códigos de status | `PREFIXO_SCREAMING_SNAKE` | `IMG_SUCESSO`, `MAT_SEM_MEMORIA`, `FIL_KERNEL_INVALIDO` |
| Funções públicas | `prefixo_verbo_objeto` | `img_criar_imagem`, `mat_mult_matriz`, `fil_sobel` |
| Funções internas (`static`) | `snake_case` descritivo | `limitar`, `criar_permutacao`, `extrair_patch` |
