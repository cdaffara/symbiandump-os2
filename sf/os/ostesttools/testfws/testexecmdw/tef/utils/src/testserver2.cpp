/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* @file
* This contains CTestServer2
*
*/



/**
 @prototype
 @test
*/

//	User Includes
#include "testserver2.h"

TUint32 CTestServer2::Hash(const TDataDictionaryName& aName)
	{
	return DefaultHash::Des16(aName);
	}

TBool CTestServer2::Identity(const TDataDictionaryName& aName1, const TDataDictionaryName& aName2)
	{
	return aName1.Compare(aName2)==0;
	}

EXPORT_C CTestServer2::CTestServer2()
/*
 * Constructor
 */
:	CTestServer()
,	iStore(Hash, Identity)
,	iActiveScheduler(NULL)
	{
	}

EXPORT_C CTestServer2::~CTestServer2()
/*
 * Destructor
 */
	{
	TDataIter	iter(iStore);
	iter.Reset();
	for ( const TDataElement* data=iter.NextValue(); data!=NULL; data=iter.NextValue() )
		{
		data->iCleanupOperation(data->iAny);
		}
	iStore.Close();

	if( iActiveScheduler )
		{
		delete iActiveScheduler;
		iActiveScheduler=NULL;
		}
	}

EXPORT_C void CTestServer2::ConstructL()
/*
 * Second phase of construction
 */
	{
	RProcess	handle = RProcess();
	TParsePtrC	serverName(handle.FileName());
	CTestServer::ConstructL(serverName.Name());
	}

// MTEFCallback::MSharedData implementation
EXPORT_C void CTestServer2::CreateActiveSchedulerL()
	{
	if( iActiveScheduler==NULL )
		{
		iActiveScheduler=new (ELeave) CActiveScheduler();
		CActiveScheduler::Install(iActiveScheduler);
		}
	}

// MTEFCallback::MSharedData implementation
EXPORT_C void CTestServer2::DeleteActiveSchedulerL()
	{
	if( iActiveScheduler )
		{
		delete iActiveScheduler;
		iActiveScheduler=NULL;
		}
	}

// MTEFCallback::MSharedData implementation
EXPORT_C TAny* CTestServer2::GetObjectAndOwnL(const TDesC& aName)
	{
	TDataElement*	element=iStore.Find(aName);
	TAny*			ret=NULL;

	if ( element != NULL )
		{
		ret=element->iAny;
		iStore.Remove(aName);
		}
	else
		{
		User::Leave(KErrNotFound);
		}

	return ret;
	}

// MTEFCallback::MSharedData implementation
EXPORT_C void CTestServer2::PutAndDisownL(const TDesC& aName, TAny* aAny, TCleanupOperation aCleanupOperation)
	{
	TDataElement*	element=iStore.Find(aName);
	//	Ensure name does not already exist
	if ( element!=NULL )
		{
		User::Leave(KErrAlreadyExists);
		}

	TDataIter	iter(iStore);
	iter.Reset();
	for ( const TDataElement* data=iter.NextValue(); data!=NULL; data=iter.NextValue() )
		{
		if ( aAny == data->iAny )
			{
			User::Leave(KErrAlreadyExists);
			}
		}

	TDataElement	newElement={aAny, aCleanupOperation};
	iStore.InsertL(aName, newElement);
	}


// CTestServer implementation
EXPORT_C CTestStep* CTestServer2::CreateTestStep(const TDesC& /*aStepName*/)
	{
	CTestStep* step = NULL;
	return step;
	}
