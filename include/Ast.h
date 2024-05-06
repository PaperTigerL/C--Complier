#ifndef AST_H
#define AST_H

#include "Token.h"
#include <memory>
#include <vector>

// AST节点基类
class ASTNode {
public:
    virtual ~ASTNode() {}
    //待添加   
};
// 前向声明
class ASTNode;
class Program;
class CompUnit;
class Declaration;
class BasicType;
class VariableDeclaration;
class ConstantDeclaration;
class ConstDef;
class ConstInitVal;
class VarDecl;
class VarDef;
class InitVal;
class FuncDef;
class FuncType;
class FuncFParams;
class FuncFParam;
class Block;
class BlockItem;
class Stmt;
class AssignStmt;
class ExprStmt;
class BlockStmt;
class ReturnStmt;
class Exp;
class Cond;
class LVal;
class PrimaryExp;
class Number;
class UnaryExp;
class UnaryOp;
class FuncRParams;
class FuncRParam;
class MulExp;
class AddExp;
class RelExp;
class EqExp;
class LAndExp;
class LOrExp;
class ConstExp;
class IntConst;
class Ident;


// 程序节点
//1. Program -> compUnit
class Program : public ASTNode {
public:
    std::vector<std::shared_ptr<CompUnit>> compUnits; // 包含零或多个compUnit

    // 添加一个compUnit到程序节点
    void addCompUnit(const std::shared_ptr<CompUnit>& unit) {
        compUnits.push_back(unit);
    }
};
//16. blockItem -> decl | stmt;
class BlockItem : public ASTNode {
    // 这里可以是通用的接口和虚析构函数
public:
    virtual ~BlockItem() {}  // 确保派生类可以正确析构

};
//2. compUnit -> (decl | funcDef)* EOF;
class CompUnit : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> nodes; // 可以包含声明或者函数定义

    // 方法用于添加节点（不区分声明和函数定义）
    void addNode(const std::shared_ptr<ASTNode>& node) {
        nodes.push_back(node);
    }
};
//5. bType -> 'int';
// 基本类型的AST节点类
class BasicType : public ASTNode {
public:
    enum Type { INT }; // 在这里我们只有'int'，如果有其他基本类型也可以在这里添加
    Type type; // 节点所代表的类型

    // 构造函数初始化为'int'
    BasicType() : type(INT) {}

    // 这个构造函数可以根据实际情况忽略，因为目前只有'int'一种基本类型
    BasicType(Type type) : type(type) {}
};
//3. decl -> constDecl | varDecl;
class Declaration : public BlockItem {
};

//4. constDecl -> 'const' bType constDef (',' constDef)* ';';
// 常量声明类，用于表示包含一个或多个constDef的完整常量声明
class ConstantDeclaration : public Declaration {
public:
    BasicType type; // 'const' 关键字后面的基本类型
    std::vector<std::shared_ptr<ConstDef>> consts; // 一个或多个常量定义
      // 添加默认构造函数
    ConstantDeclaration() : type(BasicType::INT), consts({}) {}
    ConstantDeclaration(BasicType type, const std::vector<std::shared_ptr<ConstDef>>& consts)
        : type(type), consts(consts) {}

    // 添加一个ConstDef到常量声明中
    void addConstDef(const std::shared_ptr<ConstDef>& constDef) {
        consts.push_back(constDef);
    }
};


//6. constDef -> Ident '=' constInitVal;
// 常量定义的AST节点类
class ConstDef : public ASTNode {
public:
    std::string identifier; // 标识符
    std::shared_ptr<ConstExp> constInitVal; // 常量的初始值

    // 带参数的构造函数
    ConstDef(const std::string& id, const std::shared_ptr<ConstExp>& value)
        : identifier(id), constInitVal(value) {}

    // 默认构造函数
    ConstDef()
        : identifier(""), constInitVal(nullptr) {}
};
//7. constInitVal -> constExp
// 常量初始值的AST节点类，它实质上封装一个ConstExp
class ConstInitVal : public ASTNode {
public:
    std::shared_ptr<ConstExp> constExp; // 指向常量表达式的智能指针

