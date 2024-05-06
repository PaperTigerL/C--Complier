#include "Lexer.h"
#include "Parser.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>

// ���� Parser::advance ����
void testAdvance(Parser& parser) {
    std::cout << "Testing Parser::advance" << std::endl;
    while (parser.advance()) {
        std::cout << "Advanced to token: " << parser.currentToken.lexeme << std::endl;
    }
    std::cout << "No more tokens to advance to. Reached end of token stream." << std::endl;
}

// ���� Parser::match ����
void testMatch(Parser& parser, TokenType expected) {
    std::cout << "Testing Parser::match" << std::endl;
    try {
        parser.match(expected);
        std::cout << "Matched token: " << parser.currentToken.lexeme << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Match test caught an exception: " << e.what() << std::endl;
    }
}

// ���� Parser::syntaxError ����
void testSyntaxError(Parser& parser) {
    std::cout << "Testing Parser::syntaxError" << std::endl;
    try {
        // ǿ�Ƶ��� syntaxError ������һ������
        parser.syntaxError("Forced syntax error for testing.");
    }
    catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "Forced syntax error for testing."
            && "Expected error message not received.");
        std::cerr << "Caught the expected exception: " << e.what() << std::endl;
    }
}

void testParseProgram(Parser& parser) {
    try {
        // ���� parseProgram
        auto program = parser.parseProgram();

        // ��� program �Ƿ�Ϊ nullptr
        if (program == nullptr) {
            std::cerr << "parseProgram���ص�programΪnullptr������ʧ�ܡ�" << std::endl;
        }
        else {
            std::cout << "parseProgram�ɹ���AST������ɡ�" << std::endl;

            // ����ʵ��AST������ͼ�� program �Ľṹ
            if (!program->compUnits.empty()) {
                std::cout << "��⵽��ȷ������compUnits��" << program->compUnits.size() << std::endl;
            }
            else {
                std::cout << "���棺δ��⵽�κ�compUnits��" << std::endl;
            }

           

            std::cout << "parseProgram����ͨ���������������Ԥ�ڡ�" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "parseProgram����ʧ��: " << e.what() << std::endl;
    }
}


// ���Ժ��������parseCompUnit()�Ƿ���������
void testParseCompUnit(Parser& parser) {
    auto compUnit = parser.parseCompUnit();

    if (!compUnit) {
        std::cerr << "parseCompUnitʧ�ܣ�������nullptr��" << std::endl;
    }
    else {
        std::cout << "parseCompUnit�ɹ����أ����ڼ������..." << std::endl;
        // ���compUnit�Ƿ�ɹ��ذ����˽ڵ�
        if (!compUnit->nodes.empty()) {
            std::cout << "parseCompUnit�ɹ��������� " << compUnit->nodes.size() << " ���ڵ㡣" << std::endl;
        }
        else {
            std::cout << "parseCompUnitδ�������κνڵ㡣" << std::endl;
        }
    }
}

void testParseDeclaration(Parser& parser) {
    auto declNode = parser.parseDeclaration();

    if (declNode) {
        std::cout << "parseDeclaration�ɹ��������˷ǿսڵ㡣" << std::endl;

        // ��һ�����Լ��ڵ�������Ƿ�ΪVarDecl��ConstantDeclaration
        auto varDecl = std::dynamic_pointer_cast<VarDecl>(declNode);
        auto constDecl = std::dynamic_pointer_cast<ConstantDeclaration>(declNode);
        if (varDecl) {
            std::cout << "�ڵ���VarDecl���͡�" << std::endl;
        }
        else if (constDecl) {
            std::cout << "�ڵ���ConstantDeclaration���͡�" << std::endl;
        }
        else {
            std::cerr << "���󣺷��ؽڵ�Ȳ���VarDeclҲ����ConstantDeclaration���͡�" << std::endl;
        }
    }
    else {
        std::cerr << "parseDeclarationʧ�ܣ������˿սڵ㡣" << std::endl;
    }
}

// ���� parseConstDecl ����
void testParseConstDecl() {
    std::cout << "Starting parseConstDecl() test..." << std::endl;

    // ����һ��ģ��� token �б���ʾ 'const int a = 5;'
    std::vector<Token> tokens = {
        Token(TokenType::KW_CONST, "const", 1, 1),
        Token(TokenType::KW_INT, "int", 1, 7),
        Token(TokenType::IDN, "a", 1, 11),
        Token(TokenType::OP_ASSIGN, "=", 1, 13),
        Token(TokenType::INT, "5", 1, 15),
        Token(TokenType::SE_SEMICOLON, ";", 1, 16),
        Token(TokenType::EOF_TOKEN, "", 1, 17)
    };

    // ���� Parser ���󲢸��� tokens
    Parser parser(tokens);

    // ��� parseConstDecl �ķ���ֵ
    try {
        auto constDecl = parser.parseConstDecl();

        // ��� parseConstDecl �ķ���ֵ�Ƿ�ǿ�
        if (constDecl) {
            std::cout << "parseConstDecl() test passed: Got a non-null ConstantDeclaration." << std::endl;
        }
        else {
            std::cerr << "parseConstDecl() test failed: Got a null ConstantDeclaration." << std::endl;
        }
    }
    catch (const std::exception& e) {
        // ��������쳣����������Ϣ�Խ��е���
        std::cerr << "Exception occurred: " << e.what() << std::endl;
    }
}

// ���� parseBasicType ����
void testParseBasicType() {
    std::cout << "Starting parseBasicType() test..." << std::endl;

    // ����ģ�����ȷ�ʹ��� token
    std::vector<Token> correctTokens = { Token(TokenType::KW_INT, "int", 1, 1) };
    std::vector<Token> wrongTokens = { Token(TokenType::IDN, "variable", 1, 1) };

    // ��ȷ�������
    Parser parserCorrect(correctTokens);
    auto basicTypeResult = parserCorrect.parseBasicType();
    if (basicTypeResult && basicTypeResult->type == BasicType::Type::INT) {
        std::cout << "parseBasicType() test passed for 'int'." << std::endl;
    }
    else {
        std::cerr << "parseBasicType() test failed: did not recognize 'int' correctly." << std::endl;
    }

    // �����������
    try {
        Parser parserWrong(wrongTokens);
        basicTypeResult = parserWrong.parseBasicType();
        // ���û���׳��쳣�������ʧ��
        std::cerr << "parseBasicType() test failed: did not throw an error for an unexpected token." << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "parseBasicType() correctly reported an error: " << e.what() << std::endl;
    }
}
// ���� parseConstDef ����
void testParseConstDef() {
    std::cout << "Starting parseConstDef() test..." << std::endl;

    // ����ģ��� token ����
    std::vector<Token> tokens = {
        Token(TokenType::IDN, "myConst", 1, 1),    // ��ʶ��
        Token(TokenType::OP_ASSIGN, "=", 1, 9),    // �Ⱥ�
        Token(TokenType::INT, "42", 1, 11),        // ��������ֵ
        Token(TokenType::SE_SEMICOLON, ";", 1, 13) // �������ֺţ���ѡ������ʵ���﷨������
    };

    // ���� Parser ���󲢸��� tokens
    Parser parser(tokens);

    // ���� parseConstDef �����ս��
    try {
        auto constDefResult = parser.parseConstDef();

        // ��֤���ص� ConstDef ����Ƿ���ȷ
        if (constDefResult && constDefResult->identifier == "myConst" && constDefResult->constInitVal) {
            std::cout << "parseConstDef() test passed." << std::endl;
        }
        else {
            std::cerr << "parseConstDef() test failed: incorrect ConstDef result." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
    }
}


void testParseIntConst() {
    std::cout << "Starting parseIntConst() test..." << std::endl;

    // ��ȷ������ token
    Token intToken(TokenType::INT, "123", 1, 1);

    // ����ķ����� token
    Token nonIntToken(TokenType::IDN, "abc", 1, 1);

    // ���� Parser ���󲢸�����ȷ�� token
    std::vector<Token> tokensCorrect{ intToken };
    Parser parserCorrect(tokensCorrect);
    auto intConst = parserCorrect.parseIntConst();
    if (intConst && intConst->value == 123) {
        std::cout << "parseIntConst() test passed with correct integer token." << std::endl;
    }
    else {
        std::cerr << "parseIntConst() test failed with correct integer token." << std::endl;
    }

    // ���� Parser ���󲢸������� token
    std::vector<Token> tokensIncorrect{ nonIntToken };
    Parser parserIncorrect(tokensIncorrect);
    try {
        intConst = parserIncorrect.parseIntConst();
        if (!intConst) {
            std::cout << "parseIntConst() correctly returned nullptr for non-integer token." << std::endl;
        }
        else {
            std::cerr << "parseIntConst() test failed to return nullptr for non-integer token." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "parseIntConst() incorrectly threw an exception for non-integer token: " << e.what() << std::endl;
    }
}


void testParseIdent() {
    std::cout << "Starting parseIdent() test..." << std::endl;

    // ��ȷ�ı�ʶ�� token
    Token identToken(TokenType::IDN, "myVar", 1, 1);

    // ����ķǱ�ʶ�� token
    Token nonIdentToken(TokenType::INT, "123", 1, 1);

    // ���� Parser ���󲢸�����ȷ�� token
    std::vector<Token> tokensCorrect{ identToken };
    Parser parserCorrect(tokensCorrect);
    auto ident = parserCorrect.parseIdent();
    if (ident && ident->name == "myVar") {
        std::cout << "parseIdent() test passed with correct identifier token." << std::endl;
    }
    else {
        std::cerr << "parseIdent() test failed with correct identifier token." << std::endl;
    }

    // ���� Parser ���󲢸������� token
    std::vector<Token> tokensIncorrect{ nonIdentToken };
    Parser parserIncorrect(tokensIncorrect);
    try {
        ident = parserIncorrect.parseIdent();
        if (!ident) {
            std::cout << "parseIdent() correctly returned nullptr for non-identifier token." << std::endl;
        }
        else {
            std::cerr << "parseIdent() test failed to return nullptr for non-identifier token." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "parseIdent() incorrectly threw an exception for non-identifier token: " << e.what() << std::endl;
    }
}

void testParseFuncType() {
    std::cout << "Starting parseFuncType() test..." << std::endl;

    // ��ȷ�� 'int' ���͵ĺ��� token
    Token voidToken(TokenType::KW_VOID, "void", 1, 1);
    Token intToken(TokenType::KW_INT, "int", 1, 1);

    // ����ķǺ������� token
    Token nonFuncTypeToken(TokenType::IDN, "nonFuncType", 1, 1);

    // ���� Parser ���󲢸��� 'void' token
    std::vector<Token> tokensWithVoid{ voidToken };
    Parser parserWithVoid(tokensWithVoid);
    auto funcTypeVoid = parserWithVoid.parseFuncType();
    if (funcTypeVoid && funcTypeVoid->type == FuncTypeEnum::Void) {
        std::cout << "parseFuncType() test passed with 'void' token." << std::endl;
    }
    else {
        std::cerr << "parseFuncType() test failed with 'void' token." << std::endl;
    }

    // ���� Parser ���󲢸��� 'int' token
    std::vector<Token> tokensWithInt{ intToken };
    Parser parserWithInt(tokensWithInt);
    auto funcTypeInt = parserWithInt.parseFuncType();
    if (funcTypeInt && funcTypeInt->type == FuncTypeEnum::Integer) {
        std::cout << "parseFuncType() test passed with 'int' token." << std::endl;
    }
    else {
        std::cerr << "parseFuncType() test failed with 'int' token." << std::endl;
    }

    // ���� Parser ���󲢸������� token
    std::vector<Token> tokensIncorrect{ nonFuncTypeToken };
    Parser parserIncorrect(tokensIncorrect);
    auto funcTypeIncorrect = parserIncorrect.parseFuncType();
    if (!funcTypeIncorrect) {
        std::cout << "parseFuncType() correctly returned nullptr for non-function-type token." << std::endl;
    }
    else {
        std::cerr << "parseFuncType() test failed: it should return nullptr for non-function-type token." << std::endl;
    }
}
void testParseUnaryOp() {
    std::cout << "Starting parseUnaryOp() test..." << std::endl;

    // �����õ�һԪ������ tokens
    Token plusToken(TokenType::OP_PLUS, "+", 1, 1);
    Token minusToken(TokenType::OP_MINUS, "-", 1, 2);
    // Token notToken(TokenType::YOUR_DEFINED_NOT_TOKEN, "!", 1, 3); // ��������TokenType�����滻YOUR_DEFINED_NOT_TOKEN

    // ��һԪ������ token
    Token nonUnaryToken(TokenType::INT, "123", 1, 4);

    // ���� '+' ������
    std::vector<Token> tokensPlus{ plusToken };
    Parser parserPlus(tokensPlus);
    auto resultPlus = parserPlus.parseUnaryOp();

    if (resultPlus && resultPlus->op == '+') {
        std::cout << "parseUnaryOp() test passed for '+' operator." << std::endl;
    }
    else {
        std::cerr << "parseUnaryOp() test failed for '+' operator." << std::endl;
    }

    // ���� '-' ������
    std::vector<Token> tokensMinus{ minusToken };
    Parser parserMinus(tokensMinus);
    auto resultMinus = parserMinus.parseUnaryOp();

    if (resultMinus && resultMinus->op == '-') {
        std::cout << "parseUnaryOp() test passed for '-' operator." << std::endl;
    }
    else {
        std::cerr << "parseUnaryOp() test failed for '-' operator." << std::endl;
    }

    // ���� '!' ������
    // std::vector<Token> tokensNot { notToken };
    // Parser parserNot(tokensNot);
    // auto resultNot = parserNot.parseUnaryOp();

    // if (resultNot && resultNot->op == '!') {
    //     std::cout << "parseUnaryOp() test passed for '!' operator." << std::endl;
    // } else {
    //     std::cerr << "parseUnaryOp() test failed for '!' operator." << std::endl;
    // }

    // ���Է�һԪ�����������
    std::vector<Token> tokensNonUnary{ nonUnaryToken };
    Parser parserNonUnary(tokensNonUnary);
    auto resultNonUnary = parserNonUnary.parseUnaryOp();

    if (!resultNonUnary) {
        std::cout << "parseUnaryOp() test passed for non-unary operator (expected nullptr)." << std::endl;
    }
    else {
        std::cerr << "parseUnaryOp() test failed for non-unary operator (expected nullptr)." << std::endl;
    }
}
void testParseNumber() {
    std::cout << "Starting parseNumber() test..." << std::endl;

    // ��ȷ���������� token
    Token intToken(TokenType::INT, "42", 1, 1);

    // ����ķ��������� token������һ����ʶ����
    Token nonIntToken(TokenType::IDN, "myVar", 1, 1);

    // ���� Parser ���󲢸�����ȷ�� token
    std::vector<Token> tokensCorrect{ intToken };
    Parser parserCorrect(tokensCorrect);
    auto number = parserCorrect.parseNumber();
    if (number && number->value == 42) {
        std::cout << "parseNumber() test passed with correct integer token." << std::endl;
    }
    else {
        std::cerr << "parseNumber() test failed with correct integer token." << std::endl;
    }

    // ���� Parser ���󲢸������� token
    std::vector<Token> tokensIncorrect{ nonIntToken };
    Parser parserIncorrect(tokensIncorrect);
    number = parserIncorrect.parseNumber();
    if (!number) {
        std::cout << "parseNumber() correctly returned nullptr for non-integer token." << std::endl;
    }
    else {
        std::cerr << "parseNumber() test failed to return nullptr for non-integer token." << std::endl;
    }
}
void testParseLVal() {
    std::cout << "Starting parseLVal() test..." << std::endl;

    // Create a Parser object with an identifier token
    Token identToken(TokenType::IDN, "myVar", 1, 1);
    std::vector<Token> tokens{ identToken };
    Parser parser(tokens); // Pass the tokens vector to the constructor
    auto lVal = parser.parseLVal();
    if (lVal && lVal->identifier == "myVar") {
        std::cout << "parseLVal() test passed for identifier." << std::endl;
    }
    else {
        std::cerr << "parseLVal() test failed for identifier." << std::endl;
    }

    // Create a Parser object with a non-identifier token
    Token nonIdentToken(TokenType::INT, "123", 1, 1);
    tokens = { nonIdentToken };
    Parser parser2(tokens); // Pass the tokens vector to the constructor
    lVal = parser2.parseLVal();
    if (!lVal) {
        std::cout << "parseLVal() correctly returned nullptr for non-identifier." << std::endl;
    }
    else {
        std::cerr << "parseLVal() test failed to return nullptr for non-identifier." << std::endl;
    }
}


void testParseFuncFParam() {
    // ׼�� tokens
    std::vector<Token> tokens = {
        Token(TokenType::KW_INT, "int",1,1),
        Token(TokenType::IDN, "variableName",1,5)
    };

    // ���� Parser ʵ��
    Parser parser(tokens);

    // ���� parseFuncFParam
    std::shared_ptr<FuncFParam> funcFParam = parser.parseFuncFParam();

    // У�� funcFParam ����ȷ��
    bool testPassed = false;
    if (funcFParam != nullptr) {
        if (funcFParam->type.type == BasicType::INT &&
            funcFParam->ident == "variableName") {
            testPassed = true;
        }
    }

    // ������Խ��
    if (testPassed) {
        std::cout << "Test passed for parseFuncFParam" << std::endl;
    }
    else {
        std::cerr << "Test failed for parseFuncFParam" << std::endl;
    }
}

void testParseFuncFParams() {
    // ׼��Tokens���������parseFuncFParam������ȷ����int���ͺͱ�ʶ��
    std::vector<Token> tokens = {
        Token(TokenType::KW_INT, "int", 1, 1),  // ��������
        Token(TokenType::IDN, "param1", 1, 5), // ��һ����������
        Token(TokenType::SE_COMMA, ",", 1, 11), // ���ţ��ָ�����
        Token(TokenType::KW_INT, "int", 1, 13), // �ڶ�����������
        Token(TokenType::IDN, "param2", 1, 17)  // �ڶ�����������
        // ������Լ�����Ӹ����Token���Ӳ��Ե���Ҫ����
    };

    Parser parser(tokens); // ʹ��׼���õ�token��ʼ��Parser

    // ���ú���������������
    auto result = parser.parseFuncFParams();

    // ��֤���
    assert(result != nullptr && "parseFuncFParams returned nullptr");
    assert(result->params.size() == 2 && "Expected two function parameters");


    assert(result->params[0]->type.type == BasicType::INT && "First parameter type should be INT");
    assert(result->params[0]->ident == "param1" && "First parameter identifier should match");
    assert(result->params[1]->type.type == BasicType::INT && "Second parameter type should be INT");
    assert(result->params[1]->ident == "param2" && "Second parameter identifier should match");

    std::cout << "Test for parseFuncFParams passed." << std::endl;
}


int main() {
    // ����������ַ������У�ģ���ļ������������뷽ʽ
    std::string sourceCode =
        "int a = 10;\n"
        "int main(){\n"
        "a=10;\n"
        "return 0;\n"
        "}\n";

    // ʹ�� std::istringstream �Ա� Lexer ���Դ��ж�ȡ
    std::istringstream sourceStream(sourceCode);

    // ���� Lexer ʵ��������Դ�����Դ�����ַ���������� Lexer ���캯�������� sourceCode ��Ϊ�ַ�������ô�������캯����
    Lexer lexer(sourceStream, sourceCode);

    // ʹ�� Lexer ����Դ���룬���� tokens
    std::vector<Token> tokens = lexer.tokenize();

    // ��ѡ����ӡ tokens����������
    //lexer.printTokens();

    // ���� Parser ʵ�������� tokens
    Parser parser(tokens);

    // �������򣬷��س��� AST �ĸ��ڵ�
    std::shared_ptr<Program> program = parser.parseProgram();

    //������������ program ���н�һ���Ĳ����������ӡ������ AST������Ŀ������

    return 0;
}