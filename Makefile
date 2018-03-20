# ------------------------------------------------------------------------------
# Project
PROJECT = aria

# ------------------------------------------------------------------------------
# Directories
BUILDDIR = .
SRCDIR   = $(BUILDDIR)/src
OBJDIR   = $(BUILDDIR)/obj
INCDIR   = $(BUILDDIR)/include
DOCDIR   = $(BUILDDIR)/doc
ETCDIR   = $(BUILDDIR)/etc
SHAREDIR = $(HOME)/.local/share/$(PROJECT)

# ------------------------------------------------------------------------------
# Compiler settings
CC      = g++
CPPFLAGS  = -g -Wall -std=c++14 $(DEFINES)
LIBS    = -lX11 -I $(INCDIR) `pkg-config $(PKGS) --cflags --libs`
PKGS    = gtkmm-3.0
DEFINES = -DARIA_CONFIG_FILE="\"$(SHAREDIR)/$(PROJECT).conf\""

# ------------------------------------------------------------------------------
# Files
SRC    = $(wildcard $(SRCDIR)/*.cpp)
OBJ    = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DOC    = $(DOCDIR)/doxy.conf
MEMMAP = /tmp/$(PROJECT)map

# ------------------------------------------------------------------------------
# Default target
all: $(PROJECT)

# Compile targets
$(PROJECT): $(OBJ)
	$(CC) $(CPPFLAGS) $(LIBS) \
		-o $(PROJECT) $(OBJ)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CPPFLAGS) $(LIBS) \
		-o $@ \
		-c $<

# Phony targets
.PHONY: install clean doc test
install: all
	@mkdir -pv $(SHAREDIR)
	@cp -av $(ETCDIR)/$(PROJECT).conf $(SHAREDIR)/

clean : 
	@rm -v -f $(PROJECT)
	@rm -v -f $(OBJ)
	@rm -v -f $(OBJDIR)/*
	@rm -v -f $(MEMMAP)
	@rm -f -r $(DOCDIR)/html
	@rm -f -r $(DOCDIR)/latex

doc: $(DOC)
	@doxygen $(DOC)

# Fix opacity and title/body size
test: $(PROJECT)
	$(BUILDDIR)/$(PROJECT) --title "Title" --body "Body" \
		--width 200 --height 100 \
		--xpos  700 --ypos   400 \
		--time    5 \
		--opacity 0.5 \
		--margin 40 \
		--title-size 24 --body-size 16 \
		--background "#00a866" --foreground "#ffffff"
