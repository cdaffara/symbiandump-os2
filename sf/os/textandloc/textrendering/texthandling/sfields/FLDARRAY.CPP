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


#include "FLDDEF.H"
#include "FLDARRAY.H"
#include "FLDSTD.H"

///////////////////////////////////////
// TTextFieldHeader
///////////////////////////////////////

TTextFieldHeader::TTextFieldHeader()
	:iField(NULL),
	iFieldType(KNullUid)
	{}


EXPORT_C void TTextFieldHeader::ExternalizeL(RWriteStream& aStream)const
// Persists this object
// Does not take responsibility for storing the picture
//
	{
	aStream<< iFieldType;
	aStream<< iField;
	}


EXPORT_C void TTextFieldHeader::InternalizeL(RReadStream& aStream)
// Restores this object from the specified stream
// Does not take responsibility for restoring the picture
//
	{
	aStream>> iFieldType;
	aStream>> iField;
	}

