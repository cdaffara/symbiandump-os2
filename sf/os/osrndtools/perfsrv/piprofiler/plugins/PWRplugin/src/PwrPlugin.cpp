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

#include "PwrPlugin.h"
#include <piprofiler/ProfilerTraces.h>

#include <centralrepository.h>
#include <HWRMPower.h>
#include <HWRMLight.h>
#include <hwrm/hwrmpowerdomaincrkeys.h>


// texts for notes
_LIT(KPowerTextLine1, "Power sampler:");
_LIT(KPowerTextLine2, "Failed to start power measurement");
_LIT(KPowerTextErrorSampling, "Error receiving measurement data");
_LIT(KButtonOk, "Ok");

// LITERALS

_LIT8(KSamplingPeriodMs, "sampling_period_ms");

// CONSTANTS
// Use this UID if plugin is PwrPlugin:
const TUid KSamplerPwrPluginUid = { 0x2001E5B9 };

/*
 *
 * class CPwrPlugin implementation
 * 
 */
 
CPwrPlugin* CPwrPlugin::NewL(const TUid /*aImplementationUid*/, TAny* aInitParams)
    {
    LOGTEXT(_L("CPwrPlugin::NewL() - entry"));
    CPwrPlugin* self = new (ELeave) CPwrPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    LOGTEXT(_L("CPwrPlugin::NewL() - exit"));
    return self;
    }

CPwrPlugin::CPwrPlugin() :
    iVersionDescriptor(&(this->iVersion[1]),0,19),
    iSamplerType(PROFILER_USER_MODE_SAMPLER)
    {
    iPeriod = 250;
    iSamplerId = PROFILER_PWR_SAMPLER_ID;
    iEnabled = EFalse;
    iPowerListener = NULL;
    LOGTEXT(_L("CPwrPlugin::CPwrPlugin() - konstruktori"));

    }

void CPwrPlugin::ConstructL() 
    {
    LOGTEXT(_L("CPwrPlugin::ConstructL() - entry"));
    // initiate sampler attributes array
    iSamplerAttributes = new(ELeave) CArrayFixFlat<TSamplerAttributes>(1); // only one sampler

    // insert default attributes to array
    InitiateSamplerAttributesL();

    LOGTEXT(_L("CPwrPlugin::ConstructL() - exit"));
    }

CPwrPlugin::~CPwrPlugin()
    {
    LOGTEXT(_L("CPwrPlugin::~CPwrPlugin() - entry"));
    if(iPowerListener)
        {
        if(Enabled())
            {
            iPowerListener->Stop();
            }
        delete iPowerListener;
        iPowerListener = NULL;
        }

    if(iSamplerAttributes)
        {
        iSamplerAttributes->Reset();
        delete iSamplerAttributes;
        iSamplerAttributes = NULL;
        }

    LOGTEXT(_L("CPwrPlugin::~CPwrPlugin() - exit"));
    }

TUid CPwrPlugin::Id(TInt /*aSubId*/) const
    {
    LOGSTRING2( "CPwrPlugin::Id():0x%X", KSamplerPwrPluginUid.iUid );
    return KSamplerPwrPluginUid;
    }

void CPwrPlugin::InitiateSamplerAttributesL()
    {
    // create TSamplerAttributes
    TSamplerAttributes attr(KSamplerPwrPluginUid.iUid,
            KPWRShortName(),
            KPWRLongName(),
            KPWRDescription(),
            250,
            EFalse,
            EFalse,
            0); 
    this->iSamplerAttributes->AppendL(attr);
    }

void CPwrPlugin::SetEnabled(TBool aEnabled)
    {
    iEnabled = aEnabled;
    }

// returns setting array
void CPwrPlugin::GetAttributesL(CArrayFixFlat<TSamplerAttributes>* aAttributes)
    {
    aAttributes->AppendL(iSamplerAttributes->At(0));
    }

