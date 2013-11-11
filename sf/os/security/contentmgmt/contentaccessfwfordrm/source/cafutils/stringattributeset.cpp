/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* attributeset.cpp
*
*/


#include <s32strm.h>
#include <caf/stringattributeset.h>
#include <caf/caferr.h>
#include "cafutils.h"
#include "stringAttribute.h"

using namespace ContentAccess;

EXPORT_C RStringAttributeSet::RStringAttributeSet()
	{
	}

EXPORT_C void RStringAttributeSet::Close()
	{
	// Clean up array
	iAttributes.ResetAndDestroy();
	iAttributes.Close();
	}

EXPORT_C void RStringAttributeSet::AddL(TInt aAttribute)
	{
	// Can't have duplicates so if the attribute already exists just reset its
	// value back to KErrCANotSupported
	TInt err = SetValue(aAttribute, KNullDesC(), KErrCANotSupported);
	if(err != KErrNone)
		{
		// Doesn't exist so add it
		AddL(aAttribute, KNullDesC(), KErrCANotSupported);
		}
	}

EXPORT_C TInt RStringAttributeSet::GetValue(TInt aAttribute, TDes& aValue) const
	{
	TInt i = 0;
	// Search through the set of attributes until the correct one is found
	TInt count = iAttributes.Count();
	for(i  = 0; i < count; i++)
		{
		if(iAttributes[i]->Attribute() == aAttribute)
			{
			TPtrC value = iAttributes[i]->Value();
			if(value.Length() > aValue.MaxLength())
				{
				return KErrOverflow;
				}
			else
				{
				aValue.Copy(iAttributes[i]->Value());
				return iAttributes[i]->Error();
				}
			}
		}
	// attribute not found 
	return KErrNotFound;
	}

EXPORT_C TInt RStringAttributeSet::GetValueLength(TInt aAttribute) const
	{
	TInt i = 0;
	
	// Search through the set of attributes until the correct one is found
	TInt count = iAttributes.Count();
	for(i  = 0; i < count; i++)
		{
		if(iAttributes[i]->Attribute() == aAttribute)
			{
			return iAttributes[i]->Value().Length();
			}
		}
	return 0;
	}


EXPORT_C TInt RStringAttributeSet::SetValue(TInt aAttribute, const TDesC& aValue, TInt aErrorCode)
	{
	TInt i;
	TInt err = KErrNone;
	
	// search through the attributes see if the attribute exists
	TInt count = iAttributes.Count();
	for(i  = 0; i < count; i++)
		{
		if(iAttributes[i]->Attribute() == aAttribute)
			{
			CStringAttribute* temp = NULL;
			// if it exists replace the current value with the new one
			TRAP(err, temp = CStringAttribute::NewL(aAttribute, aValue, aErrorCode));
			if(err == KErrNone)
				{
				delete iAttributes[i];
				iAttributes[i] = temp;
				}
			return err;
			}
		}
	return KErrNotFound;
	}
		
EXPORT_C TInt RStringAttributeSet::operator [] (TInt aIndex) const
		{
		//  The attribute at a particular index..... NOT the attribute's value
		return iAttributes[aIndex]->Attribute();
		}

EXPORT_C TInt RStringAttributeSet::Count() const
		{
		// Number of attributes in the set
		return iAttributes.Count();
		}
		
EXPORT_C void RStringAttributeSet::ExternalizeL(RWriteStream& aStream) const
	{
	TInt i;
	// Write the number of attributes to the stream
	TInt count = iAttributes.Count();
	aStream.WriteInt32L(count);
	
	// loop through and write each attribute and value to the stream
	for(i = 0; i < count; i++)
		{
		// write the attribute and it's value to the stream
		aStream.WriteInt32L(iAttributes[i]->Attribute());
		TCafUtils::WriteDescriptor16L(aStream, iAttributes[i]->Value());
		aStream.WriteInt32L(iAttributes[i]->Error());
		}
	}

EXPORT_C void RStringAttributeSet::InternalizeL(RReadStream& aStream)
	{
	TInt i;
	TInt attribute;
	TInt errorCode;
	TInt err;
	HBufC *value = NULL;

	// Read the number of attributes to internalize
	TInt count = aStream.ReadInt32L();
	
	// loop through all the attributes
	for(i = 0; i < count; i++)
		{
		// Read the attribute and value from the stream
		attribute = aStream.ReadInt32L();
		value = TCafUtils::ReadDescriptor16L(aStream);
		CleanupStack::PushL(value);
		errorCode = aStream.ReadInt32L();
		
		// try setting the attribute value first in case it already exists
		err = SetValue(attribute, *value, errorCode);
		if(err != KErrNone)
			{
			// Doesn't exist so set a new values
			AddL(attribute,  *value, errorCode);
			}

		CleanupStack::PopAndDestroy(value);
		value = NULL;
		}
	}

void RStringAttributeSet::AddL(TInt aAttribute, const TDesC& aValue, TInt aErrorCode)
	{
	CStringAttribute* element = CStringAttribute::NewL(aAttribute, aValue, aErrorCode);
	CleanupStack::PushL(element);
	User::LeaveIfError(iAttributes.Append(element));
	CleanupStack::Pop(element);
	}






