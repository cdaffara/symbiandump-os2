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

\epoc32\release\winscw\udeb\certtool

\epoc32\release\winscw\udeb\certtool -help

\epoc32\release\winscw\udeb\certtool -h list

REM Listing

\epoc32\release\winscw\udeb\certtool -list

\epoc32\release\winscw\udeb\certtool -ls

\epoc32\release\winscw\udeb\certtool -store 0 -list

\epoc32\release\winscw\udeb\certtool -owner ca -list

\epoc32\release\winscw\udeb\certtool -list Root5CA -d

REM Import

\epoc32\release\winscw\udeb\certtool -i c:\tcertstore\ent-wtls1.cer

\epoc32\release\winscw\udeb\certtool -label abc -import c:\tcertstore\ent-wtls2.cer

\epoc32\release\winscw\udeb\certtool -store 1 -label abc -import c:\tcertstore\ent-wtls2.cer

REM gives error  KErrAlreadyExists
\epoc32\release\winscw\udeb\certtool -label abc -import c:\tcertstore\ent-wtls2.cer

REM gives error  KErrNotFound
\epoc32\release\winscw\udeb\certtool -label abcd -import c:\tcertstore\notthere.cer

\epoc32\release\winscw\udeb\certtool -label abc2 -private c:\tcertstore\data\pkcs8dsa1.001 -import c:\tcertstore\data\cert_dsa1.der

\epoc32\release\winscw\udeb\certtool -remove abc2 -rka

REM gives error  certificate not found
\epoc32\release\winscw\udeb\certtool -remove notthere

REM Applications

\epoc32\release\winscw\udeb\certtool -setapps -apps SSL/TLS MidletInstall abc

\epoc32\release\winscw\udeb\certtool -removeapps -apps MidletInstall abc

\epoc32\release\winscw\udeb\certtool -removeapps -apps SWInstall abc

\epoc32\release\winscw\udeb\certtool -addapps -apps MidletInstall abc

\epoc32\release\winscw\udeb\certtool -remove abc

