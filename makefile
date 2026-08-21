CC = gcc
WFLAGS = -Wall -Wextra
IFLAGS = -I include
DFLAGS = -g 
OFLAGS = -O0
.PHONY = run 

# Quick Run
run: bin/cfab 
	@echo ""
	@echo ""
	@./bin/cfab

# Executable Files
bin/cfab: bin obj/main.o obj/cfab_file.o obj/preprocessor.o obj/parser.o obj/resolver.o obj/rule_name_validator.o obj/cycle_detector.o
	$(CC) obj/main.o obj/cfab_file.o obj/preprocessor.o obj/parser.o obj/resolver.o obj/rule_name_validator.o obj/cycle_detector.o -o bin/cfab 

# Pattern Rules 
obj/%.o: src/%.c | obj 
	$(CC) -c $(WFLAGS) $(IFLAGS) $< -o $@

# Required Dirs
obj:
	mkdir -p obj 

bin:
	mkdir -p bin 
