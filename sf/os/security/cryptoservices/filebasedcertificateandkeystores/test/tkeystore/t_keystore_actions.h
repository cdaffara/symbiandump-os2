/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef __T_KEYSTORE_ACTIONS_H__
#define __T_KEYSTORE_ACTIONS_H__

#include "t_testhandler.h"
#include <e32cons.h>
#include <badesca.h>
#include "t_testaction.h"
#include "t_output.h"
#include "clientsession.h"
#include <hash.h>
#include <mctkeystore.h>
#include <unifiedkeystore.h>
#include <secdlgimpldefs.h>
#include <bigint.h>

typedef RPointerArray<CUnifiedKeyStore> RUnifiedKeyStoreArray;

class CPBEncryptParms;

enum TKSPanicCode
{
	EKSTestNotInitialised,
	EKSTestUnexpected,
	EKSTestResetPanic
};

class CSharedKeyStores : public CBase
{
public:
	static void InitialiseKeyStoresL();
	static void DestroyKeyStores();
	static RUnifiedKeyStoreArray& TheUnifiedKeyStores();
private:
	CSharedKeyStores(){};
	~CSharedKeyStores();
private:
	static CSharedKeyStores* iTheKeyStores;
private:
	RUnifiedKeyStoreArray iUnifiedKeyStores;
};

class CKeyStoreTestAction : public CTestAction
{
public:
	virtual ~CKeyStoreTestAction();
protected:
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	CKeyStoreTestAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	TInt SetExpectedResultL(const TDesC8& aResult);
	TBool SetKeyUsageL(const TDesC8& aKeyUsage);
	void SetKeyLabel(const TDesC8& aKeyLabel);
	void SetKeyAlgorithm(const TDesC8& aKeyAlgorithm);
	TBool SetKeyAccessType(const TDesC8& aKeyAccessType);
	void DoPerformPrerequisite(TRequestStatus& aStatus);
	void DoPerformPostrequisite(TRequestStatus& aStatus);
	void AfterOOMFailure();
	TBool AddExpectedDialogL(const TDesC8& aData);
	void WriteExpectedDialogDataL();
	void WriteDialogRecordL(RFileWriteStream& aStream, TSecurityDialogOperation aOp, const TDesC& aLabelSpec,
							const TDesC& aResponse1, const TDesC& aResponse2);
	TInt ReadDialogCountL();
	void PrintKeyInfoL(const CCTKeyInfo& aKey);
	void SetKeyType(const TDesC8& aKeyType);
	void SetKeyStoreIndex(CUnifiedKeyStore* aUnifiedKeyStore);
	
public:
	enum TKeyTypes
		{
		ERSASign,
		EDSASign,
		EDH,
		EDecrypt,
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
		EECC,
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
		};
protected:
	TUint iServerAlloc;
	RFs& iFs;
	TKeyUsagePKCS15 iUsage;
	HBufC* iLabel;
	CCTKeyInfo::EKeyAlgorithm iAlgorithm;
	TInt iAccessType;
	RPointerArray<HBufC> iExpectedDialogs;
	TInt iKeystore;

	// The following are here because they are used by most derived test actions
	TCTKeyAttributeFilter iFilter;
	RMPointerArray<CCTKeyInfo> iKeys;
	TInt iDisableCheckDialog;
	TKeyTypes iType;
	TInt iKeyStoreImplIndex;
	TBuf8<50> iKeyStoreImplLabel;
	
#ifdef SYMBIAN_AUTH_SERVER
	// this variable would be used to retrieve the rom build variant 
	// for key store.
	TBool iUseNewApi;
	HBufC* iAuthExpression;
	TInt iFreshness;
	TInt iDeauthenticate;
#endif // SYMBIAN_AUTH_SERVER
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
	TInt iHardwareType;
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
};

class CInitialiseKeyStore : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,	Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,	Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CInitialiseKeyStore();
public:
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	CInitialiseKeyStore(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
private:
	enum TState
		{
		ENew,
		EInit,
		EAppend,
		EFinished,
		EAnalyseResults
		};

private:
	TState iState;
	TBool iKeyManager;
	TBool iEnableStoreCountCheck;
	TInt iKeyStoreManagerCount;
	TInt iKeyStoreCount;
	TBool iEnableKeyStoreLabelCheck;
	TInt iKeyStoreIndex;
	HBufC8* iKeyStoreLabel;
	
private:
	CUnifiedKeyStore* iNewUnifiedKeyStore;
};


