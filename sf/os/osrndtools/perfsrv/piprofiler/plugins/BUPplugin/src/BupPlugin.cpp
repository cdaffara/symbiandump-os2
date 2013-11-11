/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

 
#include "BupPlugin.h"	
//#include <piprofiler/EngineUIDs.h>

#include <w32std.h> 	// for listening key events

	
// LITERALS
// CONSTANTS
const TUid KSamplerBupPluginUid = { 0x2001E5B6 };

/*
 *	
 *	class CBupPlugin implementation
 * 
 */

CBupPlugin* CBupPlugin::NewL(const TUid /*aImplementationUid*/, TAny* /*aInitParams*/)
	{
	LOGTEXT(_L("CBupPlugin::NewL() - entry"));
    CBupPlugin* self = new (ELeave) CBupPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	LOGTEXT(_L("CBupPlugin::NewL() - exit"));
    return self;
	}

CBupPlugin::CBupPlugin() :
	iVersionDescriptor(&(this->iVersion[1]),0,19), 	
	iSamplerType(PROFILER_USER_MODE_SAMPLER)
	{
	iSamplerId = PROFILER_BUP_SAMPLER_ID;
    iEnabled = EFalse;
	LOGTEXT(_L("CBupPlugin::CBupPlugin() - konstruktori"));
	}

void CBupPlugin::ConstructL() 
	{
	LOGTEXT(_L("CBupPlugin::ConstructL() - entry"));
	
	// initiate sampler attributes array
	iSamplerAttributes = new(ELeave) CArrayFixFlat<TSamplerAttributes>(1); // only one sampler
	
	// insert default attributes to array
	InitiateSamplerAttributesL();
	
	LOGTEXT(_L("CBupPlugin::ConstructL() - exit"));
	}


CBupPlugin::~CBupPlugin()
	{
	LOGTEXT(_L("CBupPlugin::~CBupPlugin() - entry"));
	if(iButtonListener)
	    {
	    // check if button listener still running
	    if(Enabled())
	        {
	        // stop profiling
	        iButtonListener->Stop();
	        }
        delete iButtonListener;
	    }
	
	if(iSamplerAttributes)
	    {
	    iSamplerAttributes->Reset();
	    }
    delete iSamplerAttributes;
	
	LOGTEXT(_L("CBupPlugin::~CBupPlugin() - exit"));
	}

TUid CBupPlugin::Id(TInt /*aUid*/) const
	{
    LOGSTRING2("CBupPlugin::Id():0x%X", KSamplerBupPluginUid.iUid );
    return KSamplerBupPluginUid;
	}

void CBupPlugin::InitiateSamplerAttributesL()
    {
    // create sampler attribute container
    TSamplerAttributes attr(KSamplerBupPluginUid.iUid,
            KBUPShortName(),
            KBUPLongName(),
            KBUPDescription(),
            -1,
            ETrue,
            EFalse,
            0); // default item count
    this->iSamplerAttributes->AppendL(attr);
    }

// returns setting array
void CBupPlugin::GetAttributesL(CArrayFixFlat<TSamplerAttributes>* aAttributes)
    {
    // append my own attributes to complete array, requested by profiler engine
    aAttributes->AppendL(iSamplerAttributes->At(0));
    }

TInt CBupPlugin::SetAttributesL(TSamplerAttributes& aAttributes)
    {
    TSamplerAttributes attr;

    attr = iSamplerAttributes->At(0);
    // if UIDs match replace the old 
    if(attr.iUid == aAttributes.iUid)
        {
        // replace the old attribute container
        iSamplerAttributes->Delete(0);
        iSamplerAttributes->InsertL(0, aAttributes);
        return KErrNone;
        }
    return KErrNotFound;
    }

/* 
 * Method for parsing and transforming text array settings into TSamplerAttributes (per each sub sampler),
 * called by CSamplerController class
 * 
 * @param array of raw text setting lines, e.g. [gpp]\nenabled=true\nsampling_period_ms=1\n
 */
TInt CBupPlugin::ConvertRawSettingsToAttributes(CDesC8ArrayFlat* aAllSettingsArray)
    {
    // local literals
    _LIT8(KBUPShort, "bup");

    TInt err(KErrNone);
    TBuf8<16> samplerSearchName;
    samplerSearchName.Copy(KBUPShort);
    
    // get sampler specific settings  
    err = DoSetSamplerSettings(aAllSettingsArray, samplerSearchName, 0);
    
    // returns KErrNone if settings found, otherwise KErrNotFound
    return err;
    }

