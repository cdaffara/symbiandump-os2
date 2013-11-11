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

#include "efeatset.h"

//
// The code in this file implements the 'CFeatSet' class which, when
// populated represents the data stored within a feature set data file.
// It uses 'CEFMDSR' objects (default supported ranges) and 'CEFMFeatFlag'
// (feature flag) objects. There may be any number of these other objects
// held by this object.
// In addition to the above, a 'CFeatSet' object holds header information
// that can be written to (or read from) a feature set data file and this
// class implements methods to write this information to files or import
// this information from files.
//


/**
 * Destructor
 *
 * Destroys the arrays (RArrays) of feature flags and default supported
 * ranges.
 */
CFeatSet::~CFeatSet()
    {
    ifeatures.Close();
    idsrs.Close();
    }

// This getter method sets the value of the TPtrC passed in to the
// file type field for this feature set data file. This will normally be
// "feat".
TInt CFeatSet::GetTypeField(TPtrC8 &aval)
    {
//    _LIT(KTestFunction, "GetTypeField");
    aval.Set(ifeat);
    return KErrNone;
    }
// This setter method sets the file type field to the value passed in -
// normally "feat".
TInt CFeatSet::SetTypeField(const TPtrC& argument)
    {
//    _LIT(KTestFunction, "SetTypeField");
    TInt len = argument.Length();
    if(len != 4) return KErrArgument;
    ifeat.Copy(argument);
    return KErrNone;
    }
// This getter method returns the file version set within this object.
// This defaults to 1.
TInt CFeatSet::GetFileVersion()
    {
//    _LIT(KTestFunction, "GetFileVersion");
    return ifileversion;
    }
// This setter method sets the file version within this object.
TInt CFeatSet::SetFileVersion(const TInt& av)
    {
//    _LIT(KTestFunction, "SetFileVersion");
    ifileversion = av;
    return ifileversion;
    }
// This getter method returns the file flags within this object.
// This defaults to 0.
TInt CFeatSet::GetFileFlags()
    {
//    _LIT(KTestFunction, "GetFileFlags");
    return ifileflags;
    }
// This setter method returns the file flags within this object.
TInt CFeatSet::SetFileFlags(const TInt& aff)
    {
//    _LIT(KTestFunction, "SetFileFlags");
    ifileflags = aff;
    return ifileflags;
    }
// This getter method returns the number of feature flags we have. 
TInt CFeatSet::GetNFeatures()
    {
//    _LIT(KTestFunction, "GetNFeatures");
    return infeatures;
    }
// This setter method sets the number of feature flags we have. 
TInt CFeatSet::SetNFeatures(const TInt &aff)
    {
//    _LIT(KTestFunction, "SetNFeatures");
    infeatures = aff;
    return infeatures;
    }
// This getter method returns the number of default supported ranges we have. 
TInt CFeatSet::GetNDSRs()
    {
//    _LIT(KTestFunction, "GetNDSRs");
    return indsrs;
    }
// This setter method sets the number of default supported ranges we have. 
TInt CFeatSet::SetNDSRs(const TInt &aff)
    {
//    _LIT(KTestFunction, "SetNDSRs");
    indsrs = aff;
    return indsrs;
    }

// This method creates a new 'CEFMFeatFlag' object using the data that is
// passed in to create it, and adds that new object to the internal
// array of feature set objects. Note that this method does not modify
// the 'infeatures' counter (which tells us how many 'CEFMFeatFlag' objects
// we should end up with).
TInt CFeatSet::AddFeatureFlag(const TUint32 &auid, const TUint32 &astatus,
                              const TUint32 &audw ) 
    {
    CEFMFeatFlag *featflag = new CEFMFeatFlag(auid, astatus, audw);
    if(featflag == NULL)
        {
        return KErrNoMemory;
        }
    ifeatures.Append(*featflag);
    return KErrNone;
    }

// This method creates a new 'CEFMDSR' object using the data that is
// passed in to create it, and adds that new object to the internal
// array of default supported range objects. Note that this method does not
// modify the 'indsrs' counter (which tells us how many 'CEFMDSRs' objects
// we should end up with).
TInt CFeatSet::AddDSR(const TUint32 &alowuid, const TUint32 &ahighuid )
    {
    CEFMDSR *dsr = new CEFMDSR(alowuid, ahighuid);
    if(dsr == NULL)
        {
        return KErrNoMemory;
        }
    idsrs.Append(*dsr);
    return KErrNone;
    }
