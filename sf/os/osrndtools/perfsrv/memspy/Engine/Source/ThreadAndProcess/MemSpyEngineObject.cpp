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


#include <memspy/engine/memspyengineobject.h>


EXPORT_C CMemSpyEngineObject::CMemSpyEngineObject()
    {
    Open();
    }


EXPORT_C CMemSpyEngineObject::CMemSpyEngineObject( CMemSpyEngineObject& aParent )
:   iParent( &aParent )
    {
    Open();
    }


CMemSpyEngineObject::~CMemSpyEngineObject()
    {
    }


EXPORT_C TInt CMemSpyEngineObject::AccessCount() const
    {
    __ASSERT_ALWAYS( iAccessCount >= 0, User::Invariant() );
    return iAccessCount;
    }


EXPORT_C void CMemSpyEngineObject::Close()
    {
    __ASSERT_ALWAYS( iAccessCount >= 0, User::Invariant() );
    if  ( --iAccessCount <= 0 )
        {
        delete this;
        }
    }


EXPORT_C void CMemSpyEngineObject::Open()
    {
    __ASSERT_ALWAYS( iAccessCount >= 0, User::Invariant() );
    ++iAccessCount;
    }


EXPORT_C CMemSpyEngineObject* CMemSpyEngineObject::Parent() const
    {
    return iParent;
    }


EXPORT_C void CMemSpyEngineObject::SetParent( CMemSpyEngineObject* aParent )
    {
    iParent = aParent;
    }
