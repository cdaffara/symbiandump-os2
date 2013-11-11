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

echo This script generates  pkcs12 files for Preq1054 integration testing
echo This script uses dergen.pl which in turn uses Openssl, you can get openssl from http://www.openssl.org/
echo It generates the pkcs12(.p12) files in the \security\cerman\tpkcs12intgrtn\data folder.
echo This script works if your security folder in your local machine like M:\security\.. otherwise you must change the datapath and folderpath according to you.


ECHO.
PAUSE.

set datapath=%SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\tder
set folderpath=%SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\tpkcs12intgrtn\testdatainput
set certpath = %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\testcertificates\certman\tpkcs12ubtgrtn\testdatainput

cd "%folderpath%"\cb001

call perl "%datapath%\dergen.pl" --IN pkcs12_cb001.txt --OUT ..\..\data\pkcs12_cb001.p12 

del *.der
del cb001_mac
del cb001_macsalt

echo pkcs12_cb001.p12 has been generated.

cd "%folderpath%"\cb002

call perl "%datapath%\dergen.pl" --IN pkcs12_cb002.txt --OUT ..\..\data\pkcs12_cb002.p12 

del *.der
del cb002_mac
del cb002_macsalt
del cb002_friendlyname

echo pkcs12_cb002.p12 has been generated.


cd "%folderpath%"\cb003

call perl "%datapath%\dergen.pl" --IN pkcs12_cb003.txt --OUT ..\..\data\pkcs12_cb003.p12 

del *.der
del cb003_mac
del cb003_macsalt

echo pkcs12_cb003.p12 has been generated.


cd "%folderpath%"\cb004

call perl "%datapath%\dergen.pl" --IN pkcs12_cb004.txt --OUT ..\..\data\pkcs12_cb004.p12 

del *.der
del cb004_mac
del cb004_macsalt

echo pkcs12_cb004.p12 has been generated.


cd "%folderpath%"\cb005

call perl "%datapath%\dergen.pl" --IN pkcs12_cb005.txt --OUT ..\..\data\pkcs12_cb005.p12 

del *.der
del cb005_mac
del cb005_macsalt

echo pkcs12_cb005.p12 has been generated.


cd "%folderpath%"\cb006

call perl "%datapath%\dergen.pl" --IN pkcs12_cb006.txt --OUT ..\..\data\pkcs12_cb006.p12 

del *.der
del cb006_mac
del cb006_macsalt

echo pkcs12_cb006.p12 has been generated.


cd "%folderpath%"\ci001

call perl "%datapath%\dergen.pl" --IN pkcs12_ci001.txt --OUT ..\..\data\pkcs12_ci001.p12 

del *.der
del ci001_mac
del ci001_macsalt

echo pkcs12_ci001.p12 has been generated.

cd "%folderpath%"\ci002

call perl "%datapath%\dergen.pl" --IN pkcs12_ci002.txt --OUT ..\..\data\pkcs12_ci002.p12 

del *.der
del ci002_mac
del ci002_macsalt

echo pkcs12_ci002.p12 has been generated.




cd "%folderpath%"\im001

call perl "%datapath%\dergen.pl" --IN pkcs12_im001.txt --OUT ..\..\data\pkcs12_im001.p12 

del *.der
del im001_mac
del im001_macsalt

echo pkcs12_im001.p12 has been generated.

cd "%folderpath%"\im002

call perl "%datapath%\dergen.pl" --IN pkcs12_im002.txt --OUT ..\..\data\pkcs12_im002.p12 

del *.der
del im002_mac
del im002_macsalt

echo pkcs12_im002.p12 has been generated.



cd "%folderpath%"\sb001

call perl "%datapath%\dergen.pl" --IN pkcs12_sb001.txt --OUT ..\..\data\pkcs12_sb001.p12 

del *.der
del sb001_mac
del sb001_macsalt

echo pkcs12_sb001.p12 has been generated.


cd "%folderpath%"\sb002

call perl "%datapath%\dergen.pl" --IN pkcs12_sb002.txt --OUT ..\..\data\pkcs12_sb002.p12 

del *.der
del sb002_mac
del sb002_macsalt

echo pkcs12_sb002.p12 has been generated.



cd "%folderpath%"\sb003

call perl "%datapath%\dergen.pl" --IN pkcs12_sb003.txt --OUT ..\..\data\pkcs12_sb003.p12 

del *.der
del sb003_mac
del sb003_macsalt