//
// This method expects a descriptor as an argument and places in it the
// binary form (as found in a feature set data file) of the header information
// based on information already internal to this object.
// The descriptor should have sufficient space for the data inserted here -
// at least 16 bytes.
//
TInt CFeatSet::HeaderBinaryContent(TDes8 &abuf)
    {
    abuf.Copy(ifeat);
    
    TAny *p = &ifileversion;
    abuf.Append((TUint8 *)p, sizeof(ifileversion));
    
    p = &ifileflags;
    abuf.Append((TUint8 *)p, sizeof(ifileflags));
    
    p = &infeatures;
    abuf.Append((TUint8 *)p, sizeof(infeatures));
    
    p = &indsrs;
    abuf.Append((TUint8 *)p, sizeof(indsrs));
    
    return KErrNone;
    }

// 
// CFeatSet::BinaryContent(RBuf8 &);
//
// Set up an 'RBuf8' object (which is passed in by reference) to contain the
// binary equivalent of the information loaded into this and the feature flag
// and default supported range objects.
//
// NOTE: Perhaps passing an RBuf8 around isn't the best way..
//
TInt CFeatSet::BinaryContent(RBuf8 &athecontent)
    {
    // First calculate how much space we're going to need.
    TInt size = 16 + infeatures*12 + indsrs*8;

    // RBuf8 temporary build area. Close it in case it's been used before.
    athecontent.Close();

    TInt err;
    // Create space for the binary.
    if((err = athecontent.Create(size)) != KErrNone) return err;
    // We don't want to close the RBuf8, so no CleanupClose here.

    // Load up the header information.
    if((err = HeaderBinaryContent(athecontent)) != KErrNone)
        {
        athecontent.Close();
        return err;
        }

    // Create a 12 byte buffer for feature flag information.
    TBuf8<12> tb;

    // For each of the feature flag objects, load up the binary equivalent.
    for(TInt i=0 ; i<ifeatures.Count() ; ++i )
        {
        // Load up the binary content of the feature into 'tb'.
        err = ifeatures[i].BinaryContent(tb);
        if(err)
            {
            athecontent.Close();
            return err;
            }
        // Append it to our 'RBuf'.
        athecontent.Append(tb);
        }
    // For each of the 'default support range' objects, load up the binary
    // equivalent. Use the same temporary buffer 'tb', it's big enough.
    for(TInt i=0 ; i<idsrs.Count() ; ++i )
        {
        // Load up the binary content of the feature into 'tb'.
        err = idsrs[i].BinaryContent(tb);
        if(err)
            {
            athecontent.Close();
            return err;
            }

        // Append it to our 'RBuf'.
        athecontent.Append(tb);
        }

    return KErrNone;
    }

//
// This method creates a new feature set data file (filename passed in)
// based on the information already set up within this object. Note that
// if the file already exists it will be deleted.
//
// Note: Should this file return an error code?
//
TInt CFeatSet::WriteToFileL(const TPtrC &afile)
    {
    TInt err;
    // Create a connection to the file system.
    RFs thefs;
    if((err = thefs.Connect()) != KErrNone)
        return err;
    CleanupClosePushL(thefs);
    thefs.ShareProtected(); // Do we need this?
    // Delete the file. We don't care if it's there already, and ignore the
    // error code because it may not be present anyway.
    thefs.Delete(afile);

    // Create the file.
    RFile thefile;
    if((err = thefile.Create(thefs, afile, EFileWrite)) != KErrNone)
        {
        CleanupStack::PopAndDestroy(1, &thefs);
        return err;
        }
    CleanupClosePushL(thefile);

    // Get the binary content for this feature set data file.
    RBuf8 content;
    if((err = BinaryContent(content)) != KErrNone)
        {
        CleanupStack::PopAndDestroy(2, &thefs);
        return err;
        }
    CleanupClosePushL(content);

    // Write to the file.
    if((err = thefile.Write(content)) != KErrNone)
        {
        CleanupStack::PopAndDestroy(3, &thefs);
        return err;
        }
    CleanupStack::PopAndDestroy(3, &thefs);
    return KErrNone;
    }


