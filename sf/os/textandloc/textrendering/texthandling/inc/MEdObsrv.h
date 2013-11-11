/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MEDOBSRV_H__
#define __MEDOBSRV_H__

#include <e32def.h>



class MEditObserver
/** 
Specifies the mixin protocol for a rich text edit observer. 

This class is implemented by controls which use a rich text (CRichText) object, 
when it is possible for the text object to be changed from outside of the 
control without the control's knowledge.

An edit observer is a callback to the control from the CRichText object which 
it has been set up to observe. The edit observer is set up using CRichText::SetEditObserver(). 
@publishedAll
@released
*/
	{
public:
	// Observer function called by CRichText to signal changes to the text
	// buffer within it. A pointer to this function must be passed in to
	// a CRichText instance by <rich text instance>.SetEditObserver(<ptr>).
	// aStart is the start position of the edit and aExtent is the number
	// of characters added or, if negative, deleted after this position.
	
	
	/** Observer function called by the CRichText object when the object is edited 
	to give notification of changes to its text buffer. In this way, the control 
	knows how the text object has changed and can reformat and redraw the text 
	object appropriately.
	
	A pointer to the observer object must have been passed in to a CRichText instance 
	using CRichText::SetEditObserver().
	
	@param aStart The start position of the edit. 
	@param aExtent The number of characters added or, if negative, deleted after 
	this position. */
	virtual void EditObserver(TInt aStart, TInt aExtent) = 0;
	};

#endif
