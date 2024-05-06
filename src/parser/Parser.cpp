#include "Parser.h"
bool Parser::advance() {
    // ȷ���������ᳬ��tokens�����ĳ���
    if (index < tokens.size() - 1) {
        index++;
        currentToken = tokens.at(index);
        return true;
    }
    return false;
}
void Parser::rollback(size_t position) {
    // Ensure the rollback position is within the range of the tokens vector
    if (position < tokens.size()) {
        index = position; // Reset the index to the given position
        currentToken = tokens.at(index); // Update the currentToken to match the new index
    }
    else {
        // Handle the error for an invalid rollback position if necessary
        syntaxError("Attempting to rollback to an invalid token position");
    }
}

bool Parser::match(TokenType expected) {
    if (currentToken.type == expected) {
        advance();
        return true;
    }
    else {
        return false;
    }
}
void Parser::syntaxError(const std::string& message) {
    // ���������Ӵ�����Ĵ��룬�����¼����λ�á�������Ϣ��
    std::cerr << "Syntax Error: " << message << std::endl;
    // ѡ���׳��쳣����ֹ��������
    throw std::runtime_error(message);
}
//1. Program -> compUnit
std::shared_ptr<Program> Parser::parseProgram() {
    printParsingAction("Program", "enter");  // ���� Program ����
    auto program = std::make_shared<Program>();


    // ѭ������ֱ���ļ�����
    while (currentToken.type != TokenType::EOF_TOKEN) {
        try {
            // ����compUnit
            auto compUnit = parseCompUnit();
            if (compUnit) {
                // ��ӵ�program��compUnits��
                program->compUnits.push_back(compUnit);
            }
        }
        catch (const std::exception& e) {
            // ���յ��쳣�������﷨����
            syntaxError(e.what());
            printParsingAction("Program", "error");  // ��������
            // ������������ѡ����ֹ����
            break;
        }
    }
    printParsingAction("Program", "exit");  // �˳� Program ����
    return program;
}


//2. compUnit -> (decl | funcDef)* EOF;
std::shared_ptr<CompUnit> Parser::parseCompUnit() {
    printParsingAction("CompUnit", "enter");  // ���� CompUnit ����
    auto compUnit = std::make_shared<CompUnit>();
   
    while (currentToken.type != TokenType::EOF_TOKEN) {
        printParsingAction("CompUnit", "attempt");  // ����ʶ��������������
        std::shared_ptr<ASTNode> node;
        
        // ���ȳ��Խ���������declaration��
        node = parseDeclaration();
        if (node) {
            printParsingAction("CompUnit", "reduction: decl");  // ��Լ����
        }
        else {
            // �����������ʧ�ܣ����Խ����������壨function definition��
            node = parseFuncDef();
            if (node) {
                printParsingAction("CompUnit", "reduction: funcDef");  // ��Լ��������
            }
        }
        // ������߶��޷������������﷨����
        if (!node) {
            syntaxError("Unexpected token when expecting a declaration or a function definition.");
            printParsingAction("CompUnit", "error");  // ����
            return nullptr;
        }

        // �������ɹ��Ľڵ���ӵ� compUnit ��
        compUnit->addNode(node);

        // ����ǰ������һ�� Token
        if (currentToken.type != TokenType::EOF_TOKEN) {
            printParsingAction("CompUnit", "advance");  // �ƽ�
        }
    }

    printParsingAction("CompUnit", "exit");  // �˳� CompUnit ����
    return compUnit; // ���ع����õ� CompUnit
}

//3. decl -> constDecl | varDecl;
// ���������ĺ��������Խ���Ϊ�������������������
std::shared_ptr<Declaration> Parser::parseDeclaration() {
    printParsingAction("decl", "enter");  // ��ʼ��������

    auto saveIndex = index; // ���浱ǰλ��
    auto saveToken = currentToken; // ���浱ǰtoken
    std::shared_ptr<Declaration> node = nullptr;

    // ���Խ��� const ����
    node = parseConstDecl();
    if (node) {
        printParsingAction("decl", "reduction: constDecl");  // �ɹ����� const ����
        return node;
    }
    else {
        printParsingAction("decl", "backtrack: constDecl failure");  // const ��������ʧ�ܣ�����
    }

    // ���˵���ǰ�����λ�ú�token
    index = saveIndex;
    currentToken = saveToken;

    // ���Խ��� var ����
    node = parseVarDecl();
    if (node) {
        printParsingAction("decl", "reduction: varDecl");  // �ɹ����� var ����
        return node;
    }

    // ������߶��޷������������﷨����
    printParsingAction("decl", "error");  // ��������
    //syntaxError("Unexpected token when expecting a declaration.");

    printParsingAction("decl", "exit");  // �˳���������
    return nullptr;
}


//4. constDecl -> 'const' bType constDef (',' constDef)* ';';
std::shared_ptr<ConstantDeclaration> Parser::parseConstDecl() {
    size_t startIndex = index; // ���浱ǰtokenλ��

    printParsingAction("constDecl", "enter"); // ���볣����������

    if (!match(TokenType::KW_CONST)) {
        rollback(startIndex); // �ع���������ʼ��λ��
        return nullptr; // ���û��ƥ�䵽 'const'���򷵻ؿ�
    }
    printParsingAction("constDecl", "match KW_CONST");

    auto basicTypePtr = parseBasicType(); // ������������ bType
    if (!basicTypePtr) {
        rollback(startIndex); // �ع�
        printParsingAction("constDecl", "error: failed to parse bType");
        return nullptr;
    }
    printParsingAction("constDecl", "reduction: bType");

    BasicType basicType = *basicTypePtr; // ��ȡ BasicType ʵ��
    std::vector<std::shared_ptr<ConstDef>> constDefs;

    auto constDef = parseConstDef(); // ������һ�� constDef
    if (!constDef) {
        rollback(startIndex); // �ع�
        printParsingAction("constDecl", "error: failed to parse constDef");
        return nullptr;
    }
    constDefs.push_back(constDef);
    printParsingAction("constDecl", "reduction: constDef");

    while (currentToken.type == TokenType::SE_COMMA) {
        advance(); // �̵����� ','
        printParsingAction("constDecl", "advance after comma");

        constDef = parseConstDef(); // ���������� constDef
        if (!constDef) {
            rollback(startIndex); // �ع�
            printParsingAction("constDecl", "error: failed to parse subsequent constDef");
            return nullptr;
        }
        constDefs.push_back(constDef);
        printParsingAction("constDecl", "reduction: constDef");
    }

    if (!match(TokenType::SE_SEMICOLON)) { // ȷ�ϷֺŽ��� ';'
        rollback(startIndex); // �ع�
        return nullptr; // ���û��ƥ�䵽�ֺţ����ؿ�
    }
    printParsingAction("constDecl", "match SE_SEMICOLON");

    printParsingAction("constDecl", "exit"); // �˳�������������
    // ���������س�������ʵ��
    return std::make_shared<ConstantDeclaration>(basicType, constDefs);
}