//
// Read the content of an existing feature manager data file into this
// object and create feature set and DSR objects as required.
//
TInt CFeatSet::LoadFromFileL(const TPtrC &afile)
    {
    // Open the file system.
    RFs thefs;
    TInt err;
    if((err = thefs.Connect()) != KErrNone)
        return err;
    CleanupClosePushL(thefs);
    thefs.ShareProtected(); // Do we need this?

    // Open the file.
    RFile thefile;
    if((err = thefile.Open(thefs, afile, EFileRead)) != KErrNone)
        {
        CleanupStack::PopAndDestroy(1, &thefs);
        return err;
        }
    CleanupClosePushL(thefile);

    // How big is the file?
    TInt fsize;
    if((err = thefile.Size(fsize)) != KErrNone)
		{
		CleanupStack::PopAndDestroy(2);
        return err;
		}

    // Check that the file size is believable. Just look if it has enough for
    // the header information and is a whole multiple of 4 bytes in length.
    if((fsize < 16) || (fsize&3))
        {
        CleanupStack::PopAndDestroy(2);
        return KErrCorrupt;
        }
    
    // Allocate a buffer for the file contents.
    RBuf8 filecontent;

    // Create space for the file contents. Add a byte because the 'PtrZ'
    // method we're about to use adds a terminating byte.
    if((err = filecontent.Create(fsize+1)) != KErrNone)
        {
        CleanupStack::PopAndDestroy(2);
        return err;
        }
    CleanupClosePushL(filecontent);

    // Read the file into the buffer.
    if((err = thefile.Read(filecontent)) != KErrNone)
        {
        CleanupStack::PopAndDestroy(3);
        return KErrCorrupt;
        }
    
    // Set up everything based on the file contents.
    err = ImportFromBuffer(filecontent.PtrZ(), fsize);
    CleanupStack::PopAndDestroy(3);
    return err;
    }

//
// Load into this object the information provided in the buffer.
// The buffer is of size 'asize', this method will fail if the
// calculated size does not match the size passed in.
//
TInt CFeatSet::ImportFromBuffer(const TUint8 *abuf, const TInt& asize)
    {
    // First read in the basics. The file type field..
    ifeat.Copy(abuf, 4);
    
    // The file version...
    TUint16 *p16 = (TUint16 *) (abuf+4);
    ifileversion = *p16;

    // The file flags...
    p16 = (TUint16 *) (abuf + 6);
    ifileflags = *p16;

    // The number of listed features...
    TUint32 *p32 = (TUint32 *) (abuf+8);
    infeatures = *p32;

    // The number of default supported ranges... Increment the 32-bit
    // pointer, we're going to use that to load up the feature flags
    // and default supported ranges sections.
    p32 = (TUint32 *) (abuf + 12);
    indsrs = *p32++;

    // Don't go any further if the buffer size does not match the information
    // we now have,
    TInt expsize = 16 + 12*infeatures + 8*indsrs;
    if(expsize != asize)
        return KErrCorrupt;
    
    // Remove any previously loaded feature flag entries.
    ifeatures.Close();

    // Set up the new ones.
    for(TInt i=0 ; i<infeatures ; i++ )
        {
        TInt uid, status, udw = 0;
        uid = *p32++;
        status = *p32++;
        udw = *p32++;
        CEFMFeatFlag *featflag = new CEFMFeatFlag(uid, status, udw);
        if(featflag != NULL)
            ifeatures.Append(*featflag);
        else
            return KErrNoMemory;
        }

    // Remove any previously loaded default supported range objects.
    idsrs.Close();
    for(TInt i=0 ; i<indsrs ; i++ )
        {
        TInt lowuid, highuid = 0;
        lowuid = *p32++;
        highuid = *p32++;
        CEFMDSR *dsr = new CEFMDSR(lowuid, highuid);
        if(dsr != NULL)
            idsrs.Append(*dsr);
        else
            return KErrNoMemory;
        }
    return KErrNone;
    }

// This method returns a pointer to the 'CEFMFeatFlag' object stored at
// index 'aidx' within the internal feature flags array.
CEFMFeatFlag* CFeatSet::GetFeatureElement(const TInt &aidx)
	{
	if(aidx > ifeatures.Count())
	    return NULL;
	return &ifeatures[aidx];
	}

// This method returns a pointer to the 'CEFMDSR' object stored at
// index 'aidx' within the internal default supported ranges array.
CEFMDSR* CFeatSet::GetDSRElement(const TInt &aidx)
	{
	if(aidx > idsrs.Count())
	    return NULL;
	return &idsrs[aidx];
	}

// This method compares this 'CFeatSet' object to another one that is
// passed in. It first checks the header information and then compares
// each of the feature flag and DSR objects using their comparison methods.
TInt CFeatSet::Compare(CFeatSet &a2)
    {
	TPtrC8 a2feat;
	a2.GetTypeField(a2feat);
	if(ifeat.Compare(a2feat))
		return ETypeMismatch;

	if(GetFileVersion() != a2.GetFileVersion())
		return EFileVersionMismatch;

	if(GetFileFlags() != a2.GetFileFlags())
		return EFileFlagsMismatch;

	if(GetNFeatures() != a2.GetNFeatures())
		return ENFeatMismatch;

	if(GetNDSRs() != a2.GetNDSRs())
		return ENDSRMismatch;

    for(TInt i=0 ; i<ifeatures.Count() ; ++i )
        {
		CEFMFeatFlag *comp = a2.GetFeatureElement(i);
		if(!(ifeatures[i].Compare(*comp)))
			return EFeatMismatch;
        }

    for(TInt i=0 ; i<idsrs.Count() ; ++i )
        {
		CEFMDSR *comp = a2.GetDSRElement(i);
		if(!(idsrs[i].Compare(*comp)))
			return EDSRMismatch;
        }

	return EFeatMatch;
	}

