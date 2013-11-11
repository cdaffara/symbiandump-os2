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
// Tests CONSTRUCTORS_MAY_LEAVE and OR_LEAVE macros
// 
//

#include <e32base.h>
#include <e32test.h>
#include <e32def.h>
#include <e32cmn.h>
#include <f32file.h>
#include "emisc.h"
#include "euserhl.h"


RTest test(_L("RAII-based automatic resource management tests"));


TInt TExtendedTestL();

class CTracker : public CBase
	{
  public:
	CONSTRUCTORS_MAY_LEAVE

	enum TConstructionMode { ENonleavingConstructor, EConstructorLeaves};

	static CTracker* NewL()
		{
		return new(ELeave) CTracker;
		}

	static CTracker* NewL(TConstructionMode aMode)
		{
		return new(ELeave) CTracker(aMode);
		}

	CTracker()
		: iData(new(ELeave) TInt)
		{
		test.Printf(_L("  CTracker - %x\n"), this);
		*iData = 3;
		}

	explicit CTracker(TConstructionMode aMode)
		: iData(new(ELeave) TInt(aMode))
		{
		if (aMode == EConstructorLeaves)
			{
			test.Printf(_L("CTracker(EConstructorLeaves): Now leaving with User::Leave(KErrGeneral)\n"));
			User::Leave(KErrGeneral);
			}

		test.Printf(_L("  CTracker - %x\n"), this);
		}

	CTracker(CTracker& aTracker)
		: iData(aTracker.iData.Unmanage())
		{
		test.Printf(_L("  CTracker::CTracker(const CTracker&) - %x (copy)\n"), this);
		}

	CTracker& operator=(CTracker& aTracker)
		{
		iData = aTracker.iData.Unmanage();
		test.Printf(_L("  CTracker::operator=(const CTracker&) - %x (copy)\n"), this);
		return *this;
		}

	virtual ~CTracker()
		{
		test.Printf(_L("  ~CTracker - %x\n"), this);
		}

	virtual void MemFunc()
		{
		test.Printf(_L("  CTracker::MemFunc - %x\n"), this);
		}

	virtual TInt Value()
		{
		return *iData;
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
	LManagedPtr<TInt> iData;
	};

TBool trackerDestroyed = EFalse;

class TCTrackerDestroy
	{
public:
	static void Cleanup(CTracker* aPtr)
		{
		test.Printf(_L("TCTrackerDestroy::Cleanup\n"));
		delete aPtr;
		trackerDestroyed = ETrue;
		}
	};

class CDerivedTracker: public CTracker
	{
  public:
	CONSTRUCTORS_MAY_LEAVE

	static CDerivedTracker* NewL()
		{
		return new(ELeave) CDerivedTracker;
		}

	static CDerivedTracker* NewL(CTracker::TConstructionMode aMode)
		{
		return new(ELeave) CDerivedTracker(aMode);
		}

	CDerivedTracker()
		: iRealData(new(ELeave) TReal64)
		{
		test.Printf(_L("  CDerivedTracker - %x\n"), this);
		*iRealData = 5.5;
		}

	CDerivedTracker(CTracker::TConstructionMode aMode)
		{
		if (aMode == EConstructorLeaves)
			{
			test.Printf(_L("CDerivedTracker(EConstructorLeaves): Now leaving with User::Leave(KErrGeneral)\n"));
			User::Leave(KErrGeneral);
			}

		test.Printf(_L("  CDerivedTracker - %x\n"), this);
		}

	CDerivedTracker(CDerivedTracker& aDerivedTracker)
		: CTracker(aDerivedTracker),
		  iRealData(aDerivedTracker.iRealData.Unmanage())
		{
		test.Printf(_L("  CDerivedTracker::CDerivedTracker(const CDerivedTracker&) - %x (copy)\n"), this);
		}

	CDerivedTracker& operator=(CDerivedTracker& aDerivedTracker)
		{
		CTracker::operator=(aDerivedTracker);
		iRealData = aDerivedTracker.iRealData.Unmanage();
		test.Printf(_L("  CDerivedTracker::operator=(const CDerivedTracker&) - %x (copy)\n"), this);
		return *this;
		}

	virtual ~CDerivedTracker()
		{
		test.Printf(_L("  ~CDerivedTracker - %x\n"), this);
		}

	virtual void MemFunc()
		{
		test.Printf(_L("  CDerivedTracker::MemFunc - %x\n"), this);
		}

	static void StaticMemberRef(const CDerivedTracker& aTracker)
		{
		test.Printf(_L("  CDerivedTracker::StaticMemberRef - %x\n"), &aTracker);
		}

	static void StaticMemberPtr(CDerivedTracker* aTracker)
		{
		test.Printf(_L("  CDerivedTracker::StaticMemberPtr - %x\n"), aTracker);
		}

  private:
	LManagedPtr<TReal64> iRealData;
	};


namespace Log
{

class RLogger
	{
  public:
	RLogger()
		: iData(NULL)
		{
		test.Printf(_L("  RLogger - %x\n"), this);
		}

	RLogger(TInt* aData)
		: iData(aData)
		{
		test.Printf(_L("  RLogger - %x ptr %x -> val %d\n"), this, aData, *iData);
		}

	RLogger(TInt aValue)
		: iData(new(ELeave) TInt(aValue))
		{
		test.Printf(_L("  RLogger - %x value %d\n"), this, *iData);
		}

	void OpenL(TInt aValue)
		{
		iData = new(ELeave) TInt(aValue);
		test.Printf(_L("  RLogger::OpenL(TInt aValue) - %x value %d\n"), this, *iData);
		}

	RLogger(const RLogger& aLogger)
		: iData(aLogger.iData)
		{
		test.Printf(_L("  RLogger::RLogger(const RLogger&) - %x (copy)\n"), this);
		}


	RLogger& operator=(const RLogger& aLogger)
		{
		iData = aLogger.iData;

		test.Printf(_L("  RLogger::operator=(const RLogger&) - %x copy from %x val %d\n"), this, &aLogger, *aLogger.iData);
		return *this;
		}


	~RLogger()
		{
		test.Printf(_L("  ~RLogger - %x\n"), this);
		}


	void Close()
		{
		test.Printf(_L("  RLogger::Close - %x\n"), this);

		// Open or non-NULL initializing constructor not called or
		// cleanup function already called

		__ASSERT_ALWAYS(iData != NULL, test.Panic(_L("NULL pointer")));
		delete iData;
		iData = NULL;
		}


	void Release()
		{
		test.Printf(_L("  RLogger::Release - %x\n"), this);

		// Open or non-NULL initializing constructor not called or
		// cleanup function already called

		__ASSERT_ALWAYS(iData != NULL, test.Panic(_L("NULL pointer")));
		delete iData;
		iData = NULL;
		}


	void Destroy()
		{
		test.Printf(_L("  RLogger::Destroy - %x\n"), this);

		// Open or non-NULL initializing constructor not called or
		// cleanup function already called

		__ASSERT_ALWAYS(iData != NULL, test.Panic(_L("NULL pointer")));
		delete iData;
		iData = NULL;
		}


	void Free()
		{
		test.Printf(_L("  RLogger::Free - %x\n"), this);

		// Open or non-NULL initializing constructor not called or
		// cleanup function already called

		__ASSERT_ALWAYS(iData != NULL, test.Panic(_L("NULL pointer")));
		delete iData;
		iData = NULL;
		}

	void MemFunc()
		{
		test.Printf(_L("  RLogger::MemFunc - %x %x\n"), this, iData);
		}

	static void StaticMemberRef(const RLogger& aTracker)
		{
		test.Printf(_L("  RLogger::StaticMemberRef - %x\n"), &aTracker);
		}

	static void StaticMemberPtr(RLogger* aTracker)
		{
		test.Printf(_L("  RLogger::StaticMemberPtr - %x\n"), aTracker);
		}

	static void Cleanup(TAny* aRlogger)
		{
		static_cast<RLogger*>(aRlogger)->Close();
		}

	TInt* GetData() const
		{
		return iData;
		}

  protected:
	TInt* iData;
	};


DEFINE_CLEANUP_FUNCTION(RLogger, Release);


class ROtherLogger
	{
  public:
	ROtherLogger()
		: iData(NULL)
		{
		test.Printf(_L("  ROtherLogger - %x\n"), this);
		}

	ROtherLogger(TInt* aData)
		: iData(aData)
		{
		test.Printf(_L("  ROtherLogger - %x ptr %x -> val %d\n"), this, aData, *iData);
		}

	ROtherLogger(TInt aValue)
		: iData(new(ELeave) TInt(aValue))
		{
		test.Printf(_L("  ROtherLogger - %x value %d\n"), this, *iData);
		}

	void OpenL(TInt aValue)
		{
		iData = new(ELeave) TInt(aValue);
		test.Printf(_L("  ROtherLogger::OpenL(TInt aValue) - %x value %d\n"), this, *iData);
		}

	ROtherLogger(const ROtherLogger& aLogger)
		: iData(aLogger.iData)
		{
		test.Printf(_L("  ROtherLogger::ROtherLogger(const ROtherLogger&) - %x (copy)\n"), this);
		}


	ROtherLogger& operator=(const ROtherLogger& aLogger)
		{
		iData = aLogger.iData;

		test.Printf(_L("  ROtherLogger::operator=(const ROtherLogger&) - %x copy from %x ptr %x\n"), this, &aLogger, aLogger.iData);
		return *this;
		}


	~ROtherLogger()
		{
		test.Printf(_L("  ~ROtherLogger - %x\n"), this);
		}


	void Close()
		{
		test.Printf(_L("  ROtherLogger::Close - %x\n"), this);

		// Open or non-NULL initializing constructor not called or
		// cleanup function already called

		__ASSERT_ALWAYS(iData != NULL, test.Panic(_L("NULL pointer")));
		delete iData;
		iData = NULL;
		}


	void Release()
		{
		test.Printf(_L("  ROtherLogger::Release - %x\n"), this);

		// Open or non-NULL initializing constructor not called or
		// cleanup function already called

		__ASSERT_ALWAYS(iData != NULL, test.Panic(_L("NULL pointer")));
		delete iData;
		iData = NULL;
		}


	void Destroy()
		{
		test.Printf(_L("  ROtherLogger::Destroy - %x\n"), this);

		// Open or non-NULL initializing constructor not called or
		// cleanup function already called

		__ASSERT_ALWAYS(iData != NULL, test.Panic(_L("NULL pointer")));
		delete iData;
		iData = NULL;
		}


	void Free()
		{
		test.Printf(_L("  ROtherLogger::Free - %x\n"), this);

		// Open or non-NULL initializing constructor not called or
		// cleanup function already called

		__ASSERT_ALWAYS(iData != NULL, test.Panic(_L("NULL pointer")));
		delete iData;
		iData = NULL;
		}

	void MemFunc()
		{
		test.Printf(_L("  ROtherLogger::MemFunc - %x %x\n"), this, iData);
		}

	static void StaticMemberRef(const ROtherLogger& aTracker)
		{
		test.Printf(_L("  ROtherLogger::StaticMemberRef - %x\n"), &aTracker);
		}

	static void StaticMemberPtr(ROtherLogger* aTracker)
		{
		test.Printf(_L("  ROtherLogger::StaticMemberPtr - %x\n"), aTracker);
		}

	static void Cleanup(TAny* aRlogger)
		{
		static_cast<ROtherLogger*>(aRlogger)->Close();
		}

	operator RLogger() const
		{
		test.Printf(_L("  ROtherLogger::operator RLogger() - %x\n"), this);
		return RLogger(iData);
		}

	ROtherLogger& operator=(const RLogger& aLogger)
		{
		iData = aLogger.GetData();

		test.Printf(_L("  ROtherLogger::operator=(const RLogger&) - %x copy from %x ptr %x\n"), this, &aLogger, aLogger.GetData());
		return *this;
		}

  protected:
	TInt* iData;
	};

} // namespace Log

class RPair
	{
  public:
	RPair(const TInt& x, const TInt& y)
		: iX(x), iY(y)
		{
		test.Printf(_L("  RPair::RPair(const TInt& x, const TInt& y) %x %d %d\n"), iX, iY);
		}

	RPair(TInt& x, TInt& y)
		: iX(x), iY(y)
		{
		test.Printf(_L("  RPair::RPair(TInt& x, TInt& y) %x %d %d\n"), iX, iY);
		}

	void Close()
		{
		test.Printf(_L("  RPair::Close() %x %d %d\n"), iX, iY);
		}

  private:
	TInt iX;
	TInt iY;
	};


using Log::RLogger;
using Log::ROtherLogger;

class TLogger
	{
  public:
	TLogger()
		{
		test.Printf(_L("  TLogger - %x\n"), this);
		}

	TLogger(const TLogger&)
		{
		test.Printf(_L("  TLogger::TLogger(const TLogger& aLogger) - %x (copy)\n"), this);
		}


	TLogger& operator=(const TLogger&)
		{
		test.Printf(_L("  TLogger::operator=(const TLogger& aLogger) - %x (copy)\n"), this);
		return *this;
		}

	~TLogger()
		{
		test.Printf(_L("  ~TLogger - %x\n"), this);
		}

	void Test()
		{
		test.Printf(_L("  TLogger::Test - %x\n"), this);
		}
	};


static TInt one = 1;
static TInt two = 2;

// CComposite object owns other objects:

class CComposite: public CBase
	{
  public:
	enum TConstructionMode
	{
		ENonleavingConstructor,
		EConstructorLeaves,
		EMemberConstructorLeaves
	};

	static CComposite* NewL()
		{
		return new(ELeave) CComposite;
		}

	static CComposite* NewL(TConstructionMode aMode)
		{
		return new(ELeave) CComposite(aMode);
		}

	CONSTRUCTORS_MAY_LEAVE

	CComposite();
	CComposite(TConstructionMode aMode);
	~CComposite();

	void ReleaseLoggers();
	void ReleaseArrays();

  private:
	TInt iVal;

// By-value component objects
	const LManagedHandle<RLogger> iConstLogger;
	LManagedHandle<RLogger> iLogger;
	LManagedHandle<RLogger> iLogger2;
	LManagedHandle<RLogger> iLogger3;

	ROtherLogger iOther;
	LManagedRef<ROtherLogger> iOtherRef;
	LManagedHandle<RLogger> iAnotherLogger;

	LManagedHandle<RLogger, TDestroy> iDestroyer;

// Component objects contained by pointer
	LManagedPtr<CTracker> iNullPtr;
	//Define a custom cleanup strategy for CTracker
	LManagedPtr<CTracker> iAutoPtr;
	LManagedPtr<CTracker> iAutoPtr2;
	LManagedPtr<CTracker> iAutoPtr3;
	LManagedPtr<CTracker, TCTrackerDestroy> iAutoPtr4;
	LManagedPtr<CDerivedTracker> iAutoPtr5;

	LManagedArray<CTracker> iEmptyArray;
	LManagedArray<CTracker> iAutoArray;
	LManagedArray<CTracker> iAutoArray2;
	static const TInt KNumTrackers = 3;

	LManagedHandle<RPointerArray<CTracker>, TResetAndDestroy> iPtrArray;

	RPointerArray<CTracker> iArray;
	RPointerArray<CTracker> iArray2;
	LManagedRef<RPointerArray<CTracker>, TResetAndDestroy> iArrayWrapper;

	RLogger iRawLogger;
	LManagedGuard iGuard;

	LManagedHandle<RPair> iPair1;
	LManagedHandle<RPair> iPair2;
	LManagedHandle<RPair> iPair3;
	LManagedHandle<RPair> iPair4;
	};
	
CComposite::CComposite()
	: iConstLogger(42),
	  iLogger(1),
	  iLogger2(2),
	  iOther(42),
	  iOtherRef(iOther),
	  iAnotherLogger(iOtherRef.Unmanage()),
	  iDestroyer(2),
	  iAutoPtr(CTracker::NewL()),
	  iAutoPtr2(CTracker::NewL()),
	  iAutoPtr3(CTracker::NewL()),
	  iAutoPtr4(CTracker::NewL()),
	  iAutoPtr5(CDerivedTracker::NewL()),
	  iAutoArray(new(ELeave) CTracker[KNumTrackers]),
	  iAutoArray2(new(ELeave) CTracker[KNumTrackers]),
	  iPtrArray(3),
	  iArrayWrapper(iArray),
	  iRawLogger(42),
	  iGuard(RLogger::Cleanup, &iRawLogger),
	  iPair1(1, 2),
	  iPair2(one, 2),
	  iPair3(1, two),
	  iPair4(one, two)
	{
	test.Printf(_L("  CComposite - %x\n"), this);
	
	//Clear the trackerDestroyed flag.  This flag is set
	//in the custom cleanup strategy for CTracker which is
	//defined for iAutoPtr
	trackerDestroyed = EFalse;

	iLogger = iLogger2.Unmanage();
	iLogger3->OpenL(3);

	RLogger logger = iLogger3.Unmanage();
	iLogger3.ReleaseResource();
	logger.Release();

	iAutoPtr = iAutoPtr2.Unmanage();
	iAutoPtr2.ReleaseResource();
	
	iAutoPtr4 = iAutoPtr5.Unmanage();
	

	if (iAutoPtr3)
		{
		test.Printf(_L("  iAutoPtr3 pointer = %x\n"), iAutoPtr3.Get());
		}
	else
		{
		test.Printf(_L("  iAutoPtr3 pointer is null (%x)\n"), iAutoPtr3.Get());
		}

	if (!iNullPtr)
		{
		test.Printf(_L("  iNullPtr is null (%x)\n"), iNullPtr.Get());
		}
	else
		{
		test.Printf(_L("  iNullPtr is (%x)\n"), iNullPtr.Get());
		}

	if (iAutoPtr3 != iNullPtr)
		{
		test.Printf(_L("  iAutoPtr3 pointer is not null = %x\n"), iAutoPtr3.Get());
		}

	if (iAutoPtr3 == iNullPtr)
		{
		test.Printf(_L("  iAutoPtr3 pointer is null (%x)\n"), iAutoPtr3.Get());
		}

	if (iNullPtr < iAutoPtr3)
		{
		test.Printf(_L("  iNullPtr < iAutoPtr3 (%x)\n"), iAutoPtr3.Get());
		}

	iAutoPtr3 = NULL;


	iAutoArray = iAutoArray2.Unmanage();
	iAutoArray2.ReleaseResource();

	iPtrArray->Append(CTracker::NewL());
	iPtrArray->Append(CTracker::NewL());

	iArrayWrapper->Append(CTracker::NewL());

	iArrayWrapper = iArray2;

	iArrayWrapper->Append(CTracker::NewL());
	iArrayWrapper->Append(CTracker::NewL());

	test.Printf(_L("  iLogger val = %d\n"), iLogger->GetData());
	test.Printf(_L("  iDestroyer val = %d\n"), (*iDestroyer).GetData());

	test.Printf(_L("  iConstLogger val = %d\n"), iConstLogger->GetData());
	test.Printf(_L("  iConstLogger val = %d\n"), (*iConstLogger).GetData());

	test.Printf(_L("  iAutoArray[0] val = %d\n"), iAutoArray[0].Value());

	iOtherRef.ReleaseResource();
	}


CComposite::CComposite(TConstructionMode aMode)
	: iConstLogger(42),
	  iLogger(1),
	  iLogger2(2),
	  iLogger3(new int(3)),
	  iOther(42),
	  iOtherRef(iOther),
	  iAnotherLogger(iOtherRef.Unmanage()),
	  iDestroyer(2),
	  iAutoPtr(CTracker::NewL(aMode == EMemberConstructorLeaves ? CTracker::EConstructorLeaves : CTracker::ENonleavingConstructor)),
	  iAutoArray(new(ELeave) CTracker[KNumTrackers]),
	  iPtrArray(3),
	  iArrayWrapper(iArray),
	  iRawLogger(42),
	  iGuard(RLogger::Cleanup, &iRawLogger),
	  iPair1(1, 2),
	  iPair2(one, 2),
	  iPair3(1, two),
	  iPair4(one, two)
	{
	test.Printf(_L("  CComposite(%d) with leaving constructor - %x\n"),
				aMode,
				this);

	iLogger = iLogger2.Unmanage();

	iPtrArray->Append(CTracker::NewL());
	iPtrArray->Append(CTracker::NewL());

	iArrayWrapper->Append(CTracker::NewL());
	iArrayWrapper->Append(CTracker::NewL());

	if (aMode == EConstructorLeaves)
		{
		test.Printf(_L("CComposite(EConstructorLeaves): Now leaving with User::Leave(KErrGeneral)\n"));
		User::Leave(KErrGeneral);
		}
	}

CComposite::~CComposite()
	{
	test.Printf(_L("  ~CComposite - %x\n"), this);
	}


void CComposite::ReleaseLoggers()
	{
	test.Printf(_L("  CComposite::ReleaseLoggers - %x\n"), this);

	iLogger.ReleaseResource();
	iDestroyer.ReleaseResource();

	iGuard.Dismiss();
	iRawLogger.Release();
	}


void CComposite::ReleaseArrays()
	{
	test.Printf(_L("  CComposite::ReleaseArrays - %x\n"), this);

	iAutoArray.ReleaseResource();
	}


void TrackingDesCall(const TDesC& aConst, TDes& aMut)
	{
	test.Printf(_L("TrackingDesCall const=%S, mut=%S\n"), &aConst, &aMut);
	aMut.Append(_L(": Appended"));
	}



/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4028
@SYMTestCaseDesc Tests automatic cleanup if a leave occurs in a Constructor
@SYMTestPriority High
@SYMTestActions Creates a new CTracker instance passing EConstructorLeaves to the 
				factory function which causes the constructor to leave.
				Verifies that all memory allocated in the constructor is 
				automatically freed if the constructor leaves.				
@SYMTestExpectedResults All memory allocated in the constructor of CTracker should 
				be automatically freed if the constructor leaves.
@SYMREQ	10368
*/
void TestLeaveFromConstructor()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4028"));
	
	__UHEAP_MARK;
	test.Next(_L("Test CONSTRUCTORS_MAY_LEAVE"));

	TRAPD(status,
		  {
		  CTracker* tracker = CTracker::NewL(CTracker::EConstructorLeaves);
		  tracker->MemFunc();
		  });
	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4029
@SYMTestCaseDesc Tests automatic cleanup of LCleanedupHandle using different 
			cleanup strategies
@SYMTestPriority High
@SYMTestActions Creates a LCleanedupHandle<RLogger> objects with different cleanup strategies.
				Verifies that the objects are automatically cleaned up when they go
				out of scope
@SYMTestExpectedResults All memory allocated for the LCleanedupHandle<RLogger>
				objects is automatically freed when the object goes out of scope.
@SYMREQ	10373-8
*/
void TestLCleanedupHandleStrategies()
	{
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4029"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupHandle - test cleanup strategies"));

		{
		LCleanedupHandle<RLogger> logger(1);
		LCleanedupHandle<RLogger, TClose> logger2(2);
		LCleanedupHandle<RLogger, TRelease> logger3(3);
		LCleanedupHandle<RLogger, TDestroy> logger4(4);
		LCleanedupHandle<RLogger, TFree> logger5(5);
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4030
@SYMTestCaseDesc Tests construction of LCleanedUpHandle objects using complex constructors
@SYMTestPriority High
@SYMTestActions Creates a LCleanedupHandle<RPair> objects using different constructors.
				Verifies that the objects are automatically cleaned up when they go
				out of scope
@SYMTestExpectedResults All memory allocated for the LCleanedupHandle<RPair>
				objects is automatically freed when the object goes out of scope.
@SYMREQ	10373-8
*/
void TestLCleanedupConstructors()
	{
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4030"));
	
	__UHEAP_MARK;

		{
		LCleanedupHandle<RLogger> logger(1);

		LCleanedupHandle<RPair> pair1(1, 2);
		LCleanedupHandle<RPair> pair2(one, 2);
		LCleanedupHandle<RPair> pair3(1, two);
		LCleanedupHandle<RPair> pair4(one, two);
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4031
@SYMTestCaseDesc Tests automatic cleanup of LCleanedupHandle
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupHandle<RLogger> on the stack and uses the object.
				Verifies that the object is automatically cleaned up when it goes out of scope
@SYMTestExpectedResults All memory allocated for the LCleanedupHandle<RLogger>
				is automatically freed when the object goes out of scope
@SYMREQ	10373-8
*/
void TestLCleanedupHandleNormalL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4031"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupHandle - test normal exit from a block scope"));

		{
		LCleanedupHandle<RLogger> logger;
		logger->OpenL(42);

		LCleanedupHandle<RLogger> logger2(42);
		}

	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4032
@SYMTestCaseDesc Tests automatic cleanup of LCleanedupHandle object on a leave
@SYMTestPriority High
@SYMTestActions Creates a LCleanedupHandle<RLogger> on the stack and uses the object.
				Verifies that the object is automatically cleaned up when a leave occurs
@SYMTestExpectedResults All memory allocated for the LCleanedupHandle<RLogger>
				is automatically freed when a leave occurs
@SYMREQ	10373-8
*/
void TestLCleanedupHandleLeave()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4032"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupHandle - test leave"));

	TRAPD(status,
		  {
		  LCleanedupHandle<RLogger> logger(42);
		  test.Printf(_L("TestLCleanedupHandleLeave(): Now leaving with User::Leave(KErrGeneral)\n"));
		  User::Leave(KErrGeneral);
		  });
	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4033
