/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef FINDITEMENGINE_H
#define FINDITEMENGINE_H

#include <e32base.h>
#include <tuladdressstringtokenizer.h>

class CFindItemEngine : public CBase
/**
@deprecated
@see CTulAddressStringTokenizer
@note For compatibility with S60 only
*/
    {
public:
    enum TFindItemSearchCase
		{
		EFindItemSearchPhoneNumberBin = CTulAddressStringTokenizer::EFindItemSearchPhoneNumberBin, 
        EFindItemSearchMailAddressBin = CTulAddressStringTokenizer::EFindItemSearchMailAddressBin,
        EFindItemSearchURLBin  = CTulAddressStringTokenizer::EFindItemSearchURLBin,
        EFindItemSearchScheme  = CTulAddressStringTokenizer::EFindItemSearchScheme
        };

    // Struct to contain a found item.
    struct SFoundItem
        {
        TInt iStartPos;
        TInt iLength;
        TFindItemSearchCase iItemType;
		};
		
    IMPORT_C static CFindItemEngine* NewL( const TDesC& aText, CFindItemEngine::TFindItemSearchCase aSearchCase );
    IMPORT_C static CFindItemEngine* NewL( const TDesC& aText, CFindItemEngine::TFindItemSearchCase aSearchCase, TInt aMinNumbers );
    IMPORT_C ~CFindItemEngine();
    IMPORT_C TBool Item( CFindItemEngine::SFoundItem& aItem );
    IMPORT_C TBool NextItem( CFindItemEngine::SFoundItem& aItem );
    IMPORT_C TBool PrevItem( CFindItemEngine::SFoundItem& aItem );
    IMPORT_C const CArrayFixFlat<CFindItemEngine::SFoundItem>* ItemArray() const;
	IMPORT_C TInt Position() const;	
    IMPORT_C void ResetPosition();
	IMPORT_C TInt ItemCount() const;
    IMPORT_C TInt DoNewSearchL( const TDesC& aText, CFindItemEngine::TFindItemSearchCase aSearchCase);
    IMPORT_C TInt DoNewSearchL( const TDesC& aText, CFindItemEngine::TFindItemSearchCase aSearchCase, TInt aMinNumbers );
private:
	CFindItemEngine();
    };

#endif      // FINDITEMENGINE_H   
            
// End of File
