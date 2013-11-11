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






// INCLUDE FILES
#include "SortUtilImpl.h"

namespace SortUtilImpl
    {

    inline TPtrC StripLeadingWhiteSpace_inline(const TDesC& aText)
    {
    const TText* p = aText.Ptr();
    const TText* const end = p + aText.Length();
    for (; p != end; ++p)
        {
        if (!TChar(*p).IsSpace())
            {
            return aText.Mid(p - aText.Ptr());
            }
        }
    // All of aText was whitespace: return empty string
    return TPtrC();
    }

    TPtrC StripLeadingWhiteSpace(const TDesC& aText)
        {
        return StripLeadingWhiteSpace_inline(aText);
        }
    
    TPtrC FindNextNonEmptyKey
            (const MSortKeyArray& aKeys, 
            TSortKeyType aType, 
            TInt& aIndex)
        {
        TPtrC result;
        while (aIndex < aKeys.SortKeyCount())
            {
            const TSortKey key(aKeys.SortKeyAt(aIndex));
            ++aIndex;
            if (key.Type() == aType)
                {
                result.Set(StripLeadingWhiteSpace_inline(key.Text()));
                if (result.Length() > 0)
                    {
                    return result;
                    }
                }
            }
        return result;
        }

    }  // namespace SortUtilImpl

// End of file