class COnlyCreateKeyStore : public CKeyStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,	Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,	Output& aOut, const TTestActionSpec& aTestActionSpec);
	~COnlyCreateKeyStore();
public:
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	enum TState
		{
		EInit,
		EFinished,
		EErrNoMemory
		};
private:
	COnlyCreateKeyStore(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
private:
	TState iState;
	TBool iKeyManager;
private:
	CUnifiedKeyStore* iNewUnifiedKeyStore;
};


class CDeleteKeyStore : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CDeleteKeyStore();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	CDeleteKeyStore(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
private:
	enum TState
		{
		EDelete,
		EFinished
		};
private:
	TState iState;
};

class CAddKey : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CAddKey();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	void SetKeySize(const TDesC8& aKeySize);
	void SetStartDateL(const TDesC8& aData);
	void SetEndDateL(const TDesC8& aData);
private:
	CAddKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
private:
	enum TState
		{
		EAddKey,
		EFinished
		};
private:
	TState iState;
private:
	TUint iSize;
	TTime iStartDate, iEndDate;
	CCTKeyInfo* iKeyInfo;
};


class CListKeys : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CListKeys();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	CListKeys(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void SetKeySize(const TDesC8& aKeySize);
	void SetListCount(const TDesC8& aListCount);
	void SetListStatus(const TDesC8& aListStatus);
	void AddFoundKeyL(const TDesC8& aFoundKey);
private:	
	enum TState
		{
		EListKeys,
		EFinished
		};
private:
	TState iState;

	TInt iResultCount; // The expected result count (or -1 for don't care)
	RCPointerArray<HBufC> iKeysToFind; // Labels of expected keys

	// Filter componenets
	TUint iSize;
	CCTKeyInfo::EKeyAccess iAccessType;
	TDesC8* iListStatus;
};

/**
 The same as ListKeys except that it checks all stores
 */
class CListAllKeys : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CListAllKeys();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	CListAllKeys(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void SetKeySize(const TDesC8& aKeySize);
	void SetListCount(const TDesC8& aListCount);
	void AddFoundKeyL(const TDesC8& aFoundKey);
private:	
	enum TState
		{
		EListKeys,
		EFinished
		};
private:
	TState iState;

	TInt iStore;
	
	TInt iResultCount; // The expected result count (or -1 for don't care)
	RCPointerArray<HBufC> iKeysToFind; // Labels of expected keys

	// Filter componenets
	TUint iSize;
	CCTKeyInfo::EKeyAccess iAccessType;
};

//	Allows script to filter on keys algorithm (or all) and deletes listed keys
//	and calls list again to ensure they've gone
class CDeleteKeys : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CDeleteKeys();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	CDeleteKeys(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void SetNonExistent(const TDesC8&);
	void SetDeleteCount(const TDesC8&);
private:	
	enum TState
		{
		EListKeys,
		EDeleteAllKeys,
		EDeleteNonExistentKey,
		EDeleteKeyByLabel,
		EFinished
		};
private:
	TState iState;

	TCTTokenObjectHandle iDeleteHandle;
	TInt iInitialKeyCount;
	TInt iFinalKeyCount;
	TInt iDeletedCount;
	TInt iDeleteIndex;
	TInt iNonExistentKey;		//	Set!=0 if trying to delete something non existent
	TInt iExpectedDeleteCount;	//	Number of keys expected to be deleted (-1 for don't care)
};

class CImportKey : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CImportKey();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
protected:
	CImportKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void SetKeyDataFileL(const TDesC8& aDes);
protected:
	void DoReportAction();
	void DoCheckResult(TInt aError);
private:
	TBool SetKeyUsage(const TDesC8& aKeyUsage);
private:
	void ConstructL(const TTestActionSpec& aTestActionSpec);
protected:
	HBufC8*  iImportFileName;
	HBufC8* iKeyData;
	CCTKeyInfo* iKeyInfo;
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
	TUint iImportHardwareType;
	HBufC8* iPublicKey;
	HBufC8* iPrivateKey;
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
private:
	enum TState
		{
		EImportKey,
		EFinished
		};
	TState iState;
};


class CExportKey : public CImportKey
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CExportKey();
	virtual void PerformAction(TRequestStatus& aStatus);	
	virtual void PerformCancel();
	virtual void Reset();
protected:
	void WriteToFileL();
	void DoReportAction();
	void DoCheckResult(TInt aError);
	//void DoPerformPrerequisite(TRequestStatus& aStatus);
