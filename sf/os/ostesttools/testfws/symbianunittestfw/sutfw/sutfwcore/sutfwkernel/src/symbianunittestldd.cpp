// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <kernel/kern_priv.h>
#include "symbianunittestldd.h"
#include "symbianunittestlddif.h"


const char* KIntsNotEqualFormat = "Asserted: expected=%d, actual=%d";
const char* KDesCsNotEqualFormat = "Asserted: expected='%S', actual='%S'";
const TInt KMaxSizeOfTwoIntsAsText = 80;
const TInt KTestCaseNameLength = 256;

/*
class DCrashHandler : public DKernelEventHandler
    {
public:
    // construction & destruction
    inline DCrashHandler();
    TInt Create(DLogicalDevice* aDevice);
    ~DCrashHandler();

private:
    static TUint EventHandler(TKernelEvent aEvent, TAny* a1, TAny* a2, TAny* aThis);
    void HandleCrash(TAny* aContext);
    
private:    
    DLogicalDevice* iDevice;    // open reference to LDD for avoiding lifetime issues    
    
    };

inline DCrashHandler::DCrashHandler()
    : DKernelEventHandler(EventHandler, this)
    {
    }

//
// second-phase c'tor.  Called in thread critical section.
//

TInt DCrashHandler::Create(DLogicalDevice* aDevice)
    {
//    TInt r;
//    r = aDevice->Open();
//    if (r != KErrNone)
//        return r;
    iDevice = aDevice;
    return Add();
    }

//
// Called when reference count reaches zero.  At that point no threads
// are in the handler anymore and the handler has been removed from
// the queue.
//

DCrashHandler::~DCrashHandler()
    {
//    if (iDevice)
//        iDevice->Close(NULL);
    }

TUint DCrashHandler::EventHandler(TKernelEvent aEvent, TAny* a1, TAny* a2, TAny* aThis)
    {
    DThread* pC = &Kern::CurrentThread();
    switch (aEvent)
        {
    case EEventHwExc:
        ((DCrashHandler*)aThis)->HandleCrash(a1);
        return EExcHandled;
    case EEventKillThread:
        if (pC->iExitType == EExitPanic)
            ((DCrashHandler*)aThis)->HandleCrash(NULL);
        return EExcHandled;
    default:
        // ignore other events
        break;
        }
    return ERunNext;
    }

void DCrashHandler::HandleCrash(TAny* aContext)
    {
    Kern::Printf("CrashHandler handle crash enter");
    Kern::Printf("CrashHandler handle crash exit");
    }

*/

EXPORT_C DSymbianUnitTestFactory::DSymbianUnitTestFactory(const TDesC& aName)
    {
    iDriverName.Copy(aName);
    }

EXPORT_C TInt DSymbianUnitTestFactory::Install()
    {
    iVersion = TVersion(0,0,1);
    return SetName(&iDriverName);
    }

EXPORT_C void DSymbianUnitTestFactory::GetCaps(TDes8& /*aDes*/) const
    {
    //not supported
    }

EXPORT_C DSymbianUnitTestFactory::~DSymbianUnitTestFactory()
    {
    }

EXPORT_C DSymbianUnitTest::DSymbianUnitTest()
    {
    //get the handler for client thread
    iClient = &Kern::CurrentThread();
    ((DObject*)iClient)->Open();

//    iCrashHandler = new DCrashHandler();
//    iCrashHandler->Create(iDevice);
    }

EXPORT_C TInt DSymbianUnitTest::Setup()
    {
    //do nothing by default
    return KErrNone;
    }

EXPORT_C TInt DSymbianUnitTest::Teardown()
    {
    //do nothing by default
    return KErrNone;
    }
EXPORT_C DSymbianUnitTest::~DSymbianUnitTest()
    {
    //delete iName;
    iTestCases.ResetAndDestroy();
    
    //close the client thread handler
    Kern::SafeClose((DObject*&)iClient,NULL);
    
//    if (iCrashHandler)
//        {
//        iCrashHandler->Close();
//        delete iCrashHandler;
//        }
    }

/*
EXPORT_C void DSymbianUnitTest::Construct()
    {
    //do nothing by default
    }
*/

EXPORT_C void DSymbianUnitTest::AddTestCase( 
    const TDesC& aName,
    FunctionPtr aSetupFunction,
    FunctionPtr aTestFunction,
    FunctionPtr aTeardownFunction )
    {
    DSymbianUnitTestCase* testCase = new DSymbianUnitTestCase( 
            aName, aSetupFunction, aTestFunction, aTeardownFunction );
    iTestCases.Append( testCase );
    }


