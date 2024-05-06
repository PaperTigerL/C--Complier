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
    // ����getNextChar����
    std::istringstream input("abc\nabc");
    Lexer lexer(input, "abc\nabc");

    printTestResult("getNextChar Test 1", 'a', lexer.getNextChar());
    printTestResult("getNextChar Test 2", 'b', lexer.getNextChar());
    // ������д��������...
}

void testUngetChar() {
    // ����ungetChar����
    std::istringstream input("abc");
    Lexer lexer(input, "abc");

    lexer.getNextChar(); // 'a'
    lexer.getNextChar(); // 'b'
    lexer.ungetChar('b');
    printTestResult("ungetChar Test 1", 'b', lexer.getNextChar());
    // ������д��������...
}

void testIsWhitespace() {
    // ����isWhitespace����
    Lexer lexer(std::istringstream(""), ""); // ������һ����ֵĳ�ʼ������

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
    // ����readToken����
    std::istringstream input("int x = 10; return x;");
    Lexer lexer(input, "int x = 10; return x;");

    printTokenTestResult("readToken Test 1", Token(KW_INT, "int", 1, 1), lexer.readToken());
    printTokenTestResult("readToken Test 2", Token(IDN, "x", 1, 5), lexer.readToken());
    printTokenTestResult("readToken Test 3", Token(OP_ASSIGN, "=", 1, 7), lexer.readToken());
    printTokenTestResult("readToken Test 4", Token(INT, "10", 1, 9), lexer.readToken());
    printTokenTestResult("readToken Test 5", Token(SE_SEMICOLON, ";", 1, 11), lexer.readToken());
    printTokenTestResult("readToken Test 6", Token(KW_RETURN, "return", 1, 13), lexer.readToken());
    // ������д�������ԣ��԰�����ͬ���͵� Token
}

void testGetTokens() {
    // ���� getTokens ����
    std::string sourceCode = "int x = 10; return x;";
    std::istringstream input(sourceCode);
    Lexer lexer(input, sourceCode);

    // Ԥ�ڵ� Token �б�
    std::vector<Token> expectedTokens = {
        Token(KW_INT, "int", 1, 1),
        Token(IDN, "x", 1, 5),
        Token(OP_ASSIGN, "=", 1, 7),
        Token(INT, "10", 1, 9),
        Token(SE_SEMICOLON, ";", 1, 11),
        Token(KW_RETURN, "return", 1, 13),
        Token(IDN, "x", 1, 20),
        Token(SE_SEMICOLON, ";", 1, 21),
        Token(EOF_TOKEN, "<EOF>", 1, 22) // ���ݾ���ʵ���������Ҫ��� EOF Token
    };

    std::vector<Token> resultTokens = lexer.getTokens(); // ���� getTokens ������ȡ Token �б�

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
    // ����Ƿ��ж���� Token
    if (resultTokens.size() > expectedTokens.size()) {
        for (size_t i = expectedTokens.size(); i < resultTokens.size(); ++i) {
            std::cout << "getTokens Test " << i + 1 << ":\n  Unexpected extra token: " << resultTokens[i] << std::endl;
            std::cout << "  Test FAILED\n\n";
        }
    }
}

// ����printTokens����
void testPrintTokens() {
    // Ϊ�˴���Lexerʵ����������Ҫ׼����������Դ�����ַ���
    std::istringstream input("");  // ����һ���յ��������������ڹ��캯��
    std::string sourceCode = "";  // �յ�Դ�����ַ���

    // ����һ������������Lexerʵ��
    Lexer testLexer(input, sourceCode);

    // �趨Token�б�
    testLexer.tokens = {
        Token(KW_INT, "int", 1, 1),
        Token(IDN, "a", 1, 5),
        Token(OP_ASSIGN, "=", 1, 7),
        Token(INT, "10", 1, 9),
        Token(SE_SEMICOLON, ";", 1, 11)
    };

    // ����һ���ַ�����������printTokens�����
    std::ostringstream output;

    // �ض���cout�����ǵ��ַ�����
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
    std::cout.rdbuf(output.rdbuf());

    // ����printTokens����
    testLexer.printTokens();

    // �ָ�cout��Ĭ�������
    std::cout.rdbuf(oldCoutStreamBuf);

    // ����Ԥ��������
    std::string expectedOutput =
        "int\t<KW,1>\n"
        "a\t<IDN,a>\n"
        "=\t<OP,11>\n"
        "10\t<INT,10>\n"
        ";\t<SE,24>\n";

    // ���Ԥ�ڽ����ʵ������Ƿ�һ��
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

// ����reportError����
void testReportError() {
    // ����һ�����ڲ��Ե�Lexerʵ������������ʵ���������
    std::istringstream input(""); // һ���յ���������������
    std::string sourceCode = "";  // �յ�Դ�����ַ���
    Lexer testLexer(input, sourceCode);

    // ����һ���ַ�����������std::cerr�����
    std::ostringstream capturedError;
    // ����ɵĴ��������壬�Ա��ڲ��Ժ�ָ�
    std::streambuf* oldCerrStreamBuf = std::cerr.rdbuf();
    std::cerr.rdbuf(capturedError.rdbuf());

    // ����һ�������Բ���reportError����
    testLexer.reportError("Test error message", 10, 20);

    // �ָ�std::cerr��Ĭ�������
    std::cerr.rdbuf(oldCerrStreamBuf);

    // ����Ԥ�ڵĴ�����Ϣ���
    std::string expectedErrorOutput = "Error on line 10, column 20: Test error message\n";

    // ����Ƿ񲶻��˴�����Ϣ������Ԥ�����һ��
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
    std::istringstream emptyInput; // ����һ���յ�������
    Lexer lexer(emptyInput, ""); // �ÿ��������Ϳ�Դ���ַ�������Lexer����

    // ���������ļ�
    try {
        std::istream& is = lexer.readFile("D:\\file\\complier\\exp\\C--Compiler\\tests\\sourcecode.txt"); // �滻Ϊ�����ļ�·��
        std::cout << "File opened successfully, test passed." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Test failed with existing file: " << e.what() << std::endl;
        return;
    }

    // ���Բ����ڵ��ļ�
    try {
        std::istream& is2 = lexer.readFile("non_existing_file.txt");
        std::cerr << "Test failed with non-existing file: should have thrown an exception." << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "Exception caught successfully for non-existing file, test passed." << std::endl;
    }
}

void testTokenize() {
    // ����һ�β���Դ����
    std::string source = "int a = 10;";
    // ���������Դ���������istringstreamʵ��������Lexer��ȡ
    std::istringstream iss(source);
    // ����Lexer����ͬʱ����Դ�����ַ���
    Lexer lexer(iss, source);

    // ����tokenize��������ȡtoken�б�
    std::vector<Token> tokens = lexer.tokenize();

    // ����һ��Ԥ�ڽ�����б�
    std::vector<Token> expected = {
     Token(KW_INT, "int", 1, 1),
     Token(IDN, "a", 1, 5),
     Token(OP_ASSIGN, "=", 1, 7),
     Token(INT, "10", 1, 9),
     Token(SE_SEMICOLON, ";", 1, 11),
     Token(EOF_TOKEN, "<EOF>", 1, 12), // Add EOF token to the expected tokens
    };

    // ���Ԥ�ڽ����ʵ�ʽ���Ƿ����
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

    // ������Խ��
    if (passed) {
        std::cout << "Tokenize test passed." << std::endl;
    }
    else {
        std::cout << "Tokenize test failed." << std::endl;
    }
}
