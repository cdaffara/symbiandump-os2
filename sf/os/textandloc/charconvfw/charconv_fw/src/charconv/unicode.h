/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(__UNICODE_H__)
#define __UNICODE_H__

/**
@internalComponent
*/
LOCAL_D const SCnvConversionData::SVariableByteData::SRange unicodeVariableByteDataRanges[]=
	{
		{
		0x00,
		0xff,
		1,
		0
		}
	};

/**
@internalComponent
*/
LOCAL_D const SCnvConversionData::SOneDirectionData::SRange unicodeTounicodeDataRanges[]=
	{
		{
		0x0000,
		0xffff,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		2,
		0,
			{
			0,
			0
			}
		}
	};

#endif
