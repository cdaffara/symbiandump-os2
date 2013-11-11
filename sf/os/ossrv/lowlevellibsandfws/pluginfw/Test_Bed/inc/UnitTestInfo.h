// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the definition of the class CUnitTestInfo
// This file comment is for DOxygen only and is ignored by EDoc.
// 
//

/**
 @test
*/

#ifndef __UNITTESTINFO_H__
#define __UNITTESTINFO_H__

#include <e32base.h>

/**
	@internalAll
	Comments : Storage for information about a group of transitions.  This group will normally
	correspond to the contents of a CUnitTest.
 */

NONSHARABLE_CLASS(CUnitTestInfo) : public CBase
	{
public:

/**
	@fn				static CUnitTestInfo* NewLC(const TDesC& aSetId)
	Intended Usage	:	Two phase constructor which leaves a CTransition* on the cleanup stack
	Error Condition	: Leave with an error code.
	@leave  		KErrNoMemory
	@since			7.0
	@param			aSetId An identifier for this CUnitTestInfo
	@return			CUnitTestInfo* A pointer to the newly created class
	@pre 			None
	@post			A CUnitTestInfo is on the CleanupStack
 */
	
	static CUnitTestInfo* NewLC(const TDesC& aSetId);

/**
	@fn				static CUnitTestInfo* NewL(const TDesC& aSetId)
	Intended Usage	:	Two phase constructor which leaves nothing on the cleanup stack
	Error Condition	:	
	@since			7.0
	@param			aSetId An identifier for this CUnitTestInfo
	@return			CUnitTestInfo* A pointer to the newly created class
	@pre 			None
	@post			Nothing is on the CleanupStack
 */
	
	static CUnitTestInfo* NewL(const TDesC& aSetId);

/**
	@fn				~CUnitTestInfo()
	Intended Usage	:	Standard destructor
	Error Condition	:	
	@since			7.0
 */
	
	~CUnitTestInfo();

/**
	@fn				inline const TDesC& UnitTestId() const
	Intended Usage	:	Returns the identifier of this CUnitTestInfo
	Error Condition	:	
	@since			7.0
	@return			const TDesC& The identifier of this UnitTest
	@pre 			None
 */
	
	inline const TDesC& UnitTestId() const;

private:
/**
	@fn				CUnitTestInfo()
	Intended Usage	:	Default constructor
	Error Condition	:	
	@since			7.0
 */
	
	CUnitTestInfo();

/**
	@fn				void ConstructL(const TDesC& aSetId)
	Intended Usage	:	Second phase of safe construction method
	Error Condition	:	
	@since			7.0
	@param			aSetId An identifier for this CUnitTestInfo
	@pre 			First phase of construction is complete
	@post			Object if fully constructed
 */
	
	void ConstructL(const TDesC& aUnitTestId);

private:
/** An identifier for this unit test */
	
	HBufC* iUnitTestId;
	};

#include <ecom/test_bed/unittestinfo.inl>

#endif
