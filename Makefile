# ─────────────────────────────────────────────────────────────────────────────
#  STARBLAST  –  Makefile
#  Build environment: MSYS2 MinGW64 UCRT
#  Usage:  make          → builds bin/starblast.exe
#          make run      → builds then launches the game
#          make web      → builds WebAssembly version into web/
#          make clean    → removes build artefacts
# ─────────────────────────────────────────────────────────────────────────────

CC      := gcc
TARGET  := bin/starblast.exe

# ── Source & include ──────────────────────────────────────────────────────────
SRC_DIR  := src
INC_DIR  := include
OBJ_DIR  := obj

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# ── Compiler flags ────────────────────────────────────────────────────────────
CFLAGS  := -Wall -Wextra -O2 -std=c11 -I$(INC_DIR)

# ── Linker flags for raylib on MSYS2 UCRT ─────────────────────────────────────
LDFLAGS := -lraylib -lopengl32 -lgdi32 -lwinmm -lm

# ── Rules ─────────────────────────────────────────────────────────────────────
.PHONY: all run web clean

all: $(TARGET)

$(TARGET): $(OBJS) | bin
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "[OK]  Built $(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

bin:
	mkdir -p bin

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

run: all
	./$(TARGET)

# ── Web build (Emscripten) ────────────────────────────────────────────────────
WEB_DIR   := web
WEB_FLAGS := -Os -I$(INC_DIR) \
             -s USE_GLFW=3 \
             -s ASYNCIFY \
             -s TOTAL_MEMORY=67108864 \
             --shell-file shell.html

web: $(SRCS) | $(WEB_DIR)
	emcc $(SRCS) -o $(WEB_DIR)/index.html $(WEB_FLAGS) -lraylib
	@echo "[OK]  Web build in $(WEB_DIR)/"

$(WEB_DIR):
	mkdir -p $(WEB_DIR)

clean:
	rm -rf $(OBJ_DIR) bin $(WEB_DIR)
	@echo "[OK]  Cleaned build artefacts"
