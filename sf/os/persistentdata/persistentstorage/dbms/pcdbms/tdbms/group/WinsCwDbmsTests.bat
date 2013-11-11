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

@echo off

del  %TEMP%\epocwind.out

%EPOCROOT%epoc32\release\winscw\urel\t_alterpc.exe
copy %TEMP%\epocwind.out t_alter.txt
rem t_comp depends on t_bench
%EPOCROOT%epoc32\release\winscw\urel\t_benchpc.exe
copy %TEMP%\epocwind.out t_bench.txt
%EPOCROOT%epoc32\release\winscw\urel\t_comppc.exe
copy %TEMP%\epocwind.out t_comp.txt

%EPOCROOT%epoc32\release\winscw\urel\t_bigpc.exe
copy %TEMP%\epocwind.out t_big.txt
%EPOCROOT%epoc32\release\winscw\urel\t_bugpc.exe
copy %TEMP%\epocwind.out t_bug.txt
%EPOCROOT%epoc32\release\winscw\urel\t_clientpc.exe
copy %TEMP%\epocwind.out t_client.txt
%EPOCROOT%epoc32\release\winscw\urel\t_dbmspc.exe
copy %TEMP%\epocwind.out t_dbms.txt
%EPOCROOT%epoc32\release\winscw\urel\t_dbmsscriptpc.exe
copy %TEMP%\epocwind.out t_dbmsscript.txt
%EPOCROOT%epoc32\release\winscw\urel\t_defectpc.exe
copy %TEMP%\epocwind.out t_defect.txt
%EPOCROOT%epoc32\release\winscw\urel\t_indexpc.exe
copy %TEMP%\epocwind.out t_index.txt
%EPOCROOT%epoc32\release\winscw\urel\t_longcolpc.exe
copy %TEMP%\epocwind.out t_longcol.txt
%EPOCROOT%epoc32\release\winscw\urel\t_windowpc.exe
copy %TEMP%\epocwind.out t_window.txt

%EPOCROOT%epoc32\release\winscw\urel\t_failpc.exe
copy %TEMP%\epocwind.out t_fail.txt
%EPOCROOT%epoc32\release\winscw\urel\t_sqlpc.exe
copy %TEMP%\epocwind.out t_sql.txt
%EPOCROOT%epoc32\release\winscw\urel\t_limitpc.exe
copy %TEMP%\epocwind.out t_limit.txt
%EPOCROOT%epoc32\release\winscw\urel\t_transpc.exe
copy %TEMP%\epocwind.out t_trans.txt
%EPOCROOT%epoc32\release\winscw\urel\t_DbmsStrComppc.exe
copy %TEMP%\epocwind.out t_dbmsstrcomp.txt
%EPOCROOT%epoc32\release\winscw\urel\t_panicpc.exe
copy %TEMP%\epocwind.out t_panic.txt
