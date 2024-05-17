# hw5 report

|||
|-:|:-|
|Name|洪巧芸|
|ID|110550143|

## How much time did you spend on this project

3days, about 5-6 hours per day

## Project overview

> Please describe the structure of your code and the ideas behind your implementation in an organized way.
> The point is to show us how you deal with the problems. It is not necessary to write a lot of words or paste all of your code here.
> The report should describe your feedback about hw5 For example, is the spec and the tutorial clear? Is hw5 too easy?

因為在另外一堂課中有在打8051組合語言，所以理解這次的作業所花的時間比前兩次少很多。  

1. nodes: 一般來說每個節點都有基本的 assembly code。一些比較特殊的情況，有些節點會增加額外的 assembly code，或是與每個節點的一般代碼不同的代碼。
1. local variables: 在函數中使用的 local variables，它的值會被推送到 stack 的某個特定位置，且在 scope 內這個位置不會改變。
1. expressions: 在計算後將它的值推送到 stack 上，其父節點可以利用之前推送的值或表達式結果來完成表達式值的計算。
1. loops and condtions: 要生成一些新的標籤，存儲在 true / false stack 中，才能在知道條件結果後代碼應該跳轉到哪個標籤。

## What is the hardest you think in this project

> Not required, but bonus point may be given.

Conditional Statement 還有 loop 的部分是我覺得最難的地方，詢問了好幾個同學才寫出來。

## Feedback to T.A.s

> Not required, but bonus point may be given.

辛苦TA們這一學期的作業批改了，還有在同學有問題時及時回覆，這是一門很值得修的課～