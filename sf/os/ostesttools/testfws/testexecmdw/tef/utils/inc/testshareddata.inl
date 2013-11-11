/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of Test structured data class for sharing between process
* Initialises the member variable with zero
*
*/



/**
 @file TestSharedData.cpp
*/
void CTestSharedData::Construct()
	{
	// Initialise the data member
	for (TInt index = 0; index < KMaxSharedDataLength; index++)
		{
		iText[index] = 0;
		}
	}

/**
 * Copies the data input to the shared TText member for sharing
 * @param aVal - Descriptor containing the string to be set to the member data
 */
void CTestSharedData::SetText(TDesC& aVal)
	{
	TInt length = aVal.Length();
	if (length < KMaxSharedDataLength)
		{
		for (TInt index = 0; index < length; index++)
			{
			iText[index] = *(aVal.Mid(index,1).Ptr());
			}
		for (TInt nullIndex = length; nullIndex < KMaxSharedDataLength; nullIndex++)
			{
			iText[nullIndex] = 0;
			}
		}
	else
		{
		User::Panic(_L("OutOfMemory"), KErrNoMemory);
		}
	}

/**
 * Appends the data input to the shared TText for sharing
 * @param aVal - Descriptor containing the string to be appended to the member data
 */
void CTestSharedData::AppendText(TDesC& aVal)
	{
	TInt appendLength = aVal.Length();
	TInt originalLength = User::StringLength(iText);;
	TInt length = originalLength + appendLength;
	if (length < KMaxSharedDataLength)
		{
		TInt midLocation = 0;
		for (TInt index = originalLength; index < length; index++)
			{
			iText[index] = *(aVal.Mid(midLocation,1).Ptr());
			midLocation++;
			}
		for (TInt nullIndex = length; nullIndex < KMaxSharedDataLength; nullIndex++)
			{
			iText[nullIndex] = 0;
			}
		}
	else
		{
		User::Panic(_L("OutOfMemory"), KErrNoMemory);
		}
	}

/**
 * Copies the value within member data to the descriptor reference passed in
 * @param aVal - Reference Descriptor which gets set with member data value
 */
void CTestSharedData::GetText(TDes& aVal)
	{
	TInt length = User::StringLength(iText);
	TInt bufferLength = aVal.MaxLength();
	if (bufferLength < length)
		{
		User::Panic(_L("OutOfMemory"), KErrNoMemory);
		}

	aVal.Zero();
	for (TInt index = 0; index < length; index++)
		{
		aVal.Append(iText[index]);
		}
	}
