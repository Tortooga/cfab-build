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
bin/cfab: obj/main.o obj/cfab_file.o obj/preprocessor.o obj/parser.o
	$(CC) obj/main.o obj/cfab_file.o obj/preprocessor.o obj/parser.o -o bin/cfab 

# Pattern Rules 
obj/%.o: src/%.c
	$(CC) -c $(WFLAGS) $(IFLAGS) $< -o $@