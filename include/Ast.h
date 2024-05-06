#ifndef AST_H
#define AST_H

#include "Token.h"
#include <memory>
#include <vector>

// AST�ڵ����
class ASTNode {
public:
    virtual ~ASTNode() {}
    //�����   
};
// ǰ������
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


// ����ڵ�
//1. Program -> compUnit
class Program : public ASTNode {
public:
    std::vector<std::shared_ptr<CompUnit>> compUnits; // ���������compUnit

    // ���һ��compUnit������ڵ�
    void addCompUnit(const std::shared_ptr<CompUnit>& unit) {
        compUnits.push_back(unit);
    }
};
//16. blockItem -> decl | stmt;
class BlockItem : public ASTNode {
    // ���������ͨ�õĽӿں�����������
public:
    virtual ~BlockItem() {}  // ȷ�������������ȷ����

};
//2. compUnit -> (decl | funcDef)* EOF;
class CompUnit : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> nodes; // ���԰����������ߺ�������

    // ����������ӽڵ㣨�����������ͺ������壩
    void addNode(const std::shared_ptr<ASTNode>& node) {
        nodes.push_back(node);
    }
};
//5. bType -> 'int';
// �������͵�AST�ڵ���
class BasicType : public ASTNode {
public:
    enum Type { INT }; // ����������ֻ��'int'�������������������Ҳ�������������
    Type type; // �ڵ������������

    // ���캯����ʼ��Ϊ'int'
    BasicType() : type(INT) {}

    // ������캯�����Ը���ʵ��������ԣ���ΪĿǰֻ��'int'һ�ֻ�������
    BasicType(Type type) : type(type) {}
};
//3. decl -> constDecl | varDecl;
class Declaration : public BlockItem {
};

//4. constDecl -> 'const' bType constDef (',' constDef)* ';';
// ���������࣬���ڱ�ʾ����һ������constDef��������������
class ConstantDeclaration : public Declaration {
public:
    BasicType type; // 'const' �ؼ��ֺ���Ļ�������
    std::vector<std::shared_ptr<ConstDef>> consts; // һ��������������
      // ���Ĭ�Ϲ��캯��
    ConstantDeclaration() : type(BasicType::INT), consts({}) {}
    ConstantDeclaration(BasicType type, const std::vector<std::shared_ptr<ConstDef>>& consts)
        : type(type), consts(consts) {}

    // ���һ��ConstDef������������
    void addConstDef(const std::shared_ptr<ConstDef>& constDef) {
        consts.push_back(constDef);
    }
};


//6. constDef -> Ident '=' constInitVal;
// ���������AST�ڵ���
class ConstDef : public ASTNode {
public:
    std::string identifier; // ��ʶ��
    std::shared_ptr<ConstExp> constInitVal; // �����ĳ�ʼֵ

    // �������Ĺ��캯��
    ConstDef(const std::string& id, const std::shared_ptr<ConstExp>& value)
        : identifier(id), constInitVal(value) {}

    // Ĭ�Ϲ��캯��
    ConstDef()
        : identifier(""), constInitVal(nullptr) {}
};
//7. constInitVal -> constExp
// ������ʼֵ��AST�ڵ��࣬��ʵ���Ϸ�װһ��ConstExp
class ConstInitVal : public ASTNode {
public:
    std::shared_ptr<ConstExp> constExp; // ָ�������ʽ������ָ��

    // ���캯��������ʼ��������ʼ��ֵ
    ConstInitVal(const std::shared_ptr<ConstExp>& exp)
        : constExp(exp) {}
};
//8.varDecl -> bType varDef (',' varDef)* ';';
// ����������AST�ڵ���
class VarDecl : public Declaration {
public:
    std::shared_ptr<BasicType> type; // ����������
    std::vector<std::shared_ptr<VarDef>> varDefs; // ����һ����������������б�
    // Ĭ�Ϲ��캯��
    VarDecl() : type(std::make_shared<BasicType>()), varDefs() {
        // ����������BasicType��һ����������Ĭ�Ϲ���ķ���
        // varDefs ��ʼ��Ϊ�յ�����
    }

