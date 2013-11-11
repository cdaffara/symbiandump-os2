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

echo on

echo Expect one panic and press return to continue
\epoc32\release\winscw\udeb\scstest.exe -Dtextshell --

\epoc32\release\winscw\udeb\upstest.exe
\epoc32\release\winscw\udeb\upstestobsif.exe
\epoc32\release\winscw\udeb\upstestoom.exe
\epoc32\release\winscw\udeb\upstestobsifoom.exe


\epoc32\release\winscw\udeb\dumppolicy.exe -Dtextshell -- z:\private\10283558\policies\ups_10283559_0000002a.rsc c:\ups_10283559_0000002a.txt

\epoc32\release\winscw\udeb\testexecute z:\tups\tpolicies\scripts\tpolicycache.script
\epoc32\release\winscw\udeb\testexecute -Dstartupmode=1 -- z:\tups\tpolicies\scripts\tserviceconfig.script
\epoc32\release\winscw\udeb\testexecute z:\tups\tpolicies\scripts\tpolicyreader.script

\epoc32\release\winscw\udeb\testexecute -Dstartupmode=1 -- z:\tups\tupsdb\scripts\tupsdb.script
\epoc32\release\winscw\udeb\testexecute -Dstartupmode=1 -- z:\tups\tupsdb\scripts\tupsdbperformance.script

\epoc32\release\winscw\udeb\dumpupsdb -Dtextshell -- -t -e -db c:\private\10283690\tups.db

\epoc32\release\winscw\udeb\dumpupsdb -Dtextshell -- -t -e -db c:\Private\10283558\database\ups.db -f c:\upsdb.txt

copy /y \epoc32\winscw\c\tswi\certstore\tsis_swicertstore.dat \epoc32\release\winscw\udeb\z\resource\swicertstore.dat

copy /y \epoc32\release\winscw\udeb\tupsinteg_1.exe \epoc32\release\winscw\udeb\tupsinteg_data.exe
\epoc32\release\winscw\udeb\testexecute z:\tups\integ\scripts\tups_integ.script
