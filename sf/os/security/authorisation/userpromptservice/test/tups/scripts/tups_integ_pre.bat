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

REM this folder needed for backup and restore
md \epoc32\winscw\c\tswi\tbackuprestore\

md \epoc32\winscw\c\tswi\certstore\
copy /Y  %SECURITYSOURCEDIR%\os\security\authorisation\userpromptservice\test\tups\scripts\swicertstore.dat \epoc32\winscw\c\tswi\certstore\tsis_swicertstore.dat
copy tupsinteg_1.exe tupsinteg_data.exe 

REM Add the NETWORKING_UPS_DISABLE macro to epoc.ini
echo NETWORKING_UPS_DISABLE 1 >> \epoc32\data\epoc.ini

REM Run ced
call \epoc32\release\wins\udeb\ced -dtextshell -- -i c:\auto.cfg
call \epoc32\release\winscw\udeb\ced -dtextshell -- -i c:\auto.cfg

REM Backup the original swicertstore.dat
copy \epoc32\release\winscw\udeb\z\resource\swicertstore.dat \epoc32\release\winscw\udeb\z\resource\swicertstore_orgx.dat
copy \epoc32\release\winscw\urel\z\resource\swicertstore.dat \epoc32\release\winscw\urel\z\resource\swicertstore_orgx.dat


rem copy appropriate swicertstore
copy \epoc32\winscw\c\tswi\certstore\tsis_swicertstore.dat \epoc32\release\winscw\udeb\z\resource\swicertstore.dat
copy \epoc32\winscw\c\tswi\certstore\tsis_swicertstore.dat \epoc32\release\winscw\urel\z\resource\swicertstore.dat


rem cleanup anything from previous tests.
rem delete SCR registry
del /s /f /q \epoc32\winscw\c\sys\install\scr\
del /s /f /q \epoc32\winscw\c\sys\install\sisregistry\
del /s /f /q \epoc32\winscw\c\Documents\InstTest\
del /s /f /q \epoc32\winscw\drive_d\Documents\InstTest\
del /s /f /q \epoc32\winscw\c\sys\install\siregbackup.lst


Rem Removes original .exe file so it does not conflict with sis file.
del /q  \epoc32\release\winscw\udeb\tupsinteg_post1.exe
del /q  \epoc32\release\winscw\urel\tupsinteg_post1.exe

Rem delete original resources files.
del /q \epoc32\release\winscw\udeb\Z\private\10283558\policies\eclipse_ups_101f7f1f_0000aaa3.rsc 
del /q \epoc32\release\winscw\urel\Z\private\10283558\policies\eclipse_ups_101f7f1f_0000aaa3.rsc 

del /q \epoc32\release\winscw\udeb\Z\private\10283558\policies\t_ups_101f7f1f_0000aaa4.rsc
del /q \epoc32\release\winscw\urel\Z\private\10283558\policies\t_ups_101f7f1f_0000aaa4.rsc


del /q  \epoc32\release\winscw\udeb\z\resource\plugins\tups_policyevaluator05.rsc
del /q  \epoc32\release\winscw\urel\z\resource\plugins\tups_policyevaluator05.rsc


del /q	\epoc32\data\Z\resource\plugins\tups_policyevaluator05.rsc


Rem Removes original .dll and .rsc files so it does not conflict with sis file.
del /q  \epoc32\release\winscw\udeb\tups_policyevaluator05.dll	
del /q  \epoc32\release\winscw\urel\tups_policyevaluator05.dll	

del /q  \epoc32\release\winscw\udeb\z\resource\plugins\tups_dialogcreator07.rsc
del /q  \epoc32\release\winscw\urel\z\resource\plugins\tups_dialogcreator07.rsc

del /q \epoc32\data\Z\resource\plugins\tups_dialogcreator07.rsc


Rem Removes original .dll and .rsc files so it does not conflict with sis file.
del /q  \epoc32\release\winscw\udeb\tups_dialogcreator07.dll	
del /q  \epoc32\release\winscw\urel\tups_dialogcreator07.dll	
