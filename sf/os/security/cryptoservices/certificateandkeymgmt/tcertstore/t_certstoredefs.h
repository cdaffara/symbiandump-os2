/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __T_TCERTSTOREDEFS_H__
#define __T_TCERTSTOREDEFS_H__

#include <e32std.h>
#include <e32base.h>

//script file.Removed the hardcoded refernce c: from KCertStoreTestScript.
//Currently not refernced by any file.System drive to be appended when 
//KCertStoreTestScript is used.
_LIT(KCertStoreTestScript, "\\t_certstore.txt");

//output file,Removed the hardcoded refernce c: from KCertStoreOutput.
//Currently not refernced by any file.System drive to be appended when 
//KCertStoreOutput is used.
_LIT(KCertStoreOutput, "\\t_certstoreout.txt");

//action types
_LIT8(KCreateFileCertStore, "createfilecertstore");
_LIT8(KTestConcurrentCertStore, "testconcurrent");
_LIT8(KInitStore, "init");
_LIT8(KOnlyCreateStore, "onlycreate");
_LIT8(KDeleteStore, "delete");
_LIT8(KImportKey, "importkey");
_LIT8(KAddCert, "addcert");
_LIT8(KDeleteCert, "deletecert");
_LIT8(KDeleteRetrieveCACert, "deleteandretrievecacert");
_LIT8(KDeleteRetrieveUserCert, "deleteandretrieveusercert");
_LIT8(KListCert, "listcert");
_LIT8(KRetrieveCert, "retrieve");
_LIT8(KLoadUniqueCACert, "loaduniquecacert");
_LIT8(KLoadUniqueUserCert, "loaduniqueusercert");
_LIT8(KGetApplications, "getapplications");
_LIT8(KSetApplications, "setapplications");
_LIT8(KSetTruster, "settrust");
_LIT8(KGetTrusters, "gettrusters");
_LIT8(KInitAppManager, "initappmanager");
_LIT8(KDeleteAppManager, "deleteappmanager");
_LIT8(KAddClient, "addclient");
_LIT8(KGetClients, "getclients");
_LIT8(KRemoveClient, "removeclient");
_LIT8(KMemFail, "memfail");
_LIT8(KCUnifiedCertStoreGetCert, "CUnifiedCertStore GetCert");
_LIT8(KSetCapabilities, "setcapabilities");
_LIT8(KGetCapabilities, "getcapabilities");
_LIT8(KSetMandatory, "setmandatory");
_LIT8(KGetMandatory, "getmandatory");
_LIT8(KGetSystemUpgrade, "getsystemupgrade");
_LIT8(KSetDeletable, "setdeletable");
_LIT8(KGetDeletable, "getdeletable");
_LIT8(KInitSWICertStore, "initswicertstore");
_LIT8(KMultipleReaders, "multiplereaders");

// Action types for the key store
_LIT8(KUnifiedKeyStoreInit, "UnifiedKeyStore Init");
_LIT8(KUnifiedKeyStoreDelete, "UnifiedKeyStore Delete");
_LIT8(KUnifiedKeyStoreKeyStoreCount, "UnifiedKeyStore KeyStoreCount");
_LIT8(KUnifiedKeyStoreList, "UnifiedKeyStore List");
_LIT8(KCUnifiedCertStoreReadOnlyCertStoreCount, "CUnifiedCertStore ReadOnlyCertStoreCount");
_LIT8(KCUnifiedCertStoreWritableCertStoreCount, "CUnifiedCertStore WritableCertStoreCount");
_LIT8(KCUnifiedCertStoreCertStoreCount, "CUnifiedCertStore CertStoreCount");

_LIT8(KNumberOfStoresStart, "<numberofstores>");
_LIT8(KNumberOfStoresEnd, "</numberofstores>");

_LIT8(KCCTTokenTypeInfoAttributes, "CCTTokenTypeInfo Attributes");

_LIT8(KStoreIndexStart, "<storeindex>");
_LIT8(KStoreIndexEnd, "</storeindex>");

// Action types for CCTCertInfo
_LIT8(KCCTCertInfoHandle, "CCTCertInfo Handle");
_LIT8(KCTestCertForDeletable, "testcertfordeletable");

//tags

