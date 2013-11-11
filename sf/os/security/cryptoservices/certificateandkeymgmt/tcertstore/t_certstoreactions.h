/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef __T_CERTSTOREACTIONS_H__
#define __T_CERTSTOREACTIONS_H__

#include "t_testhandler.h"
#include <unifiedcertstore.h>
#include <unifiedkeystore.h>
#include <certificateapps.h>
#include <e32cons.h>
#include <badesca.h>
#include "t_testaction.h"
#include <cctcertinfo.h>

class Output;
class CSWICertStore;

enum TCertStoreType
	{
	EUnifiedCertStore,
	ESWICertStore
	};

class CCertStoreChangeNotifier : public CActive
  	{
  public:
  	static CCertStoreChangeNotifier* NewL(TInt& aNotificationFlag);
  	virtual ~CCertStoreChangeNotifier();
  	void StartNotification();
  	TInt& iNotifiedCounter;
  	void SetCompleteStatus(TRequestStatus* aStatus);
  private:
  	CCertStoreChangeNotifier(TInt& aNotificationFlag);
  	void ConstructL();
  	void DoCancel();
  	void RunL();
  private:	
  	RProperty iCertStoreChangeProperty;	
  	TRequestStatus* iCompleteStatus;
  	};

/**
 * Contains either a unified cert store instance or a SWI cert store instance.
 * 
 * Either can be accessed through the CertStore() method, but
 * AsUnifiedCertStore() and AsSWICertStore() will panic if the store is the
 * wrong type.
 *
 * This class owns the cert store pointer.
 */
class COpenCertStore : public CBase
	{
 public:
	virtual ~COpenCertStore();

	TCertStoreType Type();
	MCertStore& CertStore();

	void SetCertStore(CUnifiedCertStore* aCertStore);
	CUnifiedCertStore& AsUnifiedCertStore();

	void SetCertStore(CSWICertStore* aCertStore);
	CSWICertStore& AsSWICertStore();
	
 private:

 private:

	TBool iSet;
	TCertStoreType iType;
	union
		{
		TAny* iAny;
		CUnifiedCertStore* iUnifiedCertStore;
		CSWICertStore* iSwiCertStore;
		};
	};

/**
 * Per-thread shared data.
 *
 * This contains stuff that is shared by more than one test action, eg the cert
 * stores we are testing.
 */
class CSharedData : public CBase
	{
public:
	~CSharedData();
	void InitCertificateAppInfoManagerL();
	void DeleteCertificateAppInfoManager();
	RPointerArray<COpenCertStore>& CertStores();
public:
	RFs iFs;
	RPointerArray<COpenCertStore> iCertStores;	
	CCertificateAppInfoManager* iCertificateAppInfoManager;
	};

class CCertStoreTestAction : public CTestAction
	{
public:
	virtual ~CCertStoreTestAction();

protected:
	CCertStoreTestAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut);

	// Methods for accessing the shared app info manager
	void InitTheCertificateAppInfoManagerL();
	void DeleteTheCertificateAppInfoManager();
  	CCertificateAppInfoManager* TheCertificateAppInfoManager();

	// Methods for accessing the shared cert stores
	TInt CertStoreCount();
	TCertStoreType CertStoreType(TInt aIndex = 0);
	MCertStore& CertStore(TInt aIndex = 0);
	void AddCertStoreL(CUnifiedCertStore* aCertStore);
	CUnifiedCertStore& UnifiedCertStore(TInt aIndex = 0);
	void AddCertStoreL(CSWICertStore* aCertStore);
	CSWICertStore& SWICertStore(TInt aIndex = 0);
	void RemoveCertStore(TInt aIndex);

protected:
	RFs& iFs;

 private:
	CSharedData& CertStoreSharedData();
	RPointerArray<COpenCertStore>& CertStores();	
	void DoPerformPrerequisite(TRequestStatus& aStatus);
	void DoPerformPostrequisite(TRequestStatus& aStatus);
	};

class CSubscriberAction: public CCertStoreTestAction
	{
public:
 	virtual ~CSubscriberAction();
 
protected:
 	CSubscriberAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
 	void ConstructL(const TTestActionSpec& aTestActionSpec);
 	TInt iNotifierFlag;
 	TInt iNotificationSubscribed;		
 	CCertStoreChangeNotifier* iNotifier;		
 	};

/**
 * This action creates and initializes a certificate store manager.
 * It should be used before any action requiring a certificate store manager.
 * All the actions are likely to require one.
 */
class CInitialiseCertStore : public CCertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,	
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CInitialiseCertStore();

