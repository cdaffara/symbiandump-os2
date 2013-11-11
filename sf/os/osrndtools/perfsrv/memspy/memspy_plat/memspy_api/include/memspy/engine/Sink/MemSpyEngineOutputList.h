/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MEMSPYENGINEOUTPUTLIST_H
#define MEMSPYENGINEOUTPUTLIST_H

// System includes
#include <e32base.h>
#include <bamdesca.h>

// Classes referenced
class CMemSpyEngineOutputSink;
class CMemSpyEngineOutputListItem;


NONSHARABLE_CLASS( CMemSpyEngineOutputList ) : public CBase, public MDesCArray
    {
public:
    static CMemSpyEngineOutputList* NewL();
    static CMemSpyEngineOutputList* NewLC( CMemSpyEngineOutputSink& aSink );
    ~CMemSpyEngineOutputList();

private:
    CMemSpyEngineOutputList( CMemSpyEngineOutputSink* aSink = NULL );
    void ConstructL();

public: // From MDesCArray
    TInt MdcaCount() const;
    TPtrC MdcaPoint( TInt aPos ) const;

public: // API - add
    void AddItemL( const TDesC& aCaption );
    void AddItemL( const TDesC& aCaption, const TDesC& aValue );
    void AddItemL( const TDesC& aCaption, TInt aValue );
    void AddItemL( const TDesC& aCaption, TUint aValue );
    void AddItemL( const TDesC& aCaption, const TInt64& aValue );
    void AddItemL( const TDesC& aCaption, TAny* aValue );
    void AddItemL( const TDesC& aCaption, TUint* aValue );
    void AddItemL( const TDesC& aCaption, TUint8* aValue );
    void AddItemFormatL( const TDesC& aCaption, TRefByValue<const TDesC> aValueFormat, ... );
    void AddItemFormatL( TRefByValue<const TDesC> aFormat, ... );
    void AddItemUCL( const TDesC& aCaption, const TDesC& aValue );
    void AddItemFormatUCL( const TDesC& aCaption, TRefByValue<const TDesC> aValueFormat, ... );
    void AddItemFormatUCL( TRefByValue<const TDesC> aFormat, ... );
    void AddItemHexL( const TDesC& aCaption, TUint aValue );
    void AddItemYesNoL( const TDesC& aCaption, TBool aYes );
    void AddItemTrueFalseL( const TDesC& aCaption, TBool aTrue );
    void AddItemOnOffL( const TDesC& aCaption, TBool aOn );
    void AddItemPercentageL( const TDesC& aCaption, TInt aOneHundredPercentValue, TInt aValue );
    void AddBlankItemL( TInt aRepetitions = 1 );
    void AddUnderlineForPreviousItemL( TChar aUnderlineCharacter = '=', TInt aBlankItemCount = 1 );

public: // API - insert
    void InsertItemL( TInt aPos, const TDesC& aCaption );
    void InsertItemL( TInt aPos, const TDesC& aCaption, const TDesC& aValue );
    void InsertItemFormatUCL( TInt aPos, TRefByValue<const TDesC> aValueFormat, ... );
    void InsertBlankItemL( TInt aPos, TInt aRepetitions = 1 );
    void InsertUnderlineForItemAtL( TInt aPos, TChar aUnderlineCharacter = '=', TInt aBlankItemCount = 1 );

public: // API - print
    void PrintL();
    void PrintL( CMemSpyEngineOutputSink& aSink );

public: // API - access
    CMemSpyEngineOutputListItem& Item( TInt aPos );
    const CMemSpyEngineOutputListItem& Item( TInt aPos ) const;
    inline TInt Count() const { return iItems.Count(); }

private: // Internal methods
    TPtr& FormatBuffer();

private: // Data members
    CMemSpyEngineOutputSink* iSink;
    HBufC* iFormatBuffer;
    TPtr iFormatBufferPtr;
    RPointerArray<CMemSpyEngineOutputListItem> iItems;
    };


#endif