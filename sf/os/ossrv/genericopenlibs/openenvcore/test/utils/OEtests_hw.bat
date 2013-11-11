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

swiconsole /i z:\resource.SIS /optimal
md c:\tstdapis
md c:\system\data
md c:\tstdapis\tredirectionserver
md c:\data



copy z:\input.txt c:\tstdapis\input.txt
copy z:\tstdapis\data.txt c:\data.txt
copy z:\tstdapis\10202be9\*.* c:\private\10202be9\
copy z:\input c:\data\input

testexecute z:\tstdapis\tprogname.script
testexecute z:\tstdapis\tlibcwchar.script
testexecute z:\tstdapis\tsyscalls.script
testexecute z:\tstdapis\tsyscallsfcntl.script
testexecute z:\tstdapis\tsyscalls1.script
testexecute z:\tstdapis\tsyscalls2.script
c:
z:\tstdapis\cleanup_cdrive.bat
z:
testexecute z:\tstdapis\tifioctls_hw.script
testexecute z:\tstdapis\tstdlib.script
testexecute z:\tstdapis\tstdio_auto.script
testexecute z:\tstdapis\tstdio_filemode.script
testexecute z:\tstdapis\tstdio_fileop.script
testexecute z:\tstdapis\tpipe.script
testexecute z:\tstdapis\tstring.script
testexecute z:\tstdapis\tlibcbackend.script
c:
z:\tstdapis\cleanup_cdrive.bat
z:
testexecute z:\tstdapis\tbackenddes.script
testexecute z:\tstdapis\tctype.script
testexecute z:\tstdapis\tinet.script
testexecute z:\tstdapis\tlink.script
testexecute z:\tstdapis\tmkfifo.script
testexecute z:\tstdapis\tmmap.script
testexecute z:\tstdapis\tmsgqueue.script
testexecute z:\tstdapis\tnetdb.script
testexecute z:\tstdapis\tregex.script
c:
z:\tstdapis\cleanup_cdrive.bat
z:
testexecute z:\tstdapis\tsemaphore.script
testexecute z:\tstdapis\tshm.script
testexecute z:\tstdapis\tsocket_hw.script
testexecute z:\tstdapis\tsyssim.script
testexecute z:\tstdapis\tsysunistd.script
c:
z:\tstdapis\cleanup_cdrive.bat
z:
testexecute z:\tstdapis\ttime_blr.script
testexecute z:\tstdapis\twctype.script
testexecute z:\tstdapis\tselect.script
testexecute z:\tstdapis\taeselect.script
testexecute z:\tstdapis\tdb_blr.script
c:
z:\tstdapis\cleanup_cdrive.bat
z:
testexecute z:\tstdapis\tconddestroy.script
testexecute z:\tstdapis\tcondinit.script
testexecute z:\tstdapis\tmutex.script
testexecute z:\tstdapis\tpthread.script
testexecute z:\tstdapis\tpthreadonce.script
testexecute z:\tstdapis\thybridthreads.script
c:
z:\tstdapis\cleanup_cdrive.bat
z:
testexecute z:\tstdapis\tsemdestroy.script
testexecute z:\tstdapis\tsemgetvalue.script
testexecute z:\tstdapis\tseminit.script
testexecute z:\tstdapis\tsemopen.script
testexecute z:\tstdapis\tsempost.script
testexecute z:\tstdapis\tsemtrywait.script
testexecute z:\tstdapis\tsemwait.script
testexecute z:\tstdapis\tcondbroadcast.script
c:
z:\tstdapis\cleanup_cdrive.bat
z:
testexecute z:\tstdapis\tcondsignal.script
testexecute z:\tstdapis\tcondwait.script
testexecute z:\tstdapis\tsemtimedwait.script
testexecute z:\tstdapis\tfloat_blr.script
testexecute z:\tstdapis\tdouble_blr.script
testexecute z:\tstdapis\tdouble_pow.script
testexecute z:\tstdapis\tdouble_misc.script
testexecute z:\tstdapis\tldouble_blr.script
testexecute z:\tstdapis\tldouble_pow.script
c:
z:\tstdapis\cleanup_cdrive.bat
z:
testexecute z:\tstdapis\tldouble_misc.script
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
testexecute z:\tstdapis\tlocale_test.script
testexecute z:\tstdapis\tlocale.script
testexecute z:\tstdapis\tlocale_ctype.script
#endif
testexecute z:\tstdapis\tloc_blr.script
testexecute z:\tstdapis\tglob_test.script
testexecute z:\tstdapis\tdl.script
testexecute   z:\tstdapis\tsystime_captest.script
testexecute   z:\tstdapis\tfcntl_captest.script
testexecute   z:\tstdapis\twchar_captest.script
c:
z:\tstdapis\cleanup_cdrive.bat
z:
testexecute   z:\tstdapis\tipc_captest.script
testexecute   z:\tstdapis\tioccom_captest.script
testexecute   z:\tstdapis\tstdio_captest.script
testexecute   z:\tstdapis\tstat_captest.script
testexecute   z:\tstdapis\tutime_captest.script
testexecute   z:\tstdapis\tunistd_captest.script
testexecute   z:\tstdapis\twchar.script
c:
z:\tstdapis\cleanup_cdrive.bat
z:
testexecute   z:\tstdapis\twcharapi.script
testexecute   z:\tstdapis\twideapis.script

copy z:\system\tstdapis\config_tmisc.ini  c:\tstdapis\config_tmisc.ini
testexecute   z:\tstdapis\tmisc.script

c:
z:\tstdapis\cleanup_cdrive.bat
z:
testexecute   z:\tstdapis\tftw.script
testexecute   z:\tstdapis\tlocalsocket.script
rem testexecute   z:\tstdapis\tmulticastclient.script - To be uncommented when Network on BATS is enabled
testexecute   z:\tstdapis\toffsetof.script
testexecute   z:\tstdapis\tliblogger.script
c:
z:\tstdapis\cleanup_cdrive.bat
z:

copy z:\system\tstdapis\tredirectionserver\config_null.ini  c:\tstdapis\tredirectionserver\config_null.ini

testexecute   z:\tstdapis\tredirectionserver.script

testexecute   z:\tstdapis\tsetjmp.script


del e:\sys$rom.zip
del c:\logs\testexecute\TestResults.htm
copy c:\logs\testexecute\*.* e:\
