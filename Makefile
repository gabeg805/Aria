## Compiler
CC      = g++
LIBS    = gtkmm-3.0
CFLAGS  = -g -Wall -lX11 -O0
CFLAGS += `pkg-config $(LIBS) --cflags --libs`
CINC    =

## Program source files
PROGRAM = aria
COMMON  = AriaAttribute AriaUtility
CORE    = $(PROGRAM) AriaNotify AriaMap
NAMES   = $(COMMON) $(CORE)  
MEMMAP  = /tmp/ariamap

## Directories used
COMMON_DIR = ./common
CORE_DIR   = ./core
SRC_DIR    = ./src
INC_DIR    = ./include
OBJ_DIR    = ./obj

COMMON_SRC_DIR = $(COMMON_DIR)/$(SRC_DIR)
COMMON_INC_DIR = $(COMMON_DIR)/$(INC_DIR)
CORE_SRC_DIR   = $(CORE_DIR)/$(SRC_DIR)
CORE_INC_DIR   = $(CORE_DIR)/$(INC_DIR)

CINC += -I $(COMMON_INC_DIR)
CINC += -I $(CORE_INC_DIR)

## Add extension to file names
SRC  = $(addprefix $(COMMON_DIR)/, $(addsuffix .cc, $(COMMON)))
SRC += $(addprefix $(CORE_DIR)/,   $(addsuffix .cc, $(CORE)))
OBJ  = $(addprefix $(OBJ_DIR)/,    $(addsuffix .o,  $(NAMES)))

## Makefile actions
all: $(PROGRAM)

$(PROGRAM): $(OBJ)
	$(CC) $(CFLAGS) \
		-o $(PROGRAM) $(OBJ) \
		$(CINC)

$(OBJ_DIR)/%.o: $(COMMON_SRC_DIR)/%.cc
	$(CC) $(CFLAGS) \
		-c $< \
		-o $@ \
		$(CINC)

$(OBJ_DIR)/%.o: $(CORE_SRC_DIR)/%.cc
	$(CC) $(CFLAGS) \
		-c $< \
		-o $@ \
		$(CINC)


.PHONY: clean
clean : 
	@rm -v -f $(OBJ)
	@rm -v -f $(OBJ_DIR)/*
	@rm -v -f $(PROGRAM)
	@rm -v -f $(MEMMAP)