    // 构造函数用来初始化常量初始化值
    ConstInitVal(const std::shared_ptr<ConstExp>& exp)
        : constExp(exp) {}
};
//8.varDecl -> bType varDef (',' varDef)* ';';
// 变量声明的AST节点类
class VarDecl : public Declaration {
public:
    std::shared_ptr<BasicType> type; // 变量的类型
    std::vector<std::shared_ptr<VarDef>> varDefs; // 包含一个或多个变量定义的列表
    // 默认构造函数
    VarDecl() : type(std::make_shared<BasicType>()), varDefs() {
        // 这里依赖于BasicType有一个可以用来默认构造的方法
        // varDefs 初始化为空的向量
    }

    // 构造函数用于初始化变量声明
    VarDecl(const std::shared_ptr<BasicType>& type, const std::vector<std::shared_ptr<VarDef>>& defs)
        : type(type), varDefs(defs) {}
};
//9. varDef ->Ident| Ident '=' initVal;
// 变量定义的AST节点类
class VarDef : public Declaration {
public:
    std::string identifier; // 标识符，代表变量名称
    std::shared_ptr<InitVal> initVal; // 初始化值的表达式节点，可能为空

    // 构造函数用于初始化一个没有初始值的变量定义
    explicit VarDef(const std::string& id)
        : identifier(id), initVal(nullptr) {}

    // 构造函数用于初始化一个具有初始值的变量定义
    VarDef(const std::string& id, const std::shared_ptr<InitVal>& value)
        : identifier(id), initVal(value) {}

    // 析构函数，根据需要定义，如果使用智能指针通常可以不定义
    virtual ~VarDef() {}
};
//10. initVal -> exp;
// 初始化值(initVal)的AST节点类
class InitVal : public ASTNode {
public:
    std::shared_ptr<Exp> exp; // 用于持有表达式节点的指针

    // 构造函数用来初始化initVal节点，它包含一个表达式
    explicit InitVal(const std::shared_ptr<Exp>& expression)
        : exp(expression) {}

   
};
//11. funcDef -> funcType Ident '(' (funcFParams)? ')' block;
// 函数定义的AST节点类
class FuncDef : public BlockItem {
public:
    std::shared_ptr<FuncType> funcType;   // 函数返回类型节点
    std::string ident;                   // 函数名称
    std::shared_ptr<FuncFParams> params; // 函数参数列表，可选
    std::shared_ptr<Block> body;         // 函数体
    FuncDef() : funcType(nullptr), ident(""), params(nullptr), body(nullptr) {
        // 默认构造函数的实现（测试需要的时候添加）
    }
    // 构造函数用于初始化函数定义
    FuncDef(const std::shared_ptr<FuncType>& type,
        const std::string& name,
        const std::shared_ptr<FuncFParams>& parameters,
        const std::shared_ptr<Block>& block)
        : funcType(type), ident(name), params(parameters), body(block) {}

    // 析构函数，根据需要定义，如果使用智能指针通常可以不定义
    virtual ~FuncDef() {}
};
//12. funcType -> 'void' | 'int' ;
// 表示函数类型的枚举
enum class FuncTypeEnum {
    Void,
    Integer,
};
// 函数类型的AST节点类
class FuncType : public ASTNode {
public:
    FuncTypeEnum type; // 用于保存函数类型的枚举值

    // 构造函数用于初始化函数类型节点
    explicit FuncType(FuncTypeEnum t) : type(t) {}

    // 根据枚举type返回对应的字符串表示，例如 "void" 或 "int"
    std::string getTypeString() const {
        switch (type) {
        case FuncTypeEnum::Void: return "void";
        case FuncTypeEnum::Integer: return "int";
        default: return ""; // 永远不应该达到这里
        }
    }

    // 其他方法...
};

//13. funcFParams -> funcFParam (',' funcFParam)*;
// 函数形参列表的AST节点类
class FuncFParams : public ASTNode {
public:
    std::vector<std::shared_ptr<FuncFParam>> params; // 存储函数形参节点的集合

    // 构造函数初始化形参列表
    FuncFParams() = default;

