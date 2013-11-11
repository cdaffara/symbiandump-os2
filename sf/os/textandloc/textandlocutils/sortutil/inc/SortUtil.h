/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef SORTUTIL_H
#define SORTUTIL_H

//  INCLUDES
#include <e32base.h>

// DATA TYPES
/**
 * Type of a sort key.
 */
enum TSortKeyType
    {
    /// Basic sort key
    ESortKeyBasic,
    /// Pronounciation sort key
    ESortKeyPronounciation
    };

// CLASS DECLARATION

/**
 * Sort key. A pair of text and TSortKeyType.
 */
class TSortKey
    {
public:
    inline TSortKey(const TDesC& aText, TSortKeyType aType) :
        iText(aText), iType(aType) { }

    inline const TDesC& Text() const { return iText; }
    inline TSortKeyType Type() const { return iType; }

private:
    TPtrC iText;
    TSortKeyType iType;
    };

/**
 * An abstract array of TSortKey objects.
 */
class MSortKeyArray
    {
public:
    virtual ~MSortKeyArray() { }
    virtual TInt SortKeyCount() const =0;
    virtual TSortKey SortKeyAt(TInt aIndex) const =0;
    };


/**
 * SortUtil API.
 */
class MSortUtil
    {
public:
    /**
     * Compares two MSortKeyArrays.
     * @see MSortKeyArray
     *
     * @param aLhs  the left-hand-side sort key array.
     * @param aRhs  the right-hand-side sort key array.
     * @return  0  if aLhs and aRhs have equal sorting order.
     *         >0  if aLhs is sorted after aRhs
     *         <0  if aLhs is sorted before aRhs.
     */
    virtual TInt CompareItems(const MSortKeyArray& aLhs, const MSortKeyArray& aRhs) const =0;
    };

/**
* CSortUtil instance gives access to MSortUtil interface.
*/
NONSHARABLE_CLASS(CSortUtil) : public CBase
    {
    public: // Constructors and destructor
        IMPORT_C static CSortUtil* NewL();
        ~CSortUtil();

    public: // New functions
        MSortUtil* Interface() {return iInterface;}

    private: // Private constructors
        CSortUtil();
        void ConstructL();

    private: // Data
        MSortUtil* iInterface;
        RLibrary iLib;
    };

#endif // SORTUTIL_H

// End of File
