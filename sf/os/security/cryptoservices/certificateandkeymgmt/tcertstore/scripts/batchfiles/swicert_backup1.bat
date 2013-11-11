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

copy \epoc32\RELEASE\WINSCW\UREL\Z\resource\swicertstore.dat \epoc32\WINSCW\c\swicertstore.dat

call %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\test\autotesting\installSwiCertStore.bat tcertstore\data\swicertstore.dat

call %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\test\autotesting\Configuration5.bat