//5. bType -> 'int';
std::shared_ptr<BasicType> Parser::parseBasicType() {
    printParsingAction("BasicType", "enter"); // ����������͹���

    if (currentToken.type == TokenType::KW_INT) {
        printParsingAction("BasicType", "match KW_INT"); // ƥ�䵽 'int' �ؼ���

        auto basicType = std::make_shared<BasicType>(BasicType::INT);
        advance(); // �ƽ�����һ�� token

        printParsingAction("BasicType", "exit"); // �˳��������͹���
        return basicType;
    }
    else {
        printParsingAction("BasicType", "error: expected 'int'"); // ������������ 'int' �ؼ���
       
        return nullptr; // ����·��������ʧ��
    }
}

//6. constDef -> Ident '=' constInitVal;
std::shared_ptr<ConstDef> Parser::parseConstDef() {
    size_t startIndex = index; // ���溯����ʼ��λ��

    printParsingAction("ConstDef", "enter"); // ���볣���������

    if (currentToken.type != TokenType::IDN) {
        printParsingAction("ConstDef", "error: expected identifier"); // Ԥ��Ϊ��ʶ������δƥ�䵽
        syntaxError("��������ȱ�ٱ�ʶ��");
        rollback(startIndex); // �ع���������ʼ��λ��
        return nullptr;
    }

    printParsingAction("ConstDef", "match IDN"); // ƥ�䵽��ʶ��
    std::string constName = currentToken.lexeme;
    advance(); // ���ѱ�ʶ�� Token

    if (currentToken.type != TokenType::OP_ASSIGN) {
        printParsingAction("ConstDef", "error: expected '='"); // Ԥ��Ϊ�Ⱥţ���δƥ�䵽
        syntaxError("����������ȱ�ٵȺ� '='");
        rollback(startIndex); // �ع�
        return nullptr;
    }

    printParsingAction("ConstDef", "match '='"); // ƥ�䵽�Ⱥ�
    advance(); // ���� '=' Token

    auto initVal = parseConstInitVal(); // ����������ʼֵ
    if (!initVal) {
        printParsingAction("ConstDef", "error: failed to parse constInitVal"); // ����������ʼֵʧ��
        rollback(startIndex); // �ع�
        return nullptr;
    }

    printParsingAction("ConstDef", "reduction: constInitVal"); // ��Լ������ʼֵ
    printParsingAction("ConstDef", "exit"); // �˳������������
    return std::make_shared<ConstDef>(constName, initVal->constExp); // ���������� ConstDef �ڵ�
}

//7. constInitVal -> constExp
std::shared_ptr<ConstInitVal> Parser::parseConstInitVal() {
    printParsingAction("ConstInitVal", "enter");  // ���� ConstInitVal ����

    auto exp = parseConstExp(); // �����������ʽ
    if (!exp) {
        printParsingAction("ConstInitVal", "error: failed to parse constExp"); // �����������ʽʧ��
        return nullptr;
    }

    printParsingAction("ConstInitVal", "reduction: constExp"); // ��Լ�������ʽ
    printParsingAction("ConstInitVal", "exit");  // �˳� ConstInitVal ����
    return std::make_shared<ConstInitVal>(exp); // ���������� ConstInitVal �ڵ�
}

//8.varDecl -> bType varDef (',' varDef)* ';';
std::shared_ptr<VarDecl> Parser::parseVarDecl() {
    size_t startPosition = index; // ��¼��ʼʱ��λ��
    printParsingAction("VarDecl", "enter");  // ���� VarDecl ����

    auto type = parseBasicType();  // ������������
    if (!type) {
        printParsingAction("VarDecl", "error: failed to parse bType");
        rollback(startPosition); // �ع�����ʼλ��
        return nullptr;  // ����������ͽ���ʧ�ܣ��򷵻� nullptr
    }
    printParsingAction("VarDecl", "reduction: bType");

    std::vector<std::shared_ptr<VarDef>> defs;  // ���ڴ洢�������������
    auto varDef = parseVarDef();  // ������һ����������
    if (!varDef) {
        printParsingAction("VarDecl", "error: failed to parse first varDef");
        rollback(startPosition); // �ع�����ʼλ��
        return nullptr;  // ��������������ʧ�ܣ��򷵻� nullptr
    }
    defs.push_back(varDef);  // ����һ������������ӵ�������
    printParsingAction("VarDecl", "reduction: varDef");

    // ��������ı������壬����еĻ�
    while (currentToken.type == TokenType::SE_COMMA) {
        advance();  // ���Ѷ���
        printParsingAction("VarDecl", "advance after comma");

        varDef = parseVarDef();  // ������һ����������
        if (!varDef) {
            printParsingAction("VarDecl", "error: failed to parse subsequent varDef");
            rollback(startPosition); // �ع�����ʼλ��
            return nullptr;  // ��������������ʧ�ܣ������Ŀǰ�ռ�����varDef�������� nullptr
        }
        defs.push_back(varDef);  // �������õı���������ӵ�������
        printParsingAction("VarDecl", "reduction: varDef");
    }

    if (currentToken.type != TokenType::SE_SEMICOLON) {
        printParsingAction("VarDecl", "error: missing semicolon");
        syntaxError("��������û���Էֺ� ';' ����");
        rollback(startPosition); // �ع�����ʼλ��
        return nullptr;
    }
    printParsingAction("VarDecl", "match SE_SEMICOLON");

    advance();  // ���ѷֺ�

    printParsingAction("VarDecl", "exit");  // �˳� VarDecl ����
    return std::make_shared<VarDecl>(type, defs); // ���������ر�����������
}

//9. varDef -> Ident | Ident '=' initVal;
std::shared_ptr<VarDef> Parser::parseVarDef() {
    printParsingAction("VarDef", "enter");  // ���� VarDef ����

    // ȷ�ϵ�ǰ token �Ǳ�ʶ��
    if (currentToken.type != TokenType::IDN) {
        printParsingAction("VarDef", "error: expected identifier");
        return nullptr;
    }

    printParsingAction("VarDef", "match IDN"); // ƥ�䵽��ʶ��
    std::string identName = currentToken.lexeme; // ��ȡ������
    advance(); // ���ѱ�ʶ��

    std::shared_ptr<InitVal> initVal = nullptr; // ������ʼֵ���ʽ��ָ�룬Ĭ��Ϊ nullptr

    Token rollbackToken = currentToken; // ���浱ǰ��λ���Ա�ع�

    // �����һ�� token �ǵȺ� '='����ô������һ����ʼֵ��Ҫ����
    if (currentToken.type == TokenType::OP_ASSIGN) {
        advance(); // ���� '='
        printParsingAction("VarDef", "match '='"); // ƥ�䵽�Ⱥ�

        initVal = parseInitVal(); // ������ʼ��ֵ
        if (!initVal) {
            printParsingAction("VarDef", "error: failed to parse initVal");
            currentToken = rollbackToken; // �ع�����ǰ��λ��
            return nullptr; // ���� nullptr ��ʾ����ʧ��
        }
        printParsingAction("VarDef", "reduction: initVal"); // ��Լ��ʼ��ֵ
    }

    // ���������Ҫô��һ��δ��ʼ���ı���������Ҫô��һ�����г�ʼ��ֵ�ı�������
    // �����Ƿ��г�ʼֵ������Ӧ�� VarDef �ڵ㲢����
    auto varDef = initVal ? std::make_shared<VarDef>(identName, initVal)
        : std::make_shared<VarDef>(identName);
    printParsingAction("VarDef", "exit");  // �˳� VarDef ����
    return varDef;
}

