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
@echo off

del *.script /Q
del *.bin /Q

cd example
call .\makeexamples.bat
for %%i in (*.p12 *.der) do call ..\dotestimport.bat %%i
rem for %%i in (*.p12 *.der) do echo ..\dotestimport.bat %%i
cd ..
