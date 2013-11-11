@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
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
SETLOCAL
REM Set root folder to one level above location of this batch file:
SET rootfolder=%~dp0\..
REM Set name of folder to be deleted:
SET foldername=traces
REM Change to root folder:
PUSHD "%rootfolder%"
REM Delete matching folder in root of current directory:
RD /S /Q "%foldername%"
REM Delete any other matching folders in all subdirectories:
FOR /F "tokens=*" %%a IN ('DIR /B /S /AD "%foldername%"') DO RD /S /Q "%%a"
echo All '%foldername%' folders deleted under '%cd%'
POPD
ENDLOCAL