    // 添加一个形参到列表中
    void addParam(const std::shared_ptr<FuncFParam>& param) {
        params.push_back(param);
    }

    // 其他可能的方法和成员函数...
};
//14. funcFParam -> bType Ident;
// 函数形参的AST节点类
class FuncFParam : public ASTNode {
public:
    // 根据bType定义的类型
    BasicType type; // 或者 std::shared_ptr<bType> type; 如果bType是一个类
    std::string ident; // 参数名称

    // 构造函数用于初始化形参节点
    FuncFParam(BasicType t, const std::string& id) // 或者使用 std::shared_ptr<bType> t 如果bType是一个类
        : type(t), ident(id) {}

    // 析构函数，根据需要定义，如果使用智能指针通常可以不定义
    virtual ~FuncFParam() {}

    // 其他可能的方法和成员函数...
};
//17. stmt ->lVal '=' exp ';'| (exp) ? ';'| block| 'return' (exp) ? ';';
class Stmt : public BlockItem {

};

// 赋值语句
class AssignStmt : public Stmt {
public:
    std::shared_ptr<LVal> lvalue;
    std::shared_ptr<Exp> expression;

    AssignStmt(std::shared_ptr<LVal> lval, std::shared_ptr<Exp> expr)
        : lvalue(lval), expression(expr) {}
};

// 表达式语句
class ExprStmt : public Stmt {
public:
    std::shared_ptr<Exp> expression; // 可能为空

    explicit ExprStmt(std::shared_ptr<Exp> expr = nullptr)
        : expression(expr) {}
};

// 代码块
class BlockStmt : public Stmt {

};

// 返回语句
class ReturnStmt : public Stmt {
public:
    std::shared_ptr<Exp> expression; // 可能为空

    explicit ReturnStmt(std::shared_ptr<Exp> expr = nullptr)
        : expression(expr) {}
};
//15. block -> '{' (blockItem)* '}';
class Block : public Stmt {
public:
    std::vector<std::shared_ptr<BlockItem>> items; // 存储代码块条目的向量

    // 构造函数初始化代码块
    Block() = default;

    // 添加一个代码块条目到项列表
    void addItem(const std::shared_ptr<BlockItem>& item) {
        items.push_back(item);
    }

    // 其他可能的方法和成员函数...
};

//18. exp -> addExp;
// 表达式类
class Exp : public ASTNode {
public:
    std::shared_ptr<AddExp> addExpression; // 存储一个加法表达式

    // 构造函数
    explicit Exp(std::shared_ptr<AddExp> expr)
        : addExpression(expr) {}
};
//19. cond -> lOrExp;
// 条件表达式类
class Cond : public ASTNode {
public:
    std::shared_ptr<LOrExp> lOrExpression; // 存储一个逻辑或表达式

    // 构造函数
    explicit Cond(std::shared_ptr<LOrExp> expr)
        : lOrExpression(expr) {}
};
//20. lVal -> Ident;
class LVal : public ASTNode {
public:
    std::string identifier; // 用于存储标识符名称

    // 构造函数
    explicit LVal(const std::string& ident) : identifier(ident) {}
};
//21. primaryExp ->'(' exp ')'| lVal| number;
class PrimaryExp : public ASTNode {
public:
    enum class ExpressionType {
        EXPRESSION,
        LVAL,
        NUMBER
    };

    // 记录 PrimaryExp 的类型
    ExpressionType type;

    // 存放具体的子表达式
    std::shared_ptr<Exp> expression; // 如果 type 是 EXPRESSION
    std::shared_ptr<LVal> lValue; // 如果 type 是 LVAL
    std::shared_ptr<Number> number; // 如果 type 是 NUMBER

    // 构造函数，根据不同类型，构建相应的数据结构
    PrimaryExp(ExpressionType type, std::shared_ptr<Exp> exp)
        : type(type), expression(exp), lValue(nullptr), number(nullptr) {}

    PrimaryExp(ExpressionType type, std::shared_ptr<LVal> lval)
        : type(type), expression(nullptr), lValue(lval), number(nullptr) {}

