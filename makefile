IDIR = ./include
OUTDIR = ./out
POUTDIR = $(OUTDIR)/parsing
OUTOBJDIR = $(OUTDIR)/obj
POUTOBJDIR = $(POUTDIR)/obj
POUTSRCDIR = $(POUTDIR)/src
POUTINCDIR = $(POUTDIR)/include
SRCDIR = ./src

CC = g++
CFLAGS = -Wall -I$(IDIR) -I$(POUTINCDIR)

PROGRAM = assembler

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OUTOBJDIR)/%.o,$(SRC))
POBJ = $(POUTOBJDIR)/scanner.o $(POUTOBJDIR)/parser.o

$(OUTDIR)/$(PROGRAM): $(POBJ) $(OBJ)
	$(CC) -o $@ $^

$(OUTOBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -MMD -o $@ -c $< $(CFLAGS)

$(POUTOBJDIR)/%.o: $(POUTSRCDIR)/%.cpp
	$(CC) -MMD -o $@ -c $< $(CFLAGS)

$(POUTOBJDIR)/scanner.o: $(POUTSRCDIR)/parser.cpp
$(POUTOBJDIR)/parser.o: $(POUTSRCDIR)/scanner.cpp

$(POUTSRCDIR)/scanner.cpp: $(SRCDIR)/scanner.l
	flex -o $@ --header-file=$(POUTINCDIR)/scanner.h $<

$(POUTSRCDIR)/parser.cpp: $(SRCDIR)/parser.y
	bison -o $@ --define=api.location.file='"../include/location.h"'  --defines=$(POUTINCDIR)/parser.h $<

-include $(OUTOBJDIR)/*.d $(POUTOBJDIR)/*.d

clean:
	rm -f $(OUTOBJDIR)/*.o
	rm -f $(OUTOBJDIR)/*.d
	rm -f $(POUTOBJDIR)/*.o
	rm -f $(POUTOBJDIR)/*.d
	rm -f $(POUTSRCDIR)/*.cpp
	rm -f $(POUTINCDIR)/*.h
	rm -f $(OUTDIR)/$(PROGRAM)
	rm -f $(OUTDIR)/*~

.PHONY: clean
