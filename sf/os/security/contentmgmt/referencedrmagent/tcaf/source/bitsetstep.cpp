/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <caf/caf.h>
#include <caf/bitset.h>
#include <caf/attribute.h>
#include <s32mem.h>
#include "bitsetstep.h"

const TInt KAttrTop = 10;

using namespace ContentAccess;

CBitsetBaseStep::CBitsetBaseStep(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);
	}

TVerdict CBitsetBaseStep::doTestStepPreambleL()
	{
	return TestStepResult();
	}

TVerdict CBitsetBaseStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

CBitset* CBitsetBaseStep::GetBitsetLC(const TDesC& aHeader)
	{
	_LIT(KBitCount, "bitcount");
	_LIT(KBit, "bit%02d");

	// Firstly, from the script, get the number of bits to set. If aHeader
	// is "left-", then the bitcount key is "left-bitcount"
	HBufC* buf = HBufC::NewLC(aHeader.Length() + KBitCount().Length());
	TPtr ptr(buf->Des());
	ptr = aHeader;
	ptr.Append(KBitCount());
	TInt bitcount = 0;
	GetIntFromConfig(ConfigSection(), ptr, bitcount);

	INFO_PRINTF3(_L("%S = %d"), &ptr, bitcount);

	// Now, create the bitset
	CBitset* bitset = CBitset::NewLC(bitcount);
	
	TInt i = 0;
	for (; i < bitcount; ++i)
		{
		TInt bit = 0;
		ptr = aHeader;
		ptr.AppendFormat(KBit, i);
		GetIntFromConfig(ConfigSection(), ptr, bit);
		if (bit)
			{
			bitset->Set(i);
			INFO_PRINTF2(_L("%S is set"), &ptr);
			}
		}

	CleanupStack::Pop(bitset);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PushL(bitset);
	return bitset;
	}

/*
 * Step2 performs some basic internal Bitset sanity tests
 *
 */

CBasicBitsetStep::CBasicBitsetStep() 
	: CBitsetBaseStep(KBasicBitsetStep)
	{
	}

//	EIsProtected EIsForwardable EIsModifyable EIsCopyable
TVerdict CBasicBitsetStep::doTestStepL()
	{
	TInt i;
	SetTestStepResult(EPass); // Default result to PASS

	__UHEAP_MARK;
	INFO_PRINTF1(_L("Basic Bitset Test"));

	// Note we must size according to current EAttrTop (attribute.h)
	INFO_PRINTF1(_L("Creating set1..."));
	CBitset *set1 = CBitset::NewLC((TAttribute) KAttrTop); // on cleanup

	// check that all the bits are initially not set
	for(i = 0; i < KAttrTop; i++)
		{
		if (set1->IsSet(i))
			{
			INFO_PRINTF1(_L("Bitset::NewLC() test failed."));
			SetTestStepResult(EFail);
			}
		}
	
	INFO_PRINTF1(_L("Performing single bit set/test..."));
	set1->Set(EIsForwardable);

	// check that only EIsForwardable is set
	for(i = 0; i < KAttrTop; i++)
		{
		if (set1->IsSet(i) && i != EIsForwardable)
			{
			INFO_PRINTF1(_L("Single test/set(1) test failed."));
			SetTestStepResult(EFail);
			}
		}
	if (!set1->IsSet(EIsForwardable))
		{
		INFO_PRINTF1(_L("Single test/set(2) failed."));
		SetTestStepResult(EFail);
		}

	set1->Unset(EIsForwardable);

	// check that none of the bits are set
	for(i = 0; i < KAttrTop; i++)
		{
		if (set1->IsSet(i))
			{
			INFO_PRINTF1(_L("Single test/set(3) failed."));
			SetTestStepResult(EFail);
			}
		}

	INFO_PRINTF1(_L("Performing setall tests..."));
	set1->SetAll();

	// check that all bits are set
	for(i = 0; i < KAttrTop; i++)
		{
		if (!set1->IsSet(i))
			{
			INFO_PRINTF1(_L("SetAll test failed."));
			SetTestStepResult(EFail);
			}
		}
	
	set1->Reset();

	// check all bits are reset
	for(i = 0; i < KAttrTop; i++)
		{
		if (set1->IsSet(i))
			{
			INFO_PRINTF1(_L("Reset test failed."));
			SetTestStepResult(EFail);
			}
		}

	CleanupStack::PopAndDestroy(set1);	
	
	__UHEAP_MARKEND;

	return TestStepResult();
	}

