// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32test.h>
#include <e32math.h>
#include <featmgr.h>
#include <featureuids.h>
#include "featurepanics.h"
#include <featurecontrol.h>
#include <featurenotifier.h>
#include "../src/inc/featmgrconfiguration.h"
#include "../src/inc/featmgrclientserver.h"

using namespace NFeature;

static RTest TheTest(_L("t_fmgrbadclient"));

const TInt KTestIterCount = 5000;

enum TArgType 
    {
    EIntArgType, 
    ETextArgType, 
    EBinArgType, 
    ELastArgType
    };

const TInt KMaxDesArgLen = 1000;

//If the FeatMgr server crashes and the test receives KErrServerTerminated error, then the 
//next set will contain the last:
// - iteration number;
// - handle type;
// - function code;
// - handle;
// - IPC arguments values;
struct TThreadData
    {
    TInt                iIteration;
    TInt                iFunction;
    TArgType            iArgType[KMaxMessageArguments];
    TInt                iIntArg[KMaxMessageArguments];
    TBuf<KMaxDesArgLen> iTextArg[KMaxMessageArguments];
    TBuf8<KMaxDesArgLen> iBinArg[KMaxMessageArguments];
    TInt64              iSeed;
    };

_LIT(KPanicCategory, "SrvTerm");
_LIT(KPanicCategory2, "InvArg");
const TInt KPanicCode = 1111;
const TInt KPanicCode2 = 2222;

///////////////////////////////////////////////////////////////////////////////////////

//Deletes all created test files.
void DestroyTestEnv()
    {
    }

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine, TBool aPrintThreadName = EFalse)
    {
    if(!aValue)
        {
        DestroyTestEnv();
        if(aPrintThreadName)
            {
            RThread th;
            TName name = th.Name();
            RDebug::Print(_L("*** Expression evaluated to false. Thread %S, Line %d\r\n"), &name, aLine);
            }
        else
            {
            TheTest.Printf(_L("*** Expression evaluated to false. Line %d\r\n"), aLine);
            }
        TheTest(EFalse, aLine);
        }
    }
void Check2(TInt aValue, TInt aExpected, TInt aLine, TBool aPrintThreadName = EFalse)
    {
    if(aValue != aExpected)
        {
        DestroyTestEnv();
        if(aPrintThreadName)
            {
            RThread th;
            TName name = th.Name();
            RDebug::Print(_L("*** Thread %S, Line %d Expected error: %d, got: %d\r\n"), &name, aLine, aExpected, aValue);
            }
        else
            {
            RDebug::Print(_L("*** Line %d, Expected error: %d, got: %d\r\n"), aLine, aExpected, aValue);
            }
        TheTest(EFalse, aLine);
        }
    }
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)
#define TTEST(arg) ::Check1((arg), __LINE__, ETrue)
#define TTEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__, ETrue)

//////////////////////////////////////////////////////////////////////////////////////////////////

static TInt StartFeatMgrServer()
    {
    RProcess server;
    const TUidType serverUid( KNullUid, KServerUid2, KNullUid );
    TInt err = server.Create( KServerExeName, // FeatMgrServer.exe
                           KNullDesC, // A descriptor containing data passed as 
                                      // an argument to the thread function of 
                                      // the new process's main thread, when it 
                                      // is first scheduled.
                           serverUid, // FeatMgr server UID
                           EOwnerProcess ); // Ownership of this process handle 

    // Return error code if we couldn't create a process
    if ( err == KErrNone )
        {
        // Rendezvous is used to detect server start
        TRequestStatus stat;
        server.Rendezvous( stat );

        if ( stat != KRequestPending )
            {
            server.Kill( KErrNone ); // Abort startup
            }
        else
            {
            server.Resume();  // Logon OK - start the server
            }

        User::WaitForRequest( stat ); // Wait for start or death

        // We can't use the 'exit reason' if the server paniced as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone
        err = (server.ExitType() == EExitPanic)? KErrGeneral : stat.Int();

        // We can close the handle now
        server.Close();        
        }

    return err;
    }

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////        RTestFeatMgrSession          ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

class RTestFeatMgrSession : public RSessionBase
    {
public: 
    TInt Connect();
    void Close();
    TInt SendReceive(TInt aFunction);
    TInt SendReceive(TInt aFunction, const TIpcArgs& aArgs);
    void SendReceive(TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus);

private:
    TInt DoCreateSession();
    };
    
