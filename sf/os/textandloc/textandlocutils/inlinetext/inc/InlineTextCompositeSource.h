/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Classes used combining several MTmInlineTextSource interfaces into one.
*
*/


#ifndef INLINETEXTCOMPOSITESOURCE_H
#define INLINETEXTCOMPOSITESOURCE_H

// INCLUDES
#include "InlineTextBase.h"

// CONSTANTS

// CLASS DECLARATION

// This array holds all the individual implementations of CAknInlineTextSource
typedef CArrayPtrFlat<CInlineTextSource> CInlineTextSourceArray;

/**
* This class is the implementation of MFormInlineTextSource that is actually accessed by 
* Tagma. It delegates its functionality to the installed formatters
* 
*  @lib InlineText
*  @since 3.2
*/
NONSHARABLE_CLASS(CInlineTextCompositeSource) : public CInlineTextSource
    {
public: // 2-stage constructor and the destructor
    IMPORT_C static CInlineTextCompositeSource* NewL();
    ~CInlineTextCompositeSource();

public:
    /**
    * Adds the inline text source to the array.  Ownership is taken by this object
    *
    * @param    aNewSource  formatter to add
    */
    IMPORT_C void InstallInlineTextSourceL( CInlineTextSource* aNewSource);

public: // From MTmInlineTextSource. Documented in header

    virtual TInt GetNextInlineTextPosition(const TTmDocPos& aFrom, TInt aMaxLength, TTmDocPos& aNext);
    virtual TPtrC GetInlineText(const TTmDocPos& aAt);

public: // From CAknInlineTextSource
    /**
    * See AknInlineText.h
    */
    virtual void CheckFormattingL(const TTmDocPos& aFrom, const TTmDocPos& aTo);

public: // From MEditObserver
    /**
    * See Medobsrv.h
    */
	virtual void EditObserver(TInt aStart, TInt aExtent);

private:
    /**
    * Private constructor
    */ 
    CInlineTextCompositeSource();

    /**
    * 2nd stage construction
    */
    void ConstructL();

private:
    CInlineTextSourceArray* iInlineTextSourceArray;
    };

#endif

// End of File