@SYMTestCaseDesc Tests manual cleanup of LCleanedupHandle object.
@SYMTestPriority High
@SYMTestActions Creates LCleanedupHandle<RLogger> objects on the stack and 
				manually Unmanages and Closes the Handles.
				Verifies that all memory allocated for the objects can be freed
				manually.
@SYMTestExpectedResults All memory allocated for the LCleanedupHandle<RLogger>
				is freed by calling Unmanage() and Close()
@SYMREQ	10373-8, 10375-5
*/
void TestLCleanedupHandleUnmanage()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4033"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupHandle - test LCleanedupHandle::Unmanage"));

	LCleanedupHandle<RLogger> logger1(1);
	LCleanedupHandle<RLogger> logger2(2);
	logger1.Unmanage();
	logger1->Close();
	logger2.Unmanage();
	logger2->Close();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4034
@SYMTestCaseDesc Tests manual cleanup of LCleanedupHandle object on a leave
@SYMTestPriority High
@SYMTestActions Creates LCleanedupHandle<RLogger> objects on the stack and 
				manually Unmanages them.  
				Forces a leave and then Closes the Handles.
				Verifies that all memory allocated for the objects can be freed
				manually in the event of a leave occuring
@SYMTestExpectedResults All memory allocated for the LCleanedupHandle<RLogger>
				is freed by calling Unmanage() and Close()