//tags for adding cert
_LIT8(KModeStart, "<mode>");
_LIT8(KModeEnd, "</mode>");
_LIT8(KCertFormatStart, "<format>");
_LIT8(KCertFormatEnd, "</format>");
_LIT8(KCertTypeStart,"<certtype>");
_LIT8(KCertTypeEnd,"</certtype>");
_LIT8(KReadOnlyStart, "<readonly>");
_LIT8(KReadOnlyEnd, "</readonly>");
_LIT8(KIssuerKeyStart, "<issuerkey>");
_LIT8(KIssuerKeyEnd, "</issuerkey>");
_LIT8(KSubjectKeyStart, "<subjectkey>");
_LIT8(KSubjectKeyEnd, "</subjectkey>");
_LIT8(KCertFileStart, "<certfile>");
_LIT8(KCertFileEnd, "</certfile>");
_LIT8(KCertOwnerTypeStart, "<certowner>");//ca or user
_LIT8(KCertOwnerTypeEnd, "</certowner>");
_LIT8(KStoreToUseStart, "<certstore>");
_LIT8(KStoreToUseEnd, "</certstore>");
_LIT8(KDeletableStart, "<deletable>");
_LIT8(KDeletableEnd, "</deletable>");
_LIT8(KSubActionStart, "<subaction>");
_LIT8(KSubActionEnd, "</subation>");
_LIT8(KTestNewCSClasswDeletable, "TestNewCSClasswDeletable");
_LIT8(KTestOldCSClasswoDeletable, "TestOldCSClasswoDeletable");
_LIT8(KTestSerialization, "TestSerialization");
_LIT8(KTrue, "True");
_LIT8(KFalse, "False");
_LIT8(KOrderingFilterStart, "<orderingfilter>");
_LIT8(KOrderingFilterEnd, "</orderingfilter>");


// tags for filter implementation
_LIT8(KFilterStart,"<filter>");
_LIT8(KFilterEnd,"</filter>");
_LIT8(KCertLabelStart, "<label>");
_LIT8(KCertLabelEnd, "</label>");
_LIT8(KUidStart,"<uid>");
_LIT8(KUidEnd,"</uid>");
_LIT8(KOwnerTypeStart,"<ownertype>");
_LIT8(KOwnerTypeEnd,"</ownertype>");
_LIT8(KKeyUsageDescStart, "<keyusagedescription>");
_LIT8(KKeyUsageDescEnd, "</keyusagedescription>");
_LIT8(KKeyUsageSchemeStart,"<keyusagescheme>");
_LIT8(KKeyUsageSchemeEnd,"</keyusagescheme>");
_LIT8(KKeyUsageStart, "<keyusage>");
_LIT8(KKeyUsageEnd, "</keyusage>");
_LIT8(KKeyTypeStart, "<keytype>");
_LIT8(KKeyTypeEnd, "</keytype>");
_LIT8(KKeyUsagePKCS,"pkcs15");
_LIT8(KKeyUsageX509,"x509");
_LIT8(KSubjectKeyIdStart,"<subjectkeyid>");
_LIT8(KSubjectKeyIdEnd,"</subjectkeyid>");
_LIT8(KIssuerKeyIdStart,"<issuerkeyid>");
_LIT8(KIssuerKeyIdEnd,"</issuerkeyid>");
_LIT8(KUseSubjectKeyIdStart,"<usesubjectkeyid>");
_LIT8(KUseSubjectKeyIdEnd,"</usesubjectkeyid>");

//  tag for Notification of Change
_LIT8(KChangeNotifiedStart, "<ChangeNotification>");
_LIT8(KChangeNotifiedEnd, "</ChangeNotification>");

//	tag for delete all certificates
_LIT8(KDeleteAllStart, "<deleteallcerts>");
_LIT8(KDeleteAllEnd, "</deleteallcerts>");
//cert owner type
_LIT8(KCACert, "ca");
_LIT8(KUserCert, "user");
_LIT8(KPeerCert, "peer");
//cert format
_LIT8(KX509, "X509");
_LIT8(KWTLS, "WTLS");
_LIT8(KX968, "X968");
_LIT8(KX509URL, "X509URL");
_LIT8(KWTLSURL, "WTLSURL");
_LIT8(KX968URL, "X968URL");
_LIT8(KUnknown, "unknown");

//tags for showing certs
_LIT8(KUIDStart, "<uid>");
_LIT8(KUIDEnd, "</uid>");
_LIT8(KFormatFlagsStart, "<formatflags>");
_LIT8(KFormatFlagsEnd, "</formatflags>");
_LIT8(KIssuerDNStart, "<issuerdn>");
_LIT8(KIssuerDNEnd, "</issuerdn>");

//tags for adding clients
_LIT8(KNameStart, "<name>");
_LIT8(KNameEnd, "</name>");

//tags for expected result
_LIT(KReturnStart, "<return>");
_LIT(KReturnEnd, "</return>");
_LIT8(KTrustersStart, "<trust>");
_LIT8(KTrustersEnd, "</trust>");
_LIT8(KOrderingResultStart, "<orderingresult>");
_LIT8(KOrderingResultEnd, "</orderingresult>");

//tags for filecertstore creation
_LIT8(KFileNameStart, "<filename>");
_LIT8(KFileNameEnd, "</filename>");

