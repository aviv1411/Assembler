;Missing comma bitween two operands
mov r3 r4

;Label to long
mainnnnnnnnnnnnnnnvvvvvvvnnnnnkkkkkkk: mov r1, r2

;Label contains prohibited characters
la!bel: stop

;Opcode is illegal
goto r1

;Illeagal comma ',' in .data input
.data 4, 5, ,3

;Comma ',' not allowed in end of .data input
.data 4, 5, 3,

;Not a number in .data input
.data 4, 5, f

;Missing comma between different numbers in .data input
.data 4 5

;Missing " at end of string
.string "jdj

;Missing string after DIRECTIVE .string
.string 

;Missing first operand
clr

;Missing lebel name after DIRECTIVE .extern
.extern

;Missing second operand
mov r3,

;Illegal adressing type for source operand (by opcode)
lea #3, r1

;Illegal adressing type for destination operand (by opcode)
mov #3, #3

;Invalid label name (Reserved name)
mov: not r3

;No space bitween two fields
js:.data 2

;Label in empty sentence
empty: 
