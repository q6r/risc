#!/bin/sh
DESC="DEC r7-r8"
CODE="a802a803a804a805a806a807a808a809"
EXPECT="process_dec >>>>> dec r1
-----------------------------------------------------------
| pc = 00000001 ps = 00000000 r1 = ffffffff r2 = 00000000 |
| r3 = 00000000 r4 = 00000000 r5 = 00000000 r6 = 00000000 |
| r7 = 00000000 r8 = 00000000 ........................... |
-----------------------------------------------------------
|> No stack
process_dec >>>>> dec r2
-----------------------------------------------------------
| pc = 00000003 ps = 00000000 r1 = ffffffff r2 = ffffffff |
| r3 = 00000000 r4 = 00000000 r5 = 00000000 r6 = 00000000 |
| r7 = 00000000 r8 = 00000000 ........................... |
-----------------------------------------------------------
|> No stack
process_dec >>>>> dec r3
-----------------------------------------------------------
| pc = 00000005 ps = 00000000 r1 = ffffffff r2 = ffffffff |
| r3 = ffffffff r4 = 00000000 r5 = 00000000 r6 = 00000000 |
| r7 = 00000000 r8 = 00000000 ........................... |
-----------------------------------------------------------
|> No stack
process_dec >>>>> dec r4
-----------------------------------------------------------
| pc = 00000007 ps = 00000000 r1 = ffffffff r2 = ffffffff |
| r3 = ffffffff r4 = ffffffff r5 = 00000000 r6 = 00000000 |
| r7 = 00000000 r8 = 00000000 ........................... |
-----------------------------------------------------------
|> No stack
process_dec >>>>> dec r5
-----------------------------------------------------------
| pc = 00000009 ps = 00000000 r1 = ffffffff r2 = ffffffff |
| r3 = ffffffff r4 = ffffffff r5 = ffffffff r6 = 00000000 |
| r7 = 00000000 r8 = 00000000 ........................... |
-----------------------------------------------------------
|> No stack
process_dec >>>>> dec r6
-----------------------------------------------------------
| pc = 0000000b ps = 00000000 r1 = ffffffff r2 = ffffffff |
| r3 = ffffffff r4 = ffffffff r5 = ffffffff r6 = ffffffff |
| r7 = 00000000 r8 = 00000000 ........................... |
-----------------------------------------------------------
|> No stack
process_dec >>>>> dec r7
-----------------------------------------------------------
| pc = 0000000d ps = 00000000 r1 = ffffffff r2 = ffffffff |
| r3 = ffffffff r4 = ffffffff r5 = ffffffff r6 = ffffffff |
| r7 = ffffffff r8 = 00000000 ........................... |
-----------------------------------------------------------
|> No stack
process_dec >>>>> dec r8
-----------------------------------------------------------
| pc = 0000000f ps = 00000000 r1 = ffffffff r2 = ffffffff |
| r3 = ffffffff r4 = ffffffff r5 = ffffffff r6 = ffffffff |
| r7 = ffffffff r8 = ffffffff ........................... |
-----------------------------------------------------------
|> No stack"

run_test
