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

#include "t_certapps_actions.h"
#include "t_certapps_defs.h"
#include "t_input.h"
#include "t_output.h"

#include "certificateapps.h"

/////////////////////////////////////////////////////////////////////////////////
//CCertAppTestAction base class
/////////////////////////////////////////////////////////////////////////////////

// singleton instance of the manager
CCertificateAppInfoManager* CCertAppTestAction::iAppManager = NULL;

CCertAppTestAction::~CCertAppTestAction()
	{
	}


CCertAppTestAction::CCertAppTestAction(RFs& aFs, CConsoleBase& aConsole, 
										 Output& aOut)
:	CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CCertAppTestAction::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	TInt pos = 0, error = 0;
	SetExpectedResult(ParseTagString(aTestActionSpec.iActionResult, KReturn, pos, error));
	}

void CCertAppTestAction::InitialiseL(TBool& /*aMemFailureFlag*/,
									 TBool& /*aCancel*/, TInt& /*aHeapMark*/, TInt& /*aHeapMarkEnd*/)
	{
	}

TPtrC8 CCertAppTestAction::ParseTagString(const TDesC8& aBuf, const TDesC8& aTagName, TInt& aPos, TInt& aError)
	{
	// wrapper around the Input::ParseElement function. Processes aTagName
	// and produces <aTagName> and </aTagName> used by ParseElement
	TBuf8<64> start(0);
	TBuf8<64> end(0);

	start.Append('<');
	start.Append(aTagName);
	start.Append('>');

	end.Append('<');
	end.Append('/');
	end.Append(aTagName);
	end.Append('>');

	return Input::ParseElement(aBuf, start, end, aPos, aError);
	}

TInt32 CCertAppTestAction::ParseTagInt(const TDesC8& aBuf, const TDesC8& aTagName, TInt& aPos, TInt& aError)
	{
	TPtrC8 ptr(ParseTagString(aBuf, aTagName, aPos, aError));

	// Get an integer value out of it
	TInt32 retVal = 0;
	TLex8 lex(ptr);
	lex.Val(retVal);
	return retVal;
	}


void CCertAppTestAction::SetExpectedResult(const TDesC8& aResult)
	{
	if (aResult == _L8("KErrNone") || aResult == KNullDesC8)
		{
		iExpectedResult = KErrNone;
		}
	else if (aResult == _L8("KErrAccessDenied"))
		{
		iExpectedResult = KErrAccessDenied;
		}
	else if (aResult == _L8("KErrNotReady"))
		{
		iExpectedResult = KErrNotReady;
		}
	else if (aResult == _L8("KErrAlreadyExists"))
		{
		iExpectedResult = KErrAlreadyExists;
		}
	else if (aResult == _L8("KErrInUse"))
		{
		iExpectedResult = KErrInUse;
		}
	else if (aResult == _L8("KErrNotFound"))
		{
		iExpectedResult = KErrNotFound;
		}
	else if (aResult == _L8("KErrBadName"))
		{
		iExpectedResult = KErrBadName;
		}
	else if (aResult == _L8("KErrArgument"))
		{
		iExpectedResult = KErrArgument;
		}
	else if (aResult == _L8("KErrNotReady"))
		{
		iExpectedResult = KErrNotReady;
		}
	else if (aResult == _L8("KErrCorrupt"))
		{
		iExpectedResult = KErrCorrupt;
		}
	else if (aResult == _L8("KErrPermissionDenied"))
		{
		iExpectedResult = KErrPermissionDenied;
		}
	else
		{
		iOut.write(_L("Unrecognised error code: "));
		iOut.writeString(aResult);
		iOut.writeNewLine();
		User::Leave(KErrArgument);
		}
	}

