@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

rem Compilations needed in order to compile ups test code
rem =========================================================

rem ups test code requires testhandler2 in test framework in order to 
rem compile.
call cd %SECURITYSOURCEDIR%\testframework\group
call bldmake bldfiles 
call abld test build winscw udeb

rem  commonutils is needed in order to compile ups (needs at least rtestwrapper.h)
call cd %SECURITYSOURCEDIR%\commonutils\group
call bldmake bldfiles 
call abld test build winscw udeb

call cd %SECURITYSOURCEDIR%\ups\group
call bldmake bldfiles 
call abld test build winscw udeb

rem no test code currently.
rem call cd %SECURITYSOURCEDIR%\ups\techview\group
rem call bldmake bldfiles 
rem call abld test build winscw udeb



rem Additional compilations needed to run ups tests
rem ===============================================

rem Test failures (tups_integ and upstestobsif) if swi isn't compiled.
call cd %SECURITYSOURCEDIR%\swi\group
call bldmake bldfiles 
call abld test build winscw udeb

 

 

 



 
