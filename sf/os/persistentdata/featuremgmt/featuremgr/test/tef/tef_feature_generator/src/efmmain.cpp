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

#include "efmmain.h"
#include "tef_efm_suitedefs.h"
#include "common.h"


TVerdict CEFMFnStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual. Our implementation only gets called
 * if the base class doTestStepPreambleL() did not leave. That being the case,
 * the current test result value will be EPass.
 */
    {
	TPtrC cfgblk(ConfigSection());
	//
	// First look for an operation we know about.
	for(TInt count=0 ; ; count++)
		{
        TPtrC argument;
        TInt whatfun=Efn_undefined;
        for(TInt i=0 ; i < Efn_undefined ; i++)
            {
            // Construct something like 'LoadFromIni23'
			// This probably shouldn't be hardwired to 25 chars max..
            TBuf<25> stfn(*(iactionhsh->GetStringFromNum(i)));
            stfn.AppendNum(count);

			// Attempt to fetch that from the .ini file. Try the next
			// possibility it if fails.
            if(!GetStringFromConfig(cfgblk, stfn, argument))
                continue;
			whatfun = i;
			break;
		    }
		if( (whatfun == Efn_undefined) || (whatfun == Ectrl_endblock))
			break;
	    switch(whatfun)
	    	{
	    	case Efn_nop:
                 break;
    		case Efn_undefined:
                 break;
    		case Ectrl_endblock:
                 break;
			case Efn_setndsrs:  // Set the number of DSRs explicitly. Used
				                // to create bad data files for robustness
				                // testing. The correct value could be
				                // reinstated by checking the number of DSRs
				                // in the internal array.
				 if(ifeatset == NULL)
					{
					ERR_PRINTF1(_L("You must load a feature set before attempting to modify it."));
					break;
					}

				 // Limit the scope of the following..
				    {
					TLex tl = argument;
					TInt ndsrs;
					tl.Val(ndsrs);
					ifeatset->SetNDSRs(ndsrs);
				    }
				 break;

			case Efn_loadfromini:  // argument will be a new block in the .ini
				 if(ifeatset != NULL)
					delete ifeatset;
	             ifeatset = CreateFeatureSetFromIni(argument);
                 break;

			case Efn_loadfromdat:  // argument will be a data file name.
				 if(ifeatset != NULL)
					delete ifeatset;
	             ifeatset = CreateFeatureSetFromFileL(argument);
                 break;
            
			case Efn_loadfromcfg:  // argument will be a data file name.
				if(ifeatset != NULL)
					delete ifeatset;
				ifeatset = CreateFeatureSetFromLegacyFileL(argument);
				break;     

			case Efn_writefile:  // argument will be a file name.
				 WriteToFileL(argument);
                 break;

			case Efn_compareini:  // argument will be a new block in the .ini
				 {
				 if(ifeatset == NULL)
				     {
                     SetTestStepResult(EFail);
                     ERR_PRINTF1(_L("You must load a feature set before attempting a comparison"));
					 break;;
				     }
	             CFeatSet *myfs1 = CreateFeatureSetFromIni(argument);
				 // If this didn't work an error would already have been
				 // reported.
				 if(myfs1 == NULL)
                     break;
	             TInt rc = ifeatset->Compare(*myfs1);
	             if(rc != CFeatSet::EFeatMatch)
                     {
                     SetTestStepResult(EFail);
                     ERR_PRINTF2(_L("Comparison of pre-loaded feature set and ini block failed. Error %d"), rc);
					 break;
                     }
				 delete myfs1;
                 break;
				 }

			case Efn_comparedat:  // argument will be a data file name.
				 {
				 if(ifeatset == NULL)
				     {
                     SetTestStepResult(EFail);
                     ERR_PRINTF1(_L("You must load a feature set before attempting a comparison"));
					 break;;
				     }
	             CFeatSet *myfs1 = CreateFeatureSetFromFileL(argument);
				 // If this didn't work an error would already have been
				 // reported.
				 if(myfs1 == NULL)
                     break;
	             TInt rc = ifeatset->Compare(*myfs1);
	             if(rc != CFeatSet::EFeatMatch)
                     {
                     SetTestStepResult(EFail);
                     ERR_PRINTF2(_L("Comparison of pre-loaded feature set and file failed. Error %d"), rc);
                     }
				 delete myfs1;
                 break;
				 }

			default: User::Panic(_L("Logic error in efmmain.cpp"), EUnknownFunction);
					 break;
					 
	    	}
    	}
    return TestStepResult();
	}

