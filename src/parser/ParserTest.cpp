#include "Lexer.h"
#include "Parser.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>

// 测试 Parser::advance 方法
void testAdvance(Parser& parser) {
    std::cout << "Testing Parser::advance" << std::endl;
    while (parser.advance()) {
        std::cout << "Advanced to token: " << parser.currentToken.lexeme << std::endl;
    }
    std::cout << "No more tokens to advance to. Reached end of token stream." << std::endl;
}

// 测试 Parser::match 方法
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

// 测试 Parser::syntaxError 方法
void testSyntaxError(Parser& parser) {
    std::cout << "Testing Parser::syntaxError" << std::endl;
    try {
        // 强制调用 syntaxError 来产生一个错误
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
        // 调用 parseProgram
        auto program = parser.parseProgram();

        // 检查 program 是否为 nullptr
        if (program == nullptr) {
            std::cerr << "parseProgram返回的program为nullptr，解析失败。" << std::endl;
        }
        else {
            std::cout << "parseProgram成功，AST创建完成。" << std::endl;

            // 根据实际AST结点类型检查 program 的结构
            if (!program->compUnits.empty()) {
                std::cout << "检测到正确数量的compUnits：" << program->compUnits.size() << std::endl;
            }
            else {
                std::cout << "警告：未检测到任何compUnits。" << std::endl;
            }

           

            std::cout << "parseProgram测试通过，所检查点均符合预期。" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "parseProgram测试失败: " << e.what() << std::endl;
    }
}


// 测试函数，检查parseCompUnit()是否正常工作
void testParseCompUnit(Parser& parser) {
    auto compUnit = parser.parseCompUnit();

    if (!compUnit) {
        std::cerr << "parseCompUnit失败，返回了nullptr。" << std::endl;
    }
    else {
        std::cout << "parseCompUnit成功返回，正在检查内容..." << std::endl;
        // 检查compUnit是否成功地包含了节点
        if (!compUnit->nodes.empty()) {
            std::cout << "parseCompUnit成功，解析到 " << compUnit->nodes.size() << " 个节点。" << std::endl;
        }
        else {
            std::cout << "parseCompUnit未解析到任何节点。" << std::endl;
        }
    }
}

void testParseDeclaration(Parser& parser) {
    auto declNode = parser.parseDeclaration();

    if (declNode) {
        std::cout << "parseDeclaration成功，返回了非空节点。" << std::endl;

        // 进一步可以检查节点的类型是否为VarDecl或ConstantDeclaration
        auto varDecl = std::dynamic_pointer_cast<VarDecl>(declNode);
        auto constDecl = std::dynamic_pointer_cast<ConstantDeclaration>(declNode);
        if (varDecl) {
            std::cout << "节点是VarDecl类型。" << std::endl;
        }
        else if (constDecl) {
            std::cout << "节点是ConstantDeclaration类型。" << std::endl;
        }
        else {
            std::cerr << "错误：返回节点既不是VarDecl也不是ConstantDeclaration类型。" << std::endl;
        }
    }
    else {
        std::cerr << "parseDeclaration失败，返回了空节点。" << std::endl;
    }
}

// 测试 parseConstDecl 函数
void testParseConstDecl() {
    std::cout << "Starting parseConstDecl() test..." << std::endl;

    // 创建一个模拟的 token 列表，表示 'const int a = 5;'
    std::vector<Token> tokens = {
        Token(TokenType::KW_CONST, "const", 1, 1),
        Token(TokenType::KW_INT, "int", 1, 7),
        Token(TokenType::IDN, "a", 1, 11),
        Token(TokenType::OP_ASSIGN, "=", 1, 13),
        Token(TokenType::INT, "5", 1, 15),
        Token(TokenType::SE_SEMICOLON, ";", 1, 16),
        Token(TokenType::EOF_TOKEN, "", 1, 17)
    };

    // 构造 Parser 对象并赋予 tokens
    Parser parser(tokens);

    // 检查 parseConstDecl 的返回值
    try {
        auto constDecl = parser.parseConstDecl();

        // 检查 parseConstDecl 的返回值是否非空
        if (constDecl) {
            std::cout << "parseConstDecl() test passed: Got a non-null ConstantDeclaration." << std::endl;
        }
        else {
            std::cerr << "parseConstDecl() test failed: Got a null ConstantDeclaration." << std::endl;
        }
    }
    catch (const std::exception& e) {
        // 如果发生异常，输出相关信息以进行调试
        std::cerr << "Exception occurred: " << e.what() << std::endl;
    }
}

// 测试 parseBasicType 函数
void testParseBasicType() {
    std::cout << "Starting parseBasicType() test..." << std::endl;

    // 创建模拟的正确和错误 token
    std::vector<Token> correctTokens = { Token(TokenType::KW_INT, "int", 1, 1) };
    std::vector<Token> wrongTokens = { Token(TokenType::IDN, "variable", 1, 1) };

    // 正确情况测试
    Parser parserCorrect(correctTokens);
    auto basicTypeResult = parserCorrect.parseBasicType();
    if (basicTypeResult && basicTypeResult->type == BasicType::Type::INT) {
        std::cout << "parseBasicType() test passed for 'int'." << std::endl;
    }
    else {
        std::cerr << "parseBasicType() test failed: did not recognize 'int' correctly." << std::endl;
    }

    // 错误情况测试
    try {
        Parser parserWrong(wrongTokens);
        basicTypeResult = parserWrong.parseBasicType();
        // 如果没有抛出异常，则测试失败
        std::cerr << "parseBasicType() test failed: did not throw an error for an unexpected token." << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "parseBasicType() correctly reported an error: " << e.what() << std::endl;
    }
}
// 测试 parseConstDef 函数
void testParseConstDef() {
    std::cout << "Starting parseConstDef() test..." << std::endl;

    // 创建模拟的 token 序列
    std::vector<Token> tokens = {
        Token(TokenType::IDN, "myConst", 1, 1),    // 标识符
        Token(TokenType::OP_ASSIGN, "=", 1, 9),    // 等号
        Token(TokenType::INT, "42", 1, 11),        // 整数常量值
        Token(TokenType::SE_SEMICOLON, ";", 1, 13) // 语句结束分号（可选，根据实际语法决定）
    };

    // 构造 Parser 对象并赋予 tokens
    Parser parser(tokens);

    // 调用 parseConstDef 并接收结果
    try {
        auto constDefResult = parser.parseConstDef();

        // 验证返回的 ConstDef 结果是否正确
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

    // 正确的整数 token
    Token intToken(TokenType::INT, "123", 1, 1);

    // 错误的非整数 token
    Token nonIntToken(TokenType::IDN, "abc", 1, 1);

    // 创建 Parser 对象并赋予正确的 token
    std::vector<Token> tokensCorrect{ intToken };
    Parser parserCorrect(tokensCorrect);
    auto intConst = parserCorrect.parseIntConst();
    if (intConst && intConst->value == 123) {
        std::cout << "parseIntConst() test passed with correct integer token." << std::endl;
    }
    else {
        std::cerr << "parseIntConst() test failed with correct integer token." << std::endl;
    }

    // 创建 Parser 对象并赋予错误的 token
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

    // 正确的标识符 token
    Token identToken(TokenType::IDN, "myVar", 1, 1);

    // 错误的非标识符 token
    Token nonIdentToken(TokenType::INT, "123", 1, 1);

    // 创建 Parser 对象并赋予正确的 token
    std::vector<Token> tokensCorrect{ identToken };
    Parser parserCorrect(tokensCorrect);
    auto ident = parserCorrect.parseIdent();
    if (ident && ident->name == "myVar") {
        std::cout << "parseIdent() test passed with correct identifier token." << std::endl;
    }
    else {
        std::cerr << "parseIdent() test failed with correct identifier token." << std::endl;
    }

    // 创建 Parser 对象并赋予错误的 token
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

    // 正确的 'int' 类型的函数 token
    Token voidToken(TokenType::KW_VOID, "void", 1, 1);
    Token intToken(TokenType::KW_INT, "int", 1, 1);

    // 错误的非函数类型 token
    Token nonFuncTypeToken(TokenType::IDN, "nonFuncType", 1, 1);

    // 创建 Parser 对象并赋予 'void' token
    std::vector<Token> tokensWithVoid{ voidToken };
    Parser parserWithVoid(tokensWithVoid);
    auto funcTypeVoid = parserWithVoid.parseFuncType();
    if (funcTypeVoid && funcTypeVoid->type == FuncTypeEnum::Void) {
        std::cout << "parseFuncType() test passed with 'void' token." << std::endl;
    }
    else {
        std::cerr << "parseFuncType() test failed with 'void' token." << std::endl;
    }

    // 创建 Parser 对象并赋予 'int' token
    std::vector<Token> tokensWithInt{ intToken };
    Parser parserWithInt(tokensWithInt);
    auto funcTypeInt = parserWithInt.parseFuncType();
    if (funcTypeInt && funcTypeInt->type == FuncTypeEnum::Integer) {
        std::cout << "parseFuncType() test passed with 'int' token." << std::endl;
    }
    else {
        std::cerr << "parseFuncType() test failed with 'int' token." << std::endl;
    }

    // 创建 Parser 对象并赋予错误的 token
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

    // 测试用的一元操作符 tokens
    Token plusToken(TokenType::OP_PLUS, "+", 1, 1);
    Token minusToken(TokenType::OP_MINUS, "-", 1, 2);
    // Token notToken(TokenType::YOUR_DEFINED_NOT_TOKEN, "!", 1, 3); // 根据您的TokenType定义替换YOUR_DEFINED_NOT_TOKEN

    // 非一元操作符 token
    Token nonUnaryToken(TokenType::INT, "123", 1, 4);

    // 测试 '+' 操作符
    std::vector<Token> tokensPlus{ plusToken };
    Parser parserPlus(tokensPlus);
    auto resultPlus = parserPlus.parseUnaryOp();

    if (resultPlus && resultPlus->op == '+') {
        std::cout << "parseUnaryOp() test passed for '+' operator." << std::endl;
    }
    else {
        std::cerr << "parseUnaryOp() test failed for '+' operator." << std::endl;
    }

    // 测试 '-' 操作符
    std::vector<Token> tokensMinus{ minusToken };
    Parser parserMinus(tokensMinus);
    auto resultMinus = parserMinus.parseUnaryOp();

    if (resultMinus && resultMinus->op == '-') {
        std::cout << "parseUnaryOp() test passed for '-' operator." << std::endl;
    }
    else {
        std::cerr << "parseUnaryOp() test failed for '-' operator." << std::endl;
    }

    // 测试 '!' 操作符
    // std::vector<Token> tokensNot { notToken };
    // Parser parserNot(tokensNot);
    // auto resultNot = parserNot.parseUnaryOp();

    // if (resultNot && resultNot->op == '!') {
    //     std::cout << "parseUnaryOp() test passed for '!' operator." << std::endl;
    // } else {
    //     std::cerr << "parseUnaryOp() test failed for '!' operator." << std::endl;
    // }

    // 测试非一元操作符的情况
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

    // 正确的整数常量 token
    Token intToken(TokenType::INT, "42", 1, 1);

    // 错误的非整数常量 token（比如一个标识符）
    Token nonIntToken(TokenType::IDN, "myVar", 1, 1);

    // 创建 Parser 对象并赋予正确的 token
    std::vector<Token> tokensCorrect{ intToken };
    Parser parserCorrect(tokensCorrect);
    auto number = parserCorrect.parseNumber();
    if (number && number->value == 42) {
        std::cout << "parseNumber() test passed with correct integer token." << std::endl;
    }
    else {
        std::cerr << "parseNumber() test failed with correct integer token." << std::endl;
    }

    // 创建 Parser 对象并赋予错误的 token
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
    // 准备 tokens
    std::vector<Token> tokens = {
        Token(TokenType::KW_INT, "int",1,1),
        Token(TokenType::IDN, "variableName",1,5)
    };

    // 创建 Parser 实例
    Parser parser(tokens);

    // 测试 parseFuncFParam
    std::shared_ptr<FuncFParam> funcFParam = parser.parseFuncFParam();

    // 校验 funcFParam 的正确性
    bool testPassed = false;
    if (funcFParam != nullptr) {
        if (funcFParam->type.type == BasicType::INT &&
            funcFParam->ident == "variableName") {
            testPassed = true;
        }
    }

    // 输出测试结果
    if (testPassed) {
        std::cout << "Test passed for parseFuncFParam" << std::endl;
    }
    else {
        std::cerr << "Test failed for parseFuncFParam" << std::endl;
    }
}

void testParseFuncFParams() {
    // 准备Tokens。这里假设parseFuncFParam可以正确处理int类型和标识符
    std::vector<Token> tokens = {
        Token(TokenType::KW_INT, "int", 1, 1),  // 参数类型
        Token(TokenType::IDN, "param1", 1, 5), // 第一个参数名称
        Token(TokenType::SE_COMMA, ",", 1, 11), // 逗号，分隔参数
        Token(TokenType::KW_INT, "int", 1, 13), // 第二个参数类型
        Token(TokenType::IDN, "param2", 1, 17)  // 第二个参数名称
        // 这里可以继续添加更多的Token，视测试的需要而定
    };

    Parser parser(tokens); // 使用准备好的token初始化Parser

    // 调用函数解析函数参数
    auto result = parser.parseFuncFParams();

    // 验证结果
    assert(result != nullptr && "parseFuncFParams returned nullptr");
    assert(result->params.size() == 2 && "Expected two function parameters");


    assert(result->params[0]->type.type == BasicType::INT && "First parameter type should be INT");
    assert(result->params[0]->ident == "param1" && "First parameter identifier should match");
    assert(result->params[1]->type.type == BasicType::INT && "Second parameter type should be INT");
    assert(result->params[1]->ident == "param2" && "Second parameter identifier should match");

    std::cout << "Test for parseFuncFParams passed." << std::endl;
}


int main() {
    // 将代码放入字符串流中，模拟文件流或其它输入方式
    std::string sourceCode =
        "int a = 10;\n"
        "int main(){\n"
        "a=10;\n"
        "return 0;\n"
        "}\n";

    // 使用 std::istringstream 以便 Lexer 可以从中读取
    std::istringstream sourceStream(sourceCode);

    // 创建 Lexer 实例，传递源代码和源代码字符串。（如果 Lexer 构造函数不接收 sourceCode 作为字符串，那么调整构造函数）
    Lexer lexer(sourceStream, sourceCode);

    // 使用 Lexer 解析源代码，生成 tokens
    std::vector<Token> tokens = lexer.tokenize();

    // 可选：打印 tokens，帮助调试
    //lexer.printTokens();

    // 创建 Parser 实例，传递 tokens
    Parser parser(tokens);

    // 解析程序，返回程序 AST 的根节点
    std::shared_ptr<Program> program = parser.parseProgram();

    //你可以在这里对 program 进行进一步的操作，例如打印、遍历 AST、生成目标代码等

    return 0;
}