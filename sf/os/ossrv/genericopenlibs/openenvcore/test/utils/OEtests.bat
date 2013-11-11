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

testexecute c:\tstdapis\tprogname.script
testexecute c:\tstdapis\tlibcwchar.script
testexecute c:\tstdapis\tsyscalls.script
testexecute c:\tstdapis\tsyscallsfcntl.script
testexecute c:\tstdapis\tsyscalls1.script
testexecute c:\tstdapis\tsyscalls2.script
testexecute c:\tstdapis\tifioctls_hw.script
testexecute c:\tstdapis\tstdlib.script
testexecute c:\tstdapis\tstdio_auto.script
testexecute c:\tstdapis\tstdio_filemode.script
testexecute c:\tstdapis\tstdio_fileop.script
testexecute c:\tstdapis\tpipe.script
testexecute c:\tstdapis\tstring.script
testexecute c:\tstdapis\tlibcbackend.script
testexecute C:\tstdapis\tbackenddes.script
testexecute c:\tstdapis\tctype.script
testexecute c:\tstdapis\tinet.script
testexecute c:\tstdapis\tlink.script
testexecute c:\tstdapis\tmkfifo.script
testexecute c:\tstdapis\tmmap.script
testexecute c:\tstdapis\tmsgqueue.script
testexecute c:\tstdapis\tnetdb.script
testexecute c:\tstdapis\tregex.script
testexecute c:\tstdapis\tsemaphore.script
testexecute c:\tstdapis\tshm.script
testexecute c:\tstdapis\tsocket_hw.script
testexecute c:\tstdapis\tsyssim.script
testexecute c:\tstdapis\tsysunistd.script
testexecute c:\tstdapis\ttime_blr.script
testexecute c:\tstdapis\twctype.script
testexecute c:\tstdapis\tselect.script
testexecute c:\tstdapis\taeselect.script
testexecute c:\tstdapis\tdb_blr.script
testexecute c:\tstdapis\tconddestroy.script
testexecute c:\tstdapis\tcondinit.script
testexecute c:\tstdapis\tmutex.script
testexecute c:\tstdapis\tpthread.script
testexecute c:\tstdapis\tpthreadonce.script
testexecute c:\tstdapis\thybridthreads.script
testexecute c:\tstdapis\tsemdestroy.script
testexecute c:\tstdapis\tsemgetvalue.script
testexecute c:\tstdapis\tseminit.script
testexecute c:\tstdapis\tsemopen.script
testexecute c:\tstdapis\tsempost.script
testexecute c:\tstdapis\tsemtrywait.script
testexecute c:\tstdapis\tsemwait.script
testexecute c:\tstdapis\tcondbroadcast.script
testexecute c:\tstdapis\tcondsignal.script
testexecute c:\tstdapis\tcondwait.script
testexecute c:\tstdapis\tsemtimedwait.script
testexecute c:\tstdapis\tfloat_blr.script
testexecute c:\tstdapis\tdouble_blr.script
testexecute c:\tstdapis\tdouble_pow.script
testexecute c:\tstdapis\tdouble_misc.script
testexecute c:\tstdapis\tldouble_blr.script
testexecute c:\tstdapis\tldouble_pow.script
testexecute c:\tstdapis\tldouble_misc.script

#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
testexecute c:\tstdapis\tlocale_test.script
testexecute c:\tstdapis\tlocale.script
testexecute c:\tstdapis\tlocale_ctype.script
#endif
testexecute c:\tstdapis\tloc_blr.script
testexecute c:\tstdapis\tmisc.script
testexecute c:\tstdapis\tglob_test.script
testexecute c:\tstdapis\tdl.script
testexecute   c:\tstdapis\tsystime_captest.script
testexecute   c:\tstdapis\tfcntl_captest.script
testexecute   c:\tstdapis\twchar_captest.script
testexecute   c:\tstdapis\tipc_captest.script
testexecute   c:\tstdapis\tioccom_captest.script
testexecute   c:\tstdapis\tstdio_captest.script
testexecute   c:\tstdapis\tstat_captest.script
testexecute   c:\tstdapis\tutime_captest.script
testexecute   c:\tstdapis\tunistd_captest.script

testexecute   c:\tstdapis\twchar.script
testexecute   c:\tstdapis\twcharapi.script
testexecute   c:\tstdapis\twideapis.script
testexecute   c:\tstdapis\tcomport.script
testexecute   c:\tstdapis\tftw.script
testexecute   c:\tstdapis\tlocalsocket.script
testexecute   c:\tstdapis\tmulticastclient.script
testexecute   c:\tstdapis\tifioctls_hw_manual.script
testexecute   c:\tstdapis\toffsetof.script
testexecute   c:\tstdapis\tliblogger.script

testexecute   c:\tstdapis\tredirectionserver.script
testexecute   c:\tstdapis\tsetjmp.script

#ifdef SYMBIAN_OE_LARGE_FILE_SUPPORT
testexecute c:\tstdapis\tlargefile.script
#endif
