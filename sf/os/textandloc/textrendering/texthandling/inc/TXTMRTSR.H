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


#ifndef __TXTMRTSR_H__
#define __TXTMRTSR_H__

#include <e32std.h>

class CStreamStore;

class MRichTextStoreResolver
/**
A call back for determining a store as a function of the document position.
In practice, used in the potentially defered restoring of new pictures from a store.
The store might not actually vary with document position, in which case the function is
guaranteed not to leave.
@internalAll 
*/
	{
public:
	virtual const CStreamStore& StreamStoreL(TInt aPos)const=0;
	};

#endif

