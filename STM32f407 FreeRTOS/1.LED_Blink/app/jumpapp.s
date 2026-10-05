 AREA  |.text|,CODE,READONLY
 THUMB
 PRESERVE8

	EXPORT JumpAPP

JumpAPP         PROC
                LDR  SP,[R0,#0]
                LDR  PC,[R0,#4]
                ENDP

    END