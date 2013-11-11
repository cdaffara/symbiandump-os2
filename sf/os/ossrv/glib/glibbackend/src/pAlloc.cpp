/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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




#include "glibbackend.h"
#include "glibbackendinternal.h"

#ifndef __WINSCW__
RHeap*	privateHeap = NULL;
#endif //__WINSCW__

void InitGLIBHeap(RHeap*& aPrivateHeap)
	{
	//Create the Heap for all the libraries (local to this process)
	//Increase the Heap Size to 1 MB
	const TInt KMaxHeapSize = 1048576 ; // 1MB
	aPrivateHeap = UserHeap::ChunkHeap(NULL, KMinHeapSize, KMaxHeapSize);
	if (!aPrivateHeap)
		{
		_LIT(KEGlibInit, "GLIB BACKEND");
		User::Panic(KEGlibInit, KErrNoMemory);
		}
	}

EXPORT_C void *pAlloc(size_t nBytes)
	{
#ifndef __WINSCW__
	if(!privateHeap)
		InitGLIBHeap(privateHeap);
#endif //__WINSCW__
	return privateHeap->AllocZ(nBytes);
}
