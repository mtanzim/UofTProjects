include ../Makefile.common

# The default target is to build the test.
build: main

# Build the test.
main: main.c $(SRCDIR)/$(CLIENTLIB) -lcheck
	$(CC) $(CFLAGS) -I $(SRCDIR) $^ -o $@

# Run the test.
run: init main
	-rm -rf ./data
	./main

# Clean up
clean:
	-rm -rf main *.out ./$(SERVEREXEC) ./data

.PHONY: run
