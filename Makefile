## Compiler
CC      = g++
LIBS    = gtkmm-3.0
CFLAGS  = -g -Wall -lX11 -O0 -std=c++11
CFLAGS += `pkg-config $(LIBS) --cflags --libs`
CINC    =

## Program source files
PROGRAM = aria
COMMON  = AriaAttribute AriaSharedMem AriaUtility
CORE    = AriaNotify $(PROGRAM)
NAMES   = $(COMMON) $(CORE)  
DOXY    = doxy.conf
MEMMAP  = /tmp/ariamap

## Directories used
DOC_DIR    = ./doc
COMMON_DIR = ./src/common
CORE_DIR   = ./src/core
SRC_DIR    = src
INC_DIR    = include
OBJ_DIR    = ./obj

COMMON_SRC_DIR = $(COMMON_DIR)/$(SRC_DIR)
COMMON_INC_DIR = $(COMMON_DIR)/$(INC_DIR)
CORE_SRC_DIR   = $(CORE_DIR)/$(SRC_DIR)
CORE_INC_DIR   = $(CORE_DIR)/$(INC_DIR)

CINC += -I $(COMMON_INC_DIR)
CINC += -I $(CORE_INC_DIR)

## Add extension to file names
SRC  = $(addprefix $(COMMON_SRC_DIR)/, $(addsuffix .cc, $(COMMON)))
SRC += $(addprefix $(CORE_SRC_DIR)/,   $(addsuffix .cc, $(CORE)))
OBJ  = $(addprefix $(OBJ_DIR)/,    $(addsuffix .o,  $(NAMES)))
DOC  = $(DOC_DIR)/$(DOXY)

## Makefile actions
all: $(PROGRAM) doc

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

.PHONY: clean doc test
clean: 
	@rm -v -f $(OBJ)
	@rm -v -f $(OBJ_DIR)/*
	@rm -v -f $(MEMMAP)
	@rm -v -f $(PROGRAM)
	@rm -v -f -r $(DOC_DIR)/html
	@rm -v -f -r $(DOC_DIR)/latex

doc: $(DOC)
	@doxygen $(DOC)

test: $(PROGRAM)
	$(PROGRAM) --title "Title" --body "Body" \
		--width 200 --height 100 \
		--xpos  700 --ypos   400 \
		--time    5 \
		--opacity 0.5 \ #fix
		--margin 40 \
		--title-size 24 --body-size 16 \ #fix
		--background "#00a866" --foreground "#ffffff"
