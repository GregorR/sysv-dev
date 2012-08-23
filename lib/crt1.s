.global _start
_start:
    push   $0x0
    push   $0x0
    mov    %esp,%ebp
    push   %edx
    mov    $_cleanup,%eax
    test   %eax,%eax
    je     .L1
    push   $_cleanup
    call   atexit
    add    $0x4,%esp
    .L1:
    push   $_fini
    call   atexit
    mov    0x8(%ebp),%eax
    lea    0x10(%ebp,%eax,4),%edx
    mov    %edx,environ
    push   %edx
    lea    0xc(%ebp),%edx
    push   %edx
    push   %eax
    call   _init
    call   __fpstart
    call   main
    add    $0xc,%esp
    push   %eax
    call   exit
    push   $0x0
    mov    $0x1,%eax
    lcall  $0x7,$0x0
    hlt    
    ret    

.comm environ,4,4
