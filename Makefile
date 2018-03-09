# ------------------------------------------------------------------------------
# Project
PROJECT = aria

# ------------------------------------------------------------------------------
# Compiler settings
CC      = g++
CFLAGS  = -g -Wall -std=c++11
LIBS    = -lX11 -I $(INCDIR) `pkg-config $(PKGS) --cflags --libs`
PKGS    = gtkmm-3.0

# ------------------------------------------------------------------------------
# Directories
BUILDDIR = .
SRCDIR   = $(BUILDDIR)/src
OBJDIR   = $(BUILDDIR)/obj
INCDIR   = $(BUILDDIR)/include
DOCDIR   = $(BUILDDIR)/doc

# ------------------------------------------------------------------------------
# Files
SRC    = $(wildcard $(SRCDIR)/*.cpp)
OBJ    = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DOC    = $(DOCDIR)/doxy.conf
MEMMAP = /tmp/ariamap

# ------------------------------------------------------------------------------
# Default target
all: $(PROJECT)

install: all

# Compile targets
$(PROJECT): $(OBJ)
	$(CC) $(CFLAGS) $(LIBS) \
		-o $(PROJECT) $(OBJ)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(LIBS) \
		-o $@ \
		-c $<

# Phony targets
.PHONY: clean doc test
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
