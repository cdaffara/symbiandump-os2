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
*
*/


#include <s32strm.h>
#include <caf/attributeset.h>
#include <caf/caferr.h>

using namespace ContentAccess;

EXPORT_C RAttributeSet::RAttributeSet()
	{
	}

EXPORT_C void RAttributeSet::Close()
	{
	// Clean up array
	iAttributes.Reset();
	iAttributes.Close();
	}

EXPORT_C void RAttributeSet::AddL(TInt aAttribute)
	{
	// Can't have duplicates so if the attribute already exists just reset its
	// value back to KErrCANotSupported
	if(SetValue(aAttribute, 0, KErrCANotSupported) != KErrNone)
		{
		// Doesn't exist so add it
		AddL(aAttribute, 0, KErrCANotSupported);
		}
	}

EXPORT_C TInt RAttributeSet::GetValue(TInt aAttribute, TInt& aValue) const
	{
	TInt i;
	TInt count = iAttributes.Count();
	for(i  = 0; i < count; i++)
		{
		if(iAttributes[i].iAttribute == aAttribute)
			{
			// Set the value and return the associated error code
			aValue = iAttributes[i].iValue;
			return iAttributes[i].iError;
			}
		}
	return KErrNotFound;
	}

EXPORT_C TInt RAttributeSet::SetValue(TInt aAttribute, TInt aValue, TInt aErrorCode)
	{
	TInt i;
	TInt count = iAttributes.Count();
	for(i  = 0; i < count; i++)
		{
		if(iAttributes[i].iAttribute == aAttribute)
			{
			iAttributes[i].iValue = aValue;
			iAttributes[i].iError = aErrorCode;
			return KErrNone;
			}
		}
	return KErrNotFound;
	}
		
EXPORT_C TInt RAttributeSet::operator [] (TInt aIndex) const
		{
		return iAttributes[aIndex].iAttribute;
		}

EXPORT_C TInt RAttributeSet::Count() const
		{
		return iAttributes.Count();
		}
		
EXPORT_C void RAttributeSet::ExternalizeL(RWriteStream& aStream) const
	{
	TInt i;
	TInt count = iAttributes.Count();
	aStream.WriteInt32L(count);
	for(i = 0; i < count; i++)
		{
		aStream.WriteInt32L(iAttributes[i].iAttribute);
		aStream.WriteInt32L(iAttributes[i].iValue);
		aStream.WriteInt32L(iAttributes[i].iError);
		}
	}

EXPORT_C void RAttributeSet::InternalizeL(RReadStream& aStream)
	{
	TInt i;
	TInt attribute;
	TInt value;
	TInt errorCode;

	// Read the number of attributes to internalize
	TInt count = aStream.ReadInt32L();
	
	// loop through all the attributes
	for(i = 0; i < count; i++)
		{
		// Read the attribute and value from the stream
		attribute = aStream.ReadInt32L();
		value = aStream.ReadInt32L();
		errorCode = aStream.ReadInt32L();
		
		// try setting the attribute value first in case it already exists
		if(SetValue(attribute, value, errorCode) != KErrNone)
			{
			// Doesn't exist so set a new values
			AddL(attribute, value, errorCode);
			}
		}
	}

void RAttributeSet::AddL(TInt aAttribute, TInt aValue, TInt aErrorCode)
	{
	TAttributeValue attribute;
	attribute.iAttribute = aAttribute;
	attribute.iValue = aValue;
	attribute.iError = aErrorCode;
		
	// Append the new values to the iAttributes array
	User::LeaveIfError(iAttributes.Append(attribute));
	}