protected:
	void SetKeyDataFile(const TDesC8& aDes);
	void SetPBEParametersL(const TDesC8& aCipher, const TDesC8& aSalt, const TDesC8& aIV, const TInt aIteration);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void SetKeyEncrypted(const TDesC8& aDes);
protected:
	CExportKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
protected:
	TBool iEncrypted;
	TFileName iKeyDataFile;
	CPBEncryptParms* iPbeParams;
	enum TState
		{
		EExportKey,
		EListing,
		EIntermediate,
		EFinished
		};
	TState iState;	
};

class CExportPublicKey : CExportKey
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	 ~CExportPublicKey();
	virtual void PerformAction(TRequestStatus& aStatus);	
	virtual void PerformCancel();
protected:
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	CExportPublicKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	};

/**
* Compare two files bit-wise. This is used as a sanity check
* for key import/export.
*/
class CCompare : public CKeyStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CCompare();
	virtual void PerformAction(TRequestStatus& aStatus);	
	virtual void PerformCancel();
	virtual void Reset();	
protected:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	TInt CompareFilesL();
private:
	CCompare(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);	
private:
	TFileName iOriginalFile;
	TFileName iNewFile;
	TInt pos;
	enum TState
		{
		EComparing,
		EFinished
		};
	TState iState;	
	};
	
class CCorrupt : public CKeyStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CCorrupt();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
private:
	CCorrupt(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	void DoCorruptL();
private:
	enum TState
		{
		ECorrupting, 
		EFinished
		};
private:
	HBufC8* iCorruptionData;
	TState iState;
	};

// Gets a key by handle. As the script can't know the handle, the
// script lists all keys, identifies a key by name and then gets it by
// handle. This action allows all attributes of a key to be specified.
class CGetKeyInfo : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CGetKeyInfo();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	CGetKeyInfo(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	TBool SetKeyUsage(const TDesC8& aKeyUsage);
	void SetKeySize(const TDesC8& aKeySize);
	void SetNativeL(const TDesC8& aData);
	void SetStartDateL(const TDesC8& aData);
	void SetEndDateL(const TDesC8& aData);
private:	
	enum TState
		{
		EListKeysPreGet,
		EGet,
		EFinished
		};
private:
	TState iState;

	TCTTokenObjectHandle iGetHandle;
	CCTKeyInfo* iKey;
	TUint iSize;
	TBool iNativeSet, iNative;
	TTime iStartDate, iEndDate;
};


//	Allows script to filter on keys algorithm (or all) and deletes listed keys
//	and calls list again to ensure they've gone
class COpenKey : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~COpenKey();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	COpenKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

private:	
	enum TState
		{
		EListKeysPreOpen,
		EOpenKey,
		EFinished
		};
	
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
	enum TOperationType
		{
		ESigning,
		EDecryption,
		};
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
private:
	TState iState;
	TCTTokenObjectHandle iDeleteHandle;
//	Filter components
	MRSASigner* iRSASigner;
	MDSASigner* iDSASigner;
	MCTDecryptor* iDecryptor;
	MCTDH* iDH;
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
	TOperationType iOperationType;
	CryptoSpi::CSigner* iSigner;
	CryptoSpi::CAsymmetricCipher* iDecrypt;
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
};

//	Allows script to sign, filter on keys algorithm (or all) and deletes listed keys
//	and calls list again to ensure they've gone
class CSign : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CSign();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	CSign(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void SetSignText(const TDesC8&);
	void SetDigestSignL(const TDesC8&);
	
private:	
	enum TState
		{
		EListKeysPreOpen,
		EOpenKey,
		ESign,
		EExportPublic,
		EVerify,
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
		ESignInHardware,
		EVerifyHwSignature,
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
		EFinished,
		};

private:
	TState iState;
	TBool iVerifyResult;
	TCTTokenObjectHandle iExportHandle;
//	Filter components
	HBufC8* iReadText;		//	Text to sign, read in from script
	HBufC8*	iExportedPublicKey;		
	MRSASigner* iRSASigner;
	MDSASigner* iDSASigner;
	MCTDecryptor* iDecryptor;
	MCTDH* iDH;
	CDSASignature* iDSASignature;
	CRSASignature* iRSASignature;
	CMessageDigest* iHash;
	TBool iFailHashDigest;	//	Should hash the digest beforehand, but doesn't to test failure mode
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
	CryptoSpi::CSigner* iSigner;
	CryptoSpi::CCryptoParams* iSpiSignature;
	TCTTokenObjectHandle iTokenHandle;
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
};							

