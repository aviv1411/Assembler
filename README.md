\# Two-Pass Assembler in ANSI-C (Course Assignment Submission)



This repository contains our \*\*two-pass assembler\*\* implementation in \*\*ANSI C\*\*, written as part of a university programming assignment. The assembler receives one or more assembly source files (`.as`), runs a \*\*macro preprocessor\*\* to generate `.am`, performs \*\*first + second pass\*\* encoding with symbol resolution, and outputs `.ob` (and `.ent` / `.ext` when relevant).



\## What the program does



For each input file `<name>.as`:



1\. \*\*Preprocessor (macros)\*\*  

&nbsp;  Expands `mcro ... mcroend` definitions and replaces macro calls, producing `<name>.am`.



2\. \*\*First pass\*\*  

&nbsp;  - Parses each line (instructions / directives / labels).  

&nbsp;  - Builds the \*\*symbol table\*\*.  

&nbsp;  - Computes addresses and prepares initial encoding images.



3\. \*\*Second pass\*\*  

&nbsp;  - Resolves symbols (including extern/entry handling).  

&nbsp;  - Finalizes instruction/data encoding.  

&nbsp;  - Collects extern usages and entry declarations.



4\. \*\*Outputs\*\*

&nbsp;  - `<name>.ob` — object file  

&nbsp;  - `<name>.ent` — entry symbols (only if `.entry` exists)  

&nbsp;  - `<name>.ext` — external symbol usages (only if `.extern` symbols are used)



\## Supported language (as required by the assignment)



\### Directives

\- `.data` (integers)

\- `.string` (quoted string)

\- `.entry <label>`

\- `.extern <label>`



\### Instructions (16)

`mov, cmp, add, sub, lea, clr, not, inc, dec, jmp, bne, jsr, red, prn, rts, stop`



Operand counts:

\- 2 operands: `mov, cmp, add, sub, lea`

\- 1 operand: `clr, not, inc, dec, jmp, bne, jsr, red, prn`

\- 0 operands: `rts, stop`



\### Addressing modes used

\- Immediate: `#number`

\- Direct: `LABEL`

\- Relative: `\&LABEL`

\- Registers: `r0` … `r7`



\## Project structure





