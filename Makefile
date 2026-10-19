
NAME      := cimage

CC        ?= gcc
AR        ?= ar
CFLAGS    := -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude -Ipublic_library
LDLIBS    := -lm

SRC_DIR   := src
INC_DIR   := include
BUILD_DIR := build
DOCS_DIR  := docs
OBJ_DIR   := $(BUILD_DIR)/obj
BIN_DIR   := $(BUILD_DIR)/bin

PREFIX    ?= /usr/local

STATIC_LIB := $(BUILD_DIR)/lib$(NAME).a
SHARED_LIB := $(BUILD_DIR)/lib$(NAME).so

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

EXAMPLE_SRCS := $(wildcard examples/*.c)
EXAMPLE_BINS := $(patsubst examples/%.c,$(BIN_DIR)/%,$(EXAMPLE_SRCS))

TEST_SRCS := $(wildcard tests/*.c)
TEST_BINS := $(patsubst tests/%.c,$(BIN_DIR)/%,$(TEST_SRCS))

.PHONY: all static shared examples run run-shared test test-debug debug doc install uninstall clean clean-objs

all: static examples

#  Bibliotecas 

static: $(STATIC_LIB)

$(STATIC_LIB): $(OBJS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $^
	@echo "Biblioteca estática criada: $@"

shared: CFLAGS += -fPIC
shared: clean-objs $(SHARED_LIB)

$(SHARED_LIB): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^ $(LDLIBS)
	@echo "Biblioteca dinâmica criada: $@"

#  Objetos

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

# Exemplos e testes 

# Argumentos padrao do programa de exemplo (troque com: make run ARGS="a.png b.png sobel")
ARGS ?= examples/exemplo.png build/saida.png blur 2
PROGRAMA := $(BIN_DIR)/programa

# Compila (se necessario) e executa a aplicacao ligada a biblioteca ESTATICA
run: examples
	$(PROGRAMA) $(ARGS)

# Compila a aplicacao ligada a biblioteca DINAMICA e a executa
# (LD_LIBRARY_PATH faz o sistema achar libcimage.so em tempo de execucao)
run-shared: shared
	@mkdir -p $(BIN_DIR)
	$(CC) -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude -Ipublic_library examples/programa.c -L$(BUILD_DIR) -lcimage $(LDLIBS) -o $(PROGRAMA)_din
	LD_LIBRARY_PATH=$(BUILD_DIR) $(PROGRAMA)_din $(ARGS)

examples: $(EXAMPLE_BINS)

test: $(TEST_BINS)
	@for t in $(TEST_BINS); do \
		echo "==> $$t"; \
		$$t || exit 1; \
	done
	@echo "Todos os testes passaram."

$(BIN_DIR)/%: examples/%.c $(STATIC_LIB)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(STATIC_LIB) $(LDLIBS) -o $@

$(BIN_DIR)/%: tests/%.c $(STATIC_LIB)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(STATIC_LIB) $(LDLIBS) -o $@

# Testes com sanitizers (compila tudo com AddressSanitizer e UBSan e roda os testes)
test-debug: CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -g -O0 -Iinclude -Ipublic_library -fsanitize=address,undefined
test-debug: LDLIBS += -fsanitize=address,undefined
test-debug: clean test
	@$(MAKE) --no-print-directory clean

# Debug 

debug: CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -g -O0 -Iinclude -Ipublic_library -fsanitize=address,undefined
debug: LDLIBS += -fsanitize=address,undefined
debug: clean static

# Documentação 

doc:
	doxygen Doxyfile
	@echo "Documentação gerada em $(DOCS_DIR)/html/index.html"

# Instalação 

install: static shared
	install -d $(PREFIX)/lib $(PREFIX)/include/$(NAME)
	install -m 644 $(STATIC_LIB) $(PREFIX)/lib/
	install -m 755 $(SHARED_LIB) $(PREFIX)/lib/
	install -m 644 $(INC_DIR)/*.h $(PREFIX)/include/$(NAME)/

uninstall:
	rm -f $(PREFIX)/lib/lib$(NAME).a $(PREFIX)/lib/lib$(NAME).so
	rm -rf $(PREFIX)/include/$(NAME)

#  Limpeza 

clean-objs:
	rm -rf $(OBJ_DIR)

clean:
	rm -rf $(BUILD_DIR) $(DOCS_DIR)