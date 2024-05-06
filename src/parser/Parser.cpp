#include "Parser.h"
bool Parser::advance() {
    // 确保索引不会超出tokens向量的长度
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
    // 这里可以添加错误处理的代码，比如记录错误位置、错误信息等
    std::cerr << "Syntax Error: " << message << std::endl;
    // 选择抛出异常来终止解析过程
    throw std::runtime_error(message);
}
//1. Program -> compUnit
std::shared_ptr<Program> Parser::parseProgram() {
    printParsingAction("Program", "enter");  // 进入 Program 规则
    auto program = std::make_shared<Program>();


    // 循环解析直到文件结束
    while (currentToken.type != TokenType::EOF_TOKEN) {
        try {
            // 解析compUnit
            auto compUnit = parseCompUnit();
            if (compUnit) {
                // 添加到program的compUnits中
                program->compUnits.push_back(compUnit);
            }
        }
        catch (const std::exception& e) {
            // 接收到异常，报告语法错误
            syntaxError(e.what());
            printParsingAction("Program", "error");  // 发生错误
            // 假设这里我们选择终止解析
            break;
        }
    }
    printParsingAction("Program", "exit");  // 退出 Program 规则
    return program;
}


//2. compUnit -> (decl | funcDef)* EOF;
std::shared_ptr<CompUnit> Parser::parseCompUnit() {
    printParsingAction("CompUnit", "enter");  // 进入 CompUnit 规则
    auto compUnit = std::make_shared<CompUnit>();
   
    while (currentToken.type != TokenType::EOF_TOKEN) {
        printParsingAction("CompUnit", "attempt");  // 尝试识别声明或函数定义
        std::shared_ptr<ASTNode> node;
        
        // 首先尝试解析声明（declaration）
        node = parseDeclaration();
        if (node) {
            printParsingAction("CompUnit", "reduction: decl");  // 规约声明
        }
        else {
            // 如果解析声明失败，则尝试解析函数定义（function definition）
            node = parseFuncDef();
            if (node) {
                printParsingAction("CompUnit", "reduction: funcDef");  // 规约函数定义
            }
        }
        // 如果两者都无法解析，报告语法错误
        if (!node) {
            syntaxError("Unexpected token when expecting a declaration or a function definition.");
            printParsingAction("CompUnit", "error");  // 错误
            return nullptr;
        }

        // 将解析成功的节点添加到 compUnit 中
        compUnit->addNode(node);

        // 继续前进到下一个 Token
        if (currentToken.type != TokenType::EOF_TOKEN) {
            printParsingAction("CompUnit", "advance");  // 移进
        }
    }

    printParsingAction("CompUnit", "exit");  // 退出 CompUnit 规则
    return compUnit; // 返回构建好的 CompUnit
}

//3. decl -> constDecl | varDecl;
// 解析声明的函数，尝试解析为常量声明或变量声明。
std::shared_ptr<Declaration> Parser::parseDeclaration() {
    printParsingAction("decl", "enter");  // 开始解析声明

    auto saveIndex = index; // 保存当前位置
    auto saveToken = currentToken; // 保存当前token
    std::shared_ptr<Declaration> node = nullptr;

    // 尝试解析 const 声明
    node = parseConstDecl();
    if (node) {
        printParsingAction("decl", "reduction: constDecl");  // 成功解析 const 声明
        return node;
    }
    else {
        printParsingAction("decl", "backtrack: constDecl failure");  // const 声明解析失败，回溯
    }

    // 回退到先前保存的位置和token
    index = saveIndex;
    currentToken = saveToken;

    // 尝试解析 var 声明
    node = parseVarDecl();
    if (node) {
        printParsingAction("decl", "reduction: varDecl");  // 成功解析 var 声明
        return node;
    }

    // 如果两者都无法解析，报告语法错误
    printParsingAction("decl", "error");  // 发生错误
    //syntaxError("Unexpected token when expecting a declaration.");

    printParsingAction("decl", "exit");  // 退出解析声明
    return nullptr;
}


//4. constDecl -> 'const' bType constDef (',' constDef)* ';';
std::shared_ptr<ConstantDeclaration> Parser::parseConstDecl() {
    size_t startIndex = index; // 保存当前token位置

    printParsingAction("constDecl", "enter"); // 进入常量声明规则

    if (!match(TokenType::KW_CONST)) {
        rollback(startIndex); // 回滚到函数开始的位置
        return nullptr; // 如果没有匹配到 'const'，则返回空
    }
    printParsingAction("constDecl", "match KW_CONST");

    auto basicTypePtr = parseBasicType(); // 解析基本类型 bType
    if (!basicTypePtr) {
        rollback(startIndex); // 回滚
        printParsingAction("constDecl", "error: failed to parse bType");
        return nullptr;
    }
    printParsingAction("constDecl", "reduction: bType");

    BasicType basicType = *basicTypePtr; // 获取 BasicType 实例
    std::vector<std::shared_ptr<ConstDef>> constDefs;

    auto constDef = parseConstDef(); // 解析第一个 constDef
    if (!constDef) {
        rollback(startIndex); // 回滚
        printParsingAction("constDecl", "error: failed to parse constDef");
        return nullptr;
    }
    constDefs.push_back(constDef);
    printParsingAction("constDecl", "reduction: constDef");

    while (currentToken.type == TokenType::SE_COMMA) {
        advance(); // 吞掉逗号 ','
        printParsingAction("constDecl", "advance after comma");

        constDef = parseConstDef(); // 解析后续的 constDef
        if (!constDef) {
            rollback(startIndex); // 回滚
            printParsingAction("constDecl", "error: failed to parse subsequent constDef");
            return nullptr;
        }
        constDefs.push_back(constDef);
        printParsingAction("constDecl", "reduction: constDef");
    }

    if (!match(TokenType::SE_SEMICOLON)) { // 确认分号结束 ';'
        rollback(startIndex); // 回滚
        return nullptr; // 如果没有匹配到分号，返回空
    }
    printParsingAction("constDecl", "match SE_SEMICOLON");

    printParsingAction("constDecl", "exit"); // 退出常量声明规则
    // 创建并返回常量声明实例
    return std::make_shared<ConstantDeclaration>(basicType, constDefs);
}