TInt RTestFeatMgrSession::Connect()
    {
    TInt err = DoCreateSession();
    if(err != KErrNone && err != KErrAlreadyExists)
        {
        Close();
        }
    return err;
    }

void RTestFeatMgrSession::Close()
    {
    RSessionBase::Close();
    }

TInt RTestFeatMgrSession::SendReceive(TInt aFunction)
    {
    return RSessionBase::SendReceive(aFunction);    
    }
    
TInt RTestFeatMgrSession::SendReceive(TInt aFunction, const TIpcArgs& aArgs)
    {
    return RSessionBase::SendReceive(aFunction, aArgs); 
    }

void RTestFeatMgrSession::SendReceive(TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus)
    {
    RSessionBase::SendReceive(aFunction, aArgs, aStatus); 
    }

TInt RTestFeatMgrSession::DoCreateSession()
    {
    const TInt KRetry( 2 );
    // Try this twice
    TInt retry( KRetry );
    TInt err( KErrNone );

    while ( retry > 0 )
        {
        // Try to create a FeatMgr Server session
        err = CreateSession(KServerProcessName, 
                            TVersion(KServerVersionMajor, KServerVersionMinor, KServerVersionBuild), 
                            KDefaultAsyncSlots);

        if ( err != KErrNotFound && err != KErrServerTerminated )
            {
            // KErrNone or unrecoverable error
            retry = 0;
            }
        else
            {
            // Return code was KErrNotFound or KErrServerTerminated.
            // Try to start a new FeatMgr Server
            err = StartFeatMgrServer();

            if ( err != KErrNone && err != KErrAlreadyExists )
                {
                // Unrecoverable error
                retry = 0;
                }
            }
            
        retry--;
        }
            
    return err;
    }
    
void PrintIterationCount(TInt aIteration, TBool aFromThread = EFalse)
    {
    if((aIteration % 100) == 0)
        {
        TTime time;
        time.HomeTime();
        TDateTime dt = time.DateTime();
        TBuf<16> tbuf;
        tbuf.Format(_L("%02d:%02d:%02d.%06d"), dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond());
        if(aFromThread)
            {
            RDebug::Print(_L("-----[%S] Test iterations: %d\r\n"), &tbuf, aIteration);
            }
        else
            {
            TheTest.Printf(_L("-----[%S] Test iterations: %d\r\n"), &tbuf, aIteration);
            }
        }
    }