// --------------------------------------------------------------------------
// This step tests the bitset SetList and IsSetList functions

CBitsetListStep::CBitsetListStep()
	: CBitsetBaseStep(KBitsetListStep)
	{
	}

TVerdict CBitsetListStep::doTestStepL()
	{
	TInt i;
	SetTestStepResult(EPass); 

	__UHEAP_MARK;

	INFO_PRINTF1(_L("Creating set1..."));

	CBitset *set1 = CBitset::NewLC(KAttrTop); 

	INFO_PRINTF1(_L("Performing SetList call"));
	set1->SetListL(2, EIsCopyable, EIsModifyable);

	for(i = 0; i < KAttrTop; i++)
		{
		if (set1->IsSet(i) && i != EIsCopyable && i != EIsModifyable)
			{
			INFO_PRINTF1(_L("SetList(1) failed."));
			SetTestStepResult(EFail);
			}
		}

	if (!set1->IsSet(EIsModifyable) || !set1->IsSet(EIsCopyable))
		{
		INFO_PRINTF1(_L("SetList(2) failed."));
		SetTestStepResult(EFail);
		}

	// Now check the IsSetList call
	INFO_PRINTF1(_L("Performing IsSetList calls"));
	if (!set1->IsSetList(2, EIsCopyable, EIsModifyable))
		{
		INFO_PRINTF1(_L("IsSetList call(3) failed."));
		SetTestStepResult(EFail);
		}

	if (set1->IsSetList(2, EIsProtected, EIsForwardable))
		{
		INFO_PRINTF1(_L("IsSetList call(4) failed."));
		SetTestStepResult(EFail);
		}

	CleanupStack::PopAndDestroy(set1);	
	__UHEAP_MARKEND;

	return TestStepResult();
	}

// --------------------------------------------------------------------------

CBitsetEqualityStep::CBitsetEqualityStep()
	: CBitsetBaseStep(KBitsetEqualityStep)
	{
	}

