/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class wrapping necessary information for a callback method.
*
*/


#ifndef CORE_CALLBACK_H
#define CORE_CALLBACK_H

/**
 * Class wrapping necessary information for a callback method.
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS(core_callback_c)
    {
public:

	inline core_callback_c(void (*function)(void* ptr), void* ptr );

	/**
	 * A pointer to the callback function.
	 */
	void (*func_m)(void* ptr);
		
	/**
	 * A pointer that is passed to the callback function when
	 * the function is called.
	 */
	void* ptr_m;
    };

inline core_callback_c::core_callback_c(void (*function)(void* ptr), void* ptr )
	: func_m(function), ptr_m(ptr)
	{}

#endif // CORE_CALLBACK_H
