# Define C compiler & flags
CC = gcc
CFLAGS = -Wall -g
SRC = main.c process.c memScheduler.c
OBJ = $(SRC:.c=.o)
EXE = allocate
all: $(EXE)
# The first executable
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $(LDLIBS)
# Clean up artifacts
clean:
	rm -f $(OBJ) $(EXE) $(EXE2)

