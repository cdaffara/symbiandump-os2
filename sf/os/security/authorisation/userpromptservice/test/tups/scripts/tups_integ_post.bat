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

del /S /F /Q \epoc32\winscw\c\Resource\SwiCertstore\*
copy \epoc32\release\winscw\udeb\z\resource\swicertstore_orgx.dat \epoc32\release\winscw\udeb\z\resource\swicertstore.dat
copy \epoc32\release\winscw\urel\z\resource\swicertstore_orgx.dat \epoc32\release\winscw\urel\z\resource\swicertstore.dat
del \epoc32\release\winscw\udeb\z\resource\swicertstore_orgx.dat
del \epoc32\release\winscw\urel\z\resource\swicertstore_orgx.dat

REM deletes cloned clients (cloned using SETCAP)
del \epoc32\release\winscw\c\sys\bin\TUPSInteg_2.exe
del \epoc32\release\winscw\c\sys\bin\TUPSInteg_3.exe
del \epoc32\release\winscw\c\sys\bin\TUPSInteg_4.exe
del \epoc32\release\winscw\c\sys\bin\TUPSInteg_allfiles_readdevice.exe
del \epoc32\release\winscw\c\sys\bin\TUPSInteg_readdevice.exe
del \epoc32\release\winscw\c\sys\bin\TUPSInteg_writedevice.exe
del \epoc32\release\winscw\c\sys\bin\TUPSInteg_minor.exe 