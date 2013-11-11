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
@echo off
v:
if not exist "V:\Dev\GT\GT 0176 Dorothy Security\Test\%1\%2\testresults\lubbock" goto usage
cd "V:\Dev\GT\GT 0176 Dorothy Security\Test\%1\%2\testresults\lubbock"
cd >..\..\lubbocksummary.txt
perl "V:\Dev\GT\GT 0176 Dorothy Security\Test\newdigest.pl" >>..\..\lubbocksummary.txt
notepad ..\..\lubbocksummary.txt
goto end
:usage
echo USAGE:
echo .
echo anallubbock "<configuration> <date>"
echo anallubbock defects_cedar 20030811
echo . 
echo Valid configurations are 
echo .
echo master_beech
echo master_cedar
echo defects_beech
echo defects_cedar
echo features_beech
echo features_cedar
echo integ_beech
echo integ_cedar_strong
echo integ_cedar_weak

:end
d: