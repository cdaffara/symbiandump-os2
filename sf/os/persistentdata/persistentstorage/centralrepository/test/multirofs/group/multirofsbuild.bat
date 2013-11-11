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

:: Build a UREL NAND2 TECHVIEW TEST ROM for H4
:: PRE: -CBR TOOL is installed to allow retrieving base_e32 source to build nandloader
::	-This batch file should be run from epoc32\rom\tools
::
:: POST:- \PREQ2112_NAND_LOADER contains the sys$rom.bin
::      - \PREQ2112_ROM_IMG	contains the core.img and rofs1.img
::	- Copy all these files to the MMC card
::

REM As we are introducing additional ROFS layer on top of the default in NAND2 we need to update the following
REM * epoc32/rom/h4hrp/quicknand.bat
REM * epoc32/rom/h4hrp/unistore2/estartnandcomp.txt

REM -----------------------Build NANDLoader--------------------------
rmdir /S /Q PREQ2112_NAND_LOADER

mkdir PREQ2112_NAND_LOADER
cd PREQ2112_NAND_LOADER

call getsource -o -i . base_e32
pushd src\cedar\generic\base\e32\rombuild\
call rom -v=h4hrp -i=armv5 -b=urel -t=nandloader -m=_NAND2 --name=sys$rom.bin
copy /Y sys$rom.bin ..\..\..\..\..\..\sys$rom.bin

:: Back to PREQ1818_NAND_LOADER dir
popd
:: Remove base source
rd /s /q src

:: Back to main group dir
cd ..

REM ----------------------Build the Core and ROFS1 image--------------------

rmdir /S /Q PREQ2112_ROM_IMG
mkdir PREQ2112_ROM_IMG
cd PREQ2112_ROM_IMG

call buildrom -D_NAND2 -D_FULL_DEBUG h4hrp techview centreptesthelper.iby preq2112_rofs1.iby

rename h4hrp_001.techview.nand.IMG core.img
rename h4hrp_001.techview.nand.rofs.img rofs1.img

REM --------------------- Build the ROFS2 image------------------------------
cd ..
rmdir /S /Q PREQ2112_ROFS2_IMG
mkdir PREQ2112_ROFS2_IMG
cd PREQ2112_ROFS2_IMG
call rofsbuild ../../include/PREQ2112_ROFS2.iby

