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

:: buildecomMultiImage.cmd
:: Copyright (c) 2007-2008 Symbian Software Ltd. All rights reserved.

::Disclaimer - this cmd file intends to show the steps to build the
::composite ROM in an unequivocal way. It is not meant to be a fool
::proof automation. It will run to completion unattended but user
::must manually check log files for ROM build and compile errors.
::To enhance readability, this file does not use
::local variables, i.e. sacrifice configurability, and does not
::check for compile errors, i.e. sacrifice robustness.

::================================================
if NOT exist \rom\multiimage\save md \rom\multiimage\save
del /F /Q \rom\multiimage\save\*.txt 2>NUL
del /F /Q \rom\multiimage\*.img 2>NUL

if NOT exist \rom\h4hrp_syslibs_nand_text_urel0 md \rom\h4hrp_syslibs_nand_text_urel0
del /F /Q \rom\h4hrp_syslibs_nand_text_urel0\* 2>NUL

if NOT exist \rom\h4hrp_syslibs_nand_text_urel1 md \rom\h4hrp_syslibs_nand_text_urel1
del /F /Q \rom\h4hrp_syslibs_nand_text_urel1\* 2>NUL

::====================================================
echo Verify the ecom source directory
if NOT exist \syslibs\ecom3\Framework\MultipleImageTest\tools\scripts (
  echo This cmd file expects ecom source in \syslibs\ecom3\ directory.
  echo Move your source tree there and run %0 again.
  goto :EOF
)

::================================================
pushd \syslibs\ecom3\Framework\MultipleImageTest\tools\scripts

echo Generate a special nandloader which will autoflash the IMG files
echo on the MMC card to NAND.
if NOT exist \syslibs_multiimageloader.img perl .\generatenandloader.pl > \rom\multiimage\save\nandloaderbuild.txt

if NOT exist \syslibs_multiimageloader.img (
  echo generatenandloader.pl failed to build NAND loader.
  goto :EOF
)

copy /Y \syslibs_multiimageloader.img \rom\multiimage\sys$rom.bin
pushd \rom\multiimage
zip -q -m -9 sys$rom.zip sys$rom.bin
popd

::================================================
echo Modifying 3 obey files
if NOT exist \rom\multiimage\save\header.iby copy \epoc32\rom\include\header.iby \rom\multiimage\save\header.iby
perl .\EditHeaderIby.pl

if NOT exist \rom\multiimage\save\h4hrp.oby copy \epoc32\rom\include\h4hrp.oby \rom\multiimage\save\h4hrp.oby
perl .\EditH4hrpOby.pl

popd
::====================================================
::Backup the original estartnandcomp.txt and always replace it
::with the composite ROM version.
if NOT exist \epoc32\rom\h4hrp\unistore2 md \epoc32\rom\h4hrp\unistore2
if NOT exist \epoc32\rom\h4hrp\unistore2\estartnandcomp.txt goto MODIFYESTARTNANDCOMP
if NOT exist \rom\multiimage\save\estartnandcomp.txt copy \epoc32\rom\h4hrp\unistore2\estartnandcomp.txt \rom\multiimage\save 2>NUL
:MODIFYESTARTNANDCOMP
echo C: 2  ELOCAL FAT  0       FS_FORMAT_CORRUPT> \epoc32\rom\h4hrp\unistore2\estartnandcomp.txt
echo D: 0  ELOCAL FAT  0       FS_FORMAT_COLD,FS_SYNC_DRIVE>> \epoc32\rom\h4hrp\unistore2\estartnandcomp.txt
echo E: 1  ELOCAL FAT  0       FS_SCANDRIVE>> \epoc32\rom\h4hrp\unistore2\estartnandcomp.txt
echo Z: 3  EROFS  ROFS 0       FS_COMPOSITE>> \epoc32\rom\h4hrp\unistore2\estartnandcomp.txt
echo Z: 5  EROFS  ROFS 0       FS_COMPOSITE>> \epoc32\rom\h4hrp\unistore2\estartnandcomp.txt

set MISSING=
if not exist \epoc32\rom\include\syslibs_textshell.oby (
  echo You need to fetch syslibs_textshell.oby for the OS product from overnight build scripts.
  echo e.g. p4 print -o \epoc32\rom\include\syslibs_textshell.oby -q //PR/share/DABSRelease/buildscripts/projects/syslibs/Symbian_OS_v9.5/SysLibsDevTools/syslibs_textshell.oby
  echo For Vancouver: p4 print -o \epoc32\rom\include\syslibs_textshell.oby -q //EPOC/development/CDMA/build/buildscripts/projects/syslibs/Symbian_OS_v9.5/SysLibsDevTools/syslibs_textshell.oby
  set MISSING=syslibs_textshell.oby
)

if not exist \epoc32\rom\include\syslibs_utils.oby (
  echo You need to fetch syslibs_utils.oby for the OS product from overnight build scripts.
  echo e.g. p4 print -o \epoc32\rom\include\syslibs_utils.oby -q //PR/share/DABSRelease/buildscripts/projects/syslibs/Symbian_OS_v9.5/SysLibsDevTools/syslibs_utils.oby
  echo For Vancouver: p4 print -o \epoc32\rom\include\syslibs_utils.oby -q //EPOC/development/CDMA/build/buildscripts/projects/syslibs/Symbian_OS_v9.5/SysLibsDevTools/syslibs_utils.oby
  set MISSING=syslibs_utils.oby
)

