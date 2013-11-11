rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem 
rem 
rem

@echo off
md e:\logs
md e:\logs\testexecute

@echo on

@rem testexecute.exe  z:\textbasetest\textbasetest_t_rgb.script
@rem copy c:\logs\testexecute\textbasetest_t_rgb.htm  e:\logs\testexecute\textbasetest_t_rgb.htm

testexecute.exe  z:\textbasetest\textbasetest_t_types.script
copy c:\logs\testexecute\textbasetest_t_types.htm  e:\logs\testexecute\textbasetest_t_types.htm

testexecute.exe  z:\textbasetest\textbasetest_t_bididefect.script
copy c:\logs\testexecute\textbasetest_t_bididefect.htm  e:\logs\testexecute\textbasetest_t_bididefect.htm

testexecute.exe  z:\textbasetest\textbasetest_t_linebreak.script
copy c:\logs\testexecute\textbasetest_t_linebreak.htm  e:\logs\testexecute\textbasetest_t_linebreak.htm

testexecute.exe  z:\textbasetest\textbasetest_t_glyphselection.script
copy c:\logs\testexecute\textbasetest_t_glyphselection.htm  e:\logs\testexecute\textbasetest_t_glyphselection.htm

testexecute.exe  z:\textbasetest\textbasetest_t_bidi.script
copy c:\logs\testexecute\textbasetest_t_bidi.htm  e:\logs\testexecute\textbasetest_t_bidi.htm
