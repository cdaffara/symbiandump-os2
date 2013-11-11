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

 
#include "GeneralsPlugin.h"	
//#include <piprofiler/EngineUIDs.h>
#include <piprofiler/ProfilerTraces.h>

const TInt KMaxSamplerAmount = 20;

// LITERALS
_LIT8(KSamplingPeriod, "sampling_period_ms");

// CONSTANTS
// GeneralsPlugin UID:
const TUid KSamplerGeneralsPluginUid = { 0x2001E5B2 };

// Gpp sub-sampler UID:
const TUid KSamplerGppPluginUid = { 0x2001E570 };

// Gfc sub-sampler UID:
const TUid KSamplerGfcPluginUid = { 0x2001E571 };

// Itt sub-sampler UID:
const TUid KSamplerIttPluginUid = { 0x2001E572 };

// Mem sub-sampler UID:
const TUid KSamplerMemPluginUid = { 0x2001E573 };

// Pri sub-sampler UID:
const TUid KSamplerPriPluginUid = { 0x2001E574 };

/*
 *	
 *	class CGeneralsPlugin implementation
 * 
 */
 
CGeneralsPlugin* CGeneralsPlugin::NewL(const TUid aImplementationUid, TAny* /*aInitParams*/)
    {
	LOGTEXT(_L("CGeneralsPlugin::NewL() - entry"));
    CGeneralsPlugin* self = new (ELeave) CGeneralsPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	LOGTEXT(_L("CGeneralsPlugin::NewL() - exit"));
    return self;
    }

CGeneralsPlugin::CGeneralsPlugin() :
	iVersionDescriptor(&(this->iVersion[1]),0,19)
	{
	iSamplerType = PROFILER_KERNEL_MODE_SAMPLER;
	iSamplerId = PROFILER_GENERALS_SAMPLER_ID;
	iBufferHandler = NULL;
    iEnabled = EFalse;
	LOGTEXT(_L("CGeneralsPlugin::CGeneralsPlugin() - constructor"));
	}

void CGeneralsPlugin::ConstructL() 
    {
	LOGTEXT(_L("CGeneralsPlugin::ConstructL() - entry"));
	
	// create attribute array
	iSamplerAttributes = new(ELeave) CArrayFixFlat<TSamplerAttributes>(KSubSamplerCount);
	
	TInt err = InitiateSamplerL();
	if( err != KErrNone )
		{
			LOGTEXT(_L("CGeneralsPlugin::ConstructL - LEAVING, failed to load open sampler device"));
			User::Leave(err);
		}
	
	// initiate sampler attributes, i.e. settings for modification
	InitiateSamplerAttributesL();
	
	LOGTEXT(_L("CGeneralsPlugin::ConstructL() - exit"));
    }


CGeneralsPlugin::~CGeneralsPlugin()
	{
	// clean all the members
	CleanSampler();	// clean the created sampler
	}

TInt CGeneralsPlugin::InitiateSamplerL()
	{
	RThread me;
	
	LOGTEXT(_L("CGeneralsPlugin::InitiateSamplerL - #1"));

	me.SetPriority(EPriorityRealTime);

	LOGTEXT(_L("CGeneralsPlugin::InitiateSamplerL - #2"));
	
	// create 
	User::FreeLogicalDevice(KPluginSamplerName);
	TInt err(KErrGeneral);
	
	LOGTEXT(_L("CGeneralsPlugin::InitiateSamplerL - #3"));

    err = User::LoadLogicalDevice(KPluginSamplerName);
    if(err != KErrNone)
        {
        User::Leave(err);
        }

	LOGTEXT(_L("CGeneralsPlugin::InitiateSamplerL - #4"));
	
	err = KErrGeneral;

    err = iGeneralsSampler.Open();
    if(err != KErrNone)
        {
        LOGSTRING2("CGeneralsPlugin::InitiateSamplerL - Could not open sampler device - waiting and trying again: %d", err);
        User::Leave(err);
        }

	LOGTEXT(_L("CGeneralsPlugin::InitiateSamplerL - #5"));

	return err;
	}

/* 
 * 
 * Default sampler attributes
 * 
 */
