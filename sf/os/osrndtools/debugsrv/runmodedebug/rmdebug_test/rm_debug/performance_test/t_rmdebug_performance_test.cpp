// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests performance of run mode debug device component   
//

#include <e32base.h>
#include <e32property.h>
#include <e32test.h>
#include <e32def.h>
#include <e32const.h>
#include <hal.h>

#include "t_rmdebug_performance_test.h"
#include "t_debug_logging.h"
#include "t_rmdebug_app.h"

const TVersion securityServerVersion(0,1,1);

_LIT(KTestName, "T_RMDEBUG_PERFORMANCE_TEST");

LOCAL_D RTest test(KTestName);

using namespace Debug;

CRunModeAgent* CRunModeAgent::NewL()
    {
    LOG_ENTRY();
    CRunModeAgent* self = new(ELeave) CRunModeAgent();
    self->ConstructL();
    LOG_EXIT();
    return self;
    }

void CRunModeAgent::ConstructL()
    {
    // ConstructL list does not match destruction list as R-Class member variables are implicitly open.
    // DebugDriver().Connect() is conditionally set depending on the test case hence not part of this function.
    LOG_ENTRY();
    User::LeaveIfError(iIntegerProperty.Attach(RProcess().SecureId(), EPropertyTimeOfCrash, EOwnerThread));
    LOG_EXIT(); 
    }

CRunModeAgent::~CRunModeAgent()
    {
    LOG_ENTRY();   
    DebugDriver().Close();
    iTimeDifs.Close();
    iIntegerProperty.Close();
    RProperty::Delete(EPropertyTimeOfCrash);
    LOG_EXIT(); 
    }


void CRunModeAgent::ClientAppL()
    {
    LOG_ENTRY();   
   
    if ( ParseCommandLine() == EDisplayHelp )
        {
        DisplayUsage();
        }
    else
        {
        static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
        RProperty::Define(RProcess().SecureId(), EPropertyTimeOfCrash, RProperty::EInt, KAllowAllPolicy, KAllowAllPolicy);        
        StartTest();
        }
        
    LOG_EXIT(); 
    }

TInt CRunModeAgent::GetTimeInMs()
{
    return User::NTickCount() * iTickPeriodMs;
}

void CRunModeAgent::GetStartTime()
    {    
    LOG_ENTRY();

    test ( KErrNone == iIntegerProperty.Get(iParams.iCountStart) );    
    LOG_MSG2("iParams.iCountStart %d", iParams.iCountStart);
    
    LOG_EXIT();
    }

void CRunModeAgent::GetNanoTickPeriod()
    {
    LOG_ENTRY();
    
    TInt period = 0;
    User::LeaveIfError(HAL::Get(HALData::ENanoTickPeriod, period));
    iTickPeriodMs = period / 1000;
    LOG_MSG("iTickPeriodMs = %d\n", iTickPeriodMs);
    
    LOG_EXIT();
    }

void CRunModeAgent::StartTest()
    {
    LOG_ENTRY();
    
    GetNanoTickPeriod();
    
    if (iParams.iTestType == PerformanceTestParams::EBenchMarkTest)
        {
        RunBenchMarkTest();
        }
    else 
        {
        TestDriverPerformance();
        }
    
    CalculatePerformance();
    
    LOG_EXIT();
    }

