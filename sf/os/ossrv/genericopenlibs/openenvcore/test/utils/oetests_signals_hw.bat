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

testexecute   z:\tstdapis\twaitsignal.script
testexecute   z:\tstdapis\tblocksignal.script
testexecute   z:\tstdapis\tsendsignal.script
testexecute   z:\tstdapis\tmiscsignal.script
testexecute   z:\tstdapis\ttrapsignal.script
testexecute   z:\tstdapis\tsignal_captest.script
c:
z:\tstdapis\cleanup_cdrive.bat
z:
del e:\sys$rom.zip
del c:\logs\testexecute\TestResults.htm
copy c:\logs\testexecute\*.* e:\