public:
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	CInitialiseCertStore(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	enum TState
		{
		ENew,
		EAppend,
		ECheckOrder,
		EFinished
		};

private:
	TState iState;
	TBool iOpenedForWrite;
 	RArray<TInt> iFilterOrdering;
 	RArray<TInt32> iExpectedOrderingResult;
	CUnifiedCertStore* iNewUnifiedCertStore;
	};

class COnlyCreateCertStore : public CCertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	~COnlyCreateCertStore() {};

public:
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	enum TState
		{
		EInit,
		EFinished
		};

private:
	COnlyCreateCertStore(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	TState iState;
	TBool iOpenedForWrite;

	CUnifiedCertStore* iNewUnifiedCertStore;
	};

class CDeleteCertStore : public CCertStoreTestAction
	{
	public:
		static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
			const TTestActionSpec& aTestActionSpec);
		static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
			const TTestActionSpec& aTestActionSpec);
		~CDeleteCertStore();
		virtual void PerformAction(TRequestStatus& aStatus);
		virtual void PerformCancel();
		virtual void Reset();

	private:
		CDeleteCertStore(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
		void ConstructL(const TTestActionSpec& aTestActionSpec);
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


class CX509Certificate;
class CWTLSCertificate;

// This is the base class used by both CSetApplications and CSetTrusters. This
// is required because of the merging of the SetApplicability and SetTrust API
// calls on the certstore
class CSetAppsAndTrust : public CSubscriberAction

	{
public:
	~CSetAppsAndTrust();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void Reset();
	virtual void PerformCancel();

protected:
	CSetAppsAndTrust(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void GetCerts(TRequestStatus& aStatus);
	void SetCertLabel(const TDesC8& aLabel);

	/** Implemented by derived classes, called from PerformAction. */
	virtual void DoSetAppTrust(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus) = 0;
	/** Implemented by derived classes, called from PerformCancel. */
	virtual void DoPerformCancel() = 0;

protected:
	enum TState
		{
		EGetCAEntries,
		ESetAppTrust,
		ECheckNotification,
		EFinished
		};

protected:
	/**
	 * The label of the certificate we want to set the trust settings for
	 */ 
	TCertLabel iLabel;

	// Index into the certinfo list which points to the certificate we
	// are interested in
	TInt iIndex;
	 
	TState iState;
	CCertAttributeFilter *iFilter;
	RMPointerArray<CCTCertInfo> iCertInfos;
	};



/**
 * This class is used to test the trust settings of a certificate.
 */
class CSetApplications : public CSetAppsAndTrust
	{
public:
	/**
	 * @param aInfo This is what is enclosed in the <actionbody> and </actionbody> tags 
	 * of the test script.
	 */
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	~CSetApplications();

private:
	CSetApplications(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);
	void AppendUid(const TDesC8& aUid);
	void DoSetAppTrust(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus);
	void DoPerformCancel();

private:
	/**
	 * The array of applications that trust this certificate.
	 */
	RArray<TUid> iApplications;
	};

/**
 * This class is used to test the trust settings of a certificate.
 */
class CSetTrusters : public CSetAppsAndTrust
	{
public:
	/**
	 * @param aInfo This is what is enclosed in the <actionbody> and </actionbody> tags 
	 * of the test script.
	 */
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	~CSetTrusters();

private:
	CSetTrusters(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);
	void SetTrusted(const TDesC8& aTrusted);
	void DoSetAppTrust(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus);
	void DoPerformCancel();

private:
	// The trusted setting to set
	TBool iTrusted;
	};

/**
 * This class is used to test the trust settings of a certificate.
 */
class CGetTrusters : public CCertStoreTestAction
	{
public:
	/**
	 * @param aInfo This is what is enclosed in the <actionbody> and </actionbody> tags 
	 * of the test script.
	 */
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	~CGetTrusters();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	CGetTrusters(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void GetCerts(TRequestStatus& aStatus);
	void SetCertLabel(const TDesC8& aLabel);
	void SetExpectedTrusters(const TDesC8& aExpectedTrusters);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	enum TState
		{
		EGetCAEntries,
		EGetTrusters,
		EFinished
		};

private:
	/**
	 * The label of the certificate we want to get the trust settings of
	 */ 
	TCertLabel iLabel;
	TState iState;
	TBool iTrust;
	TBool iExpectedTrust;
	CCertAttributeFilter *iFilter;
	RMPointerArray<CCTCertInfo> iCertInfos;
	RArray<TUid> iTrusters;
	RArray<TUid> iExpectedTrusters;
	};

#endif
