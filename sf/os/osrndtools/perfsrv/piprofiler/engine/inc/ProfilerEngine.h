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


#ifndef __PROFILERENGINE__
#define __PROFILERENGINE__

#include <e32base.h>
#include <e32svr.h>
#include <e32property.h> 

#include <piprofiler/SamplerPluginInterface.h>
#include <piprofiler/WriterPluginInterface.h>
#include <piprofiler/ProfilerSession.h>
#include <piprofiler/ProfilerAttributes.h>	// internal settings format presentations 

#include "SamplerController.h"
#include "WriterController.h"
#include "ProfilerErrorChecker.h"
#include "ProfilerTimer.h"

// CONSTANTS
const TInt KProfilerPrefixMaxLength = 32;
const TInt KSettingsFileSize = 4096;

/*
 * This is an internal interface to the profiling engine which allows
 * an additional control DLL to be loaded, replacing the profiler's
 * default console UI
 */

/**
 * Implementation class providing access to the profiler engine
 */

class MProfilerEngine
{
public:
	virtual TInt				ControlL(TInt aCommand) =0;
	virtual TInt				ControlDataL(TInt aCommand, TAny* data1 = 0, TAny* data2 = 0) = 0; 
	
	virtual TInt                GetSamplerAttributesL(const RMessage2& aMessage) = 0;
    virtual TInt                GetSamplerAttributeCountL(const RMessage2& aMessage) = 0;
	virtual TInt                SetSamplerAttributesL(const RMessage2& aMessage) = 0;
	virtual TInt                GetGeneralAttributesL(const RMessage2& aMessage) = 0;
	virtual TInt                SetGeneralAttributesL(const RMessage2& aMessage) = 0;
    virtual TInt                 RefreshStatus(const RMessage2& aMessage) = 0;
	virtual RProfiler::TSamplerState 	State() const =0;
};

/**
 * The interface that the extra controller must implement to access
 * the profiler.
 */
class MProfilerController
{
	public:
		/** Release the controller from the profiler. This is invoked when the profiler is unloading. */
		virtual void				Release() = 0;
		/** Ask the profiler to change state */
		inline TInt					ControlL(TInt aCommand) const;
		/** Ask the profiler to change state */
		inline TInt					ControlDataL(TInt aCommand, TAny* data1 = 0, TAny* data2 = 0) const;
		
		inline TInt                 GetSamplerAttributesL(const RMessage2& aMessage) const;
        inline TInt                 SetSamplerAttributesL(const RMessage2& aMessage) const;
        inline TInt                 GetGeneralAttributesL(const RMessage2& aMessage) const;
        inline TInt                 SetGeneralAttributesL(const RMessage2& aMessage) const;
        inline TInt                 GetSamplerAttributeCountL(const RMessage2& aMessage) const;
        inline TInt                 RefreshStatus(const RMessage2& aMessage) const;
		/* Query the profiler state */
		inline RProfiler::TSamplerState	GeTComand() const;
	protected:
		inline						MProfilerController(MProfilerEngine& aEngine);
	private:
		MProfilerEngine& iEngine;
};

/** The signature of ordinal 1 in the controller DLL */
typedef MProfilerController* (*TProfilerControllerFactoryL)(TInt aPriority, MProfilerEngine& aEngine);
/** The second UID required by the controller DLL */
const TUid KUidProfilerKeys={0x1000945c};

inline MProfilerController::MProfilerController(MProfilerEngine& aEngine)
	:iEngine(aEngine)
{

}

inline TInt MProfilerController::ControlL(TInt aCommand) const
{
	return iEngine.ControlL(aCommand);
}

inline TInt MProfilerController::ControlDataL(TInt aCommand,TAny* data1, TAny* data2) const
{
	return iEngine.ControlDataL(aCommand,data1,data2);
}

inline TInt MProfilerController::GetSamplerAttributesL(const RMessage2& aMessage) const
{
    return iEngine.GetSamplerAttributesL(aMessage);
}

inline TInt MProfilerController::SetSamplerAttributesL(const RMessage2& aMessage) const
{
    return iEngine.SetSamplerAttributesL(aMessage);
}

inline TInt MProfilerController::GetGeneralAttributesL(const RMessage2& aMessage) const
{
    return iEngine.GetGeneralAttributesL(aMessage);
}

inline TInt MProfilerController::GetSamplerAttributeCountL(const RMessage2& aMessage) const
{
    return iEngine.GetSamplerAttributeCountL(aMessage);
}

inline TInt MProfilerController::SetGeneralAttributesL(const RMessage2& aMessage) const
{
    return iEngine.SetGeneralAttributesL(aMessage);
}

inline TInt MProfilerController::RefreshStatus(const RMessage2& aMessage) const
{
    return iEngine.RefreshStatus(aMessage);
}

