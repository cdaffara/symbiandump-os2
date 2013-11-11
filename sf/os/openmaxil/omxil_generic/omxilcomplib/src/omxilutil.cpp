// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


/**
 @file
 @internalComponent
*/


#include <openmax/il/common/omxilutil.h>
#include <openmax/il/common/omxilspecversion.h>

/**
   This method checks that the size of an OpenMAX IL data structure is
   correct. It also checks that the OpenMAX IL version stated in the structure
   is the same as the one returned by TOmxILSpecVersion() (at the time of
   writting this is 1.1.1). In this version of the component framework,
   backward compatibility in not supported for OpenMAX IL structures.

   @param apHeader An OpenMAX IL data structure.

   @param aSize sizeof(OMXIL structure)

   @return OMX_ErrorVersionMismatch if the version is not
   1.1.1. OMX_ErrorBadParameter if versions are the same but the nSize is
   different. OMX_ErrorNone otherwise
 */
EXPORT_C OMX_ERRORTYPE
TOmxILUtil::CheckOmxStructSizeAndVersion(OMX_PTR apHeader, OMX_U32 aSize)
	{

	if (!apHeader || aSize == 0)
		{
		return OMX_ErrorBadParameter;
		}

#ifdef _OMXIL_COMMON_SPEC_VERSION_CHECKS_ON

	OMX_U32* const pStructSize =
		reinterpret_cast<OMX_U32*>(apHeader);

    // In an OpenMAX IL structure the nSize and nVersion fields are used to
    // detect the difference in spec versions.
	OMX_VERSIONTYPE* const pOmxVersion =
		reinterpret_cast<OMX_VERSIONTYPE*>(
			pStructSize + (sizeof(OMX_U32)/sizeof(OMX_U32)));

	if ( (TOmxILSpecVersion() != *pOmxVersion)&&(TOmxILVersion(OMX_VERSION_MAJOR, OMX_VERSION_MINOR, 1, OMX_VERSION_STEP) != *pOmxVersion) )

		{
		return OMX_ErrorVersionMismatch;
		}

	// If the spec versions are the same, then the struct sizes must be the
	// same too...
	if (*pStructSize != aSize)
		{
		return OMX_ErrorBadParameter;
		}

#endif // _OMXIL_COMMON_SPEC_VERSION_CHECKS_ON

	return OMX_ErrorNone;

	}

/**
   This method simply resets the following fields in the OMX IL 1.1.x buffer header:
   nFilledLen
   hMarkTargetComponent
   pMarkData
   nTickCount
   nTimeStamp
   nFlags

   @param apBufferHeader An OpenMAX IL buffer header structure.

 */
EXPORT_C void
TOmxILUtil::ClearBufferContents(
	OMX_BUFFERHEADERTYPE* apBufferHeader)
	{

	if (!apBufferHeader)
		{
		return;
		}

	apBufferHeader->nFilledLen			 = 0;
	apBufferHeader->hMarkTargetComponent = 0;
	apBufferHeader->pMarkData			 = 0;
	apBufferHeader->nTickCount			 = 0;
	apBufferHeader->nTimeStamp			 = 0;
	apBufferHeader->nFlags				 = 0;

	}
