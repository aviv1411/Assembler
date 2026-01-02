CC = gcc
CFLAGS = -ansi -pedantic -Wall -g
OBJ = main.o ast.o text_parser.o preprocessor.o first_pass.o second_pass.o output.o mem_alloc.o
EXEC = assembler
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)
main.o: source_files/main.c \
	source_files/../header_files/mem_alloc.h \
	source_files/../header_files/preprocessor.h \
	source_files/../header_files/first_pass.h \
	source_files/../header_files/second_pass.h \
	source_files/../header_files/translation_unit.h \
	source_files/../header_files/output.h
	$(CC) $(CFLAGS) -c source_files/main.c -o main.o


ast.o: source_files/ast.c \
	source_files/../header_files/ast.h \
	source_files/../header_files/text_parser.h 
	$(CC) $(CFLAGS) -c source_files/ast.c -o ast.o

text_parser.o: source_files/text_parser.c \
	source_files/../header_files/ast.h \
	source_files/../header_files/text_parser.h
	$(CC) $(CFLAGS) -c source_files/text_parser.c -o text_parser.o
	
preprocessor.o: source_files/preprocessor.c \
	source_files/../header_files/preprocessor.h \
	source_files/../header_files/mem_alloc.h
	$(CC) $(CFLAGS) -c source_files/preprocessor.c -o preprocessor.o

first_pass.o: source_files/first_pass.c \
	source_files/../header_files/first_pass.h \
	source_files/../header_files/ast.h \
	source_files/../header_files/translation_unit.h \
	source_files/../header_files/mem_alloc.h
	$(CC) $(CFLAGS) -c source_files/first_pass.c -o first_pass.o

second_pass.o: source_files/second_pass.c \
	source_files/../header_files/second_pass.h \
	source_files/../header_files/first_pass.h \
	source_files/../header_files/ast.h \
	source_files/../header_files/translation_unit.h \
	source_files/../header_files/mem_alloc.h
	$(CC) $(CFLAGS) -c source_files/second_pass.c -o second_pass.o

output.o: source_files/output.c \
	source_files/../header_files/translation_unit.h \
	source_files/../header_files/first_pass.h \
	source_files/../header_files/mem_alloc.h \
	source_files/../header_files/output.h
	$(CC) $(CFLAGS) -c source_files/output.c -o output.o

mem_alloc.o: source_files/mem_alloc.c \
	source_files/../header_files/mem_alloc.h \
	source_files/../header_files/translation_unit.h \
	source_files/../header_files/preprocessor.h
	$(CC) $(CFLAGS) -c source_files/mem_alloc.c -o mem_alloc.o

clean:
	rm -f *.o $(EXEC)
