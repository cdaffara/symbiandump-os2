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
* Optional data owned by a CEditableText object; used by the inline FEP editing and undo-redo systems, etc.
*
*/




#ifndef __TXTOPT_H__
#define __TXTOPT_H__

NONSHARABLE_CLASS(CEditableTextOptionalData) : public CBase
/**
@internalComponent
*/
	{
	public:
	CEditableTextOptionalData();
	~CEditableTextOptionalData();

	CInlineEditData* iInlineEditData; // FEP inline editing data

	private:
	CEditableTextOptionalData(const CEditableTextOptionalData&); // unimplemented
	void operator=(const CEditableTextOptionalData&); // unimplemented
	};

#endif // __TXTOPT_H__