TVerdict CBitsetEqualityStep::doTestStepL()
	{
	SetTestStepResult(EPass); // Default result to EPass

	__UHEAP_MARK;
	// Get the bitset from the script section
	CBitset* left = GetBitsetLC(_L("left-"));
	CBitset* right = GetBitsetLC(_L("right-"));

	// Now see whether we expect the result to be equal
	TBool equalExpected = EFalse;
	GetIntFromConfig(ConfigSection(), _L("equal"), equalExpected);

	if (equalExpected)
		{
		INFO_PRINTF1(_L("Equality expected"));
		}
	else
		{
		INFO_PRINTF1(_L("Inequality expected"));
		}

	TBool result = (*left == *right);
	if (!result != !equalExpected)
		{
		INFO_PRINTF1(_L("Equality test failed."));
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(2, left);

	__UHEAP_MARKEND;

	return TestStepResult();
	}

// --------------------------------------------------------------------------

CBitsetCopyStep::CBitsetCopyStep()
	: CBitsetBaseStep(KBitsetCopyStep)
	{
	}

TVerdict CBitsetCopyStep::doTestStepL()
	{
	SetTestStepResult(EPass); // Default result to EPass

	__UHEAP_MARK;
	// Get the bitset from the script section
	CBitset* set = GetBitsetLC(KNullDesC);

	// Now, create a copy
	CBitset* copy = CBitset::NewLC(*set);

	// Now check the copy
	if (*set != *copy)
		{
		INFO_PRINTF1(_L("Copy constructor return unequal result."));
		SetTestStepResult(EFail);
		}

	// Now create another bitset of arbitrary length
	CBitset* another = CBitset::NewLC(5);

	// Perform assignment
	*another = *copy;

	// Now check another equals the original set
	if (*set != *another)
		{
		INFO_PRINTF1(_L("operator= returned unequal result."));
		SetTestStepResult(EFail);
		}

	// Now invert another and copy and make sure they are equal
	another->Invert();
	copy->Invert();

	if (*copy != *another)
		{
		INFO_PRINTF1(_L("Invert returned unequal result."));
		SetTestStepResult(EFail);
		}

	// Invert the copy again and ensure it is equal to the original
	copy->Invert();
	if (*set != *copy)
		{
		INFO_PRINTF1(_L("Double invert fails."));
		SetTestStepResult(EFail);
		}

	CleanupStack::PopAndDestroy(3, set);

	__UHEAP_MARKEND;

	return TestStepResult();
	}

// --------------------------------------------------------------------------

CBitsetSerialiseStep::CBitsetSerialiseStep()
	: CBitsetBaseStep(KBitsetSerialiseStep)
	{
	}

TVerdict CBitsetSerialiseStep::doTestStepL()
	{
	SetTestStepResult(EPass); // Default result to EPass

	__UHEAP_MARK;
	// Get the bitset from the script section
	CBitset* set = GetBitsetLC(KNullDesC);

	// Create a buffer stream
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);

	// call the stream function
	stream << *set;
	CleanupStack::PopAndDestroy(&stream);

	// Now, create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* des = HBufC8::NewL(buf->Size());
	TPtr8 ptr(des->Des());
	buf->Read(0, ptr, buf->Size());

	// destroy the buffer
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PushL(des);

	// Now, stream a new bitset from the descriptor
	CBitset* newset = CBitset::NewLC(5);
	RDesReadStream readstream(*des);
	CleanupClosePushL(readstream);
	readstream >> *newset;
	CleanupStack::PopAndDestroy(&readstream);

	// Now check that the new bitset equals the old one
	if (*set != *newset)
		{
		INFO_PRINTF1(_L("serialisation returned unequal result."));
		SetTestStepResult(EFail);
		}

	CleanupStack::PopAndDestroy(3, set);

	__UHEAP_MARKEND;

	return TestStepResult();
	}

// --------------------------------------------------------------------------

CBitsetPanicStep::CBitsetPanicStep()
	: CBitsetBaseStep(KBitsetPanicStep)
	{
	}

TVerdict CBitsetPanicStep::doTestStepL()
	{
	SetTestStepResult(EPass); // Default result to EPass

	__UHEAP_MARK;
	// Get the bitset from the script section
	CBitset* panic = GetBitsetLC(_L("panic-"));

	// Now see whether we expect the result to be equal
	TInt panictest = -1;
	GetIntFromConfig(ConfigSection(), _L("panictest"), panictest);

	// all of the following cases should panic
	switch(panictest)
		{
	case 1:
		INFO_PRINTF1(_L("IsSet(-1)"));
		panic->IsSet(-1);
		break;
	case 2:
		INFO_PRINTF1(_L("IsSet(MaxBits()+1)"));
		panic->IsSet(panic->MaxBits()+1);
		break;
	case 3:
		INFO_PRINTF1(_L("Set(-1)"));
		panic->Set(-1);
		break;
	case 4:
		INFO_PRINTF1(_L("Set(MaxBits()+1)"));
		panic->Set(panic->MaxBits()+1);
		break;
	case 5:
		INFO_PRINTF1(_L("UnSet(-1)"));
		panic->Unset(-1);
		break;
	case 6:
		INFO_PRINTF1(_L("UnSet(MaxBits()+1)"));
		panic->Unset(panic->MaxBits()+1);
		break;

	default:
		SetTestStepResult(EFail); 
		};

	SetTestStepResult(EFail); 
	CleanupStack::PopAndDestroy(panic);

	__UHEAP_MARKEND;

	return TestStepResult();
	}

