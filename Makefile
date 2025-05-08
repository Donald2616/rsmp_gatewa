# Chemins des répertoires
SRC_DIR = src/rsmp/sxl/display
TEST_DIR = $(SRC_DIR)/test
COMMANDS_DIR = $(SRC_DIR)/commands
CORE_DIR = $(SRC_DIR)/core
FIELDS_DIR = src/rsmp/fields
SXL_DIR = src/rsmp/sxl
VARS_DIR = src/rsmp/vars
LIB_DIR = lib

# Nom de l'exécutable de test
TEST_EXEC = test_disp_m31

# Bibliothèques
LIBS = -lrsmp -ljson-c

# Options de compilation
CFLAGS = -Wall
LDFLAGS = -L$(LIB_DIR) $(LIBS)

# Règle par défaut
all: $(TEST_EXEC)

# Règle pour compiler le test
$(TEST_EXEC): $(TEST_DIR)/test_disp_m31.c
	gcc $(CFLAGS) -o $(TEST_EXEC) $^ \
		-I$(COMMANDS_DIR) \
		-I$(CORE_DIR) \
		-I$(FIELDS_DIR) \
		-I$(SXL_DIR) \
		-I$(VARS_DIR) \
		-I./src \
		$(LDFLAGS)

# Règle pour exécuter le test
run: $(TEST_EXEC)
	./$(TEST_EXEC)

# Règle de nettoyage
clean:
	rm -f $(TEST_EXEC)
