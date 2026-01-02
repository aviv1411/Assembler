
;Error - text after macro name definition
mcro m 3

;Invalid macro name (Reserved language name) 
mcro mov

;Error - macro declared without a name
mcro

;Invalid macro name (contains prohibited characters)
mcro ###


;mcroend contains additional unexpected text
mcro m
mcroend kkk

