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






#ifndef SORTUTILJAPAN_H
#define SORTUTILJAPAN_H

//  INCLUDES
#include "SortUtil.h"

// CLASS DECLARATION

/**
 * MSortUtil Japanese implementation.
 */
NONSHARABLE_CLASS(TSortUtilJapan) : public MSortUtil
    {
    public:
        TSortUtilJapan();
        virtual ~TSortUtilJapan();

    public: // from MSortUtil
        TInt CompareItems(const MSortKeyArray& aLhs, const MSortKeyArray& aRhs) const;
    };

#endif // SORTUTILJAPAN_H

// End of File