TInt CBupPlugin::DoSetSamplerSettings(CDesC8ArrayFlat* aAllSettings, TDesC8& aSamplerName, TInt aIndex)
    {
    // 
    TBuf8<16> samplerSearch;
    samplerSearch.Copy(KBracketOpen);
    samplerSearch.Append(aSamplerName);
    samplerSearch.Append(KBracketClose);
    
    // read a line
    for (TInt i(0); i<aAllSettings->MdcaCount(); i++)
        {
        // check if this line has a setting block start, i.e. contains [xxx] in it
        if (aAllSettings->MdcaPoint(i).CompareF(samplerSearch) == 0)
            {
            // right settings block found, now loop until the next block is found
            for(TInt j(i+1);j<aAllSettings->MdcaCount();j++)
                {
                // check if the next settings block was found
                if(aAllSettings->MdcaPoint(j).Left(1).CompareF(KBracketOpen) != 0)
                    {
                    // save found setting value directly to its owners attributes
                    SaveSettingToAttributes(aAllSettings->MdcaPoint(j), aIndex);
                    }
                else
                    {
                    // next block found, return KErrNone
                    return KErrNone;
                    }
                }
            }
        }
    
    return KErrNotFound;
    }

/**
 * Method for setting a specific descriptor (from settings file) to attribute structure
 * 
 * @param aSetting  
 * @param aName  
 */
void CBupPlugin::SaveSettingToAttributes(const TDesC8& aSetting, TInt aIndex)
    {
    // find the equal mark from the setting line
    TInt sepPos = aSetting.Find(KSettingItemSeparator);
    // check that '=' is found
    if (sepPos > 0)
        {
        // check that the element matches
        if (aSetting.Left(sepPos).CompareF(KEnabled) == 0)
            {
            TBool en;
            CSamplerPluginInterface::Str2Bool(aSetting.Right(aSetting.Length()-sepPos-1), en);
            if(iSamplerAttributes->At(aIndex).iEnabled != en)
                {
                iSamplerAttributes->At(aIndex).iEnabled = en;
                }
            }
        }
    }

TInt CBupPlugin::GetSamplerType()
	{
	return iSamplerType;
	}

TInt CBupPlugin::ResetAndActivateL(CProfilerSampleStream& aStream) 
	{
	LOGTEXT(_L("CBupPlugin::ResetAndActivate() - entry"));
	TInt ret(KErrNone);
	
	// check if sampler enabled
	if(iSamplerAttributes->At(0).iEnabled)
	    {
        // create first the listener instance
        iButtonListener = CProfilerButtonListener::NewL(this);
        
        LOGTEXT(_L("CBupPlugin::ResetAndActivate() - listener created"));
        
        iStream = &aStream;
        TInt length = this->CreateFirstSample();
        iVersion[0] = (TUint8)length;
        LOGSTRING2("CBupPlugin::ResetAndActivate() - AddSample, length %d",length);
        ret = AddSample(iVersion, length+1, 0);
        if(ret != KErrNone)
            return ret;
        
        // activate button listener
        ret = iButtonListener->StartL();

        iEnabled = ETrue;
        
        LOGTEXT(_L("CBupPlugin::ResetAndActivate() - exit"));
	    }
	return ret;

	}
	
TInt CBupPlugin::CreateFirstSample()
	{
	LOGTEXT(_L("CBupPlugin::CreateFirstSample - entry"));
	this->iVersionDescriptor.Zero();
	this->iVersionDescriptor.Append(_L8("Bappea_BUP_V"));
	this->iVersionDescriptor.Append(PROFILER_BUP_SAMPLER_VERSION);
	LOGTEXT(_L("CBupPlugin::CreateFirstSample - exit"));
	return (TInt)(this->iVersionDescriptor.Length());
	}

TInt CBupPlugin::StopSampling() 
	{
	if(iButtonListener)
		{
		iButtonListener->Stop();
		delete iButtonListener;	// delete listener after every trace
		iButtonListener = NULL;
		}
	
    // set disabled
    iEnabled = EFalse;

	return KErrNone;
	}

void CBupPlugin::FillThisStreamBuffer(TBapBuf* /*aBapBuf*/, TRequestStatus& /*aStatus*/)
	{
	}

/*
 * 
 * Implementation of class CProfilerButtonListener
 * 
 */
CProfilerButtonListener::CProfilerButtonListener(CBupPlugin* aSampler) 
	{
	LOGTEXT(_L("CProfilerButtonListener::CProfilerButtonListener() - konstuktori"));
	this->iSampler = aSampler;
	iSampleStartTime = 0;
	LOGTEXT(_L("CProfilerButtonListener::CProfilerButtonListener() - konstuktori exit"));
	}

CProfilerButtonListener* CProfilerButtonListener::NewL(CBupPlugin* aSampler)
	{
	LOGTEXT(_L("CProfilerButtonListener::NewL() - entry"));
	CProfilerButtonListener* self = new (ELeave) CProfilerButtonListener(aSampler);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	LOGTEXT(_L("CProfilerButtonListener::NewL() - exit"));
	return self;
	}