@SYMREQ	10373-8, 10375-5
*/
void TestLCleanedupHandleUnmanageLeave()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4034"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupHandle - test LCleanedupHandle::Unmanage and leave"));

	RLogger logger1;
	RLogger logger2;
	TRAPD(status,
		  {
		  LCleanedupHandle<RLogger> auto_logger1(1);
		  LCleanedupHandle<RLogger> auto_logger2(2);

		  logger1 = auto_logger1.Unmanage();
		  logger2 = auto_logger2.Unmanage();

		  test.Printf(_L("TestLCleanedupHandleUnmanageLeave(): Now leaving with User::Leave(KErrGeneral)\n"));

		  User::Leave(KErrGeneral);
		  });
	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		logger1.Close();
		logger2.Close();
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4035
@SYMTestCaseDesc Tests access to managed object through LCleanedupHandle
@SYMTestPriority High
@SYMTestActions Creates a LCleanedupHandle<RLogger> on the stack and 
				uses the LCleanedupHandle object to access RLogger methods
				via the -> operator and the LCleanedupHandle methods via 
				the . operator
@SYMTestExpectedResults All public RLogger methods and LCleanedupHandle methods
				should be accessible through the LCleanedupHandle object.
@SYMREQ	10373-8
*/
void TestLCleanedupHandleObjectAccess()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4035"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupHandle - test object access"));

		{
		LCleanedupHandle<RLogger> logger(42);
		logger->MemFunc();
		RLogger::StaticMemberRef(*logger);
		RLogger::StaticMemberPtr(&logger.Get());
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4036
@SYMTestCaseDesc Tests forced cleanup of LCleanedupHandle object.
@SYMTestPriority High
@SYMTestActions Creates a LCleanedupHandle<RLogger> on the stack and 
				forces cleanup by calling ReleaseResource().
				Verifies that all memory allocated for the object is freed
				by calling ReleaseResource();
@SYMTestExpectedResults All memory allocated for the LCleanedupHandle<RLogger>
				is freed by calling ReleaseResource().
@SYMREQ	10373-8, 10375-4
*/
void TestLCleanedupHandleRelease()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4036"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupHandle - test LCleanedupHandle::ReleaseResource"));

	LCleanedupHandle<RLogger> logger(42);
	logger.ReleaseResource();

	LCleanedupHandle<RLogger> logger2(2);
	RLogger raw_logger = logger2.Unmanage();
	logger2.ReleaseResource();
	raw_logger.Close();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4037