//10. initVal -> exp;
std::shared_ptr<InitVal> Parser::parseInitVal() {
    printParsingAction("InitVal", "enter");  // ��ʼ���� InitVal

    // ������ʼ��ֵ�еı��ʽ
    auto expression = parseExp();
    if (!expression) {
        printParsingAction("InitVal", "error: failed to parse expression");
        // ����������ʽʧ�ܣ��򷵻� nullptr
        return nullptr;
    }
    printParsingAction("InitVal", "reduction: expression");  // �����ɹ����ʽ

    // ���������� InitVal �ڵ�
    auto initVal = std::make_shared<InitVal>(expression);
    printParsingAction("InitVal", "exit");  // ���� InitVal ���
    return initVal;
}

//11. funcDef -> funcType Ident '(' (funcFParams)? ')' block;
std::shared_ptr<FuncDef> Parser::parseFuncDef() {
    printParsingAction("FuncDef", "enter");  // ���� FuncDef ����
    Token rollbackToken = currentToken; // ���浱ǰ��λ���Ա�ع�

    // ����������������
    auto type = parseFuncType();
    if (!type) {
        printParsingAction("FuncDef", "error: failed to parse return type");
        currentToken = rollbackToken; // �ع����������忪ʼ��λ��
        return nullptr; // �������ͽ���ʧ��
    }
    printParsingAction("FuncDef", "reduction: return type"); // �����ɹ�������������

    // ��������������֧�� KW_MAIN �ؼ���
    std::string name;
    if (currentToken.type == TokenType::KW_MAIN) { // ֧�� KW_MAIN Ϊ������
        name = "main"; // ����� main �ؼ���
        printParsingAction("FuncDef", "match function name 'main'");  // ƥ�䵽������ 'main'
    }
    else if (currentToken.type == TokenType::IDN) { // ֧����ͨ��ʶ��Ϊ������
        name = currentToken.lexeme; // �������ͨ��ʶ��
        printParsingAction("FuncDef", "match function name");  // ƥ�䵽������
    }
    else {
        printParsingAction("FuncDef", "error: expected function name identifier or 'main'");
        syntaxError("ȱ�ٺ�������");
        currentToken = rollbackToken; // �ع�
        return nullptr;
    }
    advance(); // ���Ѻ�������ʶ���� main �ؼ���

    // ƥ�������� '('
    if (currentToken.type != TokenType::SE_LPAREN) {
        printParsingAction("FuncDef", "error: expected '('");
        syntaxError("�ں���������ȱ�� '('");
        currentToken = rollbackToken; // �ع�
        return nullptr;
    }
    printParsingAction("FuncDef", "match '('");  // ƥ�䵽������
    advance(); // ���� '('

    // ����в��������������б�
    std::shared_ptr<FuncFParams> parameters = nullptr;
    if (currentToken.type != TokenType::SE_RPAREN) {
        parameters = parseFuncFParams();
        if (!parameters) {
            printParsingAction("FuncDef", "error: failed to parse function parameters");
            currentToken = rollbackToken; // �ع�
            return nullptr; // ������������б�ʧ�ܣ��򷵻� nullptr
        }
        printParsingAction("FuncDef", "reduction: parameters"); // �����ɹ������б�
    }

    // ƥ�������� ')'
    if (currentToken.type != TokenType::SE_RPAREN) {
        printParsingAction("FuncDef", "error: expected ')'");
        syntaxError("�ں���������ȱ�� ')'");
        currentToken = rollbackToken; // �ع�
        return nullptr;
    }
    printParsingAction("FuncDef", "match ')'");  // ƥ�䵽������
    advance(); // ���� ')'

    // ����������
    auto block = parseBlock();
    if (!block) {
        printParsingAction("FuncDef", "error: failed to parse function body");
        currentToken = rollbackToken; // �ع�
        return nullptr; // ���������ʧ��
    }
    printParsingAction("FuncDef", "reduction: function body"); // �����ɹ�������

    // ���������� FuncDef ����
    auto funcDef = std::make_shared<FuncDef>(type, name, parameters, block);
    printParsingAction("FuncDef", "exit");  // �뿪 FuncDef ����
    return funcDef;
}

//12. funcType -> 'void' | 'int' ;
std::shared_ptr<FuncType> Parser::parseFuncType() {
    printParsingAction("FuncType", "enter");  // ���� FuncType ����
    Token rollbackToken = currentToken; // ���浱ǰ��λ���Ա�ع�

    if (currentToken.type == TokenType::KW_INT) {
        printParsingAction("FuncType", "match 'int'");  // ƥ�䵽 'int'
        advance(); // �ƽ�����һ�� token
        auto funcType = std::make_shared<FuncType>(FuncTypeEnum::Integer);
        printParsingAction("FuncType", "reduction: 'int'"); // ��ԼΪ 'int'
        printParsingAction("FuncType", "exit");  // �뿪 FuncType ����
        return funcType;
    }
    else if (currentToken.type == TokenType::KW_VOID) {
        printParsingAction("FuncType", "match 'void'");  // ƥ�䵽 'void'
        advance(); // �ƽ�����һ�� token
        auto funcType = std::make_shared<FuncType>(FuncTypeEnum::Void);
        printParsingAction("FuncType", "reduction: 'void'"); // ��ԼΪ 'void'
        printParsingAction("FuncType", "exit");  // �뿪 FuncType ����
        return funcType;
    }
    else {
        // �����ǰ token ���� 'int' �� 'void'���򱨸��﷨����
        printParsingAction("FuncType", "error: expected 'int' or 'void'");
        syntaxError("Expected 'int' or 'void' in function type, found: " + currentToken.lexeme);
        currentToken = rollbackToken; // �ع����������ͽ���֮ǰ��λ��
        return nullptr; // ���� nullptr ��ʾ����ʧ��
    }
}