void CGeneralsPlugin::InitiateSamplerAttributesL()
    {

    // 
    for(TInt i(0);i<KMaxSamplerAmount;i++)
        {
        switch(i)
            {
// Usage:
//            TSamplerAttributes(TUint32 aUid,
//                    const TDesC8& aShortName,
//                    const TDesC& aName,
//                    const TDesC& aDescription,
//                    TInt aSampleRate,
//                    TBool aEnabled,
//                    TBool aHidden,
//                    TUint32 aItemCount);
            case PROFILER_GPP_SAMPLER_ID:
                {
                TSamplerAttributes attr(KSamplerGppPluginUid.iUid,
                        KGPPShortName(),
                        KGPPLongName(),
                        KGPPDescription(),
                        1,
                        ETrue,
                        ETrue,
                        0);
                iSamplerAttributes->AppendL(attr);
                break;
                }
            case PROFILER_GFC_SAMPLER_ID:
                {
                TSamplerAttributes attr2(KSamplerGfcPluginUid.iUid,
                        KGFCShortName(),
                        KGFCLongName(),
                        KGFCDescription(),
                        -1,
                        EFalse,
                        EFalse,
                        0);
                this->iSamplerAttributes->AppendL(attr2);
                break;
                }
            case PROFILER_ITT_SAMPLER_ID:
                {
                TSamplerAttributes attr3(KSamplerIttPluginUid.iUid,
                        KITTShortName(),
                        KITTLongName(),
                        KITTDescription(),
                        -1,
                        ETrue,
                        EFalse,
                        0);
                this->iSamplerAttributes->AppendL(attr3);
                break;
                }
            case PROFILER_MEM_SAMPLER_ID:
                {
                TSamplerAttributes attr4(KSamplerMemPluginUid.iUid,
                        KMEMShortName(),
                        KMEMLongName(),
                        KMEMDescription(),
                        -1,
                        EFalse,
                        EFalse,
                        0);
//                // select event or sampling based
//                attr4.iSettingItem1.iSettingDescription.Copy(KMEM1Desc);
//                attr4.iSettingItem1.iType = TSettingItem::ESettingItemTypeBool;
//                attr4.iSettingItem1.iValue.AppendNum(0, EDecimal);
//                attr4.iSettingItem1.iUIText.Copy(KMEM1UIText);
//                attr4.iSettingItem1.iSettingText.Copy(KMemCounter1);
                
                this->iSamplerAttributes->AppendL(attr4);
                break;
                }
            case PROFILER_PRI_SAMPLER_ID:
                {
                TSamplerAttributes attr5(KSamplerPriPluginUid.iUid,
                        KPRIShortName(),
                        KPRILongName(),
                        KPRIDescription(),
                        3000,
                        EFalse,
                        EFalse,
                        0);
                this->iSamplerAttributes->AppendL(attr5);
                break;
                }
            }
        }
    }

TInt CGeneralsPlugin::CleanSampler()
	{
    LOGTEXT(_L("CGeneralsPlugin::CleanSampler() - deleting buffer handler"));
    // release the buffer handler
    if(iBufferHandler)
        {
        iBufferHandler->Cancel();
        delete iBufferHandler;
        iBufferHandler = NULL;
        }
    
    LOGTEXT(_L("CGeneralsPlugin::CleanSampler() - closing sampler"));
	iGeneralsSampler.Close();
	
	LOGTEXT(_L("CGeneralsPlugin::CleanSampler() - Freeing sampler device"));
	User::FreeLogicalDevice(KPluginSamplerName);
	
	// release attribute array
	if(iSamplerAttributes)
	    {
	    iSamplerAttributes->Reset();
	    }
    delete iSamplerAttributes;
    iSamplerAttributes = NULL;

	LOGTEXT(_L("CGeneralsPlugin::CleanSampler() - exit"));

	return KErrNone;
	}

// returns setting array
void CGeneralsPlugin::GetAttributesL(CArrayFixFlat<TSamplerAttributes>* aAttributes)
    {
    TInt count(iSamplerAttributes->Count());
    // append all sampler attributes to aAttributes array
    for(TInt i(0);i<count;i++)
        {
        aAttributes->AppendL(iSamplerAttributes->At(i));
        }
    }

