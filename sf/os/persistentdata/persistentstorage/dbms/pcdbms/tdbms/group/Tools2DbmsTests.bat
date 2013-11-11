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

rem Run the TOOLS2 versions of the tests.

%EPOCROOT%epoc32\release\tools2\rel\t_alter.exe > t_alter.txt
rem t_comp depends on t_bench
%EPOCROOT%epoc32\release\tools2\rel\t_bench.exe > t_bench.txt
%EPOCROOT%epoc32\release\tools2\rel\t_comp.exe > t_comp.txt
%EPOCROOT%epoc32\release\tools2\rel\t_big.exe > t_big.txt
%EPOCROOT%epoc32\release\tools2\rel\t_bug.exe > t_bug.txt
%EPOCROOT%epoc32\release\tools2\rel\t_client.exe > t_client.txt
%EPOCROOT%epoc32\release\tools2\rel\t_dbms.exe > t_dbms.txt
%EPOCROOT%epoc32\release\tools2\rel\t_dbmsscript.exe > t_dbmsscript.txt
%EPOCROOT%epoc32\release\tools2\rel\t_defect.exe > t_defect.txt
%EPOCROOT%epoc32\release\tools2\rel\t_index.exe > t_index.txt
%EPOCROOT%epoc32\release\tools2\rel\t_longcol.exe > t_longcol.txt
%EPOCROOT%epoc32\release\tools2\rel\t_window.exe > t_window.txt
%EPOCROOT%epoc32\release\tools2\rel\t_fail.exe > t_fail.txt
%EPOCROOT%epoc32\release\tools2\rel\t_sql.exe > t_sql.txt
%EPOCROOT%epoc32\release\tools2\rel\t_limit.exe > t_limit.txt
%EPOCROOT%epoc32\release\tools2\rel\t_trans.exe > t_trans.txt
%EPOCROOT%epoc32\release\tools2\rel\t_DbmsStrComp.exe > t_dbmsstrcomp.txt
%EPOCROOT%epoc32\release\tools2\rel\t_panic.exe > t_panic.txt

