/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_TCERTSTOREDEFS_H__
#define __T_TCERTSTOREDEFS_H__

#include <e32std.h>
#include <e32base.h>
#include <securityerr.h>

_LIT(KTokenTypeClientPanic, "TOKENTYPECLIENT");

//action types
_LIT8(KCreateFileKeyStore, "createfilekeystore");

_LIT8(KInitStore, "init");
_LIT8(KOnlyCreateStore, "onlycreate");
_LIT8(KDeleteStore, "delete");
_LIT8(KAddKey, "addkey");
_LIT8(KAddUserKey, "adduserkey");
_LIT8(KListKeys, "listkeys");
_LIT8(KListAllKeys, "listallkeys");
_LIT8(KDeleteKeys, "deletekeys");
_LIT8(KImportKey, "importkey");
_LIT8(KExportKey, "exportkey");
_LIT8(KExportPublicKey, "exportpublickey");
_LIT8(KCompare, "compare");
_LIT8(KGetKeyInfo, "getkeyinfo");
_LIT8(KOpenKey, "open");
_LIT8(KMemFail, "memfail");
_LIT8(KTestOOM, "testoom");
_LIT8(KSign, "sign");
_LIT8(KDecrypt, "decrypt");
_LIT8(KDerive, "derive");
_LIT8(KDeleteKeystoreData, "deletekeystoredata");
_LIT8(KSetUsers, "setusers");
_LIT8(KSetTimeout, "settimeout");
_LIT8(KRelockStore, "relockstore");
_LIT8(KSetClientUid, "setclientuid");
_LIT8(KChangePassphrase, "changepassphrase");
_LIT8(KListProtectedObjects, "listprotectedobjects");
_LIT8(KUnblockPassphrase, "unblockpassphrase");
_LIT8(KAuthOpen, "authopen");
_LIT8(KAuthClose, "authclose");
_LIT8(KGetTimeRemaining, "timeremaining");
_LIT8(KAuthSetTimeout, "authsettimeout");
_LIT8(KGetTimeout, "gettimeout");
_LIT8(KSetTime, "settime");
_LIT8(KCorrupt, "corrupt");
_LIT8(KGetUsePolicy, "getusepolicy");
_LIT8(KSetUsePolicy, "setusepolicy");
_LIT8(KGetManagementPolicy, "getmanagementpolicy");
_LIT8(KSetManagementPolicy, "setmanagementpolicy");
_LIT8(KKeyStoreLabel, "getkeystorelabel");
#ifdef SYMBIAN_AUTH_SERVER
_LIT8(KAuthenticationPolicy, "authenticationpolicy");
#endif // SYMBIAN_AUTH_SERVER
// Action types for the key store
_LIT8(KUnifiedKeyStoreInit, "UnifiedKeyStore Init");
_LIT8(KUnifiedKeyStoreDelete, "UnifiedKeyStore Delete");
_LIT8(KUnifiedKeyStoreKeyStoreCount, "UnifiedKeyStore KeyStoreCount");
_LIT8(KUnifiedKeyStoreList, "UnifiedKeyStore List");

_LIT8(KNumberOfStoresStart, "<numberofstores>");
_LIT8(KNumberOfStoresEnd, "</numberofstores>");

_LIT8(KCCTTokenTypeInfoAttributes, "CCTTokenTypeInfo Attributes");

_LIT8(KStoreIndexStart, "<storeindex>");
_LIT8(KStoreIndexEnd, "</storeindex>");

_LIT8(KTextStart, "<text>");
_LIT8(KTextEnd, "</text>");

// tags for indexing keystores
_LIT8(KKeystoreStart, "<keystore>");
_LIT8(KKeystoreEnd, "</keystore>");

//tags
_LIT8(KModeStart, "<mode>");
_LIT8(KModeEnd, "</mode>");

//tags for adding clients
_LIT8(KNameStart, "<name>");
_LIT8(KNameEnd, "</name>");

//tags for expected result
_LIT8(KReturnStart, "<return>");
_LIT8(KReturnEnd, "</return>");

_LIT8(KTokenTypeStart, "<tokentype>");
_LIT8(KTokenTypeEnd, "</tokentype>");
_LIT8(KTokenStart, "<token>");
_LIT8(KTokenEnd, "</token>");
_LIT8(KObjectStart, "<object>");
_LIT8(KObjectEnd, "</object>");

_LIT8(KKeyUsageStart, "<keyusage>");
_LIT8(KKeyUsageEnd, "</keyusage>");

// Key usages
_LIT8(KKeyUsageNone, "none");
_LIT8(KAllKeyUsages, "allusages");
_LIT8(KEncryptUsage, "Encrypt");
_LIT8(KDecryptUsage, "Decrypt");
_LIT8(KSignUsage, "Sign");
_LIT8(KSignRecoverUsage, "SignRecover");
_LIT8(KWrapUsage, "Wrap");
_LIT8(KUnwrapUsage, "Unwrap");
_LIT8(KVerifyUsage, "Verify");
_LIT8(KVerifyRecoverUsage, "VerifyRecover");
_LIT8(KDeriveUsage, "Derive");
_LIT8(KNR, "NR");
_LIT8(KAllKeyUsagesExceptNR, "allusagesbutNR");
_LIT8(KDSAUsage, "DSAUsage");

