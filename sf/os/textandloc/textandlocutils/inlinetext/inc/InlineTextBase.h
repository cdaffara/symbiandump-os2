/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Base classes used for holding inline text for the Form MTmInlineTextSource interface.
*
*/


#ifndef INLINETEXTBASE_H
#define INLINETEXTBASE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <medobsrv.h>
#include <inlinetext.h>

// CONSTANTS
enum TInlineTextPanic
    {
    EInlineTextBadInlineTextFetch,
    EInlineTextStoreCorrupted
    };

#define KInlineTextParagraphDelimiter 0x2029

GLREF_C void Panic(TInlineTextPanic aPanic);

// CLASS DECLARATION

class TTmDocPos;

/**
* This class is used to hold text that is being formatted into text via the inline text 
* interface.
*
*  @lib InlineText
*  @since 3.2
*/
NONSHARABLE_CLASS(CInlineTextPositionedText): public CBase
    {
    public: // 2stage constructor and descructor
    /**
    * Fully constructs the object given position and text. A copy of the passed-in text is made
    * on the heap, owned by the constructed object.
    *
    * @param    aPosition   TTmDocPos at which text is to be reported
    * @param    aInlineText Text to be returned at this position
    *
    * @return   pointer to fully constructed object
    */
    static CInlineTextPositionedText* NewL( const TTmDocPos& aPosition, const TDesC& aInlineText );

    /**
    * Destructor
    */
    ~CInlineTextPositionedText();

    public:
    
    /**
    * Access method to the position in the document of the inline text
    * @return   TTmDocPos of the inline text
    */
    const TTmDocPos& DocPos() const;

    /**
    * Access method to the text for the inline edit
    * @return   reference to an unmodifiable descriptor for the text held in this object
    */
    const TDesC& InlineText() const;

    private: 
    /**
    * C++ constructor
    */
    CInlineTextPositionedText( const TTmDocPos& aPosition );
    
    /**
    * This method completes the construction by allocating the text storage.. 
    */
    void ConstructL( const TDesC& aInlineText );

    private:
    TTmDocPos iDocPos;
    HBufC* iText; // owned
    };

/**
* Class to hold an array of inline texts for access
*
*  @lib InlineText
*  @since 3.2
*/
NONSHARABLE_CLASS(CInlineTextStore) : public CArrayPtrFlat<CInlineTextPositionedText>
    {
     public: // 2-stage constructor and destructor
        static CInlineTextStore* NewL();     
        ~CInlineTextStore();

    public:
        /**
        * Clears and destroys all inline texts
        */
        void Clear();

        /**
        * Clears and destroys inline texts in a range of document positions. Range is taken
        * to be inclusive.
        * @param    aStart  First position from which and including inline texts are removed
        * @param    aEnd    Final position up to which and including inline texts are removed
        */
        void ClearRange( const TTmDocPos& aStart, const TTmDocPos& aEnd );

        /**
        * Insert the inline text object into the store
        * @param    aInlineText     object to add
        */
        void InsertInlineTextL( CInlineTextPositionedText* aInlineText );

        /**
        * Returns the pointer to the document position closest after or including the passed-in
        * position.
        * @param    aDocPos     position from which to look for inline texts
        * @return               pointer to document position at or closest after input position 
        *                       NULL if none is found
        */
        const TTmDocPos* NextInlineTextDocPos( const TTmDocPos& aDocPos ) const;

        /**
        * Searches for the input document position and returns the index in the Store
        * @param    aDocPos     input document position
        * @return               index at which the input position is found
        *                       -1 if the document position is not in the store
        */
        TInt IndexFromDocPos( const TTmDocPos& aDocPos ) const;

    private: 
        /**
        * Private constructor; no derivation permitted
        */
        CInlineTextStore();

        /**
        * Internal method for determining index of the position later or including
        * the passed-in position
        */
        TInt NextIndexStartingAtDocPos( const TTmDocPos& aDocPos ) const;

    private: // data members

    };

/**
* Concrete implementation of MTmInlineTextSource, adding an edit observation interface
* and common infrastructure for managing inline texts
* 
*  @lib InlineText
*  @since 3.2
*/
NONSHARABLE_CLASS(CInlineTextSource) : public CBase, public MTmInlineTextSource, public MEditObserver
    {
    public: // C++ constructor and destructor
        CInlineTextSource(); // Class is intended for derivation
        ~CInlineTextSource();

        /** 
        * 2nd state constructor
        */
        void ConstructL();

    public: // From MTmInlineTextSource

	/**
    * Reports the next position into which inline text should be inserted
    *
    * See inlinetext.h
	*/
	virtual TInt GetNextInlineTextPosition(const TTmDocPos& aFrom, TInt aMaxLength, TTmDocPos& aNext);

	/**
	* Gets a view of the text to be inserted at aAt.
    * 
    * See inlinetext.h
	*/
	virtual TPtrC GetInlineText(const TTmDocPos& aAt);

    public: // From MEditObserver

    /**
    * See medobsrv.h
    * 
    * This class implements an empty implementation of this method. Implement in subclass only if 
    * action is to be taken upon edit - and if edit event is actually going to be supplied, which
    * is only available in the context of CEikRichTextEditor
    */
	virtual void EditObserver(TInt aStart, TInt aExtent);

    public:
    // New methods

    /**
    * Framework method to inform subclasses what part of the document is currently being interrogated
    * for inline edits.
    * Currently this will be where formatting is performed and the store of inline texts updated.
    * (In future, an edit observer interface of some kind will be used to trigger that)
    *
    * @param aFrom  position from which to re-consider the formatting
    * @param aTo    position up to which to re-consider the formatting
    */
    virtual void CheckFormattingL(const TTmDocPos& aFrom, const TTmDocPos& aTo );

    /**
    * Non-virtual method for checking for inline text.  Similar behaviour to GetInlineText, except 
    * that is it const, and will not panic (but just return EFalse) if there is no inline text
    * at the input TTmDocPos.
    * 
    * @param aAt        Position at which to access inline text
    * @param aPtrFound  This contains the inline text if ETrue is returned
    * @return       EFalse if there is no inline text at the input position
    */
    TBool HasInlineTextAt( const TTmDocPos& aAt, TPtrC& aPtrFound ) const;

    protected:

    /**
    * Access to the inline text store for derived classes
    */
    CInlineTextStore* InlineTextStore() const;
    
    private:
    CInlineTextStore* iInlineTextStore; // owned
    };

#endif

// End of File
