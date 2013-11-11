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

#if (!defined __EFMMAIN_H__)
#define __EFMMAIN_H__
#include <test/testexecutestepbase.h>
#include "tef_efm_suitestepbase.h"

#include "efeatset.h"
#include "efmfeatflag.h"
#include "efmdsr.h"
#include "hashing.h"


class CEFMFnStep : public CTe_EFM_SuiteStepBase
    {
public:
    CEFMFnStep()
           {
        ifeatset = NULL;
        iactionhsh = new CEFMTEFAction();    
        };
    ~CEFMFnStep()
           {
        if(ifeatset != NULL)
              delete ifeatset;
        delete iactionhsh;
        };
    virtual TVerdict doTestStepL();

    //
    // Methods to set up the required objects based on the content on .ini
    // files.
    //
    CFeatSet* CreateFeatureSetFromIni(const TPtrC &);
    TInt LoadFeatureFlagsFromIni(const TPtrC &, CFeatSet *);
    TInt LoadDSRsFromIni(const TPtrC &, CFeatSet *);

    //
    // A method to write out the current feature set.
    //
    void WriteToFileL(const TPtrC &);

    //
    // A method to create a feature set from an existing feature set data file.
    //
    CFeatSet *CreateFeatureSetFromFileL(const TPtrC &);

    //
    // A method to create a feature set from an existing feature registry binary file.
    //
    CFeatSet *CreateFeatureSetFromLegacyFileL(const TPtrC &);
    
    //
    // An enumeration to associate words with actions.
    //
    enum TEFMTEFActions { Efn_nop, Efn_writefile,
    						Efn_loadfromini, Efn_loadfromdat,
    						Efn_loadfromcfg, Efn_comparedat, 
    						Efn_setndsrs,
    						Efn_compareini, Ectrl_endblock, 
    						Efn_undefined,
    };

private:
    // Perhaps this could be an array. It depends what you want the test
    // harness to do..
    CFeatSet *ifeatset;

    // This hash associates words with actions.
    CEFMTEFAction *iactionhsh;

protected:
    };

// This word is what appears on the 'RUN_TEST_STEP' line..
_LIT(KEFMTest, "CEFMTest");


#endif  //__EFMMAIN_H__