TInt CPwrPlugin::SetAttributesL(TSamplerAttributes& aAttributes)
    {
    TSamplerAttributes attr;

    attr = iSamplerAttributes->At(0);
    // replace the old attribute container
    iSamplerAttributes->Delete(0);
    iSamplerAttributes->InsertL(0, aAttributes);
    return KErrNone;
    }

/* 
 * Method for parsing and transforming text array settings into TSamplerAttributes (per each sub sampler),
 * called by CSamplerController class
 * 
 * @param array of raw text setting lines, e.g. [gpp]\nenabled=true\nsampling_period_ms=1\n
 */
TInt CPwrPlugin::ConvertRawSettingsToAttributes(CDesC8ArrayFlat* aAllSettingsArray)
    {
    // local literals
    _LIT8(KPWRShort, "pwr");

    TInt err(KErrNone);
    TBuf8<16> samplerSearchName;
    samplerSearchName.Copy(KPWRShort);

    // get sampler specific settings  
    err = DoSetSamplerSettings(aAllSettingsArray, samplerSearchName, 0);

    // returns KErrNone if settings found, otherwise KErrNotFound
    return err;
    }

TInt CPwrPlugin::DoSetSamplerSettings(CDesC8ArrayFlat* aAllSettings, TDesC8& aSamplerName, TInt aIndex)
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
void CPwrPlugin::SaveSettingToAttributes(const TDesC8& aSetting, TInt aIndex)
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
            if(en != iSamplerAttributes->At(aIndex).iEnabled)
                {
                iSamplerAttributes->At(aIndex).iEnabled = en;
                }
            }
        else if (aSetting.Left(sepPos).CompareF(KSamplingPeriodMs) == 0)
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

TInt CPwrPlugin::GetSamplerType()
    {
    return iSamplerType;
    }

TInt CPwrPlugin::ResetAndActivateL(CProfilerSampleStream& aStream) 
    {
    LOGTEXT(_L("CPwrPlugin::ResetAndActivate() - entry"));
    // check if sampler enabled
    if(iSamplerAttributes->At(0).iEnabled)
        {
        // create a new listener instance for every trace, destroy it on stop
        iPowerListener = CProfilerPowerListener::NewL(this);

        iStream = &aStream;
        TInt length = this->CreateFirstSample();
        iVersion[0] = (TUint8)length;
        LOGSTRING2("CPwrPlugin::ResetAndActivate() - AddSample, length %d", length);
        TInt ret = this->AddSample(iVersion, length+1, 0);

        LOGSTRING2("CPwrPlugin::ConstructL() - sampling period %d", this->iPeriod);

        HBufC8* iBufRes = HBufC8::NewMaxLC(10);
        TPtr8 iPtrRes = iBufRes->Des();

        // check if sampling rate set to something reasonable, relevant only in SYNC case
        if(iSamplerAttributes->At(0).iSampleRate != -1)
            {
            iPtrRes.Num(iSamplerAttributes->At(0).iSampleRate);
            }
        else
            {
            iPtrRes.Append(KNullDesC8);
            }

        // set disabled
        SetEnabled(ETrue); 

        // activate power listener
        ret = iPowerListener->StartL(iPtrRes);
        LOGTEXT(_L("CPwrPlugin::ResetAndActivate() - exit"));

        CleanupStack::PopAndDestroy();
        
        if(ret != KErrNone)
            return ret;
        }
    return KErrNone;
    }

TInt CPwrPlugin::CreateFirstSample() 
    {
    LOGTEXT(_L("CPwrPlugin::CreateFirstSample - entry"));
    this->iVersionDescriptor.Zero();
    this->iVersionDescriptor.Append(_L8("Bappea_PWR_V"));
    this->iVersionDescriptor.Append(PROFILER_PWR_SAMPLER_VERSION);
    LOGTEXT(_L("CPwrPlugin::CreateFirstSample - exit"));
    return (TInt)(this->iVersionDescriptor.Length());
    }

TInt CPwrPlugin::StopSampling() 
    {
    if(iPowerListener)
        {
        iPowerListener->Stop();
        delete iPowerListener;
        iPowerListener = NULL;
        }

    // set disabled
    SetEnabled(EFalse);

    return KErrNone;
    }


