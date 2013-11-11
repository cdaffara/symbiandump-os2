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

inline OMX_ERRORTYPE
COmxILPortManager::GetPortIndexFromOmxStruct(
	TAny*& apComponentParameterStructure,
	OMX_U32& aIndex) const
	{
	return GetPortIndexFromOmxStruct(
		const_cast<const TAny*&>(apComponentParameterStructure), aIndex);
	}


inline OMX_ERRORTYPE
COmxILPortManager::GetPortIndexFromOmxStruct(
	const TAny*& apComponentParameterStructure,
	OMX_U32& aIndex) const
	{
	OMX_U32* const pPortIndex =
		reinterpret_cast<OMX_U32*>(
			const_cast<TAny*&>(apComponentParameterStructure)) +
		sizeof(OMX_U32) / sizeof(OMX_U32) +
		sizeof(OMX_VERSIONTYPE) / sizeof(OMX_U32);

	if (OMX_ErrorNone != CheckPortIndex(*pPortIndex))
		{
		return OMX_ErrorBadPortIndex;
		}

	aIndex = *pPortIndex;

	return OMX_ErrorNone;

	}


/**
   Checks that a port index is consistent with port information found in the
   port's OMX_PORT_PARAM_TYPE structure, i.e., the number of ports in the
   component.

   @param aPortIndex The port index to be checked.

   @return OMX_ErrorBadPortIndex in case of error. OMX_ErrorNone otherwise.
*/
inline OMX_ERRORTYPE
COmxILPortManager::CheckPortIndex(OMX_U32 aPortIndex) const
	{

	if (aPortIndex >= iAllPorts.Count())
		{
		return OMX_ErrorBadPortIndex;
		}

	return OMX_ErrorNone;

	}