// This function allows us to provide a benchmark when comparing the performance with the 
// old and new APIs using the TestDriverPerformance function below. The two possible 
// configurations are as follows: 
// 1. Run t_rmdebug_app with a prefetch abort configuration and measure the time from the 
// point of crash to the logon request completion.
// 2. Load the debug-system and then run the above configuration.  
void CRunModeAgent::RunBenchMarkTest()
    {
    LOG_ENTRY();
    
    test.Start(_L("RunBenchMarkTest"));
    
    RProcess process;
    TRequestStatus status; 
    
    if (iParams.iDriver)
         {
         RDebug::Printf("RunBenchMarkTest() - DebugDriver().Connect()");
         test(KErrNone == DebugDriver().Connect(securityServerVersion));
         }
 
    LOG_MSG("iParams.iNumOfTestRuns = %d", iParams.iNumOfTestRuns);
    
    for ( TUint i = 0; i < iParams.iNumOfTestRuns; i++ )
        {
        // Start test application
        test( KErrNone == LaunchProcessL(process, KRMDebugTestApplication(), KTargetOptions()) );
               
        process.Logon(status);
        User::WaitForRequest(status);

        // Stop timer on logon request completion
        iParams.iCountEnd = GetTimeInMs();
        LOG_MSG("iParams.iCountEnd = %d", iParams.iCountEnd);
        
        LOG_MSG( "status.Int() = %d " , status.Int() );
        
        // prefetch abort should raise a KERN-EXEC 3
        test(3 == status.Int());
        
        process.Close();
        GetStartTime(); 
       
        // NTickCount shouldn't overflow, so no reason why this assertion should fail
        test(iParams.iCountEnd > iParams.iCountStart);
        iTimeDifs.Append( iParams.iCountEnd - iParams.iCountStart );
       }
    
    LOG_EXIT();
    }

// This function can be used to compare the performance with the old (e.g. attachExe) and new 
// (e.g. attachAll) APIs depending depending on the parameters passed in when running this test.  
void CRunModeAgent::TestDriverPerformance()
    {
    LOG_ENTRY();    
    test.Start(_L("TestDriverPerformance"));
   
    RProcess process;
    
    test(KErrNone == DebugDriver().Connect(securityServerVersion));
    
    LOG_MSG("iParams.iNumOfTestRuns = %d", iParams.iNumOfTestRuns);
    for ( TUint i = 0; i < iParams.iNumOfTestRuns; i++ )
        {
        ilaunchCompleted = EFalse;
     
        Attach();
        SetEventAction();
        test(KErrNone == LaunchProcessL(process, KRMDebugTestApplication(), KTargetOptions));
        
        LOG_MSG("CRunModeAgent::TestDriverPerformance - process.Logon");
        
        while ( !ilaunchCompleted )
            {
            LOG_MSG("CRunModeAgent::TestDriverPerformance - DebugDriver().GetEvent");
       
            GetEvent();
       
            LOG_MSG("CRunModeAgent::TestDriverPerformance - User::WaitForRequest");

            User::WaitForRequest(iStatus);
            LOG_MSG( "iStatus.Int() = %d " , iStatus.Int() );
            
            LOG_MSG("CRunModeAgent::TestDriverPerformance - HandleEvent");
            HandleEvent(iSEventInfo.iEventInfo);
            }
        
        process.Logon(iStatus);
        LOG_MSG("CRunModeAgent::TestDriverPerformance - process.Logon, User::WaitForRequest");
        User::WaitForRequest(iStatus);
        LOG_MSG( "iStatus.Int() = %d " , iStatus.Int() );
        
        // Stop timer on logon request completion as in benchmark performance test
        iParams.iCountEnd = GetTimeInMs();
                
        // prefetch abort should raise a KERN-EXEC 3
        test(3 == iStatus.Int());
                    
        Detach();
        process.Close();       
        GetStartTime();
 
        // NTickCount shouldn't overflow, so no reason why this assertion should fail
        test(iParams.iCountEnd > iParams.iCountStart);
        iTimeDifs.Append( iParams.iCountEnd - iParams.iCountStart );
       }
        
       LOG_EXIT();
    }

