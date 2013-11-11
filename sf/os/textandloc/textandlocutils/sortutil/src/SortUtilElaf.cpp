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
#include "SortUtilElaf.h"
#include "SortUtilImpl.h"
#include "SortUtilImplExport.h"

inline TSortUtilElaf::TSortUtilElaf()
    {
    }

// Only export in DLL
EXPORT_C MSortUtil* SortUtilFactoryFunctionL()
    {
    MSortUtil* util = new (ELeave) TSortUtilElaf;
    return util;
    }

TSortUtilElaf::~TSortUtilElaf()
    {
    }

TInt TSortUtilElaf::CompareItems
        (const MSortKeyArray& aLhs, 
        const MSortKeyArray& aRhs) const
    {
    TInt result = 0;
    TInt lhsIndex = 0;
    TInt rhsIndex = 0;
    do  
        {
        const TDesC& lhsText = 
            SortUtilImpl::FindNextNonEmptyKey(aLhs, ESortKeyBasic, lhsIndex);
        const TDesC& rhsText = 
            SortUtilImpl::FindNextNonEmptyKey(aRhs, ESortKeyBasic, rhsIndex);
        result = lhsText.CompareC(rhsText);
        if (result != 0)
            {
            return result;
            }
        }
    while (lhsIndex <= aLhs.SortKeyCount() && rhsIndex < aRhs.SortKeyCount());
    return result;
    }
// End of file
