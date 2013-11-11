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

:: Navigate to '...\featmgr\test\tef\tef_efm_integration\scripts\helpfiles' before
:: calling this batch file.

:: Build the textshell test ROM.
@echo ** Building the textshell test ROM...
call buildrom h4hrp textshell -fm=%EPOCROOT%epoc32/rom/include/featuredatabase.xml,featregverification_test.xml efm_integration_testserver.iby featregverification_test.iby

:: Delete any existing sys$rom.bin file before renaming the generated ROM image to sys$rom.bin.
@if exist h4hrp_001.TextShell.IMG echo ** Renaming the generated ROM image from 'h4hrp_001.TextShell.IMG' to 'sys$rom.bin'.
if exist sys$rom.bin del sys$rom.bin
if exist h4hrp_001.TextShell.IMG ren h4hrp_001.TextShell.IMG sys$rom.bin
@echo.

@echo ** Please copy the sys$rom.bin ROM image to the MMC card to run the test on the hardware.