CProfilerButtonListener::~CProfilerButtonListener() 
	{
	LOGTEXT(_L("CProfilerButtonListener::~CProfilerButtonListener() - entry af"));

	if(iMainWindow)
		{
		LOGTEXT(_L("CProfilerButtonListener::~CProfilerButtonListener(): flushing iWs"));
		iWs.Flush();
		LOGTEXT(_L("CProfilerButtonListener::~CProfilerButtonListener(): finishing"));
		}
	delete iMainWindow;
	LOGTEXT(_L("CProfilerButtonListener::~CProfilerButtonListener() - exit"));
	}
	
void CProfilerButtonListener::ConstructMainWindowL()
	{
    LOGTEXT(_L("CProfilerButtonListener::ConstructMainWindowL() - Entry"));

    CWindow* window = new (ELeave) CWindow(this);
    CleanupStack::PushL( window );
	window->ConstructL(TRect(TPoint(0,0), TSize(0,0)));
    delete iMainWindow;
    iMainWindow = window;
    CleanupStack::Pop( window );
	
    LOGTEXT(_L("CProfilerButtonListener::ConstructMainWindowL() - Exit"));
	}

void CProfilerButtonListener::HandleKeyEventL (TKeyEvent& /*aKeyEvent*/)
    {
    LOGTEXT(_L("CProfilerButtonListener::HandleKeyEventL() - Start"));
    LOGTEXT(_L("CProfilerButtonListener::HandleKeyEventL() - End"));
	}


TInt CProfilerButtonListener::RunError(TInt aError)
    {
    // get rid of everything we allocated
    // deactivate the anim dll before killing window, otherwise anim dll dies too early
    iAnim->Deactivate();
    iAnim->Close();

    iAnimDll->Close();
    
    return aError;
    }

void CProfilerButtonListener::RunL() 
	{	
    // resubscribe before processing new value to prevent missing updates
	IssueRequest();
	
	TInt c = 0;
	if(RProperty::Get(KProfilerKeyEventPropertyCat, EProfilerKeyEventPropertySample, c) == KErrNone)
		{
		// do something with event
		LOGSTRING2("CProfilerButtonListener::RunL() - event [%d] received",c);
	
		iSample[0] = c;
		iSample[1] = c >> 8;
		iSample[2] = c >> 16;
		iSample[3] = c >> 24;
		
		// Space for GPP sample time		
		TUint32 sampleTime = User::NTickCount() - iSampleStartTime; 
		LOGSTRING2("CProfilerButtonListener::RunL() - sample time is %d",sampleTime);
		
		iSample[4] = sampleTime;
		iSample[5] = sampleTime >> 8;
		iSample[6] = sampleTime >> 16;
		iSample[7] = sampleTime >> 24;
		
		iSampler->AddSample(iSample, 8, 0xb0);
		}
	}
	
TInt CProfilerButtonListener::StartL()
	{
	LOGTEXT(_L("CProfilerButtonListener::StartL() - Activate touch server dll"));
	TInt err(KErrNone);
	
	// get the property value
	TInt r = RProperty::Get(KGppPropertyCat, EGppPropertySyncSampleNumber, iSampleStartTime);
	if(r != KErrNone)
		{
		LOGSTRING2("CProfilerButtonListener::StartL() - getting iSyncOffset failed, error %d", r);
		}
	
	iAnimDll = new (ELeave) RAnimDll(iWs);
	LOGTEXT(_L("CProfilerButtonListener::StartL() - #1"));
	
	TParse* fp = new (ELeave) TParse();
	CleanupStack::PushL(fp);
	fp->Set( KDllName, &KDC_SHARED_LIB_DIR , NULL );    
	LOGSTRING2("CProfilerButtonListener::StartL() - touch event server: %S" , &(fp->FullName()));

	err = iAnimDll->Load(fp->FullName());
	// check if anim dll load failed
	if(err != KErrNone)
	    {
        CleanupStack::PopAndDestroy(fp);
	    // stop plugin if failed
	    iAnimDll->Close();
	    return KErrGeneral;
	    }
    CleanupStack::PopAndDestroy(fp);
 	LOGTEXT(_L("CProfilerButtonListener::StartL() - #2"));

	iAnim = new (ELeave) RProfilerTouchEventAnim(*iAnimDll);
 	LOGTEXT(_L("CProfilerButtonListener::StartL() - #3"));
	iAnim->ConstructL(iMainWindow->Window());
	
	// activate the animation dll for collecting touch and key events
	iAnim->Activate();

	// wait for a new sample
	IssueRequest();
	
	// hide this window group from the app switcher
	iMainWindow->Client()->Group().SetOrdinalPosition(-1);
	iMainWindow->Client()->Group().EnableReceiptOfFocus(EFalse);
	return KErrNone;
	}