//5. bType -> 'int';
std::shared_ptr<BasicType> Parser::parseBasicType() {
    printParsingAction("BasicType", "enter"); // 进入基本类型规则

    if (currentToken.type == TokenType::KW_INT) {
        printParsingAction("BasicType", "match KW_INT"); // 匹配到 'int' 关键词

        auto basicType = std::make_shared<BasicType>(BasicType::INT);
        advance(); // 推进到下一个 token

        printParsingAction("BasicType", "exit"); // 退出基本类型规则
        return basicType;
    }
    else {
        printParsingAction("BasicType", "error: expected 'int'"); // 发生错误，期望 'int' 关键词
       
        return nullptr; // 错误路径，解析失败
    }
}

//6. constDef -> Ident '=' constInitVal;
std::shared_ptr<ConstDef> Parser::parseConstDef() {
    size_t startIndex = index; // 保存函数开始的位置

    printParsingAction("ConstDef", "enter"); // 进入常量定义规则

    if (currentToken.type != TokenType::IDN) {
        printParsingAction("ConstDef", "error: expected identifier"); // 预期为标识符，但未匹配到
        syntaxError("常量定义缺少标识符");
        rollback(startIndex); // 回滚到函数开始的位置
        return nullptr;
    }

    printParsingAction("ConstDef", "match IDN"); // 匹配到标识符
    std::string constName = currentToken.lexeme;
    advance(); // 消费标识符 Token

    if (currentToken.type != TokenType::OP_ASSIGN) {
        printParsingAction("ConstDef", "error: expected '='"); // 预期为等号，但未匹配到
        syntaxError("常量定义中缺少等号 '='");
        rollback(startIndex); // 回滚
        return nullptr;
    }

    printParsingAction("ConstDef", "match '='"); // 匹配到等号
    advance(); // 消费 '=' Token

    auto initVal = parseConstInitVal(); // 解析常量初始值
    if (!initVal) {
        printParsingAction("ConstDef", "error: failed to parse constInitVal"); // 解析常量初始值失败
        rollback(startIndex); // 回滚
        return nullptr;
    }

    printParsingAction("ConstDef", "reduction: constInitVal"); // 规约常量初始值
    printParsingAction("ConstDef", "exit"); // 退出常量定义规则
    return std::make_shared<ConstDef>(constName, initVal->constExp); // 创建并返回 ConstDef 节点
}

//7. constInitVal -> constExp
std::shared_ptr<ConstInitVal> Parser::parseConstInitVal() {
    printParsingAction("ConstInitVal", "enter");  // 进入 ConstInitVal 规则

    auto exp = parseConstExp(); // 解析常量表达式
    if (!exp) {
        printParsingAction("ConstInitVal", "error: failed to parse constExp"); // 解析常量表达式失败
        return nullptr;
    }

    printParsingAction("ConstInitVal", "reduction: constExp"); // 规约常量表达式
    printParsingAction("ConstInitVal", "exit");  // 退出 ConstInitVal 规则
    return std::make_shared<ConstInitVal>(exp); // 创建并返回 ConstInitVal 节点
}

//8.varDecl -> bType varDef (',' varDef)* ';';
std::shared_ptr<VarDecl> Parser::parseVarDecl() {
    size_t startPosition = index; // 记录开始时的位置
    printParsingAction("VarDecl", "enter");  // 进入 VarDecl 规则

    auto type = parseBasicType();  // 解析基本类型
    if (!type) {
        printParsingAction("VarDecl", "error: failed to parse bType");
        rollback(startPosition); // 回滚到初始位置
        return nullptr;  // 如果基本类型解析失败，则返回 nullptr
    }
    printParsingAction("VarDecl", "reduction: bType");

    std::vector<std::shared_ptr<VarDef>> defs;  // 用于存储变量定义的向量
    auto varDef = parseVarDef();  // 解析第一个变量定义
    if (!varDef) {
        printParsingAction("VarDecl", "error: failed to parse first varDef");
        rollback(startPosition); // 回滚到初始位置
        return nullptr;  // 如果变量定义解析失败，则返回 nullptr
    }
    defs.push_back(varDef);  // 将第一个变量定义添加到向量中
    printParsingAction("VarDecl", "reduction: varDef");

    // 解析其余的变量定义，如果有的话
    while (currentToken.type == TokenType::SE_COMMA) {
        advance();  // 消费逗号
        printParsingAction("VarDecl", "advance after comma");

        varDef = parseVarDef();  // 解析下一个变量定义
        if (!varDef) {
            printParsingAction("VarDecl", "error: failed to parse subsequent varDef");
            rollback(startPosition); // 回滚到初始位置
            return nullptr;  // 如果变量定义解析失败，则清除目前收集到的varDef，并返回 nullptr
        }
        defs.push_back(varDef);  // 将解析好的变量定义添加到向量中
        printParsingAction("VarDecl", "reduction: varDef");
    }

    if (currentToken.type != TokenType::SE_SEMICOLON) {
        printParsingAction("VarDecl", "error: missing semicolon");
        syntaxError("变量声明没有以分号 ';' 结束");
        rollback(startPosition); // 回滚到初始位置
        return nullptr;
    }
    printParsingAction("VarDecl", "match SE_SEMICOLON");

    advance();  // 消费分号

    printParsingAction("VarDecl", "exit");  // 退出 VarDecl 规则
    return std::make_shared<VarDecl>(type, defs); // 创建并返回变量声明对象
}

