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

IF NOT EXIST "\usewinpcap.txt" GOTO SKIP_WINPCAP
IF NOT EXIST "\epoc32\winscw\c\auto_winpcap.cfg" GOTO SKIP_WINPCAP
attrib -r \epoc32\winscw\c\auto.cfg 
copy /Y \epoc32\winscw\c\auto_winpcap.cfg \epoc32\winscw\c\auto.cfg 
attrib -r \epoc32\winscw\c\prompt.cfg 
copy /Y \epoc32\winscw\c\auto_winpcap_prompt.cfg \epoc32\winscw\c\prompt.cfg 
IF (%WINSCW_MAC%)==() (
call perl \epoc32\tools\configchange.pl --config ethernetNoCommDB --kernel EKA2 --target winscw --variant UDEB
) ELSE (
call perl \epoc32\winscw\c\configchangebats.pl --config ethernetNoCommDB --kernel EKA2 --target winscw --variant UDEB
)
GOTO CONFIG_DONE

:SKIP_WINPCAP

IF NOT EXIST "\usewintap.txt" GOTO SKIP_WINTAP
IF NOT EXIST "\epoc32\winscw\c\auto_wintap.cfg" GOTO SKIP_WINTAP
attrib -r \epoc32\winscw\c\auto.cfg
copy /Y \epoc32\winscw\c\auto_wintap.cfg \epoc32\winscw\c\auto.cfg 
attrib -r \epoc32\winscw\c\prompt.cfg 
copy /Y \epoc32\winscw\c\auto_wintap_prompt.cfg \epoc32\winscw\c\prompt.cfg 
GOTO CONFIG_DONE

:SKIP_WINTAP

:CONFIG_DONE
rem Set up tests using the automatic connection commdb configuration

call \epoc32\release\winscw\udeb\ced -dtextshell -- -i c:\auto.cfg
call \epoc32\release\wins\udeb\ced  -dtextshell --  -i c:\auto.cfg