TInt CGeneralsPlugin::SetAttributesL(TSamplerAttributes& aAttributes)
    {
    TSamplerAttributes attr;

    TInt count(iSamplerAttributes->Count());
    // loop the sub sampler attributes (by UID)
    for(TInt i(0);i<count;i++)
        {
        attr = iSamplerAttributes->At(i);
        // if UIDs match replace the old 
        if(attr.iUid == aAttributes.iUid)
            {
            // replace the old attribute container
            iSamplerAttributes->Delete(i);
            iSamplerAttributes->InsertL(i, aAttributes);
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

/* 
 * Method for parsing and transforming text array settings into TSamplerAttributes (per each sub sampler),
 * called by CSamplerController class
 * 
 * @param array of raw text setting lines, e.g. [gpp]\nenabled=true\nsampling_period_ms=1\n
 */
TInt CGeneralsPlugin::ConvertRawSettingsToAttributes(CDesC8ArrayFlat* aAllSettingsArray)
    {
    // local literals
    _LIT8(KGPPShort, "gpp");
    _LIT8(KGFCShort, "gfc");
    _LIT8(KITTShort, "itt");
    _LIT8(KMEMShort, "mem");
    _LIT8(KPRIShort, "pri");

    TInt err(KErrNone);
    
    TBuf8<16> samplerSearchName;
    
    // loop previous settings, update value if changed
    for(TInt i(0);i<KSubSamplerCount;i++)
        {
        // go through all the sub samplers
        switch (i)
            {
            case 0:
                samplerSearchName.Copy(KGPPShort);
                break;
            case 1:
                samplerSearchName.Copy(KGFCShort);
                break;
            case 2:
                samplerSearchName.Copy(KITTShort);
                break;
            case 3:
                samplerSearchName.Copy(KMEMShort);
                break;
            case 4:
                samplerSearchName.Copy(KPRIShort);
                break;
            }

        // get sampler specific settings  
        err = DoSetSamplerSettings(aAllSettingsArray, samplerSearchName, i);
        }
    
    // returns KErrNone if settings found, otherwise KErrNotFound
    return err;
    }

/**
 * Method for searching sampler specific settings among all settings (raw setting lines read from settings file)
 * 
 * @param aAllSettings array of all settings from settings file
 * @param aSamplerName short name of sampler to be searched among the settings
 * @param aIndex index number of sampler specific sampler attributes (TSamplerAttributes)
 * @return KErrNone if settings found ok else KErrNotFound
 */
TInt CGeneralsPlugin::DoSetSamplerSettings(CDesC8ArrayFlat* aAllSettings, TDesC8& aSamplerName, TInt aIndex)
    {
    // sampler name to be searched among the all settings
    TBuf8<16> samplerSearch;
    samplerSearch.Copy(KBracketOpen);
    samplerSearch.Append(aSamplerName);
    samplerSearch.Append(KBracketClose);
    
    // read a line from ALL settings array
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
    // no settings found for specific sampler
    return KErrNotFound;
    }

/**
 * Method for setting a specific descriptor (from settings file) to attribute structure
 * 
 * @param aSetting  
 * @param aName  
 */
void CGeneralsPlugin::SaveSettingToAttributes(const TDesC8& aSetting, TInt aIndex)
    {
    // local literals
    _LIT8(KSettingItemSeparator, "=");
    
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
            if(en != iSamplerAttributes->At(aIndex).iEnabled)
                {
                iSamplerAttributes->At(aIndex).iEnabled = en;
                }
            }
        else if (aSetting.Left(sepPos).CompareF(KSamplingPeriod) == 0)
            {
            TInt sr;
            CSamplerPluginInterface::Str2Int(aSetting.Right(aSetting.Length()-sepPos-1), sr);
            if(sr != iSamplerAttributes->At(aIndex).iSampleRate)
                {
                iSamplerAttributes->At(aIndex).iSampleRate = sr;
                }
            }
        }
    }


TUid CGeneralsPlugin::Id(TInt aSubId) const
	{
		if(aSubId == PROFILER_GPP_SAMPLER_ID)		
			return KSamplerGppPluginUid;
		else if (aSubId == PROFILER_GFC_SAMPLER_ID)		
			return KSamplerGfcPluginUid;
		else if (aSubId == PROFILER_ITT_SAMPLER_ID)		
			return KSamplerIttPluginUid;
		else if (aSubId == PROFILER_MEM_SAMPLER_ID)		
			return KSamplerMemPluginUid;
		else if (aSubId == PROFILER_PRI_SAMPLER_ID)		
			return KSamplerPriPluginUid;
		else
			return KSamplerGeneralsPluginUid;
	}

TInt CGeneralsPlugin::SubId(TUid aId) const
	{
/* definitions from ProfilerConfig.h:
 * 	#define		PROFILER_GPP_SAMPLER_ID			1
	#define		PROFILER_GFC_SAMPLER_ID			2
	#define		PROFILER_ITT_SAMPLER_ID			3
	#define		PROFILER_MEM_SAMPLER_ID			4
	#define		PROFILER_PRI_SAMPLER_ID			5
 */
	
		if(aId == KSamplerGppPluginUid)		
			return PROFILER_GPP_SAMPLER_ID;
		else if (aId == KSamplerGfcPluginUid)		
			return PROFILER_GFC_SAMPLER_ID;
		else if (aId == KSamplerIttPluginUid)		
			return PROFILER_ITT_SAMPLER_ID;
		else if (aId == KSamplerMemPluginUid)		
			return PROFILER_MEM_SAMPLER_ID;
		else if (aId == KSamplerPriPluginUid)		
			return PROFILER_PRI_SAMPLER_ID;
		else
			return KErrNotFound;
	}

TInt CGeneralsPlugin::GetSamplerType()
	{
	return iSamplerType;
	}


void CGeneralsPlugin::InstallStreamForActiveTraces(RGeneralsSampler& sampler, CProfilerSampleStream& aStream)
	{
	// output mode for this trace is stream
	if(!iBufferHandler)
		{
		// stream object has not been created yet
		LOGTEXT(_L("CGeneralsPlugin::InstallStreamForActiveTraces - creating stream for trace"));	
		
		// use a 32KB buffer to transfer data from sampler to client
		// commonStream = new RProfilerSampleStream(sampler,totalPrefix,32768);
		TRAPD(err, iBufferHandler = CProfilerBufferHandler::NewL(aStream, sampler));
		if(err != KErrNone)
		    {
		    LOGTEXT(_L("CGeneralsPlugin::InstallStreamForActiveTraces() - No memory"));
		    return;
		    }
		}

	// initiate receiving of data from the sampler device driver
	if(iBufferHandler)
		{
		iBufferHandler->StartReceivingData();
		}
}

void CGeneralsPlugin::SetSettingsToSamplers()
    {
    TSamplerAttributes attr;
    
    // loop through the setting attributes
    for(TInt i(0);i<iSamplerAttributes->Count();i++)
        {
        // get the attribute container
        attr = iSamplerAttributes->At(i);
        
        // make changes according to right sampler, NOTE! The old IDs of sub samplers (i+1)!
        if(attr.iEnabled)
            {
            iGeneralsSampler.MarkTraceActive(i+1);
            
            // set enabled
            iEnabled = ETrue;
            }
        else
            {
            iGeneralsSampler.MarkTraceInactive(i+1);
            }
        // set sampling period if available
        if(attr.iSampleRate != KErrNotFound)
            {
            iGeneralsSampler.SetSamplingPeriod(i+1, attr.iSampleRate);
            }
        }
    }

TInt CGeneralsPlugin::ResetAndActivateL(CProfilerSampleStream& aStream) 
    {
	// the sampler starting functionality
	LOGTEXT(_L("CGeneralsPlugin::ResetAndActivate() - entry"));

    // now before starting the latest settings must be set to samplers itself
	SetSettingsToSamplers();

	if(Enabled())
	    {
        LOGTEXT(_L("CGeneralsPlugin::ResetAndActivate() - starting sampling..."));
        // start sampling process of enabled sub samplers
        iGeneralsSampler.StartSampling();		
        LOGTEXT(_L("CGeneralsPlugin::ResetAndActivate() - installing stream for an active trace..."));
    
        // install the trace for enabled samplers
        InstallStreamForActiveTraces(iGeneralsSampler, aStream);
        LOGSTRING2("CGeneralsPlugin::ResetAndActivate() - stream installed: 0x%x", aStream);
	    }
	
	LOGTEXT(_L("CGeneralsPlugin::ResetAndActivate() - exit"));
	return KErrNone;
    }
	
TInt CGeneralsPlugin::StopSampling() 
    {
    // RDebug::Print(_L("CGeneralsPlugin::StopSampling() - Stopping sampler LDD"));
	iGeneralsSampler.StopSampling();
	// RDebug::Print(_L("CGeneralsPlugin::StopSampling() - Sampler LDD stopped"));
	
	// check if bufferhandler has died
	if(iBufferHandler)
		{
		// RDebug::Print(_L("CGeneralsPlugin::StopSampling() - Canceling the buffer handler"));
		iBufferHandler->Cancel();
		delete iBufferHandler;
		iBufferHandler = NULL;
		}	
	// set enabled
    iEnabled = EFalse;
    // RDebug::Print(_L("CGeneralsPlugin::StopSampling() - exit"));
	return KErrNone;
    }



