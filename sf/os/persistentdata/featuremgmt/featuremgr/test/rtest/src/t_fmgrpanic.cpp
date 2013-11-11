// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <featmgr.h>
#include <featureuids.h>
#include "featurepanics.h"
#include <featurecontrol.h>
#include <featurenotifier.h>

using namespace NFeature;

static RTest TheTest(_L("t_fmgrpanic"));

_LIT(KPanicCategory, "RFeatureControl");

///////////////////////////////////////////////////////////////////////////////////////

//Deletes all created test files.
void DestroyTestEnv()
    {
    }

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check1(TInt aValue, TInt aLine)
    {
    if(!aValue)
        {
        DestroyTestEnv();
        RDebug::Print(_L("*** Expression evaluated to false. Line %d\r\n"), aLine);
        TheTest(EFalse, aLine);
        }
    }
void Check2(TInt aValue, TInt aExpected, TInt aLine)
    {
    if(aValue != aExpected)
        {
        DestroyTestEnv();
        RDebug::Print(_L("*** Expected: %d, got: %d. Line %d\r\n"), aExpected, aValue, aLine);
        TheTest(EFalse, aLine);
        }
    }
#define TEST(arg) ::Check1((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

//Panic thread function. 
//It will cast aData parameter to a TFunctor pointer and call it.
//The expectation is that the called function will panic and kill the panic thread.
TInt ThreadFunc(void* aData)
    {
    CTrapCleanup* tc = CTrapCleanup::New();
    TEST(tc != NULL);
    
    User::SetJustInTime(EFalse);    // disable debugger panic handling
    
    TFunctor* obj = reinterpret_cast<TFunctor*> (aData);
    TEST(obj != NULL);
    (*obj)();//call the panic function
    
    delete tc;
    
    return KErrNone;        
    }

//Panic test.
//PanicTest function will create a new thread - panic thread, giving it a pointer to the function which has to
//be executed and the expectation is that the function will panic and kill the panic thread.
//PanicTest function will check the panic thread exit code, exit category and the panic code.