void CEFMFnStep::WriteToFileL(const TPtrC& afile)
	{
    if(ifeatset == NULL)
        {
        SetTestStepResult(EFail);
        ERR_PRINTF1(_L("You must load a feature set before attempting to save it to a file"));
		return;
        }
	TInt rc = ifeatset->WriteToFileL(afile);
	if(rc != KErrNone)
		{
        ERR_PRINTF3(_L("Error %d attempting to write to file %S"), rc, &afile);
        SetTestStepResult(EFail);
		}
    return;
    }

//
// Fetch the 'header' information from the '.ini' file. Note that a some of
// these default to sensible values if not specified.
//
CFeatSet* CEFMFnStep::CreateFeatureSetFromIni(const TPtrC& acfgblk)
    {
//    _LIT(KTestFunction, "LoadHeaderFromIni");

    // First create a 'blank' feaure set object.
	CFeatSet *featset = new CFeatSet();
    TInt err;
	if(featset == NULL)
		{
        ERR_PRINTF1(_L("Unable to create CFeatSet object.") );
        SetTestStepResult(EFail);
		return NULL;
		}

    // Look for the 'TypeField' item in the configuration file. If it
    // isn't there it doesn't matter, it defaults correctly anyway.
    _LIT(KTEFCfgTypeField, "TypeField");
    TPtrC typefield;
    if(GetStringFromConfig(acfgblk, KTEFCfgTypeField, typefield))
        {
        TInt len = typefield.Length();
        if((err = featset->SetTypeField(typefield)) != KErrNone)
            {
            ERR_PRINTF2(_L("Unable to set 'typefield' in CFeatSet object. Error %d"), err);
            SetTestStepResult(EFail);
			delete featset;
            return NULL;
            }
        }

    // Now get the file version number. Again, it doesn't matter if it isn't
    // specified. Assume this is in decimal, not hex.
    TInt arg2;
    _LIT(KTEFCfgFileVersion, "FileVersion");
    if(GetIntFromConfig(acfgblk, KTEFCfgFileVersion, arg2))
        featset->SetFileVersion(arg2);

    // Now get the file flags. Again, it doesn't matter if it isn't
    // specified. Assume this is in decimal, not hex.
    _LIT(KTEFCfgFileFlags, "FileFlags");
    if(GetIntFromConfig(acfgblk, KTEFCfgFileFlags, arg2))
        featset->SetFileFlags(arg2);

    // Now get the number of listed features. It does matter if it isn't
    // specified. 
    _LIT(KTEFCfgNFeatures, "NFeatures");
    if(GetIntFromConfig(acfgblk, KTEFCfgNFeatures, arg2))
        featset->SetNFeatures(arg2);
    else
        {
        ERR_PRINTF2(_L("Failed to specify '%S' required parameter."),
                        &KTEFCfgNFeatures);
        SetTestStepResult(EFail);
		delete featset;
        return NULL;
        }

    // Now get the number of listed 'default supported range' sections (DSRs).
    // It does matter if it isn't specified. 
    _LIT(KTEFCfgNDSRs, "NDSRs");
    if(GetIntFromConfig(acfgblk, KTEFCfgNDSRs, arg2))
        featset->SetNDSRs(arg2);
    else
        {
        ERR_PRINTF2(_L("Failed to specify '%S' required parameter."),
                        &KTEFCfgNDSRs);
        SetTestStepResult(EFail);
		delete featset;
        return NULL;
        }

	// This is broken into a separate function just for readability.
	// Any errors will already have been reported, so don't report here.
	if(LoadFeatureFlagsFromIni(acfgblk, featset) != KErrNone)
		{
        SetTestStepResult(EFail);
		delete featset;
        return NULL;
		}

	// This is broken into a separate function just for readability.
	// Any errors will already have been reported, so don't report here.
	if(LoadDSRsFromIni(acfgblk, featset) != KErrNone)
		{
        SetTestStepResult(EFail);
		delete featset;
        return NULL;
		}

    return featset;
    }

