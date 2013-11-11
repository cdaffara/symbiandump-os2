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


#include <e32std.h>
#include "TXTETEXT.H"
#include "TXTFEP.H"
#include "TXTOPT.H"

CInlineEditData::CInlineEditData()
	{
	// do nothing
	}

CInlineEditData::~CInlineEditData()
	{
	delete iInlineText;
	delete iHiddenText;
	}

void CEditableText::SetAndTransferOwnershipOfInlineEditDataL(CInlineEditData* aInlineEditData)
	{
	if (!iOptionalData)
		iOptionalData = new(ELeave) CEditableTextOptionalData;
	delete iOptionalData->iInlineEditData;
	iOptionalData->iInlineEditData = aInlineEditData;
	}

CInlineEditData* CEditableText::InlineEditData() const
	{
	if (iOptionalData)
		return iOptionalData->iInlineEditData;
	else
		return NULL;
	}

void CEditableText::DeleteInlineEditDataAndSetToNull()
	{
	if (iOptionalData)
		{
		delete iOptionalData->iInlineEditData;
		iOptionalData->iInlineEditData = NULL;
		}
	}
