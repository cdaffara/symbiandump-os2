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


#ifndef PROFILERGENERICCLASSESUSR_H
#define PROFILERGENERICCLASSESUSR_H

#include <piprofiler/ProfilerConfig.h>
#include <piprofiler/ProfilerVersion.h>
#include <piprofiler/ProfilerGenericClassesCommon.h>
#include <piprofiler/PluginSampler.h>
#include <piprofiler/WriterPluginInterface.h>

#include <e32cmn.h>
#include <e32std.h>
#include <f32file.h>


/*
 *	
 *	Class MProfilerBufferHandlerObserver definition
 *	
 */
class MProfilerBufferHandlerObserver
	{
public:
	virtual TBapBuf* GetNextFreeBuffer() = 0;
	virtual void AddToFilledBuffers(TBapBuf* aFilledBuffer) = 0;
	};

/*
 *	
 *	Class CProfilerBufferHandler definition
 *	
 */
// forward declarations
class CProfilerSampleStream;

class CProfilerBufferHandler : public CActive
	{
public:
	static CProfilerBufferHandler* CProfilerBufferHandler::NewL(CProfilerSampleStream& aStream, RPluginSampler& aSampler);
	CProfilerBufferHandler(CProfilerSampleStream& aStream, RPluginSampler& aSampler);
	~CProfilerBufferHandler();
	
	void 		ConstructL();
	
	// for plugins to start receiving trace data
	void		StartReceivingData();
private:
	void		RunL();
	TInt        RunError(TInt aError);
	void		DoCancel();
	void        HandleEndOfStream();
public:
	TInt					iFinished;
private:
	RPluginSampler&			iSampler;
	TBapBuf*				iBufferInProcess;
	TBool					iEndOfStreamDetected;
	
	CProfilerSampleStream&     iObserver;
	};

/*
 *	
 *	Class CProfilerSampleStream definition
 *	
 */
class CWriterPluginInterface;

class CProfilerSampleStream : public CBase, MProfilerBufferHandlerObserver
{
public:
	static CProfilerSampleStream* CProfilerSampleStream::NewL(TInt aBufSize);
	~CProfilerSampleStream();

	void 		ConstructL();

	void 		Finalise();
	void 		EmptyBuffers();
	inline void		AddToFreeBuffers(TBapBuf* freeBuffer);
	void        SetWriter(CWriterPluginInterface& aWriter);
	
	// from MProfilerBufferHandlerObserver
	TBapBuf* 	GetNextFreeBuffer();
	void        AddToFilledBuffers(TBapBuf* aFilledBuffer);
	inline TBapBuf*    GetNextFilledBuffer();
	void        ResetBuffers();
	void        InitialiseBuffers();
	inline void        NotifyWriter();
private:
	CProfilerSampleStream(TInt aBufSize);
	
	
public:
    TInt					iFinished;
    CWriterPluginInterface* iWriter;
private:
    TInt					iBufferSize;
    TBapBuf*				iFreeBuffers;
    TBapBuf*                iFilledBuffers;
};

#include <piprofiler/ProfilerGenericClassesUsr.inl>

#endif
