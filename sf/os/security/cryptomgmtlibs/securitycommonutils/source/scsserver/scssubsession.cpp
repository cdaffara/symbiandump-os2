/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* CScsSubsession implementation.  See class and function definitions
* for more detail.
*
*/


/**
 @file
*/

#include <scs/scsserver.h>


EXPORT_C CScsSubsession::CScsSubsession(CScsSession &aSession)
	: iSession(aSession)
/**
	This protected constructor prevents the object from being
	directly instantiated.
 */
	{
	// empty.
	}

