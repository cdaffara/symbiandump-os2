// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32const.h>
#include <e32debug.h>
#include <bautils.h>
#include <s32file.h>
#include "CentRepConvTool.h"
#include "shrepos.h"
#include "inifile.h"
#include "srvres.h"
#include "srvparams.h"


_LIT(KCentRepConvTool, "CentRep Conversion Tool:");

const TInt KNumDigitsInUID = 8;

//
// factory method
CCentRepConvTool* CCentRepConvTool::NewL(const TDesC& aCmd, RFs& aFs, TBool aWaitForAck)
	{
	CCentRepConvTool* self = new(ELeave) CCentRepConvTool(aCmd, aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aWaitForAck);
	CleanupStack::Pop(self);
	return self;
	}

//
// Constructor
CCentRepConvTool::CCentRepConvTool(const TDesC& aCmd, RFs& aFs)
	: iCmd(aCmd), iFs(aFs), iTextToBin(ETrue), iRepUid(KNullUid),
	  iMyDataCage(KNullDesC), iDefaultPath(KNullDesC)
	{
	}

//
// two phase construct
void CCentRepConvTool::ConstructL(TBool aWaitForAck)
	{
	iScrnOutput = CConsolePrint::NewL(aWaitForAck);
	iFs.PrivatePath(iMyDataCage);
	
	// set default I/O path
	_LIT(KDefaultPathMask, "_:\\");
	iDefaultPath.Copy(KDefaultPathMask);
	iDefaultPath[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
	}

//
// destructor
CCentRepConvTool::~CCentRepConvTool()
	{
	delete iScrnOutput;
	delete iCentRepShrepos;
	}

//
// setter
void CCentRepConvTool::SetOutputMode(TBool aWaitForAck)
	{
	iScrnOutput->SetWaitMode(aWaitForAck);
	}

//
// Extract input and output path from cmd line, determine text to
// binary or binary to text, and invoke code in CentRep server classes
// to do conversion.
void CCentRepConvTool::ProcessCmdL()
	{
	_LIT(KSuccessMsg,"Output saved as: %S\r\n");
	
	TPtrC inputPath(KNullDesC);
	TPtrC outputPath(KNullDesC);
	ParseCmdLineL(inputPath, outputPath);

	// default input & output path is system drive root folder

	iInputPath.Set(inputPath, NULL, NULL);
	if (!iInputPath.DrivePresent() && !iInputPath.PathPresent())
		{		
		iInputPath.Set(inputPath, NULL, &iDefaultPath);
		}

	iOutputPath.Set(outputPath, NULL, NULL);
	if (!iOutputPath.DrivePresent() && !iOutputPath.PathPresent())
		{		
		iOutputPath.Set(outputPath, NULL, &iDefaultPath);
		}

	// VerifyInputPathL must be call before VerifyOutputPathL!
	VerifyInputPathL();
	VerifyOutputPathL();
	
	// Get UID from input filename
	TLex lex(iInputPath.Name());
	TUint32 intvalue;
	lex.Val(intvalue, EHex);
	iRepUid.iUid = intvalue;

	TRAPD(err, DoConversionL());
	if (err != KErrNone)    
	    {    
        if (err == KErrCorrupt)    
            {    
            _LIT(KCorruptError, "Input file contains corrupted entries.\r\n");    
            RDebug::Print(KCentRepConvTool);    
            RDebug::Print(KCorruptError);    
            iScrnOutput->Printf(KCorruptError);    
            }    
        User::Leave(err);    
        } 

	// Success
	iScrnOutput->Printf(KSuccessMsg, &(iOutputPath.FullName()));
	}

//
void CCentRepConvTool::DoConversionL()
	{
	iCentRepShrepos = CSharedRepository::NewL(iRepUid);

	if (iTextToBin)
		{
		HBufC* tempIniFileName = iInputPath.FullName().AllocLC();
		CIniFileIn* iniFile = NULL;
		TInt ret=CIniFileIn::NewLC(iFs,iniFile,*tempIniFileName);
		if (ret==KErrCorrupt)
			User::LeaveIfError(iFs.Delete(*tempIniFileName));
		User::LeaveIfError(ret);
		User::LeaveIfError(iCentRepShrepos->ReloadContentL(*iniFile));
		CleanupStack::PopAndDestroy(2); // tempIniFileName, iniFile
		ExternalizeToCreL();
		}
	else
		{
		CDirectFileStore* store = CDirectFileStore::OpenLC(iFs,
			iInputPath.FullName(), EFileRead|EFileShareReadersOnly);
		if (store->Type()[0] != KDirectFileStoreLayoutUid)
			{
			CleanupStack::PopAndDestroy(store);
			User::Leave(KErrCorrupt);
			}		

		// Get the root stream and attempt to read the index from it
		TStreamId rootStreamId = store->Root() ;
		RStoreReadStream rootStream ;
		rootStream.OpenLC(*store, rootStreamId);
		// Internalize the repository
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS		
		TUint8 creVersion;
#endif		
		iCentRepShrepos->InternalizeCreL(rootStream
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS		
		,creVersion
#endif		
		);
		CleanupStack::PopAndDestroy(&rootStream);
		CleanupStack::PopAndDestroy(store);
		
		iCentRepShrepos->DoCommitChangesToIniFileL(iOutputPath.FullName()
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS		
		,creVersion
#endif		
		);
		}
	}

//
// Extract input path and output path from cmd line
void CCentRepConvTool::ParseCmdLineL(TPtrC& aInputPath, TPtrC& aOutputPath)
	{
	_LIT(KNoWaitSwitch, "-nowait");
	_LIT(KOutpathSwitch, "-o");
	_LIT(KHelpSwitch, "-h");
	_LIT(KBadArg, "Bad input arguments: %S\r\nUsage: CentRepConv [-o output_path]  [input_path\\]<repositoryUID>.txt\r\n");
	_LIT(KHelpMsg, "Usage: CentRepConv [-o output_path]  [input_path\\]<repositoryUID>.txt\r\nDefault output_path=%S\r\nDefault input_path=%S\r\n");
	
	const TInt KMaxNumTokens = 8; // Arbitrary. only expect 4. 
	TPtrC tokens[KMaxNumTokens];
	TLex lex(iCmd);
	TInt i;
	for (i = 0; !lex.Eos() && i < KMaxNumTokens; i++)
		{
		tokens[i].Set(lex.NextToken());
		}

	TInt numTokens = i;

	// Expect: [-nowait] [-o output_path] [input_path\]<rep_UID>.txt
	for (i = 0; i < numTokens; i++)
		{
		if (tokens[i].CompareF(KOutpathSwitch) == 0)
			{
			// Got the -o switch.
			if ((i+2) < numTokens)
				{
				aOutputPath.Set(tokens[i+1]);
				aInputPath.Set(tokens[i+2]);
				}
			break;	
			} // tokens[i] == "-o"
		else if (tokens[i].CompareF(KNoWaitSwitch) == 0)
			{
			SetOutputMode(EFalse);
			continue;
			}
		else if (tokens[i].FindF(KHelpSwitch) == 0)
			{
			numTokens = 0; // indicator for help message
			break;
			}
		else if ('-' == tokens[i][0])
			{
			continue; // unknown switch, assume intended for system
			}

		// No options. Token must be input file name.
		aInputPath.Set(tokens[i]);
		break;
		} // for
	
	if (0 == numTokens)
		{
		RDebug::Print(KCentRepConvTool);
		RDebug::Print(KHelpMsg, &iDefaultPath, &iDefaultPath);

		iScrnOutput->Printf(KHelpMsg, &iDefaultPath, &iDefaultPath);
		User::Leave(KErrArgument);
		}

	// aOutputPath can be blank but aInputPath is mandatory.
	if (aInputPath.Length() == 0)
		{
		RDebug::Print(KCentRepConvTool);
		RDebug::Print(KBadArg, &iCmd);

		iScrnOutput->Printf(KBadArg, &iCmd);
		User::Leave(KErrArgument);
		}
	}

//
// Validate the input filenames.
void CCentRepConvTool::VerifyInputPathL()
	{
	// Check input file is .txt or .cre
	iTextToBin = (iInputPath.Ext().CompareF(*TServerResources::iIniExt) == 0);
	TBool binInput = !iTextToBin && (iInputPath.Ext().CompareF(*TServerResources::iCreExt) == 0);
	
	if (!iTextToBin && !binInput)
		{
		_LIT(KBadExt, "Bad input filename: %S\r\nInput file extension must be %S or %S\r\n");
		RDebug::Print(KCentRepConvTool);
		RDebug::Print(KBadExt, &iCmd, TServerResources::iIniExt,
			TServerResources::iCreExt);

		iScrnOutput->Printf(KBadExt, &iCmd, TServerResources::iIniExt,
			TServerResources::iCreExt);
		User::Leave(KErrArgument);	
		}
	
	// check input filename is 8 hex digits.
	TPtrC p(iInputPath.Name());
	TBool validName = (KNumDigitsInUID == p.Length());
	
	if (validName)
		{
		TLex lex(p);
		for (TInt i = 0; validName && i<KNumDigitsInUID; i++)
			{
			if (lex.Peek().IsHexDigit())
				{
				lex.Inc();
				}
			else
				{
				validName = EFalse;
				}				
			} // for
		} // if validName
		
	if (!validName)	
		{
		_LIT(KBadRepUid, "Input filename in %S is not a valid UID.\r\nExpect 8 hex digits.\r\n");
		RDebug::Print(KCentRepConvTool);
		RDebug::Print(KBadRepUid, &iCmd);

		iScrnOutput->Printf(KBadRepUid, &iCmd);
		User::Leave(KErrArgument);
		}
		
	p.Set(iInputPath.FullName());

	if ( InOthersPrivatePath(p) )
		{
		_LIT(KInfileCaged, "Cannot access input file %S because it is in private data cage.\r\n");
		RDebug::Print(KCentRepConvTool);
		RDebug::Print(KInfileCaged, &p);

		iScrnOutput->Printf(KInfileCaged, &p);
		User::Leave(KErrAccessDenied);
		}

	if (!BaflUtils::FileExists(iFs, p))
		{
		_LIT(KInFileNotExists, "Input file %S does not exist.\r\n");
		RDebug::Print(KCentRepConvTool);
		RDebug::Print(KInFileNotExists, &p);

		iScrnOutput->Printf(KInFileNotExists, &p);
		User::Leave(KErrNotFound);
		}
	}

//
// Validate the output filenames.
void CCentRepConvTool::VerifyOutputPathL()
{
	// If output filename is not specified, fill in the missing parts.
	TBool EmptyOutFileName = EFalse;
	
	TPtrC outFileName(iOutputPath.Name());
	if (outFileName.Length())
		{
		if (0 != outFileName.CompareF(iInputPath.Name()))
			{
			_LIT(KUnmatchFilenames, "Bad input: %S\r\nInput filename does not match output filename.\r\n");
			RDebug::Print(KCentRepConvTool);
			RDebug::Print(KUnmatchFilenames, &iCmd);

			iScrnOutput->Printf(KUnmatchFilenames, &iCmd);
			User::Leave(KErrArgument);
			}
		}
	else
		{
		EmptyOutFileName = ETrue;
		outFileName.Set(iInputPath.Name()); 
		}
	
	
	TPtrC correctExt(*TServerResources::iCreExt);
	TPtrC outFileExt(iOutputPath.Ext());
	if (!iTextToBin)
		{
		correctExt.Set(*TServerResources::iIniExt);
		}
		
	if (outFileExt.Length())
		{
		// If output filename is specified, extension should match conversion.
		if (0 != outFileExt.CompareF(correctExt))
			{
			_LIT(KUnmatchExt, "Bad input: %S\r\nExtension of output filename not valid.\r\n");
			RDebug::Print(KCentRepConvTool);
			RDebug::Print(KUnmatchExt, &iCmd);

			iScrnOutput->Printf(KUnmatchExt, &iCmd);
			User::Leave(KErrArgument);
			}
		}
	else
		{
		EmptyOutFileName = ETrue;
		outFileExt.Set(correctExt);
		}
		
	TPtrC p;

	if (EmptyOutFileName)
		{
		const TInt KFilePlusExtLen = KNumDigitsInUID + 4; // e.g. 01234567.cre
		TBuf<KFilePlusExtLen> newName(outFileName);
		newName.Append(correctExt);
		
		p.Set(iOutputPath.DriveAndPath());
		TParse newpath;
		newpath.Set(newName, NULL, &p);
		
		p.Set(newpath.FullName());
		iOutputPath.Set(p, NULL, NULL);
		}
	
	// Check output file is in other's private data cage
	p.Set(iOutputPath.FullName());	
	if ( InOthersPrivatePath(p) )
		{
		_LIT(KOutfileInDataCage, "Cannot write output file %S because it is in private data cage.\r\n");
		RDebug::Print(KCentRepConvTool);
		RDebug::Print(KOutfileInDataCage, &p);

		iScrnOutput->Printf(KOutfileInDataCage, &p);
		User::Leave(KErrAccessDenied);
		}

	// Check saving output to read only drive.
	p.Set(iOutputPath.DriveAndPath());	
	TBool isReadOnly;
	TInt ret;
	ret = BaflUtils::DiskIsReadOnly(iFs, p, isReadOnly);
	if (ret == KErrNone)
		{
		if (isReadOnly)
			{
			_LIT(KWriteToReadOnly, "Bad argument: output dir %S is in read only drive.\r\n");
			RDebug::Print(KCentRepConvTool);
			RDebug::Print(KWriteToReadOnly, &p);

			iScrnOutput->Printf(KWriteToReadOnly, &p);
			User::Leave(KErrArgument);
			}
		}

	ret = iFs.MkDirAll(p);
	if (ret != KErrNone && ret != KErrAlreadyExists)
		{
		_LIT(KCreateOutpathFail, "Create output path %S failed, err %d.\r\n");
		RDebug::Print(KCentRepConvTool);
		RDebug::Print(KCreateOutpathFail, &p, ret);

		iScrnOutput->Printf(KCreateOutpathFail, &p, ret);
		User::Leave(ret);
		}
	}

//
// Open a RWriteStream on top of a CDirectFileStore. Call
// CSharedRepository's ExternalizeCre method to write the 
// settings to file.
void CCentRepConvTool::ExternalizeToCreL()
	{
	// Create file store
	CDirectFileStore* store = CDirectFileStore::ReplaceLC(iFs,
		iOutputPath.FullName(), (EFileWrite | EFileShareExclusive));
	const TUid uid2  = KNullUid;
	store->SetTypeL(TUidType(KDirectFileStoreLayoutUid, uid2, KServerUid3)) ; 
		
	// Write the stream index/dictionary as root stream within the store
	// so we can access it when we do a restore later on
	RStoreWriteStream rootStream ;
	TStreamId rootStreamId = rootStream.CreateLC(*store) ;
	iCentRepShrepos->ExternalizeCre(rootStream);
	rootStream.CommitL();
		
	CleanupStack::PopAndDestroy(&rootStream) ;
	store->SetRootL(rootStreamId);
	store->CommitL();
	CleanupStack::PopAndDestroy(store); 	
	}

//
// Check if given path is located in some other process's
// private data cage
TBool CCentRepConvTool::InOthersPrivatePath(const TDesC& aFullPathName)
	{
	_LIT(KPrivate, "\\private\\");
	const TInt KPosAfterDrive = 2;
	return aFullPathName.FindF(KPrivate) == KPosAfterDrive &&
		   aFullPathName.FindF(iMyDataCage) == KErrNotFound;
	}
