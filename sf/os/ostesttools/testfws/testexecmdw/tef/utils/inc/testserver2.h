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
* This contains TestServer2.h
*
*/



#ifndef __TEST_SERVER_2_H__
#define __TEST_SERVER_2_H__

#include <test/testexecuteserverbase.h>
#include <test/blockitems.h>
#include <test/activecallback.h>
#include <test/testblockcontroller.h>
#include <test/datadictionary.h>

class CTestServer2 : public CTestServer, public MSharedData
/**
 * @publishedPartner
 * @test
 *
 * @see		CTestServer
 * @see		MSharedData
 *
 * Test server that includes test block processing
 */
	{
private:
	struct TDataElement
		{
		TAny*				iAny;
		TCleanupOperation	iCleanupOperation;
		};
	typedef RHashMap<TDataDictionaryName, TDataElement>		RDataStore;
	typedef THashMapIter<TDataDictionaryName, TDataElement>	TDataIter;

public:
	IMPORT_C				CTestServer2();
	IMPORT_C virtual		~CTestServer2();
	IMPORT_C void			ConstructL();

	/**
	 * Create a test block
	 *
	 * @see		CTestBlockController
	 */
	IMPORT_C virtual CTestBlockController*	CreateTestBlock() =0;

	// CTestServer implementation
	IMPORT_C virtual CTestStep*				CreateTestStep(const TDesC& aStepName);

	// MSharedData implementation
	IMPORT_C virtual void	CreateActiveSchedulerL();
	IMPORT_C virtual void	DeleteActiveSchedulerL();
	IMPORT_C virtual TAny*	GetObjectAndOwnL(const TDesC& aName);
	IMPORT_C virtual void	PutAndDisownL(const TDesC& aName, TAny* aAny, TCleanupOperation aCleanupOperation);

private:
	static TUint32	Hash(const TDataDictionaryName& aName);
	static TBool	Identity(const TDataDictionaryName& aName1, const TDataDictionaryName& aName2);

private:
	RDataStore			iStore;
	CActiveScheduler*	iActiveScheduler;
	};

#endif // __TEST_SERVER_2_H__