/*
 *
 * class CProfilerPowerListener implementation
 * 
 */

CProfilerPowerListener::CProfilerPowerListener(CPwrPlugin* aSampler) :
    iPwrSamplingPeriod(0),
    iOriginalReportingPeriod(0),
    iNominalCapa(0),
    iVoltage(0), 
    iCurrent(0),
    iPowerAPI(0)
#ifdef PWR_SAMPLER_BACKLIGHT
    ,iLightAPI(0),
    iBackLightStatus(CHWRMLight::ELightStatusUnknown)
#endif

    {
    LOGTEXT(_L("CProfilerPowerListener::CProfilerPowerListener() - konstuktori"));
    this->iSampler = aSampler;
    LOGTEXT(_L("CProfilerPowerListener::CProfilerPowerListener() - konstuktori exit"));
    }

CProfilerPowerListener* CProfilerPowerListener::NewL(CPwrPlugin* aSampler)
    {
    LOGTEXT(_L("CProfilerPowerListener::NewL() - entry"));
    CProfilerPowerListener* self = new (ELeave) CProfilerPowerListener(aSampler);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    LOGTEXT(_L("CProfilerPowerListener::NewL() - exit"));
    return self;
    }

void CProfilerPowerListener::ConstructL()
    {
    LOGTEXT(_L("CProfilerPowerListener::ConstructL() - entry"));
    iSampleStartTime = 0;
    LOGTEXT(_L("CProfilerPowerListener::ConstructL() - exit"));
    }

CProfilerPowerListener::~CProfilerPowerListener() 
    {
    LOGTEXT(_L("CProfilerPowerListener::~CProfilerPowerListener() - entry"));

    if (iPowerAPI)
        {
        delete iPowerAPI;
        iPowerAPI = 0;
        }
#ifdef PWR_SAMPLER_BACKLIGHT
    if (iLightAPI)
        {
        delete iLightAPI;
        iLightAPI = 0;
        }
#endif

    LOGTEXT(_L("CProfilerPowerListener::~CProfilerPowerListener() - exit"));
    }

TInt CProfilerPowerListener::DisplayNotifierL(const TDesC& aLine1, const TDesC& aLine2, const TDesC& aButton1, const TDesC& aButton2)
    {
    RNotifier notifier;
    TRequestStatus stat;

    TInt buttonValue(0);

    User::LeaveIfError(notifier.Connect());

    notifier.Notify(aLine1, aLine2, aButton1, aButton2, buttonValue, stat);
    User::WaitForRequest(stat);

    notifier.Close();
    return buttonValue;
    }


