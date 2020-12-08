//
// version
//
        .vers   4

//
// execution framework
//
__start:
        call    _main
        call    _exit
__stop:
        jmp     __stop

//
// Integer readInteger()
//
_readInteger:
        asf     0
        rdint
        popr
        rsf
        ret

//
// void writeInteger(Integer)
//
_writeInteger:
        asf     0
        pushl   -3
        wrint
        rsf
        ret

//
// Character readCharacter()
//
_readCharacter:
        asf     0
        rdchr
        popr
        rsf
        ret

//
// void writeCharacter(Character)
//
_writeCharacter:
        asf     0
        pushl   -3
        wrchr
        rsf
        ret

//
// Integer char2int(Character)
//
_char2int:
        asf     0
        pushl   -3
        popr
        rsf
        ret

//
// Character int2char(Integer)
//
_int2char:
        asf     0
        pushl   -3
        popr
        rsf
        ret

//
// void exit()
//
_exit:
        asf     0
        halt
        rsf
        ret

//
// void main()
//
_main:
        asf     4
        pushc   0
        popl    2
        pushc   0
        popl    3
        call    _readInteger
        pushr
        popl    0
        call    _readInteger
        pushr
        popl    1
        jmp     __2
__1:
        pushl   3
        pushl   1
        add
        popl    3
        pushl   2
        pushc   1
        add
        popl    2
__2:
        pushl   2
        pushl   0
        lt
        brt     __1
__3:
        pushl   1
        call    _writeInteger
        drop    1
__0:
        rsf
        ret
        