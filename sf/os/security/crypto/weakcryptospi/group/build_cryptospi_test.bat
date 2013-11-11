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

rem testframework provides testhandler2 which is needed to compile
rem cryptospi test code
call cd %SECURITYSOURCEDIR%\testframework\group
call bldmake bldfiles 
call abld test build winscw udeb

call cd %SECURITYSOURCEDIR%\cryptospi\group
call bldmake bldfiles 
call abld test build winscw udeb

rem no test code in this section currently
rem call cd %SECURITYSOURCEDIR%\cryptospi\strong
rem call bldmake bldfiles 
rem call abld test build winscw udeb

rem tests pass without this currently
rem call cd %SECURITYSOURCEDIR%\cryptospi\plugins\group
rem call bldmake bldfiles 
rem call abld test build winscw udeb
 
 




 
  
 



 
