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
bin/cfab: obj/main.o obj/cfab_file.o obj/cfab_file_processor.o
	$(CC) obj/main.o obj/cfab_file.o -o bin/cfab obj/cfab_file_processor.o

# Pattern Rules 
obj/%.o: src/%.c
	$(CC) -c $(WFLAGS) $(IFLAGS) $< -o $@