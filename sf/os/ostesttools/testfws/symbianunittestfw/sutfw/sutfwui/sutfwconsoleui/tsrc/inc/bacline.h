// BACLINE.H
//
// Copyright (c) 1997-1999 Symbian Ltd.  All rights reserved.
//

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
* Description: Stub for unit testing  
*
*/

#ifndef BACLINESTUB_H
#define BACLINESTUB_H

#include <e32base.h>
#include <badesca.h>

class CCommandLineArguments : public CBase
    {
    public:
	    // construct/destruct
        static CCommandLineArguments* NewLC();
        static CCommandLineArguments* NewL();
        ~CCommandLineArguments();
	    // extract
        TPtrC Arg(TInt aArg) const;
        TInt Count() const;
    
    public: // For unit testing purposes, not in the real implementation 
        CDesCArray& Args();
        
    private:
        CCommandLineArguments();
        void ConstructL();
    
    private: // For unit testing purposes, not in the real implementation
        CDesCArray* iArgs;
    };

#endif