class CDecrypt : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CDecrypt();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	CDecrypt(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void SetSignText(const TDesC8&);
private:	
	enum TState
		{
		EListKeysPreOpen,
		EOpenKey,
		EDecrypt,
		EExportPublic,
		EVerify,
		EFinished
		};

private:
	TState iState;
	TBool iVerifyResult;

	TCTTokenObjectHandle iExportHandle;
//	Filter components
	HBufC8* iReadText;		//	Text to sign, read in from script
	HBufC8*	iPlainText;	
	TPtr8 iPlainTextPtr;
	MCTDecryptor* iDecryptor;
	HBufC8* iPublic;
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
	CryptoSpi::CAsymmetricCipher* iDecrypt;
#endif
};

class CDHKeyPair;
class CDHPublicKey;

class CDerive : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CDerive();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	CDerive(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:	
	enum TState
		{
		EListKeysPreOpen,
		EOpenKey,
		EDerive,
		EExportPublic,
		EVerify,
		EFinished
		};

private:
	TState iState;
	TBool iVerifyResult;

	TCTTokenObjectHandle iExportHandle;
	RInteger iN;
	RInteger iG;
	CDHKeyPair* iRemote;
	MCTDH* iDH;
	CDHPublicKey* iPublicKey;
	HBufC8* iOutput;
};

class CDeleteKeyStoreData : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CDeleteKeyStoreData();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	CDeleteKeyStoreData(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
private:
	enum TState
		{
		EDelete,
		EFinished
		};
private:
	TState iState;
};

/**
 * Test setting the users of a key.
 */
class CSetUsers : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CSetUsers();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	TBool AddUserL(const TDesC8& aKeySize);
private:
	CSetUsers(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
private:
	enum TState
		{
		EListKeys,
		EMain,
		EFinished
		};
private:
	TState iState;
private:
	RArray<TUid> iUsers;
};

/**
 * Test setting the passphrase timeout using the SetPassphraseTimeout method on
 * the unified key store.
 */
class CSetTimeout : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CSetTimeout();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	CSetTimeout(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
private:
	enum TState
		{
		EMain,
		EFinished
		};
private:
	TState iState;
private:
	TInt iNewTimeout;
};

/**
 * Test relocking the store.
 */
class CRelockStore : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CRelockStore();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	CRelockStore(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
private:
	enum TState
		{
		EMain,
		EFinished
		};
private:
	TState iState;
};

/**
 * Set the client UID.
 *
 * In order to test the keystore easily, it's sometimes necessary to impersonate
 * different processes.  This test action sets the UID of the current process.
 * Currently this actually changyes the process UID - when this becomes
 * impossible due to platform security, a differnt approach will be needed
 * (probably a debug-mode-only keystore message telling to to treat the client
 * as if it had a different UID).
 */
class CSetClientUid : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CSetClientUid();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	CSetClientUid(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
private:
	enum TState
		{
		EMain,
		EFinished
		};
private:
	TState iState;
	TUid iUid;
};

/**
 * Set the home time on the device.
 *
 * We need to be able to set the time so we can create keys and then make them
 * expire.
 */
class CSetTime : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CSetTime();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	CSetTime(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	void SetNewTimeL(const TDesC8& aData);
private:
	enum TState
		{
		EMain,
		EFinished
		};
private:
	TState iState;
	TTime iNewTime;
};

/**
 * A templated class for test actions that test authentication objects.  This
 * does most of the fiddly test stuff - the implementation of the test itself is
 * provided by the template parameter.
 */
template <class TTestImpl>
class CAuthObjectTest : public CKeyStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CAuthObjectTest();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
private:
	CAuthObjectTest(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	void Reset();
private:
	enum TState
		{
		EListKeys,
		EMain,
		EFinished
		};
private:
	TState iState;
	TTestImpl iImpl;
    MCTAuthenticationObject* iAuth;
	};

/**
 * Base class for auth object test implmentations, supplies default method implementations.
 */
class TAuthObjectTest
	{
 public:
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void Reset();
	TBool CheckResult(Output& aOut);
	};

/**
 * Test changing the passphrase.
 */
class TChangePassphrase : public TAuthObjectTest
	{
 public:
	const TDesC& Name();
	void DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus);
    void DoCancel(MCTAuthenticationObject& aAuth);
	};