TInt CProfilerPowerListener::StartL(const TDesC8& aBuf)
    {
    LOGTEXT(_L("CProfilerPowerListener::StartL() - entry"));

    // get the property value
    TInt r = RProperty::Get(KGppPropertyCat, EGppPropertySyncSampleNumber, iSampleStartTime);
    if(r != KErrNone)
        {
        LOGSTRING2("CProfilerPowerListener::StartL() - getting iSyncOffset failed, error %d", r);
        }

   // check if given sampling period is valid
    if(aBuf.CompareF(KNullDesC8)!= 0)
        {
        TLex8* lex = new TLex8(aBuf);
        lex->Val(iPwrSamplingPeriod);
        delete lex;
        }
    else
        {
        // set default period
        iPwrSamplingPeriod = 250;
        }

    // Check that sampling period is in allowed range
    if (KMinSampleInterval > 0 && iPwrSamplingPeriod < KMinSampleInterval)
        {
        iPwrSamplingPeriod = KMinSampleInterval;
        }

    LOGSTRING2("CProfilerPowerListener::StartL() - Sampling period %d", iPwrSamplingPeriod);

    // Start monitoring voltage and current
    iPowerAPI = CHWRMPower::NewL();
    iPowerAPI->SetPowerReportObserver(this);

    // Read HWRM reporting settings from central repository
    CRepository* centRep = CRepository::NewL(KCRUidPowerSettings);
    TInt baseInterval(0);
    User::LeaveIfError(centRep->Get(KPowerBaseTimeInterval, baseInterval));
    User::LeaveIfError(centRep->Get(KPowerMaxReportingPeriod, iOriginalReportingPeriod));

    LOGSTRING2("CProfilerPowerListener::StartL() - HWRM base power report interval: %d", baseInterval);
    LOGSTRING2("CProfilerPowerListener::StartL() - Original HWRM max power reporting period: %d", iOriginalReportingPeriod);

    User::LeaveIfError(centRep->Set(KPowerMaxReportingPeriod, KReportingPeriodInfinite));

    // Power reporting interval reading may return too low value sometimes. Minimum value expected to be 250ms.
    if ( baseInterval < KMinSampleInterval )
        {
        baseInterval = KMinSampleInterval;
        LOGSTRING2("CProfilerPowerListener::StartL() - Power report interval too low. Changed to: %d", baseInterval);
        }

    // Power reporting period is multiplier of HWRM base period
    TInt intervalMultiplier = iPwrSamplingPeriod / baseInterval;

    if (intervalMultiplier < 1)
        {
        intervalMultiplier = 1;
        }

    LOGSTRING2("CProfilerPowerListener::StartL() - Reporting period multiplier: %d", intervalMultiplier);

    TRequestStatus status(KRequestPending);
    iPowerAPI->StartAveragePowerReporting(status, intervalMultiplier);
    User::WaitForRequest(status);

    if (status.Int() != KErrNone)
        {
        LOGTEXT(_L("CProfilerPowerListener::StartL() - Failed to initialize power reporting"));

        DisplayNotifierL(KPowerTextLine1, KPowerTextLine2, KButtonOk, KNullDesC);

        return status.Int();
        }

#ifdef PWR_SAMPLER_BACKLIGHT
    // Start monitoring backlight status
    iLightAPI = CHWRMLight::NewL(this);
#endif

    LOGTEXT(_L("CProfilerPowerListener::StartL() - exit"));
    return KErrNone;
    }

void CProfilerPowerListener::Sample()
    {
    LOGTEXT(_L("CProfilerPowerListener::Sample() - entry"));

    TRequestStatus status;
    CHWRMPower::TBatteryConsumptionData consumptionData;

    iPowerAPI->GetBatteryInfo(status, consumptionData);
    User::WaitForRequest(status);

    // Data is valid only if status == KErrNone 
    if (status.Int() != KErrNone)
        {
        LOGSTRING2("CProfilerPowerListener::Sample() - Getting battery info failed with code: ", status.Int());
        iNominalCapa = 0;
        }
    else
        {
        iNominalCapa = consumptionData.iNominalCapacity;
        }

    // Space for GPP sample time        
    //TUint32 sampleTime = iSampler->iStream->iSampler->GetSampleTime();
    TUint32 sampleTime = User::NTickCount() - iSampleStartTime;
    LOGSTRING2("CProfilerPowerListener::Sample() - Sample time: %d", sampleTime);
    LOGSTRING2("CProfilerPowerListener::Sample() - Nominal capacitance: %d", iNominalCapa);
    LOGSTRING2("CProfilerPowerListener::Sample() - Voltage: %d", iVoltage);
    LOGSTRING2("CProfilerPowerListener::Sample() - Current: %d", iCurrent);
#ifdef PWR_SAMPLER_BACKLIGHT
    LOGSTRING2("CProfilerPowerListener::Sample() - Backlight status: %d", (TUint8)iBackLightStatus);
#endif

    iSample[0] = iNominalCapa;
    iSample[1] = iNominalCapa >> 8;
    iSample[2] = iVoltage;
    iSample[3] = iVoltage >> 8;
    iSample[4] = iCurrent;
    iSample[5] = iCurrent >> 8;
    iSample[6] = iCurrent >> 16;
    iSample[7] = iCurrent >> 24;
#ifdef PWR_SAMPLER_BACKLIGHT
    iSample[8] = (TUint8)iBackLightStatus;
    iSample[9] = sampleTime;
    iSample[10] = sampleTime >> 8;
    iSample[11] = sampleTime >> 16;
    iSample[12] = sampleTime >> 24;

    iSampler->AddSample(iSample, 13, 0);
#else
    iSample[8] = sampleTime;
    iSample[9] = sampleTime >> 8;
    iSample[10] = sampleTime >> 16;
    iSample[11] = sampleTime >> 24;

    iSampler->AddSample(iSample, 12, 0);
#endif

    LOGTEXT(_L("CProfilerPowerListener::Sample() - exit"));
    }

