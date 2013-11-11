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
@@rem

rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available

rem which accompanies this distribution, and is available

rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

copy ..\lib\inc\statexp.h \epoc32\tools\stat\headers\statexp.h
copy ..\lib\inc\stat.h \epoc32\tools\stat\headers\stat.h

copy ..\..\lib\statdll.lib \epoc32\tools\stat\stat.lib
copy ..\..\lib\statdll.exp \epoc32\tools\stat\stat.exp

copy ..\..\bin\stat.dll \epoc32\tools\stat\stat.dll
