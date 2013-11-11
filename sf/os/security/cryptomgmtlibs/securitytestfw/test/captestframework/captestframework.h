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

#ifndef CAPTESTFRAMEWORK_H__
#define CAPTESTFRAMEWORK_H__

// Used to transfer data to the helper
const TInt KDllNameTransferSlot=5;
const TInt KShouldPassTransferSlot=6;
const TInt KTestNumberTransferSlot=7;
const TInt KLogFileTransferSlot=8;
const TInt KFileSessionTransferSlot=9;
const TInt KLogFileNameTransferSlot=10;


// markers for the file used to transfer the results

enum TFileMarker
	{
	ETestPassed,
	ETestFailed,
	ETestsEnded,
	EFileEnd,
	};


class RFile;

// classes to be implemented by the actaul test dll.
class MCapabilityTest
	{
public:
	virtual const TDesC& Name() const=0;
	virtual const TDesC& SubName() const=0;
	
	virtual TCapabilitySet CapabilitiesRequired() const=0;
	virtual TUid VidRequired() const=0;
	virtual TUid SidRequired() const=0;

	virtual void RunTestL(RFile& aLogFile)=0;
	virtual void SetExpectPermissionDenied(TBool aExpectPermissionDenied)=0;
	virtual ~MCapabilityTest() {};
	};

class MCapabilityTestFactory
	{
public:
	virtual TInt NumberOfTests()=0;
	virtual MCapabilityTest* Test(TInt aTestNumber)=0;
	virtual ~MCapabilityTestFactory() {};
	};




#endif // #ifndef CAPTESTFRAMEWORK_H__
