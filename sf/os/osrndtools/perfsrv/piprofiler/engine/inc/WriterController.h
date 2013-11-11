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


#ifndef __WRITERCONTROLLER_H__
#define __WRITERCONTROLLER_H__

// system includes
#include <utf.h>
#include <e32cmn.h>


// The user-interface to the sampling device driver sued by the profiling engine
// user includes
#include <piprofiler/ProfilerConfig.h>
#include <piprofiler/ProfilerVersion.h>
#include <piprofiler/WriterPluginInterface.h>

#include "WriterPluginLoader.h"

/*
 *	Forward declarations
 */
class TBapBuf;
class CWriterPluginLoader;
class CWriterPluginInterface;

/**
 * The writer controller for handling the available Data writer plugins.
 */
  
class CWriterController : public CBase, MWriterPluginLoadObserver
	{
	
public:
	static CWriterController* NewL(CProfilerSampleStream& aStream);
	void ConstructL();
	
	CWriterController(CProfilerSampleStream& aStream);
    ~CWriterController();
	
	/** 
	 * 
	 * Methods for user mode sampling
	 * 
	 **/
	
	/** Initialise the user mode samplers **/					
	void InitialiseWriterListL();
	
	/* Overrider of MSamplerPluginLoaderObserver class **/
	void HandlePluginLoaded( KWriterPluginLoaderStatus /*aStatus*/ );
	
	/* returns the matching plugin Uid */
	TUid GetPluginUID(TInt traceId);
	
	CWriterPluginInterface* GetActiveWriter();
	
	 /** Returns the sampler type (kernel/user) **/
	TUint32 GetWriterType(TUint32 samplerId);

	/** Set additional settings for a trace **/			
	TInt AdditionalUserTraceSettings(TInt traceId, TInt settings);

	/** Set data stream for samplers **/
	//void SetSampleStream(RProfilerSampleStream* iStream);

	TInt StartSelectedPlugin();

	void StopSelectedPlugin();

	/** Set selected plugin active **/
	void SetPluginActive(TUid uid, const TWriterPluginValueKeys aKey);

	TInt SetPluginSettings(TUid aUid, TDes& aDes);
	
	/** Get settings for a specific plugin **/
	void GetPluginSettings(TUid uid, TDes& aVal);
	
	/** Request stream read **/
	//void FillThisStreamBuffer(TBapBuf* aBuffer, TRequestStatus& aStatus);
	
	CWriterPluginInterface* GetPlugin(TUid aUid);
	
	CArrayPtrFlat<CWriterPluginInterface>* GetPluginList();
	
	void InitialisePluginStream();
public:
    CArrayPtrFlat<CWriterPluginInterface>* iPluginArray;
    
    // Asynchronous loader for the writer plug-ins.
    CWriterPluginLoader* iPluginLoader;
    
    // UID of the selected plugin in the container's lbx.
    TUid iSelectedPluginUid;
    
    CProfilerSampleStream& iStream;
};

	
#endif	// __WRITERCONTROLLER_H__
