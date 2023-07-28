# Project name, aloso name of executable file
PROJECT_NAME = 3_TCP_Socket_Exercise

# Folders's name
SRC_DIR = source
INC_DIR = include
OBJ_DIR = object
BIN_DIR = bin
OUT_DIR = output

# Get all folder in include folder
INC = $(wildcard $(INC_DIR)/*)
# Get all .c files in source folder
SRC = $(wildcard $(SRC_DIR)/*/*.c)
# Create list of object files from list of source files
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
# Location of executable file
EXE = $(BIN_DIR)/main
# List of folders holding object file
OBJ_FOLDER = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(wildcard $(SRC_DIR)/*))

# Compler
CC			=	gcc
# Remove command
RM			=	rm -r
# Flag for C compiler
CFLAGS		=	-Wall
# Flag to preprocessor
# "-I" + "[directory]"	=> Find header file in [directory]
#	Ex: -Iinclude		=> Find header file in folder name "include"
# CPPFLAGS	=	-Iinclude
CPPFLAGS	=	$(patsubst $(INC_DIR)/%, -I$(INC_DIR)/%, $(INC))
# Flag for linker
# "-L" + "directory"	=> Finc libraries in [directory]
#	Ex: -Llib			=> Find libraries in folder name "lib"
#LDFLAGS		:=	-Llib
#LDLIBS		:=	-lm

# Rule build code
all: $(EXE)
	@echo ""
	@echo "==================== Server Command ===================="
	@echo "./main server <port_number>"
	@echo "==================== Client Command ===================="
	@echo "./main client <server_ip_address> <server_port_number>"

# Rule create executable file and folder holding executable file
$(EXE): $(OBJ)
	@mkdir -p $(BIN_DIR)
	@gcc $^ -o $@

# Rule create object files and folder holding object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_FOLDER)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Clean object filess, executable files, and output files of program
clean:
	@$(RM) $(BIN_DIR)/* $(OBJ_DIR)/* $(OUT_DIR)/*

# Rule printing variable
print:
	@echo $(INC)
	@echo $(SRC)
	@echo $(OBJ)
	@echo $(CPPFLAGS)

.PHONY: run build clean print