echo pkcs12_sb003.p12 has been generated.


echo negative tests

cd "%folderpath%"\attributeval_changed

call perl "%datapath%\dergen.pl" --IN attrvalchanged.txt --OUT ..\..\data\pkcs12_corruptedattrval.p12 

del *.der
del attrvalchangedfriendlyname
del attrvalchangedlocalkeyid

echo pkcs12_corruptedattrval.p12 has been generated.

cd "%folderpath%"\alg_sha1_wrongdigest

call perl "%datapath%\dergen.pl" --IN alg_sha1_wrongdigest.txt --OUT ..\..\data\pkcs12_digestcorrupted.p12 

del *.der

echo pkcs12_digestcorrupted.p12 has been generated.


cd "%folderpath%"\changedtags

call perl "%datapath%\dergen.pl" --IN changedtags.txt --OUT ..\..\data\pkcs12_changetag.p12 

del *.der

echo pkcs12_changetag.p12 has been generated.


cd "%folderpath%"\empty_contentinfo

call perl "%datapath%\dergen.pl" --IN empty_contentinfo.txt --OUT ..\..\data\pkcs12_emptycontentinfo.p12  

del *.der
del	empty_contentinfo_mac
del empty_contentinfo_salt

echo pkcs12_emptycontentinfo.p12  has been generated.


cd "%folderpath%"\empty_digest

call perl "%datapath%\dergen.pl" --IN empty_digest.txt --OUT ..\..\data\pkcs12_emptydigest.p12  

del *.der

echo pkcs12_emptydigest.p12  has been generated.


cd "%folderpath%"\empty_encryptedcontent

call perl "%datapath%\dergen.pl" --IN empty_encryptedcontent.txt --OUT ..\..\data\pkcs12_encryptedcontentabsent.p12  

del *.der

echo pkcs12_encryptedcontentabsent.p12  has been generated.


cd "%folderpath%"\empty_encrypteddata

call perl "%datapath%\dergen.pl" --IN empty_encrypteddata.txt --OUT ..\..\data\pkcs12_encrypteddataabsent.p12  

del *.der
del	empty_encrypteddata_mac
del	empty_encrypteddata_macsalt

echo pkcs12_encrypteddataabsent.p12  has been generated.


cd "%folderpath%"\encrypted_usingdiffalg

call perl "%datapath%\dergen.pl" --IN encrypted_usingdiffalg.txt --OUT ..\..\data\pkcs12_encryptedusingdifferentAlg.p12  

del *.der

echo pkcs12_encryptedusingdifferentAlg.p12  has been generated.


cd "%folderpath%"\authsafe_unsupportedoid

call perl "%datapath%\dergen.pl" --IN authsafe_unsupportedoid.txt --OUT ..\..\data\pkcs12_unsupportedcontentinfo.p12  

del *.der
del authsafe_unsupportedoid_mac
del authsafe_unsupportedoid_macsalt

echo pkcs12_unsupportedcontentinfo.p12  has been generated.


cd "%folderpath%"\encrypteddata_zeroiterationcount

call perl "%datapath%\dergen.pl" --IN encrypteddata_zeroiterationcount.txt --OUT ..\..\data\pkcs12_encryptedcontentiteration1.p12  

del *.der

echo pkcs12_encryptedcontentiteration1.p12  has been generated.


cd "%folderpath%"\encrypteddata_negiterationcount

call perl "%datapath%\dergen.pl" --IN encrypteddata_negiterationcount.txt --OUT ..\..\data\pkcs12_encryptedcontentiteration2.p12  

del *.der

echo pkcs12_encryptedcontentiteration2.p12  has been generated.


cd "%folderpath%"\shroudedbag_zeroiteration

call perl "%datapath%\dergen.pl" --IN shroudedbag_zeroiteration.txt --OUT ..\..\data\pkcs12_corruptedshroudediteration1.p12  

del *.der

echo pkcs12_corruptedshroudediteration1.p12  has been generated.


cd "%folderpath%"\shroudedbag_negiteration

call perl "%datapath%\dergen.pl" --IN shroudedbag_negiteration.txt --OUT ..\..\data\pkcs12_corruptedshroudediteration2.p12  

del *.der
del	shroudedbag_negiteration_mac
del	shroudedbag_negiteration_salt

echo pkcs12_corruptedshroudediteration2.p12  has been generated.


cd "%folderpath%"\unsupported_contentinfo

