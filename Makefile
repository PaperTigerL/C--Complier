CC = g++
CFLAGS = -Wall -g
INCLUDES = -Iinclude
LIBS = -Llib -lLLVM
OBJS = src/lexer/Lexer.o src/parser/Parser.o src/ir/IRGenerator.o src/main.o
TARGET = build/C--Compiler

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

src/lexer/Lexer.o: src/lexer/Lexer.cpp include/Lexer.h include/Token.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

src/parser/Parser.o: src/parser/Parser.cpp include/Parser.h include/Ast.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

src/ir/IRGenerator.o: src/ir/IRGenerator.cpp include/IRGenerator.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

src/main.o: src/main.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)