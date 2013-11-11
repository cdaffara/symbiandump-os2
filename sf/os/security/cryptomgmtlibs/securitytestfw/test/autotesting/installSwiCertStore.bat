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

@ECHO Off

REM Copies swi cert store data file into place for emulator tests
REM Called with the path to the data file as the only argument

set FILE=\epoc32\winscw\c\%1%
echo Installing SWI cert store data file from %FILE%

mkdir \epoc32\release\winscw\udeb\z\resource\
copy %FILE% \epoc32\release\winscw\udeb\z\resource\
copy %FILE% \epoc32\release\winscw\urel\z\resource\