//9. varDef -> Ident | Ident '=' initVal;
std::shared_ptr<VarDef> Parser::parseVarDef() {
    printParsingAction("VarDef", "enter");  // 进入 VarDef 规则

    // 确认当前 token 是标识符
    if (currentToken.type != TokenType::IDN) {
        printParsingAction("VarDef", "error: expected identifier");
        return nullptr;
    }

    printParsingAction("VarDef", "match IDN"); // 匹配到标识符
    std::string identName = currentToken.lexeme; // 获取变量名
    advance(); // 消费标识符

    std::shared_ptr<InitVal> initVal = nullptr; // 创建初始值表达式的指针，默认为 nullptr

    Token rollbackToken = currentToken; // 保存当前的位置以便回滚

    // 如果下一个 token 是等号 '='，那么我们有一个初始值需要解析
    if (currentToken.type == TokenType::OP_ASSIGN) {
        advance(); // 消费 '='
        printParsingAction("VarDef", "match '='"); // 匹配到等号

        initVal = parseInitVal(); // 解析初始化值
        if (!initVal) {
            printParsingAction("VarDef", "error: failed to parse initVal");
            currentToken = rollbackToken; // 回滚到先前的位置
            return nullptr; // 返回 nullptr 表示解析失败
        }
        printParsingAction("VarDef", "reduction: initVal"); // 规约初始化值
    }

    // 到这里，我们要么有一个未初始化的变量声明，要么有一个带有初始化值的变量声明
    // 根据是否有初始值创建对应的 VarDef 节点并返回
    auto varDef = initVal ? std::make_shared<VarDef>(identName, initVal)
        : std::make_shared<VarDef>(identName);
    printParsingAction("VarDef", "exit");  // 退出 VarDef 规则
    return varDef;
}

//10. initVal -> exp;
std::shared_ptr<InitVal> Parser::parseInitVal() {
    printParsingAction("InitVal", "enter");  // 开始解析 InitVal

    // 解析初始化值中的表达式
    auto expression = parseExp();
    if (!expression) {
        printParsingAction("InitVal", "error: failed to parse expression");
        // 如果解析表达式失败，则返回 nullptr
        return nullptr;
    }
    printParsingAction("InitVal", "reduction: expression");  // 解析成功表达式

    // 创建并返回 InitVal 节点
    auto initVal = std::make_shared<InitVal>(expression);
    printParsingAction("InitVal", "exit");  // 解析 InitVal 完毕
    return initVal;
}

//11. funcDef -> funcType Ident '(' (funcFParams)? ')' block;
std::shared_ptr<FuncDef> Parser::parseFuncDef() {
    printParsingAction("FuncDef", "enter");  // 进入 FuncDef 规则
    Token rollbackToken = currentToken; // 保存当前的位置以便回滚

    // 解析函数返回类型
    auto type = parseFuncType();
    if (!type) {
        printParsingAction("FuncDef", "error: failed to parse return type");
        currentToken = rollbackToken; // 回滚到函数定义开始的位置
        return nullptr; // 返回类型解析失败
    }
    printParsingAction("FuncDef", "reduction: return type"); // 解析成功函数返回类型

    // 新增函数名解析支持 KW_MAIN 关键字
    std::string name;
    if (currentToken.type == TokenType::KW_MAIN) { // 支持 KW_MAIN 为函数名
        name = "main"; // 如果是 main 关键字
        printParsingAction("FuncDef", "match function name 'main'");  // 匹配到函数名 'main'
    }
    else if (currentToken.type == TokenType::IDN) { // 支持普通标识符为函数名
        name = currentToken.lexeme; // 如果是普通标识符
        printParsingAction("FuncDef", "match function name");  // 匹配到函数名
    }
    else {
        printParsingAction("FuncDef", "error: expected function name identifier or 'main'");
        syntaxError("缺少函数名称");
        currentToken = rollbackToken; // 回滚
        return nullptr;
    }
    advance(); // 消费函数名标识符或 main 关键字

    // 匹配左括号 '('
    if (currentToken.type != TokenType::SE_LPAREN) {
        printParsingAction("FuncDef", "error: expected '('");
        syntaxError("在函数定义中缺少 '('");
        currentToken = rollbackToken; // 回滚
        return nullptr;
    }
    printParsingAction("FuncDef", "match '('");  // 匹配到左括号
    advance(); // 消费 '('

    // 如果有参数，解析参数列表
    std::shared_ptr<FuncFParams> parameters = nullptr;
    if (currentToken.type != TokenType::SE_RPAREN) {
        parameters = parseFuncFParams();
        if (!parameters) {
            printParsingAction("FuncDef", "error: failed to parse function parameters");
            currentToken = rollbackToken; // 回滚
            return nullptr; // 如果解析参数列表失败，则返回 nullptr
        }
        printParsingAction("FuncDef", "reduction: parameters"); // 解析成功参数列表
    }

    // 匹配右括号 ')'
    if (currentToken.type != TokenType::SE_RPAREN) {
        printParsingAction("FuncDef", "error: expected ')'");
        syntaxError("在函数定义中缺少 ')'");
        currentToken = rollbackToken; // 回滚
        return nullptr;
    }
    printParsingAction("FuncDef", "match ')'");  // 匹配到右括号
    advance(); // 消费 ')'

    // 解析函数体
    auto block = parseBlock();
    if (!block) {
        printParsingAction("FuncDef", "error: failed to parse function body");
        currentToken = rollbackToken; // 回滚
        return nullptr; // 函数体解析失败
    }
    printParsingAction("FuncDef", "reduction: function body"); // 解析成功函数体

    // 构建并返回 FuncDef 对象
    auto funcDef = std::make_shared<FuncDef>(type, name, parameters, block);
    printParsingAction("FuncDef", "exit");  // 离开 FuncDef 规则
    return funcDef;
}

