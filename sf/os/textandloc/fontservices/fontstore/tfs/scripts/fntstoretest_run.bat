@rem
@rem Copyright (c) 2007 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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

@echo on
testexecute.exe  z:\fntstoretest\fntstoretest_t_fnt.script
testexecute.exe  z:\fntstoretest\fntstoretest_t_fntmem.script
testexecute.exe  z:\fntstoretest\fntstoretest_t_fontbitmap.script
testexecute.exe  z:\fntstoretest\fntstoretest_t_fsopen.script
testexecute.exe  z:\fntstoretest\fntstoretest_t_glyphtreedel.script
testexecute.exe  z:\fntstoretest\fntstoretest_t_matchfonts.script
testexecute.exe  z:\fntstoretest\fntstoretest_t_openfonttypes.script
testexecute.exe  z:\fntstoretest\fntstoretest_t_fontmetrics.script
testexecute.exe  z:\fntstoretest\fntstoretest_t_getnearestfont.script
testexecute.exe  z:\fntstoretest\fntstoretest_t_getnearestfontcapability.script
testexecute.exe  z:\fntstoretest\fntstoretest_t_corruptfonts.script
testexecute.exe	 z:\fntstoretest\fntstoretest_t_shapercache.script
testexecute.exe	 z:\fntstoretest\fntstoretest_t_cachedeletion.script
testexecute.exe	 z:\fntstoretest\fntstoretest_t_setdefaulttypefacename.script
testexecute.exe	 z:\fntstoretest\fntstoretest_t_linkedfonts.script
testexecute.exe	 z:\fntstoretest\fntstoretest_t_cache.script
testexecute.exe  z:\fntstoretest\fntstoretest_t_fontsessioncache.script

@echo off
md e:\logs
md e:\logs\testexecute

copy c:\logs\testexecute\fntstoretest_t_*.htm e:\logs\testexecute\