void CCertAppTestAction::PerformAction(TRequestStatus& aStatus)
	{
	TInt err = KErrNone;
	TRAP(err, DoPerformActionL());

	if (err != KErrNoMemory)
		{
		iFinished = ETrue;
		}
	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CCertAppTestAction::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TInt err = KErrNone;

	// For all tests (with the exception of InitManager which overrides this
	// method), we check to make sure that the AppManager is set
	if (iAppManager)
		{
		iActionState = EAction;
		}
	else
		{
		iFinished = ETrue;
		err = KErrNotFound;
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CCertAppTestAction::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CCertAppTestAction::PerformCancel()
	{
	}

void CCertAppTestAction::Reset()
	{
	}

void CCertAppTestAction::DoReportAction()
	{
	}

void CCertAppTestAction::DoCheckResult(TInt aError)
	{
	iResult = (aError == iExpectedResult);
	}


/////////////////////////////////////////////////////////////////////////////////
//CInitManager - initialises the singleton manager
/////////////////////////////////////////////////////////////////////////////////
CInitManager::CInitManager(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CCertAppTestAction(aFs, aConsole, aOut)
	{
	}

void CInitManager::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TInt err = KErrNone;

	// If it does not exist, then we are OK
	if (!iAppManager)
		{
		iActionState = EAction;
		}
	else
		{
		iFinished = ETrue;
		err = KErrAlreadyExists;
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CInitManager::DoPerformActionL()
	{
	iAppManager = CCertificateAppInfoManager::NewL();
	}

/////////////////////////////////////////////////////////////////////////////////
//CDestroyManager - destroys the singleton manager
/////////////////////////////////////////////////////////////////////////////////
CDestroyManager::CDestroyManager(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CCertAppTestAction(aFs, aConsole, aOut)
	{
	}

void CDestroyManager::DoPerformActionL()
	{
	delete iAppManager;
	iAppManager = NULL;
	}


/////////////////////////////////////////////////////////////////////////////////
//CClearAllApps - removes all the applications from the app list
/////////////////////////////////////////////////////////////////////////////////
CClearAllApps::CClearAllApps(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CCertAppTestAction(aFs, aConsole, aOut)
	{
	}

void CClearAllApps::DoPerformActionL()
	{
	for (;;)
		{
		// apps is owned by the app manager, don't try accessing it after we've
		// removed any of the applications though
		
		RArray<TCertificateAppInfo> apps;
		apps = iAppManager->Applications();
		if (apps.Count() == 0)
			{
			break;
			}
		iAppManager->RemoveL(apps[0].Id());
		}
	}

/////////////////////////////////////////////////////////////////////////////////
//CAddApp - adds applications
/////////////////////////////////////////////////////////////////////////////////
CAddApp::CAddApp(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CCertAppTestAction(aFs, aConsole, aOut)
	{
	}

CAddApp::~CAddApp()
	{
	iAppArray.Close();
	}

void CAddApp::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertAppTestAction::ConstructL(aTestActionSpec);
	TInt pos = 0;
	TInt err = KErrNone;

	// Parse the UID and name for the new app
	do
		{
		TUid uid(TUid::Uid(ParseTagInt(aTestActionSpec.iActionBody, KUid, pos, err)));
		if (!err)
			{
			TName name;
			name.Copy(ParseTagString(aTestActionSpec.iActionBody, KAppName, pos, err));
			iAppArray.Append(TCertificateAppInfo(uid, name));
			}
		}
	while (!err);
	}

void CAddApp::DoPerformActionL()
	{
	for (TInt i = 0; i < iAppArray.Count(); ++i)
		{
		iAppManager->AddL(iAppArray[i]);
		}
	}

/////////////////////////////////////////////////////////////////////////////////
//CRemoveApp - removes an application
/////////////////////////////////////////////////////////////////////////////////
CRemoveApp::CRemoveApp(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CCertAppTestAction(aFs, aConsole, aOut)
	{
	}
	
void CRemoveApp::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertAppTestAction::ConstructL(aTestActionSpec);
	TInt pos = 0;
	TInt err = KErrNone;

	// Parse the UID
	iUid = TUid::Uid(ParseTagInt(aTestActionSpec.iActionBody, KUid, pos, err));
	}

void CRemoveApp::DoPerformActionL()
	{
	iAppManager->RemoveL(iUid);
	}

/////////////////////////////////////////////////////////////////////////////////
//CAppCount - Gets the number of applications
/////////////////////////////////////////////////////////////////////////////////
CAppCount::CAppCount(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CCertAppTestAction(aFs, aConsole, aOut)
	{
	}
	
void CAppCount::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertAppTestAction::ConstructL(aTestActionSpec);
	TInt pos = 0;
	TInt err = KErrNone;

	// Parse the expected number of applications
	iCount = ParseTagInt(aTestActionSpec.iActionBody, KCount, pos, err);
	}

void CAppCount::DoPerformActionL()
	{
	if (iCount != iAppManager->Applications().Count())
		User::Leave(KErrArgument);
	}

/////////////////////////////////////////////////////////////////////////////////
//CGetApp - Gets an application with a given ID
/////////////////////////////////////////////////////////////////////////////////
CGetApp::CGetApp(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CCertAppTestAction(aFs, aConsole, aOut)
	{
	}
	
void CGetApp::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertAppTestAction::ConstructL(aTestActionSpec);
	TInt pos = 0;
	TInt err = KErrNone;

	// Parse the UID to retrieve and the name to expect
	iUid = TUid::Uid(ParseTagInt(aTestActionSpec.iActionBody, KUid, pos, err));
	iName.Copy(ParseTagString(aTestActionSpec.iActionBody, KAppName, pos, err));
	}

void CGetApp::DoPerformActionL()
	{
	TCertificateAppInfo app;
	TInt index;
	app = iAppManager->ApplicationL(iUid, index);

	if (app.Name() != iName)
		User::Leave(KErrCorrupt);
	}

/////////////////////////////////////////////////////////////////////////////////
//CGetApplications - Gets the applications and compares with what is expected
/////////////////////////////////////////////////////////////////////////////////
CGetApplications::CGetApplications(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CCertAppTestAction(aFs, aConsole, aOut)
	{
	}

CGetApplications::~CGetApplications()
	{
	iAppArray.Close();
	}

void CGetApplications::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertAppTestAction::ConstructL(aTestActionSpec);
	TInt pos = 0;
	TInt err = KErrNone;

	// Parse the UID and name for the new app
	do
		{
		TUid uid(TUid::Uid(ParseTagInt(aTestActionSpec.iActionBody, KUid, pos, err)));
		if (!err)
			{
			TName name;
			name.Copy(ParseTagString(aTestActionSpec.iActionBody, KAppName, pos, err));
			iAppArray.Append(TCertificateAppInfo(uid, name));
			}
		}
	while (!err);
	}

void CGetApplications::DoPerformActionL()
	{
	const RArray<TCertificateAppInfo>& recArray = iAppManager->Applications();
	TInt count = iAppArray.Count();

	if (count != recArray.Count())
		{
		User::Leave(KErrArgument);
		}

	for (TInt i = 0 ; i < count ; ++i)
		{
		TInt j;
		for (j = 0 ; j < count ; ++j)
			{
			if ((iAppArray[i].Id() == recArray[j].Id()) &&
				(iAppArray[i].Name() == recArray[j].Name()))
				{
				break;
				}
			}

		if (j == count)
			{
			// If we get to the end of recArray and there is no match then
			// the arrays definitely do not match
			User::Leave(KErrArgument);
			}
		}
	}