//Worker thread function.
//It behaves as a malicious client. Connects to the FeatMgr server. In each test iteration generates some random values
//for the function number, handle, IPC arguments. Then sends a command to the server using these
//randomly generated values. If the server crashes and the thread function receives KErrServerTerminated error,
//then the thread kills itself and the main thread will get KPanicCategory and KPanicCode as a reason for the
//worker thread's death. The last set of randomly generated values will be stored in the memory, pointed by aData argument.
TInt ThreadFunc1(void* aData)
    {
    __UHEAP_MARK;
    
    CTrapCleanup* tc = CTrapCleanup::New();
    TTEST(tc != NULL);

    TThreadData* p = static_cast <TThreadData*> (aData);
    TTEST(p != NULL);
    TThreadData& data = *p;

    RTestFeatMgrSession sess;
    TInt err = sess.Connect();
    TTEST2(err, KErrNone);

    while(++data.iIteration <= KTestIterCount)
        {
        PrintIterationCount(data.iIteration, ETrue);
        TIpcArgs args;
        data.iFunction = Math::Rand(data.iSeed) % (EFeatMgrSWIEnd + 1);//EFeatMgrSWIEnd - the last server message number (without resource checking IPCs))
        for(TInt i=0;i<KMaxMessageArguments;++i)
            {
            //Initialize arguments
            data.iArgType[i] = static_cast <TArgType> (Math::Rand(data.iSeed) % ELastArgType);
            switch(data.iArgType[i])
                {
                case EIntArgType:
                    data.iIntArg[i] = Math::Rand(data.iSeed) % 9711;
                    args.Set(i, data.iIntArg[i]);
                    break;
                case ETextArgType:
                    {
                    TInt len = Math::Rand(data.iSeed) % KMaxDesArgLen;  
                    data.iTextArg[i].SetLength(len);
                    args.Set(i, &data.iTextArg[i]);
                    }
                    break;
                case EBinArgType:
                    {
                    TInt len = Math::Rand(data.iSeed) % KMaxDesArgLen;  
                    data.iBinArg[i].SetLength(len);
                    args.Set(i, &data.iBinArg[i]);
                    }
                    break;
                default:
                    User::Panic(KPanicCategory2, KPanicCode2);
                    break;
                }
            }
        //Send arguments
        User::SetJustInTime(EFalse);
        TInt err = KErrNone;
        if(data.iFunction == EFeatMgrReqNotify)
            {
            TRequestStatus status;
            sess.SendReceive(data.iFunction, args, status);
            if(status == KRequestPending)
                {
                err = sess.SendReceive(EFeatMgrReqNotifyCancelAll);
                }
            else
                {
                err = status.Int();
                }
            }
        else
            {
            err = sess.SendReceive(data.iFunction, args);
            }
        if(err == KErrServerTerminated)
            {
            User::Panic(KPanicCategory, KPanicCode);
            }
        User::SetJustInTime(ETrue);
        }

    sess.Close();

    delete tc;  
    
    __UHEAP_MARKEND;
    
    return KErrNone;        
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID          PDS-EFM-CT-4065
@SYMTestCaseDesc        
@SYMTestPriority        High
@SYMTestActions         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void BadClientTest()
    {
    TThreadData* p = new TThreadData;
    TEST(p != NULL);
    TThreadData& data = *p;
    data.iFunction = 0;
    TTime now;
    now.UniversalTime();
    data.iSeed = now.Int64();
    
    _LIT(KThreadName, "WorkThrd");
    
    for(data.iIteration=0;data.iIteration<KTestIterCount;++data.iIteration)
        {
        PrintIterationCount(data.iIteration);
        //Run the malicious client (one worker theread which will try to crash the FeatMgr server)
        RThread thread;
        TEST2(thread.Create(KThreadName, &ThreadFunc1, 0x2000, 0x1000, 0x10000, &data, EOwnerProcess), KErrNone);
        TRequestStatus status;
        thread.Logon(status);
        TEST2(status.Int(), KRequestPending);
        thread.Resume();
        User::WaitForRequest(status);
        User::SetJustInTime(ETrue); // enable debugger panic handling
        if(thread.ExitType() == EExitPanic)
            {
            if(thread.ExitReason() == KPanicCode)
                {
                TheTest.Printf(_L("##Server terminated!\r\n"));
                TheTest.Printf(_L("##Iteration=%d, Function(hex)=%X, Handle=%d\r\n"), data.iIteration, data.iFunction);
                for(TInt i=0;i<KMaxMessageArguments;++i)
                    {
                    switch(data.iArgType[i])
                        {
                        case EIntArgType:
                            TheTest.Printf(_L("##Arg %d, Integer, value=%d\r\n"), i, data.iIntArg[i]);
                            break;
                        case ETextArgType:
                            TheTest.Printf(_L("##Arg %d, Text,    length=%d\r\n"), i, data.iTextArg[i].Length());
                            break;
                        case EBinArgType:
                            TheTest.Printf(_L("##Arg %d, Binary,  length=%d\r\n"), i, data.iBinArg[i].Length());
                            break;
                        default:
                            TheTest.Printf(_L("##Arg %d, Invalid argument type: %d\r\n"), i, data.iArgType[i]);
                            break;
                        }
                    }
                TEST(0);
                }
            }
        thread.Close();
        }
    User::SetJustInTime(ETrue); // enable debugger panic handling
    delete p;
    }

void DoTestsL()
    {
    //This test won't pass
    TheTest.Start(_L("@SYMTestCaseID:PDS-EFM-CT-4065 Bad client test"));
    BadClientTest();
    }

TInt E32Main()
    {
    TheTest.Title();
    
    CTrapCleanup* tc = CTrapCleanup::New();
    TheTest(tc != NULL);
    
    __UHEAP_MARK;
    
    TRAPD(err, DoTestsL());
    DestroyTestEnv();
    TEST2(err, KErrNone);

    __UHEAP_MARKEND;
    
    TheTest.End();
    TheTest.Close();
    
    delete tc;

    User::Heap().Check();
    return KErrNone;
    }
