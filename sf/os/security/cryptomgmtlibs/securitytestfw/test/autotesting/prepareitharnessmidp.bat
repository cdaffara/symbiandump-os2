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
if not exist \epoc32\release\winscw\udeb\z\Resource\JavaHelper\JavaHelper.rsc.original copy \epoc32\release\winscw\udeb\z\Resource\JavaHelper\JavaHelper.rsc \epoc32\release\winscw\udeb\z\Resource\JavaHelper\JavaHelper.rsc.original
if not exist \epoc32\release\winscw\urel\z\Resource\JavaHelper\JavaHelper.rsc.original copy \epoc32\release\winscw\urel\z\Resource\JavaHelper\JavaHelper.rsc \epoc32\release\winscw\urel\z\Resource\JavaHelper\JavaHelper.rsc.original

if not exist \epoc32\release\wins\udeb\z\system\data\JavaHelper.rsc copy \epoc32\release\wins\udeb\z\system\data\JavaHelper.rsc \epoc32\release\wins\udeb\z\system\data\JavaHelper.rsc.original
if not exist \epoc32\release\wins\urel\z\system\data\JavaHelper.rsc copy \epoc32\release\wins\urel\z\system\data\JavaHelper.rsc \epoc32\release\wins\urel\z\system\data\JavaHelper.rsc.original
if not exist \epoc32\release\winscw\udeb\z\system\data\JavaHelper.rsc copy \epoc32\release\winscw\udeb\z\system\data\JavaHelper.rsc \epoc32\release\winscw\udeb\z\system\data\JavaHelper.rsc.original
if not exist \epoc32\release\winscw\urel\z\system\data\JavaHelper.rsc copy \epoc32\release\winscw\urel\z\system\data\JavaHelper.rsc \epoc32\release\winscw\urel\z\system\data\JavaHelper.rsc.original


rem If a parameter is specified then use this as the name of the alternate resource file
if x==%1x goto restoreoriginal
rem OS v9 onwards
copy \epoc32\winscw\c\itharnessmidp\resources\%1.rsc \epoc32\release\winscw\udeb\z\Resource\JavaHelper\JavaHelper.rsc
copy \epoc32\winscw\c\itharnessmidp\resources\%1.rsc \epoc32\release\winscw\urel\z\Resource\JavaHelper\JavaHelper.rsc

rem Pre OS v9
copy \epoc32\wins\c\itharnessmidp\resources\%1.rsc \epoc32\release\wins\udeb\z\system\data\JavaHelper.rsc
copy \epoc32\wins\c\itharnessmidp\resources\%1.rsc \epoc32\release\wins\urel\z\system\data\JavaHelper.rsc
copy \epoc32\winscw\c\itharnessmidp\resources\%1.rsc \epoc32\release\winscw\udeb\z\system\data\JavaHelper.rsc
copy \epoc32\winscw\c\itharnessmidp\resources\%1.rsc \epoc32\release\winscw\urel\z\system\data\JavaHelper.rsc
goto exit

rem No arguments so restore the original resource file
:restoreoriginal
rem OS v9 onwards
copy \epoc32\release\winscw\udeb\z\Resource\JavaHelper\JavaHelper.rsc.original \epoc32\release\winscw\udeb\z\Resource\JavaHelper\JavaHelper.rsc
copy \epoc32\release\winscw\urel\z\Resource\JavaHelper\JavaHelper.rsc.original \epoc32\release\winscw\urel\z\Resource\JavaHelper\JavaHelper.rsc

rem Pre OS v9
copy \epoc32\release\wins\udeb\z\system\data\JavaHelper.rsc.original \epoc32\release\wins\udeb\z\system\data\JavaHelper.rsc
copy \epoc32\release\wins\urel\z\system\data\JavaHelper.rsc.original  \epoc32\release\wins\urel\z\system\data\JavaHelper.rsc
copy \epoc32\release\winscw\udeb\z\system\data\JavaHelper.rsc.original \epoc32\release\winscw\udeb\z\system\data\JavaHelper.rsc
copy \epoc32\release\winscw\urel\z\system\data\JavaHelper.rsc.original  \epoc32\release\winscw\urel\z\system\data\JavaHelper.rsc
goto exit

:exit