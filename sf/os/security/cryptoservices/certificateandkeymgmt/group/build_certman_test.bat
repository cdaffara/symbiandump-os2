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

rem Compilations needed in order to compile certman test code
rem =========================================================

rem rtestwrapper.h (and possibly more) in commonutils needed to compile swi test code
call cd %SECURITYSOURCEDIR%\commonutils\group  
call bldmake bldfiles
call abld test build winscw udeb

rem  testutilclient (and possibly more) in swi needed to compile certman test code
call cd %SECURITYSOURCEDIR%\swi\group 
call bldmake bldfiles 
call abld test build winscw udeb


rem testhandler2 in testframework is needed by filetokens and certman test code
call cd %SECURITYSOURCEDIR%\testframework\group 
call bldmake bldfiles 
call abld test build winscw udeb

rem tfiletokenslib in filetokens needed to compile certman test code
call cd %SECURITYSOURCEDIR%\filetokens\group
call bldmake bldfiles 
call abld test build winscw udeb


rem actual certman tests 
call cd %SECURITYSOURCEDIR%\certman\group
call bldmake bldfiles 
call abld test build winscw udeb





rem Additional compilations needed to run certman tests
rem =======================================

call cd %SECURITYSOURCEDIR%\testcertificates\group
call bldmake bldfiles 
call abld test build winscw udeb


call cd %SECURITYSOURCEDIR%\cryptotokens\group  
call bldmake bldfiles 
call abld test build winscw udeb