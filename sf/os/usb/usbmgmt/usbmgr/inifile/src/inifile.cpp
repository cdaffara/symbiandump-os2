/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines the class CIniFile for accessing ini file.
*
*/

/**
 @file
*/

#include <f32file.h>
#include <usb/usblogger.h>
#include "inifile.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "inifileTraces.h"
#endif


const TUint KTokenSize = 32;
_LIT(KDefaultIniFileDir,"\\");

void CIniFile::Panic(TIniPanic aPanic)
	{
	_LIT(KIniData,"CIniFile");	
	OstTrace1( TRACE_NORMAL, CINIFILE_PANIC, "CIniFile::Panic;aPanic=%d", aPanic );
	User::Panic(KIniData,aPanic );
	}

CIniFile::CIniFile() 
 :	iPtr(NULL,0)
	{
    OstTraceFunctionEntry1( CINIFILE_CINIFILE_CONS_ENTRY, this );
	}

CIniFile::~CIniFile()
	{
	delete (TText*)iPtr.Ptr();
	delete iToken;
	delete iName;
	}

CIniFile* CIniFile::NewL(const TDesC& aName)
/**
 * Factory function for CIniFile.
 *
 * @param aName The name of the ini file to be used, e.g. "GPRSBTT.INI".
 */
	{
    OstTraceFunctionEntry0( CINIFILE_NEWL_TDESC_ENTRY );

	CIniFile* self = new(ELeave) CIniFile;
	CleanupStack::PushL(self);
	self->ConstructL(aName, KDefaultIniFileDir);
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CINIFILE_NEWL_TDESC_EXIT );
	return self;
	}

CIniFile* CIniFile::NewL(const TDesC& aName, const TDesC& aPath)
/**
 * Factory function for CIniFile that allows the user to specify both filename
 * and path
 *
 * @param aName The name of the ini file to be used, e.g. "GPRSBTT.INI".
 * @param aPath The location of the file e.g. "\\system\\data\\".
 */
 {
    OstTraceFunctionEntry0( CINIFILE_NEWL_TDESC_TDESC_ENTRY );

 	CIniFile* self = new(ELeave) CIniFile;
	CleanupStack::PushL(self);
	self->ConstructL(aName, aPath);
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CINIFILE_NEWL_TDESC_TDESC_EXIT );
	return self;	 	
 }
 
void CIniFile::ConstructL(const TDesC& aName, const TDesC& aPath)
/**
 * Allocate a buffer and Read file's contents into iPtr
 *
 * @param aName is the name of the ini file to be used, e.g. "REFTSY.INI"
 */
	{
    iToken = HBufC::NewL(KTokenSize+2);	// 2 extra chars for []

	RFs fs;
	TInt err;
	err = fs.Connect();	
	if(err < 0)
	    {
        OstTrace1( TRACE_NORMAL, CINIFILE_CONSTRUCTL, "CIniFile::ConstructL;fs.Connect() with error=%d", err );
        User::Leave(err);
	    }
	CleanupClosePushL(fs);

	TFindFile ff(fs);

	err = ff.FindByDir(aName, aPath);
	if(err < 0)
	    {
        OstTrace1( TRACE_NORMAL, CINIFILE_CONSTRUCTL_DUP1, "CIniFile::ConstructL;ff.FindByDir(aName, aPath) with error=%d", err );
        User::Leave(err);
	    }

	iName = ff.File().AllocL();
	
	RFile file;
	TInt size;
	err = file.Open(fs,*iName,EFileStreamText|EFileRead|EFileShareReadersOnly);
	if(err < 0)
	    {
        OstTrace1( TRACE_NORMAL, CINIFILE_CONSTRUCTL_DUP2, "CIniFile::ConstructL;file.Open(fs,*iName,EFileStreamText|EFileRead|EFileShareReadersOnly) with error=%d", err );
        User::Leave(err);
	    }
	CleanupClosePushL(file);
	
	err = file.Size(size);
	if(err < 0)
	    {
        OstTrace1( TRACE_NORMAL, CINIFILE_CONSTRUCTL_DUP3, "CIniFile::ConstructL;file.Size(size) with error=%d", err );
        User::Leave(err);
	    }


	TText* data = REINTERPRET_CAST(TText*, User::AllocL(size));
	iPtr.Set(data, size/sizeof(TText), size/sizeof(TText));
	TPtr8 dest(REINTERPRET_CAST(TUint8*,data), 0, size);
	err = file.Read(dest);
	if(err < 0)
	    {
        OstTrace1( TRACE_NORMAL, CINIFILE_CONSTRUCTL_DUP4, "CIniFile::ConstructL;file.Read(dest) with error=%d", err );
        User::Leave(err);
	    }

	TUint8* ptr = REINTERPRET_CAST(TUint8*,data);

	//
	// This is orderred as FEFF assuming the processor is Little Endian
	// The data in the file is FFFE.		PRR 28/9/98
	//
	if(size>= STATIC_CAST(TInt,sizeof(TText)) && iPtr[0]==0xFEFF)
   		{
		Mem::Copy(ptr, ptr+sizeof(TText), size-sizeof(TText));
		iPtr.Set(data, size/sizeof(TText)-1, size/sizeof(TText)-1);
		}
	else if(size)
		{
		TText* newdata = REINTERPRET_CAST(TText*,
			                              User::AllocL(size*sizeof(TText)));
		iPtr.Set(newdata, size, size);
		TInt i;
		for(i=0 ; i<size ; ++i)
			{
			iPtr[i] = ptr[i];
			}
		delete data;
		}

	CleanupStack::PopAndDestroy(); // file
	CleanupStack::PopAndDestroy(); // fs
	}

