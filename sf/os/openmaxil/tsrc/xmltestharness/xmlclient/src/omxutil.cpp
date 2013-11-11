/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "omxutil.h"

void OMXUtil::ErrorDes(OMX_ERRORTYPE aError, TBuf<64>& aErrorDes)
	{
	switch(aError)
		{
#define DEFERR(x) case x: aErrorDes = _L(#x); break;
        DEFERR(OMX_ErrorNone);
        DEFERR(OMX_ErrorInsufficientResources);
		DEFERR(OMX_ErrorUndefined);
		DEFERR(OMX_ErrorInvalidComponentName);
		DEFERR(OMX_ErrorComponentNotFound);
		DEFERR(OMX_ErrorInvalidComponent);
		DEFERR(OMX_ErrorBadParameter);
		DEFERR(OMX_ErrorNotImplemented);
		DEFERR(OMX_ErrorUnderflow);
		DEFERR(OMX_ErrorOverflow);
		DEFERR(OMX_ErrorHardware);
		DEFERR(OMX_ErrorInvalidState);
		DEFERR(OMX_ErrorStreamCorrupt);
		DEFERR(OMX_ErrorPortsNotCompatible);
		DEFERR(OMX_ErrorResourcesLost);
		DEFERR(OMX_ErrorNoMore);
		DEFERR(OMX_ErrorVersionMismatch);
		DEFERR(OMX_ErrorNotReady);
		DEFERR(OMX_ErrorTimeout);
		DEFERR(OMX_ErrorSameState);
		DEFERR(OMX_ErrorResourcesPreempted);
		DEFERR(OMX_ErrorPortUnresponsiveDuringAllocation);
		DEFERR(OMX_ErrorPortUnresponsiveDuringDeallocation);
		DEFERR(OMX_ErrorPortUnresponsiveDuringStop);
		DEFERR(OMX_ErrorIncorrectStateTransition);
		DEFERR(OMX_ErrorIncorrectStateOperation);
		DEFERR(OMX_ErrorUnsupportedSetting);
		DEFERR(OMX_ErrorUnsupportedIndex);
		DEFERR(OMX_ErrorBadPortIndex);
		DEFERR(OMX_ErrorPortUnpopulated);
		DEFERR(OMX_ErrorComponentSuspended);
		DEFERR(OMX_ErrorDynamicResourcesUnavailable);
		DEFERR(OMX_ErrorMbErrorsInFrame);
		DEFERR(OMX_ErrorFormatNotDetected);
		DEFERR(OMX_ErrorContentPipeOpenFailed);
		DEFERR(OMX_ErrorContentPipeCreationFailed);
		DEFERR(OMX_ErrorSeperateTablesUsed);
		DEFERR(OMX_ErrorTunnelingUnsupported);
		DEFERR(OMX_ErrorKhronosExtensions);
		DEFERR(OMX_ErrorVendorStartUnused);
#undef DEFERR
	default:
		aErrorDes.Format(_L("%u"), aError);
		break;
		}	
	}
