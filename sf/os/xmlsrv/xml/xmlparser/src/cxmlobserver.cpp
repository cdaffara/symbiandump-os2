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



#include "cxmlobserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXMLObserver::CXMLObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXMLObserver::CXMLObserver() 
	{
		
	}

// -----------------------------------------------------------------------------
// CXMLObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXMLObserver::ConstructL()
    {
		iCurrentAttribute = new(ELeave) TPtrC();
		iAttributes = CXMLAttributes::NewL();
		iDescElements = new (ELeave) CDesCArraySeg(1);
		iNSextensions = new (ELeave) CDesCArraySeg(1);
		iURIs = new (ELeave) CDesCArraySeg(1);
		iContentHandler = NULL;
		iBinaryContentHandler = NULL;
    }

// -----------------------------------------------------------------------------
// CXMLObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXMLObserver* CXMLObserver::NewL()
    {
		CXMLObserver* self = new( ELeave ) CXMLObserver();
    
		CleanupStack::PushL( self );
		self->ConstructL();
		CleanupStack::Pop();

		return self;
    }

    
// -----------------------------------------------------------------------------
// CXMLObserver::~CXMLObserver
// Destructor.
// -----------------------------------------------------------------------------
//
CXMLObserver::~CXMLObserver()
    {
		if (iCurrentAttribute)
		{
			delete iCurrentAttribute;
		}
		if(iAttributes)
		{
			delete iAttributes;
		}
		if(iDescElements)
		{
			delete iDescElements;
		}
		if(iNSextensions)
		{
			delete iNSextensions;
		}
		if(iURIs)
		{
			delete iURIs;
		}
    }

// -----------------------------------------------------------------------------
// CXMLObserver::SetElement
// 
// -----------------------------------------------------------------------------
//
void CXMLObserver::SetElementL(const TPtrC &aName)
{
	if(iDescElements)
	{
		iDescElements->AppendL(aName);
	}
}

// -----------------------------------------------------------------------------
// CXMLObserver::GetElement
// 
// -----------------------------------------------------------------------------
//
TPtrC CXMLObserver::GetElement() const
{
	TInt wIndex = iDescElements->Count()-1; 
	if(wIndex < 0)
	{
		TPtrC wEmpty = _L(" ");
		return wEmpty;
		
	}
	return iDescElements->MdcaPoint(wIndex);
}

// -----------------------------------------------------------------------------
// CXMLObserver::GetElement
// 
// -----------------------------------------------------------------------------
//
void CXMLObserver::RemoveElement()
{
	TInt wIndex = iDescElements->Count()-1;
	if(wIndex > -1)
	{
		iDescElements->Delete(wIndex);
		iDescElements->Compress();
	}
}

void CXMLObserver::SetURIL(const TPtrC &aURI, const TPtrC &aNSextension)
{
	iNSextensions->AppendL(aNSextension);
	iURIs->AppendL(aURI);
}

TPtrC CXMLObserver::GetURI(const TPtrC &aNSextension) const
{
	TInt wIndex = 0;
	TInt wRes = 0;

	wRes = iNSextensions->Find(aNSextension,wIndex); 
	if(wRes != 0)
	{
		TPtrC wEmpty = _L(" ");
		return wEmpty;
	}
	return iURIs->MdcaPoint(wIndex);
}
void CXMLObserver::ResetURI()
{
	iNSextensions->Reset();
	iURIs->Reset();
}

//  End of File  