    // ���캯�����ڳ�ʼ����������
    VarDecl(const std::shared_ptr<BasicType>& type, const std::vector<std::shared_ptr<VarDef>>& defs)
        : type(type), varDefs(defs) {}
};
//9. varDef ->Ident| Ident '=' initVal;
// ���������AST�ڵ���
class VarDef : public Declaration {
public:
    std::string identifier; // ��ʶ���������������
    std::shared_ptr<InitVal> initVal; // ��ʼ��ֵ�ı��ʽ�ڵ㣬����Ϊ��

    // ���캯�����ڳ�ʼ��һ��û�г�ʼֵ�ı�������
    explicit VarDef(const std::string& id)
        : identifier(id), initVal(nullptr) {}

    // ���캯�����ڳ�ʼ��һ�����г�ʼֵ�ı�������
    VarDef(const std::string& id, const std::shared_ptr<InitVal>& value)
        : identifier(id), initVal(value) {}

    // ����������������Ҫ���壬���ʹ������ָ��ͨ�����Բ�����
    virtual ~VarDef() {}
};
//10. initVal -> exp;
// ��ʼ��ֵ(initVal)��AST�ڵ���
class InitVal : public ASTNode {
public:
    std::shared_ptr<Exp> exp; // ���ڳ��б��ʽ�ڵ��ָ��

    // ���캯��������ʼ��initVal�ڵ㣬������һ�����ʽ
    explicit InitVal(const std::shared_ptr<Exp>& expression)
        : exp(expression) {}

   
};
//11. funcDef -> funcType Ident '(' (funcFParams)? ')' block;
// ���������AST�ڵ���
class FuncDef : public BlockItem {
public:
    std::shared_ptr<FuncType> funcType;   // �����������ͽڵ�
    std::string ident;                   // ��������
    std::shared_ptr<FuncFParams> params; // ���������б���ѡ
    std::shared_ptr<Block> body;         // ������
    FuncDef() : funcType(nullptr), ident(""), params(nullptr), body(nullptr) {
        // Ĭ�Ϲ��캯����ʵ�֣�������Ҫ��ʱ����ӣ�
    }
    // ���캯�����ڳ�ʼ����������
    FuncDef(const std::shared_ptr<FuncType>& type,
        const std::string& name,
        const std::shared_ptr<FuncFParams>& parameters,
        const std::shared_ptr<Block>& block)
        : funcType(type), ident(name), params(parameters), body(block) {}

    // ����������������Ҫ���壬���ʹ������ָ��ͨ�����Բ�����
    virtual ~FuncDef() {}
};
//12. funcType -> 'void' | 'int' ;
// ��ʾ�������͵�ö��
enum class FuncTypeEnum {
    Void,
    Integer,
};
// �������͵�AST�ڵ���
class FuncType : public ASTNode {
public:
    FuncTypeEnum type; // ���ڱ��溯�����͵�ö��ֵ

    // ���캯�����ڳ�ʼ���������ͽڵ�
    explicit FuncType(FuncTypeEnum t) : type(t) {}

    // ����ö��type���ض�Ӧ���ַ�����ʾ������ "void" �� "int"
    std::string getTypeString() const {
        switch (type) {
        case FuncTypeEnum::Void: return "void";
        case FuncTypeEnum::Integer: return "int";
        default: return ""; // ��Զ��Ӧ�ôﵽ����
        }
    }

    // ��������...
};

//13. funcFParams -> funcFParam (',' funcFParam)*;
// �����β��б��AST�ڵ���
class FuncFParams : public ASTNode {
public:
    std::vector<std::shared_ptr<FuncFParam>> params; // �洢�����βνڵ�ļ���

    // ���캯����ʼ���β��б�
    FuncFParams() = default;

