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

#ifndef __T_CERTAPPS_ACTIONS_H__
#define __T_CERTAPPS_ACTIONS_H__

#include "t_testhandler.h"
#include "t_testaction.h"
#include <certificateapps.h>
#include <e32cons.h>
#include <badesca.h>
#include <mctcertapps.h>

class Output;

// Base class which defines common actions
class CCertAppTestAction : public CTestAction
	{
public:
	~CCertAppTestAction();

	// These methods have their default behaviour set to do nothing. May be
	// overriden if required
	virtual void PerformCancel();
	virtual void Reset();
	virtual void InitialiseL(TBool& aMemFailureFlag,
							 TBool& aCancel, TInt& aHeapMark, TInt& aHeapMarkEnd);

	// This method calls the abstract leaving function DoPerformActionL, and
	// returns its leaving status if there is an error. Since all certapps
	// calls are synchronous, PerformAction will set the finished flag to
	// true always
	virtual void PerformAction(TRequestStatus& aStatus);

protected:
	CCertAppTestAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut);

	// Chained 2nd phase constructor. All this does is extract the
	// expected result
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	virtual void DoPerformActionL() = 0;
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void DoReportAction();
	virtual void DoCheckResult(TInt aError);

	// set the iExpectedResult parameter from a descriptor
	virtual void SetExpectedResult(const TDesC8& aResult);

	// Parses a tag with <tagname>[TAG]</tagname> - makes a call
	// to Input::ParseElement
	static TPtrC8 ParseTagString(const TDesC8& aBuf, const TDesC8& aTagName, TInt& aPos, TInt& aError);
	static TInt32 ParseTagInt(const TDesC8& aBuf, const TDesC8& aTagName, TInt& aPos, TInt& aError);

protected:
	// singleton instance of the app info manager - initialised by CInitManager
	// and destroyed by CDestroyManager
	static CCertificateAppInfoManager* iAppManager;

private:
	RFs& iFs;
	};

// template wrapper class which implements mundane operations required by the
// test framework
template <class T>
class CTestWrapper : public T
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,	Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,	Output& aOut, const TTestActionSpec& aTestActionSpec);
	~CTestWrapper() {}

protected:
	CTestWrapper(RFs& aFs, CConsoleBase& aConsole, Output& aOut) :
		 T(aFs, aConsole, aOut) {}
	};

template <class T>
CTestAction* CTestWrapper<T>::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CTestWrapper<T>::NewLC(aFs, aConsole, aOut,	aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

template <class T>
CTestAction* CTestWrapper<T>::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestWrapper<T>* self = new(ELeave) CTestWrapper<T>(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

// Initialises the iAppManager
class CInitManager : public CCertAppTestAction
	{
	// override the default prerequisite since this has to check that
	// the manager does NOT exist
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);

protected:
	CInitManager(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	virtual void DoPerformActionL();
	};

// destroys the iAppManager
class CDestroyManager : public CCertAppTestAction
	{
protected:
	CDestroyManager(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	virtual void DoPerformActionL();
	};

// Clears all the applications from the app list
class CClearAllApps : public CCertAppTestAction
	{
protected:
	CClearAllApps(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	virtual void DoPerformActionL();
	};

// Adds a new application
class CAddApp : public CCertAppTestAction
	{
public:
	~CAddApp();

protected:
	CAddApp(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void DoPerformActionL();

private:
	// The CertApps to add
	RArray<TCertificateAppInfo> iAppArray;
	};

// Removes an application
class CRemoveApp : public CCertAppTestAction
	{
protected:
	CRemoveApp(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void DoPerformActionL();

private:
	// The UID to remove
	TUid iUid;
	};

// Gets the application count
class CAppCount : public CCertAppTestAction
	{
protected:
	CAppCount(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void DoPerformActionL();

private:
	TInt iCount; // number of apps expected
	};

// Gets an application
class CGetApp : public CCertAppTestAction
	{
protected:
	CGetApp(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void DoPerformActionL();

private:
	// The uid to retrieve and the name to expect
	TUid iUid;
	TName iName;
	};

// Gets the applications
class CGetApplications : public CCertAppTestAction
	{
public:
	~CGetApplications();

protected:
	CGetApplications(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void DoPerformActionL();

private:
	// The CertApps expected to be received
	RArray<TCertificateAppInfo> iAppArray;
	};

#endif	//	__T_CERTAPPS_ACTIONS_H__