@SYMTestCaseDesc Tests cleanup of derived objects using LCleanedupHandle<XXX>
@SYMTestPriority High
@SYMTestActions Creates LCleanedupHandle<XXX> objects on the stack by instantiating 
				derived classes.  
				Verifies that all memory allocated for the objects is freed 
				automatically when the objects go out of scope.
@SYMTestExpectedResults All memory allocated for the derived classes is freed
				automatically.
@SYMREQ	10373-8
*/
void TestLCleanedupHandleConversionL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4037"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupHandle - test convertible type support"));

		{
		ROtherLogger olog;
		olog.OpenL(1);
		LCleanedupHandle<RLogger> logger(olog);
		}

		{
		RLogger log2;
		log2.OpenL(2);
		LCleanedupHandle<ROtherLogger> ologger(3);
		ologger = log2;

		ROtherLogger olog4(4);
		ologger = olog4;
		}

	__UHEAP_MARKEND;
	}


void TestLCleanedupHandleL()
	{
	__UHEAP_MARK;

	TestLCleanedupHandleStrategies();
	TestLCleanedupConstructors();
	TestLCleanedupHandleNormalL();
	TestLCleanedupHandleLeave();
	TestLCleanedupHandleUnmanage();
	TestLCleanedupHandleUnmanageLeave();
	TestLCleanedupHandleObjectAccess();
	TestLCleanedupHandleRelease();
	TestLCleanedupHandleConversionL();

	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4038
@SYMTestCaseDesc Tests cleanup of Null pointer LCleanedupPtr object
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupPtr<CTracker> on the stack but doesnt 
				intialise the pointer.
				Verifies that the object is cleaned up when it goes out of scope
@SYMTestExpectedResults Cleanup of a NULL pointer should complete successfully
@SYMREQ	10373-6
*/
void TestLCleanedupPtrNull()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4038"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupPtr - test null pointer"));

		{
		LCleanedupPtr<CTracker> tracker;
		LCleanedupPtr<TInt> tint;
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4039
@SYMTestCaseDesc Tests automatic cleanup of LCleanedupPtr object
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupPtr<CTracker> on the stack and uses the object.
				Verifies that the object is automatically cleaned up when it goes out of scope
@SYMTestExpectedResults All memory allocated for the LCleanedupPtr<CTracker>
				is automatically freed when it goes out of scope.
@SYMREQ	10373-6
*/
void TestLCleanedupPtrNormalL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4039"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupPtr - test normal exit from a block scope"));

	{
	LCleanedupPtr<CTracker> tracker(CTracker::NewL());
	LCleanedupPtr<TInt> tint(new TInt(42));
	}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4040
@SYMTestCaseDesc Tests automatic cleanup of LCleanedupPtr object on a leave
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupPtr<CTracker> on the stack and uses the object.
				Verifies that the object is automatically cleaned up when a leave occurs
@SYMTestExpectedResults All memory allocated for the LCleanedupPtr<CTracker>
				is automatically freed when a leave occurs
@SYMREQ	10373-6
*/
void TestLCleanedupPtrLeave()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4040"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupPtr - test leave"));

	TRAPD(status,
		  {
		  LCleanedupPtr<CTracker> tracker(CTracker::NewL());
		  LCleanedupPtr<CTracker> nulltracker;
		  LCleanedupPtr<TInt> tint(new TInt(42));
		  LCleanedupPtr<TInt> nullint;

		  test.Printf(_L("TestLCleanedupPtrLeave(): Now leaving with User::Leave(KErrGeneral)\n"));
		  User::Leave(KErrGeneral);
		  });
	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4041
