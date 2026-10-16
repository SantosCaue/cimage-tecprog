
NAME      := imgedit

CC        ?= gcc
AR        ?= ar
CFLAGS    := -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude
LDLIBS    := -lm

SRC_DIR   := src
INC_DIR   := include
BUILD_DIR := build
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

.PHONY: all static shared examples test debug install uninstall clean

all: static

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

# Debug 

debug: CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -g -O0 -Iinclude -fsanitize=address,undefined
debug: LDLIBS += -fsanitize=address,undefined
debug: clean static

# Instalação 

install: static
	install -d $(PREFIX)/lib $(PREFIX)/include/$(NAME)
	install -m 644 $(STATIC_LIB) $(PREFIX)/lib/
	install -m 644 $(INC_DIR)/*.h $(PREFIX)/include/$(NAME)/

uninstall:
	rm -f $(PREFIX)/lib/lib$(NAME).a
	rm -rf $(PREFIX)/include/$(NAME)

#  Limpeza 

clean-objs:
	rm -rf $(OBJ_DIR)

clean:
	rm -rf $(BUILD_DIR)