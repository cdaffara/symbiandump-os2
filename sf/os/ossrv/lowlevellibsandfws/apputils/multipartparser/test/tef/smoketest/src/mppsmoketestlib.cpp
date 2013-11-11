// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "mppsmoketestlib.h"
#include <flogger.h>
//#include "xmlengtesterdef.h"
//#include <utils/xmlengxestd.h>

/**
 * Utility fcn to compare two files ( skip or not skip white space ).
 * @param aSrcFile full filename of a file you want to check/compare.
 * @param aVerificationFile fill filename of a correct output file, aSrcFile will be compared to this to verify it's validity.
 * @param aSkipWhiteSpace do not include whitespace when comparing the two files.
 * @return Symbian OS error code.
 */
TInt CMultipartTestContainer::CompareFilesL( TPtrC aSrcFile, TPtrC aVerificationFile,
																				 TBool aSkipWhiteSpace)
{
    _LIT(KSourceFileError,"Source file error.");
    _LIT(KPatternFileError,"Pattern file error.");
    _LIT(KComparePassed,"Files compare test PASSED.");
    _LIT(KCompareFailed,"Files compare test FAILED.");
    
    TInt nResult = KErrNone;
    
    TBool skipWhite = FALSE;
    TBool foundRes = FALSE;
    TBool foundRef = FALSE;
    
    RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
    
    RFile srcFile;
    RFile referenceFile;

    TFileName srcFileName;
	TFileName referenceFileName;
    
  TPtrC SrcFile = aSrcFile;
  TPtrC ReferenceFile = aVerificationFile;
	if (aSkipWhiteSpace)
	 {
		 skipWhite = TRUE;
	 }	
    
    if ( nResult == KErrNone )
	{
		srcFileName.Copy(SrcFile);
		referenceFileName.Copy(ReferenceFile);

	    if(srcFile.Open(fs, srcFileName, EFileStream|EFileRead) == KErrNone)
		{
			// Does reference file exist.
			if(referenceFile.Open(fs, referenceFileName, EFileStream|EFileRead) == KErrNone)
		    {
		        // Integer variables for compare to length of files (result and reference).
		        TInt resSize;
		        TInt refSize;

		        srcFile.Size(resSize);
		        referenceFile.Size(refSize);

	            // Next compare one letter at the time, but only if files have same length.
	            if(skipWhite)
		        {
			        TBuf8<1> resBuf;
			        TBuf8<1> refBuf;
			        nResult = KErrNone;
			        TInt j = 0;
			        TInt i = 0;
			        
			        //for(TInt i = 0; i < Size; i++)
			        while (TRUE)
				    {
				    	foundRes = FALSE;
				    	foundRef = FALSE;
				        // Read result file
				        while(i < (resSize + 1))
				        {
				        	i++;
				        	srcFile.Read(resBuf);
				        	resBuf.Trim();	
				        	if ( resBuf.Length() > 0)
				        	{
				        		foundRes = TRUE;
				        		break;
				        	}
				        }
						
						// Read reference file
				        while(j < (refSize + 1))
				        {
				        	j++;
				        	referenceFile.Read(refBuf);
				        	refBuf.Trim();
				        	if ( refBuf.Length() > 0)
				        	{
				        		foundRef = TRUE;
				        		break;
				        	}
				        }
				        
				        // Compare single letter at the time.
				        if( ( i < resSize ) && ( j < refSize ) && (resBuf[0] != refBuf[0]) )
				        {
					        nResult = KErrGeneral;
					        break;
					    }
					    if( (i == (resSize + 1)) && (j < refSize) && foundRef)
				    	{
					    	nResult = KErrGeneral;
					        break;
				    	}
					    if( (i < resSize) && (j == (refSize + 1)) && foundRes)
				    	{
					    	nResult = KErrGeneral;
					        break;
				    	}
				    	if ((i > resSize) && (j > refSize))
				    		break;
				    }
			    }
		        else
			    {
			        if (resSize != refSize)
			       		nResult = KErrGeneral;
			        else
		        	{
				        TBuf8<1> resBuf;
			        	TBuf8<1> refBuf;
			        	nResult = KErrNone;
			        	for(TInt i = 0; i < resSize; i++)
			        	{
				        	// Read result file
				        	srcFile.Read(resBuf);

					        // Read reference file
					        referenceFile.Read(refBuf);

					        // Compare single letter at the time.

					        if(resBuf[0] != refBuf[0])
					        {
						        nResult = KErrGeneral;
					    	    break;
				        	}
			        	}
		        	}
			    }
	            referenceFile.Close();
	            srcFile.Close();
			}
			else
			{
				nResult = KErrGeneral;
				INFO_PRINTF1(KPatternFileError);
			}
			srcFile.Close();
		}
		else
		{
			nResult = KErrGeneral;
			INFO_PRINTF1(KSourceFileError);
		}
			
	}
	
	CleanupStack::PopAndDestroy(&fs);
	
	if ( nResult == KErrNone)
		{
		INFO_PRINTF1(KComparePassed);
		}
	else
		{
		INFO_PRINTF1(KCompareFailed);
		}

	return nResult;
    }

CMultipartTestContainer::CMultipartTestContainer(CTestExecuteLogger& aLogger)
 : iLog(aLogger)
    {
    }

/**
 * @param aLogger TEF logger from CTestStep.
 */
void CMultipartTestContainer::ConstructL()
	{
	}

/**
 * @param aLogger TEF logger from CTestStep.
 */
CMultipartTestContainer* CMultipartTestContainer::NewLC(CTestExecuteLogger& aLogger)
	{
	CMultipartTestContainer* self = new (ELeave) CMultipartTestContainer(aLogger);
    
	CleanupStack::PushL( self );
	self->ConstructL();

	return self;
    
	}
    