    PrimaryExp(ExpressionType type, std::shared_ptr<Number> num)
        : type(type), expression(nullptr), lValue(nullptr), number(num) {}

    
};
//22. number -> IntConst ;
class Number : public ASTNode {
public:
    int value; // 用来存储整数常量

    // 构造函数
    explicit Number(int val) : value(val) {}

    // getters, setters, 或其他方法（如果需要）
};
//23. unaryExp ->primaryExp| Ident '(' (funcRParams) ? ')'| unaryOp unaryExp;
class UnaryExp : public ASTNode {
public:
    enum class Type {
        PrimaryExp,
        FunctionCall,
        UnaryOperation
    };

    Type type; // 表示UnaryExp实际的类型

    // 对于PrimaryExp类型
    std::shared_ptr<PrimaryExp> primaryExp;

    // 对于FunctionCall类型
    std::string ident;
    std::shared_ptr<FuncRParams> funcRParams;

    // 对于UnaryOperation类型
    std::shared_ptr<UnaryOp> unaryOp;
    std::shared_ptr<UnaryExp> operand;

};
//24. unaryOp -> '+' | '-' | '!';
class UnaryOp : public ASTNode {
public:
    char op; // 用于存储一元操作符的字符

    // 构造函数接收一个表示一元操作符的字符
    explicit UnaryOp(char operation)
        : op(operation) {}

    // 其他逻辑和方法
};
//25. funcRParams -> funcRParam (',' funcRParam)*;
class FuncRParams : public ASTNode {
public:
    std::vector<std::shared_ptr<FuncRParam>> params; // 存储 FuncRParam 实例的向量

    // 构造函数
    FuncRParams() {}

    // 向参数列表中添加新的 FuncRParam
    void addParam(const std::shared_ptr<FuncRParam>& param) {
        params.push_back(param);
    }

    // 其他可能的逻辑和方法
};
//26. funcRParam -> exp;
class FuncRParam : public ASTNode {
public:
    std::shared_ptr<Exp> expression; // 包含单个表达式的成员

    // 构造函数接收一个表达式节点，并将其保存为实参
    explicit FuncRParam(std::shared_ptr<Exp> exp)
        : expression(exp) {}

    // 其他逻辑和方法
};
//27. mulExp ->unaryExp| mulExp('*' | '/' | '%') unaryExp;
class MulExp : public ASTNode {
public:
    std::shared_ptr<MulExp> left;     // 左侧表达式，可能是 MulExp 或 UnaryExp
    std::shared_ptr<UnaryExp> right;    // 右侧表达式，是 UnaryExp
    char operation; // 存储操作符，'*'、'/' 或 '%'

    // 构造函数适用于单个 UnaryExp
    explicit MulExp(std::shared_ptr<UnaryExp> expr) : left(nullptr), right(expr), operation(0) {}

    // 构造函数适用于两个表达式的乘法、除法或取模
    MulExp(std::shared_ptr<MulExp> left, char op, std::shared_ptr<UnaryExp> right)
        : left(left), right(right), operation(op) {}

    // 更多逻辑和方法...
};
//28. addExp -> mulExp # add1 | addExp ('+' | '-') mulExp;
class AddExp : public ASTNode {
public:
    std::shared_ptr<AddExp> left;  // 左边的表达式节点，可以是AddExp或MulExp
    std::shared_ptr<MulExp> right; // 右边的乘法表达式节点
    char op;                        // 操作符，'+' 或 '-'

    // 如果是简单的乘法表达式
    explicit AddExp(std::shared_ptr<MulExp> simpleExp)
        : left(nullptr), right(simpleExp), op('\0') {}

    // 如果左边是一个加法表达式，右边是乘法表达式
    AddExp(std::shared_ptr<AddExp> left, char op, std::shared_ptr<MulExp> right)
        : left(left), right(right), op(op) {}

