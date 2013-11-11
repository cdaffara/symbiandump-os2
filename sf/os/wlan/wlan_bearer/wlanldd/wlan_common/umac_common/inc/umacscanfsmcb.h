/*
* Copyright (c) 2005-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   scan fsm callback interface layer
*
*/

/*
* %version: 5 %
*/

#ifndef M_WLANSCANFSMCB_H
#define M_WLANSCANFSMCB_H

class MWlanScanFsmCb 
    {

public:

    enum TEvent 
        {
        EFSMSTARTSCANNINGMODE,
        EFSMSCANMODERUNNING,
        EFSMSTOPSCANNINGMODE,
        EFSMSCANMODESTOPPED
        };

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    virtual void OnScanFsmEvent( TEvent aEvent ) = 0;


protected:



    };


#endif // M_WLANSCANFSMCB_H
