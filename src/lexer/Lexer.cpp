#include "Lexer.h"
char Lexer::getNextChar() {
    // ���ȼ���Ƿ�ﵽ�ļ�ĩβ
    if (source.peek() == EOF) {
        return EOF; // ����ǣ�����EOF
    }

    char ch;
    source.get(ch); // ���������ж�ȡ�ַ�

    // ���µ�ǰ�ַ���λ��
    ++pos;

    // �����ȡ���ַ��ǻ��з����������кŲ������к�
    if (ch == '\n') {
        ++line;
        lastLineLength = column;
        column = 0;
    }
    else {
        // ����ֻ�����к�
        lastLineLength = column;
        ++column;
    }

    return ch; // �������Ƕ������ַ�
}

void Lexer::ungetChar(char ch) {
    // ���ַ��Żص�������
    source.putback(ch);

    // ���µ�ǰλ��
    --pos;

    // ����ַ��ǻ��з��������кŲ��ָ�����һ�е��к�
    if (ch == '\n') {
        --line;
        if (lastLineLength > 0) {
            column = lastLineLength;
        }
        else {
            column = 0; // ���û�б�����һ�г��ȣ������к�
        }
    }
    else {
        // ����ֻ�����к�
        --column;
    }
}

bool Lexer::isWhitespace(char ch) const {
    // ���ڼ���������͵Ŀհ��ַ��������ո��Ʊ�������кͻس�
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

    // ��ʶ���������ؼ��ʣ�
    if (isalpha(ch) || ch == '_') { // Assuming isalpha checks for letters
        std::string lexeme(1, ch);
        while (isalnum((ch = getNextChar())) || ch == '_') { // Assuming isalnum checks for letters and digits
            lexeme += ch;
        }
        ungetChar(ch); // The last read character is not part of the identifier

        // Keyword or identifier check
        if (lexeme == "int") return Token(KW_INT, lexeme, tokenLine, tokenColumn);
        if (lexeme == "void") return Token(KW_VOID, lexeme, tokenLine, tokenColumn);
        if (lexeme == "return") return Token(KW_RETURN, lexeme, tokenLine, tokenColumn); // ��� return �ؼ�������
        if (lexeme == "main")return Token(KW_MAIN, lexeme, tokenLine, tokenColumn);
        if (lexeme == "const")return Token(KW_CONST, lexeme, tokenLine, tokenColumn);
        return Token(IDN, lexeme, tokenLine, tokenColumn);
    }

    // ����
    if (isdigit(ch)) {
        std::string lexeme(1, ch);
        while (isdigit((ch = getNextChar()))) {
            lexeme += ch;
        }
        ungetChar(ch); // The last read character is not part of the number
        return Token(INT, lexeme, tokenLine, tokenColumn);
    }

    // ������ͷָ���
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
        // ����� EOF_TOKEN ������
        if (token.type == EOF_TOKEN) {
            continue; // ������ǰѭ��������ӡEOF_TOKEN
        }

        // ����Token���������������ַ���������
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
            // ...�������������...
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
            // ...����������...
            // ...����Token����...
        default:
            // ����IDN��INT���͵�Token��������ʷ���Ԫ������Ϊtype_num
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

        // �����ʽ�����Token��Ϣ
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

    // ��ȡ��������
    for (int i = 0; i < errLine; ++i) {
        std::getline(sourceStream, line);
        if (i == errLine - 1) {
            // ��������еľ�������
            std::cerr << "  " << line << std::endl;

            // ָ�����λ�õ�ָʾ��
            std::cerr << "  " << std::string(errColumn - 1, ' ') << "^" << std::endl;
        }
    }
}

std::istream& Lexer::readFile(const std::string& filename) {
    sourceFile.open(filename);   // ���ļ�
    if (!sourceFile) {           // �����ʧ�ܣ��׳��쳣
        // ��������������׳�һ�� std::runtime_error �쳣
        throw std::runtime_error("Unable to open file: " + filename);
    }

    return sourceFile;           // �����ļ�������
}

std::vector<Token>& Lexer::tokenize() {
    tokens.clear(); // ����κ����е�tokens

    Token token = readToken();

    // ѭ��ֱ�� readToken ���� EOF_TOKEN
    while (token.type != EOF_TOKEN) {
        tokens.push_back(token);
        token = readToken(); // ��ȡ��һ�� token
    }

    // ����ļ������� token
    tokens.push_back(token);

    return tokens; // ���ﷵ���˳�Ա����tokens������
}
