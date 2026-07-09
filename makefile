CC = gcc
WFLAGS = -Wall -Wextra
IFLAGS = -I include

.PHONY = run 

# Quick Run
run: bin/cfab 
	@echo ""
	@echo ""
	@./bin/cfab

# Executable Files
bin/cfab: obj/main.o 
	$(CC) obj/main.o -o bin/cfab

# Pattern Rules 
obj/%.o: src/%.c
	$(CC) -c $(WFLAGS) $(IFLAGS) $< -o $@