@SYMTestCaseDesc Tests manual cleanup of LCleanedupPtr object.
@SYMTestPriority High
@SYMTestActions Creates 2 LCleanedupPtr<CTracker> objects on the stack and 
				manually Unmanages and deletes the CTracker objects
				Verifies that all memory allocated for the objects can be freed
				manually.
@SYMTestExpectedResults All memory allocated for the LCleanedupPtr<CTracker>
				is freed by calling Unmanage() and deleting the objects
@SYMREQ	10373-6, 10375-5
*/
void TestLCleanedupPtrUnmanageL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4041"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupPtr - test LCleanedupPtr::Unmanage"));

	{
	LCleanedupPtr<CTracker> tracker1(CTracker::NewL());
	LCleanedupPtr<CTracker> tracker2(CTracker::NewL());
	delete tracker1.Unmanage();
	delete tracker2.Unmanage();
	}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4042
@SYMTestCaseDesc Tests manual cleanup of LCleanedupPtr object on a leave
@SYMTestPriority High
@SYMTestActions Creates 2 LCleanedupPtr<CTracker> objects on the stack and 
				manually Unmanages them.  
				Forces a leave and then deletes the CTracker objects.
				Verifies that all memory allocated for the objects can be freed
				manually in the event of a leave occuring
@SYMTestExpectedResults All memory allocated for the LCleanedupPtr<CTracker>
				is freed by calling Unmanage() and deleting the objects
@SYMREQ	10373-6, 10375-5
*/
void TestLCleanedupPtrUnmanageLeave()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4042"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupPtr - test LCleanedupPtr::Unmanage and leave"));


	{
	CTracker* ptr1 = NULL;
	CTracker* ptr2 = NULL;
	TRAPD(status2,
		{
		LCleanedupPtr<CTracker> tracker1(CTracker::NewL());
		LCleanedupPtr<CTracker> tracker2(CTracker::NewL());

		ptr1 = tracker1.Unmanage();
		ptr2 = tracker2.Unmanage();

		test.Printf(_L("TestLCleanedupPtrUnmanageLeave(): Now leaving with User::Leave(KErrGeneral)\n"));
		User::Leave(KErrGeneral);
		});

	if (status2 != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status2);
		delete ptr1;
		delete ptr2;
		}
	}


	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4043
@SYMTestCaseDesc Tests access to managed object through LCleanedupPtr
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupPtr<CTracker> on the stack and 
				uses the LCleanedupPtr object to access CTracker methods
				via the -> operator and the LCleanedupPtr methods via 
				the . operator
@SYMTestExpectedResults All public CTracker methods and LCleanedupPtr
				should be accessible through the LCleanedupPtr object.
@SYMREQ	10373-6
*/
void TestLCleanedupPtrObjectAccessL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4043"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupPtr - test managed object access"));

	{
	LCleanedupPtr<CTracker> tracker(CTracker::NewL());
	tracker->MemFunc();
	CTracker::StaticMemberRef(*tracker);
	CTracker::StaticMemberPtr(tracker.Get());
	}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4044
@SYMTestCaseDesc Tests forced cleanup of LCleanedupPtr object.
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupPtr<CTracker>> on the stack and 
				forces cleanup by calling ReleaseResource().
				Verifies that all memory allocated for the object is freed
				by calling ReleaseResource()
@SYMTestExpectedResults All memory allocated for the LCleanedupPtr<CTracker>
				is freed by calling ReleaseResource().
@SYMREQ	10373-6, 10375-4
*/
void TestLCleanedupPtrReleaseL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4044"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupPtr - test LCleanedupPtr::ReleaseResource"));


	{
	LCleanedupPtr<CTracker> tracker(CTracker::NewL());
	tracker.ReleaseResource();
	
	LCleanedupPtr<CTracker> tracker2(CTracker::NewL());
	CTracker* ptracker2 = tracker2.Unmanage();
	tracker2.ReleaseResource();
	delete ptracker2;

	LCleanedupPtr<CTracker> tracker3(CTracker::NewL());
	tracker3 = NULL;
	}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4045
@SYMTestCaseDesc Tests cleanup of LCleanedupPtr object using TPointerFree strategy
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupPtr<TText, TPointerFree> on the stack and 
				uses the object.
				Verifies that all memory allocated for the object is freed by the
				TPointerFree cleanup strategy when the object goes out of scope.
@SYMTestExpectedResults All memory allocated for the LCleanedupPtr<TText, TPointerFree>
				object is freed by the TPointerFree cleanup strategy.
@SYMREQ	10373-6
*/
void TestLCleanedupPtrPointerFree()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4045"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupPtr - test TPointerFree"));

		{
		LCleanedupPtr<TText, TPointerFree> buffer(
		  static_cast<TText*>(User::Alloc(100 * sizeof(TText))));
		TPtr bufferPtr(buffer.Get(), 100);	// create a pointer to the buffer
		// use the buffer
		bufferPtr.Copy(_L("Hello RAII"));
		test.Printf(_L("%S\n"), &bufferPtr);
		}

	__UHEAP_MARKEND;
	}


void TestLCleanedupPtrConversionL()
	{
	LCleanedupPtr<CTracker> tracker(CDerivedTracker::NewL());

	LCleanedupPtr<TReal64, TPointerFree> real(
	  static_cast<TReal64*>(User::Alloc(sizeof(TReal64))));
	
	LCleanedupPtr<CTracker> tracker1(CTracker::NewL());
	LCleanedupPtr<CDerivedTracker> derivedTracker(CDerivedTracker::NewL());
	
	tracker1 = derivedTracker.Unmanage();

	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4046
@SYMTestCaseDesc Tests cleanup of derived objects using LCleanedupPtr<XXX>
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupPtr<XXX> objects on the stack by instantiating 
				derived classes.  
				Verifies that all memory allocated for the objects is freed 
				automatically when the objects go out of scope.
@SYMTestExpectedResults All memory allocated for the derived classes is freed
				automatically.
@SYMREQ	10373-6
*/
void TestLCleanedupPtrConversion()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4046"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupPtr - test convertible type support"));

	TRAPD(status, TestLCleanedupPtrConversionL());

	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4047
@SYMTestCaseDesc Tests assignment of LCleanedupPtr<XXX> objects
@SYMTestPriority High
@SYMTestActions Creates 2 LCleanedupPtr<CTracker> objects on the stack.
				Unamanages one of the objects and assigns the managed pointer 
				to the second LCleanedupPtr
				Verifies that all memory allocated for the objects is freed 
				automatically when the objects go out of scope.
@SYMTestExpectedResults All memory allocated for the objects is freed
				automatically when the objects go out of scope.
@SYMREQ	10373-6
*/
void TestLCleanedupPtrAssignL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4047"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupPtr - test LCleanedupPtr reset"));

	{
	LCleanedupPtr<CTracker> tracker1(CTracker::NewL());
	LCleanedupPtr<CTracker> tracker2(CTracker::NewL());

	tracker2 = tracker1.Unmanage();
	}

	__UHEAP_MARKEND;
	}

void TestLCleanedupPtrBoolConversionL()
	{
	LCleanedupPtr<CTracker> tracker1(CTracker::NewL());

	if (tracker1)
		{
		test.Printf(_L("TestLCleanedupPtrBoolConversion: tracker1 pointer = %x\n"), tracker1.Get());
		}
	else
		{
		test.Printf(_L("TestLCleanedupPtrBoolConversion: tracker1 pointer is null (%x)\n"), tracker1.Get());
		}

	LCleanedupPtr<CTracker> nullPtr;

	if (!nullPtr)
		{
		test.Printf(_L("TestLCleanedupPtrBoolConversion: nullPtr pointer is null (%x)\n"), nullPtr.Get());
		}
	else
		{
		test.Printf(_L("TestLCleanedupPtrBoolConversion: nullPtr pointer = %x\n"), nullPtr.Get());
		}
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4048
@SYMTestCaseDesc Tests conversion of LCleanedupPtr<XXX> objects
@SYMTestPriority High
@SYMTestActions Calls TestLCleanedupPtrBoolConversionL which creates 2 
				LCleanedupPtr<CTracker> objects on the stack, one of which
				is a null pointer.  Tests conversion of the null pointer
				into a CTracker* via the Get() method and ensures all 
				memory is freed on scope exit.
@SYMTestExpectedResults All memory allocated for the objects is freed
				automatically when the objects go out of scope.
@SYMREQ	10373-6
*/
void TestLCleanedupPtrBoolConversion()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4048"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupPtr - test bool conversion"));

	TRAPD(status, TestLCleanedupPtrBoolConversionL());

	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		}

	__UHEAP_MARKEND;
	}


