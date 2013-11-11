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



#ifndef VIDEOBUFFERHANDLERMPEG4_H
#define VIDEOBUFFERHANDLERMPEG4_H

#include <e32base.h>
#include <e32msgqueue.h>
#include <f32file.h>
#include <openmax/il/extensions/omxilsymbianbuffersextensions.h>
#include <multimedia/mmfbuffershared.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>
#include <multimedia/t_videoframesupplier.h>
#include "videobufferhandler.h"

const TInt KVideoBufferLength=102400;

typedef struct
    {
    // Maybe these should be 64-bit variables for large video files.
    TInt iPosition;
    TInt iLength;
    }
TFrameLocation;

NONSHARABLE_CLASS(CVideoBufferHandlerMPEG4) : public CVideoBufferHandler
	{
public:
    
	~CVideoBufferHandlerMPEG4();
	static CVideoBufferHandlerMPEG4* NewL(TDesC& aFilename, OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA& aChunkDataMsg, const OMX_SYMBIAN_PARAM_BUFFER_SIZE& aBufferSize, OMX_PARAM_PORTDEFINITIONTYPE& portDef);
	
private:
	CVideoBufferHandlerMPEG4(TDesC& aFilename);
	void ConstructL(OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA& aChunkDataMsg, const OMX_SYMBIAN_PARAM_BUFFER_SIZE& aBufferSize, OMX_PARAM_PORTDEFINITIONTYPE& portDef);

	void SetChunkDetailAndOpenL(TInt aChunkHandleId, TUint64 aThreadId);

	// From CActive
	void RunL();
	
private:
    CVideoFrameSupplier* iFrameSupplier;


	};

#endif // VIDEOBUFFERHANDLERMPEG4_H
