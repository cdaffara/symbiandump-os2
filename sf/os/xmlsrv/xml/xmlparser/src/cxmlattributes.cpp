/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include "cxmlattributes.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXMLAttributes::CXMLAttributes()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXMLAttributes::CXMLAttributes()
	{
	}

// -----------------------------------------------------------------------------
// CXMLAttributes::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXMLAttributes::ConstructL()
    {
		iDescAttName = new (ELeave) CDesCArraySeg(1);
		iDescAttValue = new (ELeave) CDesCArraySeg(1);
    }

// -----------------------------------------------------------------------------
// CXMLAttributes::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXMLAttributes* CXMLAttributes::NewL()
    {
		CXMLAttributes* self = new( ELeave ) CXMLAttributes;
    
		CleanupStack::PushL( self );
		self->ConstructL();
		CleanupStack::Pop();

		return self;
    }

    
// -----------------------------------------------------------------------------
// CXMLAttributes::~CXMLAttributes()
// Destructor.
// -----------------------------------------------------------------------------
//
CXMLAttributes::~CXMLAttributes()
    {
		if(iDescAttName)
		{
			delete iDescAttName;
		}

		if(iDescAttValue)
		{
			delete iDescAttValue;
		}

    }


// -----------------------------------------------------------------------------
// CXMLAttributes::GetIndex
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CXMLAttributes::GetIndex(TDesC& aName)
   {

		TInt wIndex = 0;
		if(iDescAttName->Find(aName,wIndex) != KErrNone)
		{
			return KErrNotFound;
		}
		else
		{
			return wIndex;
		}

   }

// -----------------------------------------------------------------------------
// CXMLAttributes::GetLength
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CXMLAttributes::GetLength()
   {
		return iDescAttName->Count();
   }

// -----------------------------------------------------------------------------
// CXMLAttributes::GetName
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CXMLAttributes::GetName(TInt  aIndex)
   {
		if(aIndex > iDescAttName->Count() || aIndex < 0)
		{
			TPtrC wEmpty = _L(" ");
			return wEmpty;
		}
		return iDescAttName->MdcaPoint(aIndex);
		
   }

// -----------------------------------------------------------------------------
// CXMLAttributes::GetValue
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CXMLAttributes::GetValue(TDesC& aName)
   {
		TInt wPos = GetIndex(aName);

		if (wPos < 0)
		{
			TPtrC wEmpty = _L(" ");
			return wEmpty;
		}
		return iDescAttValue->MdcaPoint(wPos);

   }

// -----------------------------------------------------------------------------
// CXMLAttributes::GetValue
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CXMLAttributes::GetValue(TInt& aIndex)
   {
		if(aIndex > iDescAttName->Count() || aIndex < 0)
		{
			TPtrC wEmpty = _L(" ");
			return wEmpty;
		}
		return iDescAttValue->MdcaPoint(aIndex);
   }

// -----------------------------------------------------------------------------
// CXMLAttributes::GetType
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
/// R&D: Feature: GetType missing from Attributes
TPtrC CXMLAttributes::GetType(TInt /*aIndex*/)
   {
		TPtrC wType = _L(" ");
		return(wType);
   }

// -----------------------------------------------------------------------------
// CXMLAttributes::GetType
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// R&D: Feature: GetType missing from Attributes
TPtrC CXMLAttributes::GetType(TDesC& /*aName*/)
   {
		TPtrC wType = _L(" ");
		return wType;
   }

// -----------------------------------------------------------------------------
// CXMLAttributes::Append
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXMLAttributes::AppendL(const TDesC& aName, const TDesC& aValue)
   {
		if ( iDescAttName && iDescAttValue )
		{
			iDescAttName->AppendL(aName);
			iDescAttValue->AppendL(aValue);
		}
   }

// -----------------------------------------------------------------------------
// CXMLAttributes::Reset()
// Resets attribute Name and Value arrays
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXMLAttributes::Reset()
	{
		iDescAttName->Reset();
		iDescAttValue->Reset();
	}

//  End of File  
