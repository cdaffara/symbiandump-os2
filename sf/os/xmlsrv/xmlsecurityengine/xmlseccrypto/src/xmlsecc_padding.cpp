/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class with implementation of xmlenc padding.
*
*/


#include "xmlsecc_padding.h"

// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
//   
CXmlSecPadding* CXmlSecPadding::NewLC(TInt aBlockBytes)
    {
    CXmlSecPadding* self = new (ELeave) CXmlSecPadding(aBlockBytes);
    CleanupStack::PushL(self);
    return self;
    }
    
// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
// 
CXmlSecPadding* CXmlSecPadding::NewL(TInt aBlockBytes)
    {
    CXmlSecPadding* self = CXmlSecPadding::NewLC(aBlockBytes);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// 
CXmlSecPadding::CXmlSecPadding(TInt aBlockBytes) : CPadding(aBlockBytes)
    {
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// 
void CXmlSecPadding::DoPadL(const TDesC8& aInput,TDes8& aOutput)
    {
    TInt padLength = BlockSize() - aInput.Length();
    aOutput.Append(aInput);
    aOutput.AppendFill( (TChar)padLength, padLength );
    }

// ---------------------------------------------------------------------------
// Add pad to buffer
// ---------------------------------------------------------------------------
// 
void CXmlSecPadding::UnPadL(const TDesC8& aInput,TDes8& aOutput)
    {
    TInt padLength = (TInt) aInput[ BlockSize()-1 ];
    aOutput.Copy( aInput.Left( BlockSize() - padLength ) );
    }

// ---------------------------------------------------------------------------
// Remove pad
// ---------------------------------------------------------------------------
// 
TInt CXmlSecPadding::MinPaddingLength(void) const
    {
    return KMinPaddingLenght;
    }
