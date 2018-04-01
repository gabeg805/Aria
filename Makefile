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
LIBS    = -lX11 -lXrandr -I $(INCDIR) `pkg-config $(PKGS) --cflags --libs`
PKGS    = gtkmm-3.0
DEFINES = -DPROGRAM="\"$(PROJECT)\"" -DARIA_CONFIG_FILE="\"$(SHAREDIR)/$(PROJECT).conf\""

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
	@rm -v -f -r $(DOCDIR)/*/

doc: $(DOC)
	@doxygen $(DOC)

test: $(PROJECT)
	$(BUILDDIR)/$(PROJECT) \
		--title="<b>Import Message</b>" \
		--body="Here is the body of the message." \
		--icon="/usr/share/icons/hicolor/32x32/apps/firefox.png" \
		--xpos=700 --ypos=400 \
		--time=3 \
		--opacity=0.6 \
		--background="#ffa500" --foreground="#ffffff" \
		--margin=10 \
		--title-size=18 --body-size=14
