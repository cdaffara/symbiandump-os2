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
copy /s z:\tpkcs7\ c:\tpkcs7\
testexecute c:\tpkcs7\tpkcs7_v2.script
move c:\logs\testexecute\tpkcs7_v2.htm e:\testresults\tpkcs7_v2.htm
testexecute c:\tpkcs7\tcms.script
move c:\logs\testexecute\tcms.htm e:\testresults\tcms.htm
testexecute c:\tpkcs7\tinteg_cms.script
move c:\logs\testexecute\tinteg_cms.htm e:\testresults\tinteg_cms.htm
del /s c:\tpkcs7\

rem TCERTSTORE
rem copy tcertstore test data
copy /s z:\tcertstore\ c:\tcertstore\
copy z:\certappmanager.txt c:\certappmanager.txt
copy /s z:\unifiedcertstore2\ c:\unifiedcertstore2\
copy z:\tcertstore\data\TestKeyStore.txt c:\system\data\testkeystore.txt
del c:\system\data\cacerts.dat

del c:\system\data\certclients.dat
t_certstore c:\tcertstore\scripts\t_certstore_sha2.script c:\testresults\t_certstore_sha2.htm
move c:\testresults\t_certstore_sha2.htm e:\testresults\t_certstore_sha2.htm

rem run tcertstore tests
t_certstore c:\tcertstore\scripts\unifiedcertstore1-conf1.script c:\logs\testexecute\unifiedcertstore1-conf1.htm
move c:\logs\testexecute\unifiedcertstore1-conf1.htm e:\testresults\unifiedcertstore1-conf1.htm

del c:\system\data\cacerts.dat
del c:\system\data\certclients.dat
t_certstore c:\tcertstore\scripts\unifiedcertstore2-conf1.script c:\logs\testexecute\unifiedcertstore2-conf1.htm
move c:\logs\testexecute\unifiedcertstore2-conf1.htm e:\testresults\unifiedcertstore2-conf1.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\unifiedcertstore3-conf1.script c:\logs\testexecute\unifiedcertstore3-conf1.htm
move c:\logs\testexecute\unifiedcertstore3-conf1.htm e:\testresults\unifiedcertstore3-conf1.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\dsa.script c:\logs\testexecute\tcertstore-dsa.htm
move c:\logs\testexecute\tcertstore-dsa.htm e:\testresults\tcertstore-dsa.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\reload.script c:\logs\testexecute\tcertstore-reload.htm
move c:\logs\testexecute\tcertstore-reload.htm e:\testresults\tcertstore-reload.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\apipolicing.script c:\logs\testexecute\tcertstore-apipolicing.htm
move c:\logs\testexecute\tcertstore-apipolicing.htm e:\testresults\tcertstore-apipolicing.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\filecertstore_errors.script c:\logs\testexecute\tcertstore-errors.htm
move c:\logs\testexecute\tcertstore-errors.htm e:\testresults\tcertstore-errors.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\swicertstore1.script c:\logs\testexecute\swicertstore1.htm
move c:\logs\testexecute\swicertstore1.htm e:\testresults\swicertstore1.htm

t_certstore c:\tcertstore\scripts\swicertstore4.script c:\logs\testexecute\swicertstore4.htm
move c:\logs\testexecute\swicertstore4.htm e:\testresults\swicertstore4.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\wri-swicertstore1.script c:\logs\testexecute\wri-swicertstore1.htm
move c:\logs\testexecute\wri-swicertstore1.htm e:\testresults\wri-swicertstore1.htm

del c:\system\data\cacerts.dat

REM Composite Certstore Configuration CR1393
del c:\private\101f72a6\cacerts.dat
del c:\private\101f72a6\certclients.dat
t_certstore c:\tcertstore\multiple_certstore\scripts\tfilecertstore14.script c:\logs\testexecute\tfilecertstore14.htm
move c:\logs\testexecute\tfilecertstore14.htm e:\testresults\tfilecertstore14.htm

del c:\private\101f72a6\cacerts.dat
del c:\private\101f72a6\certclients.dat

rem deleting data on c: to save room and leave slots in root folder
del /s c:\unifiedcertstore2\
del /s c:\tcertstore\
del c:\certappmanager.txt
attrib c:\*.* -r
del c:\*.*

rem TOCSP
rem copy tocsp test data
copy /s z:\system\tocsp\ c:\system\tocsp\
// copy this file to use wi-fi access point to connect to ocsp server
copy z:\private\10202be9\cccccc00_wifi.cre c:\private\10202be9\cccccc00.cre
del c:\system\data\cacerts.dat

rem run tocsp tests
tocsp c:\system\tocsp\scripts\OPENSSL.script c:\logs\testexecute\TOCSP_openssl.htm
del /s c:\system\tocsp\
copy /s z:\system\tocsp\ c:\system\tocsp\


tocsp c:\system\tocsp\scripts\XCERT-LOCAL.script c:\logs\testexecute\xcert_local.htm
del /s c:\system\tocsp\
copy /s z:\system\tocsp\ c:\system\tocsp\


tocsp c:\system\tocsp\scripts\OPENSSL-LOCAL.script c:\logs\testexecute\xcert_openssl_local.htm
del /s c:\system\tocsp\
copy /s z:\system\tocsp\ c:\system\tocsp\