    // ���һ���βε��б���
    void addParam(const std::shared_ptr<FuncFParam>& param) {
        params.push_back(param);
    }

    // �������ܵķ����ͳ�Ա����...
};
//14. funcFParam -> bType Ident;
// �����βε�AST�ڵ���
class FuncFParam : public ASTNode {
public:
    // ����bType���������
    BasicType type; // ���� std::shared_ptr<bType> type; ���bType��һ����
    std::string ident; // ��������

    // ���캯�����ڳ�ʼ���βνڵ�
    FuncFParam(BasicType t, const std::string& id) // ����ʹ�� std::shared_ptr<bType> t ���bType��һ����
        : type(t), ident(id) {}

    // ����������������Ҫ���壬���ʹ������ָ��ͨ�����Բ�����
    virtual ~FuncFParam() {}

    // �������ܵķ����ͳ�Ա����...
};
//17. stmt ->lVal '=' exp ';'| (exp) ? ';'| block| 'return' (exp) ? ';';
class Stmt : public BlockItem {

};

// ��ֵ���
class AssignStmt : public Stmt {
public:
    std::shared_ptr<LVal> lvalue;
    std::shared_ptr<Exp> expression;

    AssignStmt(std::shared_ptr<LVal> lval, std::shared_ptr<Exp> expr)
        : lvalue(lval), expression(expr) {}
};

// ���ʽ���
class ExprStmt : public Stmt {
public:
    std::shared_ptr<Exp> expression; // ����Ϊ��

    explicit ExprStmt(std::shared_ptr<Exp> expr = nullptr)
        : expression(expr) {}
};

// �����
class BlockStmt : public Stmt {

};

// �������
class ReturnStmt : public Stmt {
public:
    std::shared_ptr<Exp> expression; // ����Ϊ��

    explicit ReturnStmt(std::shared_ptr<Exp> expr = nullptr)
        : expression(expr) {}
};
//15. block -> '{' (blockItem)* '}';
class Block : public Stmt {
public:
    std::vector<std::shared_ptr<BlockItem>> items; // �洢�������Ŀ������

    // ���캯����ʼ�������
    Block() = default;

    // ���һ���������Ŀ�����б�
    void addItem(const std::shared_ptr<BlockItem>& item) {
        items.push_back(item);
    }

    // �������ܵķ����ͳ�Ա����...
};

//18. exp -> addExp;
// ���ʽ��
class Exp : public ASTNode {
public:
    std::shared_ptr<AddExp> addExpression; // �洢һ���ӷ����ʽ

    // ���캯��
    explicit Exp(std::shared_ptr<AddExp> expr)
        : addExpression(expr) {}
};
//19. cond -> lOrExp;
// �������ʽ��
class Cond : public ASTNode {
public:
    std::shared_ptr<LOrExp> lOrExpression; // �洢һ���߼�����ʽ

    // ���캯��
    explicit Cond(std::shared_ptr<LOrExp> expr)
        : lOrExpression(expr) {}
};
//20. lVal -> Ident;
class LVal : public ASTNode {
public:
    std::string identifier; // ���ڴ洢��ʶ������

    // ���캯��
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

    // ��¼ PrimaryExp ������
    ExpressionType type;

    // ��ž�����ӱ��ʽ
    std::shared_ptr<Exp> expression; // ��� type �� EXPRESSION
    std::shared_ptr<LVal> lValue; // ��� type �� LVAL
    std::shared_ptr<Number> number; // ��� type �� NUMBER

    // ���캯�������ݲ�ͬ���ͣ�������Ӧ�����ݽṹ
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
    int value; // �����洢��������

    // ���캯��
    explicit Number(int val) : value(val) {}

    // getters, setters, �����������������Ҫ��
};
//23. unaryExp ->primaryExp| Ident '(' (funcRParams) ? ')'| unaryOp unaryExp;
class UnaryExp : public ASTNode {
public:
    enum class Type {
        PrimaryExp,
        FunctionCall,
        UnaryOperation
    };