//13. funcFParams -> funcFParam (',' funcFParam)*;
std::shared_ptr<FuncFParams> Parser::parseFuncFParams() {
    printParsingAction("FuncFParams", "enter");  // ��ʼ���� FuncFParams
    Token rollbackToken = currentToken; // ���浱ǰ��λ���Ա�ع�

    auto funcFParams = std::make_shared<FuncFParams>();

    // Parse the first parameter (at least one parameter must exist for this rule)
    auto param = parseFuncFParam();
    if (!param) {
        printParsingAction("FuncFParams", "error: expected a function parameter");  // ������Ϣ
        syntaxError("Expected a function parameter");
        currentToken = rollbackToken; // �ع����������ͽ���֮ǰ��λ��
        return nullptr;
    }
    funcFParams->addParam(param);

    // Continue parsing parameters if a comma is found
    while (currentToken.type == SE_COMMA) {
        printParsingAction("FuncFParams", "match ','");  // ƥ�䵽 ','
        // Consume the comma
        advance();

        // Parse the next parameter
        param = parseFuncFParam();
        if (!param) {
            printParsingAction("FuncFParams", "error: expected a function parameter after comma");  // ������Ϣ
            syntaxError("Expected a function parameter after comma");
            currentToken = rollbackToken; // �ع����������ͽ���֮ǰ��λ��
            return nullptr;
        }
        funcFParams->addParam(param);
    }

    printParsingAction("FuncFParams", "exit");  // �뿪 FuncFParams ����
    return funcFParams;
}
//14. funcFParam -> bType Ident;
std::shared_ptr<FuncFParam> Parser::parseFuncFParam() {
    printParsingAction("FuncFParam", "enter");  // ��ʼ���� FuncFParam

    // Parse bType
    auto bType = parseBasicType();
    if (!bType) {
        printParsingAction("FuncFParam", "error: expected a basic type for funcFParam");  // ������Ϣ
        syntaxError("Expected basic type for funcFParam");
        return nullptr;
    }
    printParsingAction("FuncFParam", "reduction: bType");  // �ɹ�������������

    // Parse Ident
    auto ident = parseIdent();
    if (!ident) {
        printParsingAction("FuncFParam", "error: expected an identifier for funcFParam");  // ������Ϣ
        syntaxError("Expected identifier for funcFParam");
        return nullptr;
    }
    printParsingAction("FuncFParam", "match ident");  // ƥ�䵽��ʶ��

    // ʹ������Ident�����Ѿ�����õ�name��Ա����
    std::string identName = ident->name;

    // Create and return FuncFParam object
    // ����ʹ�ý����ò�����*��������bType���Ի�ȡBasicType��ʵ��
    // Ȼ��identName���ݸ�FuncFParam�Ĺ��캯��
    auto funcFParam = std::make_shared<FuncFParam>(*bType, identName);
    printParsingAction("FuncFParam", "exit");  // �뿪 FuncFParam ����
    return funcFParam;
}
//15. block -> '{' (blockItem)* '}';
std::shared_ptr<Block> Parser::parseBlock() {
    printParsingAction("Block", "enter");  // ��ʼ���� Block
    Token rollbackToken = currentToken; // ���浱ǰ��λ���Ա�ع�

    // ȷ�ϵ�ǰ token �ǲ���������� '{'
    if (currentToken.type != TokenType::SE_LBRACE) {
        printParsingAction("Block", "error: expected '{'");  // ������Ϣ
        syntaxError("Expected '{' at the beginning of block");
        currentToken = rollbackToken; // �ع������� Block ����ǰ��λ��
        return nullptr;
    }
    printParsingAction("Block", "match '{'");  // ƥ�䵽�������
    advance(); // ���� '{'

    // ����һ���µ� Block ʵ��
    auto block = std::make_shared<Block>();

    // ѭ������ blockItem��ֱ�������Ҵ����� '}'
    while (currentToken.type != TokenType::EOF_TOKEN) {
        // ��������Ҵ����ţ����������
        if (currentToken.type == TokenType::SE_RBRACE) {
            printParsingAction("Block", "match '}'");  // ƥ�䵽�Ҵ�����
            advance(); // �����Ҵ����� '}'
            break; // ��ʱӦ������ while ѭ��
        }

        // ��ʼ���� BlockItem
        printParsingAction("Block", "start parse BlockItem");
        auto item = parseBlockItem();
        if (!item) {
            // ����ʧ�ܵ�����£�parseBlockItem Ӧ���Ѿ������˴�����Ϣ
            currentToken = rollbackToken; // �ع������� Block ����ǰ��λ��
            return nullptr;
        }
        // �ɹ����� BlockItem ����ӵ� block ��
        printParsingAction("Block", "reduce BlockItem");
        block->addItem(item);

        // ������� advance()��parseBlockItem() �ڲ�����
    }

    printParsingAction("Block", "exit");  // �뿪 Block ����
    return block; // ������ɽ����Ĵ����
}

//16. blockItem -> decl | stmt;
std::shared_ptr<BlockItem> Parser::parseBlockItem() {
    printParsingAction("BlockItem", "enter");  // ��ʼ���� BlockItem

    // ���ȣ�����Ҫ���ֵ�ǰ�� token �Ƿ��ʾ�������
    // ������俪ʼ�ڹؼ��֣����� 'int'��'const' ��
    if (currentToken.type == TokenType::KW_INT || currentToken.type == TokenType::KW_CONST) {
        printParsingAction("BlockItem", "start parse Declaration");  // ��ʼ�����������

        auto declaration = parseDeclaration();

        if (!declaration) {
            printParsingAction("BlockItem", "error: failed to parse Declaration");  // �����������ʧ��
            return nullptr;
        }

        printParsingAction("BlockItem", "reduce Declaration");  // �ɹ������������
        return declaration;

    }
    else {
        printParsingAction("BlockItem", "start parse Stmt");  // ��ʼ�������
        auto statement = parseStmt();

        if (!statement) {
            printParsingAction("BlockItem", "error: failed to parse Stmt");  // �������ʧ��
            return nullptr;
        }

        printParsingAction("BlockItem", "reduce Stmt");  // �ɹ��������
        return statement;
    }

    printParsingAction("BlockItem", "exit");  // �˳� BlockItem ����
}


