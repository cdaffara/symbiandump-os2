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



#ifndef __FLDARRAY_H__
#define __FLDARRAY_H__

#include <e32std.h>
#include <e32base.h>
#include <s32stor.h>
#include <fldbase.h>

class TTextFieldHeader
/**
Encodes a field type and the stream ID of the picture body.
The stream ID can be replaced by a pointer.
@internalComponent
*/
	{
public:
	TTextFieldHeader();
	IMPORT_C void InternalizeL(RReadStream& aStream); // internalizes the header info only
	IMPORT_C void ExternalizeL(RWriteStream& aStream)const; // externalizes the header info only
public:
	TSwizzle<CTextField> iField;
	TUid iFieldType;
	};


class TTextFieldEntry
/**
Container for a TTextField - placed in the field array
@internalComponent
*/
	{
public:
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream)const;
public:
	TInt iPreFieldLen; // the gap before the field in characters
	TInt iFieldValueLen; // length of field in characters
	TTextFieldHeader iFieldHeader; // contains the field itself
	};


class TRollbackInfo
/**
@internalComponent
*/
	{
public:
	TInt iTotalEntries;
	TInt iEntryNum;
	TInt iPreFieldLen;
	TInt iFieldValueLen;
	};


#endif
