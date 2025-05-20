# Compile setting for chat application

# Source directory
CUR_DIR := .
SRC_DIR := $(CUR_DIR)/src
INC_DIR := $(CUR_DIR)/inc
OBJ_DIR := $(CUR_DIR)/obj
BIN_DIR := $(CUR_DIR)/bin
LOG_DIR := $(CUR_DIR)/log

# Compiler and tools
CFLAGS = -pthread -Wall -g -I$(INC_DIR)

# Get all source files (*.c) in /src
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

# Convert *.c files to *.o and input to /obj
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Check and create the required directories if not exist yet
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

# Export file
EXP_FILE = $(BIN_DIR)/chatc

# Compile source code
all: $(EXP_FILE)

# Output object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/*.h
	$(COMPILE.c) $(CFLAGS) -c $< -o $@

# Export file
$(EXP_FILE): $(OBJ_FILES)
	$(LINK.o) $(CFLAGS) $(OBJ_FILES) -o $@
#	$(LINK.o) $(CFLAGS) $(OBJ_FILES) -o $@

# Run Valgrind to check for memory leak
check-mleak: $(EXP_FILE)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(EXP_FILE) 8000

# Check memory leak and output log using valgrind
log-mleak: $(EXP_FILE)
	mkdir -p $(LOG_DIR)
	nohup valgrind --leak-check=full --show-leak-kinds=all $(EXP_FILE) > $(LOG_DIR)/mleak.log

# Clean all compiled and log data
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	rm -rf $(LOG_DIR)

checkmake:
	@echo -e "CURDIR = \n ${CURDIR}"
	@echo -e "\nMAKE_VERSION = \n ${MAKE_VERSION}"
	@echo -e "\nMAKEFILE_LIST = \n $(MAKEFILE_LIST)"
	@echo -e "\nCOMPILE.c = \n $(COMPILE.c)"
	@echo -e "\nLINK.o = \n $(LINK.o)"
	@echo -e "\CFLAGS = \n $(CFLAGS)"
	
.PHONY: all clean check-mleak log-mleak checkmake