TInt CProfilerPowerListener::Stop() 
    {
    LOGTEXT(_L("CProfilerPowerListener::Stop() - entry"));

    if (iPowerAPI)
        {
        TRAPD(err, iPowerAPI->StopAveragePowerReportingL());
        if(err != KErrNone)
            {
            LOGSTRING2("CProfilerPowerListener::Stop() - Failed to stop power reporting: %d", err);
            }
        else
            {
            LOGTEXT(_L("CProfilerPowerListener::Stop() - Stopped power monitoring"));
            }
        delete iPowerAPI;
        iPowerAPI = 0;

        // Restore original value to max sampling period
        CRepository* centRep = 0;
        TRAP(err, centRep = CRepository::NewL(KCRUidPowerSettings));
        if (err != KErrNone)
            {
            LOGSTRING2("CProfilerPowerListener::Stop() - Failed to open Central Repository: %d", err);
            }
        else
            {
            err = centRep->Set(KPowerMaxReportingPeriod, iOriginalReportingPeriod);
            if(err != KErrNone)
                {
                LOGSTRING2("CProfilerPowerListener::Stop() - Failed to restore max sampling period: %d", err);
                }
            }
        }
#ifdef PWR_SAMPLER_BACKLIGHT
    if (iLightAPI)
        {
        delete iLightAPI;
        iLightAPI = 0;
        }
#endif

    LOGTEXT(_L("CProfilerPowerListener::Stop() - exit"));
    return KErrNone;
    }

void CProfilerPowerListener::PowerMeasurement(TInt aErr, CHWRMPower::TBatteryPowerMeasurementData& aMeasurement)
    {
    LOGTEXT(_L("CProfilerPowerListener::PowerMeasurement - entry"));

    if (aErr == KErrNone)
        {
        LOGSTRING3("CProfilerPowerListener::PowerMeasurement - Previous values - Voltage: %d Current: %d", iVoltage, iCurrent);
        iVoltage = aMeasurement.iAverageVoltage;
        iCurrent = aMeasurement.iAverageCurrent;
        LOGSTRING3("CProfilerPowerListener::PowerMeasurement - New values - Voltage: %d Current: %d", iVoltage, iCurrent);

        this->Sample();
        }
    else
        {
        LOGSTRING2("CProfilerPowerListener::PowerMeasurement - Failed with error code: %d", aErr);
        DisplayNotifierL(KPowerTextLine1, KPowerTextErrorSampling, KButtonOk, KNullDesC);
        }
    LOGTEXT(_L("CProfilerPowerListener::PowerMeasurement - exit"));
    }

#ifdef PWR_SAMPLER_BACKLIGHT
void CProfilerPowerListener::LightStatusChanged(TInt aTarget, CHWRMLight::TLightStatus aStatus)
    {
    LOGTEXT(_L("CProfilerPowerListener::LightStatusChanged - entry"));
    LOGSTRING3("CProfilerPowerListener::LightStatusChanged - Target: %d Status: %d", aTarget, aStatus);

    if (aTarget == CHWRMLight::EPrimaryDisplay)
        {
        LOGSTRING2("CProfilerPowerListener::LightStatusChanged - Previous light status: %d", iBackLightStatus);
        iBackLightStatus = aStatus;
        LOGSTRING2("CProfilerPowerListener::LightStatusChanged - New light status: %d", iBackLightStatus);

        this->Sample();
        }
    LOGTEXT(_L("CProfilerPowerListener::LightStatusChanged - exit"));
    }
#endif
