##############################################################################
##
##  API Extention to Measure time slept.
##
##  Microsoft Research Detours Package
##
##  Copyright (c) Microsoft Corporation.  All rights reserved.
##

!include ..\common.mak

LIBS=$(LIBS) kernel32.lib

##############################################################################

all: dirs \
    $(BIND)\zsub$(DETOURS_BITS).dll \
    \
!IF $(DETOURS_SOURCE_BROWSING)==1
    $(OBJD)\zsub$(DETOURS_BITS).bsc \
!ENDIF
    option

##############################################################################

dirs:
    @if not exist $(BIND) mkdir $(BIND) && echo.   Created $(BIND)
    @if not exist $(OBJD) mkdir $(OBJD) && echo.   Created $(OBJD)

$(OBJD)\zsub.obj : zsub.cpp

$(OBJD)\zsub.res : zsub.rc

$(BIND)\zsub$(DETOURS_BITS).dll $(BIND)\zsub$(DETOURS_BITS).lib: \
        $(OBJD)\zsub.obj $(OBJD)\zsub.res $(DEPS)
    cl /LD $(CFLAGS) /Fe$(@R).dll /Fd$(@R).pdb \
        $(OBJD)\zsub.obj $(OBJD)\zsub.res \
        /link $(LINKFLAGS) /subsystem:console \
        /export:DetourFinishHelperProcess,@1,NONAME \
        /export:Hsbt \
        $(LIBS)

$(OBJD)\zsub$(DETOURS_BITS).bsc : $(OBJD)\zsub.obj
    bscmake /v /n /o $@ $(OBJD)\zsub.sbr

##############################################################################

clean:
    -del *~ 2>nul
    -del $(BIND)\zsub*.* 2>nul
    -rmdir /q /s $(OBJD) 2>nul

realclean: clean
    -rmdir /q /s $(OBJDS) 2>nul

############################################### Install non-bit-size binaries.

!IF "$(DETOURS_OPTION_PROCESSOR)" != ""

$(OPTD)\zsub$(DETOURS_OPTION_BITS).dll:
$(OPTD)\zsub$(DETOURS_OPTION_BITS).pdb:

$(BIND)\zsub$(DETOURS_OPTION_BITS).dll : $(OPTD)\zsub$(DETOURS_OPTION_BITS).dll
    @if exist $? copy /y $? $(BIND) >nul && echo $@ copied from $(DETOURS_OPTION_PROCESSOR).
$(BIND)\zsub$(DETOURS_OPTION_BITS).pdb : $(OPTD)\zsub$(DETOURS_OPTION_BITS).pdb
    @if exist $? copy /y $? $(BIND) >nul && echo $@ copied from $(DETOURS_OPTION_PROCESSOR).

option: \
    $(BIND)\zsub$(DETOURS_OPTION_BITS).dll \
    $(BIND)\zsub$(DETOURS_OPTION_BITS).pdb \

!ELSE

option:

!ENDIF
