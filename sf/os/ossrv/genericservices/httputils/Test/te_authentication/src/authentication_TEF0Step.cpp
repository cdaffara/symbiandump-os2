// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Example CTestStep derived implementation
// 
//

/**
 @file Authentication_TEF0Step.cpp
*/
#include "authentication_TEF0Step.h"
#include "Te_authentication_TEFSuiteDefs.h"

#include <cauthentication.h>

CAuthentication_TEF0Step::~CAuthentication_TEF0Step()
/**
 * Destructor
 */
	{
	}

CAuthentication_TEF0Step::CAuthentication_TEF0Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KAuthentication_TEF0Step);
	}

TVerdict CAuthentication_TEF0Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CAuthentication_TEF0Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	__UHEAP_MARK;		

	CAuthentication* auth;
	CUri8* uri;
	TPtrC8 name;
	TPtrC8 pwd;
	
	// create auth obj from name and pwd
	auth = CAuthentication::NewL(KUser1, KPwd1);
	CleanupStack::PushL(auth);

	
	// *********** test default parameters *********** //	              

	// get name and compare to expected
	name.Set(auth->Name());
	if(name.Compare(KUser1) != 0)
		{
		INFO_PRINTF1(KNameNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}
	
	// get pwd and compare to expected
	pwd.Set(auth->Password());
	if(pwd.Compare(KPwd1) != 0)
		{
		INFO_PRINTF1(KPasswordNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}
		
	// compare default method to expected
	if(auth->Method() != CAuthentication::EDigest)
		{
		INFO_PRINTF1(KMethodNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}
	
	// *********** test changing parameters *********** //
	
	// change and check new name
	auth->SetNameL(KUser2);
	name.Set(auth->Name());
	if(name.Compare(KUser2) != 0)
		{
		INFO_PRINTF1(KNameNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}
	
	// change and check new pwd
	auth->SetPasswordL(KPwd2);
	pwd.Set(auth->Password());
	if(pwd.Compare(KPwd2) != 0)
		{
		INFO_PRINTF1(KPasswordNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}
		
	// change and check new method
	auth->SetMethod(CAuthentication::EBasic);
	if(auth->Method() != CAuthentication::EBasic)
		{
		INFO_PRINTF1(KMethodNotRetreivedProperly); 
		SetTestStepResult(EFail);
		}
		
	CleanupStack::PopAndDestroy(auth);
		
	// *********** test default parameters set using TUriC *********** //
	
	TUriParser8 uriParser;
	uriParser.Parse(KUriUserInfoComplete);
	uri = CUri8::NewLC(uriParser);
	TUriC8 tUri = uri->Uri();
	
	auth = CAuthentication::NewL(tUri);
	CleanupStack::PushL(auth);
	
	name.Set(auth->Name());
	if(name.Compare(KUser1) != 0)
		{
		INFO_PRINTF1(KNameNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}
	
	// get pwd and compare to expected
	pwd.Set(auth->Password());
	if(pwd.Compare(KPwd1) != 0)
		{
		INFO_PRINTF1(KPasswordNotRetreivedProperly); 
		SetTestStepResult(EFail);
		}
		
	// compare default method to expected
	if(auth->Method() != CAuthentication::EDigest)
		{
		INFO_PRINTF1(KMethodNotRetreivedProperly); 
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(2, uri);
		
	// *********** test with incomplete or missing User Info *********** //
	
	TUriParser8 incompleteUriParser;
	CUri8* incompleteUri;
	
	// *** test incomplete uri type 1
	incompleteUriParser.Parse(KUriUserInfoIncomplete1);
	incompleteUri = CUri8::NewLC(incompleteUriParser);
	TUriC8 tIncompleteUri = incompleteUri->Uri();
	
	// create from uri type 1
	auth = CAuthentication::NewL(tIncompleteUri);
	CleanupStack::PushL(auth);
	
	// check has user and no pwd
	name.Set(auth->Name());
	if(name.Compare(KUser1) != 0)
		{
		INFO_PRINTF1(KNameNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}
	pwd.Set(auth->Password());
	if(pwd.Compare(KNullDesC8) != 0)
		{
		INFO_PRINTF1(KPasswordNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}
	CleanupStack::PopAndDestroy(2, incompleteUri);
	
	// *** test incomplete uri type 2
	incompleteUriParser.Parse(KUriUserInfoIncomplete2);
	incompleteUri = CUri8::NewLC(incompleteUriParser);
	TUriC8 tIncompleteUri2 = incompleteUri->Uri();
	
	// create from uri type 2
	auth = CAuthentication::NewL(tIncompleteUri2);
	CleanupStack::PushL(auth);	
	
	// check has user and no pwd
	name.Set(auth->Name());
	if(name.Compare(KUser1) != 0)
		{
		INFO_PRINTF1(KNameNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}		
	pwd.Set(auth->Password());
	if(pwd.Compare(KNullDesC8) != 0)
		{
		INFO_PRINTF1(KPasswordNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}	
	CleanupStack::PopAndDestroy(2, incompleteUri);
	
	// *** test incomplete uri type 3
	incompleteUriParser.Parse(KUriUserInfoIncomplete3);
	incompleteUri = CUri8::NewLC(incompleteUriParser);
	TUriC8 tIncompleteUri3 = incompleteUri->Uri();
	
	// create from uri type 3
	auth = CAuthentication::NewL(tIncompleteUri3);
	CleanupStack::PushL(auth);
	
	// check no user name but has pwd
	name.Set(auth->Name());
	if(name.Compare(KNullDesC8) != 0)
		{
		INFO_PRINTF1(KNameNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}
	pwd.Set(auth->Password());
	if(pwd.Compare(KPwd1) != 0)
		{
		INFO_PRINTF1(KPasswordNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}
	CleanupStack::PopAndDestroy(2, incompleteUri);
	
	// *** test incomplete uri type 4
	incompleteUriParser.Parse(KUriUserInfoIncomplete4);
	incompleteUri = CUri8::NewLC(incompleteUriParser);
	TUriC8 tIncompleteUri4 = incompleteUri->Uri();
	
	// create from uri type 4
	auth = CAuthentication::NewL(tIncompleteUri4);
	CleanupStack::PushL(auth);
	
	// check there is neither user nor pwd
	name.Set(auth->Name());
	if(name.Compare(KNullDesC8) != 0)
		{
		INFO_PRINTF1(KNameNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}		
	pwd.Set(auth->Password());
	if(pwd.Compare(KNullDesC8) != 0)
		{
		INFO_PRINTF1(KPasswordNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}
	CleanupStack::PopAndDestroy(2, incompleteUri);
	
	// *** test incomplete uri type 5
	incompleteUriParser.Parse(KUriUserInfoIncomplete4);
	incompleteUri = CUri8::NewLC(incompleteUriParser);
	TUriC8 tIncompleteUri5 = incompleteUri->Uri();
	
	// create from uri type 5
	auth = CAuthentication::NewL(tIncompleteUri5);
	CleanupStack::PushL(auth);
	
	// check there is neither user nor pwd
	name.Set(auth->Name());
	if(name.Compare(KNullDesC8) != 0)
		{
		INFO_PRINTF1(KNameNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}		
	pwd.Set(auth->Password());
	if(pwd.Compare(KNullDesC8) != 0)
		{
		INFO_PRINTF1(KPasswordNotRetreivedProperly); 
		SetTestStepResult(EFail);		
		}	
	CleanupStack::PopAndDestroy(2, incompleteUri);
	
	// *** test no user info. Must Leave with KErrNotFound.
	incompleteUriParser.Parse(KUriNoUserInfo);
	incompleteUri = CUri8::NewLC(incompleteUriParser);
	TUriC8 tNoUserInfo = incompleteUri->Uri();
	
	// create from uri with no user info
	TRAPD(err, auth = CAuthentication::NewL(tNoUserInfo));
	if(err != KErrNotFound)
		{
		INFO_PRINTF1(KFailedToLeaveWithKErrNotFound); 
		SetTestStepResult(EFail);
		delete auth;
		}	
	CleanupStack::PopAndDestroy(incompleteUri);
		
	__UHEAP_MARKEND;
	return TestStepResult();
	}

TVerdict CAuthentication_TEF0Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
