CC = gcc
CFLAGS = -g
LIBS = -lX11
MAIN = main
SRC = 
OBJ = $(MAIN:%=%.o) $(SRC:%=%.o)
DEPS = $(SRC:%=%.h)

$(MAIN): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(MAIN)
	rm -f $(OBJ)
