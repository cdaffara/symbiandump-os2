rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.

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

del c:\spd_logs\xml\*.xml

openssl.bat
bccrypto.bat
bcssltest.bat