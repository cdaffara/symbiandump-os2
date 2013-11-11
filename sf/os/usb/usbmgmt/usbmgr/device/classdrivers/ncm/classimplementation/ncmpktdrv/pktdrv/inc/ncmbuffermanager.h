// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
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
//


/**
@file
@internalComponent
*/


#ifndef NCMBUFFERMANAGER_H
#define NCMBUFFERMANAGER_H

#include <e32base.h>
#include "ncmpktdrvcommon.h"



/**
Responsible for share chunk buffer managerment
*/

NONSHARABLE_CLASS(CNcmBufferManager) : public CBase

    {
public:
    static CNcmBufferManager* NewL();
    ~CNcmBufferManager();
    /**
    * get a free buffer block
    *
    * @param aBuffer to store the buffer block
    * @return KErrNone        if success
    *         KErrCongestion  if there is not enough buffer block, should start flow control
    */
    TInt GetBuffer(TNcmBuffer& aBuffer);
    /**
    * free a buffer block
    *
    * @param aBuffer store the buffer block to free
    */
    void FreeBuffer(const TNcmBuffer& aBuffer);
    void InitBufferArea(TAny* aBuf, TInt aLength);
    /**
    * get a free buffer block
    *
    * @param aSize new ntb size
    * @return KErrNone        if success
    *         KErrUnknown  if too big ntb size, can't create enought buffer block
    */    
    TInt SetBufferCellSize(TInt aSize);
    TBool IsCongestion();
    TInt RequiredBufferCount();	
    inline void SetAlignSize(TInt aSize);	
    inline TInt FreeBufferCount();		

private:
    CNcmBufferManager();

private:
    /**
    * array of free buffer blocks 
    */
    RArray<TNcmBuffer> iFreeQueue;
    /**
    * share chunk LDD buffer area 
    */
    TUint8*            iBuf;
    TInt               iLen;
    TInt               iCellSize;
    TInt               iAlignSize;	
    /**
    * there is enough buffer blocks or not 
    */
    TBool              iIsCongestion;
    
    };

inline void CNcmBufferManager::SetAlignSize(TInt aSize)
    {
    iAlignSize = aSize;
    }

inline TInt CNcmBufferManager::FreeBufferCount()
    {
    return iFreeQueue.Count();
    }


#endif    //NCMBUFFERMANAGER_H