typedef CAuthObjectTest<TChangePassphrase> CChangePassphrase;

/**
 * Test listing protected objects.
 */
class TListProtectedObjects : public TAuthObjectTest
	{
 public:
	~TListProtectedObjects();
	const TDesC& Name();
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus);
	TBool CheckResult(Output& aOut);
	void Reset();
    void DoCancel(MCTAuthenticationObject& aAuth);
 private:
	TBool AddExpectedKeyL(const TDesC8& aKeyLabel);
 private:
	RPointerArray<HBufC> iExpectedKeys;
	RMPointerArray<MCTTokenObject> iObjects;
	};

typedef CAuthObjectTest<TListProtectedObjects> CListProtectedObjects;

/**
 * Test unblocking the passphrase.
 */
class TUnblockPassphrase : public TAuthObjectTest
	{
 public:
	const TDesC& Name();
	void DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus);	
    void DoCancel(MCTAuthenticationObject& aAuth);
	};

typedef CAuthObjectTest<TUnblockPassphrase> CUnblockPassphrase;

/**
 * Test auth object open method.
 */
class TAuthOpen : public TAuthObjectTest
	{
 public:
	const TDesC& Name();
	void DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus);	
    void DoCancel(MCTAuthenticationObject& aAuth);
	};

typedef CAuthObjectTest<TAuthOpen> CAuthOpen;

/**
 * Test auth object close method.
 */
class TAuthClose : public TAuthObjectTest
	{
 public:
	const TDesC& Name();
	void DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus);	
    void DoCancel(MCTAuthenticationObject& aAuth);
	};

typedef CAuthObjectTest<TAuthClose> CAuthClose;

/**
 * Test get time remaining.
 */
class TGetTimeRemaining : public TAuthObjectTest
	{
 public:
	const TDesC& Name();
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus);
	TBool CheckResult(Output& aOut);
    void DoCancel(MCTAuthenticationObject& aAuth);
 private:
	TInt iExpectedTime;
	TInt iTimeRemaining;
	};

typedef CAuthObjectTest<TGetTimeRemaining> CGetTimeRemaining;

/**
 * Test set timeout via auth object.
 */
class TAuthSetTimeout : public TAuthObjectTest
	{
 public:
	const TDesC& Name();
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus);
    void DoCancel(MCTAuthenticationObject& aAuth);
 private:
	TInt iNewTimeout;
	};

typedef CAuthObjectTest<TAuthSetTimeout> CAuthSetTimeout;

/**
 * Test get timeout.
 */
class TGetTimeout : public TAuthObjectTest
	{
 public:
	const TDesC& Name();
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoTest(MCTAuthenticationObject& aAuth, TRequestStatus& aStatus);
	TBool CheckResult(Output& aOut);
    void DoCancel(MCTAuthenticationObject& aAuth);
 private:
	TInt iExpectedTimeout;
	TInt iTimeout;
	};

typedef CAuthObjectTest<TGetTimeout> CGetTimeout;


/**
 * Get the use or management policy of a key.
 */
class CGetPolicy : public CKeyStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CGetPolicy();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	CGetPolicy(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	void SetNewTimeL(const TDesC8& aData);
private:
	enum TState
		{
		EListKeys,
		EFinished
		};
private:
	TBool iTestUsePolicy;
	TSecurityPolicy iExpectedPolicy;
	TState iState;
	TSecurityPolicy iPolicy;
	};

/**
 * Set the use or management policy of a key.
 */
class CSetPolicy : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CSetPolicy();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	CSetPolicy(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
	void SetNewTimeL(const TDesC8& aData);
private:
	enum TState
		{
		EListKeys,
		ESetPolicy,
		EFinished
		};
private:
	TBool iTestUsePolicy;
	TSecurityPolicy iNewPolicy;
	TState iState;
};

class CAuthenticationPolicy : public CKeyStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CAuthenticationPolicy();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
private:
	void SetAuthExpression(TPtrC8& aAuthExpression);
	void SetFreshness(TUint aFreshness);
	void SetMode(const TDesC8& aMode);
private:
	CAuthenticationPolicy(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
private:
	enum TState
		{
		EInit,
		EListing,
		EGetAuthenticationPolicy,
		EFinished
		};
private:
	TState iState;
private:
	TBuf8<256> iMode;
	TBuf8<256> iExpectedExpression;
	TInt iExpectedFreshness;
};

#endif	//	__T_KEYSTORE_ACTIONS_H__
