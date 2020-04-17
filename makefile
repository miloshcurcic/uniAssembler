IDIR = ./include
OUTDIR = ./out
OBJDIR = $(OUTDIR)/obj
SRCDIR = ./src

CC = g++
CFLAGS = -Wall -I$(IDIR)

PROGRAM = assembler

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

$(OUTDIR)/$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -MMD -o $@ -c $< $(CFLAGS)

-include $(OBJDIR)/*.d

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(OBJDIR)/*.d
	rm -f $(OUTDIR)/$(PROGRAM)
	rm -f $(OUTDIR)/*~

.PHONY: clean
