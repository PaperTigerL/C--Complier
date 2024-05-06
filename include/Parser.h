#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "Ast.h"
#include <stdexcept>

class Parser {
public:
    std::vector<Token>& tokens;  // Parser不再直接引用Lexer实例，而是引用一个tokens向量
    size_t index;                // 当前token索引
    Token currentToken;          // 当前处理的token
    int step = 1;
    std::string stackTop;  // 你可能需要在适当的时机更新这个变量
    std::string action; // 当前的解析动作 ("move", "reduction", "accept", "error")
    bool advance(); // 推进到下一个 Token
    void rollback(size_t position);
    bool match(TokenType expected); // 匹配期望的 Token 类型，并推进
    // Parser 功能
    //1. Program -> compUnit
    std::shared_ptr<Program> parseProgram();
    //2. compUnit -> (decl | funcDef)* EOF;
    std::shared_ptr<CompUnit> parseCompUnit();
    //3. decl -> constDecl | varDecl;
    std::shared_ptr<Declaration> parseDeclaration();
    //4. constDecl -> 'const' bType constDef (',' constDef)* ';';
    std::shared_ptr<ConstantDeclaration> parseConstDecl();
    //5. bType -> 'int';
    std::shared_ptr<BasicType>parseBasicType();
    //6. constDef -> Ident '=' constInitVal;
    std::shared_ptr<ConstDef>parseConstDef();
    //7. constInitVal -> constExp
    std::shared_ptr<ConstInitVal>parseConstInitVal();
    //8.varDecl -> bType varDef (',' varDef)* ';';
    std::shared_ptr<VarDecl>parseVarDecl();
    //9. varDef ->Ident| Ident '=' initVal;
    std::shared_ptr<VarDef>parseVarDef();
    //10. initVal -> exp;
    std::shared_ptr<InitVal>parseInitVal();
    //11. funcDef -> funcType Ident '(' (funcFParams)? ')' block;
    std::shared_ptr<FuncDef> parseFuncDef();
    //12. funcType -> 'void' | 'int' ;
    std::shared_ptr<FuncType>parseFuncType();
    //13. funcFParams -> funcFParam (',' funcFParam)*;
    std::shared_ptr<FuncFParams>parseFuncFParams();
    //14. funcFParam -> bType Ident;
    std::shared_ptr<FuncFParam>parseFuncFParam();
    //15. block -> '{' (blockItem)* '}';
    std::shared_ptr<Block>parseBlock();
    //16. blockItem -> decl | stmt;
    std::shared_ptr<BlockItem>parseBlockItem();
    //17. stmt ->lVal '=' exp ';'| (exp) ? ';'| block| 'return' (exp) ? ';';
    std::shared_ptr<Stmt>parseStmt();
    //18. exp -> addExp;
    std::shared_ptr<Exp>parseExp();
    //19. cond -> lOrExp;
    std::shared_ptr<Cond>parseCond();
    //20. lVal -> Ident;
    std::shared_ptr<LVal>parseLVal();
    //21. primaryExp ->'(' exp ')'| lVal| number;
    std::shared_ptr<PrimaryExp>parsePrimaryExp();
    //22. number -> IntConst ;
    std::shared_ptr<Number>parseNumber();
    //23. unaryExp ->primaryExp| Ident '(' (funcRParams) ? ')'| unaryOp unaryExp;
    std::shared_ptr<UnaryExp>parseUnaryExp();
    //24. unaryOp -> '+' | '-' | '!';
    std::shared_ptr<UnaryOp>parseUnaryOp();
    //25. funcRParams -> funcRParam (',' funcRParam)*;
    std::shared_ptr<FuncRParams>parseFuncRParams();
    //26. funcRParam -> exp;
    std::shared_ptr<FuncRParam>parseFuncRParam();
    //27. mulExp ->unaryExp| mulExp('*' | '/' | '%') unaryExp;
    std::shared_ptr<MulExp>parseMulExp();
    //28. addExp -> mulExp # add1 | addExp ('+' | '-') mulExp;
    std::shared_ptr<AddExp>parseAddExp();
    //29. relExp ->addExp| relExp('<' | '>' | '<=' | '>=') addExp;
    std::shared_ptr<RelExp>parseRelExp();
    //30. eqExp ->relExp| eqExp('==' | '!=') relExp;
    std::shared_ptr<EqExp>parseEqExp();
    //31. lAndExp ->eqExp| lAndExp '&&' eqExp;
    std::shared_ptr<LAndExp>parseLAndExp();
    //32. lOrExp ->lAndExp| lOrExp '||' lAndExp;
    std::shared_ptr<LOrExp>parseLOrExp();
    //33. constExp -> addExp;
    std::shared_ptr<ConstExp>parseConstExp();
    //34. IntConst -> [0-9]+ ;
    std::shared_ptr<IntConst>parseIntConst();
    //35. Ident -> [a-zA-Z_][a-zA-Z_0-9]*;
    std::shared_ptr<Ident>parseIdent();
    void syntaxError(const std::string& message); // 报告语法错误
    // 构造函数，同时获取第一个 Token，并处理可能的错误
    Parser(std::vector<Token>& tokens)
        : tokens(tokens), index(0), currentToken(tokens.at(0)) {
        // 确认第一个Token是否有效
        if (currentToken.type == TokenType::INVALID) {
            // 处理Token错误，可以抛出异常或进行错误记录
            syntaxError("Invalid token at the beginning of the source");
        }
    }
    void printParsingAction(const std::string& rule, const std::string& action) {
        std::cout << step << "\t" << rule << "#" << currentToken.lexeme << "\t" << action << std::endl;
        ++step; // 每次打印后步骤计数器增加
    }
};
#endif // PARSER_H