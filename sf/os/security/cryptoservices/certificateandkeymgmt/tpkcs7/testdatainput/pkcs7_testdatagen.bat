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

@ECHO OFF
ECHO.

Echo This script generates  pkcs12 files for testing the enhancements made in PKCS7 to support PKCS12(Preq1054).
Echo This uses dergen.pl which intern uses Openssl, you can get openssl from http://www.openssl.org/

ECHO.
PAUSE.

set perlscriptpath=%SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\tder
set datapath=%SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\tpkcs7\testdatainput
set cert_path=%SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\tpkcs7\data

cd "%datapath%"\128bit_rc4
call perl "%perlscriptpath%\dergen.pl" --IN 128bit_rc4.txt --OUT SHA1And128BitRC4.der
copy sha1and128bitrc4.der %cert_path%\data\sha1and128bitrc4.der
copy sha1and128bitrc4_encryptedcontent.der %cert_path%\data\sha1and128bitrc4_encryptedcontent.der
copy sha1and128bitrc4_salt.der %cert_path%\data\sha1and128bitrc4_salt.der
del sha1and128bitrc4.der
del sha1and128bitrc4_encryptedcontent.der
del sha1and128bitrc4_salt.der
ECHO PKCS#7 Encrypted Data with 128BitRC4 is generated.


cd "%datapath%"\40bit_rc4
call perl "%perlscriptpath%\dergen.pl" --IN 40bit_rc4.txt --OUT sha1and40bitrc4.der
copy sha1and40bitrc4.der %cert_path%\data\sha1and40bitrc4.der
copy sha1and40bitrc4_encryptedcontent.der %cert_path%\data\sha1and40bitrc4_encryptedcontent.der
copy sha1and40bitrc4_salt.der %cert_path%\data\sha1and40bitrc4_salt.der
del sha1and40bitrc4.der
del sha1and40bitrc4_encryptedcontent.der
del sha1and40bitrc4_salt.der
ECHO PKCS#7 Encrypted Data with 40bitrc4 is generated.



cd "%datapath%"\128bit_rc2
call perl "%perlscriptpath%\dergen.pl" --IN 128bit_rc2.txt --OUT sha1and128bitrc2.der
copy sha1and128bitrc2.der %cert_path%\data\sha1and128bitrc2.der
copy sha1and128bitrc2_encryptedcontent.der %cert_path%\data\sha1and128bitrc2_encryptedcontent.der
copy sha1and128bitrc2_salt.der %cert_path%\data\sha1and128bitrc2_salt.der
del sha1and128bitrc2.der
del sha1and128bitrc2_encryptedcontent.der
del sha1and128bitrc2_salt.der
ECHO PKCS#7 Encrypted Data with 128bitrc2 is generated.



cd "%datapath%"\3key_tripledes
call perl "%perlscriptpath%\dergen.pl" --IN 3key_tripledes.txt --OUT sha1and3_keytripledes_cbc.der
copy sha1and3_keytripledes_cbc.der %cert_path%\data\sha1and3_keytripledes_cbc.der
copy sha1and3_keytripledes_cbc_encryptedcontent.der %cert_path%\data\sha1and3_keytripledes_cbc_encryptedcontent.der
copy sha1and3_keytripledes_cbc_salt.der %cert_path%\data\sha1and3_keytripledes_cbc_salt.der
del sha1and3_keytripledes_cbc.der
del sha1and3_keytripledes_cbc_encryptedcontent.der
del sha1and3_keytripledes_cbc_salt.der
ECHO PKCS#7 Encrypted Data with 3key_tripledes is generated.


cd "%datapath%"\40bit_rc2
call perl "%perlscriptpath%\dergen.pl" --IN 40bit_rc2.txt --OUT sha1and40bitrc2_cbc.der
copy sha1and40bitrc2_cbc.der %cert_path%\data\sha1and40bitrc2_cbc.der
copy sha1and40bitrc2_cbc_encryptedcontent.der %cert_path%\data\sha1and40bitrc2_cbc_encryptedcontent.der
copy sha1and40bitrc2_cbc_salt.der %cert_path%\data\sha1and40bitrc2_cbc_salt.der
del sha1and40bitrc2_cbc.der
del sha1and40bitrc2_cbc_encryptedcontent.der
del sha1and40bitrc2_cbc_salt.der
ECHO PKCS#7 Encrypted Data with 40bit_rc2 is generated.


cd "%datapath%"\notencrypteddata
call perl "%perlscriptpath%\dergen.pl" --IN nonencrypteddata.txt --OUT notencrypteddata.der
copy notencrypteddata.der %cert_path%\data\notencrypteddata.der
del notencrypteddata.der
ECHO PKCS#7 Data is generated.


cd "%datapath%"\2key_tripledes
call perl "%perlscriptpath%\dergen.pl" --IN 2key_tripledes.txt --OUT sha1and2_keytripledes.der
copy sha1and2_keytripledes.der %cert_path%\data\sha1and2_keytripledes.der
copy sha1and2_keytripledes_encryptedcontent.der %cert_path%\data\sha1and2_keytripledes_encryptedcontent.der
copy sha1and2_keytripledes_salt.der %cert_path%\data\sha1and2_keytripledes_salt.der
del sha1and2_keytripledes.der
del sha1and2_keytripledes_encryptedcontent.der
del sha1and2_keytripledes_salt.der
ECHO PKCS#7 Encrypted Data with 2key_tripledes is generated.