call perl "%datapath%\dergen.pl" --IN unsupported_contentinfo.txt --OUT ..\..\data\pkcs12_unsupportedcontentinfo.p12  

del *.der

echo pkcs12_unsupportedcontentinfo.p12  has been generated.

cd "%folderpath%"\unsupported_contentinfo

call perl "%datapath%\dergen.pl" --IN unsupported_contentinfo.txt --OUT ..\..\data\pkcs12_contenttype.p12 

del *.der

echo pkcs12_contenttype.p12  has been generated.

cd "%folderpath%"\corruptedversion

call perl "%datapath%\dergen.pl" --IN corruptedversion.txt --OUT ..\..\data\pkcs12_corruptedversion.p12

del *.der

echo pkcs12_corruptedversion.p12  has been generated.


cd "%folderpath%"\corrupted_salt_encrypted_data

call perl "%datapath%\dergen.pl" --IN corrupted_salt_encrypted_data.txt --OUT ..\..\data\pkcs12_corruptedSalt.p12

del *.der

echo pkcs12_corruptedSalt.p12  has been generated.


cd "%folderpath%"\unsupportedbag_oid

call perl "%datapath%\dergen.pl" --IN unsupportedbag_oid.txt --OUT ..\..\data\pkcs12_unsupportedbag.p12  

del *.der

echo pkcs12_unsupportedbag.p12  has been generated.


cd "%folderpath%"\encrypteddata_unsupportedversion

call perl "%datapath%\dergen.pl" --IN encrypteddata_unsupportedversion.txt --OUT ..\..\data\pkcs12_encryptedcontentversion.p12  

del *.der

echo pkcs12_encryptedcontentversion.p12  has been generated.


cd "%folderpath%"\md5_digestalg

call perl "%datapath%\dergen.pl" --IN md5_digestalg.txt --OUT ..\..\data\pkcs12_digestalg1.p12  

del *.der

echo pkcs12_digestalg1.p12  has been generated.


cd "%folderpath%"\digestalg_unsupported

call perl "%datapath%\dergen.pl" --IN digestalg_unsupported.txt --OUT ..\..\data\pkcs12_digestalg2.p12  

del *.der

echo pkcs12_digestalg2.p12  has been generated.


cd "%folderpath%"\pkcs12_macdatanotasequence

call perl "%datapath%\dergen.pl" --IN pkcs12_macdatanotasequence.txt --OUT ..\..\data\pkcs12_macdatanotasequence.p12  

del *.der

echo pkcs12_macdatanotasequence.p12  has been generated.


cd "%folderpath%"\pkcs12_macsaltnotoctetstring

call perl "%datapath%\dergen.pl" --IN pkcs12_macsaltnotoctetstring.txt --OUT ..\..\data\pkcs12_macsaltnotoctetstring.p12  

del *.der

echo pkcs12_macsaltnotoctetstring.p12 has been generated.


cd "%folderpath%"\pkcs12_digestalgorithmnotasequence

call perl "%datapath%\dergen.pl" --IN pkcs12_digestalgorithmnotasequence.txt --OUT ..\..\data\pkcs12_digestalgorithmnotasequence.p12  

del *.der

echo pkcs12_digestalgorithmnotasequence.p12 has been generated.


cd "%folderpath%"\pkcs12_digestinfonotasequence

call perl "%datapath%\dergen.pl" --IN pkcs12_digestinfonotasequence.txt --OUT ..\..\data\pkcs12_digestinfonotasequence.p12  

del *.der

echo pkcs12_digestinfonotasequence.p12 has been generated.


cd "%folderpath%"\pkcs12_macdata_nosalt_noiteration

call perl "%datapath%\dergen.pl" --IN pkcs12_macdata_nosalt_noiteration.txt --OUT ..\..\data\pkcs12_macdata_nosalt_noiteration.p12  

del *.der

echo pkcs12_macdata_nosalt_noiteration.p12 has been generated.


cd "%folderpath%"\pkcs12_digestalgnotsha1

call perl "%datapath%\dergen.pl" --IN pkcs12_digestalgnotsha1.txt --OUT ..\..\data\pkcs12_digestalgnotsha1.p12  

del *.der

echo pkcs12_digestalgnotsha1.p12 has been generated.


cd "%folderpath%"\pkcs12_macdata_iterationnotinteger

call perl "%datapath%\dergen.pl" --IN pkcs12_macdata_iterationnotinteger.txt --OUT ..\..\data\pkcs12_macdata_iterationnotinteger.p12  

