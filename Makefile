CC = gcc

CFLAGS = -std=gnu11 -Wall -Wextra -O2 -Iinclude

LDFLAGS = -lraylib -lm -lpthread -ldl -lrt -lX11

SRC = \
	src/main.c \
	src/menu.c \
	src/dialogo.c \
	src/editor.c \
	src/ritual.c \
	src/arquivos.c \
	src/rsa.c

OUT = escriba_arcano

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)

rebuild: clean all

.PHONY: all run clean rebuild