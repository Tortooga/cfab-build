CC = gcc
WFLAGS = -Wall -Wextra
IFLAGS = -I /include

.PHONY = run 

# Quick Run
run: bin/cfab 
	@echo ""
	@echo ""
	@./bin/cfab

# Executable Files
bin/cfab: obj/main.o 
	$(CC) obj/main.o -o bin/cfab

# Object Files
obj/main.o: src/main.c	

# Pattern Rules 
obj/%.o: src/%.c
	$(CC) -c $(WCFLAGS) $(IFLAGS) $< -o $@