inline RProfiler::TSamplerState MProfilerController::GeTComand() const
{
	return iEngine.State();
}

class CSamplerController;
class CWriterController;
class MSamplerControllerObserver;
class MProfilerErrorObserver;

class CProfiler : public CBase, private MProfilerEngine, 
    MSamplerControllerObserver, 
    MProfilerErrorObserver,
    MProfilerTimerObserver
{
	public:
		
		static CProfiler*	NewLC(const TDesC& aSettingsFile);

		/**
		 * Method for control commands, i.e. start, stop and exit 
		 * 
		 * @param aCommand command to be parsed and executed
		 * @return TInt KErrNone if succeed, else error code
		 */
		TInt				ControlL(TInt aCommand);

		/**
         * Method for control data, e.g. settings
         * 
         * @param aCommand command to be parsed and executed
         * @param value1 can contain any value, integer or string, depends on use case 
         * @param value2 can contain any value, integer or string, depends on use case 
         * @return TInt KErrNone if succeed, else error code
         */
		TInt				ControlDataL(TInt aCommand, TAny* value1 = 0, TAny* value2 = 0);	

		// setting attributes manipulation
		TInt  GetGeneralAttributesL(const RMessage2& aMessage);
		TInt  GetSamplerAttributesL(const RMessage2& aMessage);
		TInt  SetGeneralAttributesL(const RMessage2& aMessage);
		TInt  SetSamplerAttributesL(const RMessage2& aMessage);
		TInt  GetSamplerAttributeCountL(const RMessage2& aMessage);
		TInt  RefreshStatus(const RMessage2& /*aMessage*/);

		// from CProfilerErrorChecker
		void  HandleSamplerControllerReadyL();
		void  NotifyRequesterForSettingsUpdate();
		void  HandleProfilerErrorChangeL( TInt aError );
		
		// from MProfilerTimerObserver
		void HandleTimerExpiresL(TInt aError);
	    
		void 				Finalise();
		CProfilerSampleStream* GetSamplerStream();
	    void  HandleError(TInt aErr);
	    static TBool CheckLocationSanity(RFs& fs, const TDesC8& aLocation);
private:
							CProfiler(const TDesC& aSettingsFile);
							~CProfiler();
		void				ConstructL();
		TInt 				LoadSettingsL(/*const TDesC& configFile*/);		
		void                DoGetValueFromSettingsArray(CDesC8ArrayFlat* aLineArray, const TDesC8& aAttribute, TDes8& aValue);
		void                DoGetValueFromSettingsArray(CDesC8ArrayFlat* aLineArray, const TDesC8& aAttribute, TInt& aValue);
		void                UpdateSavedGeneralAttributes(CDesC8ArrayFlat* aSavedAttributes);
		template<class T> CBufFlat* ExternalizeLC(const T& aElements);
		
		RProfiler::TSamplerState	State() const;

		void DrainSampleStream();
		void InstallStreamForActiveTraces(CSamplerPluginInterface& aSampler, CWriterPluginInterface& aWriter, TDesC& totalPrefix);
		void SaveSettingsL();
		
		TInt HandleGeneralSettingsChange();
		TInt CheckOldProfilerRunning();
public:
        // trace file settings
		TBuf8<KProfilerPrefixMaxLength>	iFilePrefix;
		TBuf8<KProfilerPrefixMaxLength>	iDriveLetter;
		TBuf8<KProfilerPrefixMaxLength*2> iTotalPrefix;

		TBuf<256>                       iSettingsFileLocation;
		
		MProfilerController*			iServer;
		RProfiler::TSamplerState		iState;
		
		CProfilerSampleStream* 			iUserStream;
		
		// plug-in controllers
		CWriterController*				iWriterHandler;
		CSamplerController*				iSamplerHandler;

		// setting attribute containers
		TGeneralAttributes				iGeneralAttributes;
		CArrayFixFlat<TSamplerAttributes>*	iDefaultSamplerAttributesArray;
		
		// temporary settings file array container
		CDesC8ArrayFlat*                iSavedLineArray;	
		TInt                            iSavedLinesCount;
	    // saved settings, add extra 1 byte space to end buffer with a '\n'
	    TBuf8<KSettingsFileSize + 1>   iSettingsBuffer;   

	    // P&S status properties
		RProperty						iEngineStatus;
        RProperty                       iUpdateStatus;
	    TBuf<128>                       iFileNameStream;
private:
        TBool                           iSettingsFileLoaded;
        CProfilerErrorChecker*          iErrorChecker;
        CProfilerTimer*                 iTimer;
};

#include <piprofiler/ProfilerGenericClassesUsr.h>
#endif	// __PROFILERENGINE__

