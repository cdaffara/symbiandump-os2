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
* Description:
*
*/

#ifndef MEMSPYENGINEOBJECT_H
#define MEMSPYENGINEOBJECT_H

// System includes
#include <e32base.h>


NONSHARABLE_CLASS( CMemSpyEngineObject ) : public CBase
    {
public:
    IMPORT_C CMemSpyEngineObject();
    IMPORT_C CMemSpyEngineObject( CMemSpyEngineObject& aParent );

protected:
    ~CMemSpyEngineObject();

public: // API
    IMPORT_C TInt AccessCount() const;
    IMPORT_C virtual void Close();
    IMPORT_C virtual void Open();
    IMPORT_C CMemSpyEngineObject* Parent() const;
    IMPORT_C void SetParent( CMemSpyEngineObject* aParent );
    
protected: // Internal API
    inline TBool OpenOrCloseInProgress() const { return iOpenOrCloseInProgress; }
    inline void SetOpenOrCloseInProgress( TBool aOpenOrCloseInProgress ) { iOpenOrCloseInProgress = aOpenOrCloseInProgress; }

private:
    TInt iAccessCount;
    CMemSpyEngineObject* iParent;
    TBool iOpenOrCloseInProgress;
    };




#endif