void CRunModeAgent::CalculatePerformance()
    {
    LOG_ENTRY();

    TUint median;
    TUint arrayCount = iTimeDifs.Count();

    for (TInt i = 0; i < arrayCount; i++)
         {
         RDebug::Printf("iTimeDifs[%d] = %d ",i,iTimeDifs[i]);
         }
    
    // Sort in ascending order
    iTimeDifs.Sort();
            
    //If the number of elements is odd, the middle element in the sorted array is the median. 
    //If the number of elements is even, the median is the average of the two midmost elements.
    if ( arrayCount%2  != 0 )
        {
        median = iTimeDifs[arrayCount/2];
        }
    else
        {
        median = (iTimeDifs[arrayCount/2] + iTimeDifs[arrayCount/2 -1])/2;
        }
    
    RDebug::Printf("Median time %d ms", median );
    
    LOG_EXIT();
    }

/**
  Launch a process
  @param aExeName the executable used to create the process
  @param aCommandLine the commandline parameters passed to the new process file name of the executable used to create the process
  @return KErrNone on success, or one of the other system wide error codes
  */
TInt CRunModeAgent::LaunchProcessL( RProcess& aProcess, const TDesC& aExeName, const TDesC& aCommandLine )
    {
    LOG_ENTRY(); 
    
    RBuf launcherOptions;
    launcherOptions.CleanupClosePushL();
    const TInt additionalWords = 1; 
    launcherOptions.CreateL( aCommandLine.Length() + additionalWords );
    launcherOptions.Format( aCommandLine, iParams.iTestTargetPriority);
   
    LOG_DES(_L("launcherOptions %S"), &launcherOptions);
    
    TInt err = aProcess.Create( aExeName, launcherOptions );   
    CleanupStack::PopAndDestroy();
    
    // check that there was no error raised
    if (err != KErrNone)
        return err;
    
    // rendezvous with process
    TRequestStatus status = KRequestPending;
    aProcess.Rendezvous(status);

    // start the test target
    aProcess.Resume();
    User::WaitForRequest(status);
  
    if(KErrNone != status.Int())
        {
        aProcess.Kill(KErrNone);
        }
     LOG_EXIT(); 
     return status.Int();

    }

void CRunModeAgent::SetEventAction()
    {
    LOG_ENTRY();
    
    if (iParams.iTestType == PerformanceTestParams::EAttachExe)
        {
        test(KErrNone == DebugDriver().SetEventAction( KRMDebugTestApplication(), EEventsKillThread, EActionContinue));
             
        if ( iParams.iEvents )
            {
            test(KErrNone == DebugDriver().SetEventAction( KRMDebugTestApplication(), EEventsAddLibrary, EActionContinue));
            test(KErrNone == DebugDriver().SetEventAction( KRMDebugTestApplication(), EEventsUserTrace, EActionContinue));
            test(KErrNone == DebugDriver().SetEventAction( KRMDebugTestApplication(), EEventsStartThread, EActionContinue));
            test(KErrNone == DebugDriver().SetEventAction( KRMDebugTestApplication(), EEventsAddProcess, EActionContinue));
            test(KErrNone == DebugDriver().SetEventAction( KRMDebugTestApplication(), EEventsRemoveProcess, EActionContinue));
            }
        }
    else
        {
        test(KErrNone == DebugDriver().SetEventAction( EEventsKillThread, EActionContinue));
             
        if ( iParams.iEvents )
            {
            test(KErrNone == DebugDriver().SetEventAction( EEventsAddLibrary, EActionContinue));
            test(KErrNone == DebugDriver().SetEventAction( EEventsUserTrace, EActionContinue));
            test(KErrNone == DebugDriver().SetEventAction( EEventsStartThread, EActionContinue));
            test(KErrNone == DebugDriver().SetEventAction( EEventsAddProcess, EActionContinue));
            test(KErrNone == DebugDriver().SetEventAction( EEventsRemoveProcess, EActionContinue));
            }
        }
    
    LOG_EXIT();
    }

