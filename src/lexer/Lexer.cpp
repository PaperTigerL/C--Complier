#include "Lexer.h"
char Lexer::getNextChar() {
    // 首先检查是否达到文件末尾
    if (source.peek() == EOF) {
        return EOF; // 如果是，返回EOF
    }

    char ch;
    source.get(ch); // 从输入流中读取字符

    // 更新当前字符的位置
    ++pos;

    // 如果读取的字符是换行符，则增加行号并重置列号
    if (ch == '\n') {
        ++line;
        lastLineLength = column;
        column = 0;
    }
    else {
        // 否则只更新列号
        lastLineLength = column;
        ++column;
    }

    return ch; // 返回我们读到的字符
}

void Lexer::ungetChar(char ch) {
    // 将字符放回到输入流
    source.putback(ch);

    // 更新当前位置
    --pos;

    // 如果字符是换行符，减少行号并恢复到上一行的列号
    if (ch == '\n') {
        --line;
        if (lastLineLength > 0) {
            column = lastLineLength;
        }
        else {
            column = 0; // 如果没有保存上一行长度，重置列号
        }
    }
    else {
        // 否则只更新列号
        --column;
    }
}

bool Lexer::isWhitespace(char ch) const {
    // 现在检查所有类型的空白字符，包括空格、制表符、换行和回车
    return isspace(static_cast<unsigned char>(ch));
}

bool Lexer::isAlpha(char ch) const {
    return isalpha(ch);
}

bool Lexer::isDigit(char ch) const {
    return isdigit(ch);
}

void Lexer::skipWhitespace() {
    while (isWhitespace(source.peek())) {
        getNextChar(); // read and skip the WhiteSpace 
    }
}

void Lexer::skipComment() {
    if (source.peek() == '/') {
        getNextChar(); // read'/'
        if (source.peek() == '/') {
            // from the begin of the '/'
            while (getNextChar() != '\n' && source.peek() != EOF) {
                // read until the lind is ended
            }
            ++line; // due to the getchar will compelete the work so don't add the number
            column = 0; //restart the column number
        }
        else {
            // not commnent return '/'
            ungetChar('/');
        }
    }
}

