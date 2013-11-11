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

#ifndef __BREAKOBSERVER_H__
#define __BREAKOBSERVER_H__

#include <e32std.h>
 
class MBreakObserver
/**
 * Interface for notifications of break changes.
 */
	{
public:
	/**
	 * Called when an MBreakObserver-requested break has completed. 
	 */
	virtual void BreakRequestCompleted() = 0;
	/**
	 * Called when the break state has changed (i.e. it has been off and gone 
	 * either timed or locked, or if it was on and has gone off).
	 */
	virtual void BreakStateChange() = 0;
	};

#endif // __BREAKOBSERVER_H__
