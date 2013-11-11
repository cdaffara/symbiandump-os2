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

copy /Y \epoc32\WINSCW\c\swicertstore.dat \epoc32\RELEASE\WINSCW\UREL\Z\resource\swicertstore.dat
copy /Y \epoc32\WINSCW\c\swicertstore.dat \epoc32\RELEASE\WINSCW\UDEB\Z\resource\swicertstore.dat

del \epoc32\WINSCW\c\swicertstore.dat 
del \epoc32\WINSCW\C\Resource\SwiCertstore\dat\00000001

call \epoc32\winscw\c\twtlscert\scripts\batchfiles\enableplugin.bat