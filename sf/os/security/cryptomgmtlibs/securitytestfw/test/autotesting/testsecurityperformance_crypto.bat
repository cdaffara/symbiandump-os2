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

REM set debug port for RTest
debugport 3

REM create testresults directory
md e:\testresults
md c:\testresults

REM crypto, symmetric
tsymmetric z:\tsymmetric\tsymmetricperformancetests.txt c:\testresults\tsymmetricperformancetests.log
move c:\testresults\tsymmetricperformancetests.log e:\testresults\

tsymmetric z:\tsymmetric\tsymmetricperformancetests.script c:\testresults\tsymmetricperformancetests.htm
move c:\testresults\tsymmetricperformancetests.htm e:\testresults\

REM crypto, asymmetric
tasymmetric z:\tasymmetric\asymmetricperformance.txt c:\testresults\tasymmetricperformance.log
move c:\testresults\tasymmetricperformance.log e:\testresults\

tasymmetric z:\tasymmetric\asymmetricperformance.script c:\testresults\tasymmetricperformance.htm
move c:\testresults\tasymmetricperformance.htm e:\testresults\

REM crypto, bigint
tbigint z:\tbigint\tperformancetests.txt c:\testresults\tperformancetests.log
move c:\testresults\tperformancetests.log e:\testresults\
tbigint z:\tbigint\tprimetests2.txt c:\testresults\tprime2.log
move c:\testresults\tprime2.log e:\testresults\