//17. stmt ->lVal '=' exp ';'| (exp) ? ';'| block| 'return' (exp) ? ';';
std::shared_ptr<Stmt> Parser::parseStmt() {
    printParsingAction("Stmt", "enter");  // ��ʼ���� Stmt
    Token rollbackToken = currentToken; // ���浱ǰ��λ���Ա�ع�

    if (currentToken.type == TokenType::SE_LBRACE) {
        printParsingAction("Stmt", "start parse Block");  // ��ʼ���������

        // ���������
        auto block = parseBlock(); // ���� parseBlock() ���� std::shared_ptr<BlockStmt>
        if (!block) {
            printParsingAction("Stmt", "error: failed to parse Block");  // ���������ʧ��
            currentToken = rollbackToken; // �ع�
            return nullptr;
        }
        printParsingAction("Stmt", "reduce Block");  // �ɹ����������
        return std::static_pointer_cast<Stmt>(block); // �����Ҫ�Ļ�����������ת��
    }
    else if (currentToken.type == TokenType::IDN) {
        printParsingAction("Stmt", "start parse AssignStmt");  // ��ʼ������ֵ���

        // ������ֵ���
        auto lval = parseLVal();
        if (!lval) {
            printParsingAction("Stmt", "error: failed to parse LVal");  // ������ֵʧ��
            return nullptr;
        }
        if (currentToken.type != TokenType::OP_ASSIGN) {
            printParsingAction("Stmt", "error: AssignStmt lacks '='");  // ��ֵ���ȱ�� '='
            syntaxError("��ֵ���ȱ�� '='");
            return nullptr;
        }
        advance(); // ���� '='
        auto expr = parseExp(); // ������ֵ���ʽ
        if (!expr) {
            printParsingAction("Stmt", "error: failed to parse Exp");  // ������ֵʧ��
            return nullptr;
        }
        if (currentToken.type != TokenType::SE_SEMICOLON) {
            printParsingAction("Stmt", "error: AssignStmt lacks ';'");  // ��ֵ���ȱ�� ';'
            syntaxError("��ֵ���ȱ�� ';'");
            return nullptr;
        }
        advance(); // ���� ';'
        printParsingAction("Stmt", "reduce AssignStmt");  // �ɹ�������ֵ���
        return std::make_shared<AssignStmt>(lval, expr); // ���ظ�ֵ���
    }
    else if (currentToken.type == TokenType::KW_RETURN) {
        printParsingAction("Stmt", "start parse ReturnStmt");  // ��ʼ�����������

        // �����������
        advance(); // ���� 'return'
        std::shared_ptr<Exp> expr = nullptr;
        if (currentToken.type != TokenType::SE_SEMICOLON) {
            expr = parseExp();
            if (!expr) {
                printParsingAction("Stmt", "error: failed to parse Exp for ReturnStmt");  // ��������ֵʧ��
                return nullptr;
            }
        }
        if (currentToken.type != TokenType::SE_SEMICOLON) {
            printParsingAction("Stmt", "error: ReturnStmt lacks ';'");  // �������ȱ�� ';'
            syntaxError("�������ȱ�� ';'");
            return nullptr;
        }
        advance(); // ���� ';'
        printParsingAction("Stmt", "reduce ReturnStmt");  // �ɹ������������
        return std::make_shared<ReturnStmt>(expr); // ���ط������
    }
    else {
        printParsingAction("Stmt", "start parse ExprStmt");  // ��ʼ�������ʽ��������

        // �������ʽ��������
        std::shared_ptr<Exp> expr = nullptr;
        if (currentToken.type != TokenType::SE_SEMICOLON) {
            expr = parseExp();
            if (!expr) {
                printParsingAction("Stmt", "error: failed to parse Exp for ExprStmt");  // �������ʽʧ��
                return nullptr;
            }
        }
        if (currentToken.type != TokenType::SE_SEMICOLON) {
            printParsingAction("Stmt", "error: ExprStmt lacks ';'");  // ���ʽ���ȱ�� ';'
            syntaxError("���ʽ���ȱ�� ';'");
            return nullptr;
        }
        advance(); // ���� ';'
        printParsingAction("Stmt", "reduce ExprStmt");  // �ɹ��������ʽ��������
        return std::make_shared<ExprStmt>(expr);
    }
    printParsingAction("Stmt", "error: unrecognized Stmt type");  // �޷�ʶ����������
    currentToken = rollbackToken; // �ع�
    return nullptr;
}

//18. exp -> addExp;
std::shared_ptr<Exp>Parser::parseExp() {
    printParsingAction("Exp", "enter");  // ��ʼ���� Exp

    auto addExpression = parseAddExp();
    if (!addExpression) {
        printParsingAction("Exp", "error: failed to parse AddExp");  // ���� AddExp ʧ��
        return nullptr;
    }
    printParsingAction("Exp", "reduce AddExp"); // �ɹ������� AddExp

    return std::make_shared<Exp>(addExpression);

    printParsingAction("Exp", "exit");  // ��� Exp ����
}
//19. cond -> lOrExp;
std::shared_ptr<Cond> Parser::parseCond() {
    printParsingAction("Cond", "enter");  // ��ʼ���� Cond

    // Parse the logical OR expression
    auto lOrExp = parseLOrExp();
    if (!lOrExp) {
        // If parsing the logical OR expression fails, return nullptr
        printParsingAction("Cond", "error: failed to parse LOrExp");  // ���� LOrExp ʧ��
        return nullptr;
    }

    printParsingAction("Cond", "reduce LOrExp");  // �ɹ������� LOrExp

    // If successful, wrap it in a Cond object and return it
    auto result = std::make_shared<Cond>(lOrExp);

    printParsingAction("Cond", "exit");  // ��� Cond ����

    return result;
}

//20. lVal -> Ident;
std::shared_ptr<LVal> Parser::parseLVal() {
    printParsingAction("LVal", "enter");  // ��ʼ���� LVal
    Token rollbackToken = currentToken; // ���浱ǰ��λ���Ա�ع�

    // ��鵱ǰ token �Ƿ��Ǳ�ʶ��
    if (currentToken.type != TokenType::IDN) {
        // �����ǰ token ���� IDN ���ͣ��򱨸��﷨����
        syntaxError("Expected identifier for lVal, found: " + currentToken.lexeme);
        printParsingAction("LVal", "error: token is not an identifier");  // ��ǰ token ���Ǳ�ʶ��
        currentToken = rollbackToken; // �ع�����ʼλ��
        return nullptr; // ���� nullptr ��ʾ����ʧ��
    }

    // �����ʶ��������
    std::string name = currentToken.lexeme;
    printParsingAction("LVal", "got identifier: " + name);  // ��ȡ����ʶ������

    // �ƶ�����һ�� token
    advance();
    printParsingAction("LVal", "advance to next token"); // �ƶ�����һ�� token

    // ���� LVal ��ʵ��������
    auto result = std::make_shared<LVal>(name);
    printParsingAction("LVal", "reduce LVal");  // �ɹ����� LVal ʵ��

    return result;
}

//21. primaryExp ->'(' exp ')'| lVal| number;
std::shared_ptr<PrimaryExp> Parser::parsePrimaryExp() {
    printParsingAction("PrimaryExp", "enter");
    Token rollbackToken = currentToken; // ���浱ǰ��λ���Ա�ع�

    if (currentToken.type == TokenType::SE_LPAREN) {
        printParsingAction("PrimaryExp", "parsing '(' expression ')'");
        advance(); // ���� '('

        auto expr = parseExp();
        if (!expr) {
            currentToken = rollbackToken; // �ع�����ʼλ��
            return nullptr; // ����ʧ��
        }

        if (currentToken.type != TokenType::SE_RPAREN) {
            syntaxError("Expected ')' after expression");
            currentToken = rollbackToken; // �ع�����ʼλ��
            return nullptr; // ����ʧ��
        }
        advance(); // ���� ')'

        printParsingAction("PrimaryExp", "reduce to expression");
        return std::make_shared<PrimaryExp>(PrimaryExp::ExpressionType::EXPRESSION, expr);
    }
    else if (currentToken.type == TokenType::IDN) {
        printParsingAction("PrimaryExp", "parsing LVal");

        auto lval = parseLVal();
        if (!lval) {
            currentToken = rollbackToken; // �ع�����ʼλ��
            return nullptr; // ����ʧ��
        }

        printParsingAction("PrimaryExp", "reduce to LVal");
        return std::make_shared<PrimaryExp>(PrimaryExp::ExpressionType::LVAL, lval);
    }
    else if (currentToken.type == TokenType::INT) {
        printParsingAction("PrimaryExp", "parsing number");

        auto num = parseNumber();
        if (!num) {
            currentToken = rollbackToken; // �ع�����ʼλ��
            return nullptr; // ����ʧ��
        }

        printParsingAction("PrimaryExp", "reduce to number");
        return std::make_shared<PrimaryExp>(PrimaryExp::ExpressionType::NUMBER, num);
    }
    else {
        syntaxError("Expected primary expression");
        printParsingAction("PrimaryExp", "error: no match for primary expression");
        currentToken = rollbackToken; // �ع�����ʼλ��
        return nullptr; // ��ǰ token ��ƥ���κ� primaryExp ���ӹ���
    }
}

