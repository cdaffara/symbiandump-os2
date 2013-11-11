/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file contains base functions for the unified editor interface classes.
*
*/


#include "TAGMA.H"
#include "unified_editor.h"

EXPORT_C void MUnifiedEditor::GetText(TInt aPos,TDes& aText) const
	{
	int remainder = aText.MaxLength();
	int document_length = DocumentLength();
	aText.SetLength(0);
	TPtrC p;
	while (aPos < document_length && remainder > 0)
		{
		GetText(aPos,p);
		if (p.Length() > remainder)
			p.Set(p.Ptr(),remainder);
		aText.Append(p);
		aPos += p.Length();
		remainder -= p.Length();
		}
	}
