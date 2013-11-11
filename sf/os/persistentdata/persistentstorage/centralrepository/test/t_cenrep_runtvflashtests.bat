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

@SETLOCAL
@rem This batch file will run the tests automatically.
@IF "%~1"=="" goto ERROR
@IF "%~2"=="" goto ERROR
@if exist \textshell.bat call \textshell.bat
@FIND "startupmode 1" %EPOCROOT%EPOC32\data\epoc.ini >NUL
@if errorlevel 1 goto BODY
goto ERROR

:BODY
@REM Change the attributes of rom version file to rw
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt attrib -r %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt
@REM #####################################################
@rem FIRST TEST PART1 SYSLIB-CENTRALREPOSITORY-CT-4006 AND SYSLIB-CENTRALREPOSITORY-CT-4023
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043334.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043335.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem1.exe call %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem1.exe

@REM FIRST PRETEST SETUP PART2
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_corrupt.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_corrupt.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043334.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043335.txt


@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt 10033333_txt.tmp
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt erase %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt sw_txt.tmp
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\sw_modified.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\sw_modified.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt


@REM FIRST TEST PART2
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem2.exe call %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem2.exe

@REM FIRST POSTTEST CLEANUP
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt erase %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_txt.tmp rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_txt.tmp 10033333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt erase %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw_txt.tmp rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw_txt.tmp sw.txt


@REM ####################################################
@REM SECOND TEST PART1 SYSLIB-CENTRALREPOSITORY -CT-4007-10
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt attrib -r %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_hw_setup.exe call %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_hw_setup.exe 

@REM SECOND TEST PART2 SETUP - Remove repository from rom
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt 10033333_txt.tmp
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033335.txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033335.txt 10033335_txt.tmp
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033336.txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033336.txt 10033336_txt.tmp
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033339.txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033339.txt 10033339_txt.tmp
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_005.txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_005.txt 10033336.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt sw_txt.tmp
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\sw_modified.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\sw_modified.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt

@REM SECOND TEST - Restart cenrep and the persists repository should have been removed.
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_hw_tests.exe call %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_hw_tests.exe 

@REM SECOND TEST PART2 POSTTEST CLEANUP
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_txt.tmp rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_txt.tmp 10033333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033335_txt.tmp rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033335_txt.tmp 10033335.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033339_txt.tmp rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033339_txt.tmp 10033339.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033336.txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033336.txt 10033333_005.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033336_txt.tmp rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033336_txt.tmp 10033336.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt erase %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw_txt.tmp rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw_txt.tmp sw.txt


@REM ####################################################
@REM SYSLIB-CENTRALREPOSITORY-CT-4021
@REM Modify a repository, then update the rom repository. remove the resource\versions directory so a merge shouldnt happen.
@REM Change the attributes of rom version file to rw
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt attrib -r %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt
@REM #####################################################
@rem THIRD TEST PART1
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043334.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043335.txt

@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem1.exe call %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem1.exe

@REM THIRD PRETEST SETUP PART2
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_corrupt.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_corrupt.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043334.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043335.txt

@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt 10033333_txt.tmp
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions versions1
@REM THIRD TEST PART2
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem3.exe call %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem3.exe

@REM THIRD POSTTEST CLEANUP
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt erase %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_txt.tmp rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_txt.tmp 10033333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions1 rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions1 versions


@REM ####################################################
@REM SYSLIB-CENTRALREPOSITORY-CT-4022
@REM Modify a repository, then update the rom repository. Place a corrupt rominfo_cache.txt file for comparison.
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt attrib -r %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt
@REM #####################################################
@rem FOURTH TEST PART1
@REM if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043333.txt
@REM if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043334.txt
@REM if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043335.txt
@REM if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem1.exe call %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem1.exe

@REM FOURTH PRETEST SETUP PART2
@REM if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043333.txt
@REM if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_corrupt.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_corrupt.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043334.txt
@REM if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043335.txt

@REM if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt 10033333_txt.tmp
@REM if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt
@REM if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\sw_corrupt.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\sw_corrupt.txt %EPOCROOT%EPOC32\winscw\c\private\10202be9\romversion\romversion_info.txt
@REM FOURTH TEST PART2
@REM if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem3.exe call %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem3.exe

@REM FOURTH POSTTEST CLEANUP
@REM if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt erase %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt
@REM if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_txt.tmp rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_txt.tmp 10033333.txt
@REM if exist %EPOCROOT%EPOC32\winscw\c\private\10202be9\romversion\romversion_info.txt erase %EPOCROOT%EPOC32\winscw\c\private\10202be9\romversion\romversion_info.txt

@REM ####################################################
@REM SYSLIB-CENTRALREPOSITORY -CT-4020
@REM Modify a repository, then update the rom repository. remove the sw.txt file so a merge shouldnt happen.
@REM Change the attributes of rom version file to rw
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt attrib -r %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt
@REM #####################################################
@rem FIFTH TEST PART1
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043334.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043335.txt

@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem1.exe call %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem1.exe

@REM FIFTH PRETEST SETUP PART2
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_corrupt.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_corrupt.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043334.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10043335.txt

@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt 10033333_txt.tmp
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt copy %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_extra.txt %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw.txt sw_txt 
@REM THIRD TEST PART2
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem3.exe call %EPOCROOT%EPOC32\RELEASE\%1\%2\t_cenrep_fota_tvem3.exe

@REM FIFTH POSTTEST CLEANUP
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt erase %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_txt.tmp rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\private\10202be9\10033333_txt.tmp 10033333.txt
@if exist %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw_txt rename %EPOCROOT%EPOC32\RELEASE\%1\%2\Z\resource\versions\sw_txt sw.txt
GOTO EOF

:ERROR 
@echo "Needs to be run in techview mode and command line args winscw udeb|urel"

:EOF

