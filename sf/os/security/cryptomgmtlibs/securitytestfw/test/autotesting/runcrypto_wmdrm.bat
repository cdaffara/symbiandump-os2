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

REM hash
copy /s z:\thash\ c:\thash\
thash
move c:\hashlog.txt c:\testresults\
move c:\testresults\hashlog.txt e:\testresults\hashlog.txt
del /s c:\thash\

REM random
t_random
move c:\t_random.log c:\testresults\
move c:\testresults\t_random.log e:\testresults\t_random.log

REM padding - uses testexecute framework
copy /s z:\tpadding\ c:\tpadding\
testexecute c:\tpadding\tpadding.script
move c:\logs\testexecute\tpadding.htm e:\testresults\tpadding.htm
del /s c:\tpadding\

REM new cryptospi tests
copy /s z:\testdata\tcryptospi\ c:\testdata\tcryptospi\
testexecute z:\testdata\tcryptospi\scripts\tcryptospi.script
move c:\logs\testexecute\tcryptospi.htm e:\testresults\tcryptospi.htm

testexecute z:\testdata\tcryptospi\scripts\te_dummyeccciphertestcases.script
move c:\logs\testexecute\te_dummyeccciphertestcases.htm e:\testresults\te_dummyeccciphertestcases.htm

del /s c:\testdata\tcryptospi\

REM crypto, asymmetric

REM tasymmetric z:\tasymmetric\asymmetricPerformance.txt c:\testresults\asymmetricPerformance.log

tasymmetric z:\tasymmetric\tasymmetrictests.script c:\logs\testexecute\tasymmetrictests.htm
move c:\logs\testexecute\tasymmetrictests.htm e:\testresults\tasymmetrictests.htm

REM crypto, symmetric

tsymmetric z:\tsymmetric\tsymmetrictests.script c:\logs\testexecute\tsymmetrictests.htm
move c:\logs\testexecute\tsymmetrictests.htm e:\testresults\tsymmetrictests.htm

tsymmetric z:\tsymmetric\AESECBVectorsScript.script c:\logs\testexecute\AESECBVectorsScript.htm
move c:\logs\testexecute\AESECBVectorsScript.htm e:\testresults\AESECBVectorsScript.htm

tsymmetric z:\tsymmetric\AESECB_KAT_VT.script c:\logs\testexecute\AESECB_KAT_VT.htm
move c:\logs\testexecute\AESECB_KAT_VT.htm e:\testresults\AESECB_KAT_VT.htm

tsymmetric z:\tsymmetric\AESECB_KAT_VK.script c:\logs\testexecute\AESECB_KAT_VK.htm
move c:\logs\testexecute\AESECB_KAT_VK.htm e:\testresults\AESECB_KAT_VK.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptECB128.script c:\logs\testexecute\AESMonteCarloEncryptECB128.htm
move c:\logs\testexecute\AESMonteCarloEncryptECB128.htm e:\testresults\AESMonteCarloEncryptECB128.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptECB192.script c:\logs\testexecute\AESMonteCarloEncryptECB192.htm
move c:\logs\testexecute\AESMonteCarloEncryptECB192.htm e:\testresults\AESMonteCarloEncryptECB192.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptECB256.script c:\logs\testexecute\AESMonteCarloEncryptECB256.htm
move c:\logs\testexecute\AESMonteCarloEncryptECB256.htm e:\testresults\AESMonteCarloEncryptECB256.htm

tsymmetric z:\tsymmetric\AESMonteCarloDecryptECB128.script c:\logs\testexecute\AESMonteCarloDecryptECB128.htm
move c:\logs\testexecute\AESMonteCarloDecryptECB128.htm e:\testresults\AESMonteCarloDecryptECB128.htm

tsymmetric z:\tsymmetric\AESMonteCarloDecryptECB192.script c:\logs\testexecute\AESMonteCarloDecryptECB192.htm
move c:\logs\testexecute\AESMonteCarloDecryptECB192.htm e:\testresults\AESMonteCarloDecryptECB192.htm