del *.der

echo pkcs12_macdata_iterationnotinteger.p12 has been generated.


cd "%folderpath%"\macdata_negiteration

call perl "%datapath%\dergen.pl" --IN macdata_negiteration.txt --OUT ..\..\data\pkcs12_macdata_negiteration.p12  

del *.der

echo pkcs12_macdata_negiteration.p12  has been generated.


cd "%folderpath%"\pkcs12_pfx_notasequence

call perl "%datapath%\dergen.pl" --IN pkcs12_pfx_notasequence.txt --OUT ..\..\data\pkcs12_pfx_notasequence.p12  

del *.der

echo pkcs12_pfx_notasequence.p12 has been generated.


cd "%folderpath%"\pkcs12_version_notinteger

call perl "%datapath%\dergen.pl" --IN pkcs12_version_notinteger.txt --OUT ..\..\data\pkcs12_version_notinteger.p12  

del *.der

echo pkcs12_version_notinteger.p12 has been generated.


cd "%folderpath%"\pkcs12_pfx_missingcontents1

call perl "%datapath%\dergen.pl" --IN pkcs12_pfx_missingcontents1.txt --OUT ..\..\data\pkcs12_pfx_missingcontents1.p12  

echo pkcs12_pfx_missingcontents1.p12 has been generated.


cd "%folderpath%"\pkcs12_authsafe_notasequence

call perl "%datapath%\dergen.pl" --IN pkcs12_authsafe_notasequence.txt --OUT ..\..\data\pkcs12_authsafe_notasequence.p12  

del *.der

echo pkcs12_authsafe_notasequence.p12 has been generated.


cd "%folderpath%"\pkcs12_authsafe_oidnotoctetstring

call perl "%datapath%\dergen.pl" --IN pkcs12_authsafe_oidnotoctetstring.txt --OUT ..\..\data\pkcs12_authsafe_oidnotoctetstring.p12  

del *.der

echo pkcs12_authsafe_oidnotoctetstring.p12 has been generated.


cd "%folderpath%"\pkcs12_authsafe_contentnotoctetstring

call perl "%datapath%\dergen.pl" --IN pkcs12_authsafe_contentnotoctetstring.txt --OUT ..\..\data\pkcs12_authsafe_contentnotoctetstring.p12  

del *.der

echo pkcs12_authsafe_contentnotoctetstring.p12 has been generated.


cd "%folderpath%"\pkcs12_authsafe_contentnotsequence

call perl "%datapath%\dergen.pl" --IN pkcs12_authsafe_contentnotsequence.txt --OUT ..\..\data\pkcs12_authsafe_contentnotsequence.p12  

del *.der

echo pkcs12_authsafe_contentnotsequence.p12 has been generated.


cd "%folderpath%"\pkcs12_unsupported_contenttype

call perl "%datapath%\dergen.pl" --IN pkcs12_unsupported_contenttype.txt --OUT ..\..\data\pkcs12_unsupported_contenttype.p12  

del *.der

echo pkcs12_unsupported_contenttype.p12 has been generated.


cd "%folderpath%"\pkcs12_certbag_notasequence

call perl "%datapath%\dergen.pl" --IN pkcs12_certbag_notasequence.txt --OUT ..\..\data\pkcs12_certbag_notasequence.p12  

del *.der

echo pkcs12_certbag_notasequence.p12 has been generated.


cd "%folderpath%"\pkcs12_certbag_oidnotoctetstring

call perl "%datapath%\dergen.pl" --IN pkcs12_certbag_oidnotoctetstring.txt --OUT ..\..\data\pkcs12_certbag_oidnotoctetstring.p12  

del *.der

echo pkcs12_certbag_oidnotoctetstring.p12 has been generated.


cd "%folderpath%"\pkcs12_certbag_explicittagchanged

call perl "%datapath%\dergen.pl" --IN pkcs12_certbag_explicittagchanged.txt --OUT ..\..\data\pkcs12_certbag_explicittagchanged.p12  

del *.der

echo pkcs12_certbag_explicittagchanged.p12  has been generated.


cd "%folderpath%"\pkcs12_certbag_notoctetstring

call perl "%datapath%\dergen.pl" --IN pkcs12_certbag_notoctetstring.txt --OUT ..\..\data\pkcs12_certbag_notoctetstring.p12  

del *.der

