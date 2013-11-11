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






#ifndef SORTUTILELAF_H
#define SORTUTILELAF_H

//  INCLUDES
#include "SortUtil.h"

// CLASS DECLARATION

/**
 * MSortUtil ELAF implementation.
 */
NONSHARABLE_CLASS(TSortUtilElaf) : public MSortUtil
    {
    public:
        TSortUtilElaf();
        ~TSortUtilElaf();

    public: // from MSortUtil
        TInt CompareItems(const MSortKeyArray& aLhs, const MSortKeyArray& aRhs) const;
    };

#endif // SORTUTILELAF_H

// End of File
