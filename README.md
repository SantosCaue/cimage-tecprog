# cimage-tecprog

Biblioteca modular em C para representação, transformação, processamento e álgebra matricial sobre imagens digitais (em tons de cinza ou RGB).

Desenvolvida no contexto da disciplina **MAC0216 - Técnicas de Programação** (IME-USP).

---

## Sumário

- [Visão Geral](#visão-geral)
- [Estrutura do Repositório](#estrutura-do-repositório)
- [Módulos da Biblioteca](#módulos-da-biblioteca)
- [Compilação e Uso](#compilação-e-uso)
- [Programa de Exemplo](#programa-de-exemplo)
- [Testes](#testes)
- [Instalação](#instalação)
- [Documentação (Doxygen)](#documentação-doxygen)
- [Alvos do Makefile](#alvos-do-makefile)
- [Padrões de Nomenclatura](#padrões-de-nomenclatura)

---

## Visão Geral

A biblioteca adota tipos de dados opacos (`Image` e `Matriz`) e tratamento explícito de erros via enums de status, promovendo encapsulamento e robustez.

As imagens podem ter 1 canal (tons de cinza) ou 3 canais (RGB). Cada componente de pixel é armazenado em `unsigned char` (0 a 255). No módulo de matrizes, os elementos são números em ponto flutuante de precisão dupla (`double`), permitindo que filtros de convolução e operações algébricas preservem precisão numérica.

---

## Estrutura do Repositório

```text
.
├── Doxyfile                # Configuração do Doxygen para geração de docs
├── Makefile                # Automação de compilação, testes, exemplos e instalação
├── README.md               # Documentação principal do projeto
├── examples/               # Aplicações de exemplo que usam a biblioteca
│   └── programa.c          # Programa de linha de comando (aplica filtros a uma imagem)
├── include/                # Cabeçalhos públicos (.h)
│   ├── filtros.h           # Filtros espaciais (convolução, blur, sobel, etc.)
│   ├── image.h             # Tipo Image e manipulação básica de pixels
│   ├── image_convert.h     # Conversão entre Image e Matriz
│   ├── image_io.h          # Carregamento e salvamento de arquivos
│   ├── image_transform.h   # Transformações espaciais (corte, espelho, rotação)
│   └── matriz.h            # Tipo Matriz e operações algébricas lineares
├── public_library/         # Bibliotecas públicas de terceiros (stb)
│   ├── stb_image.h         # Decodificação de arquivos de imagem (PNG, JPG, BMP)
│   └── stb_image_write.h   # Codificação e escrita em PNG
├── src/                    # Implementações em C (.c)
│   ├── filtros.c
│   ├── image.c
│   ├── image_convert.c
│   ├── image_internal.h    # Struct Image concreta (visível apenas para o núcleo)
│   ├── image_io.c
│   ├── image_transform.c
│   └── matriz.c
└── tests/                  # Suíte de testes automatizados
    ├── test_convert.c      # Testes do módulo de conversão
    ├── test_filtros.c      # Testes do módulo de filtros
    ├── test_image.c        # Testes do tipo Image
    ├── test_io.c           # Testes de carregamento e salvamento
    ├── test_matriz.c       # Testes do módulo de matrizes
    └── test_transform.c    # Testes do módulo de transformações
```

Os artefatos gerados pelo `make` ficam em `build/` (objetos em `build/obj/`, binários em `build/bin/`) e a documentação em `docs/`.

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

O projeto requer um compilador com suporte a **C11** (como `gcc` ou `clang`) e o utilitário `make`. O compilador e o arquivador podem ser trocados pelas variáveis `CC` e `AR` (por exemplo, `make CC=clang`).

### Compilação padrão (biblioteca estática + exemplos)
```bash
make
# equivale a:
make all
```
Compila a biblioteca estática em `build/libcimage.a` e os programas de `examples/` em `build/bin/`.

### Apenas a biblioteca estática (`libcimage.a`)
```bash
make static
```

### Biblioteca dinâmica (`libcimage.so`)
```bash
make shared
```
A biblioteca compartilhada será gerada em `build/libcimage.so`. Os objetos são recompilados com `-fPIC` automaticamente.

### Modo de depuração (Debug com Sanitizers)
Compila a biblioteca estática com `-g -O0` e habilita AddressSanitizer e UndefinedBehaviorSanitizer:
```bash
make debug
```

### Limpeza de artefatos
```bash
make clean        # remove build/ e docs/
make clean-objs   # remove apenas os objetos (build/obj/)
```

### Usando a biblioteca no seu código
Compilando diretamente a partir do repositório (ligação estática):
```bash
gcc -std=c11 -Iinclude meu_programa.c build/libcimage.a -lm -o meu_programa
```

Ou com a biblioteca dinâmica:
```bash
gcc -std=c11 -Iinclude meu_programa.c -Lbuild -lcimage -lm -o meu_programa
LD_LIBRARY_PATH=build ./meu_programa
```

---

## Programa de Exemplo

O diretório `examples/` contém `programa.c`, uma aplicação de linha de comando que usa a biblioteca. Todo arquivo `.c` em `examples/` é compilado automaticamente pelo `make` e gera um binário em `build/bin/`.

### Executar com a biblioteca estática
```bash
make run
```
Compila (se necessário) e executa o programa com os argumentos padrão:
`examples/exemplo.png build/saida.png blur 2`.

Para usar outros argumentos, passe `ARGS`:
```bash
make run ARGS="entrada.png build/saida.png sobel"
```

> O arquivo `examples/exemplo.png` não faz parte do repositório. Forneça uma imagem sua via `ARGS` ou coloque uma imagem com esse nome em `examples/`.

### Executar com a biblioteca dinâmica
```bash
make run-shared
```
Gera `libcimage.so`, liga o programa dinamicamente (binário `build/bin/programa_din`) e o executa com `LD_LIBRARY_PATH=build`, para que o sistema encontre a biblioteca em tempo de execução. Também aceita `ARGS`.

---

## Testes

Para compilar e rodar a suíte de testes automatizados:

```bash
make test
```

Cada arquivo de `tests/` gera um binário em `build/bin/`; os testes são executados em sequência e a execução é interrompida no primeiro que falhar. Ao final, se tudo passar, é exibida a mensagem `Todos os testes passaram.`

| Arquivo | Módulo testado |
|---|---|
| `test_image.c` | `Image` |
| `test_matriz.c` | `Matriz` |
| `test_transform.c` | Transformações espaciais |
| `test_filtros.c` | Filtros digitais |
| `test_convert.c` | Conversão `Image` ↔ `Matriz` |
| `test_io.c` | Carregamento e salvamento de arquivos |

### Testes com sanitizers
```bash
make test-debug
```
Recompila tudo com `-g -O0` e AddressSanitizer + UBSan, roda os testes e limpa `build/` e `docs/` ao final. Útil para detectar vazamentos de memória, acessos fora dos limites e comportamento indefinido.

---

## Instalação

Para instalar as bibliotecas (estática e dinâmica) e os cabeçalhos no sistema:

```bash
sudo make install
```

Por padrão, a instalação usa o prefixo `/usr/local`:

| Item | Destino |
|---|---|
| `libcimage.a`, `libcimage.so` | `$(PREFIX)/lib/` |
| Cabeçalhos `include/*.h` | `$(PREFIX)/include/cimage/` |

Para instalar em outro local (sem `sudo`, por exemplo):
```bash
make install PREFIX=$HOME/.local
```

Depois de instalada, a biblioteca pode ser usada assim:
```bash
gcc meu_programa.c -I$(PREFIX)/include/cimage -L$(PREFIX)/lib -lcimage -lm -o meu_programa
```

> Se instalar em um prefixo fora dos caminhos padrão do sistema, pode ser necessário incluir `$(PREFIX)/lib` em `LD_LIBRARY_PATH` (ou rodar `sudo ldconfig`, no caso de `/usr/local`) para usar a versão dinâmica.

Para remover:
```bash
sudo make uninstall
```

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

## Alvos do Makefile

| Alvo | Descrição |
|---|---|
| `make` / `make all` | Biblioteca estática + programas de exemplo |
| `make static` | Gera `build/libcimage.a` |
| `make shared` | Gera `build/libcimage.so` (com `-fPIC`) |
| `make examples` | Compila os programas de `examples/` em `build/bin/` |
| `make run` | Executa o exemplo ligado à biblioteca estática (aceita `ARGS="..."`) |
| `make run-shared` | Executa o exemplo ligado à biblioteca dinâmica (aceita `ARGS="..."`) |
| `make test` | Compila e executa todos os testes |
| `make test-debug` | Testes com AddressSanitizer e UBSan (limpa `build/` e `docs/` ao final) |
| `make debug` | Biblioteca estática em modo debug com sanitizers |
| `make doc` | Gera a documentação HTML com Doxygen em `docs/html/` |
| `make install` | Instala bibliotecas e cabeçalhos em `$(PREFIX)` (padrão `/usr/local`) |
| `make uninstall` | Remove os arquivos instalados |
| `make clean` | Remove `build/` e `docs/` |
| `make clean-objs` | Remove apenas `build/obj/` |

### Variáveis configuráveis

| Variável | Padrão | Uso |
|---|---|---|
| `CC` | `gcc` | Compilador C |
| `AR` | `ar` | Arquivador para a biblioteca estática |
| `PREFIX` | `/usr/local` | Destino de `make install` / `make uninstall` |
| `ARGS` | `examples/exemplo.png build/saida.png blur 2` | Argumentos do programa de exemplo em `make run` / `make run-shared` |

Todos os alvos compilam com `-std=c11 -Wall -Wextra -Wpedantic`. Os modos normais usam `-O2`; os modos de depuração usam `-g -O0` com sanitizers.

---

## Padrões de Nomenclatura

A base de código segue as convenções estabelecidas:

| Elemento | Convenção | Exemplo |
|---|---|---|
| Tipos opacos | `PascalCase` | `Image`, `Matriz` |
| Códigos de status | `PREFIXO_SCREAMING_SNAKE` | `IMG_SUCESSO`, `MAT_SEM_MEMORIA`, `FIL_KERNEL_INVALIDO` |
| Funções públicas | `prefixo_verbo_objeto` | `img_criar_imagem`, `mat_mult_matriz`, `fil_sobel` |
| Funções internas (`static`) | `snake_case` descritivo | `limitar`, `criar_permutacao`, `extrair_patch` |