TInt CProfilerButtonListener::Stop() 
	{
	LOGTEXT(_L("CProfilerButtonListener::Stop() - enter"));
	// deactivate the anim dll before killing window, otherwise anim dll dies too early
	iAnim->Deactivate();
	iAnim->Close();

	iAnimDll->Close();

	Cancel();
	LOGTEXT(_L("CProfilerButtonListener::Stop() - exit"));
	return KErrNone;
	}


///////////////////////////////////////////////////////////////////////////////
////////////////////////// CWindow implementation /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CWindow::CWindow(CWsClient* aClient)
: iClient(aClient)
	{
    LOGTEXT(_L("CWindow::CWindow()"));
	}

void CWindow::ConstructL (const TRect& aRect, CWindow* aParent)
	{
	LOGTEXT(_L("CWindow::ConstructL(): Start"));

	// If a parent window was specified, use it; if not, use the window group
	// (aParent defaults to 0).
	RWindowTreeNode* parent= aParent ? (RWindowTreeNode*) &(aParent->Window()) : &(iClient->iGroup);
	iWindow=RWindow(iClient->iWs); // use app's session to window server
	User::LeaveIfError(iWindow.Construct(*parent,(TUint32)this));
	LOGSTRING2("CWindow::ConstructL(): Start - window handle is: 0x%08x", this);
	iRect = aRect;
	iWindow.SetExtent(iRect.iTl, iRect.Size()); // set extent relative to group coords
	iWindow.Activate(); // window is now active
	LOGTEXT(_L("CWindow::ConstructL(): End"));
	}


CWindow::~CWindow()
	{
    LOGTEXT(_L("CWindow::~CWindow(): Start"));
	iWindow.Close(); // close our window
    LOGTEXT(_L("CWindow::~CWindow(): End"));
	}

RWindow& CWindow::Window()
	{
    LOGTEXT(_L("CWindow::Window()"));
	return iWindow;
	}

CWindowGc* CWindow::SystemGc()
	{
    LOGTEXT(_L("CWindow::SystemGc()"));
	return iClient->iGc;
	}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// CWsClient implementation ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
CWsClient::CWsClient()
: CActive(CActive::EPriorityStandard)
	{
    LOGTEXT(_L("CWsClient::CWsClient()"));
	}

void CWsClient::ConstructL()
	{
    LOGTEXT(_L("CWsClient::ConstructL() - Start"));
    TInt r = RProperty::Define(EProfilerKeyEventPropertySample, RProperty::EInt, KAllowAllPolicy, KCapabilityNone);
    if (r!=KErrAlreadyExists)
        {
        User::LeaveIfError(r);
        }
    
	CActiveScheduler::Add(this);

	// attach to 
	User::LeaveIfError(iProperty.Attach(KProfilerKeyEventPropertyCat,EProfilerKeyEventPropertySample));
    
	// get a session going
	User::LeaveIfError(iWs.Connect());

	// construct screen device and graphics context
	iScreen=new (ELeave) CWsScreenDevice(iWs); // make device for this session
	User::LeaveIfError(iScreen->Construct( 0 )); // and complete its construction
	User::LeaveIfError(iScreen->CreateContext(iGc)); // create graphics context

	// construct our one and only window group
	iGroup=RWindowGroup(iWs);
	User::LeaveIfError(iGroup.Construct((TInt)this, EFalse)); // meaningless handle; enable focus
	
	// construct main window
	ConstructMainWindowL();

	LOGTEXT(_L("CWsClient::CWsClient() - End"));
	}

CWsClient::~CWsClient()
	{
    LOGTEXT(_L("CWsClient::~CWsClient() - Start"));
    
	// get rid of everything we allocated
	delete iGc;
	delete iScreen;
	
	iGroup.Close();
	// finish with window server
	iWs.Close();
	
    LOGTEXT(_L("CWsClient::~CWsClient() - Exit"));
	}

void CWsClient::Exit()
	{
    LOGTEXT(_L("CWsClient::Exit() - Start"));

	// destroy window group
	iGroup.Close();
	// finish with window server
    iProperty.Close();
	iWs.Close();
    LOGTEXT(_L("CWsClient::Exit() - Exit"));
	}

void CWsClient::IssueRequest()
	{
    LOGTEXT(_L("CWsClient::IssueRequest() - Start"));
    iProperty.Subscribe( iStatus );
    SetActive(); // so we're now active
    LOGTEXT(_L("CWsClient::IssueRequest() - Exit"));
	}

void CWsClient::DoCancel()
	{
    LOGTEXT(_L("CWsClient::DoCancel() - Start"));
	// clean up the sample property
    iProperty.Cancel();
    iProperty.Close();
    LOGTEXT(_L("CWsClient::DoCancel() - Exit"));
	}

void CWsClient::ConstructMainWindowL()
	{
    LOGTEXT(_L("CWsClient::ConstructMainWindowL()"));
	}