Token Lexer::readToken() {
    skipComment();
    skipWhitespace();
    char ch = getNextChar();
    int tokenLine = line;    // Store the starting line
    int tokenColumn = column; // Store the starting column

    if (ch == EOF) {
        return Token(EOF_TOKEN, "<EOF>", tokenLine, tokenColumn+1);
    }

    // 标识符（包括关键词）
    if (isalpha(ch) || ch == '_') { // Assuming isalpha checks for letters
        std::string lexeme(1, ch);
        while (isalnum((ch = getNextChar())) || ch == '_') { // Assuming isalnum checks for letters and digits
            lexeme += ch;
        }
        ungetChar(ch); // The last read character is not part of the identifier

        // Keyword or identifier check
        if (lexeme == "int") return Token(KW_INT, lexeme, tokenLine, tokenColumn);
        if (lexeme == "void") return Token(KW_VOID, lexeme, tokenLine, tokenColumn);
        if (lexeme == "return") return Token(KW_RETURN, lexeme, tokenLine, tokenColumn); // 解决 return 关键字问题
        if (lexeme == "main")return Token(KW_MAIN, lexeme, tokenLine, tokenColumn);
        if (lexeme == "const")return Token(KW_CONST, lexeme, tokenLine, tokenColumn);
        return Token(IDN, lexeme, tokenLine, tokenColumn);
    }

    // 数字
    if (isdigit(ch)) {
        std::string lexeme(1, ch);
        while (isdigit((ch = getNextChar()))) {
            lexeme += ch;
        }
        ungetChar(ch); // The last read character is not part of the number
        return Token(INT, lexeme, tokenLine, tokenColumn);
    }

    // 运算符和分隔符
    switch (ch) {
    case '+': return Token(OP_PLUS, "+", tokenLine, tokenColumn);
    case '-': return Token(OP_MINUS, "-", tokenLine, tokenColumn);
    case '*': return Token(OP_MULT, "*", tokenLine, tokenColumn);
    case '/': return Token(OP_DIV, "/", tokenLine, tokenColumn);
    case '%':return Token(OP_MOD, "%", tokenLine, tokenColumn);
    case '=': {
        char next = getNextChar();
        if (next == '=') {
            return Token(OP_EQ, "==", tokenLine, tokenColumn);
        }
        ungetChar(next);
        return Token(OP_ASSIGN, "=", tokenLine, tokenColumn);
    }
    case'<': {
        char next = getNextChar();
        if (next == '=') {
            return Token(OP_LE, "<=", tokenLine, tokenColumn);
        }
        ungetChar(next);
        return Token(OP_LT, "<", tokenLine, tokenColumn);
    }
    case'>': {
        char next = getNextChar();
        if (next == '=') {
            return Token(OP_GE, ">=", tokenLine, tokenColumn);
        }
        ungetChar(next);
        return Token(OP_GT, ">", tokenLine, tokenColumn);
    }
    case '!': {
        char next = getNextChar();
        if (next == '=') {
            return Token(OP_NE, "!=", tokenLine, tokenColumn);
        }
        ungetChar(next);
        return Token(INVALID, std::string(1, ch), tokenLine, tokenColumn);
    }
    case '&':{
        char next = getNextChar();
        if (next == '&') {
            return Token(OP_AND, "&&", tokenLine, tokenColumn);
        }
        ungetChar(next);
        return Token(INVALID, std::string(1, ch), tokenLine, tokenColumn);
    }
    case '|': {
        char next = getNextChar();
        if (next == '|') {
            return Token(OP_OR, "||", tokenLine, tokenColumn);
        }
        ungetChar(next);
        return Token(INVALID, std::string(1, ch), tokenLine, tokenColumn);
    }
            // ... add other multi-character operators
    case '(': return Token(SE_LPAREN, "(", tokenLine, tokenColumn);
    case ')': return Token(SE_RPAREN, ")", tokenLine, tokenColumn);
    case '{': return Token(SE_LBRACE, "{", tokenLine, tokenColumn);
    case '}': return Token(SE_RBRACE, "}", tokenLine, tokenColumn);
    case ';': return Token(SE_SEMICOLON, ";", tokenLine, tokenColumn); 
    case ',': return Token(SE_COMMA, ",", tokenLine, tokenColumn);
        // ... add other single-character separators
    default: return Token(INVALID, std::string(1, ch), tokenLine, tokenColumn);
    }
}

std::vector<Token> Lexer::getTokens() {
    std::vector<Token> tokens;
    Token token = readToken(); // Read the first token

    while (token.type != EOF_TOKEN) { // Continue until EOF token
        tokens.push_back(token); // Add the token to the vector
        token = readToken(); // Read the next token
    }
    // You may want to also add the EOF token to the result vector,
    // depending on how you want to use it later.
    tokens.push_back(token); // Add the EOF token to signal the end of the stream

    return tokens;
}