if not exist \epoc32\rom\include\syslibs_graphics.iby (
  echo You need to fetch syslibs_graphics.iby for the OS product from overnight build scripts.
  echo e.g. p4 print -o \epoc32\rom\include\syslibs_graphics.iby -q //PR/share/DABSRelease/buildscripts/projects/syslibs/Symbian_OS_v9.5/SysLibsDevTools/syslibs_graphics.iby
  echo For Vancouver: p4 print -o \epoc32\rom\include\syslibs_graphics.iby -q //EPOC/development/CDMA/build/buildscripts/projects/syslibs/Symbian_OS_v9.5/SysLibsDevTools/syslibs_graphics.iby
  set MISSING=syslibs_graphics.iby
)

if defined MISSING (
  set MISSING=
  goto :ENDOFCMD
)

::===========================================================================
echo Compile multipleimagetest project twice and build the composite ROM

::There are about 12 lines dedicated to ensure clean build
::of the multipleimagetest project. They may seem excessively
::paranoid but could save you days of debugging.
pushd \syslibs\ecom3\framework\multipleimagetest

rd /s /q \epoc32\build\syslibs\ecom3\framework\multipleimagetest 2>NUL
del /F /Q bld.inf 2>NUL
copy bld_defaultImage.inf bld.inf
call bldmake clean
call bldmake bldfiles
call abld -v test build armv5 urel > \rom\multiimage\save\Image1build.txt 2>&1
call abld -w test build armv5 urel > \rom\multiimage\save\Image1what.txt
echo scanning for compile errors in Image1 build
call \epoc32\tools\scanlog.pl \rom\multiimage\save\Image1build.txt

pushd \rom\h4hrp_syslibs_nand_text_urel0
call buildrom.cmd -D_SERIAL_DOWNLOAD -DUSE_STRONG_CRYPTOGRAPHY -DTEXT_ONLY_ROM -DRVCT -D_EABI=ARMV5 -D_NAND2 -nofm h4hrp syslibs_textshell syslibs_utils platsec EcomMultipleImageTest.iby -orombld.img > rombuild.txt
popd

call abld -v test clean armv5 urel > \rom\multiimage\save\Image1clean.txt 2>&1
call abld -v test reallyclean armv5 urel >> \rom\multiimage\save\Image1clean.txt 2>&1

::========= build image2 =================
rd /s /q \epoc32\build\syslibs\ecom3\framework\multipleimagetest
del /F /Q bld.inf 2>NUL
copy bld_image2.inf bld.inf
call bldmake clean
call bldmake bldfiles
call abld -v test build armv5 urel > \rom\multiimage\save\Image2build.txt 2>&1
call abld -w test build armv5 urel > \rom\multiimage\save\Image2what.txt
echo scanning for compile errors in Image2 build
call \epoc32\tools\scanlog.pl \rom\multiimage\save\Image2build.txt

pushd \rom\h4hrp_syslibs_nand_text_urel1
call buildrom.cmd -D_SERIAL_DOWNLOAD -DUSE_STRONG_CRYPTOGRAPHY -DTEXT_ONLY_ROM -DRVCT -D_EABI=ARMV5 -D_NAND2 -nofm h4hrp syslibs_textshell syslibs_utils platsec EcomMultipleImage2.iby -orombld.img > rombuild.txt
popd

call abld -v test clean armv5 urel > \rom\multiimage\save\Image2clean.txt 2>&1
call abld -v test reallyclean armv5 urel >> \rom\multiimage\save\Image2clean.txt 2>&1

::================================================
::put all img files in one place
set MISSING=
copy /Y \rom\h4hrp_syslibs_nand_text_urel0\rombld.img \rom\multiimage\core.img || (
  echo ROM Build fail missing Image1 rombld.img
  set MISSING=Image1 rombld.img
)

copy /Y \rom\h4hrp_syslibs_nand_text_urel0\rombld.rofs.img \rom\multiimage\rofs1.img || (
  echo ROM Build fail missing Image1 rombld.rofs.img
  set MISSING=Image1 rombld.rofs.img
)

copy /Y \rom\h4hrp_syslibs_nand_text_urel1\rombld.rofs2.img \rom\multiimage\rofs2.img || (
  echo ROM Build fail missing Image2 rombld.rofs2.img
  set MISSING=Image2 rombld.rofs2.img
)


if defined MISSING (
  set MISSING=
) else (
  echo ROM build successful but check rombuild.txt in \rom\h4hrp_syslibs_nand_text_urel0
  echo and \rom\h4hrp_syslibs_nand_text_urel1 for any missing files.
  echo Also check \rom\multiimage\save\Image1build.txt and 
  echo \rom\multiimage\save\Image2build.txt for build errors.
)

popd

::================================================
:ENDOFCMD
echo Restore modified obey files.
copy /Y \rom\multiimage\save\h4hrp.oby \epoc32\rom\include\h4hrp.oby
copy /Y \rom\multiimage\save\header.iby \epoc32\rom\include\header.iby
if exist \rom\multiimage\save\estartnandcomp.txt copy /Y \rom\multiimage\save\estartnandcomp.txt \epoc32\rom\h4hrp\unistore2\estartnandcomp.txt