    // 这里你可以添加更多的函数和逻辑，比如遍历表达式树的方法等
};
//29. relExp ->addExp| relExp('<' | '>' | '<=' | '>=') addExp;
class RelExp : public ASTNode {
public:
    std::shared_ptr<ASTNode> left;  // 左侧表达式，可能为RelExp或AddExp
    std::shared_ptr<AddExp> right; // 右侧表达式，总是一个AddExp
    std::string op;                 // 关系操作符 '<', '>', '<=', '>='

    // 适用于单独的 AddExp 情形
    explicit RelExp(std::shared_ptr<AddExp> right)
        : left(nullptr), right(right), op("") {}

    // 适用于左侧是 RelExp 的情形
    RelExp(std::shared_ptr<RelExp> left, const std::string& op, std::shared_ptr<AddExp> right)
        : left(left), right(right), op(op) {}

    // 适用于左侧是 AddExp 的情形
    RelExp(std::shared_ptr<AddExp> left, const std::string& op, std::shared_ptr<AddExp> right)
        : left(left), right(right), op(op) {}

    // 其他逻辑和方法
};
//30. eqExp ->relExp| eqExp('==' | '!=') relExp;
class EqExp : public ASTNode {
public:
    std::shared_ptr<EqExp> left;  // 左侧表达式，可能为另一个EqExp或nullptr
    std::shared_ptr<RelExp> right; // 右侧表达式，总是一个RelExp
    std::string op;                 // 操作符，'==' 或 '!='

    // 构造函数：适用于纯RelExp情况
    explicit EqExp(std::shared_ptr<RelExp> right)
        : left(nullptr), right(right), op("") {}

    // 构造函数：适用于 'eqExp == relExp' 或 'eqExp != relExp' 情况
    EqExp(std::shared_ptr<EqExp> left, const std::string& op, std::shared_ptr<RelExp> right)
        : left(left), right(right), op(op) {}

    // 其他逻辑和方法
};
//31. lAndExp ->eqExp| lAndExp '&&' eqExp;
class LAndExp : public ASTNode {
public:
    std::shared_ptr<LAndExp> left;  // 左侧，可能是另一个LAndExp
    std::shared_ptr<EqExp> right; // 右侧，总是一个EqExp
    bool isPureEqExp;               // 是否纯粹的EqExp

    // 构造函数: 适用于纯EqExp情况
    explicit LAndExp(std::shared_ptr<EqExp> right)
        : left(nullptr), right(right), isPureEqExp(true) {}

    // 构造函数: 适用于'lAndExp && eqExp'情况
    LAndExp(std::shared_ptr<LAndExp> left, std::shared_ptr<EqExp> right)
        : left(left), right(right), isPureEqExp(false) {}

    // 其他逻辑和方法
};
//32. lOrExp ->lAndExp| lOrExp '||' lAndExp;
class LOrExp : public ASTNode {
public:
    std::shared_ptr<LOrExp> left; // 左边的表达式节点
    std::shared_ptr<LAndExp> right; // 右边的表达式节点

    // 如果是简单的 lAndExp
    explicit LOrExp(std::shared_ptr<LAndExp> landExp)
        : left(nullptr), right(landExp) {}

    // LOrExp 可能需要一个构造器来进行初始化
    LOrExp(std::shared_ptr<LOrExp> leftExp, std::shared_ptr<LAndExp> rightExp)
        : left(leftExp), right(rightExp) {}
    // 这里可以添加其他成员函数和逻辑
};
//33. constExp -> addExp;
class ConstExp : public ASTNode {
public:
    std::shared_ptr<AddExp> addExp; // 指向加法表达式的指针

    explicit ConstExp(std::shared_ptr<AddExp> expr) : addExp(expr) {}

    // 如果有需要，可以在这里添加额外的逻辑或方法
};
//34. IntConst -> [0-9]+ ;
class IntConst : public ASTNode {
public:
    int value; // 存储整数常量的值

    explicit IntConst(int value) : value(value) {}

    // 这里可以根据需要添加成员函数
    // 例如返回整数常量的值等
};
//35. Ident -> [a-zA-Z_][a-zA-Z_0-9]*;
class Ident : public ASTNode {
public:
    std::string name; // 存储标识符的字符串s

    explicit Ident(const std::string& name) : name(name) {}

};


#endif // AST_H