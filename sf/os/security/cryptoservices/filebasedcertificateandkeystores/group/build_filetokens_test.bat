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
 
call cd %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\group
call bldmake bldfiles 
call abld test build winscw udeb

call cd %SECURITYSOURCEDIR%\filetokens\group
call bldmake bldfiles 
call abld test build winscw udeb


rem Additional compilations needed to run filetokens tests
rem =======================================================

call cd %SECURITYSOURCEDIR%\cryptotokens\group
call bldmake bldfiles 
call abld test build winscw udeb
 
call cd %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\testcertificates\group
call bldmake bldfiles 
call abld test build winscw udeb

rem Needed to compile swi test (need at least rtestwrapper)
call cd %SECURITYSOURCEDIR%\commonutils\group
call bldmake bldfiles 
call abld test build winscw udeb

rem Needed to compile certman test (needs at least testutilclient)
call cd %SECURITYSOURCEDIR%\swi\group
call bldmake bldfiles 
call abld test build winscw udeb

rem Needed for execution of test sec-filetokens-tools
call cd %SECURITYSOURCEDIR%\certman\group
call bldmake bldfiles 
call abld test build winscw udeb
 
 
 
 


 
