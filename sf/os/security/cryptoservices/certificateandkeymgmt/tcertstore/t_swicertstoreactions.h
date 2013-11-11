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
* Actions for testing additional metadata supplied by the SWI cert store, ie
* capabilties, mandatory and systemupgrade flag: - get capabilities - get mandatory - get systemupgrade flag
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __T_METADATAACTIONS_H__
#define __T_METADATAACTIONS_H__

#include "t_certstoreactions.h"

class CSWICertStore;

/**
 * Test action to initialise an instance of the SWI cert store.
 *
 * Delete is handled by the standard delete action.
 */
class CInitSWICertStoreAction : public CCertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
							 const TTestActionSpec& aTestActionSpec);	

protected:
	// Implement test action methods
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	CInitSWICertStoreAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	
	virtual void DoReportAction();
	virtual void DoCheckResult(TInt aError);
	
private:
	enum TState
		{
		EInitCertStore,
		EFinished
		};

	TState iState;
	RFs& iFs;

	};

/**
 * Base class for SWI cert store meta data test actions.
 */
class CSWICertStoreTestAction : public CCertStoreTestAction
	{
public:
	virtual ~CSWICertStoreTestAction();

	// Implement test action methods
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

protected:
	CSWICertStoreTestAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	// Define interface for derived classes
	virtual void DoConstructL(const TTestActionSpec& aTestActionSpec) = 0;
	virtual void DoPerformActionL(CCTCertInfo& aCertInfo) = 0;
	virtual void DoCancel() = 0;

private:
	enum TState
		{
		EGetCert,
		EPerformAction,
		EFinished
		};

private:
	CCTCertInfo* FindCertificate();

private:
	TState iState;
	TCertLabel iCertificateLabel;			///< The certificate we're acting on
	RMPointerArray<CCTCertInfo> iCertInfos; ///< List of all certificates found
	CCertAttributeFilter* iFilter;			///< Filter to use when listing certs
	};

/**
 * Test action to get the capabilties associated with a certificate.
 */
class CGetCapabilitiesAction : public CSWICertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
							 const TTestActionSpec& aTestActionSpec);	
	
private:
	CGetCapabilitiesAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	virtual void DoConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void DoReportAction();
	virtual void DoPerformActionL(CCTCertInfo& aCertInfo);
	virtual void DoCheckResult(TInt aError);
	virtual void DoCancel();

private:
	TCapabilitySet iExpectedCapapbilities;
	TCapabilitySet iCapabilities;
	};

/**
 * Test action to get the mandatory flag associated with a certificate.
 */
class CGetMandatoryAction : public CSWICertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
							 const TTestActionSpec& aTestActionSpec);	
	
private:
	CGetMandatoryAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	virtual void DoConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void DoReportAction();
	virtual void DoPerformActionL(CCTCertInfo& aCertInfo);
	virtual void DoCheckResult(TInt aError);
	virtual void DoCancel();

private:
	TBool iExpectedValue;
	TBool iValue;
	};

/**
 * Test action to get the systemupgrade flag associated with a certificate.
 */
class CGetSystemUpgradeAction : public CSWICertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
							 const TTestActionSpec& aTestActionSpec);	
	
private:
	CGetSystemUpgradeAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	virtual void DoConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void DoReportAction();
	virtual void DoPerformActionL(CCTCertInfo& aCertInfo);
	virtual void DoCheckResult(TInt aError);
	virtual void DoCancel();

private:
	TBool iExpectedSystemUpgradeValue;
	TBool iSystemUpgradeValue;
	};
	
/**
* Test action to get the deletable flag associated with a certificate.
*/
 
class CGetDeletionAction : public CSWICertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
							 const TTestActionSpec& aTestActionSpec);	
	
private:
	CGetDeletionAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	virtual void DoConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void DoReportAction();
	virtual void DoPerformActionL(CCTCertInfo& aCertInfo);
	virtual void DoCheckResult(TInt aError);
	virtual void DoCancel();

private:
	TBool iExpectedDeletableValue;
	TBool iDeletableValue;
	};

/**
* Test action to verify multiple readers
*/
 
class CMultipleReadersAction : public CSWICertStoreTestAction
       {
public:
       static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
                                                        const TTestActionSpec& 
TestActionSpec);       
       
private:
       CMultipleReadersAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
       virtual void DoConstructL(const TTestActionSpec& aTestActionSpec);
       virtual void DoReportAction();
       virtual void DoPerformActionL(CCTCertInfo& aCertInfo);
       virtual void DoCheckResult(TInt aError);
       virtual void DoCancel();

private:
       TBool iExpectedValue;
       TBool iValue; 
       };
	

#endif
