// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __EFEATSET_H__)
#define __EFEATSET_H__

#include<e32base.h>
#include<f32file.h>

#include "efmfeatflag.h"
#include "efmdsr.h"

// The first four bytes of a feature set data file.
_LIT8(KFeatSetDataFileMarker, "feat");

class CFeatSet : public CBase
    {
public:
    CFeatSet()
        {    
        infeatures=0; indsrs=0;
        ifeat.Copy(KFeatSetDataFileMarker);
        ifileversion = idefaultfileversion;
        ifileflags = idefaultfileflags;
        }
    virtual ~CFeatSet();

    //
    // Getters and setters.
    //
    TInt GetTypeField(TPtrC8 &);
    TInt SetTypeField(const TPtrC &); // Note TPtrC, not TPtrC8
    TInt GetFileVersion();
    TInt SetFileVersion(const TInt&);
    TInt GetFileFlags();
    TInt SetFileFlags(const TInt&);
    TInt GetNFeatures();
    TInt SetNFeatures(const TInt &);
    TInt GetNDSRs();
    TInt SetNDSRs(const TInt &);
	CEFMFeatFlag* GetFeatureElement(const TInt &aidx);
	CEFMDSR* GetDSRElement(const TInt &aidx);

    //
    // These two methods create new feature flag or DSR objects and place
    // them in the internal array.
    //
    TInt AddFeatureFlag(const TUint32 &, const TUint32 &, const TUint32 &);
    TInt AddDSR(const TUint32 &, const TUint32 &);

    //
    // These methods create the binary equivalent of the data stored in this
    // object.
    //
    TInt HeaderBinaryContent(TDes8 &);
    TInt BinaryContent(RBuf8 &);

    //
    // Importers and exporters.
    //
    TInt WriteToFileL(const TPtrC &);
    virtual TInt LoadFromFileL(const TPtrC &);
    virtual TInt ImportFromBuffer(const TUint8 *, const TInt &);

    //
    // Utilities
    //
    TInt Compare(CFeatSet &);

    //
    // Return codes for comparison. These have to be public so the owning
    // object can see them.
    //
    enum TFeatCompReturnCodes { ETypeMismatch, EFileVersionMismatch,
                                EFileFlagsMismatch, ENFeatMismatch,
                                ENDSRMismatch, EFeatMismatch,
                                EDSRMismatch, EFeatMatch };

	// Couldn't these be private?
    RArray<CEFMFeatFlag> ifeatures;
    RArray<CEFMDSR> idsrs;

protected:
    TBuf8<4> ifeat;
    static const TInt16 idefaultfileversion = 1;
    static const TInt16 idefaultfileflags = 0;

    TUint16 ifileversion, ifileflags;
    TUint32 infeatures, indsrs;


    };

class CLegacyFeatureSet : public CFeatSet
    {
public:
	CLegacyFeatureSet():CFeatSet() 
        {    
        }
    virtual ~CLegacyFeatureSet()
    {
    }
  
    virtual TInt LoadFromFileL(const TPtrC &);     
    virtual TInt ImportFromBuffer(const TUint8 *, const TInt &);
    };
    
#endif  //__EFEATSET_H__
