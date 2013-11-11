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

:: multiplesections_test_rombuild.bat
::
echo off

::	IMPORTANT NOTE:
::	Before running this batch file, the following steps need to be done:
::
::	- copy estartnandcomp.txt in the testfiles folder to epoc32\rom\h4hrp\unistore2
::
::	- copy file quicknand.bat to epoc32\rom\h4hrp\
::
::	- Create the nandloader:
::		- get the source code for base_e32 component using CBR
::		- navigate to ..\cd cedar\generic\base\e32\rombuild\
::		- run the following command
::		- rom -v=h4hrp -i=armv5 -b=urel -t=nandloader -m=_NAND2
::		- zip this into a sis$rom.zip and copy it on the MMC card
::
::	- Copy the core.img,rofs1.img,rofs2.img,rofs3.img to MMC card
::
::	Upon boot-up of the board the device will go in to the NAND LOADER.  Select 0 to Format the NAND Drive.
::	Then press 9 to exit the NAND Loader
::
::	Call quicknand.bat to set-up the core image and the rofs partitions.
::
::	- Run tef_efm_multipleromsections.script
::

:: Generate a multisection_test_featuredatabase_core.iby file based on the supplied test multisection_test_featuredatabase_core.xml. 
:: The file will be created in the 'epoc32/rom/include' directory. 
:: The multisection_test_featuredatabase_core.iby file will be included into the ROM by the buildrom command
@echo ** Generating a feature.iby file based on the supplied test case multisection_test_featuredatabase_core.xml file...
call features -i multisection_test_featuredatabase_core.xml

echo.
echo  Building ROM section 1 of 3...
echo.

call buildrom -D_NAND2 h4hrp textshell -fm=%EPOCROOT%epoc32/rom/include/featuredatabase.xml,multisection_test_featuredatabase_core.xml multisection_test_featuredatabase_core.iby efm_integration_testserver.iby
if exist rofs1.img del rofs1.img
if exist h4hrp_001.TextShell.nand.rofs.img ren h4hrp_001.TextShell.nand.rofs.img rofs1.img

if exist core.img del core.img
if exist h4hrp_001.TextShell.nand.IMG ren h4hrp_001.TextShell.nand.IMG core.img

:: For the additional ROFS section we are using pre-created iby files 
:: This is done in order to exercise such aspect of ROMBUILD tool such as overwriting feature information by IBY files
echo.
echo  Building ROM section 2 of 3...
echo.

call buildrom -D_NAND2 h4hrp -fm=multisection_test_featuredatabase_rofs2.xml multisection_test_featuredatabase_rofs2.iby
if exist rofs2.img del rofs2.img
if exist .rofs2.img ren .rofs2.img rofs2.img

:: For the additional ROFS section we are using pre-created iby files 
:: This is done in order to exercise such aspect of ROMBUILD tool such as overwriting feature information by IBY files
echo.
echo  Building ROM section 3 of 3...
echo.

call buildrom -D_NAND2 h4hrp -fm=multisection_test_featuredatabase_rofs3.xml multisection_test_featuredatabase_rofs3.iby
if exist rofs3.img del rofs3.img
if exist .rofs3.img ren .rofs3.img rofs3.img


::	NOW JUST COPY THESE TO THE MMC CARD WITH THE NANDLOADER