    Type type; // ��ʾUnaryExpʵ�ʵ�����

    // ����PrimaryExp����
    std::shared_ptr<PrimaryExp> primaryExp;

    // ����FunctionCall����
    std::string ident;
    std::shared_ptr<FuncRParams> funcRParams;

    // ����UnaryOperation����
    std::shared_ptr<UnaryOp> unaryOp;
    std::shared_ptr<UnaryExp> operand;

};
//24. unaryOp -> '+' | '-' | '!';
class UnaryOp : public ASTNode {
public:
    char op; // ���ڴ洢һԪ���������ַ�

    // ���캯������һ����ʾһԪ���������ַ�
    explicit UnaryOp(char operation)
        : op(operation) {}

    // �����߼��ͷ���
};
//25. funcRParams -> funcRParam (',' funcRParam)*;
class FuncRParams : public ASTNode {
public:
    std::vector<std::shared_ptr<FuncRParam>> params; // �洢 FuncRParam ʵ��������

    // ���캯��
    FuncRParams() {}

    // ������б�������µ� FuncRParam
    void addParam(const std::shared_ptr<FuncRParam>& param) {
        params.push_back(param);
    }

    // �������ܵ��߼��ͷ���
};
//26. funcRParam -> exp;
class FuncRParam : public ASTNode {
public:
    std::shared_ptr<Exp> expression; // �����������ʽ�ĳ�Ա

    // ���캯������һ�����ʽ�ڵ㣬�����䱣��Ϊʵ��
    explicit FuncRParam(std::shared_ptr<Exp> exp)
        : expression(exp) {}

    // �����߼��ͷ���
};
//27. mulExp ->unaryExp| mulExp('*' | '/' | '%') unaryExp;
class MulExp : public ASTNode {
public:
    std::shared_ptr<MulExp> left;     // �����ʽ�������� MulExp �� UnaryExp
    std::shared_ptr<UnaryExp> right;    // �Ҳ���ʽ���� UnaryExp
    char operation; // �洢��������'*'��'/' �� '%'

    // ���캯�������ڵ��� UnaryExp
    explicit MulExp(std::shared_ptr<UnaryExp> expr) : left(nullptr), right(expr), operation(0) {}

    // ���캯���������������ʽ�ĳ˷���������ȡģ
    MulExp(std::shared_ptr<MulExp> left, char op, std::shared_ptr<UnaryExp> right)
        : left(left), right(right), operation(op) {}

    // �����߼��ͷ���...
};
//28. addExp -> mulExp # add1 | addExp ('+' | '-') mulExp;
class AddExp : public ASTNode {
public:
    std::shared_ptr<AddExp> left;  // ��ߵı��ʽ�ڵ㣬������AddExp��MulExp
    std::shared_ptr<MulExp> right; // �ұߵĳ˷����ʽ�ڵ�
    char op;                        // ��������'+' �� '-'

    // ����Ǽ򵥵ĳ˷����ʽ
    explicit AddExp(std::shared_ptr<MulExp> simpleExp)
        : left(nullptr), right(simpleExp), op('\0') {}

    // ��������һ���ӷ����ʽ���ұ��ǳ˷����ʽ
    AddExp(std::shared_ptr<AddExp> left, char op, std::shared_ptr<MulExp> right)
        : left(left), right(right), op(op) {}

    // �����������Ӹ���ĺ������߼�������������ʽ���ķ�����
};
//29. relExp ->addExp| relExp('<' | '>' | '<=' | '>=') addExp;
class RelExp : public ASTNode {
public:
    std::shared_ptr<ASTNode> left;  // �����ʽ������ΪRelExp��AddExp
    std::shared_ptr<AddExp> right; // �Ҳ���ʽ������һ��AddExp
    std::string op;                 // ��ϵ������ '<', '>', '<=', '>='