void CRunModeAgent::Attach()
    {
    LOG_ENTRY();
    
    if( iParams.iTestType == PerformanceTestParams::EAttachExe ) 
        {
        // Attach to process non-passively
        test(KErrNone == DebugDriver().AttachExecutable( KRMDebugTestApplication(), EFalse));
        LOG_MSG("DebugDriver().AttachExecutable");
        }
    else 
        {
        // Attach to all the processes on the system
        test(KErrNone == DebugDriver().AttachAll());
        LOG_MSG("DebugDriver().AttachAll()");
        }
    
    LOG_EXIT();
    }

void CRunModeAgent::GetEvent()
    {
    LOG_ENTRY();

    if( iParams.iTestType == PerformanceTestParams::EAttachExe ) 
        {
        DebugDriver().GetEvent( KRMDebugTestApplication(), iStatus, iSEventInfo.iEventInfoBuf );
        }
    else
        {
        DebugDriver().GetEvent( iStatus, iSEventInfo.iEventInfoBuf );
        }
    
    LOG_EXIT();
    }

void CRunModeAgent::Detach()
    {
    LOG_ENTRY();
    
    if( iParams.iTestType == PerformanceTestParams::EAttachExe )
        {
        test (KErrNone == DebugDriver().DetachExecutable(KRMDebugTestApplication()));
        }
    else
        {
        test(KErrNone == DebugDriver().DetachAll());
        }
    
    LOG_EXIT();
    }

void CRunModeAgent::HandleEvent(TEventInfo& aEventInfo)
    {
    LOG_ENTRY(); 

    switch ( aEventInfo.iEventType )
        {
        case EEventsAddProcess:
            {
            LOG_MSG(">>> EEventsAddProcess");                        
            break;
            }
    
        case EEventsStartThread:
            {
            LOG_MSG(">>> EEventsStartThread");                
            break;                    
            }                       

        case EEventsUserTrace:
            {
            LOG_MSG(">>> EEventsUserTrace");  
            break;
            }

        case EEventsRemoveProcess:
            {
            LOG_MSG(">>> EEventsRemoveProcess");                        
            break;
            }
    
        case EEventsKillThread:
            {
            LOG_MSG(">>> EEventsKillThread");   
            ilaunchCompleted = ETrue;          
            break;
            }
            
        default:   
            {
            LOG_MSG( ">>> Unknown event ");
            break;
            }   
        }
     
    LOG_EXIT(); 
    }

void CRunModeAgent::SetDefaultParamValues()
    {
    LOG_ENTRY();
    
    iParams.iNumOfTestRuns = KNumOfTestRuns;
    iParams.iTestType = PerformanceTestParams::EBenchMarkTest;
    iParams.iTestTargetPriority = 0;
    iParams.iEvents = 0;
    iParams.iDriver = 0;
    
    LOG_EXIT();
    }

TInt CRunModeAgent::ParseCommandLine()
    {
    LOG_ENTRY();
    
    TBool ifDisplayHelp = EDontDisplayHelp;
    SetDefaultParamValues();
    
    TInt argc = User::CommandLineLength();
    LOG_MSG( "Launcher Process() argc=%d", argc );

    if( argc )
        {
        HBufC* commandLine = NULL;
        commandLine = HBufC::NewLC(argc);
        TPtr commandLineBuffer = commandLine->Des();
        User::CommandLine(commandLineBuffer);
       
        LOG_DES(_L("CommandLine = %S"), &commandLineBuffer);
        
        // create a lexer and read through the command line
        TLex lex(*commandLine);
        while (!lex.Eos())
             {
             // only look for options with first character '-', other switches are for the targets
             if (lex.Get() == '-')
                 {
                 TChar arg = lex.Get();
                 switch (arg)
                     {
                     case 'n':
                         lex.Val( iParams.iNumOfTestRuns );
                         LOG_MSG("Number of test runs %d", iParams.iNumOfTestRuns);
                         break;
                      case 't':
                          lex.Val( iParams.iTestType );
                          LOG_MSG("parsed testType as %d", iParams.iTestType );
                          break;
                      case 'p':
                          lex.Val( iParams.iTestTargetPriority );
                          LOG_MSG("parsed test target priority as %d", iParams.iTestTargetPriority );
                          break;
                      case 'e':
                          lex.Val( iParams.iEvents );
                          LOG_MSG("parsed events as %d", iParams.iEvents );
                          break;
                      case 'd':
                          lex.Val( iParams.iDriver );
                          LOG_MSG("parsed iDriver as %d", iParams.iDriver );
                          break;
                       case 'h':
                          LOG_MSG( "Display help" );
                          ifDisplayHelp = EDisplayHelp;
                       default:
                           LOG_MSG( "Default usage" );
                           break;             
                       }
                  }
              }
            CleanupStack::PopAndDestroy(commandLine);
        }
    
    LOG_EXIT();
    return ifDisplayHelp;   
    }