_LIT8(KKeySizeStart, "<keysize>");
_LIT8(KKeySizeEnd, "</keysize>");

_LIT8(KKeyLabelStart, "<keylabel>");
_LIT8(KKeyLabelEnd, "</keylabel>");

_LIT8(KKeyAlgorithmStart, "<keyalgorithm>");
_LIT8(KKeyAlgorithmEnd, "</keyalgorithm>");
_LIT8(KAlgRSA, "RSA");
_LIT8(KAlgDSA, "DSA");
_LIT8(KAlgDH, "DH");
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
_LIT8(KAlgEcc,"ECC");
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
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

_LIT8(KKeyNativeStart, "<native>");
_LIT8(KKeyStartDateStart, "<startdate>");
_LIT8(KKeyEndDateStart, "<enddate>");

_LIT8(KListCountStart, "<listcount>");
_LIT8(KListCountEnd, "</listcount>");

_LIT8(KFoundKeyStart, "<foundkey>");
_LIT8(KFoundKeyEnd, "</foundkey>");

_LIT8(KDeleteCountStart, "<deletecount>");
_LIT8(KDeleteCountEnd, "</deletecount>");

_LIT8(KNonExistentKeyStart, "<nonexistent>");
_LIT8(KNonExistentKeyEnd, "</nonexistent>");

//	Tag for key import
_LIT8(KImportEncrypted, "<encrypted>");
_LIT8(KImportEncryptedEnd, "</encrypted>");

_LIT8(KImportDataFile, "<ImportData>");
_LIT8(KImportDataFileEnd, "</ImportData>");

_LIT8(KOpenStart, "<open>");
_LIT8(KOpenEnd, "</open>");

_LIT8(KSignDigestStart, "<signdigest>");
_LIT8(KSignDigestEnd, "</signdigest>");

// Tags for key export
_LIT8(KExportDataFile, "<ExportFile>");
_LIT8(KExportDataFileEnd, "</ExportFile>");

_LIT8(KExportEncrypted, "<encrypted>");
_LIT8(KExportEncryptedEnd, "</encrypted>");

_LIT8(KExportEncryptedCipher, "<cipher>");
_LIT8(KExportEncryptedCipherEnd, "</cipher>");

_LIT8(KExportCipherKeyLength, "<keylength>");
_LIT8(KExportCipherKeyLengthEnd, "</keylength>");

_LIT8(KExportCipherItarations, "<iterations>");
_LIT8(KExportCipherItarationsEnd, "</iterations>");


// Tags for compare
_LIT8(KOriginalFile, "<originalfile>");
_LIT8(KOriginalFileEnd, "</originalfile>");
_LIT8(KNewFile, "<newfile>");
_LIT8(KNewFileEnd, "</newfile>");

// t_secdlg tags
_LIT8(KSecDlg, "<passphrase>");
_LIT8(KSecDlgEnd, "</passphrase>");
_LIT(KSecDlgImport, "import");
_LIT(KSecDlgExport, "export");
_LIT(KSecDlgChange, "change");
_LIT(KSecDlgCreate, "create");

// t_secdlg filenames
_LIT(KTSecDlgInputFile, "\\t_secdlg_in.dat");
_LIT(KTSecDlgOutputFile, "\\t_secdlg_out.dat");
//_LIT8(KExportPassphrase, "<exportpassphrase>");
//_LIT8(KExportPassphraseEnd, "</exportpassphrase>");

// t_secdlg operations
_LIT(KTSecDlgEnterPIN, "EEnterPIN");
_LIT(KTSecDlgChangePIN, "EChangePIN");

// Tag for server OOM testing
_LIT8(KServerOOMTest, "<serveroom>");
_LIT8(KServerOOMTestEnd, "</serveroom>");

//	Tag for cancel testing
_LIT8(KCancelCreateKeyStart, "<cancelcreatekey>");
_LIT8(KCancelCreateKeyEnd, "</cancelcreatekey>");

_LIT8(KNoCancel, "NoCancel");
_LIT8(KPreCreateKeyCall, "PreCreateCancel");
_LIT8(KPostCreateKeyCall, "PostCreateCancel");