void TestLCleanedupPtrCompareL()
	{
	LCleanedupPtr<CTracker> tracker1(CTracker::NewL());
	LCleanedupPtr<CTracker> tracker2(CTracker::NewL());

	if (tracker1 == tracker2)
		{
		test.Printf(_L("tracker1 == tracker2 %x %x\n"), tracker1.Get(), tracker2.Get());
		}

	if (tracker1 != tracker2)
		{
		test.Printf(_L("tracker1 != tracker2 %x %x\n"), tracker1.Get(), tracker2.Get());
		}


	if (tracker1 < tracker2)
		{
		test.Printf(_L("tracker1 < tracker2 %x %x\n"), tracker1.Get(), tracker2.Get());
		}


	tracker2 = tracker1.Get();

	if (tracker1 == tracker2)
		{
		test.Printf(_L("tracker1 == tracker2 %x %x\n"), tracker1.Get(), tracker2.Get());
		}

	if (tracker1 != tracker2)
		{
		test.Printf(_L("tracker1 != tracker2 %x %x\n"), tracker1.Get(), tracker2.Get());
		}

	if (tracker1 < tracker2)
		{
		test.Printf(_L("tracker1 < tracker2 %x %x\n"), tracker1.Get(), tracker2.Get());
		}

	tracker1.Unmanage();
	}


/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4049
@SYMTestCaseDesc Tests comparison of LCleanedupPtr<XXX> objects
@SYMTestPriority High
@SYMTestActions Calls TestLCleanedupPtrCompareL which creates 2 
				LCleanedupPtr<CTracker> objects on the stack.
				Compares the objects using the comparison operators 
				and ensures all memory is freed on scope exit.
@SYMTestExpectedResults All memory allocated for the objects is freed
				automatically when the objects go out of scope.
@SYMREQ	10373-6
*/
void TestLCleanedupPtrCompare()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4049"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupPtr - test compare"));

	TRAPD(status, TestLCleanedupPtrCompareL());

	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4067
@SYMTestCaseDesc Tests execution of custom cleanup strategy for LCleanedupPtr<XXX> objects
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupPtr<CTracker, TCTrackerDestroy> object 
				on the stack which uses a custom cleanup strategy.
				Verifies that the custom strategy is invoked when the object goes out
				of scope.
@SYMTestExpectedResults All memory allocated for the object is freed automatically by 
						the custom cleanup strategy when the objecs goes out of scope.
@SYMREQ	10373
*/
void TestLCleanedupPtrCustomStrategyL()
	{
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4067"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupPtr - test TCTrackerDestroy strategy"));

		{
		LCleanedupPtr<CTracker, TCTrackerDestroy> t(new(ELeave) CTracker);
		}
		
	test(trackerDestroyed);

	__UHEAP_MARKEND;

	}


void TestLCleanedupPtrL()
	{
	__UHEAP_MARK;

	TestLCleanedupPtrNull();
	TestLCleanedupPtrNormalL();
	TestLCleanedupPtrLeave();
	TestLCleanedupPtrUnmanageL();
	TestLCleanedupPtrUnmanageLeave();
	TestLCleanedupPtrObjectAccessL();
	TestLCleanedupPtrReleaseL();
	TestLCleanedupPtrPointerFree();
	TestLCleanedupPtrConversion();
	TestLCleanedupPtrAssignL();
	TestLCleanedupPtrBoolConversion();
	TestLCleanedupPtrCompare();
	TestLCleanedupPtrCustomStrategyL();
	
	__UHEAP_MARKEND;
	}


class TLogCleanupStrategy
	{
  public:
	template <class T>
	static void Cleanup(T* aObjPtr)
		{
		test.Printf(_L("Cleanup log: %x\n"), aObjPtr);
		}
	};

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4050
@SYMTestCaseDesc Tests automatic cleanup of LManagedXX objects
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupPtr<CComposite> on the stack -
				CComposite internally contains LManagedXX objects
				Verify that all LManagedXX objects are cleaned up when 
				the LCleanedupPtr<CComposite> object goes out of scope.
@SYMTestExpectedResults All memory allocated for the CComposite object
				is automatically freed when it goes out of scope.
@SYMREQ	10374
*/
void TestLManagedNormalL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4050"));
	
	__UHEAP_MARK;
	test.Next(_L("LManaged - test composite object with normal exit from a block scope "));

	{
	LCleanedupPtr<CComposite> comp(CComposite::NewL());
	}
	
	//Check that the custom cleanup strategy for the iAutoPtr member of
	//CComposite has been invoked
	test(trackerDestroyed);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4051
@SYMTestCaseDesc Tests automatic cleanup of LManagedXX objects if 
				a leave occurs in a Constructor
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupPtr<CComposite> object on the stack 
				passing EConstructorLeaves to the CComposite factory function 
				which causes the constructor to leave.
				Verifies that all memory allocated in the constructor is 
				automatically freed if the constructor leaves.	
@SYMTestExpectedResults All memory allocated for the CComposite object
				is automatically freed when it goes out of scope.
@SYMREQ	10368, 10374 
*/
void TestLManagedConstructorLeave()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4051"));
	
	__UHEAP_MARK;
	test.Next(_L("LManaged - test composite object with leave from constructor"));

	TRAPD(status,
		  {
		  LCleanedupPtr<CComposite> comp(CComposite::NewL(CComposite::EConstructorLeaves));
		  });

	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		}
	
	//Check that the custom cleanup strategy for the iAutoPtr member of
	//CComposite has been invoked
	test(trackerDestroyed);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4052
@SYMTestCaseDesc Tests automatic cleanup of LManagedXX objects if 
				a leave occurs in a Constructor
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupPtr<CComposite> object on the stack 
				passing EMemberConstructorLeaves to the CComposite factory function 
				which causes the constructor of the CTracker member to leave.
				The CComposite object contains several LManagedX members which are 
				instantiated in the constructor.
				Verifies that all memory allocated in the constructor is 
				automatically freed if the constructor leaves.	
@SYMTestExpectedResults All memory allocated for the CComposite object
				is automatically freed when it goes out of scope.
@SYMREQ	10368, 10374
*/
void TestLManagedMemberConstructorLeave()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4052"));
	
	__UHEAP_MARK;
	test.Next(_L("LManaged - test composite object with leave from member constructor"));

	TRAPD(status,
		  {
		  LCleanedupPtr<CComposite> comp(CComposite::NewL(CComposite::EMemberConstructorLeaves));
		  });

	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		}
	
	//Check that the custom cleanup strategy for the iAutoPtr member of
	//CComposite has been invoked
	test(trackerDestroyed);

	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4053
@SYMTestCaseDesc Tests realease of LManagedX classes
@SYMTestPriority High
@SYMTestActions Creates 2 LCleanedupPtr<CComposite> objects on the stack.
				Calls ReleaseLoggers() and ReleaseArrays() on the managed objects.
				Verifies that all memory allocated for the objects is freed 
				automatically when the objects go out of scope.
@SYMTestExpectedResults All memory allocated for the objects is freed
				automatically when the objects go out of scope.
@SYMREQ	10374
*/
void TestLManagedReleaseL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4053"));
	
	__UHEAP_MARK;
	test.Next(_L("LManaged - test composite object release"));

	{
	LCleanedupPtr<CComposite> comp1(CComposite::NewL());
	comp1->ReleaseLoggers();

	LCleanedupPtr<CComposite> comp2(CComposite::NewL());
	comp2->ReleaseArrays();
	}
	
	//Check that the custom cleanup strategy for the iAutoPtr member of
	//CComposite has been invoked
	test(trackerDestroyed);

	__UHEAP_MARKEND;
	}