TBool CIniFile::FindVar(const TDesC &aSection,
						const TDesC &aVarName,
						TPtrC &aResult)
//
// Find a variable's value given a section name and a var name
//
	{
	__ASSERT_DEBUG(aSection.Length()<=(TInt)KTokenSize,Panic(ESectionNameTooBig));
	__ASSERT_DEBUG(aVarName.Length()<=(TInt)KTokenSize,Panic(EVarNameTooBig));

	TPtr sectionToken = iToken->Des();
	_LIT(KSectionTokenString,"[%S]");
	sectionToken.Format(KSectionTokenString,&aSection);
	TInt sectionStart = iPtr.Find(sectionToken);
	TInt ret = ETrue;
	if (sectionStart == KErrNotFound)
		{
		ret = EFalse;
		}
	else
		{		
		TPtrC section = iPtr.Mid(sectionStart);
		TInt endBracket = section.Find(TPtrC(_S("]")));
		if (endBracket == KErrNotFound)
			{
			ret = EFalse;
			}
		else
			{
			sectionStart += endBracket + 1;
			section.Set(iPtr.Mid(sectionStart));
			
			TInt sectionEnd = section.Find(TPtrC(_S("[")));
			if (sectionEnd == KErrNotFound)
				{
				sectionEnd = iPtr.Length() - sectionStart;
				}
			else
				{
				sectionEnd--;
				}
			section.Set(iPtr.Mid(sectionStart,sectionEnd));
			TPtr varToken = iToken->Des();
			_LIT(KVarTokenString,"%S=");
			varToken.Format(KVarTokenString,&aVarName);
			TInt pos = section.Find(varToken);
			if (pos == KErrNotFound)
				{
				ret = EFalse;
				}
			else
				{
				// 'lex' points at the start of the data
				TPtrC lex(section.Mid(pos));
				TInt startpos = lex.Locate(TChar('='));
				startpos++; // startpos points immediately after the =.
				while ( TChar(lex[startpos]).IsSpace() )
					{
					startpos++; // skip to start of data
					}
				TInt endpos = lex.Locate(TChar('\n')); // assumes \n is after =.
				if ( endpos == KErrNotFound ) // may not be \n on last line
					{
					endpos = section.Length()-1;
					}
				aResult.Set(lex.Mid(startpos).Ptr(),endpos-startpos-1);
				}
			}
		}

	return ret;
	}

TBool CIniFile::FindVar(const TDesC &aSection,const TDesC &aVarName,
						TInt &aResult)
	{
	TInt ret = EFalse;
	TPtrC ptr(NULL,0);
	if (FindVar(aSection,aVarName,ptr))
		{
		TLex lex(ptr);
		if (lex.Val(aResult)==KErrNone)
			ret = ETrue;
		}

	return ret;
	}

//
// End of file
