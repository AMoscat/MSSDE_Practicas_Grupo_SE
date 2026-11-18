
 
    ; Hardware Abstraction Layer for the 33E family
    
    .include "p33Exxxx.inc"
    
    ; Variables used by the scheduler
    .extern task_stack_address
    .extern temp_W0
    .extern task_code_address_page
    .extern task_code_address_offset
    .extern new_task_stack_pointer
    .extern temp_stack_pointer
    .extern adr_current_task_stack_pointer
    ; ------------------------------------------
    
    .global _asmStackFrame
    
    .global _TSlock
    .global _TSunlock
    
    
    ; Creates first stack frame of a task with current registers data
_asmStackFrame: 
    ; Save current stack pointer and W0 register
    MOV	    W15, _temp_stack_pointer
    MOV	    W0, _temp_W0
    
    ; Change stack to the task stack
    MOV	    _task_stack_address, W15
    
    ; Push task offset address of code
    MOV	    _task_code_address_offset, W0
    PUSH    W0
    
    ; Push initial SR<7:0> IPL3 PC<22:16> = tblpage (code page address)
    ; If another initial priority is needed then W0<15:13> must be used
    ; to contain initial IPL<2:0>
    MOV	    _task_code_address_page, W0
    AND.W   #0x007F, W0
    PUSH    W0
    
    ; Push registers
    MOV	    _temp_W0, W0    ; Recover W0
    
    PUSH    SR
    PUSH    W0
    PUSH    W1
    PUSH.D  W2
    PUSH.D  W4
    PUSH.D  W6
    PUSH.D  W8
    PUSH.D  W10
    PUSH.D  W12
    PUSH    W14
    PUSH    RCOUNT
    PUSH    TBLPAG
    PUSH    ACCAL
    PUSH    ACCAH
    PUSH    ACCAU
    PUSH    ACCBL
    PUSH    ACCBH
    PUSH    ACCBU
    PUSH    DCOUNT
    PUSH    DOSTARTL
    PUSH    DOSTARTH
    PUSH    DOENDL
    PUSH    DOENDH
    PUSH    CORCON
    PUSH    DSRPAG
    PUSH    DSWPAG
    
    ; Comunicate task stack pointer
    MOV	    W15, _new_task_stack_pointer
    
    ; Recover initial stack pointer
    MOV	    _temp_stack_pointer, W15
    
    return
    
      
    ; This saves context of current running task, then call scheduler and
    ; prepare context of the selected task to run
__T1Interrupt:
    ; Return addres is in stack + low byte of SR + IPL3
    
    BCLR    IFS0, #T1IF	    ; Clears the interrupt flag
    PUSH    SR
    PUSH    W0
    PUSH    W1
    PUSH.D  W2
    PUSH.D  W4
    PUSH.D  W6
    PUSH.D  W8
    PUSH.D  W10
    PUSH.D  W12
    PUSH    W14
    PUSH    RCOUNT
    PUSH    TBLPAG
    PUSH    ACCAL
    PUSH    ACCAH
    PUSH    ACCAU
    PUSH    ACCBL
    PUSH    ACCBH
    PUSH    ACCBU
    PUSH    DCOUNT
    PUSH    DOSTARTL
    PUSH    DOSTARTH
    PUSH    DOENDL
    PUSH    DOENDH
    PUSH    CORCON
    PUSH    DSRPAG
    PUSH    DSWPAG   
    
    ; Save old task stack pointer
    MOV	    _adr_current_task_stack_pointer, W0
    MOV	    W15, [W0]
    
    CALL    _count_tic
    CALL    _scheduler
    
    ; Charge new task stack pointer    
    MOV	    _adr_current_task_stack_pointer, W0
    MOV	    [w0], W15
    
    POP	    DSWPAG
    POP	    DSRPAG
    POP	    CORCON
    POP	    DOENDH
    POP	    DOENDL
    POP	    DOSTARTH
    POP	    DOSTARTL
    POP	    DCOUNT
    POP	    ACCBU
    POP	    ACCBH
    POP	    ACCBL
    POP	    ACCAU
    POP	    ACCAH
    POP	    ACCAL
    POP	    TBLPAG
    POP	    RCOUNT
    POP	    W14
    POP.D   W12
    POP.D   W10
    POP.D   W8
    POP.D   W6
    POP.D   W4
    POP.D   W2
    POP.D   W0
    POP	    SR

    RETFIE

    ; This saves context of current running task, then call scheduler and
    ; prepare context of the selected task to run
__INT2Interrupt:   
    ; Return addres is in stack + low byte of SR + IPL3
    
    BCLR    IFS1, #INT2IF	    ; Clears the interrupt flag
    PUSH    SR
    PUSH    W0
    PUSH    W1
    PUSH.D  W2
    PUSH.D  W4
    PUSH.D  W6
    PUSH.D  W8
    PUSH.D  W10
    PUSH.D  W12
    PUSH    W14
    PUSH    RCOUNT
    PUSH    TBLPAG
    PUSH    ACCAL
    PUSH    ACCAH
    PUSH    ACCAU
    PUSH    ACCBL
    PUSH    ACCBH
    PUSH    ACCBU
    PUSH    DCOUNT
    PUSH    DOSTARTL
    PUSH    DOSTARTH
    PUSH    DOENDL
    PUSH    DOENDH
    PUSH    CORCON
    PUSH    DSRPAG
    PUSH    DSWPAG
    
    ; Save old task stack pointer
    MOV	    _adr_current_task_stack_pointer, W0
    MOV	    W15, [W0]
    
    CALL    _scheduler
    
    ; Charge new task stack pointer    
    MOV	    _adr_current_task_stack_pointer, W0
    MOV	    [w0], W15
    
    POP	    DSWPAG
    POP	    DSRPAG
    POP	    CORCON
    POP	    DOENDH
    POP	    DOENDL
    POP	    DOSTARTH
    POP	    DOSTARTL
    POP	    DCOUNT
    POP	    ACCBU
    POP	    ACCBH
    POP	    ACCBL
    POP	    ACCAU
    POP	    ACCAH
    POP	    ACCAL
    POP	    TBLPAG
    POP	    RCOUNT
    POP	    W14
    POP.D   W12
    POP.D   W10
    POP.D   W8
    POP.D   W6
    POP.D   W4
    POP.D   W2
    POP.D   W0
    POP	    SR

    RETFIE
    
    
    .end