void TestCompositeL()
	{
	__UHEAP_MARK;

	  TestLManagedNormalL();
	  TestLManagedConstructorLeave();
	  TestLManagedMemberConstructorLeave();
	  TestLManagedReleaseL();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4054
@SYMTestCaseDesc Tests automatic cleanup of LCleanedupRef object
@SYMTestPriority High
@SYMTestActions Creates a LCleanedupRef<RLogger> on the stack and uses the object.
				Verifies that the object is automatically cleaned up when it goes out of scope
@SYMTestExpectedResults All memory allocated for the LCleanedupRef<RLogger>
				is automatically freed when it goes out of scope.
@SYMREQ	10373-7
*/
void TestLCleanedupRefNormalL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4054"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupRef - test normal exit from a block scope"));

		{
		RLogger logger;
		logger.OpenL(42);
		LCleanedupRef<RLogger> wlogger(logger);
		wlogger->MemFunc();
		}

	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4055
@SYMTestCaseDesc Tests automatic cleanup of LCleanedupRef object on a leave
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupRef<RLogger> on the stack and uses the object.
				Verifies that the object is automatically cleaned up when a leave occurs
@SYMTestExpectedResults All memory allocated for the LCleanedupRef<RLogger>
				is automatically freed when a leave occurs
@SYMREQ	10373-7
*/
void TestLCleanedupRefLeave()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4055"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupRef - test leave"));

	TRAPD(status,
		  {
		  RLogger logger;
		  logger.OpenL(42);
		  LCleanedupRef<RLogger> wlogger(logger);
		  wlogger->MemFunc();

		  test.Printf(_L("TestLCleanedupRefLeave(): Now leaving with User::Leave(KErrGeneral)\n"));
		  User::Leave(KErrGeneral);
		  });
	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4056
@SYMTestCaseDesc Tests manual cleanup of LCleanedupRef object.
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupRef<RLogger> object on the stack and 
				manually Unmanages and closes the RLogger.
				Verifies that all memory allocated for the object can be freed
				manually.
@SYMTestExpectedResults All memory allocated for the LCleanedupRef<RLogger>
				is freed by calling Unmanage() and Close()
@SYMREQ	10373-7, 10375-5
*/
void TestLCleanedupRefUnmanageL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4056"));
	
	__UHEAP_MARK;

	test.Next(_L("LCleanedupPtr - test LCleanedupPtr::Unmanage"));

	RLogger logger;

	LCleanedupRef<RLogger> rlog(logger);
	rlog->OpenL(42);
	rlog->MemFunc();
	rlog.Unmanage();
	logger.Close();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4057
@SYMTestCaseDesc Tests manual cleanup of LCleanedupRef object on a leave
@SYMTestPriority High
@SYMTestActions Creates LCleanedupRef<RLogger> objects on the stack and 
				manually Unmanages them.  
				Forces a leave and then Closes the RLogger objects.
				Verifies that all memory allocated for the objects can be freed
				manually in the event of a leave occuring
@SYMTestExpectedResults All memory allocated for the LCleanedupRef<RLogger>
				is freed by calling Unmanage() and Close()
@SYMREQ	10373-7, 10375-5
*/
void TestLCleanedupRefUnmanageLeave()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4057"));
	
	__UHEAP_MARK;

	test.Next(_L("LCleanedupRef - test LCleanedupRef::Unmanage"));

	RLogger logger1(1);
	RLogger logger2(2);

	TRAPD(status,
		  {
		  LCleanedupRef<RLogger> rlog1(logger1);
		  LCleanedupRef<RLogger> rlog2(logger2);
		  rlog1->MemFunc();
		  rlog2->MemFunc();
		  rlog1.Unmanage();
		  rlog2.Unmanage();

		  test.Printf(_L("TestLCleanedupRefUnmanageLeave(): Now leaving with User::Leave(KErrGeneral)\n"));
		  User::Leave(KErrGeneral);
		  });

	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		logger1.Close();
		logger2.Close();
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4058
@SYMTestCaseDesc Tests access to managed object through LCleanedupRef
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupRef<RLogger> on the stack and 
				uses the LCleanedupRef object to access RLogger methods
				via the -> operator and the LCleanedupRef methods via 
				the . operator
@SYMTestExpectedResults All public RLogger methods and LCleanedupRef
				should be accessible through the LCleanedupRef object.
@SYMREQ	10373-7
*/
void TestLCleanedupRefObjectAccessL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4058"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupRef - test managed object access"));

		{
		RLogger logger;
		logger.OpenL(42);
		LCleanedupRef<RLogger> rlog(logger);
		rlog->MemFunc();
		RLogger::StaticMemberRef(*rlog);
		RLogger::StaticMemberPtr(&rlog.Get());
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4059
@SYMTestCaseDesc Tests forced cleanup of LCleanedupRef object.
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupRef<RLogger> on the stack and 
				forces cleanup by calling ReleaseResource().
				Verifies that all memory allocated for the object is freed
				by calling ReleaseResource()
@SYMTestExpectedResults All memory allocated for the LCleanedupRef<RLogger>
				is freed by calling ReleaseResource().
@SYMREQ	10373-7, 10375-4
*/
void TestLCleanedupRefReleaseL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4059"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupRef - test LCleanedupRef::ReleaseResource"));

		{
		RLogger logger;
		logger.OpenL(42);

		LCleanedupRef<RLogger> wlogger(logger);
		wlogger->MemFunc();
		wlogger.ReleaseResource();

		RLogger logger2(2);
		LCleanedupRef<RLogger> wlogger2(logger2);
		wlogger2->MemFunc();
		RLogger& logref2 = wlogger2.Unmanage();
		wlogger2.ReleaseResource();
		logref2.Release();
		}

	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4060
@SYMTestCaseDesc Tests assignment of LCleanedupRef<XXX> objects
@SYMTestPriority High
@SYMTestActions Creates 2 LCleanedupRef<CTracker> objects on the stack.
				Unamanages one of the objects and assigns the managed 
				reference to the second LCleanedupPtr
				Verifies that all memory allocated for the objects is freed 
				automatically when the objects go out of scope.
@SYMTestExpectedResults All memory allocated for the objects is freed
				automatically when the objects go out of scope.
@SYMREQ	10373-7
*/
void TestLCleanedupRefAssignL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4060"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupRef - test LCleanedupRef::operator="));

		{
		RLogger logger1;
		logger1.OpenL(1);
		LCleanedupRef<RLogger> wlogger1(logger1);

		RLogger logger2;
		logger2.OpenL(2);
		LCleanedupRef<RLogger> wlogger2(logger2);

		//The assignment results in logger2 being cleaned up before 
		//logger1 is assigned
		wlogger2 = wlogger1.Unmanage();
		}

	__UHEAP_MARKEND;
	}



