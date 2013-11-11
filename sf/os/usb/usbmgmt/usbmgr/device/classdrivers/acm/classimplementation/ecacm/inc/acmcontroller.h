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

#ifndef __ACMCONTROLLER_H__
#define __ACMCONTROLLER_H__

#include <e32base.h>

class MAcmController

/**
 * Interface for bringing up and down ACM functions.
 * Used by both the (deprecated) registration port method of controlling ACM 
 * functions, and by the 'ACM server' method.
 */
	{
public:
	virtual TInt CreateFunctions(const TUint aNoAcms, const TUint8 aProtocolNum, const TDesC& aAcmControlIfcName, const TDesC& aAcmDataIfcName) = 0;
	virtual void DestroyFunctions(const TUint aNoAcms) = 0;
	};

#endif // __ACMCONTROLLER_H__
