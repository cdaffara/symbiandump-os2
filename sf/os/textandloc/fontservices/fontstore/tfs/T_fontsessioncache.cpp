/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

/**
 @file
 @test
 @internalComponent Internal Symbian test code
*/

#include "FNTSTORE.H"
#include "t_fontsessioncache.h"
#include "T_IsolatedFontStore.h"

_LIT(KWorkerProcess,"tfontsessioncacheproc");
_LIT(KSharedChunk,"TestSharedChunk_T_FontSessionCache");

const TInt KNumOfProc = 4;
const TInt KRunningTime = 1000 * 1000 * 5;

//Make sure font is large enough that font and session caches are used sufficiently. 
const TInt KFontHeight = 250;   

// This class is a data mirror to CBitmapFont in order to check its private 
// member iOpenFont. It is only used by TestOpenFontForQtL().
class CBitmapFontDummy:public CFont
    {
public:
    TFontSpec iFontSpecInTwips;
    TAlgStyle iAlgStyle;        
    RHeap* iHeap;
    TInt iFontBitmapOffset;
    COpenFont* iOpenFont; 
    TUint32 iReserved;
    TUint32 iUniqueFontId;     
    };

class CTFontSessionCache : public CTGraphicsBase
    {
public:
    CTFontSessionCache(CTestStep* aStep);
    ~CTFontSessionCache();
    TInt Base();

protected:
// From CTGraphicsStep
    virtual void RunTestCaseL(TInt aCurTestCase);
    virtual void ConstructL();
private:
    void TestOpenFontForQtL();
    void RunMultiWorkerProcessL();
    
    void FlushCaches();
    
private:
    CTIsolatedFontStore *iIFontStore;
    RHeap   *iSharedHeap;
    RChunk  iChunk;
    CFont *iFont;
    };

CTFontSessionCache::CTFontSessionCache(CTestStep* aStep)
 :  CTGraphicsBase(aStep)
    {

    }

CTFontSessionCache::~CTFontSessionCache()
    {
    iIFontStore->iFs->ReleaseFont(iFont);
    delete iIFontStore;
    iChunk.Close(); 
    }

inline TInt CTFontSessionCache::Base() 
    {
    return reinterpret_cast<TInt>(iChunk.Base());
    }

void CTFontSessionCache::ConstructL()
    {
    User::LeaveIfError(iChunk.CreateGlobal(KNullDesC,0x10000,0x10000));
    iSharedHeap = UserHeap::ChunkHeap(iChunk,0x10000,0x1000,0x10000,0,EFalse,0);
    if(iSharedHeap == NULL)
        {
        RDebug::Print(_L("iSharedHeap = NULL"));
        User::Leave(KErrNoMemory);
        }
    iIFontStore = CTIsolatedFontStore::NewL(iSharedHeap);
    iIFontStore->LoadRasterizersL();
    iIFontStore->iFs->LoadFontsAtStartupL();
    
    _LIT(KTypefaceName, "DejaVu Sans Condensed");
    TFontSpec spec(KTypefaceName, KFontHeight);   
    
    TInt ret = iIFontStore->iFs->GetNearestFontToDesignHeightInPixels(iFont,spec);
    TEST(ret == KErrNone);
 
    }

void CTFontSessionCache::FlushCaches()
    {
    TText ch;
    TOpenFontGlyphData *glyphData = NULL;
    for (TInt sHandle = 0; sHandle < KNumOfProc; sHandle++)
        {
        for (ch = 'A'; ch <= 'z'; ch++)
            {
            static_cast<CBitmapFont*> (iFont)->Rasterize(sHandle, ch, glyphData);
            }
        }
    }

/**
    @SYMTestCaseID
    TI18N-FNTSTORE-UT--4003

    @SYMTestCaseDesc
    Qt needs the last bit of iOpenFont to be set 1 as a workaround to maintain
    its compatibility across difference Symbian OS versions.

    @SYMTestActions
    1. Get a CBitmapFont in the constructor
    2. Check the LSB of its iOpenFont by using CBitmapFontDummy
        
    @SYMTestExpectedResults
    Test should pass
*/

void CTFontSessionCache::TestOpenFontForQtL()
    {    
    TEST(reinterpret_cast<TInt>(reinterpret_cast<CBitmapFontDummy*>(iFont)->iOpenFont) & 1);
    }