EXPORT_C  TInt DSymbianUnitTest::Request(TInt aReqNo, TAny* a1, TAny* a2)
	{
	switch(aReqNo)
		{
		//new
		case RSymbianUnitTest::EEXECUTETESTCASE:
			{
			return ExecuteTestCase(a1, (TDes8*)a2);
			}
		case RSymbianUnitTest::ETESTCOUNT:
		    return TestCount((TInt*) a1);
		case RSymbianUnitTest::ETESTCASENAME:
		    return GetTestCaseName(a1, (TDes*)a2);
		default:
			break;
		}
	return KErrGeneral;
	}


TInt DSymbianUnitTest::TestCount(TInt* aDest)
    {
    __KTRACE_OPT(KHARDWARE,
        Kern::Printf("DSymbianUnitTest::TestCount() enter"));
    TInt count = iTestCases.Count();
    __KTRACE_OPT(KHARDWARE, 
        Kern::Printf("  kernel space, test case count [%d]", count));
    TInt ret = Kern::ThreadRawWrite(iClient, aDest, &count, sizeof(TInt), NULL);
    __KTRACE_OPT(KHARDWARE, 
        Kern::Printf("DSymbianUnitTest::TestCount() exit"));
    return ret;
    }

TInt DSymbianUnitTest::GetTestCaseName(TAny* aIndex, TDes* aCaseName)
    {
    __KTRACE_OPT(KHARDWARE,
            Kern::Printf("DSymbianUnitTest::GetTestCaseName() enter"));
        TInt index;
        Kern::ThreadRawRead(iClient, aIndex, &index, sizeof(TInt));
        DSymbianUnitTestCase* testCase = iTestCases[index];
        TBuf8<KTestCaseNameLength> buf8;
        buf8.Copy(testCase->Name());
        TInt ret = Kern::ThreadDesWrite(iClient, aCaseName, buf8, 0, &Kern::CurrentThread());
        return ret;
    }


TInt DSymbianUnitTest::ExecuteTestCase(TAny* aIndex, TDes8* aResult)
    {
    __KTRACE_OPT(KHARDWARE,
        Kern::Printf("DSymbianUnitTest::ExecuteTestCase() enter"));
    TInt index;
    Kern::ThreadRawRead(iClient, aIndex, &index, sizeof(TInt));
    DSymbianUnitTestCase* testCase = iTestCases[index];
    
    TSUTTestCaseResult result;
    TPckg<TSUTTestCaseResult> resultPckg(result);
    iCurrentTestCase = testCase;
    iCurrentResult = &result;
    
    result.iTestName.Copy(iCurrentTestCase->Name());
    __KTRACE_OPT(KHARDWARE,
        Kern::Printf("DSymbianUnitTest::Run test case: %S",
                &iCurrentTestCase->Name()));
    
    //set the owning thread for sem
    iExecSem.iOwningThread = &Kern::CurrentThread().iNThread;
    
    __KTRACE_OPT(KHARDWARE,
        Kern::Printf("create exec thread"));
    SThreadCreateInfo info;
    info.iType=EThreadSupervisor;
    info.iFunction=(TThreadFunction)DSymbianUnitTest::TestThreadEntryFunction;
    info.iPtr=this;
    info.iSupervisorStack=NULL;
    info.iSupervisorStackSize=0;    // zero means use default value
    info.iInitialThreadPriority=NKern::CurrentThread()->iPriority;
    
    info.iName.Set(iCurrentTestCase->Name());
    info.iTotalSize = sizeof(info);
    
    
    NKern::ThreadEnterCS();
    TInt ret = Kern::ThreadCreate(info);
    NKern::ThreadLeaveCS();
    
    if (ret == KErrNone)
        {
    
        DThread* pT=(DThread*)info.iHandle;
        __KTRACE_OPT(KHARDWARE, 
            Kern::Printf("start exec thread %0 at %08x", pT, pT));
        Kern::ThreadResume(*pT);
        __KTRACE_OPT(KHARDWARE,
            Kern::Printf("main thread starts to wait on NFastSemaphore"));
        NKern::FSWait(&(iExecSem));
        }
    else
        {
        __KTRACE_OPT(KHARDWARE,
            Kern::Printf("error from TheadCreate: %d", ret));
        //ret the result
        result.iRetCode = ret;
        result.iFailureMessage.Copy(_L("error to create kernel thread to execute test case"));
        }
    
    __KTRACE_OPT(KHARDWARE,
        Kern::Printf("DSymbianUnitTest::Run test case [%S] return: %d",
            &result.iTestName , result.iRetCode));
    __KTRACE_OPT(KHARDWARE,
        Kern::Printf("kernel msg:%S line:%d file:%S",
                        &result.iFailureMessage,
                        result.iLineNumber,
                        &result.iFileName));
    return  Kern::ThreadDesWrite(iClient, aResult, resultPckg, 0, 0, &Kern::CurrentThread());
    }