    // �����ڵ����� AddExp ����
    explicit RelExp(std::shared_ptr<AddExp> right)
        : left(nullptr), right(right), op("") {}

    // ����������� RelExp ������
    RelExp(std::shared_ptr<RelExp> left, const std::string& op, std::shared_ptr<AddExp> right)
        : left(left), right(right), op(op) {}

    // ����������� AddExp ������
    RelExp(std::shared_ptr<AddExp> left, const std::string& op, std::shared_ptr<AddExp> right)
        : left(left), right(right), op(op) {}

    // �����߼��ͷ���
};
//30. eqExp ->relExp| eqExp('==' | '!=') relExp;
class EqExp : public ASTNode {
public:
    std::shared_ptr<EqExp> left;  // �����ʽ������Ϊ��һ��EqExp��nullptr
    std::shared_ptr<RelExp> right; // �Ҳ���ʽ������һ��RelExp
    std::string op;                 // ��������'==' �� '!='

    // ���캯���������ڴ�RelExp���
    explicit EqExp(std::shared_ptr<RelExp> right)
        : left(nullptr), right(right), op("") {}

    // ���캯���������� 'eqExp == relExp' �� 'eqExp != relExp' ���
    EqExp(std::shared_ptr<EqExp> left, const std::string& op, std::shared_ptr<RelExp> right)
        : left(left), right(right), op(op) {}

    // �����߼��ͷ���
};
//31. lAndExp ->eqExp| lAndExp '&&' eqExp;
class LAndExp : public ASTNode {
public:
    std::shared_ptr<LAndExp> left;  // ��࣬��������һ��LAndExp
    std::shared_ptr<EqExp> right; // �Ҳ࣬����һ��EqExp
    bool isPureEqExp;               // �Ƿ񴿴��EqExp

    // ���캯��: �����ڴ�EqExp���
    explicit LAndExp(std::shared_ptr<EqExp> right)
        : left(nullptr), right(right), isPureEqExp(true) {}

    // ���캯��: ������'lAndExp && eqExp'���
    LAndExp(std::shared_ptr<LAndExp> left, std::shared_ptr<EqExp> right)
        : left(left), right(right), isPureEqExp(false) {}

    // �����߼��ͷ���
};
//32. lOrExp ->lAndExp| lOrExp '||' lAndExp;
class LOrExp : public ASTNode {
public:
    std::shared_ptr<LOrExp> left; // ��ߵı��ʽ�ڵ�
    std::shared_ptr<LAndExp> right; // �ұߵı��ʽ�ڵ�

    // ����Ǽ򵥵� lAndExp
    explicit LOrExp(std::shared_ptr<LAndExp> landExp)
        : left(nullptr), right(landExp) {}

    // LOrExp ������Ҫһ�������������г�ʼ��
    LOrExp(std::shared_ptr<LOrExp> leftExp, std::shared_ptr<LAndExp> rightExp)
        : left(leftExp), right(rightExp) {}
    // ����������������Ա�������߼�
};
//33. constExp -> addExp;
class ConstExp : public ASTNode {
public:
    std::shared_ptr<AddExp> addExp; // ָ��ӷ����ʽ��ָ��

    explicit ConstExp(std::shared_ptr<AddExp> expr) : addExp(expr) {}

    // �������Ҫ��������������Ӷ�����߼��򷽷�
};
//34. IntConst -> [0-9]+ ;
class IntConst : public ASTNode {
public:
    int value; // �洢����������ֵ

    explicit IntConst(int value) : value(value) {}

    // ������Ը�����Ҫ��ӳ�Ա����
    // ���緵������������ֵ��
};
//35. Ident -> [a-zA-Z_][a-zA-Z_0-9]*;
class Ident : public ASTNode {
public:
    std::string name; // �洢��ʶ�����ַ���s

    explicit Ident(const std::string& name) : name(name) {}

};


#endif // AST_H