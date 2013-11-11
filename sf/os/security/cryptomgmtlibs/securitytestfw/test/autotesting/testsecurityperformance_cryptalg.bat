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
md d:\testresults
md c:\testresults

REM cryptalg, symmetric
z:\system\libs\tsymmetricbench z:\tsymmetric\symmetricperformance.txt c:\testresults\tsymmetricperformance.log
move c:\testresults\tsymmetricperformance.log d:\testresults\

REM cryptalg, asymmetric
z:\system\libs\tasymmetric z:\tasymmetric\asymmetricperformance.txt c:\testresults\tasymmetricperformance.log
move c:\testresults\tasymmetricperformance.log d:\testresults\