//22. number -> IntConst ;
std::shared_ptr<Number> Parser::parseNumber() {
    printParsingAction("Number", "enter");  // ��ʼ���� Number
    Token rollbackToken = currentToken; // ���浱ǰ��λ���Ա�ع�

    // ��鵱ǰ token �Ƿ�Ϊ INT ���͵��ַ�
    if (currentToken.type == TokenType::INT) {
        int value;
        try {
            value = std::stoi(currentToken.lexeme); // ��������ֵ
            printParsingAction("Number", "parsed integer: " + currentToken.lexeme);  // ���������ɹ�
        }
        catch (const std::invalid_argument& e) {
            syntaxError("Invalid integer: " + currentToken.lexeme);
            printParsingAction("Number", "error: invalid integer format");  // ��Ч��������ʽ
            currentToken = rollbackToken; // �ع�����ʼλ��
            return nullptr;
        }
        catch (const std::out_of_range& e) {
            syntaxError("Integer out of range: " + currentToken.lexeme);
            printParsingAction("Number", "error: integer out of range");  // ����������Χ
            currentToken = rollbackToken; // �ع�����ʼλ��
            return nullptr;
        }

        advance(); // �ƽ�����һ�� token
        printParsingAction("Number", "advance to next token");  // �ƽ�����һ�� token

        auto number = std::make_shared<Number>(value);
        printParsingAction("Number", "reduce to number");  // �ɹ����� Number ʵ��
        return number;
    }
    else {
        syntaxError("Expected integer constant, found: " + currentToken.lexeme);
        printParsingAction("Number", "error: expected integer, found other");  // Ԥ��������������������������
        currentToken = rollbackToken; // �ع�����ʼλ��
        return nullptr; // �����ǰ token ���� INT ���ͣ��򷵻� nullptr
    }
}


//23. unaryExp ->primaryExp| Ident '(' (funcRParams) ? ')'| unaryOp unaryExp;
std::shared_ptr<UnaryExp> Parser::parseUnaryExp() {
    printParsingAction("UnaryExp", "enter");  // ��ʼ���� UnaryExp

    if (currentToken.type == TokenType::IDN) {
        std::string functionName = currentToken.lexeme;
        printParsingAction("UnaryExp", "function name: " + functionName);
        advance(); // ���ѱ�ʶ��

        if (currentToken.type == TokenType::SE_LPAREN) {
            printParsingAction("UnaryExp", "parsing function call");
            advance(); // ���� '('

            std::shared_ptr<FuncRParams> frp = nullptr;
            if (currentToken.type != TokenType::SE_RPAREN) {
                frp = parseFuncRParams();
                printParsingAction("UnaryExp", "parsing function parameters");
                if (!frp) {
                    return nullptr;
                }
            }

            if (currentToken.type != TokenType::SE_RPAREN) {
                syntaxError("Expected ')' after function call parameters");
                return nullptr;
            }
            advance(); // ���� ')'
            auto funcExp = std::make_shared<UnaryExp>();
            funcExp->type = UnaryExp::Type::FunctionCall;
            funcExp->ident = functionName;
            funcExp->funcRParams = frp;
            printParsingAction("UnaryExp", "reduce to function call");
            return funcExp;
        }
        else {
            index--; // ����
            currentToken = tokens.at(index); // ���� currentToken
            printParsingAction("UnaryExp", "not a function call, backtracking");
        }
    }

    if (currentToken.type == TokenType::OP_PLUS ||
        currentToken.type == TokenType::OP_MINUS) {
        printParsingAction("UnaryExp", "parsing unary operation");
        auto unaryOp = parseUnaryOp(); // ����һԪ������
        auto unaryExp = parseUnaryExp(); // ���������� unaryExp
        if (!unaryOp || !unaryExp) {
            return nullptr;
        }
        auto unaryExpNode = std::make_shared<UnaryExp>();
        unaryExpNode->type = UnaryExp::Type::UnaryOperation;
        unaryExpNode->unaryOp = unaryOp;
        unaryExpNode->operand = unaryExp;
        printParsingAction("UnaryExp", "reduce to unary operation");
        return unaryExpNode;
    }

    printParsingAction("UnaryExp", "parsing primary expression");
    auto primary = parsePrimaryExp();
    if (!primary) {
        return nullptr;
    }
    auto primaryExpNode = std::make_shared<UnaryExp>();
    primaryExpNode->type = UnaryExp::Type::PrimaryExp;
    primaryExpNode->primaryExp = primary;
    printParsingAction("UnaryExp", "reduce to primary expression");
    return primaryExpNode;
}
//24. unaryOp -> '+' | '-' | '!';
std::shared_ptr<UnaryOp> Parser::parseUnaryOp() {
    printParsingAction("UnaryOp", "enter");  // ��ʼ���� UnaryOp
    Token rollbackToken = currentToken; // ���浱ǰ��λ���Ա�ع�
    char op;
    switch (currentToken.type) {
    case TokenType::OP_PLUS:
        op = '+'; // '+' һԪ�����
        printParsingAction("UnaryOp", "found '+' operator");
        break;
    case TokenType::OP_MINUS:
        op = '-'; // '-' һԪ�����
        printParsingAction("UnaryOp", "found '-' operator");
        break;
    default:
        printParsingAction("UnaryOp", "unexpected operator, exiting");
        syntaxError("Expected unary operator '+', '-' or '!', found: " + currentToken.lexeme);
        currentToken = rollbackToken; // �ع�����ʼλ��
        return nullptr; // �����ǰ token ����һԪ����������� nullptr
    }
    advance(); // �ƽ�����һ�� token
    printParsingAction("UnaryOp", "advanced to next token");

    // ���� UnaryOp ��ʵ��������
    printParsingAction("UnaryOp", "reduce");
    return std::make_shared<UnaryOp>(op);
}

