#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

// 定义Token的类型枚举
enum TokenType {
    KW_INT,         // int
    KW_VOID,        // void
    KW_RETURN,      // return
    KW_CONST,       // const
    KW_MAIN,        // main
    OP_PLUS,        // +
    OP_MINUS,       // -
    OP_MULT,        // *
    OP_DIV,         // /
    OP_MOD,         // %
    OP_ASSIGN,      // =
    OP_GT,          // >
    OP_LT,          // <
    OP_EQ,          // ==
    OP_LE,          // <=
    OP_GE,          // >=
    OP_NE,          // !=
    OP_AND,         // &&
    OP_OR,          // ||
    SE_LPAREN,      // (
    SE_RPAREN,      // )
    SE_LBRACE,      // {
    SE_RBRACE,      // }
    SE_SEMICOLON,   // ;
    SE_COMMA,       // ,
    IDN,            // Identifier
    INT,            // Integer
    EOF_TOKEN,      // End of file
    INVALID         // Invalid token (error handling)
};

// Token's struct body
struct Token {
    TokenType type;   // the type of token
    std::string lexeme; // the original text of token
    int line;         // the line token appear
    int column;       // the column of token appear
    int Val;          // Token defined value by pdf 

    // the construct function of Token
    Token(TokenType type, const std::string& lexeme, int line, int column)
        : type(type), lexeme(lexeme), line(line), column(column) {}


    // the help function of token to string 
    static std::string tokenTypeToString(TokenType type) {
        switch (type) {
        case KW_INT: return "KW_INT";
        case KW_VOID: return "KW_VOID";
        case KW_RETURN: return "KW_RETURN";
        case KW_CONST: return "KW_CONST";
        case KW_MAIN: return "KW_MAIN";
        case OP_PLUS: return "OP_PLUS";
        case OP_MINUS: return "OP_MINUS";
        case OP_MULT: return "OP_MULT";
        case OP_DIV: return "OP_DIV";
        case OP_MOD: return "OP_MOD";
        case OP_ASSIGN: return "OP_ASSIGN";
        case OP_GT: return "OP_GT";
        case OP_LT: return "OP_LT";
        case OP_EQ: return "OP_EQ";
        case OP_LE: return "OP_LE";
        case OP_GE: return "OP_GE";
        case OP_NE: return "OP_NE";
        case OP_AND: return "OP_AND";
        case OP_OR: return "OP_OR";
        case SE_LPAREN: return "SE_LPAREN";
        case SE_RPAREN: return "SE_RPAREN";
        case SE_LBRACE: return "SE_LBRACE";
        case SE_RBRACE: return "SE_RBRACE";
        case SE_SEMICOLON: return "SE_SEMICOLON";
        case SE_COMMA: return "SE_COMMA";
        case IDN: return "IDN";
        case INT: return "INT";
        case EOF_TOKEN: return "EOF_TOKEN";
        case INVALID: return "INVALID";
        default: return "UNKNOWN";
        }
    }

    // the way to get token string
    std::string toString() const {
        return "<" + tokenTypeToString(type) + ", "
            + "\"" + lexeme + "\", "
            + "line " + std::to_string(line) + ", "
            + "column " + std::to_string(column) + ">";
    }

    // operator<< override function to debug
    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        os << token.toString();
        return os;
    }
};

#endif // TOKEN_H