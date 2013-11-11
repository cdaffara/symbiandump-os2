/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* TokenServerDebug class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __TOKENSERVERDEBUG_H__
#define __TOKENSERVERDEBUG_h__

#ifdef _DEBUG

#include <e32std.h>

/**
 * OOM testing functionality.
 */

class TokenServerDebug
	{
 public:
	// Server-side OOM testing
	static void StartOOMTest();
	static void IncHeapFailPoint();
	static void ResetHeapFail();
	static void PauseOOMTest();
	static void ResumeOOMTest();

	// Heap checking
	static void HeapCheckStart();
	static void HeapCheckEnd();
 private:
	static void HeapErrorL(TBool aError);
 private:
	static TInt iCounter;
	static TInt iStartCount;
	static TInt iPauseCount;
	static TInt iInitialAllocCount;
	};

#endif // _DEBUG

#endif
