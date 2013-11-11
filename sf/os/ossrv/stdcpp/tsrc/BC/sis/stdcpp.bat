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

rem Redistribution and use in source and binary forms, with or without 
rem modification, are permitted provided that the following conditions are met:

rem Redistributions of source code must retain the above copyright notice, this 
rem list of conditions and the following disclaimer.
rem Redistributions in binary form must reproduce the above copyright notice, 
rem this list of conditions and the following disclaimer in the documentation 
rem and/or other materials provided with the distribution.
rem Neither the name of Nokia Corporation nor the names of its contributors 
rem may be used to endorse or promote products derived from this software 
rem without specific prior written permission.

rem THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
rem AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
rem IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
rem DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
rem FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
rem DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
rem SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
rem CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
rem OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
rem OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
rem
rem Description:      
rem




md e:\BCTest
md e:\BCTest\Results



copy C:\TestFramework\TestFramework_cppwrap.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\bccppwrap.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_cppwrap.txt



copy C:\TestFramework\TestFramework_mrtruntime.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\bccsruntimesup.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_csruntime.txt


copy C:\TestFramework\TestFramework_mrtexp.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\BCException.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_exception.txt


copy C:\TestFramework\TestFramework_mrtnons.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\BCNoNamespace.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_nonamespace.txt


copy C:\TestFramework\TestFramework_mrtopenc.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\BCOpenCHeaders.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_opencheaders.txt


copy C:\TestFramework\TestFramework_ocw.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\BCOpenCWrap.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_opencwrap.txt


copy C:\TestFramework\TestFramework_mrtthread.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\bcthread.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_thread.txt


copy C:\TestFramework\TestFramework_mrttypea.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\BCTypeAttrib.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_typeattrib.txt


copy C:\TestFramework\TestFramework_mrttypei.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\BCTypeInfo.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_typeinfo.txt


copy C:\TestFramework\TestFramework_ct.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule chartypes
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_chartypes.txt


copy C:\TestFramework\TestFramework_mrtmath.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\tmath_operations.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_mathoperations.txt


copy C:\TestFramework\TestFramework_mrtmem.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\tmemory.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_tmemory.txt


copy C:\TestFramework\TestFramework_mrtutil.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\tutility.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_utility.txt


copy C:\TestFramework\testframework_wc.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule widecharclassapiBCTest
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_widecharclass.txt


copy C:\TestFramework\TestFramework_tstl_1.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\tstl_1.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_tstl.txt


copy C:\TestFramework\TestFramework_tlocale.ini C:\TestFramework\TestFramework.ini
ATSINTERFACE.EXE -testmodule testscripter -config C:\TestFramework\tlocale.cfg
copy c:\Logs\TestFramework\TestReport.txt e:\BCTest\results\mrt_tlocale.txt
