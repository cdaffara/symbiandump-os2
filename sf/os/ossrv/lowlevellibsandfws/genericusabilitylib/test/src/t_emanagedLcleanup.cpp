// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Overview:
// Test methods of the LManagedX and LCleanedupX classes.
// 
// 
//

#include "emanaged.h"
#include <e32base.h>
#include <e32test.h>
#include <e32def.h>
#include <e32cmn.h>


extern RTest test;

template<TInt Value>
class CTracker : public CBase
	{
public:
	enum TConstructionMode { ENonleavingConstructor, EConstructorLeaves};

	static CTracker* NewL()
		{
		CTracker* ptr = new(ELeave) CTracker;
		ptr->ConstructL();
		return ptr;
		}
	CTracker():iData(NULL)
		{
		test.Printf(_L("  CTracker - %x\n"), this);
		}

	void ConstructL()
		{
		test.Printf(_L("  CTracker::ConstructL - %x\n"), this);
		iData = new(ELeave) TInt(Value);
		}
public:
	virtual ~CTracker()
		{
		test.Printf(_L("  ~CTracker - %x\n"), this);
		if(iData)
			{
			delete iData;	
			}
		}

	virtual void MemFunc()
		{
		test.Printf(_L("  CTracker::MemFunc - %x\n"), this);
		}

	static void StaticMemberRef(const CTracker& aTracker)
		{
		test.Printf(_L("  CTracker::StaticMemberRef - %x\n"), &aTracker);
		}

	static void StaticMemberPtr(CTracker* aTracker)
		{
		test.Printf(_L("  CTracker::StaticMemberPtr - %x\n"), aTracker);
		}

  private:
	TInt* iData;
	};

namespace Log
{

class RLoggerNew
	{
  public:
	RLoggerNew()
		: iData(NULL)
		{
		test.Printf(_L("  RLoggerNew - %x\n"), this);
		}

	RLoggerNew(TInt* aData)
		: iData(aData)
		{
		test.Printf(_L("  RLoggerNew - %x ptr %x -> val %d\n"), this, aData, *iData);
		}

	RLoggerNew(TInt aValue)
		: iData(new(ELeave) TInt(aValue))
		{
		test.Printf(_L("  RLoggerNew - %x value %d\n"), this, *iData);
		}

	void OpenL(TInt aValue)
		{
		iData = new(ELeave) TInt(aValue);
		test.Printf(_L("  RLoggerNew::OpenL(TInt aValue) - %x value %d\n"), this, *iData);
		}

	RLoggerNew(const RLoggerNew& aLogger)
		: iData(aLogger.iData)
		{
		test.Printf(_L("  RLoggerNew::RLoggerNew(const RLoggerNew&) - %x (copy)\n"), this);
		}


	RLoggerNew& operator=(const RLoggerNew& aLogger)
		{
		iData = aLogger.iData;

		test.Printf(_L("  RLoggerNew::operator=(const RLoggerNew&) - %x copy from %x val %d\n"), this, &aLogger, *aLogger.iData);
		return *this;
		}


	~RLoggerNew()
		{
		test.Printf(_L("  ~RLoggerNew - %x\n"), this);
		}


	void Close()
		{
		test.Printf(_L("  RLoggerNew::Close - %x\n"), this);

		// Open or non-NULL initializing constructor not called or
		// cleanup function already called

		__ASSERT_ALWAYS(iData != NULL, test.Panic(_L("NULL pointer")));
		delete iData;
		iData = NULL;
		}


	void Release()
		{
		test.Printf(_L("  RLoggerNew::Release - %x\n"), this);

		// Open or non-NULL initializing constructor not called or
		// cleanup function already called

		__ASSERT_ALWAYS(iData != NULL, test.Panic(_L("NULL pointer")));
		delete iData;
		iData = NULL;
		}


	void Destroy()
		{
		test.Printf(_L("  RLoggerNew::Destroy - %x\n"), this);

		// Open or non-NULL initializing constructor not called or
		// cleanup function already called

		__ASSERT_ALWAYS(iData != NULL, test.Panic(_L("NULL pointer")));
		delete iData;
		iData = NULL;
		}


	void Free()
		{
		test.Printf(_L("  RLoggerNew::Free - %x\n"), this);

		// Open or non-NULL initializing constructor not called or
		// cleanup function already called

		__ASSERT_ALWAYS(iData != NULL, test.Panic(_L("NULL pointer")));
		delete iData;
		iData = NULL;
		}

	void MemFunc()
		{
		test.Printf(_L("  RLoggerNew::MemFunc - %x %x\n"), this, iData);
		}

	static void StaticMemberRef(const RLoggerNew& aTracker)
		{
		test.Printf(_L("  RLoggerNew::StaticMemberRef - %x\n"), &aTracker);
		}

	static void StaticMemberPtr(RLoggerNew* aTracker)
		{
		test.Printf(_L("  RLoggerNew::StaticMemberPtr - %x\n"), aTracker);
		}

	static void Cleanup(TAny* aRLoggerNew)
		{
		static_cast<RLoggerNew*>(aRLoggerNew)->Close();
		}

	TInt* GetData() const
		{
		return iData;
		}

  protected:
	TInt* iData;
	};


DEFINE_CLEANUP_FUNCTION(RLoggerNew, Release);
}//namespace Log



using Log::RLoggerNew;

//------------------------------------------	
//LCleanedupPtr tests here
//------------------
template<TInt Value>
void TestLCleanedupPtrGenerateL()
	{
	__UHEAP_MARK;

		{
		LCleanedupPtr<CTracker<Value> > tracker(CTracker<Value>::NewL());

 		tracker->MemFunc();
 		
		}

	__UHEAP_MARKEND;
	test.Printf(_L("__UHEAP_MARKEND - OK\n"));
	}

