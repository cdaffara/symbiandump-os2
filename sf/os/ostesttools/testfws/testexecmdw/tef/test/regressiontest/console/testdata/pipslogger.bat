@rem
@rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

ced c:\loopback.xml
copy c:\system\data\testexecute.ini  c:\backupTEFc.ini 
copy z:\system\data\testexecute.ini  c:\backupTEFz.ini 

copy z:\testdata\configs\pips.ini c:\system\data\testexecute.ini
copy z:\testdata\configs\pips.ini z:\system\data\testexecute.ini
test_telnetd -D
