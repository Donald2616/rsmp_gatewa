# === COMPILATION CONFIG ===
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -ljson-c

# === REPERTOIRES ===
BIN_DIR = bin
SRC_DIR = src
TEST_DIR = test

# === SOURCES COMMUNES ===
SRC_COMMON = $(SRC_DIR)/disp_json_generic.c

# === M31 ===
SRC_M31 = $(SRC_DIR)/disp_m31.c $(SRC_COMMON) $(TEST_DIR)/test_disp_m31.c
OUT_M31 = $(BIN_DIR)/test_disp_m31

# === M32 ===
SRC_M32 = $(SRC_DIR)/disp_m32.c $(SRC_COMMON) $(TEST_DIR)/test_disp_m32.c
OUT_M32 = $(BIN_DIR)/test_disp_m32

# === M40 ===
SRC_M40 = $(SRC_DIR)/disp_m40.c $(SRC_COMMON) $(TEST_DIR)/test_disp_m40.c
OUT_M40 = $(BIN_DIR)/test_disp_m40

# === M41 ===
SRC_M41 = $(SRC_DIR)/disp_m41.c $(SRC_COMMON) $(TEST_DIR)/test_disp_m41.c
OUT_M41 = $(BIN_DIR)/test_disp_m41

# === S20 ===
SRC_S20 = $(SRC_DIR)/disp_s20.c $(SRC_COMMON) $(TEST_DIR)/test_disp_s20.c
OUT_S20 = $(BIN_DIR)/test_disp_s20

# === S10 ===
SRC_S10 = $(SRC_DIR)/disp_s10.c $(SRC_COMMON) $(TEST_DIR)/test_disp_s10.c
OUT_S10 = $(BIN_DIR)/test_disp_s10

# === S31 ===
SRC_S31 = $(SRC_DIR)/disp_s31.c $(SRC_COMMON) $(TEST_DIR)/test_disp_s31.c
OUT_M31 = $(BIN_DIR)/test_disp_s31

# === CIBLES ===
all: $(OUT_M31) $(OUT_M32) $(OUT_M40) $(OUT_M41) $(OUT_S20) $(OUT_S10)

# Compilation générique
$(BIN_DIR)/test_%: $(SRC_DIR)/%.c $(SRC_COMMON) $(TEST_DIR)/test_%.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# === COMMANDES SIMPLES ===
run-m31: $(OUT_M31)
	./$(OUT_M31)

run-m32: $(OUT_M32)
	./$(OUT_M32)

run-m40: $(OUT_M40)
	./$(OUT_M40)

run-m41: $(OUT_M41)
	./$(OUT_M41)

run-s20: $(OUT_S20)
	./$(OUT_S20)

run-s10: $(OUT_S10)
	./$(OUT_S10)

# === S40 ===
SRC_S40 = $(SRC_DIR)/disp_s40.c $(SRC_COMMON) $(TEST_DIR)/test_disp_s40.c
OUT_S40 = $(BIN_DIR)/test_disp_s40

run-s40: $(OUT_S40)
	./$(OUT_S40)
all: $(OUT_M31) ... $(OUT_S40)

# === S30 ===
SRC_S30 = $(SRC_DIR)/disp_s30.c $(SRC_COMMON) $(TEST_DIR)/test_disp_s30.c
OUT_S30 = $(BIN_DIR)/test_disp_s30

# Ajoute la cible à "all"
all: $(OUT_M31) ... $(OUT_S30)

# Ajoute commande d’exécution
run-s30: $(OUT_S30)
	./$(OUT_S30)



# === CLEAN ===
clean:
	rm -rf $(BIN_DIR)

# === PHONY ===
.PHONY: all clean run-m31 run-m32 run-m40 run-m41 run-s20 run-s10