/**
 * @param aLogger TEF logger from CTestStep.
 */
CMultipartTestContainer* CMultipartTestContainer::NewL(CTestExecuteLogger &aLogger)
	{
	CMultipartTestContainer* self = NewLC(aLogger );
	CleanupStack::Pop( self );

	return self;
    
	}
    
CMultipartTestContainer::~CMultipartTestContainer()
	{ 
	}

/**
 * Read a file from the hard drive.
 * 
 * @param aFileName full filename to read in.
 * @return contents of the file.
 */ 
HBufC8* CMultipartTestContainer::ReadFileLC(const TDesC& aFileName)
    {
    RFs rfs;
    RFile file;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);
    User::LeaveIfError(file.Open(rfs, aFileName, EFileRead));
    CleanupClosePushL(file);
    TInt size;
    User::LeaveIfError(file.Size(size));
    HBufC8* buf = HBufC8::NewLC(size);
    TPtr8 bufPtr(buf->Des());
    User::LeaveIfError(file.Read(bufPtr));
    CleanupStack::Pop(); // buf
    CleanupStack::PopAndDestroy(2); // file, rfs
    CleanupStack::PushL(buf);
    return buf;
    }

/**
 * Write the file to the hard drive.
 * 
 * @param aFileName 
 * @param aBodyPartArray
 */ 
void CMultipartTestContainer::WriteFileL( const TDesC& aFileName,
                                           RPointerArray<CBodyPart>& aBodyPartArray )
    {
    RFs fs;
    fs.Connect();
    CleanupClosePushL(fs);
    RFile output;
    TInt err = output.Open(fs, aFileName, EFileWrite);
    if (err == KErrNotFound)
    	{
			User::LeaveIfError(output.Create(fs, aFileName, EFileWrite));
			}		
    
    // output file
    TInt size = aBodyPartArray.Count();
    TInt i;
		_LIT8(KEol, "\r\n");
    for (i = 0; i < size; i++)
        {

        CBodyPart* bodyPart = aBodyPartArray[i];

        if( bodyPart->Url().Ptr() )
            {
            output.Write(_L8("***************Ur"));
            output.Write(KEol);
			RBuf8 narrow;
			err = narrow.Create(bodyPart->Url().Length()*2);
			if (err != KErrNone)
				{
				INFO_PRINTF1(_L("Error printing Url to output file; continueing..."));
				}						
			narrow.Copy(bodyPart->Url());
            output.Write(narrow.Left(narrow.Length()));
            output.Write(KEol);
            }
        if( bodyPart->ContentID().Ptr() )
            {
            output.Write(_L8("***************ContentID"));
            output.Write(KEol);
            output.Write(bodyPart->ContentID() );
            output.Write(KEol);
            }
        if( bodyPart->Charset().Ptr() )
            {
            output.Write(_L8("***************Charset"));
            output.Write(KEol);
            output.Write( bodyPart->Charset() );
            output.Write(KEol);
            }
        if( bodyPart->ContentType().Ptr() )
            {
            output.Write(_L8("***************ContentType"));
            output.Write(KEol);
            output.Write( bodyPart->ContentType() );
            output.Write(KEol);
            }
        if( bodyPart->Headers().Ptr() )
            {
            output.Write(_L8("***************Headers"));
            output.Write(KEol);
            output.Write(bodyPart->Headers() );
            output.Write(KEol);
            }
        if( bodyPart->Body().Ptr() )
            {
            output.Write(_L8("***************Body"));
            output.Write(KEol);
            output.Write(bodyPart->Body() );
            output.Write(KEol);
            }
        output.Write(_L8("=========================================part ends"));
        output.Write(KEol);

        } // end of loop
    
    output.Close();
    CleanupStack::PopAndDestroy(1, &fs);
    fs.Close();
		}
		

/**
 * Write the data to a file.
 * 
 * @param aFileName  filename to write the data to.
 * @param aData data to write.
 */
void CMultipartTestContainer::WriteToDiskL( const TDesC& aFileName,
                                            const TDesC8& aData )
    {
    RFs fsSession;
    RFile file;
    TInt status( KErrNone );
    User::LeaveIfError( fsSession.Connect() );
    status = file.Replace( fsSession, aFileName, EFileWrite|EFileShareAny);    
    if( status == KErrNone )
        {
        file.Write( aData );
        }
    file.Close();
    }


/**
 * Parse a file with base64 encoding.
 * 
 * @param aFile filename to be read in.
 * @param aUrl  ???  //DEBUG
 * @param aOutput name out output file.
 */ 
void CMultipartTestContainer::ParseFileL( const TDesC& aFile,
                                          const TDesC& aUrl,
                                          const TDesC& aOutput )
    {
    // body
    HBufC8* data = ReadFileLC( aFile );
    // content type
    _LIT8(KMultipartMixed, "multipart/mixed");
    // boundary
    _LIT8(KBoundary1, "KBoundary");
    // body part array
    RPointerArray<CBodyPart> bodyPartArray;  

    // parse
		TInt err;
    if( aUrl.Ptr() )
        {
        TRAP( err, MultipartParser::ParseL( *data, KMultipartMixed, KBoundary1, aUrl, bodyPartArray ) );
        }
    else
        {
        TRAP( err, MultipartParser::ParseL( *data, KMultipartMixed, KBoundary1, aFile, bodyPartArray ) );
        }
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Error parsing file."));
		}			

    // output
    WriteFileL( aOutput, bodyPartArray);

    // clean up memory
    CleanupStack::PopAndDestroy(); // data
    // clean up body part array and its content
    TInt size = bodyPartArray.Count();
    TInt i;
    for (i = 0; i < size; i++)
        {
        delete bodyPartArray[i];
        }
    bodyPartArray.Reset();
    }