void CRunModeAgent::DisplayUsage()
    {
    LOG_ENTRY();
    test.Printf(_L("\nUsage: t_rmdebug_performance_test [options] \nOptions:\n"));
    
    test.Printf(_L("\t-t  \t\ttest type\n"));
    test.Printf(_L("\t\t\t  0 - AttachAll\n"));
    test.Printf(_L("\t\t\t  1 - AttachExe\n"));
    test.Printf(_L("\t\t\t  2 - None\n"));
           
    test.Printf(_L("\t-n \t\tnumber of iterations\n"));
    test.Printf(_L("\t-e \t\ttest with events\n"));
    test.Printf(_L("\t\t\t  0 - No\n"));
    test.Printf(_L("\t\t\t  1 - Yes\n"));
    test.Printf(_L("\t-p \t\tpriority of test target thread\n"));
    
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteVeryLow \n"), EPriorityAbsoluteVeryLow);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteLowNormal \n"), EPriorityAbsoluteLowNormal);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteLow \n"), EPriorityAbsoluteLow);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteBackgroundNormal \n"), EPriorityAbsoluteBackgroundNormal);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteBackground \n"), EPriorityAbsoluteBackground);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteForegroundNormal \n"), EPriorityAbsoluteForegroundNormal);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteForeground \n"), EPriorityAbsoluteForeground);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteHighNormal \n"), EPriorityAbsoluteHighNormal);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteHigh \n"), EPriorityAbsoluteHigh);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteRealTime1 \n"), EPriorityAbsoluteRealTime1);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteRealTime2 \n"), EPriorityAbsoluteRealTime2);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteRealTime3 \n"), EPriorityAbsoluteRealTime3);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteRealTime4 \n"), EPriorityAbsoluteRealTime4);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteRealTime5 \n"), EPriorityAbsoluteRealTime5);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteRealTime6 \n"), EPriorityAbsoluteRealTime6);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteRealTime7 \n"), EPriorityAbsoluteRealTime7);
    test.Printf(_L("\t\t\t  %d - EPriorityAbsoluteRealTime8 \n"), EPriorityAbsoluteRealTime8);
    
    test.Printf(_L("\t-d  \t\tload driver\n"));
    test.Printf(_L("\t\t\t  0 - No\n"));
    test.Printf(_L("\t\t\t  1 - Yes\n"));
        
    test.Printf(_L("\t-h \t\tdisplay usage information\n\n"));
    
    test.Printf(_L("Press any key...\n"));
    test.Getch();
    
    LOG_EXIT();
    }

GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;

    TInt ret = KErrNone;        
    CTrapCleanup* trap = CTrapCleanup::New();
        
    if (!trap)
        return KErrNoMemory;
    
    test.Start(KTestName);
       
    CRunModeAgent *runModeAgent = CRunModeAgent::NewL();

    if (runModeAgent != NULL)
        {
        TRAP(ret,runModeAgent->ClientAppL());
        LOG_MSG( "ClientAppL() returned %d", ret );
        delete runModeAgent;
        }
    
    test.End();
    test.Close();

    delete trap;
    __UHEAP_MARKEND;
    return ret;
    }

