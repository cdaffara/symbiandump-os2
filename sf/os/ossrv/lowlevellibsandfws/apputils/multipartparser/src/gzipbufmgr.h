// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @internalComponent
// @prototype
//

#ifndef GZIPBUFMGR_H
#define GZIPBUFMGR_H

//-----------------------------------------------------------------------------
#include <e32base.h>
#include <ezbufman.h>
#include <ezgzip.h>
#include <s32mem.h>

//-----------------------------------------------------------------------------
class CGzipStream;
class RMemReadStream;

//-----------------------------------------------------------------------------
class GZipBufMgr : public MEZBufferManager
    {
    public:
        // Constructor and Destructor
        static GZipBufMgr* NewL(TPtrC8 in);
        virtual ~GZipBufMgr();

        // Overridden virtual methods from MEZBufferManager
        void InitializeL(CEZZStream &aZStream);
        void NeedInputL(CEZZStream &aZStream);
        void NeedOutputL(CEZZStream &aZStream);
        void FinalizeL(CEZZStream &aZStream);
        
        // Public methods
        void ReadGZipHeaderL(TEZGZipHeader& aHeader);
        void ReadStringIntoDescriptorL(RMemReadStream& aMem, HBufC8 **aDes) const;
        void SetGZipBuffer(TPtrC8 data);

    private:
        void ConstructL();
        GZipBufMgr(TPtrC8 in);

        // Always points to start of the input Buffer
        TPtrC8	iInput;

        // Always points to start of the output Buffer
        TPtr8	iOutputDes;
        HBufC8*	iOutput;
        
        // Used by the virtual methods from MEZBufferManager
        TBool	iNeedInput;
        TBool	iNeedOutput;
        TBool	iFinalized;

        // Marks the start of the data, after the header
        TUint   iOffset;

        // Magic GZip numbers
        TUint8  iID1;
        TUint8  iID2;
    };

#endif // GZIPBUFMGR_H