//12. funcType -> 'void' | 'int' ;
std::shared_ptr<FuncType> Parser::parseFuncType() {
    printParsingAction("FuncType", "enter");  // 进入 FuncType 规则
    Token rollbackToken = currentToken; // 保存当前的位置以便回滚

    if (currentToken.type == TokenType::KW_INT) {
        printParsingAction("FuncType", "match 'int'");  // 匹配到 'int'
        advance(); // 推进到下一个 token
        auto funcType = std::make_shared<FuncType>(FuncTypeEnum::Integer);
        printParsingAction("FuncType", "reduction: 'int'"); // 简约为 'int'
        printParsingAction("FuncType", "exit");  // 离开 FuncType 规则
        return funcType;
    }
    else if (currentToken.type == TokenType::KW_VOID) {
        printParsingAction("FuncType", "match 'void'");  // 匹配到 'void'
        advance(); // 推进到下一个 token
        auto funcType = std::make_shared<FuncType>(FuncTypeEnum::Void);
        printParsingAction("FuncType", "reduction: 'void'"); // 简约为 'void'
        printParsingAction("FuncType", "exit");  // 离开 FuncType 规则
        return funcType;
    }
    else {
        // 如果当前 token 不是 'int' 或 'void'，则报告语法错误
        printParsingAction("FuncType", "error: expected 'int' or 'void'");
        syntaxError("Expected 'int' or 'void' in function type, found: " + currentToken.lexeme);
        currentToken = rollbackToken; // 回滚到函数类型解析之前的位置
        return nullptr; // 返回 nullptr 表示解析失败
    }
}

//13. funcFParams -> funcFParam (',' funcFParam)*;
std::shared_ptr<FuncFParams> Parser::parseFuncFParams() {
    printParsingAction("FuncFParams", "enter");  // 开始解析 FuncFParams
    Token rollbackToken = currentToken; // 保存当前的位置以便回滚

    auto funcFParams = std::make_shared<FuncFParams>();

    // Parse the first parameter (at least one parameter must exist for this rule)
    auto param = parseFuncFParam();
    if (!param) {
        printParsingAction("FuncFParams", "error: expected a function parameter");  // 报错信息
        syntaxError("Expected a function parameter");
        currentToken = rollbackToken; // 回滚到函数类型解析之前的位置
        return nullptr;
    }
    funcFParams->addParam(param);

    // Continue parsing parameters if a comma is found
    while (currentToken.type == SE_COMMA) {
        printParsingAction("FuncFParams", "match ','");  // 匹配到 ','
        // Consume the comma
        advance();

        // Parse the next parameter
        param = parseFuncFParam();
        if (!param) {
            printParsingAction("FuncFParams", "error: expected a function parameter after comma");  // 报错信息
            syntaxError("Expected a function parameter after comma");
            currentToken = rollbackToken; // 回滚到函数类型解析之前的位置
            return nullptr;
        }
        funcFParams->addParam(param);
    }

    printParsingAction("FuncFParams", "exit");  // 离开 FuncFParams 规则
    return funcFParams;
}
//14. funcFParam -> bType Ident;
std::shared_ptr<FuncFParam> Parser::parseFuncFParam() {
    printParsingAction("FuncFParam", "enter");  // 开始解析 FuncFParam

    // Parse bType
    auto bType = parseBasicType();
    if (!bType) {
        printParsingAction("FuncFParam", "error: expected a basic type for funcFParam");  // 报错信息
        syntaxError("Expected basic type for funcFParam");
        return nullptr;
    }
    printParsingAction("FuncFParam", "reduction: bType");  // 成功解析基本类型

    // Parse Ident
    auto ident = parseIdent();
    if (!ident) {
        printParsingAction("FuncFParam", "error: expected an identifier for funcFParam");  // 报错信息
        syntaxError("Expected identifier for funcFParam");
        return nullptr;
    }
    printParsingAction("FuncFParam", "match ident");  // 匹配到标识符

    // 使用你在Ident类中已经定义好的name成员变量
    std::string identName = ident->name;

    // Create and return FuncFParam object
    // 我们使用解引用操作符*来解引用bType，以获取BasicType的实例
    // 然后将identName传递给FuncFParam的构造函数
    auto funcFParam = std::make_shared<FuncFParam>(*bType, identName);
    printParsingAction("FuncFParam", "exit");  // 离开 FuncFParam 规则
    return funcFParam;
}
//15. block -> '{' (blockItem)* '}';
std::shared_ptr<Block> Parser::parseBlock() {
    printParsingAction("Block", "enter");  // 开始解析 Block
    Token rollbackToken = currentToken; // 保存当前的位置以便回滚

    // 确认当前 token 是不是左大括号 '{'
    if (currentToken.type != TokenType::SE_LBRACE) {
        printParsingAction("Block", "error: expected '{'");  // 报错信息
        syntaxError("Expected '{' at the beginning of block");
        currentToken = rollbackToken; // 回滚到进入 Block 解析前的位置
        return nullptr;
    }
    printParsingAction("Block", "match '{'");  // 匹配到左大括号
    advance(); // 消费 '{'

    // 创建一个新的 Block 实例
    auto block = std::make_shared<Block>();

    // 循环解析 blockItem，直到遇到右大括号 '}'
    while (currentToken.type != TokenType::EOF_TOKEN) {
        // 如果遇到右大括号，则结束解析
        if (currentToken.type == TokenType::SE_RBRACE) {
            printParsingAction("Block", "match '}'");  // 匹配到右大括号
            advance(); // 消费右大括号 '}'
            break; // 此时应该跳出 while 循环
        }

        // 开始解析 BlockItem
        printParsingAction("Block", "start parse BlockItem");
        auto item = parseBlockItem();
        if (!item) {
            // 解析失败的情况下，parseBlockItem 应该已经设置了错误信息
            currentToken = rollbackToken; // 回滚到进入 Block 解析前的位置
            return nullptr;
        }
        // 成功解析 BlockItem 并添加到 block 中
        printParsingAction("Block", "reduce BlockItem");
        block->addItem(item);

        // 无需调用 advance()，parseBlockItem() 内部处理
    }

    printParsingAction("Block", "exit");  // 离开 Block 规则
    return block; // 返回完成解析的代码块
}

