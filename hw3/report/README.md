# hw3 report

|      |           |
| ---: | :-------- |
| Name | 洪巧芸    |
|   ID | 110550143 |

## How much time did you spend on this project

> 5 days, about 5-7 hours per day.

## Project overview

### 流程：
1. 先照著hint打 case 1_program -> 初步了解各個檔案間的關係，知道誰會牽動到誰
1. 開始寫 case 2_declaration (decl.hpp -> decl.cpp)
    - 照著Todo, 先增加public與private中的宣告  
    - 把 void accept 與 void visitChildNodes 加上  
    - 把.cpp中的Node初始化調整成與引用到的.hpp相同
    - 遍歷 declNode的所有子節點（調用 AstNodeVisitor 的 accept 函數）
    - 去新增 parser.y 中的參數與規則
1. 改完所有 include/ ~.hpp, lib/ ~.cpp, parser.y 即可

### 程式概要：
1. .hpp中private與public中構造函數所用到的變數型態與名稱均相同
1. 除了 ast.hpp, astdumper.hpp 兩個基本建構檔案(設定會使用到的參數\各個檔案的visit函數呼叫)之外都會調用 accept 與 visitChildNodes 兩個 function 去遍歷當前的所有子節點。
1. 而 BinaryOperator, ConstantValue, function, functionInvocation, program, UnaryOperator, Variable, VariableReference 則多呼叫了額外的 get function 去獲取需要的資訊(ex. operator_type等)
1. 在 parser.y 中新增 Yacc 的 token 類型(%union)及其他會在後面規則中生成的值的類型(%type)，並運用在後面的 rules 協助建立對應的 Ast 節點。而最後就是各個規則的設立。  

## What is the hardest you think in this project
> 最一開始在讀spec的時候就花了很久去釐清每個檔案之間的關係，
寫的時候也很容易因為一個相關的檔沒有改到或是在parser中忘記宣告後續使用的東西就導致程式不能運作，
因此覺得有耐心在這次的作業中是很重要的角色。

## Feedback to T.A.s

> Thank you for going through this report!
