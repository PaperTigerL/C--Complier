#include "Lexer.h"
#include <iostream>
#include <sstream>
#include <string>

// Auxiliary function to print test results
void printTestResult(const std::string& testName, char expected, char result) {
    std::cout << testName << ":\n";
    std::cout << "  Expected: " << expected << std::endl;
    std::cout << "  Result: " << result << std::endl;
    if (expected == result) {
        std::cout << "  Test PASSED\n\n";
    }
    else {
        std::cout << "  Test FAILED\n\n";
    }
}

void testGetNextChar() {
    // 测试getNextChar函数
    std::istringstream input("abc\nabc");
    Lexer lexer(input, "abc\nabc");

    printTestResult("getNextChar Test 1", 'a', lexer.getNextChar());
    printTestResult("getNextChar Test 2", 'b', lexer.getNextChar());
    // 继续编写其他测试...
}

void testUngetChar() {
    // 测试ungetChar函数
    std::istringstream input("abc");
    Lexer lexer(input, "abc");

    lexer.getNextChar(); // 'a'
    lexer.getNextChar(); // 'b'
    lexer.ungetChar('b');
    printTestResult("ungetChar Test 1", 'b', lexer.getNextChar());
    // 继续编写其他测试...
}

void testIsWhitespace() {
    // 测试isWhitespace函数
    Lexer lexer(std::istringstream(""), ""); // 假设有一个充分的初始化方法

    char space = ' ';
    char nonSpace = 'a';
    std::cout << "isWhitespace Test:\n";
    std::cout << "  Space Expected: true, Result: " << lexer.isWhitespace(space) << std::endl;
    std::cout << "  Non-space Expected: false, Result: " << lexer.isWhitespace(nonSpace) << std::endl;
}
void printTokenTestResult(const std::string& testName, const Token& expected, const Token& result) {
    std::cout << testName << ":\n";
    std::cout << "  Expected: " << expected << std::endl;
    std::cout << "  Result: " << result << std::endl;
    if (expected.type == result.type && expected.lexeme == result.lexeme && expected.line == result.line && expected.column == result.column) {
        std::cout << "  Test PASSED\n\n";
    }
    else {
        std::cout << "  Test FAILED\n\n";
    }
}

void testReadToken() {
    // 测试readToken函数
    std::istringstream input("int x = 10; return x;");
    Lexer lexer(input, "int x = 10; return x;");

    printTokenTestResult("readToken Test 1", Token(KW_INT, "int", 1, 1), lexer.readToken());
    printTokenTestResult("readToken Test 2", Token(IDN, "x", 1, 5), lexer.readToken());
    printTokenTestResult("readToken Test 3", Token(OP_ASSIGN, "=", 1, 7), lexer.readToken());
    printTokenTestResult("readToken Test 4", Token(INT, "10", 1, 9), lexer.readToken());
    printTokenTestResult("readToken Test 5", Token(SE_SEMICOLON, ";", 1, 11), lexer.readToken());
    printTokenTestResult("readToken Test 6", Token(KW_RETURN, "return", 1, 13), lexer.readToken());
    // 继续编写其他测试，以包括不同类型的 Token
}

void testGetTokens() {
    // 测试 getTokens 函数
    std::string sourceCode = "int x = 10; return x;";
    std::istringstream input(sourceCode);
    Lexer lexer(input, sourceCode);

    // 预期的 Token 列表
    std::vector<Token> expectedTokens = {
        Token(KW_INT, "int", 1, 1),
        Token(IDN, "x", 1, 5),
        Token(OP_ASSIGN, "=", 1, 7),
        Token(INT, "10", 1, 9),
        Token(SE_SEMICOLON, ";", 1, 11),
        Token(KW_RETURN, "return", 1, 13),
        Token(IDN, "x", 1, 20),
        Token(SE_SEMICOLON, ";", 1, 21),
        Token(EOF_TOKEN, "<EOF>", 1, 22) // 根据具体实现你可能需要添加 EOF Token
    };

    std::vector<Token> resultTokens = lexer.getTokens(); // 调用 getTokens 函数获取 Token 列表

    std::cout << "getTokens Test:\n";
    for (size_t i = 0; i < expectedTokens.size(); ++i) {
        if (i < resultTokens.size()) {
            printTokenTestResult("getTokens Test " + std::to_string(i + 1), expectedTokens[i], resultTokens[i]);
        }
        else {
            std::cout << "getTokens Test " << i + 1 << ":\n  Expected: " << expectedTokens[i] << std::endl;
            std::cout << "  No matching result token found.\n  Test FAILED\n\n";
        }
    }
    // 检查是否有多余的 Token
    if (resultTokens.size() > expectedTokens.size()) {
        for (size_t i = expectedTokens.size(); i < resultTokens.size(); ++i) {
            std::cout << "getTokens Test " << i + 1 << ":\n  Unexpected extra token: " << resultTokens[i] << std::endl;
            std::cout << "  Test FAILED\n\n";
        }
    }
}

