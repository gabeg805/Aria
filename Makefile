# ------------------------------------------------------------------------------
# Project
PROJECT = aria

# ------------------------------------------------------------------------------
# Directories
BUILDDIR = .
SRCDIR   = $(BUILDDIR)/src
INCDIR   = $(BUILDDIR)/include
OBJDIR   = $(BUILDDIR)/obj
DOCDIR   = $(BUILDDIR)/doc
SHAREDIR = $(BUILDDIR)/share
LOCALSHAREDIR = $(HOME)/.local/share/$(PROJECT)

# ------------------------------------------------------------------------------
# Compiler settings
CC      = g++
CPPFLAGS  = -g -Wall -std=c++14 $(DEFINES)
LIBS    = -lX11 -lXrandr -I $(INCDIR) `pkg-config $(PKGS) --cflags --libs`
PKGS    = gtkmm-3.0
DEFINES = -DPROGRAM="\"$(PROJECT)\"" -DARIA_CONFIG_FILE="\"$(LOCALSHAREDIR)/$(PROJECT).conf\""

# ------------------------------------------------------------------------------
# Files
SRC    = $(wildcard $(SRCDIR)/*.cpp)
OBJ    = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DOC    = $(DOCDIR)/doxy.conf
MEMMAP = /tmp/$(PROJECT)map

# ------------------------------------------------------------------------------
# Default target
all: $(PROJECT) configure

# Compile targets
$(PROJECT): $(OBJ)
	$(CC) $(CPPFLAGS) $(LIBS) \
		-o $(PROJECT) $(OBJ)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(OBJDIR)
	$(CC) $(CPPFLAGS) $(LIBS) \
		-o $@ \
		-c $<

$(OBJDIR):
	@mkdir -pv $@

# Phony targets
clean: 
	@rm -v -f $(PROJECT)
	@rm -v -f $(MEMMAP)
	@rm -v -f -r $(OBJDIR)
	@rm -f -r $(DOCDIR)/*/

configure: $(SHAREDIR)/$(PROJECT).conf
	@mkdir -pv $(LOCALSHAREDIR)
	@cp -av $< $(LOCALSHAREDIR)/

doc: $(DOC)
	@doxygen $(DOC)

install:
	@sudo cp -av $(PROJECT) /usr/local/bin/

uninstall: clean
	@rm -v -f -r $(LOCALSHAREDIR)
	@sudo rm -v -f /usr/local/bin/$(PROJECT)

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

.PHONY: clean configure doc install uninstall test
