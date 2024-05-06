// lexer.h
// wrritered by lyr 2024/4/2
#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream> // 对于 std::istringstream
#include <string>  // 对于 std::string 和 std::getline
#include "Token.h"

// class Lexer for the token generated.
class Lexer {
public:
    std::istream& source;                                // source code stream 
    size_t pos;                                          // current postion to lex
    int line;                                            // current line 
    int column;                                          // current column 
    int lastLineLength;                                   // for the use of unchar,if get back, we need to know last number of colum is what 
    std::unordered_map<std::string, TokenType> keywords; // key word table 
    std::vector<Token> tokens;                           // the generated token 
    std::string sourceCode;                            // source code in string for debug 
    std::ifstream sourceFile;                         // 文件流成员变量

    char getNextChar();                               // get the char 
    void ungetChar(char ch);                          // return the char to stream
    bool isAlpha(char ch) const;                      // judge the char is in alphabet
    bool isDigit(char ch) const;                      // judge the char is number
    bool isWhitespace(char ch) const;                 // judge the symbol whether can be skip
    void skipWhitespace();                            // do the movement of skip 
    void skipComment();                               // skip the commnent                              //
    Token readToken();
    std::vector<Token> getTokens();
    void printTokens() const;
    void reportError(const std::string& error, int line, int column);
    std::istream& readFile(const std::string& filename);
    // Modify the constructor to accept an input stream and a source code string (for error reporting).
    explicit Lexer(std::istream& source, const std::string& sourceCode)
        : source(source), pos(0), line(1), column(0), sourceCode(sourceCode) {
        // Initialize the keyword table.
        keywords = {
            {"int", KW_INT},
            {"void", KW_VOID},
            {"return", KW_RETURN},
            {"const", KW_CONST},
            {"main", KW_MAIN}
        };
    }
    std::vector<Token>& Lexer::tokenize(); // The function that converts the source code into a sequence of tokens.
};
#endif // LEXER_H