/**
@SYMTestCaseID          PDS-EFM-CT-4094
@SYMTestCaseDesc        Include test case 4105 too
@SYMTestPriority        High
@SYMTestActions         PanicTest function will create a new thread - panic 
                        thread, giving it a pointer to the function which has to
                        be executed and the expectation is that the function 
                        will panic and kill the panic thread.
                        PanicTest function will check the panic thread exit code,
                        exit category and the panic code.         
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF144262
*/
void PanicTest(TFunctor& aFunctor, TExitType aExpectedExitType, const TDesC& aExpectedCategory, TInt aExpectedPanicCode)
    {
    RThread thread;
    _LIT(KThreadName,"FeatMgrPanicThread");
    TEST2(thread.Create(KThreadName, &ThreadFunc, 0x2000, 0x1000, 0x10000, (void*)&aFunctor, EOwnerThread), KErrNone);
    
    TRequestStatus status;
    thread.Logon(status);
    TEST2(status.Int(), KRequestPending);
    thread.Resume();
    User::WaitForRequest(status);
    User::SetJustInTime(ETrue); // enable debugger panic handling

    TEST2(thread.ExitType(), aExpectedExitType);
    TEST(thread.ExitCategory() == aExpectedCategory);
    TEST2(thread.ExitReason(), aExpectedPanicCode);
    
    CLOSE_AND_WAIT(thread);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////     Panic test functions    /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

//1 Panic when calling RFeatureControl::FeatureSupported() on an invalid RFeatureControl object.
class TFeatureControl_NotCreated_FeatureSupported1 : public TFunctor
    {
private:        
    virtual void operator()()
        {
        RFeatureControl ctrl;
        (void)ctrl.FeatureSupported(KConnectivity);
        }
    };
static TFeatureControl_NotCreated_FeatureSupported1 TheFeatureControl_NotCreated_FeatureSupported1;

//2 Panic when calling RFeatureControl::FeatureSupported() on an invalid RFeatureControl object.
class TFeatureControl_NotCreated_FeatureSupported2 : public TFunctor
    {
private:        
    virtual void operator()()
        {
        RFeatureControl ctrl;
        TFeatureEntry fentry;
        (void)ctrl.FeatureSupported(fentry);
        }
    };
static TFeatureControl_NotCreated_FeatureSupported2 TheFeatureControl_NotCreated_FeatureSupported2;

//Panic when calling RFeatureControl::FeaturesSupported() on an invalid RFeatureControl object.
class TFeatureControl_NotCreated_FeaturesSupported : public TFunctor
    {
private:        
    virtual void operator()()
        {
        RFeatureControl ctrl;
        RFeatureArray farray;
        TFeatureEntry fentry;
        TInt err = farray.Append(fentry);
        TEST2(err, KErrNone);
        (void)ctrl.FeaturesSupported(farray);
        }
    };
static TFeatureControl_NotCreated_FeaturesSupported TheFeatureControl_NotCreated_FeaturesSupported;

//Panic when calling RFeatureControl::EnableFeature() on an invalid RFeatureControl object.
class TFeatureControl_NotCreated_EnableFeature : public TFunctor
    {
private:        
    virtual void operator()()
        {
        RFeatureControl ctrl;
        (void)ctrl.EnableFeature(KConnectivity);
        }
    };
static TFeatureControl_NotCreated_EnableFeature TheFeatureControl_NotCreated_EnableFeature;

//Panic when calling RFeatureControl::DisableFeature() on an invalid RFeatureControl object.
class TFeatureControl_NotCreated_DisableFeature : public TFunctor
    {
private:        
    virtual void operator()()
        {
        RFeatureControl ctrl;
        (void)ctrl.DisableFeature(KConnectivity);
        }
    };
static TFeatureControl_NotCreated_DisableFeature TheFeatureControl_NotCreated_DisableFeature;

//1 Panic when calling RFeatureControl::SetFeature() on an invalid RFeatureControl object.
class TFeatureControl_NotCreated_SetFeature1 : public TFunctor
    {
private:        
    virtual void operator()()
        {
        RFeatureControl ctrl;
        (void)ctrl.SetFeature(KConnectivity, EFalse, 0);
        }
    };
static TFeatureControl_NotCreated_SetFeature1 TheFeatureControl_NotCreated_SetFeature1;

//2 Panic when calling RFeatureControl::SetFeature() on an invalid RFeatureControl object.
class TFeatureControl_NotCreated_SetFeature2 : public TFunctor
    {
private:        
    virtual void operator()()
        {
        RFeatureControl ctrl;
        (void)ctrl.SetFeature(KConnectivity, 0);
        }
    };
static TFeatureControl_NotCreated_SetFeature2 TheFeatureControl_NotCreated_SetFeature2;

//Panic when calling RFeatureControl::AddFeature() on an invalid RFeatureControl object.
class TFeatureControl_NotCreated_AddFeature : public TFunctor
    {
private:        
    virtual void operator()()
        {
        RFeatureControl ctrl;
        TFeatureEntry fentry;
        (void)ctrl.AddFeature(fentry);
        }
    };
static TFeatureControl_NotCreated_AddFeature TheFeatureControl_NotCreated_AddFeature;

//Panic when calling RFeatureControl::DeleteFeature() on an invalid RFeatureControl object.
class TFeatureControl_NotCreated_DeleteFeature : public TFunctor
    {
private:        
    virtual void operator()()
        {
        RFeatureControl ctrl;
        (void)ctrl.DeleteFeature(KConnectivity);
        }
    };
static TFeatureControl_NotCreated_DeleteFeature TheFeatureControl_NotCreated_DeleteFeature;

//Panic when calling RFeatureControl::ListSupportedFeatures() on an invalid RFeatureControl object.
class TFeatureControl_NotCreated_ListSupportedFeatures : public TFunctor
    {
private:        
    virtual void operator()()
        {
        RFeatureControl ctrl;
        RFeatureUidArray farray;
        (void)ctrl.ListSupportedFeatures(farray);
        }
    };
static TFeatureControl_NotCreated_ListSupportedFeatures TheFeatureControl_NotCreated_ListSupportedFeatures;

//Panic when calling RFeatureControl::SWIStart() on an invalid RFeatureControl object.
class TFeatureControl_NotCreated_SWIStart : public TFunctor
    {
private:        
    virtual void operator()()
        {
        RFeatureControl ctrl;
        (void)ctrl.SWIStart();
        }
    };
static TFeatureControl_NotCreated_SWIStart TheFeatureControl_NotCreated_SWIStart;

//Panic when calling RFeatureControl::SWIEnd() on an invalid RFeatureControl object.
class TFeatureControl_NotCreated_SWIEnd : public TFunctor
    {
private:        
    virtual void operator()()
        {
        RFeatureControl ctrl;
        (void)ctrl.SWIEnd();
        }
    };
static TFeatureControl_NotCreated_SWIEnd TheFeatureControl_NotCreated_SWIEnd;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DoTestsL()
    {
    TheTest.Start(_L("@SYMTestCaseID:PDS-EFM-CT-4094 RFeatureControl::FeatureSupported() panic test 1"));
    PanicTest(TheFeatureControl_NotCreated_FeatureSupported1, EExitPanic, KPanicCategory, EPanicBadHandle);

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4095 RFeatureControl::FeatureSupported() panic test 2"));
    PanicTest(TheFeatureControl_NotCreated_FeatureSupported2, EExitPanic, KPanicCategory, EPanicBadHandle);

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4096 RFeatureControl::FeaturesSupported() panic test"));
    PanicTest(TheFeatureControl_NotCreated_FeaturesSupported, EExitPanic, KPanicCategory, EPanicBadHandle);

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4097 RFeatureControl::EnableFeature() panic test"));
    PanicTest(TheFeatureControl_NotCreated_EnableFeature, EExitPanic, KPanicCategory, EPanicBadHandle);

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4098 RFeatureControl::DisableFeature() panic test"));
    PanicTest(TheFeatureControl_NotCreated_DisableFeature, EExitPanic, KPanicCategory, EPanicBadHandle);

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4099 RFeatureControl::SetFeature() panic test 1"));
    PanicTest(TheFeatureControl_NotCreated_SetFeature1, EExitPanic, KPanicCategory, EPanicBadHandle);

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4100 RFeatureControl::SetFeature() panic test 2"));
    PanicTest(TheFeatureControl_NotCreated_SetFeature2, EExitPanic, KPanicCategory, EPanicBadHandle);

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4101 RFeatureControl::AddFeature() panic test"));
    PanicTest(TheFeatureControl_NotCreated_AddFeature, EExitPanic, KPanicCategory, EPanicBadHandle);

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4102 RFeatureControl::DeleteFeature() panic test"));
    PanicTest(TheFeatureControl_NotCreated_DeleteFeature, EExitPanic, KPanicCategory, EPanicBadHandle);

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4103 RFeatureControl::ListSupportedFeatures() panic test"));
    PanicTest(TheFeatureControl_NotCreated_ListSupportedFeatures, EExitPanic, KPanicCategory, EPanicBadHandle);

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4104 RFeatureControl::SWIStart() panic test"));
    PanicTest(TheFeatureControl_NotCreated_SWIStart, EExitPanic, KPanicCategory, EPanicBadHandle);

    TheTest.Next(_L("@SYMTestCaseID:PDS-EFM-CT-4105 RFeatureControl::SWIEnd() panic test"));
    PanicTest(TheFeatureControl_NotCreated_SWIEnd, EExitPanic, KPanicCategory, EPanicBadHandle);
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