//16. blockItem -> decl | stmt;
std::shared_ptr<BlockItem> Parser::parseBlockItem() {
    printParsingAction("BlockItem", "enter");  // 开始解析 BlockItem

    // 首先，我们要区分当前的 token 是否表示声明语句
    // 声明语句开始于关键字，比如 'int'、'const' 等
    if (currentToken.type == TokenType::KW_INT || currentToken.type == TokenType::KW_CONST) {
        printParsingAction("BlockItem", "start parse Declaration");  // 开始解析声明语句

        auto declaration = parseDeclaration();

        if (!declaration) {
            printParsingAction("BlockItem", "error: failed to parse Declaration");  // 解析声明语句失败
            return nullptr;
        }

        printParsingAction("BlockItem", "reduce Declaration");  // 成功解析声明语句
        return declaration;

    }
    else {
        printParsingAction("BlockItem", "start parse Stmt");  // 开始解析语句
        auto statement = parseStmt();

        if (!statement) {
            printParsingAction("BlockItem", "error: failed to parse Stmt");  // 解析语句失败
            return nullptr;
        }

        printParsingAction("BlockItem", "reduce Stmt");  // 成功解析语句
        return statement;
    }

    printParsingAction("BlockItem", "exit");  // 退出 BlockItem 解析
}


//17. stmt ->lVal '=' exp ';'| (exp) ? ';'| block| 'return' (exp) ? ';';
std::shared_ptr<Stmt> Parser::parseStmt() {
    printParsingAction("Stmt", "enter");  // 开始解析 Stmt
    Token rollbackToken = currentToken; // 保存当前的位置以便回滚

    if (currentToken.type == TokenType::SE_LBRACE) {
        printParsingAction("Stmt", "start parse Block");  // 开始解析代码块

        // 解析代码块
        auto block = parseBlock(); // 假设 parseBlock() 返回 std::shared_ptr<BlockStmt>
        if (!block) {
            printParsingAction("Stmt", "error: failed to parse Block");  // 解析代码块失败
            currentToken = rollbackToken; // 回滚
            return nullptr;
        }
        printParsingAction("Stmt", "reduce Block");  // 成功解析代码块
        return std::static_pointer_cast<Stmt>(block); // 如果需要的话，进行类型转换
    }
    else if (currentToken.type == TokenType::IDN) {
        printParsingAction("Stmt", "start parse AssignStmt");  // 开始解析赋值语句

        // 解析赋值语句
        auto lval = parseLVal();
        if (!lval) {
            printParsingAction("Stmt", "error: failed to parse LVal");  // 解析左值失败
            return nullptr;
        }
        if (currentToken.type != TokenType::OP_ASSIGN) {
            printParsingAction("Stmt", "error: AssignStmt lacks '='");  // 赋值语句缺少 '='
            syntaxError("赋值语句缺少 '='");
            return nullptr;
        }
        advance(); // 消费 '='
        auto expr = parseExp(); // 解析右值表达式
        if (!expr) {
            printParsingAction("Stmt", "error: failed to parse Exp");  // 解析右值失败
            return nullptr;
        }
        if (currentToken.type != TokenType::SE_SEMICOLON) {
            printParsingAction("Stmt", "error: AssignStmt lacks ';'");  // 赋值语句缺少 ';'
            syntaxError("赋值语句缺少 ';'");
            return nullptr;
        }
        advance(); // 消费 ';'
        printParsingAction("Stmt", "reduce AssignStmt");  // 成功解析赋值语句
        return std::make_shared<AssignStmt>(lval, expr); // 返回赋值语句
    }
    else if (currentToken.type == TokenType::KW_RETURN) {
        printParsingAction("Stmt", "start parse ReturnStmt");  // 开始解析返回语句

        // 解析返回语句
        advance(); // 消费 'return'
        std::shared_ptr<Exp> expr = nullptr;
        if (currentToken.type != TokenType::SE_SEMICOLON) {
            expr = parseExp();
            if (!expr) {
                printParsingAction("Stmt", "error: failed to parse Exp for ReturnStmt");  // 解析返回值失败
                return nullptr;
            }
        }
        if (currentToken.type != TokenType::SE_SEMICOLON) {
            printParsingAction("Stmt", "error: ReturnStmt lacks ';'");  // 返回语句缺少 ';'
            syntaxError("返回语句缺少 ';'");
            return nullptr;
        }
        advance(); // 消费 ';'
        printParsingAction("Stmt", "reduce ReturnStmt");  // 成功解析返回语句
        return std::make_shared<ReturnStmt>(expr); // 返回返回语句
    }
    else {
        printParsingAction("Stmt", "start parse ExprStmt");  // 开始解析表达式语句或空语句

        // 解析表达式语句或空语句
        std::shared_ptr<Exp> expr = nullptr;
        if (currentToken.type != TokenType::SE_SEMICOLON) {
            expr = parseExp();
            if (!expr) {
                printParsingAction("Stmt", "error: failed to parse Exp for ExprStmt");  // 解析表达式失败
                return nullptr;
            }
        }
        if (currentToken.type != TokenType::SE_SEMICOLON) {
            printParsingAction("Stmt", "error: ExprStmt lacks ';'");  // 表达式语句缺少 ';'
            syntaxError("表达式语句缺少 ';'");
            return nullptr;
        }
        advance(); // 消费 ';'
        printParsingAction("Stmt", "reduce ExprStmt");  // 成功解析表达式语句或空语句
        return std::make_shared<ExprStmt>(expr);
    }
    printParsingAction("Stmt", "error: unrecognized Stmt type");  // 无法识别的语句类型
    currentToken = rollbackToken; // 回滚
    return nullptr;
}

