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

#ifndef MEMSPYENGINEOUTPUTLISTITEM_H
#define MEMSPYENGINEOUTPUTLISTITEM_H

// System includes
#include <e32base.h>


NONSHARABLE_CLASS( CMemSpyEngineOutputListItem ) : public CBase
    {
    friend class CMemSpyEngineOutputList;

public:
    static CMemSpyEngineOutputListItem* NewLC( const CMemSpyEngineOutputListItem& aCopyMe );
    static CMemSpyEngineOutputListItem* NewLC( const TDesC& aCaption );
    static CMemSpyEngineOutputListItem* NewLC( const TDesC& aCaption, const TDesC& aValue );
    static CMemSpyEngineOutputListItem* NewLC( const TDesC& aCaption, TUint aValue );
    static CMemSpyEngineOutputListItem* NewHexLC( const TDesC& aCaption, TUint aValue );
    static CMemSpyEngineOutputListItem* NewDecimalLC( const TDesC& aCaption, TInt aValue );
    static CMemSpyEngineOutputListItem* NewLongLC( const TDesC& aCaption, const TInt64& aValue );
    static CMemSpyEngineOutputListItem* NewYesNoLC( const TDesC& aCaption, TBool aYes );
    static CMemSpyEngineOutputListItem* NewTrueFalseLC( const TDesC& aCaption, TBool aTrue );
    static CMemSpyEngineOutputListItem* NewOnOffLC( const TDesC& aCaption, TBool aOn );
    static CMemSpyEngineOutputListItem* NewPercentageLC( const TDesC& aCaption, TInt aOneHundredPercentValue, TInt aValue );
    ~CMemSpyEngineOutputListItem();

private:
    CMemSpyEngineOutputListItem();
    void ConstructL( const TDesC& aCaption, const TDesC& aValue );

public: // API
    inline const TDesC& Caption() const { return *iCaption; }
    inline const TDesC& Value() const { return *iValue; }
    inline const TDesC& Combined() const { return *iCombined; }
    void UpdateCombinedL();

public:
    void SetValueL( const TDesC& aValue );
    void SetHexL( TUint aValue );
    void SetDecimalL( TInt aValue );
    void SetUnsignedL( TUint aValue );
    void SetLongL( const TInt64& aValue );
    void SetYesNoL( TBool aYes );
    void SetTrueFalseL( TBool aTrue );
    void SetOnOffL( TBool aOn );
    void SetPercentageL( TInt aOneHundredPercentValue, TInt aValue );

private:
    HBufC* iCaption;
    HBufC* iValue;
    HBufC* iCombined;
    };



#endif