//
// This method reads feature flag information from the config block passed in and
// adds that information to the feature set object also passed in.
// This method does not delete the feature set object if things go wrong,
// that is the responsibility of the calling method.
//
// This method is provided as a convenience function for the above
// 'CreateFeatureSetFromIni'.
//
TInt CEFMFnStep::LoadFeatureFlagsFromIni(const TPtrC& acfgblk, CFeatSet *afeatset)
    {
	TInt err;
//    _LIT(KTestFunction, "LoadFeatureFlagsFromIni");

    _LIT(KTEFCfgFeature, "Feature");
    _LIT(KTEFCfgFeatUID, "UID");
    _LIT(KTEFCfgFeatStat, "Status");
    _LIT(KTEFCfgFeatUDW, "UDW");  // Feature 'user data word'

	// Assumes we've already loaded up the header.
	TInt nfeatures = afeatset->GetNFeatures();
    for(TInt i=1 ; i<=nfeatures ; ++i )
        {
        // Construct a string item like 'Feature1'..
        TBuf<20> cfgitempfx(KTEFCfgFeature);
        cfgitempfx.AppendNum(i);
        // Now construct another one, 'Feature1UID' etc..
        TBuf<20> cfgitemuid(cfgitempfx);
        cfgitemuid.Append(KTEFCfgFeatUID);
        // Now construct another one, 'Feature1Status' etc..
        TBuf<20> cfgitemstatus(cfgitempfx);
        cfgitemstatus.Append(KTEFCfgFeatStat);
        // Now construct another one, 'Feature1UDW' etc..
        TBuf<20> cfgitemudw(cfgitempfx);
        cfgitemudw.Append(KTEFCfgFeatUDW);

        // Prepare some variables..
        TInt uid, status, udw;

        // Get them from the configuration file..
        if(!GetHexFromConfig(acfgblk, cfgitemuid, uid))
            {
            ERR_PRINTF2(_L("Failed to find expected config item '%S'"),
                            &cfgitemuid );
            SetTestStepResult(EFail);
            return KErrNotFound;
            }
        // Status will probably need a more 'english' method of entering
        // the status flag, a 32-bit hex value just isn't enough. Soon..
        if(!GetHexFromConfig(acfgblk, cfgitemstatus, status))
            {
                    // idea - fetch it as a string, pass the string to a
                    // clever constructor in the CEFMFeatFlag class.
            ERR_PRINTF2(_L("Failed to find expected config item '%S'"),
                            &cfgitemstatus );
            SetTestStepResult(EFail);
            return KErrNotFound;
            }
        if(!GetHexFromConfig(acfgblk, cfgitemudw, udw))
            {
            ERR_PRINTF2(_L("Failed to find expected config item '%S'"),
                            &cfgitemudw );
            SetTestStepResult(EFail);
            return KErrNotFound;
            }

		// If this fails don't delete the featset object, the calling method must
		// clean up.
        if((err = afeatset->AddFeatureFlag(uid, status, udw)) != KErrNone)
            {
            ERR_PRINTF2(_L("Failed to add a new feature to the feature set, error %d."), err);
            SetTestStepResult(EFail);
            return err;
            }
        }
    return KErrNone;
    }

