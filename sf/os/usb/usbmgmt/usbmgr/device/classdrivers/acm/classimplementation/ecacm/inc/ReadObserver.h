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

#ifndef __READOBSERVER_H__
#define __READOBSERVER_H__

#include <e32std.h>

class MReadObserver
/** 
 * An interface for notification of read completions.
 */
	{
public:
	virtual void ReadCompleted(TInt aError) = 0;
	};


#endif // __READOBSERVER_H__