echo pkcs12_certbag_notoctetstring.p12  has been generated.


cd "%folderpath%"\pkcs12_bagattributenotaseq

call perl "%datapath%\dergen.pl" --IN pkcs12_bagattributenotaseq.txt --OUT ..\..\data\pkcs12_bagattributenotaseq.p12  

del *.der

echo pkcs12_bagattributenotaseq.p12  has been generated.


cd "%folderpath%"\pkcs12_bagattributeoid_notoctet

call perl "%datapath%\dergen.pl" --IN pkcs12_bagattributeoid_notoctet.txt --OUT ..\..\data\pkcs12_bagattributeoid_notoctet.p12  

del *.der

echo pkcs12_bagattributeoid_notoctet.p12  has been generated.

cd "%folderpath%"\pkcs12_bagattributeval_notset

call perl "%datapath%\dergen.pl" --IN pkcs12_bagattributeval_notset.txt --OUT ..\..\data\pkcs12_bagattributeval_notset.p12  

del *.der

echo pkcs12_bagattributeoid_notoctet.p12  has been generated.


echo OOMTest

cd "%folderpath%"\oomencrypteddata

call perl "%datapath%\dergen.pl" --IN oomencrypteddata.txt --OUT ..\..\data\pkcs12_oomsafecontentencrypteddata.p12
  
del *.der

echo pkcs12_oomsafecontentencrypteddata.p12 has been generated.


rem data creation using Openssl starts

cd..

openssl  pkcs12 -export  -inkey %certpath%\root5ca_key.pem  -in %certpath%\root5ca.pem  -out ..\data\pkcs12_pm001.p12  -keypbe PBE-SHA1-RC2-40  -certpbe PBE-SHA1-RC2-40 -password pass:password

echo pkcs12_pm001.p12 generated ...

openssl  pkcs12 -export  -inkey %certpath%\root5ca_key.pem  -in %certpath%\root5ca.pem  -out ..\data\pkcs12_pm002.p12  -keypbe PBE-SHA1-RC4-128  -certpbe PBE-SHA1-RC4-128 -password pass:password

echo pkcs12_pm002.p12 generated ...

openssl  pkcs12 -export  -inkey %certpath%\root5ca_key.pem  -in %certpath%\root5ca.pem  -out ..\data\pkcs12_pm003.p12  -keypbe PBE-SHA1-RC4-40  -certpbe PBE-SHA1-RC4-40 -password pass:password

echo pkcs12_pm003.p12 generated ...

openssl  pkcs12 -export  -inkey root5ca_key.pem  -in %certpath%\root5ca.pem  -out ..\data\pkcs12_pm004.p12  -keypbe PBE-SHA1-3DES  -certpbe PBE-SHA1-3DES -password pass:password

echo pkcs12_pm004.p12 generated ...

openssl  pkcs12 -export  -inkey root5ca_key.pem  -in %certpath%\root5ca.pem  -out ..\data\pkcs12_pm005.p12  -keypbe PBE-SHA1-2DES  -certpbe PBE-SHA1-2DES -password pass:password

echo pkcs12_pm005.p12 generated ...

openssl  pkcs12 -export  -inkey root5ca_key.pem  -in %certpath%\root5ca.pem  -out ..\data\pkcs12_pm006.p12  -keypbe PBE-SHA1-RC2-128  -certpbe PBE-SHA1-RC2-128 -password pass:password

echo pkcs12_pm006.p12 generated ...

openssl  pkcs12 -export  -inkey root5ca_key.pem  -in %certpath%\root5ca.pem  -out ..\data\pkcs12_pm007.p12  -keypbe PBE-SHA1-RC2-40  -certpbe PBE-SHA1-RC2-40 -password pass:password

echo pkcs12_pm007.p12 generated ...

openssl  pkcs12 -export  -inkey root5ca_key.pem  -in %certpath%\root5ca.pem  -out ..\data\pkcs12_pm008.p12  -keypbe PBE-SHA1-RC2-40  -certpbe PBE-SHA1-RC2-40 -nomaciter -noiter -password pass:password

echo pkcs12_pm008.p12 generated ...

openssl  pkcs12 -export  -inkey root5ca_key.pem  -in %certpath%\root5ca.pem  -out ..\data\pkcs12_pm010.p12  -keypbe PBE-SHA1-RC2-40  -certpbe PBE-SHA1-RC2-40 -name Symbian -password pass:password

echo pkcs12_pm010.p12 generated ...

