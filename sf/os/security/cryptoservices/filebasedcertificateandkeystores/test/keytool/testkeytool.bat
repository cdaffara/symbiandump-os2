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

\epoc32\release\winscw\udeb\keytool

\epoc32\release\winscw\udeb\keytool -help

\epoc32\release\winscw\udeb\keytool -help import

REM Listing

\epoc32\release\winscw\udeb\keytool -list

\epoc32\release\winscw\udeb\keytool -list -d

\epoc32\release\winscw\udeb\keytool -store 0 -list

\epoc32\release\winscw\udeb\keytool -ls

\epoc32\release\winscw\udeb\keytool -label test -import c:\tkeystore\data\pkcs8rsa.001

\epoc32\release\winscw\udeb\keytool -label pkcs -import c:\tkeystore\data\pkcs8dsa.001 

\epoc32\release\winscw\udeb\keytool -import c:\tkeystore\data\pkcs8dsa.001

\epoc32\release\winscw\udeb\keytool -label pkcs1-store 1 -import c:\tkeystore\data\pkcs8dsa1.001  


REM Users

\epoc32\release\winscw\udeb\keytool -s 0x1234abcd pkcs

\epoc32\release\winscw\udeb\keytool -a pkcs

\epoc32\release\winscw\udeb\keytool -m pkcs

\epoc32\release\winscw\udeb\keytool -ru pkcs

\epoc32\release\winscw\udeb\keytool -remove test