cd "%datapath%"\withoutencryptparams
call perl "%perlscriptpath%\dergen.pl" --IN withoutencryptparams.txt --OUT encrypteddata_withoutsalt.der
copy encrypteddata_withoutsalt.der %cert_path%\data\encrypteddata_withoutsalt.der
copy encrypteddata_withoutsalt_content.der %cert_path%\data\encrypteddata_withoutsalt_content.der
del encrypteddata_withoutsalt.der
del encrypteddata_withoutsalt_content.der
ECHO PKCS#7 Encrypted Data without encryptparams is generated.


cd "%datapath%"\version_notzero
call perl "%perlscriptpath%\dergen.pl"  --IN version_notzero.txt --OUT pkcs7_encryptedneg_version.der
copy pkcs7_encryptedneg_version.der %cert_path%\data\pkcs7_encryptedneg_version.der
del pkcs7_encryptedneg_version.der
ECHO PKCS#7 Encrypted Data with version 0 is generated.


cd "%datapath%"\contenttype_notdata
call perl "%perlscriptpath%\dergen.pl"  --IN contenttype_notdata.txt --OUT pkcs7_encryptedneg_contentType.der
copy pkcs7_encryptedneg_contentType.der %cert_path%\data\pkcs7_encryptedneg_contentType.der
del pkcs7_encryptedneg_contentType.der
ECHO PKCS#7 with contenttype_notdata is generated.


cd "%datapath%"\noencryptedcontent
call perl "%perlscriptpath%\dergen.pl"  --IN noencryptedcontent.txt --OUT pkcs7_encryptedneg_noencryptedcon.der
copy pkcs7_encryptedneg_nocontsalt.der %cert_path%\data\pkcs7_encryptedneg_nocontsalt.der
copy pkcs7_encryptedneg_noencryptedcon.der %cert_path%\data\pkcs7_encryptedneg_noencryptedcon.der
del pkcs7_encryptedneg_nocontsalt.der
del pkcs7_encryptedneg_noencryptedcon.der
ECHO PKCS#7 Encrypted Data without encrypted content is generated.


cd "%datapath%"\versiontag_changed
call perl "%perlscriptpath%\dergen.pl" --IN versiontag_changed.txt --OUT encrypteddata_versiontag.der
copy encrypteddata_versiontag.der %cert_path%\data\encrypteddata_versiontag.der
del encrypteddata_versiontag.der
ECHO PKCS#7 Encrypted Data with version tag is generated.


cd "%datapath%"\sha1_digestalg
call perl "%perlscriptpath%\dergen.pl" --IN authsafecontents.txt --OUT authsafecontents.der
call perl "%perlscriptpath%\dergen.pl"  --IN sha1_digestalg.TXT --OUT digestinfo_sha1.der 
copy digestinfo_sha1.der %cert_path%\data\digestinfo_sha1.der
copy digest_sha1.der %cert_path%\data\digest_sha1.der
del digestinfo_sha1.der
del digest_sha1.der
del authsafecontents.der
ECHO PKCS#7 DigestInfo with SHA1 is generated.


cd "%datapath%"\md5_digestalg
call perl "%perlscriptpath%\dergen.pl" --IN authsafecontents.txt --OUT authsafecontents.der
call perl "%perlscriptpath%\dergen.pl"  --IN md5_digestalg.txt --OUT digestinfo_md5alg.der 
copy digestinfo_md5alg.der %cert_path%\data\digestinfo_md5alg.der
copy pkcs7_digest_md5alg.der %cert_path%\data\pkcs7_digest_md5alg.der
del digestinfo_md5alg.der
del pkcs7_digest_md5alg.der
del authsafecontents.der
ECHO PKCS#7 DigestInfo with MD5 is generated.


cd "%datapath%"\digest_negalgtag
call perl "%perlscriptpath%\dergen.pl" --IN authsafecontents.txt --OUT authsafecontents.der
call perl "%perlscriptpath%\dergen.pl"  --IN digest_negalgtag.txt --OUT digestinfo_negalgtag.der
copy digestinfo_negalgtag.der %cert_path%\data\digestinfo_negalgtag.der
del digestinfo_negalgtag.der
del authsafecontents.der
ECHO PKCS#7 DigestInfo with Algorithm Tag changed is generated.


cd "%datapath%"\digest_negdigesttag
call perl "%perlscriptpath%\dergen.pl" --IN authsafecontents.txt --OUT authsafecontents.der
call perl "%perlscriptpath%\dergen.pl"  --IN digest_negdigesttag.txt --OUT digestinfo_digest_notoctetstring.der 
copy digestinfo_digest_notoctetstring.der %cert_path%\data\digestinfo_digest_notoctetstring.der
del digestinfo_digest_notoctetstring.der
del authsafecontents.der
ECHO PKCS#7 DigestInfo with Digest Tag Changed is generated.


cd "%datapath%"\digestinfo_withoutdigest
call perl "%perlscriptpath%\dergen.pl" --IN authsafecontents.txt --OUT authsafecontents.der
call perl "%perlscriptpath%\dergen.pl"  --IN digestinfo_withoutdigest.txt --OUT digestinfo_nodigest.der
copy digestinfo_nodigest.der %cert_path%\data\digestinfo_nodigest.der
del digestinfo_nodigest.der
del authsafecontents.der
ECHO PKCS#7 DigestInfo without Digest is generated.


cd "%datapath%"\unsupported_digestalg
call perl "%perlscriptpath%\dergen.pl" --IN authsafecontents.txt --OUT authsafecontents.der
call perl "%perlscriptpath%\dergen.pl"  --IN unsupported_digestalg.txt --OUT digestinfo_unsupportedalg.der
copy digestinfo_unsupportedalg.der %cert_path%\data\digestinfo_unsupportedalg.der
del authsafecontents.der
del digestinfo_unsupportedalg.der
ECHO PKCS#7 DigestInfo with unsupported Digest Algorithm is generated.

PAUSE.
