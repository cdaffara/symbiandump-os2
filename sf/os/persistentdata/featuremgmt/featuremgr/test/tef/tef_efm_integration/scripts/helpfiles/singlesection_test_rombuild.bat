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
:: singlesection_test_rombuild.bat
::
:: Copyright (c) Symbian Software Ltd 2007 - 2008. All rights reserved.

:: Navigate to '...\featmgr\test\tef\tef_efm_integration\scripts\helpfiles' before
:: calling this batch file.

@echo on

:: Generate a singlesection_test_features.iby file based on the supplied test singlesection_test_featuredatabase.xml. 
:: The file will be created in the 'epoc32/rom/include' directory. 
@echo ** Generating a singlesection_test_features.iby file based on the supplied test case singlesection_test_featuredatabase.xml file...
call features -i singlesection_test_featuredatabase.xml

@echo.

:: Build the textshell test ROM.
:: -DSYMBIAN_INCLUDE_TESTOVERWRITTENBYIBY option is used to overwrite the value of the TestOverwrittenByIBYUid feature
@echo ** Building the UREL textshell test ROM...
call buildrom h4hrp textshell -fm=%EPOCROOT%epoc32/rom/include/featuredatabase.xml,singlesection_test_featuredatabase.xml efm_integration_testserver.iby singlesection_test_features.iby -DSYMBIAN_INCLUDE_TESTOVERWRITTENBYIBY
@echo.

:: Delete any existing sys$rom.bin file before renaming the generated ROM image to sys$rom.bin.
@if exist h4hrp_001.TextShell.IMG echo ** Renaming the generated ROM image from 'h4hrp_001.TextShell.IMG' to 'sys$rom.bin'.
if exist sys$rom.bin del sys$rom.bin
if exist h4hrp_001.TextShell.IMG ren h4hrp_001.TextShell.IMG sys$rom.bin
@echo.

@echo ** Please copy the sys$rom.bin ROM image to the MMC card to run the test on the hardware.
