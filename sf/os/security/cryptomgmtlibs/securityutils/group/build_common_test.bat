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

rem Compilations needed in order to compile common test code
rem =========================================================

call cd %SECURITYSOURCEDIR%\common\group 
call bldmake bldfiles 
call abld test build winscw udeb

 

rem Additional compilations needed to run common tests
rem ====================================================

rem The file huge.jar is exported by testexportsbld.inf within the 
rem JavaMidletInstaller component.
rem It is needed by test trecogjava.script in common. Should be decoupled.
call cd %SECURITYSOURCEDIR%\JavaMIDletInstaller\group 
call bldmake bldfiles 
call abld test export

 