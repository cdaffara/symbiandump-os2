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


#ifndef PROFILER_GENERALSECOM_SAMPLER_H
#define PROFILER_GENERALSECOM_SAMPLER_H

#include <w32std.h>

#include "GeneralsDriver.h"
#include "GeneralsConfig.h"
#include <piprofiler/SamplerPluginInterface.h>
#include <piprofiler/ProfilerGenericClassesUsr.h>

// constants 
const TUint KDefaultOutputCombination = 3;

const TUint KSubSamplerCount = 5;

// parent itself
_LIT(KGENShortName, "gen");
_LIT(KGENMediumName, "Generic samplers plug-in");
_LIT(KGENLongName, "Generic samplers plug-in");

// gpp caption definitions
_LIT8(KGPPShortName, "gpp");
#ifdef CARBIDE_NAMES
_LIT8(KGPPLongName, "Address/Thread sampling");
#else
_LIT8(KGPPLongName, "CPU load sampler");
#endif
_LIT8(KGPPDescription, "CPU load sampler\nSampling thread and process load\nHW dep: N/A\nSW dep: S60 3.0\n");

// gfc caption definitions
_LIT8(KGFCShortName, "gfc");
#ifdef CARBIDE_NAMES
_LIT8(KGFCLongName, "Function call sampling");
#else
_LIT8(KGFCLongName, "Function call sampler");
#endif
_LIT8(KGFCDescription, "Function call sampler\nCapturing function call info\nHW dep: N/A\nSW dep: S60 3.0\n");

// itt caption definitions
_LIT8(KITTShortName, "itt");
#ifdef CARBIDE_NAMES
_LIT8(KITTLongName, "Dynamic binary support");
#else
_LIT8(KITTLongName, "Dynamic binary sampler");
#endif
_LIT8(KITTDescription, "Dynamic binary sampler\nTracing dynamically loaded binaries, e.g. from ROFS\nHW dep: N/A\nSW dep: S60 3.0\n");

// mem caption definitions
_LIT8(KMEMShortName, "mem");
#ifdef CARBIDE_NAMES
_LIT8(KMEMLongName, "Memory usage sampler");
#else
_LIT8(KMEMLongName, "Memory trace sampler");
#endif
_LIT8(KMEMDescription, "Memory trace sampler\nTracing memory, i.e. stack and chunk usage\nHW dep: N/A\nSW dep: S60 3.0\n");

// pri caption definitions
_LIT8(KPRIShortName, "pri");
#ifdef CARBIDE_NAMES
_LIT8(KPRILongName, "Thread priority sampling");
#else
_LIT8(KPRILongName, "Priority trace sampler");
#endif
_LIT8(KPRIDescription, "Priority trace sampler\nTracing thread priorities\nHW dep: N/A\nSW dep: S60 3.0\n");


// forward definitions
class CConfigInfoArray;
class CProfilerBufferHandler;
class CProfilerSampleStream;
class TSamplerAttributes;

class CGeneralsPlugin : public CSamplerPluginInterface
{
public:
	static CGeneralsPlugin* NewL(const TUid aImplementationUid, TAny* /*aInitParams*/);
	~CGeneralsPlugin();

	TUint32 GetSampleTime();
	
	/* 
	 * Sub sampler specific functions
	 * 
	 */
	
	// from CSamplerPluginInterface
	TInt 	ResetAndActivateL(CProfilerSampleStream& aStream);
	TInt 	StopSampling();
    TBool   Enabled() { return iEnabled; }

	TUid 	Id( TInt aSubId ) const;
	TInt 	SubId( TUid aSubId ) const;	// internal

	void    GetAttributesL(CArrayFixFlat<TSamplerAttributes>* aAttributes);
	TInt    SetAttributesL(TSamplerAttributes& aAttributes);
	void    InitiateSamplerAttributesL();
	
	TInt    ConvertRawSettingsToAttributes(CDesC8ArrayFlat* aSingleSettingArray);
	TInt    DoSetSamplerSettings(CDesC8ArrayFlat* aAllSettings, TDesC8& aSamplerName, TInt aIndex);
	void    SaveSettingToAttributes(const TDesC8& aSetting, TInt aIndex);

	TInt 	GetSamplerType();
	
	void    InstallStreamForActiveTraces(RGeneralsSampler& sampler, CProfilerSampleStream& aStream);
	
private:
	CGeneralsPlugin();
	void ConstructL();

	TInt InitiateSamplerL();
	TInt CleanSampler();
    void SetSettingsToSamplers();
	
private:
	TUint8						iVersion[20];
	TPtr8						iVersionDescriptor;
	
	RGeneralsSampler 			iGeneralsSampler;

	CProfilerBufferHandler*		iBufferHandler;
	
	CArrayFixFlat<TSamplerAttributes>* iSamplerAttributes;

	TBuf8<9>                    iSearchTexts;
public:
	TUint32* 					iSampleTime;
};

#endif
