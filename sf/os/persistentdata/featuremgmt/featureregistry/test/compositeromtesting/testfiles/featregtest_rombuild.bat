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

::	IMPORTANT NOTE:
::	Before running these batch file, the following steps need to be done:
::
::	- copy estartnandcomp.txt in the testfiles folder to epoc32\rom\h4hrp\unistore2
::
::	- Update the quicknand.bat to include "rofs2.img" and "rofs3.img" and copy this file to epoc32\rom\h4hrp\
::	  (copy quicknand.bat from the testfiles folder)

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
::	- Run featregsetuptest
::
::	F=FEATURE & XF=EXCLUDE_FEATURE
::	CORE
::	F=		XF
::	
::	ROFS2.IBY
::	F=TestAA  	XF=TestBB, Fax, XF=TestCC
::
::	ROFS3.IBY
::	ROFS3	F=TestBB,Usb	XF=TestAA, F=TestCC	
::
:: This batch file should be run from \epoc32\rom\tools
::
:: BUILDING THE DEFAULT TECHVIEW ROM IN NAND2 I.E CORE AND ROFS1 
::
call buildrom -D_FULL_DEBUG -D_NAND2 -D_ROFS_SIZE=20000000 h4hrp techview removablefeatures.iby featregsetupTest.iby -f..\include\featureUIDs.xml
ren h4hrp_001.techview.nand.rofs.img rofs1.img

:: BUILDING THE ADDITIONAL ROM SECTION 2
::
call buildrom -D_FULL_DEBUG -D_NAND2 -D_ROFS_SIZE=20000000 h4hrp techview removablefeatures.iby rofs2.iby -f..\include\testfeatureUIDs2.xml
ren h4hrp_001.techview.nand.rofs2.img rofs2.img

:: BUILDING THE FINAL ROM SECTION 3
::
call buildrom -D_FULL_DEBUG -D_NAND2  -D_ROFS_SIZE=20000000 h4hrp techview removablefeatures.iby rofs2.iby rofs3.iby -f..\include\testfeatureUIDs3.xml
ren h4hrp_001.techview.nand.rofs3.img rofs3.img
ren h4hrp_001.techview.nand.IMG core.img

:: NOW JUST COPY THESE TO THE MMC CARD WITH THE NANDLOADER