/**
    @SYMTestCaseID
    TI18N-FNTSTORE-CIT-4002

    @SYMTestCaseDesc
    This case is to test the safty of actions over the shared heap.

    @SYMTestActions
    1. Shared heap is initialised in the constructor
    2. Run rasterizing function to Flush the font caches(glyph tree and session cache).
    3. Launch KNumOfProc worker processes running with random latency at beginning, which
       is to seach the cache from different processes. The globle chunk and font handles are
       passed via process environment variables.
    4. Each one lasts about 1 sec. Within duration of 5 sec, if one terminates, re-launch it.   
        
    @SYMTestExpectedResults
    Test should pass without any Panic.
*/
void CTFontSessionCache::RunMultiWorkerProcessL()
    {
    RProcess ProcArray[KNumOfProc];
    TRequestStatus *completeStatus[KNumOfProc];

    FlushCaches();
    
    for (TInt i = 0; i < KNumOfProc; i++)
        {                    
        RDebug::Print(_L(">>> Launching %d..."),i);
        TInt err;
        err = ProcArray[i].Create(KWorkerProcess, KNullDesC);
        User::LeaveIfError(err);

        TInt FontOffset = reinterpret_cast<TInt>(iFont) - Base();
        ProcArray[i].SetParameter(1,iChunk);        
        ProcArray[i].SetParameter(2,FontOffset);
        ProcArray[i].SetParameter(3,i);
        
        completeStatus[i] = new(ELeave) TRequestStatus; 
        CleanupStack::PushL(completeStatus[i]);
        *completeStatus[i] = KRequestPending;
        
        ProcArray[i].Logon(*completeStatus[i]);
        ProcArray[i].Resume(); //start the process
        }
      
    RTimer timer;
    timer.CreateLocal();
    TRequestStatus timerStatus = KRequestPending;
    TTimeIntervalMicroSeconds32 timeout(KRunningTime);
    timer.After(timerStatus, timeout);
    
    do
        {     
        User::WaitForNRequest(completeStatus, KNumOfProc);
        TInt i = 0;
        for(;i < KNumOfProc;i++ )
            {
                if (*completeStatus[i] != KRequestPending)
                {
                break;
                }
            }

        TExitType exit = ProcArray[i].ExitType();
        TEST(exit == EExitKill);
        TInt reason = ProcArray[i].ExitReason();
        TEST (reason == 0);

        RDebug::Print(_L("<<< Close %d..."), i);
        ProcArray[i].Close();

        RDebug::Print(_L(">>> Launching %d..."), i);
        TInt err;
        err = ProcArray[i].Create(KWorkerProcess, KNullDesC);
        User::LeaveIfError(err);
        
        TInt FontOffset = reinterpret_cast<TInt>(iFont) - Base();
        ProcArray[i].SetParameter(1,iChunk);        
        ProcArray[i].SetParameter(2,FontOffset);
        ProcArray[i].SetParameter(3,i);
        
        //run process 1
        *completeStatus[i] = KRequestPending;
        ProcArray[i].Logon(*completeStatus[i]);
        ProcArray[i].Resume(); //start the process
        }
    while (timerStatus == KRequestPending);
    
    for (TInt i = 0; i < KNumOfProc; i++)
        {
        if(*completeStatus[i] == KRequestPending)
            {
            User::WaitForRequest(*completeStatus[i]);
            }       
        RDebug::Print(_L("<<< Tear down Close %d..."),i);
        ProcArray[i].Close(); //tear down
        }        
    CleanupStack::PopAndDestroy(KNumOfProc);
    }

void CTFontSessionCache::RunTestCaseL( TInt aCurTestCase )
    {
    ((CTFontSessionCacheStep*) iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

    switch (aCurTestCase)
        {
        case 1:
            ((CTFontSessionCacheStep*) iStep)->SetTestStepID(_L("TI18N-FNTSTORE-UT--4003"));
            INFO_PRINTF1(_L("Test CBitmapFont::iOpenFont last bit for Qt\n"));
            TestOpenFontForQtL();
            break;

        case 2:
            ((CTFontSessionCacheStep*) iStep)->SetTestStepID(_L("TI18N-FNTSTORE-CIT-4002"));
            INFO_PRINTF1(_L("Test GetCharacterData() in muti-process client\n"));
            RunMultiWorkerProcessL();
            break;
            
        case 3:
            ((CTFontSessionCacheStep*) iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
            ((CTFontSessionCacheStep*) iStep)->CloseTMSGraphicsStep();
            TestComplete();
            break;
        }
    ((CTFontSessionCacheStep*)iStep)->RecordTestResultL();
    }

// --------------
__CONSTRUCT_STEP__(FontSessionCache)
