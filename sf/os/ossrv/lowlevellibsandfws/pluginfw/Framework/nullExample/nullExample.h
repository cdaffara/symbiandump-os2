// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef CINTERFACEIMPL_H
#define CINTERFACEIMPL_H

//  INCLUDES
#include <e32base.h>
#include "Interface.h"

// CLASS DECLARATION

class CInterfaceImpl : public CExampleInterface
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @return A new instance of this class.
        */
        static CInterfaceImpl* NewL(TAny* aConstructionParams);
        
        /**
        * Destructor.
        */
        virtual ~CInterfaceImpl();
        void DoMethodL();
        TInt FireAndForget();
        TUid ImplId();

    public: // New functions
        
        TInt Version() const;

    protected:

        /**
        * C++ default constructor.
        */
        CInterfaceImpl();

    private:

        // By default, prohibit copy constructor
        CInterfaceImpl( const CInterfaceImpl& );
        // Prohibit assigment operator
        CInterfaceImpl& operator= ( const CInterfaceImpl& );
        
        void RunL();
		void DoCancel();
		TInt RunError(TInt aError);

    };

#endif      // CINTERFACEIMPL_H  
            
// End of File
