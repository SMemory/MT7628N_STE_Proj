EXEC=ste_proj  
SRC=ste_proj.c

all: $(EXEC)
ste_proj: $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