//	strings representing potential errors returned
_LIT8(KErrNoneString, "KErrNone");
_LIT8(KErrAccessDeniedString, "KErrAccessDenied");
_LIT8(KErrNotReadyString, "KErrNotReady");
_LIT8(KErrAlreadyExistsString, "KErrAlreadyExists");
_LIT8(KErrInUseString, "KErrInUse");
_LIT8(KErrNotFoundString, "KErrNotFound");
_LIT8(KErrBadNameString, "KErrBadName");
_LIT8(KErrArgumentString, "KErrArgument");
_LIT8(KErrCancelString, "KErrCancel");
_LIT8(KErrCorruptString, "KErrCorrupt");
_LIT8(KErrNotSupportedString, "KErrNotSupported");
_LIT8(KErrOverflowString, "KErrOverflow");
_LIT8(KErrBadPassphraseString, "KErrBadPassphrase");
_LIT8(KErrGeneralString, "KErrGeneral");
_LIT8(KErrKeyAlgorithmString, "KErrKeyAlgorithm");
_LIT8(KErrKeyUsageString, "KErrKeyUsage");
_LIT8(KErrKeyValidityString, "KErrKeyValidity");
_LIT8(KErrKeySizeString, "KErrKeySize");
_LIT8(KErrKeyAccessString, "KErrKeyAccess");
_LIT8(KErrPrivateKeyNotFoundString, "KErrPrivateKeyNotFound");
_LIT8(KErrPermissionDeniedString, "KErrPermissionDenied");
_LIT8(KErrAuthFailureString, "KErrAuthenticationFailure");
_LIT8(KErrUnknownAuthStrengthAliasString, "KErrUnknownAuthStrengthAlias"); 
_LIT8(KErrNoMemoryString, "KErrNoMemory");


// tags for auth object testing
_LIT8(KTimeoutStart, "<timeout>");
_LIT8(KOwnerStart, "<owner>");
_LIT8(KUserStart, "<user>");
_LIT8(KUserEnd, "</user>");
_LIT8(KUidStart, "<uid>");

// Corruption tag
_LIT8(KCorruptData, "<corruption>");
_LIT8(KCorruptDataEnd, "</corruption>");

_LIT8(KHomeTimeStart, "<hometime>");

_LIT8(KPolicyStart, "<policy>");

// Location of keystore data file
_LIT(KKeystoreDataPath, "\\private\\101F72A6\\keys.dat");

// tags for enabling interface count check
_LIT8(KCheckStoreCountStart, "<CheckStoreCount>");
_LIT8(KCheckStoreCountEnd, "</CheckStoreCount>");

// tags for checking manager interface count
_LIT8(KManagerCountStart, "<KeyStoreManagerCount>");
_LIT8(KManagerCountEnd, "</KeyStoreManagerCount>");

// tags for checking total interface count
_LIT8(KStoreCountStart, "<KeyStoreCount>");
_LIT8(KStoreCountEnd, "</KeyStoreCount>");

// tags for enabling interface count check
_LIT8(KCheckKeyStoreLabelStart, "<CheckKeyStoreLabel>");
_LIT8(KCheckKeyStoreLabelEnd, "</CheckKeyStoreLabel>");

// tags for retrieving key store label
_LIT8(KKeyStoreIndexStart, "<keystoreindex>");
_LIT8(KKeyStoreIndexEnd, "</keystoreindex>");

// tags for retrieving key store label
_LIT8(KKeyStoreLabelStart, "<keystorelabel>");
_LIT8(KKeyStoreLabelEnd, "</keystorelabel>");

// tags for disabling dialog check
_LIT8(KDisableDialogStart, "<disabledialogcheck>");
_LIT8(KDisableDialogEnd, "</disabledialogcheck>");

_LIT8(KListingStatusStart,"<listingstatus>");
_LIT8(KListingStatusEnd,"</listingstatus>");
_LIT8(KErrListingFailure,"fail");
_LIT8(KErrListingSuccess,"pass");

_LIT8(KUseKeyStoreStart,"<usekeystore>");
_LIT8(KUseKeyStoreEnd,"</usekeystore>");

#ifdef SYMBIAN_AUTH_SERVER
// tags for authentication expression
_LIT8(KAuthExpressionStart, "<authexpression>");
_LIT8(KAuthExpressionEnd, "</authexpression>");

// tags for freshness
_LIT8(KFreshnessStart, "<freshness>");
_LIT8(KFreshnessEnd, "</freshness>");

// tags for freshness
_LIT8(KNegativeFreshnessStart, "<negativefreshness>");
_LIT8(KNegativeFreshnessEnd, "</negativefreshness>");

// tags for authentication expression
_LIT8(KExpectedExpressionStart, "<expectedexpression>");
_LIT8(KExpectedExpressionEnd, "</expectedexpression>");

// tags for freshness
_LIT8(KExpectedFreshnessStart, "<expectedfreshness>");
_LIT8(KExpectedFreshnessEnd, "</expectedfreshness>");

_LIT8(KAuthModeStart, "<authmode>");
_LIT8(KAuthModeEnd, "</authmode>");

_LIT8(KAuthenticateStart, "<authenticate>");
_LIT8(KAuthenticateEnd, "</authenticate>");

#endif // SYMBIAN_AUTH_SERVER

#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
_LIT8(KHwTypeStart, "<hwtype>");
_LIT8(KHwTypeEnd, "</hwtype>");
_LIT8(KPublicKeyStart, "<publickey>");
_LIT8(KPublicKeyEnd, "</publickey>");
_LIT8(KPrivateKeyStart, "<privatekey>");
_LIT8(KPrivateKeyEnd, "</privatekey>");
_LIT8(KOperationTypeStart, "<operationtype>");
_LIT8(KOperationTypeEnd, "</operationtype>");

#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC

#endif	//	__T_TCERTSTOREDEFS_H__
