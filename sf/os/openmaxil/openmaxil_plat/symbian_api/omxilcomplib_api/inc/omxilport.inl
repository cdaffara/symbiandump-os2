// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

template<typename T>
inline OMX_ERRORTYPE
COmxILPort::GetParamStructureFromTunnel(
	T& aComponentConfigStructure, OMX_INDEXTYPE aParamIndex) const
	{

	__ASSERT_ALWAYS(GetTunnelledComponent(),
					User::Panic(KOmxILPortPanicCategory, 1));

	aComponentConfigStructure.nSize		 = sizeof(T);
	aComponentConfigStructure.nVersion	 = TOmxILSpecVersion();
	aComponentConfigStructure.nPortIndex = GetTunnelledPort();

	if (OMX_ErrorNone !=
		OMX_GetParameter(GetTunnelledComponent(),
						 aParamIndex,
						 &aComponentConfigStructure) )
		{
		return OMX_ErrorUndefined;
		}

	return OMX_ErrorNone;

	}


