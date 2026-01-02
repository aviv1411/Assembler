;redefinition of label
label1: mov #1 ,r1
label1: .data 1


;undefined label - direct
lea HELLO, r1


;undefined label - relative
jmp &Line1

;undefined label - relative(extern label)
.extern Line2
jmp &Line2

;redefinition of label type
.entry label2
.entry label2

;declared entry but was never defined
.entry label3