_LIT8(KTokenTypeStart, "<tokentype>");
_LIT8(KTokenTypeEnd, "</tokentype>");
_LIT8(KTokenStart, "<token>");
_LIT8(KTokenEnd, "</token>");
_LIT8(KObjectStart, "<object>");
_LIT8(KObjectEnd, "</object>");

_LIT8(KCCTCertInfoStart, "<CCTCertInfo>");
_LIT8(KCCTCertInfoEnd, "</CCTCertInfo>");

_LIT8(KMandatoryStart, "<mandatory>");
_LIT8(KMandatoryEnd, "</mandatory>");

_LIT8(KSystemUpgradeStart, "<systemupgrade>");
_LIT8(KSystemUpgradeEnd, "</systemupgrade>");


_LIT8(KDeletionStart, "<deletable>");
_LIT8(KDeletionEnd, "</deletable>");

/////////////////////
//	Keystore defs
////////////////////
// Used for generating filters based on the key usage for X509 certificate scheme
_LIT8(KAllKeyUsages, "allusages");
_LIT8(KDigitalSignature, "DigitalSignature");
_LIT8(KDS, "DS");
_LIT8(KNonRepudiation, "NonRepudiation");
_LIT8(KNR, "NR");
_LIT8(KKeyEncipherment, "KeyEncipherment");
_LIT8(KKE, "KE");
_LIT8(KDataEncipherment, "DataEncipherment");
_LIT8(KDE, "DE");
_LIT8(KKeyAgreement, "KeyAgreement");
_LIT8(KKA, "KA");
_LIT8(KKeyCertSign, "KeyCertSign");
_LIT8(KKCS, "KCS");
_LIT8(KCRLSign, "CRLSign");
_LIT8(KCRLS, "CRLS");
_LIT8(KEncipherOnly, "EncipherOnly");
_LIT8(KE, "E");
_LIT8(KDecipherOnly, "DecipherOnly");
_LIT8(KD, "D");

// Used for generating filters based on the key usage for PKCS15 scheme
_LIT8(KPublic, "public");
_LIT8(KPrivate, "private");
_LIT8(KAll, "all");

// key usage for public keys for PKCS15 scheme
_LIT8(KEncrypt, "Encrypt");
_LIT8(KVerify, "Verify");
_LIT8(KVerifyRecover, "VerifyRecover");
_LIT8(KDerive, "Derive");
_LIT8(KWrap, "Wrap");
// the last usage is for non-repudiation which has already been defined(KNonRepudiation).

// key usage for private keys for PKCS15 scheme
_LIT8(KDecrypt, "Decrypt");
_LIT8(KSign, "Sign");
_LIT8(KSignRecover, "SignRecover");
// usage for derive has already been defined (KDerived).
_LIT8(KUnwrap, "Unwrap");
// the last usage is for non-repudiation which has already been defined(KNonRepudiation).

// common combinations for private key usage
_LIT8(KSigning, "Signing");
_LIT8(KEncipherAndSign, "EncipherAndSign");
_LIT8(KAllKeyUsagesButNR, "AllUsagesButNR");

_LIT8(KDSAUsage, "DSAUsage");



_LIT8(KImportEncrypted, "<encrypted>");
_LIT8(KImportEncryptedEnd, "</encrypted>");
_LIT8(KImportDataFile, "<ImportData>");
_LIT8(KImportDataFileEnd, "</ImportData>");
_LIT8(KKeySizeStart, "<keysize>");
_LIT8(KKeySizeEnd, "</keysize>");
_LIT8(KKeyLabelStart, "<keylabel>");
_LIT8(KKeyLabelEnd, "</keylabel>");
_LIT8(KKeyAlgorithmStart, "<keyalgorithm>");
_LIT8(KKeyAlgorithmEnd, "</keyalgorithm>");
_LIT8(KKeyPassphraseStart, "<passphrase>");
_LIT8(KKeyPassphraseEnd, "</passphrase>");
_LIT8(KAlgRSA, "RSA");
_LIT8(KAlgDSA, "DSA");
_LIT8(KAlgDH, "DH");
_LIT8(KAlgBAD, "BAD");
_LIT8(KKeyExportableStart, "<keyexportable>");
_LIT8(KKeyExportableEnd, "</keyexportable>");
_LIT8(KIsExportable, "IsExportable");
_LIT8(KIsNotExportable, "IsNotExportable");
_LIT8(KKeyAccessTypeStart, "<keyaccesstype>");
_LIT8(KKeyAccessTypeEnd, "</keyaccesstype>");
_LIT8(KExtractable, "Extractable");
_LIT8(KSensitive, "Sensitive");
_LIT8(KAlwaysSensitive, "AlwaysSensitive");
_LIT8(KNeverExtractable, "NeverExtractable");
_LIT8(KLocal, "Local");

#endif
