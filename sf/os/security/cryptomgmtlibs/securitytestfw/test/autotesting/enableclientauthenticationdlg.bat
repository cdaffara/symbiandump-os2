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

if "%1"=="ini" goto ini;
if "%1"=="cert" goto certstore; 
if "%1"=="restore" goto restore;
goto exit

:ini
REM copy the Client Authentication enabled policy file 
goto exit

:restore
move /Y \epoc32\winscw\c\private\101f72a6_original\*.* \epoc32\winscw\c\private\101f72a6\.
rd /Q /S \epoc32\winscw\c\private\101f72a6_original
goto ini

:certstore
md \epoc32\winscw\c\private\101f72a6_original
move \epoc32\winscw\c\private\101f72a6\*.* \epoc32\winscw\c\private\101f72a6_original\.
goto exit

:exit