//25. funcRParams -> funcRParam (',' funcRParam)*;
std::shared_ptr<FuncRParams> Parser::parseFuncRParams() {
    printParsingAction("FuncRParams", "enter");  // ��ʼ���� FuncRParams

    auto paramsNode = std::make_shared<FuncRParams>();

    auto firstParam = parseFuncRParam();
    if (!firstParam) {
        printParsingAction("FuncRParams", "parsing first parameter failed");
        return nullptr;
    }

    printParsingAction("FuncRParams", "first parameter parsed");
    paramsNode->addParam(firstParam);

    while (currentToken.type == TokenType::SE_COMMA) {
        printParsingAction("FuncRParams", "found comma, expecting another parameter");
        Token rollbackToken = currentToken; // ���浱ǰ�� token �Ա�ع�

        advance();  // ���Ѷ��� token

        auto param = parseFuncRParam();
        if (!param) {
            printParsingAction("FuncRParams", "parsing next parameter failed");
            currentToken = rollbackToken; // �ع���֮ǰ����� token
            break;
        }

        printParsingAction("FuncRParams", "next parameter parsed");
        paramsNode->addParam(param);
    }

    printParsingAction("FuncRParams", "all parameters parsed");  // ���������в���
    return paramsNode;
}
//26. funcRParam -> exp;
std::shared_ptr<FuncRParam> Parser::parseFuncRParam() {
    printParsingAction("FuncRParam", "enter");  // ��ʼ���� FuncRParam

    auto expNode = parseExp();
    if (!expNode) {
        printParsingAction("FuncRParam", "parsing expression failed, exiting");  // �������ʽʧ��
        return nullptr;
    }

    printParsingAction("FuncRParam", "expression parsed, creating FuncRParam instance");  // �������ʽ�ɹ������� FuncRParam ʵ��

    auto funcRParam = std::make_shared<FuncRParam>(expNode);
    printParsingAction("FuncRParam", "exit");  // ���� FuncRParam ����

    return funcRParam;
}

//27. mulExp ->unaryExp| mulExp('*' | '/' | '%') unaryExp;
std::shared_ptr<MulExp> Parser::parseMulExp() {
    printParsingAction("MulExp", "enter");  // ��ʼ���� MulExp

    auto right = parseUnaryExp();
    if (!right) {
        printParsingAction("MulExp", "parsing initial UnaryExp failed, exiting");  // ���� UnaryExp ʧ��
        return nullptr;
    }

    auto result = std::make_shared<MulExp>(right);
    printParsingAction("MulExp", "initial UnaryExp parsed");  // �����˳�ʼ�� UnaryExp

    while (currentToken.type == TokenType::OP_MULT ||
        currentToken.type == TokenType::OP_DIV ||
        currentToken.type == TokenType::OP_MOD) {
        char op;
        if (currentToken.type == TokenType::OP_MULT) {
            op = '*';
            printParsingAction("MulExp", "found '*' operator");
        }
        else if (currentToken.type == TokenType::OP_DIV) {
            op = '/';
            printParsingAction("MulExp", "found '/' operator");
        }
        else if (currentToken.type == TokenType::OP_MOD) {
            op = '%';
            printParsingAction("MulExp", "found '%' operator");
        }

        advance(); // �ƽ���������

        auto nextRight = parseUnaryExp();
        if (!nextRight) {
            printParsingAction("MulExp", "parsing next UnaryExp failed, exiting");  // ������һ�� UnaryExp ʧ��
            return nullptr;
        }

        result = std::make_shared<MulExp>(result, op, nextRight);
        printParsingAction("MulExp", "new UnaryExp parsed and added");  // �������µ� UnaryExp �����
    }

    printParsingAction("MulExp", "exit");  // �������� MulExp
    return result;
}


//28. addExp -> mulExp # add1 | addExp ('+' | '-') mulExp;
std::shared_ptr<AddExp> Parser::parseAddExp() {
    printParsingAction("AddExp", "enter");

    auto right = parseMulExp();
    if (!right) {
        printParsingAction("AddExp", "parsing MulExp failed, exiting");
        return nullptr;
    }

    auto result = std::make_shared<AddExp>(right);
    printParsingAction("AddExp", "MulExp parsed successfully, creating base AddExp");

    while (currentToken.type == TokenType::OP_PLUS || currentToken.type == TokenType::OP_MINUS) {
        char op = (currentToken.type == TokenType::OP_PLUS) ? '+' : '-';
        printParsingAction("AddExp", std::string("found '") + op + "' operator");

        // ���浱ǰ������Ϊ�ع��㣬����֮����ܵĻع�
        size_t rollbackPosition = index;

        advance();

        auto nextRight = parseMulExp();
        if (!nextRight) {
            printParsingAction("AddExp", "parsing next MulExp failed, rolling back and exiting");

            // �������ʧ�ܣ��ع���֮ǰ���������λ��
            rollback(rollbackPosition);

            // ����ѡ�񷵻� nullptr ���ǲ��ع������ݾ���Ľ������Ժ�Ԥ����Ϊ����
            return nullptr; // ���ߣ�������ѡ�� break; ����������������
        }

        result = std::make_shared<AddExp>(result, op, nextRight);
        printParsingAction("AddExp", "MulExp combined with current result using operator");
    }

    printParsingAction("AddExp", "exit");
    return result;
}

//29. relExp ->addExp| relExp('<' | '>' | '<=' | '>=') addExp;
std::shared_ptr<RelExp> Parser::parseRelExp() {
    printParsingAction("RelExp", "enter");

    auto firstExpr = parseAddExp();
    if (!firstExpr) {
        printParsingAction("RelExp", "parsing AddExp failed, exiting");
        return nullptr;
    }

    size_t rollbackPosition = index; // ���浱ǰ�����Ա���ܵĻع�

    // ��鵱ǰ token �Ƿ��ǹ�ϵ������
    std::string opStr;
    switch (currentToken.type) {
    case TokenType::OP_LT:
        opStr = "<";
        printParsingAction("RelExp", "found '<' operator");
        break;
    case TokenType::OP_GT:
        opStr = ">";
        printParsingAction("RelExp", "found '>' operator");
        break;
    case TokenType::OP_LE:
        opStr = "<=";
        printParsingAction("RelExp", "found '<=' operator");
        break;
    case TokenType::OP_GE:
        opStr = ">=";
        printParsingAction("RelExp", "found '>=' operator");
        break;
    default:
        printParsingAction("RelExp", "no relational operator, treated as AddExp, exit");
        return std::make_shared<RelExp>(firstExpr);
    }

    // �ƹ���ϵ������
    advance();

    auto secondExpr = parseAddExp();
    if (!secondExpr) {
        printParsingAction("RelExp", "parsing second AddExp failed, rolling back and exiting");
        rollback(rollbackPosition); // �ع���ȷ�ϲ�����֮ǰ������λ��
        return std::make_shared<RelExp>(firstExpr); // ѡ�񷵻� firstExpr ��Ϊ�ع���Ĵ����ѡ����������ʽ
    }

    auto relExp = std::make_shared<RelExp>(firstExpr, opStr, secondExpr);
    printParsingAction("RelExp", "RelExp constructed, exiting");

    return relExp;
}
//30. eqExp ->relExp| eqExp('==' | '!=') relExp;
std::shared_ptr<EqExp> Parser::parseEqExp() {
    printParsingAction("EqExp", "enter");

    auto leftRelExp = parseRelExp();
    if (!leftRelExp) {
        printParsingAction("EqExp", "Expected RelExp at the beginning of EqExp, exiting");
        return nullptr;
    }

    auto node = std::make_shared<EqExp>(leftRelExp);

    while (currentToken.type == TokenType::OP_EQ || currentToken.type == TokenType::OP_NE) {
        std::string op;

        if (currentToken.type == TokenType::OP_EQ) {
            op = "==";
            printParsingAction("EqExp", "found '==' operator");
        }
        else {
            op = "!=";
            printParsingAction("EqExp", "found '!=' operator");
        }

        // Save current position to rollback if next parse fails
        size_t rollbackPosition = index;

        advance();

        auto rightRelExp = parseRelExp();
        if (!rightRelExp) {
            // Parsing failed, rollback to previous state
            printParsingAction("EqExp", "Expected RelExp after '" + op + "', parsing failed, rolling back");
            rollback(rollbackPosition);

            // Here we either exit the loop or handle the failure differently
            // For simplicity, let's break the loop and return the present node without including the failed part
            break;
        }

        node = std::make_shared<EqExp>(node, op, rightRelExp);
        printParsingAction("EqExp", "new EqExp node created with operator '" + op + "'");
    }

    printParsingAction("EqExp", "exit");
    return node;
}