//
// This method reads DSR information from the config block passed in and
// adds that information to the feature set object also passed in.
// This method does not delete the feature set object if things go wrong,
// that is the responsibility of the calling method.
//
// This method is provided as a convenience function for the above
// 'CreateFeatureSetFromIni'.
//
TInt CEFMFnStep::LoadDSRsFromIni(const TPtrC& acfgblk, CFeatSet *afeatset)
    {
//    _LIT(KTestFunction, "LoadDSRsFromIni");
    TInt err;

    _LIT(KTEFCfgDSR, "DSR");
    _LIT(KTEFCfgDSRLowUID, "LowUID");
    _LIT(KTEFCfgDSRHighUID, "HighUID");

    // Assumes we've already loaded up the header...
	TInt ndsrs = afeatset->GetNDSRs();
    for(TInt i=1 ; i<=ndsrs ; ++i )
        {
        // Construct a string item like 'DSR1'..
        TBuf<20> cfgitempfx(KTEFCfgDSR);
        cfgitempfx.AppendNum(i);
        // Now construct another one, 'DSR1LowUID' etc..
        TBuf<20> cfgitemlowuid(cfgitempfx);
        cfgitemlowuid.Append(KTEFCfgDSRLowUID);
        // Now construct another one, 'Feature1Status' etc..
        TBuf<20> cfgitemhighuid(cfgitempfx);
        cfgitemhighuid.Append(KTEFCfgDSRHighUID);

        // Prepare some variables..
        TInt lowuid, highuid;

        // Get them from the configuration file..
        if(!GetHexFromConfig(acfgblk, cfgitemlowuid, lowuid))
            {
            ERR_PRINTF2(_L("Failed to find expected config item '%S'"),
                            &cfgitemlowuid );
            SetTestStepResult(EFail);
            return KErrNotFound;
            }
        if(!GetHexFromConfig(acfgblk, cfgitemhighuid, highuid))
            {
            ERR_PRINTF2(_L("Failed to find expected config item '%S'"),
                            &cfgitemhighuid );
            SetTestStepResult(EFail);
            return KErrNotFound;
            }

        // Now create a new 'CEFMDSR' object to hold the information we've
        // gathered.
        // Should we be worrying about two phase construction? This is a test
        // and if we're that short of memory everything is going to fall over
        // anyway..
        if((err = afeatset->AddDSR(lowuid, highuid)) != KErrNone)
            {
            ERR_PRINTF2(_L("Failed to add a new DSR to the feature set, error %d."), err);
            SetTestStepResult(EFail);
            return err;
            }
        }
    return KErrNone;
    }

//
// Read the content of an existing feature manager data file into a new
// object and return that or NULL if things go wrong.
//
CFeatSet* CEFMFnStep::CreateFeatureSetFromFileL(const TPtrC &afile)
    {
	TInt err;
	// Create a new feature set object.
	CFeatSet *featset = new CFeatSet();
	if(featset == NULL)
		{
        ERR_PRINTF1(_L("Unable to create CFeatSet object.") );
        SetTestStepResult(EFail);
		return NULL;
		}
	CleanupStack::PushL(featset);

	// Load in the content from the file.
	if((err = featset->LoadFromFileL(afile)) != KErrNone)
		{
        ERR_PRINTF2(_L("Unable to load feature set from file, error %d."), err );
        SetTestStepResult(EFail);
		delete featset;
		return NULL;
		}
	CleanupStack::Pop(featset);
    return featset;
    }

//
// Read the content of an existing feature registry data file into a new
// object and return that or NULL if things go wrong.
//
CFeatSet* CEFMFnStep::CreateFeatureSetFromLegacyFileL(const TPtrC &afile)
    {
	TInt err;
	// Create a new feature set object.
	CLegacyFeatureSet *featset = new CLegacyFeatureSet();
	if(featset == NULL)
		{
        ERR_PRINTF1(_L("Unable to create CFeatSet object.") );
        SetTestStepResult(EFail);
		return NULL;
		}
	CleanupStack::PushL(featset);
	// Load in the content from the file.
	if((err = featset->LoadFromFileL(afile)) != KErrNone)
		{
        ERR_PRINTF2(_L("Unable to load feature set from file, error %d."), err );
        SetTestStepResult(EFail);
		delete featset;
		return NULL;
		}
	CleanupStack::Pop(featset);
    return featset;
    }

