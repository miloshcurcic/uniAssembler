IDIR = ./include
OUTDIR = ./out
OBJDIR = $(OUTDIR)/obj
SRCDIR = ./src

CC = g++
CFLAGS = -Wall -I$(IDIR)

PROGRAM = assembler

SRC = $(SRCDIR)/parser.cpp $(SRCDIR)/scanner.cpp $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

$(OUTDIR)/$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -MMD -o $@ -c $< $(CFLAGS)

$(OBJDIR)/scanner.o: $(SRCDIR)/parser.cpp
$(OBJDIR)/parser.o: $(SRCDIR)/scanner.cpp

$(SRCDIR)/scanner.cpp: $(SRCDIR)/scanner.l
	flex -o $@ --header-file=$(IDIR)/scanner.h $<

$(SRCDIR)/parser.cpp: $(SRCDIR)/parser.y
	bison -o $@ --define=api.location.file='"../include/location.h"'  --defines=$(IDIR)/parser.h $<

-include $(OBJDIR)/*.d

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(OBJDIR)/*.d
	rm -f $(OUTDIR)/$(PROGRAM)
	rm -f $(OUTDIR)/*~
	rm -f $(SRCDIR)/scanner.cpp
	rm -f $(IDIR)/scanner.h
	rm -f $(SRCDIR)/parser.cpp
	rm -f $(IDIR)/parser.h
	rm -f $(IDIR)/location.h

.PHONY: clean
