@ECHO OFF

@del  *_ADAMS.msg
@del  *_ADAMS.out
@del  *_ADAMS_LIN.msg
@del  *_ADAMS_LIN.gra
@del  *_ADAMS_LIN.res

@move  CertTest.out           tstfiles

@move Test*.sts               tstfiles
@move Test*.azi               tstfiles
@move Test*.pmf               tstfiles

@move Test*.sum               tstfiles
@move Test*.outb              tstfiles
@move Test*.out               tstfiles