//
// Read the content of an existing feature registry config file into this
// object and create feature set and DSR objects as required.
//
TInt CLegacyFeatureSet::LoadFromFileL(const TPtrC &afile)
    {
    // Open the file system.
    RFs thefs;
    TInt err;
    if((err = thefs.Connect()) != KErrNone)
        return err;
    CleanupClosePushL(thefs);
    thefs.ShareProtected(); // Do we need this?

    // Open the file.
    RFile thefile;
    if((err = thefile.Open(thefs, afile, EFileRead)) != KErrNone)
        {
        CleanupStack::PopAndDestroy(1, &thefs);
        return err;
        }
    CleanupClosePushL(thefile);

    // How big is the file?
    TInt fsize;
    if((err = thefile.Size(fsize)) != KErrNone)
        return err;

    // Check that the file size is believable. Just look if it has enough for
    // the header information and is a whole multiple of 4 bytes in length.
    if((fsize < 16) || (fsize&3))
        {
        CleanupStack::PopAndDestroy(2, &thefs);
        return KErrCorrupt;
        }
    
    // Allocate a buffer for the file contents.
    RBuf8 filecontent;

    // Create space for the file contents. Add a byte because the 'PtrZ'
    // method we're about to use adds a terminating byte.
    if((err = filecontent.Create(fsize+1)) != KErrNone)
        {
        CleanupStack::PopAndDestroy(2, &thefs);
        return err;
        }
    CleanupClosePushL(filecontent);

    // Read the file into the buffer.
    if((err = thefile.Read(filecontent)) != KErrNone)
        {
        CleanupStack::PopAndDestroy(3, &thefs);
        return KErrCorrupt;
        }
    
    // Set up everything based on the file contents.
    err = ImportFromBuffer(filecontent.PtrZ(), fsize);
    CleanupStack::PopAndDestroy(3, &thefs);
    return err;
    }

//
// Load into this object the information provided in the buffer.
// The buffer is of size 'asize', this method will fail if the
// calculated size does not match the size passed in.
//
TInt CLegacyFeatureSet::ImportFromBuffer(const TUint8 *abuf, const TInt& asize)
    {
    // First read in the basics. The file type field..
    ifeat.Copy(abuf, 4);
    
    // The file version...
    ifileversion = 1;//always set the file version to EFM file version

    // The file flags...
    TUint16* p16 = (TUint16 *) (abuf + 6);
    ifileflags = *p16;

    // The number of listed features...
    TUint32 *p32 = (TUint32 *) (abuf+8);
    infeatures = *p32;

    // The number of default supported ranges... Increment the 32-bit
    // pointer, we're going to use that to load up the feature flags
    // and default supported ranges sections.
    p32 = (TUint32 *) (abuf + 12);
    indsrs = *p32++;

    // Don't go any further if the buffer size does not match the information
    // we now have,
    TInt expsize = 16 + 8*infeatures + 8*indsrs;
    if(expsize != asize)
        return KErrCorrupt;
    
    // Remove any previously loaded feature flag entries.
    ifeatures.Close();

    // Set up the new ones.
    for(TInt i=0 ; i<infeatures ; i++ )
        {
        TInt uid, status = 0;
        uid = *p32++;
        status = *p32++;

        CEFMFeatFlag *featflag = new CEFMFeatFlag(uid, status, 0); //data field is not supported by Feature registryy file format, so set it to 0 
        if(featflag != NULL)
            ifeatures.Append(*featflag);
        else
            return KErrNoMemory;
        }

    // Remove any previously loaded default supported range objects.
    idsrs.Close();
    for(TInt i=0 ; i<indsrs ; i++ )
        {
        TInt lowuid, highuid = 0;
        lowuid = *p32++;
        highuid = *p32++;
        CEFMDSR *dsr = new CEFMDSR(lowuid, highuid);
        if(dsr != NULL)
            idsrs.Append(*dsr);
        else
            return KErrNoMemory;
        }
    return KErrNone;
    }
