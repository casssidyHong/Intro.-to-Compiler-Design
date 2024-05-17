# hw4 report

|||
|-:|:-|
|Name|洪巧芸|
|ID|110550143|

## How much time did you spend on this project

4days, about 6-8hours/day.

## Project overview

大致方式：倚賴AST Dumper提供的結構和訊息進行語義檢查和錯誤處理，所以在AST遍歷完時錯誤訊息也會剛好偵測完  
Semantic Analyzer會遍歷所有nodes然後去尋找錯誤訊息是否存在  
我把semantic analyzer拆分成兩個部分：  
1. Symbol  
    2. 負責檢查每個的error type，然後維持symbol table  
    2. 在AST遍歷時即時進行錯誤訊息的檢查
1. Error Message  
    3. 負責處理error message，然後維持並儲存error message
    3. 當Symbol偵測到錯誤訊息時及時進行儲存


## What is the hardest you think in this project

1. 這次的程式碼可以說是有史以來最多的，需要自己去考慮錯誤訊息的儲存方式與輸出，很容易打到後面就忘記前面自己打了什麼，然後亂掉。
1. 有時候在本地的local跑是對的，但是git push上去會報錯，最後請教同學才找到錯誤的地方。

## Feedback to T.A.s

> Not required, but bonus point may be given.