void Lexer::printTokens() const {
    //std::cout << "printTokens: " << std::endl;
    for (const Token& token : tokens) {
        std::string type_str;
        std::string type_num;
        // 如果是 EOF_TOKEN 则跳过
        if (token.type == EOF_TOKEN) {
            continue; // 跳过当前循环，不打印EOF_TOKEN
        }

        // 根据Token类型来设置类型字符串和数字
        switch (token.type) {
        case KW_INT:
            type_str = "KW";
            type_num = "1";
            break;
        case KW_VOID:
            type_str = "KW";
            type_num = "2";
            break;
        case KW_RETURN:
            type_str = "KW";
            type_num = "3";
            break;
        case KW_CONST:
            type_str = "KW";
            type_num = "4";
            break;
        case KW_MAIN:
            type_str = "KW";
            type_num = "5";
            break;
        case OP_PLUS:
            type_str = "OP";
            type_num = "6";
            break;
        case OP_MINUS:
            type_str = "OP";
            type_num = "7";
            break;
        case OP_MULT:
            type_str = "OP";
            type_num = "8";
            break;
        case OP_DIV:
            type_str = "OP";
            type_num = "9";
            break;
        case OP_MOD:
            type_str = "OP";
            type_num = "10";
            break;
        case OP_ASSIGN:
            type_str = "OP";
            type_num = "11";
            break;
        case OP_GT:
            type_str = "OP";
            type_num = "12";
            break;
        case OP_LT:
            type_str = "OP";
            type_num = "13";
            break;
        case OP_EQ:
            type_str = "OP";
            type_num = "14";
            break;
        case OP_LE:
            type_str = "OP";
            type_num = "15";
            break;
        case OP_GE:
            type_str = "OP";
            type_num = "16";
            break;
        case OP_NE:
            type_str = "OP";
            type_num = "17";
            break;
        case OP_AND:
            type_str = "OP";
            type_num = "18";
            break;
        case OP_OR:
            type_str = "OP";
            type_num = "19";
            break;
            // ...其余运算符类型...
        case SE_LPAREN:
            type_str = "SE";
            type_num = "20";
            break;
        case SE_RPAREN:
            type_str = "SE";
            type_num = "21";
            break;
        case SE_LBRACE:
            type_str = "SE";
            type_num = "22";
            break;
        case SE_RBRACE:
            type_str = "SE";
            type_num = "23";
            break;
        case SE_SEMICOLON:
            type_str = "SE";
            type_num = "24";
            break;
        case SE_COMMA:
            type_str = "SE";
            type_num = "25";
            break;
            // ...其余界符类型...
            // ...其他Token类型...
        default:
            // 对于IDN和INT类型的Token，保持其词法单元内容作为type_num
            if (token.type == IDN) {
                type_str = "IDN";
                type_num = token.lexeme;
            }
            else if (token.type == INT) {
                type_str = "INT";
                type_num = token.lexeme;
            }
            else {
                type_str = "UNKNOWN";
                type_num = "";
            }
            break;
        }

        // 输出格式化后的Token信息
        std::cout << token.lexeme << "\t<" << type_str << ",";
        if (!type_num.empty()) {
            std::cout << type_num;
        }
        std::cout << ">" << std::endl;
    }
}

void Lexer::reportError(const std::string& error, int errLine, int errColumn) {
    std::cerr << "Error on line " << errLine << ", column " << errColumn << ": " << error << std::endl;
    std::istringstream sourceStream(sourceCode);
    std::string line;

    // 读取到错误行
    for (int i = 0; i < errLine; ++i) {
        std::getline(sourceStream, line);
        if (i == errLine - 1) {
            // 输出错误行的具体内容
            std::cerr << "  " << line << std::endl;

            // 指向错误位置的指示器
            std::cerr << "  " << std::string(errColumn - 1, ' ') << "^" << std::endl;
        }
    }
}

std::istream& Lexer::readFile(const std::string& filename) {
    sourceFile.open(filename);   // 打开文件
    if (!sourceFile) {           // 如果打开失败，抛出异常
        // 错误处理，比如可以抛出一个 std::runtime_error 异常
        throw std::runtime_error("Unable to open file: " + filename);
    }

    return sourceFile;           // 返回文件流引用
}

std::vector<Token>& Lexer::tokenize() {
    tokens.clear(); // 清空任何现有的tokens

    Token token = readToken();

    // 循环直到 readToken 返回 EOF_TOKEN
    while (token.type != EOF_TOKEN) {
        tokens.push_back(token);
        token = readToken(); // 读取下一个 token
    }

    // 添加文件结束的 token
    tokens.push_back(token);

    return tokens; // 这里返回了成员变量tokens的引用
}