tsymmetric z:\tsymmetric\AESMonteCarloDecryptECB256.script c:\logs\testexecute\AESMonteCarloDecryptECB256.htm
move c:\logs\testexecute\AESMonteCarloDecryptECB256.htm e:\testresults\AESMonteCarloDecryptECB256.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptCBC128.script c:\logs\testexecute\AESMonteCarloEncryptCBC128.htm
move c:\logs\testexecute\AESMonteCarloEncryptCBC128.htm e:\testresults\AESMonteCarloEncryptCBC128.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptCBC192A.script c:\logs\testexecute\AESMonteCarloEncryptCBC192A.htm
move c:\logs\testexecute\AESMonteCarloEncryptCBC192A.htm e:\testresults\AESMonteCarloEncryptCBC192A.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptCBC192B.script c:\logs\testexecute\AESMonteCarloEncryptCBC192B.htm
move c:\logs\testexecute\AESMonteCarloEncryptCBC192B.htm e:\testresults\AESMonteCarloEncryptCBC192B.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptCBC256.script c:\logs\testexecute\AESMonteCarloEncryptCBC256.htm
move c:\logs\testexecute\AESMonteCarloEncryptCBC256.htm e:\testresults\AESMonteCarloEncryptCBC256.htm

tsymmetric z:\tsymmetric\AESMonteCarloDecryptCBC128.script c:\logs\testexecute\AESMonteCarloDecryptCBC128.htm
move c:\logs\testexecute\AESMonteCarloDecryptCBC128.htm e:\testresults\AESMonteCarloDecryptCBC128.htm

tsymmetric z:\tsymmetric\AESMonteCarloDecryptCBC192.script c:\logs\testexecute\AESMonteCarloDecryptCBC192.htm
move c:\logs\testexecute\AESMonteCarloDecryptCBC192.htm e:\testresults\AESMonteCarloDecryptCBC192.htm

tsymmetric z:\tsymmetric\AESMonteCarloDecryptCBC256.script c:\logs\testexecute\AESMonteCarloDecryptCBC256.htm
move c:\logs\testexecute\AESMonteCarloDecryptCBC256.htm e:\testresults\AESMonteCarloDecryptCBC256.htm



REM crypto, bigint

tbigint z:\tbigint\tconstructiontests.script c:\logs\testexecute\tconstruction.htm
move c:\logs\testexecute\tconstruction.htm e:\testresults\tconstruction.htm

tbigint z:\tbigint\tbasicmathstests.script c:\logs\testexecute\tbasicmaths.htm
move c:\logs\testexecute\tbasicmaths.htm e:\testresults\tbasicmaths.htm

tbigint z:\tbigint\tmontgomerytests.script c:\logs\testexecute\tmontgomery.htm
move c:\logs\testexecute\tmontgomery.htm e:\testresults\tmontgomery.htm

REM tbigint z:\tbigint\tperformancetests.script c:\logs\testexecute\tperformance.htm
REM move c:\logs\testexecute\tperformance.htm e:\testresults\tperformance.htm

tbigint z:\tbigint\tprimetests.script c:\logs\testexecute\tprime.htm
move c:\logs\testexecute\tprime.htm e:\testresults\tprime.htm

REM tbigint z:\tbigint\tprimetests2.script c:\logs\testexecute\tprime2.htm
REM move c:\logs\testexecute\tprime2.htm e:\testresults\tprime2.htm

tbigint z:\tbigint\tprimefailuretests.script  c:\logs\testexecute\tprimefailure.htm
move c:\logs\testexecute\tprimefailure.htm e:\testresults\tprimefailure.htm

tbigint z:\tbigint\tprimegen.script c:\logs\testexecute\tprimegen.htm
move c:\logs\testexecute\tprimegen.htm e:\testresults\tprimegen.htm



REM crypto, tpkcs5kdf
md c:\tpkcs5kdf

copy z:\tpkcs12kdftests.script c:\tpkcs5kdf\tpkcs12kdftests.script
tpkcs5kdf c:\tpkcs5kdf\tpkcs12kdftests.script c:\tpkcs5kdf\tpkcs12kdftests.htm
move c:\tpkcs5kdf\tpkcs12kdftests.htm e:\testresults\tpkcs12kdftests.htm
del /s c:\tpkcs5kdf\


REM crypto, pbe

md c:\tpbe
copy z:\tpbe\*.dat c:\tpbe
attrib c:\tpbe\strong.dat -r
attrib c:\tpbe\weak.dat -r
tpbe z:\tpbe\tpbetests_v2.script c:\logs\testexecute\tpbetestsv2.htm
move c:\logs\testexecute\tpbetestsv2.htm e:\testresults\tpbetestsv2.htm

del /s c:\tpbe\
