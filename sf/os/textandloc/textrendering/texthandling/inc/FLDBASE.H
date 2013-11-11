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


#ifndef __FLDBASE_H__
#define __FLDBASE_H__

#include <e32std.h>
#include <e32base.h>
#include <s32stor.h>

// Classes defined:
class CTextField;
class MTextFieldFactory;




class CTextField : public CBase
/** 
Abstract class: derive from this to instantiate a particular type of field (eg date etc)
Abstract base class for all field types.

A field contains information which relates to a text object and can be automatically 
updated, e.g. page number or current date and time. Fields must implement 
the pure virtual functions defined in this class, including Value() which 
should calculate and return the field's new value, and Type() which returns 
the field's type UID. The type UID identifies the field type to the field 
factory (see class MTextFieldFactory). 
@publishedAll
@released
*/
	{
public:
	IMPORT_C virtual TStreamId StoreL(CStreamStore& aStore)const; // calls ExternalizeL()
	IMPORT_C virtual void RestoreL(const CStreamStore& aStore,TStreamId aId); // calls InternalizeL()
	//
	// Should be replaced by concrete derived classes.
	IMPORT_C virtual void ExternalizeL(RWriteStream& aStream)const; // Externalize state info for the field
    /** Internalises the field data. Called by RestoreL().
	
	@param aStream Stream from which the field data should be internalised. */
	virtual void InternalizeL(RReadStream& aStream)=0;
	//
	/** Sets aValueText to the current field value if the buffer is large enough. If 
	not, aValueText is not changed, and the function returns the length which 
	is required to hold the field's value.
	
	@param aValueText Descriptor which on return contains the field's updated 
	value. 
	@return Zero on success, otherwise, the length of the buffer which is required 
	to hold the field's updated value. */
	virtual TInt Value(TPtr& aValueText)=0; 
	//	
	/** Returns the field's type UID.
	
	@return The field's type UID. */
	virtual TUid Type()const=0;
	};


 

class MTextFieldFactory
/** 
Abstract class that should be derived from by any application that wishes to support fields
Abstract base class for field factories.

To use fields in editable text,

1) Define a field factory class (derived from MTextFieldFactory) that implements 
NewFieldL().

2) Create an instance of the field factory and set this to be the editable 
text object's field factory (see CPlainText::SetFieldFactory(), or you can 
specify a field factory in the text object 's NewL()).

3) Create a new field (CPlainText::NewTextFieldL()), specifying the field 
type UID (the built in field type UID values are defined in flddef.h). This 
calls the factory's NewFieldL() function.

4) Insert the field into the text object (CPlainText::InsertFieldL()).

5) Evaluate the field (CPlainText::UpdateFieldL()) and then re-evaluate when 
required. 
@publishedAll
@released
*/
	{
public:
	
	/** Implementations of this function should create a field of the type specified, 
	returning NULL if the field type is not recognised or supported.
	
	@param aFieldType The field's type UID. 
	@return Pointer to the new text field, or NULL if the factory does not recognise 
	or support the field type. */
	virtual CTextField* NewFieldL(TUid aFieldType)=0; 
	// Creates a field of the type specified
	// Returns NULL if it does not recognise/support the field type
	};


#endif
