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

echo license
pause
certapp --license

echo bad arg
pause
certapp bad_arg

echo help
pause
certapp -h

echo tapi
pause
tapi - - -h

echo does not exist
pause
certapp --hcertclients=does_not_exist

echo duplicate labels
pause
rem duplicate app labels
certapp --bcertclients=certclients.dat --out --hcertclients=certclients.txt
certapp --hcertclients=certclients.txt --hcertclients=certclients.txt

echo Extract certs to swicertsore_cert*.der
pause
certapp --bswicertstore=swicertstore.dat --out --hswicertstore=swicertstore.txt

echo tcertappunit
pause
tcertappunit

pause
certapp --bcertclients=certclients.dat --hfilecertstore=generated.txt --out --hcertclients=certclients.txt --hfilecertstore=generated2.txt --bfilecertstore=generated.dat
pause

certapp --hcertclients=certclients.txt --out --hcertclients=certclients2.txt --bcertclients=certclients2.dat
pause
certapp --bcertclients=certclients.dat --bswicertstore=swicertstore.dat --out --hswicertstore=swicertstore2.txt --bswicertstore=swicertstore2.dat
pause
certapp --hcertclients=certclients.txt --hswicertstore=swicertstore2.txt 
pause
certapp --pemout --hcertclients=certclients.txt --hswicertstore=swi.txt --out --hswicertstore=swi2.txt --bswicertstore=swi2.dat
pause

certapp --hcertclients=certclients.txt --bfilecertstore=cacerts_new.dat --out --hfilecertstore=cacerts_new2.txt
pause
certapp --hcertclients=certclients.txt --hfilecertstore=cacerts_new2.txt --out --bfilecertstore=cacerts_new3.dat
pause
certapp --bcertclients=certclients.dat --hswicertstore=swi2.txt --out --hswicertstore=swi3.txt

pause
echo tcertapp
tcertapp
