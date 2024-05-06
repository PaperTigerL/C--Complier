
# C-- Compiler

## 项目概述

C-- Compiler是一个编译器项目，旨在将C--语言的源代码转换为可执行的机器代码。该项目由词法分析器、语法分析器和中间代码生成器三个主要组件组成。

## 目录结构

```
C--Compiler/
│
├── src/                     # 源代码目录
│   ├── lexer/               # 词法分析器文件
│   │   ├── Lexer.cpp       # 词法分析器实现（C++）
│   │   └── LexerTest.cpp   # 词法分析器测试用例（C++）
│   │
│   ├── parser/              # 语法分析器文件
│   │   ├── Parser.cpp      # 语法分析器实现（C++）
│   │   └── ParserTest.cpp  # 语法分析器测试用例（C++）
│   │
│   ├── ir/                  # 中间代码生成文件
│   │   ├── IRGenerator.cpp  # IR生成器实现文件（C++）
│   │   └── IRGeneratorTest.cpp # IR生成器测试用例（C++）
│   │
│   └── main.cpp             # 编译器的主程序入口（C++）
│
├── include/                 # 头文件目录
│   ├── Lexer.h
│   ├── Token.h
│   ├── Parser.h
│   ├── Ast.h
│   └── IRGenerator.h
│
├── lib/                     # 项目依赖库目录（比如LLVM库文件）
│
├── tests/                   # 测试目录（用于存放测试用例）
│   ├── LexerTest.cpp        # 词法分析器的测试用例（C++）
│   ├── ParserTest.cpp       # 语法分析器的测试用例（C++）
│   └── IRGeneratorTest.cpp  # 中间代码生成的测试用例（C++）
│
├── build/                   # 构建输出目录
│
├── Makefile                 # 用于编译项目的Makefile（C++）
└── README.md                # 项目的说明文档
```

## 构建和运行

1. **依赖**：确保已安装所需的编译器和库（如LLVM）。
2. **构建**：在终端中进入项目根目录，运行`make`命令来构建项目。这将在`build`目录中生成可执行文件。
3. **运行**：在终端中进入`build`目录，运行`./C--Compiler <input_file>`来编译指定的C--源文件。生成的机器代码将保存在与源文件相同的目录中。

## 测试

在`tests`目录下提供了词法分析器、语法分析器和中间代码生成器的测试用例。您可以使用以下命令来运行测试：

- **词法分析器测试**：`cd tests && make LexerTest`。
- **语法分析器测试**：`cd tests && make ParserTest`。
- **中间代码生成器测试**：`cd tests && make IRGeneratorTest`。

## 文档和资源

- [C--语言规范](<<<unknown_1>>> - C--语言的详细规范。
- [LLVM文档](<<<unknown_2>>> - LLVM项目的官方文档，提供了有关LLVM IR的更多信息。
- [编译原理](<<<unknown_3>>> - 关于编译器构造的一般信息。

## 贡献指南

欢迎您为该项目做出贡献！请查看[贡献指南](CONTRIBUTING.md)以了解更多信息。

## 许可证

该项目使用[MIT许可证](LICENSE)。


### 词法分析器
目标：编写C--语言的词法分析器，输出单词的二元属性，并填写符号表。
大致框架：
1. 定义一个Tokenizer/Lexer类或函数，用于读取C--源代码，识别并输出单词符号的二元属性。
2. 定义一个符号表数据结构，所有识别出的标识符都将被添加到此表中。
3. 实现一个函数，根据C--语言的词汇规则识别出不同分类的符号，例如关键字（KW）、运算符（OP）、界符（SE）、标识符（IDN）、整数（INT）等。
4. 实现一个函数来规范和格式化输出识别到的单词符号序列。
### 语法分析器
目标：编写C--语言的语法分析器，输出最左推导或规范规约产生的语法树所用的产生式序列。
大致框架：
1. 定义一个Parser类或函数，处理由词法分析器输出的单词符号序列。
2. 采用递归下降分析、预测分析表、LL(1)分析等方法实现语法分析。
3. 实现不同的函数对应于文法的不同规则，如funcDef、exp、stmt等。
4. 在处理推导或归约过程中，当成功应用一个产生式时，输出或记录该产生式，以便构建语法树或输出产生式序列。
5. 检查输入的单词符号序列是否合法，并输出正确或错误的判别结果。
### 中间代码生成
目标：补全中间代码生成部分代码，将编译器前端与中端衔接，输出LLVM IR中间代码。
大致框架：
1. 定义一个或多个IR生成函数或类，与语法分析器联动，遍历语法树。
2. 对于语法分析阶段识别的每个构造（如表达式、语句、函数定义等），编写相应的代码逻辑来生成等价的LLVM IR代码。
3. 输出LLVM IR中间代码，包括全局声明、函数声明与定义等。
4. 确保生成的中间代码格式正确，能够被LLVM工具链进一步处理或执行。
在实际开发过程中，这些部分会离不开数据结构（如树、列表等）、算法（如递归下降、表驱动）、以及深入理解C--语言的语法规则和LLVM IR的语法。此外，错误处理和调试信息的输出也是编译器开发中重要的组成部分。
def go(self, I, X):
    print(f"Initial Item Set (I): {I}")
    
    J = set()
    for (lhs, rhs, lookahead) in I:
        dot_position = rhs.find('•')
        print(f"Dot position for ({lhs}, {rhs}, {lookahead}): {dot_position}")
        
        if dot_position != -1 and dot_position + 1 < len(rhs):
            next_symbol = rhs[dot_position + 1]  # 获取点号后的下一个符号
            print(f"Next symbol after dot for ({lhs}, {rhs}, {lookahead}): {next_symbol}")
            
            if next_symbol == X:
                new_rhs = rhs[:dot_position] + X + '•' + rhs[dot_position + len(X):]
                print(f"Adding new rule before closure: ({lhs}, {new_rhs}, {lookahead})")
                J.add((lhs, new_rhs, lookahead))
    
    print(f"New Item Set (J) before closure: {J}")
    closure_result = self.compute_closure(J)
    print(f"Closure result: {closure_result}")
    return closure_result