//18. exp -> addExp;
std::shared_ptr<Exp>Parser::parseExp() {
    printParsingAction("Exp", "enter");  // 开始解析 Exp

    auto addExpression = parseAddExp();
    if (!addExpression) {
        printParsingAction("Exp", "error: failed to parse AddExp");  // 解析 AddExp 失败
        return nullptr;
    }
    printParsingAction("Exp", "reduce AddExp"); // 成功解析出 AddExp

    return std::make_shared<Exp>(addExpression);

    printParsingAction("Exp", "exit");  // 完成 Exp 解析
}
//19. cond -> lOrExp;
std::shared_ptr<Cond> Parser::parseCond() {
    printParsingAction("Cond", "enter");  // 开始解析 Cond

    // Parse the logical OR expression
    auto lOrExp = parseLOrExp();
    if (!lOrExp) {
        // If parsing the logical OR expression fails, return nullptr
        printParsingAction("Cond", "error: failed to parse LOrExp");  // 解析 LOrExp 失败
        return nullptr;
    }

    printParsingAction("Cond", "reduce LOrExp");  // 成功解析出 LOrExp

    // If successful, wrap it in a Cond object and return it
    auto result = std::make_shared<Cond>(lOrExp);

    printParsingAction("Cond", "exit");  // 完成 Cond 解析

    return result;
}

//20. lVal -> Ident;
std::shared_ptr<LVal> Parser::parseLVal() {
    printParsingAction("LVal", "enter");  // 开始解析 LVal
    Token rollbackToken = currentToken; // 保存当前的位置以便回滚

    // 检查当前 token 是否是标识符
    if (currentToken.type != TokenType::IDN) {
        // 如果当前 token 不是 IDN 类型，则报告语法错误
        syntaxError("Expected identifier for lVal, found: " + currentToken.lexeme);
        printParsingAction("LVal", "error: token is not an identifier");  // 当前 token 不是标识符
        currentToken = rollbackToken; // 回滚至初始位置
        return nullptr; // 返回 nullptr 表示解析失败
    }

    // 保存标识符的名称
    std::string name = currentToken.lexeme;
    printParsingAction("LVal", "got identifier: " + name);  // 获取到标识符名称

    // 移动到下一个 token
    advance();
    printParsingAction("LVal", "advance to next token"); // 移动到下一个 token

    // 创建 LVal 的实例并返回
    auto result = std::make_shared<LVal>(name);
    printParsingAction("LVal", "reduce LVal");  // 成功创建 LVal 实例

    return result;
}

//21. primaryExp ->'(' exp ')'| lVal| number;
std::shared_ptr<PrimaryExp> Parser::parsePrimaryExp() {
    printParsingAction("PrimaryExp", "enter");
    Token rollbackToken = currentToken; // 保存当前的位置以便回滚

    if (currentToken.type == TokenType::SE_LPAREN) {
        printParsingAction("PrimaryExp", "parsing '(' expression ')'");
        advance(); // 消费 '('

        auto expr = parseExp();
        if (!expr) {
            currentToken = rollbackToken; // 回滚至初始位置
            return nullptr; // 解析失败
        }

        if (currentToken.type != TokenType::SE_RPAREN) {
            syntaxError("Expected ')' after expression");
            currentToken = rollbackToken; // 回滚至初始位置
            return nullptr; // 解析失败
        }
        advance(); // 消费 ')'

        printParsingAction("PrimaryExp", "reduce to expression");
        return std::make_shared<PrimaryExp>(PrimaryExp::ExpressionType::EXPRESSION, expr);
    }
    else if (currentToken.type == TokenType::IDN) {
        printParsingAction("PrimaryExp", "parsing LVal");

        auto lval = parseLVal();
        if (!lval) {
            currentToken = rollbackToken; // 回滚至初始位置
            return nullptr; // 解析失败
        }

        printParsingAction("PrimaryExp", "reduce to LVal");
        return std::make_shared<PrimaryExp>(PrimaryExp::ExpressionType::LVAL, lval);
    }
    else if (currentToken.type == TokenType::INT) {
        printParsingAction("PrimaryExp", "parsing number");

        auto num = parseNumber();
        if (!num) {
            currentToken = rollbackToken; // 回滚至初始位置
            return nullptr; // 解析失败
        }

        printParsingAction("PrimaryExp", "reduce to number");
        return std::make_shared<PrimaryExp>(PrimaryExp::ExpressionType::NUMBER, num);
    }
    else {
        syntaxError("Expected primary expression");
        printParsingAction("PrimaryExp", "error: no match for primary expression");
        currentToken = rollbackToken; // 回滚至初始位置
        return nullptr; // 当前 token 不匹配任何 primaryExp 的子规则
    }
}