template<TInt Value>
void GenerateLCleanedupPtrTestL()
	{
	TestLCleanedupPtrGenerateL<Value>();
	GenerateLCleanedupPtrTestL<Value - 1>();
	}

template<>
void GenerateLCleanedupPtrTestL<0>()
	{
	TestLCleanedupPtrGenerateL<0>();
	}


//-------------------------------------------
//LCleanedupArray tests here
//-----------------
template<TInt Value>
void TestLCleanedupArrayGenerateL()
	{
	__UHEAP_MARK;

		{
		LCleanedupArray<CTracker<Value> > tracker(new(ELeave) CTracker<Value>[Value]);
 		}

	__UHEAP_MARKEND;
	test.Printf(_L("__UHEAP_MARKEND - OK\n"));
	}

template<TInt Value>
void GenerateLCleanedupArrayTestL()
	{
	TestLCleanedupArrayGenerateL<Value>();
	GenerateLCleanedupArrayTestL<Value - 1>();
	}

template<>
void GenerateLCleanedupArrayTestL<0>()
	{
	TestLCleanedupArrayGenerateL<0>();
	}
	
//-----------------------------------
//LCleanedupGuard Tests Here
//---------------------------
template<TInt Value>
void TestLCleanedupGuardTestGenerateL()
	{
	__UHEAP_MARK;

		{
		RLoggerNew logger(Value);
		LCleanedupGuard cleanGuard(RLoggerNew::Cleanup, &logger);
 		}

	__UHEAP_MARKEND;
	test.Printf(_L("__UHEAP_MARKEND - OK\n"));
	}
	
template<TInt Value>
void GenerateLCleanedupGuardTestL()
	{
	TestLCleanedupGuardTestGenerateL<Value>();
	GenerateLCleanedupGuardTestL<Value - 1>();
	}

template<>
void GenerateLCleanedupGuardTestL<0>()
	{
	TestLCleanedupGuardTestGenerateL<0>();
	}

//----------------------------------------
//LCleanedupHandle Tests Here
//-----------------------

template<TInt Value>
void TestLCleanedupHandleGenerateL()
	{
	__UHEAP_MARK;

		{
		LCleanedupHandle<RLoggerNew> logger(Value);
		}

	__UHEAP_MARKEND;
	test.Printf(_L("__UHEAP_MARKEND - OK\n"));
	}
	
template<TInt Value>
void GenerateLCleanedupHandleTestL()
	{
	TestLCleanedupHandleGenerateL<Value>();
	GenerateLCleanedupHandleTestL<Value - 1>();
	}

template<>
void GenerateLCleanedupHandleTestL<0>()
	{
	TestLCleanedupHandleGenerateL<0>();
	}
//------------------------------------------
//LCleanedupRef Tests Here
//-------------------------

template<TInt Value>
void TestLCleanedupRefGenerateL()
	{
	__UHEAP_MARK;

		{
		RLoggerNew logger;
		logger.OpenL(Value);
		LCleanedupRef<RLoggerNew> rlog(logger);
		rlog.ReleaseResource();		
		}

	__UHEAP_MARKEND;
	test.Printf(_L("__UHEAP_MARKEND - OK\n"));
	}
	
template<TInt Value>
void GenerateLCleanedupRefTestL()
	{
	TestLCleanedupRefGenerateL<Value>();
	GenerateLCleanedupRefTestL<Value - 1>();
	}

template<>
void GenerateLCleanedupRefTestL<0>()
	{
	TestLCleanedupRefGenerateL<0>();
	}

/**
@SYMTestCaseID 			BASESRVCS-EUSERHL-UT-4071
@SYMTestCaseDesc 		Tests multiple defined symbols of ManagedPopCleanupStackItem method 
						and also does stress testing of the template classes.
						Tests All the different Templated Class such as
						LCleanedupRef,LCleanedupGuard,LCleanedupHandle,LCleanedupArray and LCleanedupPtr
						for performance as well by explicit instantiation of the templated class using recurrsion.		
@SYMTestPriority 		High
@SYMTestActions 		Creates multiple instances of LCleanedupArray,LCleanedupPtr and LCleanedupRef etc
						Verifies that the objects are automatically cleaned up when they go
						out of scope
@SYMTestExpectedResults All memory allocated for the LXXX objects
						is automatically freed when they go out of scope and there are no 
						errors on multiple definition of ManagedPopCleanupStackItem is produced.
@SYMREQ					DEF137572
*/

TInt TExtendedTestL()
	{
	//------------
	TRAPD(status, GenerateLCleanedupPtrTestL<64>());
	if (status != KErrNone)
		{
		test.Printf(_L("LCleanedupPtr; leave code: %d\n"), status);
		}
	
	TRAPD(status1,GenerateLCleanedupArrayTestL<64>());
	if(status1 !=KErrNone)
		{
		test.Printf(_L("LCleanedupArray completed;leave code:%d\n"),status1);			
		}
	
	TRAPD(status2,GenerateLCleanedupGuardTestL<64>());
	if(status2 !=KErrNone)
		{
		test.Printf(_L("LCleanedupGuard completed;leave code:%d\n"),status2);			
		}
	
	TRAPD(status3,GenerateLCleanedupHandleTestL<64>());
	if(status3 !=KErrNone)
		{
		test.Printf(_L("LCleanedupHandle completed;leave code:%d\n"),status3);			
		}
	
	TRAPD(status4,GenerateLCleanedupRefTestL<64>());
	if(status4 !=KErrNone)
		{
		test.Printf(_L("LCleanedupRef completed;leave code:%d\n"),status4);			
		}
	return status;
	}



