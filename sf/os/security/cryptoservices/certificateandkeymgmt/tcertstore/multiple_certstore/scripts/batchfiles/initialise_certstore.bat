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

ECHO Y | del \epoc32\release\winscw\udeb\z\private\101f72a6\cacerts*.dat
ECHO Y | del \epoc32\release\winscw\urel\z\private\101f72a6\cacerts*.dat
ECHO Y | del \epoc32\release\winscw\udeb\z\private\101f72a6\certclients*.dat
ECHO Y | del \epoc32\release\winscw\urel\z\private\101f72a6\certclients*.dat
ECHO Y | del \epoc32\winscw\c\private\101f72a6\*.dat
ECHO Y | del \epoc32\release\winscw\udeb\z\resource\swicertstore*.dat
ECHO Y | del \epoc32\release\winscw\urel\z\resource\swicertstore*.dat
ECHO Y | del \epoc32\winscw\c\resource\swicertstore\dat\swicertstore*.dat

rename \epoc32\data\epoc.ini epoc_temp.ini
rename \epoc32\data\epoc_multiplecertstore.ini epoc.ini

IF /i %1==initialisefilecertstore (
    IF /i %2==void GOTO END
    IF /i %2==corrupt (
       IF /i %3==middle ( 
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\cacerts%4.dat \epoc32\release\winscw\udeb\z\private\101f72a6\cacerts4.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\corrupt.dat \epoc32\release\winscw\udeb\z\private\101f72a6\cacerts3.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\cacerts%5.dat \epoc32\release\winscw\udeb\z\private\101f72a6\cacerts2.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\cacerts%4.dat \epoc32\release\winscw\urel\z\private\101f72a6\cacerts4.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\corrupt.dat \epoc32\release\winscw\urel\z\private\101f72a6\cacerts3.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\cacerts%5.dat \epoc32\release\winscw\urel\z\private\101f72a6\cacerts2.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\certclients%4.dat \epoc32\release\winscw\udeb\z\private\101f72a6\certclients4.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\corrupt.dat \epoc32\release\winscw\udeb\z\private\101f72a6\certclients3.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\certclients%5.dat \epoc32\release\winscw\udeb\z\private\101f72a6\certclients2.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\certclients%4.dat \epoc32\release\winscw\urel\z\private\101f72a6\certclients4.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\corrupt.dat \epoc32\release\winscw\urel\z\private\101f72a6\certclients3.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\certclients%5.dat \epoc32\release\winscw\urel\z\private\101f72a6\certclients2.dat
       ) ELSE (
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\corrupt.dat \epoc32\release\winscw\udeb\z\private\101f72a6\cacerts%3.dat 
       for %%i in (%4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\cacerts%%i.dat \epoc32\release\winscw\udeb\z\private\101f72a6\cacerts%%i.dat
       for %%i in (%4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\cacerts%%i.dat \epoc32\release\winscw\urel\z\private\101f72a6\cacerts%%i.dat
       for %%i in (%4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\certclients%%i.dat \epoc32\release\winscw\udeb\z\private\101f72a6\certclients%%i.dat
       for %%i in (%4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\certclients%%i.dat \epoc32\release\winscw\urel\z\private\101f72a6\certclients%%i.dat
       )
    GOTO END  
    ) ELSE (
      IF /i %2==renamed (
      copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\cacerts5.dat \epoc32\release\winscw\udeb\z\private\101f72a6\cacerts%3.dat 
      copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\cacerts5.dat \epoc32\release\winscw\urel\z\private\101f72a6\cacerts%3.dat 
      copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\certclients5.dat \epoc32\release\winscw\udeb\z\private\101f72a6\certclients%3.dat 
      copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\certclients5.dat \epoc32\release\winscw\urel\z\private\101f72a6\certclients%3.dat 
      for %%i in (%4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\cacerts%%i.dat \epoc32\release\winscw\udeb\z\private\101f72a6\cacerts%%i.dat
      for %%i in (%4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\cacerts%%i.dat \epoc32\release\winscw\urel\z\private\101f72a6\cacerts%%i.dat
      for %%i in (%4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\certclients%%i.dat \epoc32\release\winscw\udeb\z\private\101f72a6\certclients%%i.dat
      for %%i in (%4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\certclients%%i.dat \epoc32\release\winscw\urel\z\private\101f72a6\certclients%%i.dat
      GOTO END
      )
    )
    for %%a in (%2 %3 %4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\cacerts%%a.dat \epoc32\release\winscw\udeb\z\private\101f72a6\cacerts%%a.dat
    for %%a in (%2 %3 %4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\certclients%%a.dat \epoc32\release\winscw\udeb\z\private\101f72a6\certclients%%a.dat
    for %%a in (%2 %3 %4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\cacerts%%a.dat \epoc32\release\winscw\urel\z\private\101f72a6\cacerts%%a.dat
    for %%a in (%2 %3 %4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\certclients%%a.dat \epoc32\release\winscw\urel\z\private\101f72a6\certclients%%a.dat
    GOTO END
) 

REM ---------------------------------------------------------------------------------------------------------------------


IF /i %1==initialiseswicertstore (
    IF /i %2==void GOTO END
    
    IF /i %2==writable (
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore2.dat \epoc32\release\winscw\udeb\z\resource\swicertstore2.dat    
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore2.dat \epoc32\release\winscw\urel\z\resource\swicertstore2.dat    
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore3.dat \epoc32\winscw\c\resource\swicertstore\dat\swicertstore3.dat    
       GOTO END
    )
     
    IF /i %2==corrupt (
       IF /i %3==middle ( 
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore%4.dat \epoc32\release\winscw\udeb\z\resource\swicertstore4.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\corrupt.dat        \epoc32\release\winscw\udeb\z\resource\swicertstore3.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore%5.dat \epoc32\release\winscw\udeb\z\resource\swicertstore2.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore%4.dat \epoc32\release\winscw\urel\z\resource\swicertstore4.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\corrupt.dat        \epoc32\release\winscw\urel\z\resource\swicertstore3.dat
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore%5.dat \epoc32\release\winscw\urel\z\resource\swicertstore2.dat
       ) ELSE (
       copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\corrupt.dat \epoc32\release\winscw\udeb\z\resource\swicertstore%3.dat 
       for %%i in (%4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore%%i.dat \epoc32\release\winscw\udeb\z\resource\swicertstore%%i.dat
       for %%i in (%4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore%%i.dat \epoc32\release\winscw\urel\z\resource\swicertstore%%i.dat
       )
    GOTO END  
    ) ELSE (
      IF /i %2==renamed (
      copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore5.dat \epoc32\release\winscw\udeb\z\resource\swicertstore%3.dat 
      copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore5.dat \epoc32\release\winscw\urel\z\resource\swicertstore%3.dat 
      for %%i in (%4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore%%i.dat \epoc32\release\winscw\udeb\z\resource\swicertstore%%i.dat
      for %%i in (%4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore%%i.dat \epoc32\release\winscw\urel\z\resource\swicertstore%%i.dat
      GOTO END
      )
    )
    for %%a in (%2 %3 %4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore%%a.dat \epoc32\release\winscw\udeb\z\resource\swicertstore%%a.dat
    for %%a in (%2 %3 %4 %5 %6) do copy \epoc32\winscw\c\tcertstore\multiple_certstore\tdata\swicertstore%%a.dat \epoc32\release\winscw\urel\z\resource\swicertstore%%a.dat
    GOTO END
) 


:END
