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

#ifndef __READONEORMOREOBSERVER_H__
#define __READONEORMOREOBSERVER_H__

#include <e32std.h>

class MReadOneOrMoreObserver
/**
 * Interface for notifications of read one or more completions.
 */
	{
public:
	virtual void ReadOneOrMoreCompleted(TInt aError) = 0;
	};

#endif // __READONEORMOREOBSERVER_H__