//22. number -> IntConst ;
std::shared_ptr<Number> Parser::parseNumber() {
    printParsingAction("Number", "enter");  // 开始解析 Number
    Token rollbackToken = currentToken; // 保存当前的位置以便回滚

    // 检查当前 token 是否为 INT 类型的字符
    if (currentToken.type == TokenType::INT) {
        int value;
        try {
            value = std::stoi(currentToken.lexeme); // 解析整数值
            printParsingAction("Number", "parsed integer: " + currentToken.lexeme);  // 解析整数成功
        }
        catch (const std::invalid_argument& e) {
            syntaxError("Invalid integer: " + currentToken.lexeme);
            printParsingAction("Number", "error: invalid integer format");  // 无效的整数格式
            currentToken = rollbackToken; // 回滚至初始位置
            return nullptr;
        }
        catch (const std::out_of_range& e) {
            syntaxError("Integer out of range: " + currentToken.lexeme);
            printParsingAction("Number", "error: integer out of range");  // 整数超出范围
            currentToken = rollbackToken; // 回滚至初始位置
            return nullptr;
        }

        advance(); // 推进到下一个 token
        printParsingAction("Number", "advance to next token");  // 推进到下一个 token

        auto number = std::make_shared<Number>(value);
        printParsingAction("Number", "reduce to number");  // 成功创建 Number 实例
        return number;
    }
    else {
        syntaxError("Expected integer constant, found: " + currentToken.lexeme);
        printParsingAction("Number", "error: expected integer, found other");  // 预期是整数常量，发现其他类型
        currentToken = rollbackToken; // 回滚至初始位置
        return nullptr; // 如果当前 token 不是 INT 类型，则返回 nullptr
    }
}


//23. unaryExp ->primaryExp| Ident '(' (funcRParams) ? ')'| unaryOp unaryExp;
std::shared_ptr<UnaryExp> Parser::parseUnaryExp() {
    printParsingAction("UnaryExp", "enter");  // 开始解析 UnaryExp

    if (currentToken.type == TokenType::IDN) {
        std::string functionName = currentToken.lexeme;
        printParsingAction("UnaryExp", "function name: " + functionName);
        advance(); // 消费标识符

        if (currentToken.type == TokenType::SE_LPAREN) {
            printParsingAction("UnaryExp", "parsing function call");
            advance(); // 消费 '('

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
            advance(); // 消费 ')'
            auto funcExp = std::make_shared<UnaryExp>();
            funcExp->type = UnaryExp::Type::FunctionCall;
            funcExp->ident = functionName;
            funcExp->funcRParams = frp;
            printParsingAction("UnaryExp", "reduce to function call");
            return funcExp;
        }
        else {
            index--; // 回退
            currentToken = tokens.at(index); // 重置 currentToken
            printParsingAction("UnaryExp", "not a function call, backtracking");
        }
    }

    if (currentToken.type == TokenType::OP_PLUS ||
        currentToken.type == TokenType::OP_MINUS) {
        printParsingAction("UnaryExp", "parsing unary operation");
        auto unaryOp = parseUnaryOp(); // 解析一元操作符
        auto unaryExp = parseUnaryExp(); // 解析后续的 unaryExp
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
    printParsingAction("UnaryOp", "enter");  // 开始解析 UnaryOp
    Token rollbackToken = currentToken; // 保存当前的位置以便回滚
    char op;
    switch (currentToken.type) {
    case TokenType::OP_PLUS:
        op = '+'; // '+' 一元运算符
        printParsingAction("UnaryOp", "found '+' operator");
        break;
    case TokenType::OP_MINUS:
        op = '-'; // '-' 一元运算符
        printParsingAction("UnaryOp", "found '-' operator");
        break;
    default:
        printParsingAction("UnaryOp", "unexpected operator, exiting");
        syntaxError("Expected unary operator '+', '-' or '!', found: " + currentToken.lexeme);
        currentToken = rollbackToken; // 回滚至初始位置
        return nullptr; // 如果当前 token 不是一元运算符，返回 nullptr
    }
    advance(); // 推进到下一个 token
    printParsingAction("UnaryOp", "advanced to next token");

    // 创建 UnaryOp 的实例并返回
    printParsingAction("UnaryOp", "reduce");
    return std::make_shared<UnaryOp>(op);
}

//25. funcRParams -> funcRParam (',' funcRParam)*;
std::shared_ptr<FuncRParams> Parser::parseFuncRParams() {
    printParsingAction("FuncRParams", "enter");  // 开始解析 FuncRParams

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
        Token rollbackToken = currentToken; // 保存当前的 token 以便回滚

        advance();  // 消费逗号 token

        auto param = parseFuncRParam();
        if (!param) {
            printParsingAction("FuncRParams", "parsing next parameter failed");
            currentToken = rollbackToken; // 回滚至之前保存的 token
            break;
        }

        printParsingAction("FuncRParams", "next parameter parsed");
        paramsNode->addParam(param);
    }

    printParsingAction("FuncRParams", "all parameters parsed");  // 解析了所有参数
    return paramsNode;
}
//26. funcRParam -> exp;
std::shared_ptr<FuncRParam> Parser::parseFuncRParam() {
    printParsingAction("FuncRParam", "enter");  // 开始解析 FuncRParam

    auto expNode = parseExp();
    if (!expNode) {
        printParsingAction("FuncRParam", "parsing expression failed, exiting");  // 解析表达式失败
        return nullptr;
    }

    printParsingAction("FuncRParam", "expression parsed, creating FuncRParam instance");  // 解析表达式成功，创建 FuncRParam 实例

    auto funcRParam = std::make_shared<FuncRParam>(expNode);
    printParsingAction("FuncRParam", "exit");  // 结束 FuncRParam 解析

    return funcRParam;
}