TInt DSymbianUnitTest::ExecuteTestCaseInThread()
    {
    TInt ret;
    ret = (this->*iCurrentTestCase->iSetupFunction)();
    if (ret != KErrNone)
        {
        return ret;
        }
    ret = (this->*iCurrentTestCase->iTestFunction)();
    if (ret != KErrNone)
        {
        return ret;
        }
    ret = (this->*iCurrentTestCase->iTeardownFunction)();
    if (ret != KErrNone)
        {
        return ret;
        }
    return KErrNone;
    }

TInt DSymbianUnitTest::TestThreadEntryFunction( TAny* aPtr )
    {
    __KTRACE_OPT(KHARDWARE,
        Kern::Printf("enter TestThreadEntryFunction enter"));
    DSymbianUnitTest* self = reinterpret_cast< DSymbianUnitTest* >( aPtr );
    TInt ret = self->ExecuteTestCaseInThread();
    __KTRACE_OPT(KHARDWARE,
        Kern::Printf("exec thread FSSignal on Semaphore"));
    NKern::FSSignal(&(self->iExecSem));
    return KErrNone;
    }


EXPORT_C TInt DSymbianUnitTest::AssertEquals(
    TInt aExpectedValue, 
    TInt aActualValue, 
    TInt aLineNumber,
    const TDesC8& aFileName )
    {
    if ( aExpectedValue != aActualValue )
        {
        TBuf8<sizeof(KIntsNotEqualFormat) + KMaxSizeOfTwoIntsAsText> msg;
        Printf(msg, KIntsNotEqualFormat, aExpectedValue, aActualValue);
        return AssertionFailed( msg, aLineNumber, aFileName );
        }
    return KErrNone;
    }


EXPORT_C TInt DSymbianUnitTest::AssertEquals(
        const TDesC8& aExpectedValue,
        const TDesC8& aActualValue, 
        TInt aLineNumber, 
        const TDesC8& aFileName )
    {
    if ( aExpectedValue.Compare( aActualValue ) != 0 )
            {
            TBuf8<sizeof(KDesCsNotEqualFormat) + KMaxSizeOfTwoIntsAsText> msg;
            Printf(msg, KDesCsNotEqualFormat, &aExpectedValue, &aActualValue);
            return AssertionFailed( msg, aLineNumber, aFileName );
            }
        return KErrNone;
    }


EXPORT_C TInt DSymbianUnitTest::AssertionFailed(
    const TDesC8& aFailureMessage,
    TInt aLineNumber,
    const TDesC8& aFileName )
    {
    
    __KTRACE_OPT(KHARDWARE,
        Kern::Printf("Assertion failed [%S] at file:%S , line number:%d", 
        &aFailureMessage, &aFileName, aLineNumber));
    iCurrentResult->iLineNumber = aLineNumber;
    iCurrentResult->iFailureMessage.Copy(aFailureMessage);
    iCurrentResult->iFileName.Copy(aFileName);
    
    __KTRACE_OPT(KHARDWARE,
        Kern::Printf("kernel msg:%S line:%d file:%S", 
            &iCurrentResult->iFailureMessage,
            iCurrentResult->iLineNumber,
            &iCurrentResult->iFileName));
    iCurrentResult->iRetCode = KErrGeneral; 
    return KErrGeneral;
    }

void DSymbianUnitTest::Printf(TDes8 & aDes, const char* aFmt, ...)
    {
    VA_LIST list;
    VA_START(list, aFmt);
    Kern::AppendFormat(aDes, aFmt, list);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
DSymbianUnitTest::DSymbianUnitTestCase::DSymbianUnitTestCase(
    const TDesC& aName,
    FunctionPtr aSetupFunction,
    FunctionPtr aTestFunction,
    FunctionPtr aTeardownFunction ) :
    iSetupFunction( aSetupFunction ),
    iTestFunction( aTestFunction ),
    iTeardownFunction( aTeardownFunction )
    {
    iName = HBuf8::New(aName);
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
DSymbianUnitTest::DSymbianUnitTestCase::~DSymbianUnitTestCase()
    {
    delete iName;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC& DSymbianUnitTest::DSymbianUnitTestCase::Name() const
    {
    return *iName;
    }

