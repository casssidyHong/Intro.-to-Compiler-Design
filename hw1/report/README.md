# hw1 report

|Field|Value|
|-:|:-|
|Name| 洪巧芸 |
|ID| 110550143 |

## How much time did you spend on this project

3 days. 

## Project overview

％{

### Definitions Section
- 引用要用到的函式庫<stdint.h>, <stdio.h>, <stdlib.h>
- 定義東西
    - LIST_SOURCE: 把字(yytext)加進去currentline裡
    - LIST_TOKEN: 開啟列印token模式時印出符號 <>
    - LIST_RESERVED: 開啟列印token模式時印出reserved word <KW___>
    - LIST_LITERAL: 開啟列印token模式時印出 <type: ___>
    - MAX_LINE_LENG: 最大長度為512
- 宣告其他會在下面用到的東西, 並初始化 ex.line_num, opt_scr...
- 宣告函示appendToCurrentLine

%}

- 把需要在下面用到的同一種類的東西包再一起
    ex. 出現什麼字詞時可以用Reserved代替 ex. var, def...都屬於reserved
- 還有宣告下面要用到的state名稱
    ex. COMMENT

％％

### Reules Section (收到每個不一樣的字符串要做什麼事情)
- 換行符號 -> (opt_src=1)時印出整行文字及符號
- 要改變模式的符號 -> 改變opt_src或是opt_tok
- 單行註解 -> "//"出現時
- 多行註解 -> "/**/“出現時 需要考慮到因為是多行註解而且有固定的結尾，所以用State去寫可以考慮後面出現換行等情況
- Delimiter -> 避免程式的讀取困難，後續需要校驗，所以我用手刻的
- Operator -> 避免程式的讀取困難，後續需要校驗，所以我用手刻的
- Reserved -> 配合上一個部分宣告的東西，只要出現在Reserved裡的字元就以統一的格式印出
- Scientfic -> 配合上一個部分宣告的東西，只要出現在Scientific裡的字元就以統一的格式印出
- Identifier -> 配合上一個部分宣告的東西，只要出現在Identifier裡的字元就以統一的格式印出
- Intiger -> 配合上一個部分宣告的東西，是符合Int可以組合而成的東西就以桶椅的格式印出
- Octal Integer -> 配合上一個部分宣告的東西，是符合Oct_int可以組合而成的東西就以桶椅的格式印出
- Float -> 配合上一個部分宣告的東西，是符合Float可以組合而成的東西就以桶椅的格式印出
- 模式改變 -> 因為上面的模式改變只有考慮到第一組出現的字元是否為要求改變模式，因此在這裡多加一個以因應模式改變符號出現在字串結尾的情況
- 吃掉空白鍵 -> 掃描時要忽略的情況 ex. 空白鍵, tab
- 讀取string -> 用一個一個字符串輸入進string, 避免有""印出兩次的情形, 並用times將"兩兩綁為一組, 出現為奇數次時才輸入string裡
- 抓取錯誤的符號

%%

### User Code Section

## What is the hardest you think in this project

The unfamiliar of the typing of regular definitions.
By the way, reading and understanding the request of homework also took a lot of time.

## Feedback to T.A.s

> Please help us improve our assignment, thanks.
