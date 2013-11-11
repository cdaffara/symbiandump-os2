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


#ifndef __SAMPLERCONTROLLER_H__
#define __SAMPLERCONTROLLER_H__

// system includes
#include <utf.h>
#include <e32cmn.h>


// The user-interface to the sampling device driver sued by the profiling engine
// user includes
#include <piprofiler/ProfilerConfig.h>
#include <piprofiler/ProfilerVersion.h>
#include <piprofiler/SamplerPluginInterface.h>

#include "SamplerPluginLoader.h"

/*
 *	Forward declarations
 */
class TBapBuf;
class CProfilerSampleStream;
class CSamplerPluginLoader;
class CSamplerPluginInterface;
class MSamplerControllerObserver;
/**
 * The sampler controller for handling the actual sampler plugins.
 */

class CSamplerController : public CBase, MSamplerPluginLoadObserver
	{
	
public:
	static CSamplerController* NewL(CProfilerSampleStream& aStream);
	void ConstructL();
	
	CSamplerController(CProfilerSampleStream& aStream);
	~CSamplerController();
	
	/** 
	 * 
	 * Methods for user mode sampling
	 * 
	 **/
	
	/** Initialise the user mode samplers **/					
	void InitialiseSamplerListL();
		 
	/* Overrider of MSamplerPluginLoaderObserver class **/
	void HandlePluginLoaded( KSamplerPluginLoaderStatus aStatus );
	
	/* returns the name matching plugin Uid */
	TUid GetPluginUID(TDesC8& name);

	/** Start enabled samplers **/				
	void StartSamplerPluginsL();

	/** Stop enabled samplers **/				
	TInt StopSamplerPlugins();

public:

	CSamplerPluginInterface* GetPlugin(TUid aUid);

	TInt GetPluginList(TDes* aList);
	
    TInt SetSamplerSettingsL(TInt aUid, TSamplerAttributes& aAttributes);

    void GetSamplerAttributesL(CArrayFixFlat<TSamplerAttributes>* aAttributes);
	
	TInt UpdateSavedSamplerAttributesL(CDesC8ArrayFlat* aSavedLineArray, CArrayFixFlat<TSamplerAttributes>* aAttributes);
	
	/*
	 * Compose all sampler (sampler or sub-sampler) attributes as text array for saving to settings file, 
	 * called by CSamplerController (and Profiler Engine) 
	 * 
	 * @param aFile settings file where to write the settings 
	 */
	void ComposeAttributesToSettingsFileFormat(RFile& aFile, CArrayFixFlat<TSamplerAttributes>* aAttributes);
	
	/*
	 * Compose all sampler (sampler or sub-sampler) attributes as text array for saving to settings file, 
	 * called by CSamplerController (and Profiler Engine) 
	 * 
     * @param aFile settings file where to write the settings 
	 * @param aAttrArray is container for saving the text to 
	 */
	void ComposeSettingsText(RFile& aFile, CArrayFixFlat<TSamplerAttributes>* aAttrArray);
	
	void SetObserver(MSamplerControllerObserver* aObserver);
	
	void     Str2Bool(const TDesC8& aBuf, TBool& aValue);
    
    void     Str2Int(const TDesC8& aBuf, TInt& aValue);
    
    void     Str2Int(const TDesC8& aBuf, TUint32& aValue);
    
    TBuf8<16> Bool2Str(const TBool& aValue);
    
    TBuf8<16> Int2Str(const TInt& aValue);
public:

    CArrayPtrFlat<CSamplerPluginInterface>* iPluginArray;
    
    // Asynchronous loader for the sampler plug-ins.
    CSamplerPluginLoader*     iPluginLoader;
    
    // UID of the selected plugin in the container's lbx.
    TUid                      iSelectedPluginUid;
    
    // shared sample stream for all plugin samplers
    CProfilerSampleStream&    iStream;

private:
	MSamplerControllerObserver* iObserver;
};

/**
* Interface for SamplerPluginLoader observer. MSamplerPluginLoadObserver gets
* notifications when plugins are loaded.
*
* @lib ProfilerEngine.exe/.lib
* @since Series60_30.1
*/

class MSamplerControllerObserver
    {
    public: // New

        //CSamplerController calls this function when each plug-in is loaded or
        //loading is finished..
        
        virtual void HandleSamplerControllerReadyL() = 0;
        virtual void HandleError(TInt aError) = 0;
    };
    
#endif	// __SAMPLERCONTROLLER_H__