//31. lAndExp ->eqExp| lAndExp '&&' eqExp;
std::shared_ptr<LAndExp> Parser::parseLAndExp() {
    printParsingAction("LAndExp", "enter");

    auto firstEqExp = parseEqExp();
    if (!firstEqExp) {
        printParsingAction("LAndExp", "Expected EqExp at the beginning of LAndExp, exiting");
        return nullptr;
    }

    auto node = std::make_shared<LAndExp>(firstEqExp);
    printParsingAction("LAndExp", "First EqExp parsed successfully");

    while (currentToken.type == TokenType::OP_AND) {
        printParsingAction("LAndExp", "found '&&' operator");

        // Save the current position to rollback if next parse fails
        size_t rollbackPosition = index;

        advance();

        auto rightEqExp = parseEqExp();
        if (!rightEqExp) {
            // Parsing failed, rollback to previous state
            printParsingAction("LAndExp", "Expected EqExp after '&&', parsing failed, rolling back");
            rollback(rollbackPosition);

            // Here we either exit the loop or handle the failure differently
            // For simplicity, let's break the loop and return the present node without including the failed part
            break;
        }

        node = std::make_shared<LAndExp>(node, rightEqExp);
        printParsingAction("LAndExp", "new LAndExp node created with '&&'");

        if (index >= tokens.size()) break;
        currentToken = tokens.at(index);
    }

    printParsingAction("LAndExp", "exit");
    return node;
}
//32. lOrExp ->lAndExp| lOrExp '||' lAndExp;
std::shared_ptr<LOrExp> Parser::parseLOrExp() {
    printParsingAction("LOrExp", "enter");  // ��ʼ���� LOrExp

    std::shared_ptr<LOrExp> leftOrExp = nullptr;
    std::shared_ptr<LAndExp> leftLAndExp = parseLAndExp();

    if (leftLAndExp == nullptr) {
        printParsingAction("LOrExp", "Expected LAndExp at the beginning of LOrExp, exiting");  // LAndExp ����ʧ��
        return nullptr;
    }
    else {
        printParsingAction("LOrExp", "First LAndExp parsed successfully");  // �ɹ������˵�һ�� LAndExp
    }

    while (currentToken.type == TokenType::OP_OR) {
        printParsingAction("LOrExp", "found '||' operator");  // ���� '||' ������

        advance(); // �ƽ��� '||' ֮��� token

        auto right = parseLAndExp();
        if (!right) {
            printParsingAction("LOrExp", "Expected LAndExp after '||', exiting");  // ���� LAndExp ʧ��
            return nullptr;
        }

        printParsingAction("LOrExp", "Parsed another LAndExp after '||'");  // �ɹ������� '||' ��� LAndExp

        if (leftOrExp == nullptr) {
            leftOrExp = std::make_shared<LOrExp>(leftLAndExp);
        }

        leftOrExp = std::make_shared<LOrExp>(leftOrExp, right);

        printParsingAction("LOrExp", "Updated LOrExp with new LAndExp on the right");  // ʹ���µ� LAndExp ������ LOrExp

        if (index >= tokens.size()) break;
        currentToken = tokens.at(index);
    }

    if (leftOrExp == nullptr) {
        printParsingAction("LOrExp", "No '||' encountered, wrapping single LAndExp, exit");  // û������ '||'����������
        return std::make_shared<LOrExp>(leftLAndExp);
    }

    printParsingAction("LOrExp", "exit");  // �������� LOrExp

    return leftOrExp;
}
//33. constExp -> addExp;
std::shared_ptr<ConstExp> Parser::parseConstExp() {
    printParsingAction("ConstExp", "enter");  // ��ʼ���� ConstExp

    std::shared_ptr<AddExp> addExp = parseAddExp();
    if (addExp != nullptr) {
        printParsingAction("ConstExp", "AddExp parsed successfully, creating ConstExp");  // �ɹ����� AddExp ������ ConstExp
        return std::make_shared<ConstExp>(addExp);
    }

    printParsingAction("ConstExp", "parsing AddExp failed, exiting");  // ���� AddExp ʧ�ܣ��˳�
    return nullptr; // If parseAddExp did not succeed
}
//34. IntConst -> [0-9]+ ;
std::shared_ptr<IntConst> Parser::parseIntConst() {
    printParsingAction("IntConst", "enter");  // ��ʼ���� IntConst

    if (currentToken.type != TokenType::INT) {
        printParsingAction("IntConst", "Syntax error, expected integer constant, found: " + currentToken.lexeme);
        return nullptr;
    }

    int intValue;
    try {
        intValue = std::stoi(currentToken.lexeme);
        printParsingAction("IntConst", "Integer constant parsed successfully");
    }
    catch (const std::invalid_argument& e) {
        printParsingAction("IntConst", "Invalid integer constant: " + currentToken.lexeme);
        return nullptr;
    }
    catch (const std::out_of_range& e) {
        printParsingAction("IntConst", "Integer constant out of range: " + currentToken.lexeme);
        return nullptr;
    }

    advance();  // �ƽ�����һ�� token

    // ���� IntConst ��ʵ��������
    printParsingAction("IntConst", "IntConst created, exit");  // ���� IntConst �ɹ����˳�����
    return std::make_shared<IntConst>(intValue);
}
//35. Ident -> [a-zA-Z_][a-zA-Z_0-9]*;
std::shared_ptr<Ident> Parser::parseIdent() {
    printParsingAction("Ident", "enter");  // ��ʼ���� Ident

    if (currentToken.type != TokenType::IDN) {
        printParsingAction("Ident", "Syntax error, expected identifier, found: " + currentToken.lexeme);
        return nullptr;
    }

    // �����ʶ��������
    std::string identName = currentToken.lexeme;
    printParsingAction("Ident", "Identifier found: " + identName);  // �ҵ���ʶ��

    advance(); // �ƽ�����һ�� token

    // ���� Ident ��ʵ��������
    printParsingAction("Ident", "Ident created, exit");  // ���� Ident �ɹ����˳�����
    return std::make_shared<Ident>(identName);
}