//27. mulExp ->unaryExp| mulExp('*' | '/' | '%') unaryExp;
std::shared_ptr<MulExp> Parser::parseMulExp() {
    printParsingAction("MulExp", "enter");  // 开始解析 MulExp

    auto right = parseUnaryExp();
    if (!right) {
        printParsingAction("MulExp", "parsing initial UnaryExp failed, exiting");  // 解析 UnaryExp 失败
        return nullptr;
    }

    auto result = std::make_shared<MulExp>(right);
    printParsingAction("MulExp", "initial UnaryExp parsed");  // 解析了初始的 UnaryExp

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

        advance(); // 推进过操作符

        auto nextRight = parseUnaryExp();
        if (!nextRight) {
            printParsingAction("MulExp", "parsing next UnaryExp failed, exiting");  // 解析下一个 UnaryExp 失败
            return nullptr;
        }

        result = std::make_shared<MulExp>(result, op, nextRight);
        printParsingAction("MulExp", "new UnaryExp parsed and added");  // 解析了新的 UnaryExp 并添加
    }

    printParsingAction("MulExp", "exit");  // 结束解析 MulExp
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

        // 保存当前索引作为回滚点，用于之后可能的回滚
        size_t rollbackPosition = index;

        advance();

        auto nextRight = parseMulExp();
        if (!nextRight) {
            printParsingAction("AddExp", "parsing next MulExp failed, rolling back and exiting");

            // 如果解析失败，回滚到之前保存的索引位置
            rollback(rollbackPosition);

            // 这里选择返回 nullptr 或是不回滚，根据具体的解析策略和预期行为决定
            return nullptr; // 或者，您可以选择 break; 继续解析后续内容
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

    size_t rollbackPosition = index; // 保存当前索引以便可能的回滚

    // 检查当前 token 是否是关系操作符
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

    // 移过关系操作符
    advance();

    auto secondExpr = parseAddExp();
    if (!secondExpr) {
        printParsingAction("RelExp", "parsing second AddExp failed, rolling back and exiting");
        rollback(rollbackPosition); // 回滚到确认操作符之前的索引位置
        return std::make_shared<RelExp>(firstExpr); // 选择返回 firstExpr 作为回滚后的处理或选择其他处理方式
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
    printParsingAction("LOrExp", "enter");  // 开始解析 LOrExp

    std::shared_ptr<LOrExp> leftOrExp = nullptr;
    std::shared_ptr<LAndExp> leftLAndExp = parseLAndExp();

    if (leftLAndExp == nullptr) {
        printParsingAction("LOrExp", "Expected LAndExp at the beginning of LOrExp, exiting");  // LAndExp 解析失败
        return nullptr;
    }
    else {
        printParsingAction("LOrExp", "First LAndExp parsed successfully");  // 成功解析了第一个 LAndExp
    }

    while (currentToken.type == TokenType::OP_OR) {
        printParsingAction("LOrExp", "found '||' operator");  // 发现 '||' 操作符

        advance(); // 推进到 '||' 之后的 token

        auto right = parseLAndExp();
        if (!right) {
            printParsingAction("LOrExp", "Expected LAndExp after '||', exiting");  // 解析 LAndExp 失败
            return nullptr;
        }

        printParsingAction("LOrExp", "Parsed another LAndExp after '||'");  // 成功解析了 '||' 后的 LAndExp

        if (leftOrExp == nullptr) {
            leftOrExp = std::make_shared<LOrExp>(leftLAndExp);
        }

        leftOrExp = std::make_shared<LOrExp>(leftOrExp, right);

        printParsingAction("LOrExp", "Updated LOrExp with new LAndExp on the right");  // 使用新的 LAndExp 更新了 LOrExp

        if (index >= tokens.size()) break;
        currentToken = tokens.at(index);
    }

    if (leftOrExp == nullptr) {
        printParsingAction("LOrExp", "No '||' encountered, wrapping single LAndExp, exit");  // 没有遇到 '||'，结束解析
        return std::make_shared<LOrExp>(leftLAndExp);
    }

    printParsingAction("LOrExp", "exit");  // 结束解析 LOrExp

    return leftOrExp;
}
//33. constExp -> addExp;
std::shared_ptr<ConstExp> Parser::parseConstExp() {
    printParsingAction("ConstExp", "enter");  // 开始解析 ConstExp

    std::shared_ptr<AddExp> addExp = parseAddExp();
    if (addExp != nullptr) {
        printParsingAction("ConstExp", "AddExp parsed successfully, creating ConstExp");  // 成功解析 AddExp 并创建 ConstExp
        return std::make_shared<ConstExp>(addExp);
    }

    printParsingAction("ConstExp", "parsing AddExp failed, exiting");  // 解析 AddExp 失败，退出
    return nullptr; // If parseAddExp did not succeed
}
//34. IntConst -> [0-9]+ ;
std::shared_ptr<IntConst> Parser::parseIntConst() {
    printParsingAction("IntConst", "enter");  // 开始解析 IntConst

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

    advance();  // 推进到下一个 token

    // 创建 IntConst 的实例并返回
    printParsingAction("IntConst", "IntConst created, exit");  // 创建 IntConst 成功，退出解析
    return std::make_shared<IntConst>(intValue);
}
//35. Ident -> [a-zA-Z_][a-zA-Z_0-9]*;
std::shared_ptr<Ident> Parser::parseIdent() {
    printParsingAction("Ident", "enter");  // 开始解析 Ident

    if (currentToken.type != TokenType::IDN) {
        printParsingAction("Ident", "Syntax error, expected identifier, found: " + currentToken.lexeme);
        return nullptr;
    }

    // 保存标识符的名称
    std::string identName = currentToken.lexeme;
    printParsingAction("Ident", "Identifier found: " + identName);  // 找到标识符

    advance(); // 推进到下一个 token

    // 创建 Ident 的实例并返回
    printParsingAction("Ident", "Ident created, exit");  // 创建 Ident 成功，退出解析
    return std::make_shared<Ident>(identName);
}