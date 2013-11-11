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
* Description:  MWlanBgScanCommandListener class declaration.
*
*/

/*
* %version: 1 %
*/

#ifndef M_WLANBGSCANCOMMANDLISTENER_H
#define M_WLANBGSCANCOMMANDLISTENER_H

#include <e32base.h>

/**
 *  MWlanBgScanCommandListener class.
 *  
 *  Command queue uses this interface to deliver the received
 *  commands to listener. The listener has to derive from this 
 *  class and implement the defined methods.
 * 
 *  @since S60 v5.2
 */
 
NONSHARABLE_CLASS( MWlanBgScanCommandListener )
    {
public:
    
    /**
     * DoSetInterval.
     * This method is called by the command queue when
     * SetInterval request is processed.
     * 
     * @since S60 v5.2
     * @param aNewInterval new interval to be taken into use
     */
    virtual void DoSetInterval( TUint32 aNewInterval ) = 0;
    };
    
#endif // M_WLANBGSCANCOMMANDLISTENER_H