void TestLCleanedupRefL()
	{
	__UHEAP_MARK;

	TestLCleanedupRefNormalL();
 	TestLCleanedupRefLeave();
 	TestLCleanedupRefUnmanageL();
 	TestLCleanedupRefUnmanageLeave();
 	TestLCleanedupRefObjectAccessL();
 	TestLCleanedupRefReleaseL();
	TestLCleanedupRefAssignL();

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4061
@SYMTestCaseDesc Tests automatic cleanup of LCleanedupGuard object
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupGuard on the stack to clean up an RLogger object
				via the RLogger::Cleanup function.
				Verifies that the object is automatically cleaned up when it goes 
				out of scope
@SYMTestExpectedResults All memory allocated for the RLogger
				is automatically freed when it goes out of scope.
@SYMREQ	10373-10
*/
void TestLCleanedupGuardNormal()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4061"));
	
	__UHEAP_MARK;
	test.Printf(_L("LCleanedupGuard - test normal exit from a block scope\n"));

		{
		RLogger logger(42);
		LCleanedupGuard cleanGuard(RLogger::Cleanup, &logger);
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4062
@SYMTestCaseDesc Tests automatic cleanup of LCleanedupGuard object on a leave
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupGuard on the stack to clean up an RLogger object
				via the RLogger::Cleanup function.
				Verifies that the object is automatically cleaned up when a leave occurs
@SYMTestExpectedResults All memory allocated for the RLogger
				is automatically freed when a leave occurs
@SYMREQ	10373-10
*/
void TestLCleanedupGuardLeave()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4062"));
	
	__UHEAP_MARK;
	test.Printf(_L("LCleanedupGuard - test leave"));

	TRAPD(status,
		  {
		  RLogger logger(42);
		  LCleanedupGuard cleanGuard(RLogger::Cleanup, &logger);

		  test.Printf(_L("TestLCleanedupGuardLeave(): Now leaving with User::Leave(KErrGeneral)\n"));
		  User::Leave(KErrGeneral);
		  });

	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4063
@SYMTestCaseDesc Tests dissmissing of LCleanedupGuard
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupGuard on the stack to clean up an RLogger object
				via the RLogger::Cleanup function.
				Calls LCleanedupGuard::Dismiss to disable the guard and manually calls 
				cleanup function.
				Verifies that the memory allocated for the RLogger object is cleaned up
@SYMTestExpectedResults All memory allocated for the RLogger is freed by calling
				Dismiss and manually calling RLogger::Cleanup()
@SYMREQ	10373-10
*/
void TestLCleanedupGuardDismiss()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4063"));
	
	__UHEAP_MARK;
	test.Printf(_L("LCleanedupGuard - test LCleanedupGuard::Dismiss\n"));


	RLogger logger(42);
	LCleanedupGuard cleanGuard(RLogger::Cleanup, &logger);
	cleanGuard.Dismiss();
	RLogger::Cleanup(&logger);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4064
@SYMTestCaseDesc Tests dismissing of LCleanedupGuard on a leave
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupGuard on the stack to clean up an RLogger object
				via the RLogger::Cleanup function.
				Calls LCleanedupGuard::Dismiss to disable the guard and forces a leave.
				Manually calls the RLogger cleanup function.
				Verifies that the memory allocated for the RLogger object is cleaned up
@SYMTestExpectedResults All memory allocated for the RLogger is freed on a leave by 
			calling Dismiss and manually cleaning up the object.
@SYMREQ	10373-10
*/
void TestLCleanedupGuardDismissLeave()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4064"));
	
	__UHEAP_MARK;
	test.Printf(_L("LCleanedupGuard - test LCleanedupGuard::Dismiss\n"));

	RLogger logger(42);
	TRAPD(status,
		  {
		  LCleanedupGuard cleanGuard(RLogger::Cleanup, &logger);
		  cleanGuard.Dismiss();

		  test.Printf(_L("TestLCleanedupGuardDismissLeave(): Now leaving with User::Leave(KErrGeneral)\n"));
		  User::Leave(KErrGeneral);
		  });

	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		RLogger::Cleanup(&logger);
		}

	__UHEAP_MARKEND;
	}


void TestLCleanedupGuard()
	{
	__UHEAP_MARK;

	TestLCleanedupGuardNormal();
	TestLCleanedupGuardLeave();
	TestLCleanedupGuardDismiss();
	TestLCleanedupGuardDismissLeave();

	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4065
@SYMTestCaseDesc Tests manual cleanup of LCleanedupArray object on a leave
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupArray<TLogger> on the stack and calls Unmanage().
				Forces a leave and then manually cleans up the array.
				Verifies that the objects are automatically cleaned up when they go
				out of scope
@SYMTestExpectedResults All memory allocated for the LCleanedupArray objects
				is automatically freed when they go out of scope.
@SYMREQ	10373-9, 10375-5
*/
void TestLCleanedupArrayUnmanageLeave()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4065"));
	
	__UHEAP_MARK;

	const TInt KNumLoggers = 3;
	TLogger* ploggers = NULL;
	TRAPD(status,
		  {
		  LCleanedupArray<TLogger> loggers(new(ELeave) TLogger[KNumLoggers]);

		  ploggers = loggers.Unmanage();

		  test.Printf(_L("TestLCleanedupArrayUnmanageLeave(): Now leaving with User::Leave(KErrGeneral)\n"));

		  User::Leave(KErrGeneral);
		  });
	if (status != KErrNone)
		{
		test.Printf(_L("Leave trapped; leave code: %d\n"), status);
		delete[] ploggers;
		}

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID SYSLIB-EUSERHL-UT-4066
@SYMTestCaseDesc Tests automatic cleanup of LCleanedupArray object
@SYMTestPriority High
@SYMTestActions Creates an LCleanedupArray<TLogger>and uses the object.
				Creates an LCleanedupArray<TLogger, TLogCleanupStrategy> with a custom
				cleanup strategy and uses the object.
				Creates an LCleanedupArray<TLogger> and forces cleanup
				Creates an LCleanedupArray<TLogger> and calls UNmanage()
				Verifies that the objects are automatically cleaned up when they go
				out of scope
@SYMTestExpectedResults All memory allocated for the LCleanedupArray objects
				is automatically freed when they go out of scope.
@SYMREQ	10373-9, 10375-4, 10375-5
*/
void TestLCleanedupArrayL()
	{
	
	test.Next(_L ("@SYMTestCaseID:SYSLIB-EUSERHL-UT-4066"));
	
	__UHEAP_MARK;
	test.Next(_L("LCleanedupArray - test normal exit from a block scope"));

		{
		const TInt KNumLoggers = 3;
		LCleanedupArray<TLogger> array(new(ELeave) TLogger[KNumLoggers]);
		array[0].Test();

		TLogger rawarr[2];
		LCleanedupArray<TLogger, TLogCleanupStrategy> marr(rawarr);
		marr[0].Test();

		LCleanedupArray<TLogger> array2(new(ELeave) TLogger[KNumLoggers]);
		array2.ReleaseResource();

		LCleanedupArray<TLogger> array3(new(ELeave) TLogger[KNumLoggers]);
		array2 = array3.Unmanage();

		LCleanedupArray<TLogger> nullarr;

		TestLCleanedupArrayUnmanageLeave();
		}

	__UHEAP_MARKEND;
	}


void TestOrLeave()
 	{
	__UHEAP_MARK;

		{		
		TRAPD(err,KErrGeneral OR_LEAVE);
		test(err == KErrGeneral);
  	
		TRAP(err,(5-7) OR_LEAVE);
		test(err == -2);
 	
		RLogger logger;
		logger.OpenL(KErrNoMemory);
 	
		LCleanedupRef<RLogger> cLogger(logger);	
		TRAP(err,{
 			*(cLogger->GetData()) OR_LEAVE;
 			});
		test(err == KErrNoMemory);
		
		LCleanedupHandle<RFs> cFs;
		LCleanedupHandle<RFile> cFile;
 	
		TRAP(err, cFs->Connect() OR_LEAVE);
		test(err == KErrNone);

		_LIT(KTestFile,"c:\\test_emanaged");
		err = cFile->Open(*cFs, KTestFile,EFileRead);
		if (err != KErrNone)
			{
			test.Printf(_L("Error opening file: %d\n"), err);
			if (err == KErrNotFound)
				{
				test.Printf(_L("Creating new file c:\\test_emanaged ... "));
				err = cFile->Create(*cFs,
									KTestFile,
									EFileWrite | EFileShareAny);
				test.Printf(_L("File created\n"));
				}
			}
 	
		test(err == KErrNone);

		LCleanedupHandle<RDir> dir;
		err = dir->Open(*cFs, _L("c:\\resource"), KEntryAttMaskSupported);

		LCleanedupHandle<RFs> aFs(cFs.Unmanage());
		LCleanedupHandle<RFile> aFile(cFile.Unmanage());
		LCleanedupHandle<RDir> adir(dir.Unmanage());

		test(err == KErrNone);
		}
 	
	__UHEAP_MARKEND;
 	}


void TestL()
	{
	__UHEAP_MARK;

	TestLeaveFromConstructor();

	TestLCleanedupHandleL();
	TestLCleanedupPtrL();
	TestLCleanedupArrayL();
	TestLCleanedupRefL();
	TestLCleanedupGuard();
	TestCompositeL();
	TestOrLeave();
	TExtendedTestL();
	__UHEAP_MARKEND;
	}


TInt E32Main()
	{
	CTrapCleanup* stack = CTrapCleanup::New();
	if (stack == NULL)
		return KErrNoMemory;

	test.Title();
	test.Start(_L("RAII-based automatic resource management tests"));

	TRAPD(status, TestL());
	if (status != KErrNone)
		{
		test.Printf(_L("Test leave trapped; leave code: %d\n"), status);
		}
	else
		{
		test.End();
		}

	delete stack;
	return status;
	}


