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
REM This should be run after to running the ZipTestManual test for ezlib on the Emulator (the test does not run on hardware), to restore files deleted in the ZipTestManualPre.bat.

copy /y \epoc32\winscw\c\test\data\cp850.RSC \epoc32\RELEASE\WINSCW\UREL\Z\resource\plugins\cp850.RSC	
copy /y \epoc32\winscw\c\test\data\cp850.RSC \epoc32\RELEASE\WINSCW\UDEB\Z\resource\plugins\cp850.RSC	