tocsp c:\system\tocsp\scripts\error.script c:\logs\testexecute\ocsp_error.htm

TOCSP c:\SYSTEM\TOCSP\SCRIPTS\DefaultPolicy.script c:\logs\testexecute\TOCSP_DefaultPolicy.htm

rem copy test results to mmc card
move c:\logs\testexecute\TOCSP_openssl.htm e:\testresults\TOCSP_openssl.htm
move c:\logs\testexecute\xcert_local.htm e:\testresults\xcert_local.htm
move c:\logs\testexecute\xcert_openssl_local.htm e:\testresults\xcert_openssl_local.htm
move c:\logs\testexecute\ocsp_error.htm e:\testresults\ocsp_error.htm
move c:\logs\testexecute\TOCSP_DefaultPolicy.htm e:\testresults\TOCSP_DefaultPolicy.htm

rem deleting data on c: to save room and leave slots in root folder
del /s c:\system\tocsp\
attrib c:\*.* -r
del c:\*.*

rem TX509
rem copy tx509 test data
copy /s z:\tx509\ c:\tx509\

rem run tx509 tests
tx509 \TX509\SCRIPTS\TX509.script c:\logs\testexecute\tx509.htm
rem copy results to mmc card 
move c:\logs\testexecute\tx509.htm e:\testresults\tx509.htm
rem run tx509 distinguished name tests
tx509 \tx509\scripts\tx509-dnames.script c:\logs\testexecute\tx509_dnames.htm
rem copy results to mmc card
move c:\logs\testexecute\tx509_dnames.htm e:\testresults\tx509_dnames.htm
rem run tx509 dev certs tests
tx509 \TX509\SCRIPTS\TX509_DEV_CERTS.script c:\logs\testexecute\tx509_dev_certs.htm
rem copy results to mmc card
move c:\logs\testexecute\tx509_dev_certs.htm e:\testresults\tx509_dev_certs.htm
rem tidy up
del /s c:\tx509\
attrib c:\*.* -r
del c:\*.*

rem TASN1
rem run tasn1 tests
copy /s z:\tasn1\ c:\tasn1\
tasn1 c:\tasn1\scripts\tasn1.script c:\logs\testexecute\tasn1_log.htm
rem copy test results to mmc card
move c:\logs\testexecute\tasn1_log.htm e:\testresults\TASN1_log.htm
del /s c:\tasn1\


rem TWTLSCERT
rem copy twtlscert test data
copy /s z:\twtlscert\ c:\twtlscert\

rem run twtlscert tests
twtlscert c:\twtlscert\scripts\twtlscert.script c:\logs\testexecute\twtlscert.htm
twtlscert c:\twtlscert\scripts\twtlscert1.script c:\logs\testexecute\twtlscert1.htm
twtlscert c:\twtlscert\scripts\twtlscert2.script c:\logs\testexecute\twtlscert2.htm
twtlscert c:\twtlscert\scripts\twtlscert3.script c:\logs\testexecute\twtlscert3.htm

rem copy test results to mmc card 
move c:\logs\testexecute\twtlscert.htm e:\testresults\twtlscert.htm
move c:\logs\testexecute\twtlscert1.htm e:\testresults\twtlscert1.htm
move c:\logs\testexecute\twtlscert2.htm e:\testresults\twtlscert2.htm
move c:\logs\testexecute\twtlscert3.htm e:\testresults\twtlscert3.htm

del /s c:\twtlscert\
attrib c:\*.* -r
del c:\*.*

rem TPKIXCERT
rem copy tpkixcert test data
copy /s z:\tpkixcert\ c:\tpkixcert\
copy /s z:\pkixtestdata\ c:\pkixtestdata\

rem run tpkixcert tests
tpkixcert c:\tpkixcert\scripts\tpkixtest_part1.script c:\logs\testexecute\tpkixtest1.htm
tpkixcert c:\tpkixcert\scripts\tpkixtest_part2.script c:\logs\testexecute\tpkixtest2.htm
tpkixcert c:\tpkixcert\scripts\cancel1.script c:\logs\testexecute\cancel1.htm

rem copy test results to mmc card 
move c:\logs\testexecute\tpkixtest1.htm e:\testresults\tpkixtest1.htm
move c:\logs\testexecute\tpkixtest2.htm e:\testresults\tpkixtest2.htm
move c:\logs\testexecute\cancel1.htm e:\testresults\tpkixcancel1.htm

attrib c:\*.* -r
del c:\*.*

REM tpkcs10_v2 - Uses TEF
copy /s z:\tpkcs10\ c:\tpkcs10\
testexecute c:\tpkcs10\scripts\tpkcs10_v2.script
move c:\logs\testexecute\tpkcs10_v2.htm e:\testresults\tpkcs10_v2.htm
del /s c:\tpkcs10

REM tpkixcert_tef
copy /s z:\tpkixcert_tef\ c:\tpkixcert_tef\
testexecute c:\tpkixcert_tef\scripts\tpkixcert_tef.script
move c:\logs\testexecute\tpkixcert_tef.htm e:\testresults\tpkixcert_tef.htm
del /s c:\tpkixcert_tef

attrib c:\*.* -r
del c:\*.*

del c:\system\data\cacerts.dat
del c:\system\data\certclients.dat


 
