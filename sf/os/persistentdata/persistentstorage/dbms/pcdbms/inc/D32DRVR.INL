// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// common DBMS driver functionality
// 
//


template <class T,class Factory>
inline T* AttachContext(Factory* aF,T* aT)
	{
    return (T*)aF->Attach(aT);
    }

// Class CDbContext
inline TInt CDbContext::Ref() const
	{
    return iRef;
    }
