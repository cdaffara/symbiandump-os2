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
cd \
call %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\test\autotesting\useautocfg.bat

rmdir /s /q epoc32\wins\c\
rmdir /s /q epoc32\winscw\c\
rmdir /s /q epoc32\drive_d\
rmdir /s /q epoc32\drive_e\

xcopy backup\wins\c epoc32\wins\c\ /e
xcopy backup\winscw\c epoc32\winscw\c\ /e

xcopy /y/i/e/s/r/q \epoc32\winscw\c\tswi \epoc32\release\winscw\urel\z\tswi

xcopy backup\drive_d epoc32\drive_d\ /e
xcopy backup\drive_e epoc32\drive_e\ /e

rmdir /s /q backup

call \epoc32\release\wins\udeb\ced -dtextshell -- -i c:\auto.cfg
call \epoc32\release\winscw\udeb\ced -dtextshell -- -i c:\auto.cfg

perl %SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\twtlscert\scripts\batchfiles\certstorePlugins disable_all
perl %SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\twtlscert\scripts\batchfiles\certstorePlugins enable filecertstore.dll

copy \epoc32\wins\c\tcertstore\data\testkeystore.txt \epoc32\wins\c\system\data\testkeystore.txt
copy \epoc32\winscw\c\tcertstore\data\testkeystore.txt \epoc32\winscw\c\system\data\testkeystore.txt

call %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\test\autotesting\installSwiCertStore.bat tcertstore\data\swicertstore.dat
call %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\test\autotesting\setupSwiPolicy swipolicy.ini

copy /Y \epoc32\winscw\c\tJavaHelper\trust\midp2_trp.xml \epoc32\RELEASE\WINSCW\UREL\z\private\10203636\security\midp2_trp.xml
copy \epoc32\winscw\c\tJavaHelper\trust\security_root5ca.der \epoc32\RELEASE\WINSCW\UREL\z\private\10203636\security\trustroots\device\certificates\security_root5ca.der

rem Copy eclipsable ROM stub and accompanying files to correct places

md \epoc32\release\winscw\urel\z\eclipsetest
copy /y \epoc32\winscw\c\tswi\teclipsing\data\eclispingteststub.sis \epoc32\release\winscw\urel\z\system\install\eclispingteststub.sis
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file1.txt \epoc32\RELEASE\WINSCW\UREL\Z\eclipsetest\file1.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file2.txt \epoc32\RELEASE\WINSCW\UREL\Z\eclipsetest\file2.txt
copy /y \epoc32\winscw\c\tswi\teclipsing\data\file3.txt \epoc32\RELEASE\WINSCW\UREL\Z\eclipsetest\file3.txt

copy /y \epoc32\release\winscw\urel\tswisidupgradeexe.exe 	\epoc32\release\winscw\urel\z\sys\bin\tswisidupgradeexe.exe
copy /y \epoc32\release\winscw\urel\eclipsetest1.dll		\epoc32\release\winscw\urel\z\sys\bin\eclipsetest.dll

md \epoc32\release\winscw\urel\z\sys\bin\test\
md \epoc32\release\winscw\urel\z\sys\bin\test1\test2\
copy /y \epoc32\release\winscw\urel\tswisidcache1.exe 		\epoc32\release\winscw\urel\z\sys\bin\test\testsidcache1.exe
copy /y \epoc32\release\winscw\urel\tswisidcache2.exe 		\epoc32\release\winscw\urel\z\sys\bin\test1\test2\testsidcache2.exe
