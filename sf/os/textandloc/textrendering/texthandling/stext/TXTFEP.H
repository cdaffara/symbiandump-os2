/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TXTFEP_H__
#define __TXTFEP_H__

#include <e32std.h>

class MFepInlineTextFormatRetriever;

NONSHARABLE_CLASS(CInlineEditData) : public CBase
/**
@internalComponent
*/
	{
	public:
	CInlineEditData();
	~CInlineEditData();

	TInt iPositionOfInlineTextInDocument;
	TInt iLengthOfInlineText;
	HBufC* iInlineText; // owned by this pointer
	HBufC* iHiddenText; // owned by this pointer
	const MFepInlineTextFormatRetriever* iInlineTextFormatRetriever; // not owned by this pointer
	};

#endif
