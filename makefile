IDIR = ./include
OUTDIR = ./out
BINDIR = ./bin
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

$(BINDIR)/$(PROGRAM): $(POBJ) $(OBJ) | $(BINDIR)
	$(CC) -o $@ $^

$(OUTOBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OUTOBJDIR)
	$(CC) -MMD -o $@ -c $< $(CFLAGS)

$(POUTOBJDIR)/%.o: $(POUTSRCDIR)/%.cpp | $(POUTOBJDIR)
	$(CC) -MMD -o $@ -c $< $(CFLAGS)

$(POUTOBJDIR)/scanner.o: $(POUTSRCDIR)/parser.cpp | $(POUTOBJDIR)
$(POUTOBJDIR)/parser.o: $(POUTSRCDIR)/scanner.cpp | $(POUTOBJDIR)

$(POUTSRCDIR)/scanner.cpp: $(SRCDIR)/scanner.l | $(POUTSRCDIR) $(POUTINCDIR)
	flex -o $@ --header-file=$(POUTINCDIR)/scanner.h $<

$(POUTSRCDIR)/parser.cpp: $(SRCDIR)/parser.y | $(POUTSRCDIR) $(POUTINCDIR)
	bison -o $@ --define=api.location.file='"../include/location.h"'  --defines=$(POUTINCDIR)/parser.h $<

$(IDIR):
	mkdir -p $@
$(OUTDIR):
	mkdir -p $@
$(BINDIR):
	mkdir -p $@
$(POUTDIR):
	mkdir -p $@
$(OUTOBJDIR):
	mkdir -p $@
$(POUTOBJDIR):
	mkdir -p $@
$(POUTSRCDIR):
	mkdir -p $@
$(POUTINCDIR):
	mkdir -p $@
$(SRCDIR):
	mkdir -p $@

-include $(OUTOBJDIR)/*.d $(POUTOBJDIR)/*.d

clean:
	rm -r $(OUTDIR)
	rm -r $(BINDIR)

.PHONY: clean
