/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:     Korean SortUtil header file
*
*/



#ifndef SORTUTILKOREAN_H
#define SORTUTILKOREAN_H

//  INCLUDES
#include "SortUtil.h"

// CLASS DECLARATION

/**
 * MSortUtil Korean implementation.
 */
NONSHARABLE_CLASS(TSortUtilKorean) : public MSortUtil
    {
    public:
        
        TSortUtilKorean();
        virtual ~TSortUtilKorean();

    public: // from MSortUtil
        
        TInt CompareItems( 
            const MSortKeyArray& aLhs, 
            const MSortKeyArray& aRhs ) const;
    };

#endif // SORTUTILKOREAN_H

// End of File
