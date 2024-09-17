OUTPUT_DIR ?= .
FILE ?= main.c
OUTPUT_FILE = $(OUTPUT_DIR)/$(basename $(notdir $(FILE)))

# Default target
all: $(OUTPUT_FILE)

# Rule to compile the input file
$(OUTPUT_FILE): $(FILE)
	echo Compiling $(FILE) to $(OUTPUT_FILE)
	$(CC) $(FILE) -o $(OUTPUT_FILE)