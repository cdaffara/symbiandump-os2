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

#ifndef BAPPEA_PWR_SAMPLER_H
#define BAPPEA_PWR_SAMPLER_H

//#define PWR_SAMPLER_BACKLIGHT

//#include "PIProfilerConfigInternal.h"

// system definitions
#include <w32std.h>
#include <e32std.h>

#include <e32property.h>
#include <HWRMPower.h>
#include <HWRMLight.h>

// user definitions
#include <piprofiler/SamplerPluginInterface.h>
#include <piprofiler/ProfilerGenericClassesUsr.h>

// caption definitions
_LIT8(KPWRShortName, "pwr");
_LIT8(KPWRMediumName, "Power sampler");
_LIT8(KPWRLongName, "Power usage sampler");
_LIT8(KPWRDescription, "Power sampler: \nSampling power consumption on Nokia S60 devices\nHW dep: N/A\nSW dep: S60 3.0\n");

// Minimum allowed sampling interval (in ms). 0 means undefined.
const TInt KMinSampleInterval = 250;
const TInt KReportingPeriodInfinite = 0;

const TUid KGppPropertyCat={0x20201F70};
enum TGppPropertyKeys
    {
    EGppPropertySyncSampleNumber
    };

const TUid KPwrNotifierUid = { 0x2001E5B9 };
class CProfilerPowerListener;

/*
 *
 * PWR sampler plug-in definition
 *
 */

class CPwrPlugin : public CSamplerPluginInterface
{
public:
    static  CPwrPlugin* NewL(const TUid aImplementationUid, TAny* aInitParams);
            ~CPwrPlugin();

    TInt    ResetAndActivateL(CProfilerSampleStream& aStream);
    TInt    StopSampling();
    TBool   Enabled() { return iEnabled; }
    void    SetEnabled(TBool aEnabled);
    TInt    GetSamplerType();

    TInt    CreateFirstSample();

    void    GetAttributesL(CArrayFixFlat<TSamplerAttributes>* aAttributes);
    TInt    SetAttributesL(TSamplerAttributes& aAttributes);
    void    InitiateSamplerAttributesL();

    TInt    ConvertRawSettingsToAttributes(CDesC8ArrayFlat* aSingleSettingArray);

    TInt    DoSetSamplerSettings(CDesC8ArrayFlat* aAllSettings, TDesC8& aSamplerName, TInt aIndex);
    void    SaveSettingToAttributes(const TDesC8& aSetting, TInt aIndex);

    TUid    Id(TInt aSubId) const;

    // subsampler settings, i.e. samplers implemented within a plugin
    // no sub samplers, from CSamplerPluginInterface
    TInt    SubId(TUid aId) const {return KErrNotFound;}
    TInt    GetSubSamplers(TDes* aDes){return KErrNotFound;}

private:
            CPwrPlugin();
    void    ConstructL();

private:
    TUint8                  iVersion[20];
    TPtr8                   iVersionDescriptor;

    TInt                    iSamplerType;

    CProfilerPowerListener* iPowerListener;

    TInt                    iPeriod;
    CArrayFixFlat<TSamplerAttributes>* iSamplerAttributes;
public:
    TUint32*                iSampleTime;
};

#ifdef PWR_SAMPLER_BACKLIGHT
class CProfilerPowerListener : public CBase, public MHWRMBatteryPowerObserver, public MHWRMLightObserver
#else
class CProfilerPowerListener : public CBase, public MHWRMBatteryPowerObserver
#endif
{
public:
    static  CProfilerPowerListener* NewL(CPwrPlugin* aSampler);
            ~CProfilerPowerListener();

private:
            CProfilerPowerListener(CPwrPlugin* aSampler);
    void    ConstructL();

public:
    TInt    StartL(const TDesC8& aBuf);
    TInt    Stop();
    TInt    DisplayNotifierL(const TDesC& aLine1, const TDesC& aLine2, const TDesC& aButton1, const TDesC& aButton2);

    // From MHWRMBatteryPowerObserver
    virtual void PowerMeasurement(TInt aErr, CHWRMPower::TBatteryPowerMeasurementData& aMeasurement);
#ifdef PWR_SAMPLER_BACKLIGHT
    // From MHWRMLightObserver
    virtual void LightStatusChanged(TInt aTarget, CHWRMLight::TLightStatus aStatus);
#endif

private:
    void    Sample();

public:
    TInt                    iPwrSamplingPeriod;
    TInt                    iSampleStartTime;
    // Value that is read from Central Repository and restored after sampling
    TInt                    iOriginalReportingPeriod;

private:

#ifdef PWR_SAMPLER_BACKLIGHT
    TUint8                  iSample[13];
#else
    TUint8                  iSample[12];
#endif

    TUint16                 iNominalCapa;
    TUint16                 iVoltage;
    TUint16                 iCurrent;

    CPwrPlugin*             iSampler;
    CHWRMPower*             iPowerAPI;

#ifdef PWR_SAMPLER_BACKLIGHT
    CHWRMLight*             iLightAPI;
    CHWRMLight::TLightStatus iBackLightStatus;
#endif
};

#endif

