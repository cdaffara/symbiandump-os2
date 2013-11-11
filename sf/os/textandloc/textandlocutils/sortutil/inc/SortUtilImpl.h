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






#ifndef SORTUTILIMPL_H
#define SORTUTILIMPL_H

//  INCLUDES

#include <e32std.h>
#include "SortUtil.h"

namespace SortUtilImpl
    {

    /**
     * Returns aText with all leading whitespace characters stripped from it.
     */
    TPtrC StripLeadingWhiteSpace(const TDesC& aText);

    /**
     * Searches for the next nonempty key in a sort key array.
     *
     * @param aKeys     the sort key array to search.
     * @param aType     the sort key type to search for.
     * @param aIndex    the index where to start the search from. If a key is
     *                  returned the index is incremented.
     * @return  the first nonempty sort key's text or empty string if 
     *          aIndex > aKeys.SortKeyCount(). Any leading whitespace in the
     *          sort key text is removed in the returned string.
     */    
    TPtrC FindNextNonEmptyKey
            (const MSortKeyArray& aKeys, 
            TSortKeyType aType, 
            TInt& aIndex);

    }  // namespace SortUtilImpl

#endif // SORTUTILIMPL_H
// End of file
