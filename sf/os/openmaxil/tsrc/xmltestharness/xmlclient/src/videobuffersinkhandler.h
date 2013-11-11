/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef VIDEOBUFFERSINKHANDLER_H
#define VIDEOBUFFERSINKHANDLER_H

#include <e32base.h>
#include <e32msgqueue.h>
#include <f32file.h>
#include <openmax/il/extensions/omxilsymbianbuffersextensions.h>
#include <multimedia/mmfbuffershared.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>

NONSHARABLE_CLASS(CVideoBufferSinkHandler) : public CActive
	{
public:
	~CVideoBufferSinkHandler();
	static CVideoBufferSinkHandler* NewL(TDesC& aFilename, OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA& aChunkDataMsg, const OMX_SYMBIAN_PARAM_BUFFER_SIZE& aBufferSize, OMX_PARAM_PORTDEFINITIONTYPE& portDef);
	    
	
public: 
	void Start();
	
private:
	CVideoBufferSinkHandler(TDesC& aFilename);
	void ConstructL(OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA& aChunkDataMsg, const OMX_SYMBIAN_PARAM_BUFFER_SIZE& aBufferSize, OMX_PARAM_PORTDEFINITIONTYPE& portDef);
		
	// From CActive
	void RunL();
	void DoCancel();
	
private:
	RMsgQueue<TFilledBufferHeaderV2> iReceiveBuffer;
	RMsgQueue<TFilledBufferHeaderV2> iSendBuffer;
	TMMSharedChunkBufConfig iBufConfig;
	TPtr8 iWriteDes;
	

	RChunk iBufferChunk;
	TBuf<KMaxFileName> iFileName;
	RFile iFileSink;
	RFs iFs;
	};

#endif // VIDEOBUFFERSINKHANDLER_H