// 测试printTokens函数
void testPrintTokens() {
    // 为了创建Lexer实例，我们需要准备输入流和源代码字符串
    std::istringstream input("");  // 创建一个空的输入流，仅用于构造函数
    std::string sourceCode = "";  // 空的源代码字符串

    // 创建一个测试用例的Lexer实例
    Lexer testLexer(input, sourceCode);

    // 设定Token列表
    testLexer.tokens = {
        Token(KW_INT, "int", 1, 1),
        Token(IDN, "a", 1, 5),
        Token(OP_ASSIGN, "=", 1, 7),
        Token(INT, "10", 1, 9),
        Token(SE_SEMICOLON, ";", 1, 11)
    };

    // 创建一个字符串流来捕获printTokens的输出
    std::ostringstream output;

    // 重定向cout到我们的字符串流
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
    std::cout.rdbuf(output.rdbuf());

    // 调用printTokens函数
    testLexer.printTokens();

    // 恢复cout的默认输出流
    std::cout.rdbuf(oldCoutStreamBuf);

    // 构造预期输出结果
    std::string expectedOutput =
        "int\t<KW,1>\n"
        "a\t<IDN,a>\n"
        "=\t<OP,11>\n"
        "10\t<INT,10>\n"
        ";\t<SE,24>\n";

    // 检查预期结果和实际输出是否一致
    if (output.str() == expectedOutput) {
        std::cout << "Test for printTokens PASSED" << std::endl;
    }
    else {
        std::cout << "Test for printTokens FAILED" << std::endl;
        std::cout << "Expected output:" << std::endl;
        std::cout << expectedOutput << std::endl;
        std::cout << "Actual output:" << std::endl;
        std::cout << output.str() << std::endl;
    }
}

// 测试reportError函数
void testReportError() {
    // 创建一个用于测试的Lexer实例，参数根据实际情况传递
    std::istringstream input(""); // 一个空的输入流用作参数
    std::string sourceCode = "";  // 空的源代码字符串
    Lexer testLexer(input, sourceCode);

    // 创建一个字符串流来捕获std::cerr的输出
    std::ostringstream capturedError;
    // 保存旧的错误流缓冲，以便在测试后恢复
    std::streambuf* oldCerrStreamBuf = std::cerr.rdbuf();
    std::cerr.rdbuf(capturedError.rdbuf());

    // 触发一个错误以测试reportError函数
    testLexer.reportError("Test error message", 10, 20);

    // 恢复std::cerr的默认输出流
    std::cerr.rdbuf(oldCerrStreamBuf);

    // 构造预期的错误信息输出
    std::string expectedErrorOutput = "Error on line 10, column 20: Test error message\n";

    // 检查是否捕获到了错误信息，并与预期输出一致
    if (capturedError.str() == expectedErrorOutput) {
        std::cout << "Test for reportError PASSED" << std::endl;
    }
    else {
        std::cout << "Test for reportError FAILED" << std::endl;
        std::cout << "Expected error output:\n" << expectedErrorOutput;
        std::cout << "Captured error output:\n" << capturedError.str();

        // Debug: Print ASCII values of expected and captured strings
        std::cout << "Expected error ASCII: ";
        for (const char c : expectedErrorOutput) {
            std::cout << static_cast<int>(c) << " ";
        }
        std::cout << std::endl;

        std::cout << "Captured error ASCII: ";
        for (const char c : capturedError.str()) {
            std::cout << static_cast<int>(c) << " ";
        }
        std::cout << std::endl;
    }
}


void testReadFile() {
    std::istringstream emptyInput; // 创建一个空的输入流
    Lexer lexer(emptyInput, ""); // 用空输入流和空源码字符串创建Lexer对象

    // 测试现有文件
    try {
        std::istream& is = lexer.readFile("D:\\file\\complier\\exp\\C--Compiler\\tests\\sourcecode.txt"); // 替换为您的文件路径
        std::cout << "File opened successfully, test passed." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Test failed with existing file: " << e.what() << std::endl;
        return;
    }

    // 测试不存在的文件
    try {
        std::istream& is2 = lexer.readFile("non_existing_file.txt");
        std::cerr << "Test failed with non-existing file: should have thrown an exception." << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "Exception caught successfully for non-existing file, test passed." << std::endl;
    }
}

void testTokenize() {
    // 定义一段测试源代码
    std::string source = "int a = 10;";
    // 创建与测试源代码关联的istringstream实例，用于Lexer读取
    std::istringstream iss(source);
    // 创建Lexer对象，同时传入源代码字符串
    Lexer lexer(iss, source);

    // 调用tokenize函数，获取token列表
    std::vector<Token> tokens = lexer.tokenize();

    // 创建一个预期结果的列表
    std::vector<Token> expected = {
     Token(KW_INT, "int", 1, 1),
     Token(IDN, "a", 1, 5),
     Token(OP_ASSIGN, "=", 1, 7),
     Token(INT, "10", 1, 9),
     Token(SE_SEMICOLON, ";", 1, 11),
     Token(EOF_TOKEN, "<EOF>", 1, 12), // Add EOF token to the expected tokens
    };

    // 检查预期结果与实际结果是否相符
    bool passed = true;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i >= expected.size() || tokens[i].toString() != expected[i].toString()) {
            passed = false;
            std::cout << "Mismatch at token index " << i << ":\n";
            std::cout << "Expected: " << (i < expected.size() ? expected[i].toString() : "No token") << "\n";
            std::cout << "Got: " << (i < tokens.size() ? tokens[i].toString() : "No token") << "\n\n";
        }
    }
    if (expected.size() > tokens.size()) {
        std::cout << "Tokens missing from the end.\n";
        passed = false;
    }

    // 输出测试结果
    if (passed) {
        std::cout << "Tokenize test passed." << std::endl;
    }
    else {
        std::cout << "Tokenize test failed." << std::endl;
    }
}
