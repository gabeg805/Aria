CC      = gcc
LIBS    = gtk+-3.0 cairo
CFLAGS  = -g -Wall -lX11 -O2
CFLAGS += `pkg-config $(LIBS) --cflags --libs`

PROGRAM = noti
NAMES   = $(PROGRAM) notilib
SOURCES = $(addprefix ./src/, $(addsuffix .c, $(NAMES)))
HEADERS = $(addprefix ./hdr/, $(addsuffix .h, $(NAMES)))
OBJECTS = $(addprefix ./obj/, $(addsuffix .o, $(NAMES)))

all: $(PROGRAM)

./obj/%.o: ./src/%.c 
	$(CC) $(CFLAGS) -o $@ -c $<

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(PROGRAM) 

.PHONY: clean
clean : 
	@rm -v -f $(OBJECTS)
	@rm -v -f $(PROGRAM)
