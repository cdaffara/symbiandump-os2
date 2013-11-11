// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Feature Registry panic codes.
// 
//

/**
 @file
 @publishedPartner
 @deprecated
*/

#ifndef FEATREGPAN_H
#define FEATREGPAN_H

enum TFeatRegPanic
	{
	/** feature registry configuration file or setup utility missing or invalid
		a debug-only panic */
	EFeatRegBadConfig = 1,
	/** invalid use of RFeatureRegistry or RFeatureRegistryNotify, including
		calling Open() twice, making queries or subscribing when not open */
	EFeatRegInvalidUse = 2
	};

#endif
