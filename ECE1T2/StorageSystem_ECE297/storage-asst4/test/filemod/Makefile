include ../Makefile.common

# The default target is to build the test.
build: main

# Build the test.
main: main.c -lcheck
	$(CC) $(CFLAGS) $^ -o $@

# Run the test
run: storage.h main
	./main

# Make storage.h available in the current directory.
storage.h:
	ln -s $(SRCDIR)/storage.h

# Clean up
clean:
	-rm -f main *.out ./storage.h

.PHONY: run
