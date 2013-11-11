/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CINIDATA.CPP
*
*/



#include <e32std.h>
#include "iniparser.h"

// Default directory to look for INI file
_LIT(KIniFileDir,"\\System\\Data\\");
_LIT(KIniSysDrive, "${SYSDRIVE}");
_LIT(KLegacySysDrive, "C:");

// Constant Value changed for defect047130 fix
const TInt KTokenSize=256;

enum TIniPanic
	{
	ESectionNameTooBig,
	EKeyNameTooBig,
	};

#ifndef EKA2
////////////////////////////////////////////////////////////////////////////////
//
// DLL entry point
//
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)	

	{
	return (KErrNone);
	}
#endif

////////////////////////////////////////////////////////////////////////////////
void Panic(TIniPanic aPanic)
	{
	_LIT(CIniData,"CIniData");
	User::Panic(CIniData,aPanic);
	}

/**
 * Updates ${SYSDRIVE} variables in the ini file buffer
 * To the system drive set during object constrcution
 */
void CIniData::UpdateVariablesL()
	{
	if (iIniData != NULL)
		{
		//prevent re-allocating to copy the same data again
		TPtr temp(iIniData->Des()) ; 
		if(iPtr.Compare(temp) != 0)
			{
			delete iIniData;
			iIniData = NULL ; 
			}
		}
	// if iIniData was never allocated or just deleted...
	if(iIniData == NULL)
		{
		iIniData = iPtr.AllocL();
		}
		
	iPtrExpandedVars.Set(iIniData->Des());
	
	if(iPtrExpandedVars.FindF(KIniSysDrive) == KErrNotFound)
		return;

	while ( iPtrExpandedVars.Find(KIniSysDrive) != KErrNotFound )
		{
		TInt offset = iPtrExpandedVars.Find(KIniSysDrive);
		iPtrExpandedVars.Replace(offset, 11, iSysDrive);
		}
	}

/**
 * Constructor
 */
EXPORT_C CIniData::CIniData() 
	: iPtr(NULL,0), iPtrExpandedVars(NULL,0)
	{
	__DECLARE_NAME(_S("CIniData"));
	iSysDrive.Copy(KLegacySysDrive);
	}

/**
 * Overloaded constructor
 * Takes in the system drive letter to overwrite
 */
EXPORT_C CIniData::CIniData(const TDesC& aSysDrive) 
	: iPtr(NULL,0), iPtrExpandedVars(NULL,0)
	{
	__DECLARE_NAME(_S("CIniData"));
	if ( aSysDrive.Length() == 2 )
		iSysDrive.Copy(aSysDrive);
	}

/**
 * Destructor
 * Frees the resources located in second-phase constructor
 */
EXPORT_C CIniData::~CIniData()
	{
	if( iIniData != NULL )
		delete iIniData;
	delete (TText*)iPtr.Ptr();
	delete iToken;
	delete iName;
	}

/**
 * Creates, and returns a pointer to CIniData object, leave on failure
 * @param aName - Path and name of the ini file to be parsed
 * @return A pointer to the CiniData object
 */
EXPORT_C CIniData* CIniData::NewL(const TDesC& aName)
	{
	CIniData* p=new(ELeave) CIniData();
	CleanupStack::PushL(p);
	p->ConstructL(aName);
	CleanupStack::Pop();
	return p;
	}

/**
 * Creates, and returns a pointer to CIniData object, leave on failure
 * @param aName - Path and name of the ini file to be parsed
 * @param aSysDrive - Drive letter to overwrite the default system drive
 * @return A pointer to the CiniData object
 */
EXPORT_C CIniData* CIniData::NewL(const TDesC& aName, const TDesC& aSysDrive)
	{
	CIniData* p=new(ELeave) CIniData(aSysDrive);
	CleanupStack::PushL(p);
	p->ConstructL(aName);
	CleanupStack::Pop();
	return p;
	}

/**
 * Second-phase constructor.
 * The function attempts to allocate a buffer and Read file's contents into iPtr
 * @param aName the name of the file which contains the ini data
 * @leave One of the system-wide error codes
 */
EXPORT_C void CIniData::ConstructL(const TDesC& aName)
	{
 	// Allocate space for token
	iToken=HBufC::NewL(KTokenSize+2);	// 2 extra chars for [tokenName]

	// Connect to file server
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();

	// Find file, given name
	TFindFile ff(fs.iObj);
	TFileName iniFileDir(KLegacySysDrive);
	iniFileDir.Append(KIniFileDir);
	User::LeaveIfError(ff.FindByDir(aName, iniFileDir));
	iName=ff.File().AllocL();

	// Open file
	TAutoClose<RFile> file;
	TInt size;
	User::LeaveIfError(file.iObj.Open(fs.iObj,*iName,EFileStreamText|EFileShareReadersOrWriters));
	file.PushL();

	// Get file size and read in
	User::LeaveIfError(file.iObj.Size(size));
	TText* data=(TText*)User::AllocL(size);
	iPtr.Set(data, size/sizeof(TText), size/sizeof(TText));
	TPtr8 dest((TUint8*)data, 0, size);
	User::LeaveIfError(file.iObj.Read(dest));
	TUint8* ptr = (TUint8*)data;

	//
	// This is orderred as FEFF assuming the processor is Little Endian
	// The data in the file is FFFE.		PRR 28/9/98
	//
	if(size>=(TInt)sizeof(TText) && iPtr[0]==0xFEFF)
	{
		// UNICODE Text file so lose the FFFE
		Mem::Copy(ptr, ptr+sizeof(TText), size-sizeof(TText));
		iPtr.Set(data, size/sizeof(TText)-1, size/sizeof(TText)-1);
	}
	else if(size)
	{
		// NON-UNICODE so convert to UNICODE
		TText* newdata = (TText*)User::AllocL(size*sizeof(TText));
		iPtr.Set(newdata, size, size);
		TInt i;
		for(i=0 ; i<size ; ++i)
			iPtr[i]=ptr[i];
		delete data;
	}

	// Create a duplicate of the iPtr
	// Expand all ${SYSDRIVE} entries to system drive letter
	UpdateVariablesL();

	file.Pop();
	fs.Pop();
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Find a text value from given aKeyName regardless the section in the ini data file
 * @param aKeyName Key being searched for
 * @param aResult On return, contains the text result 
 * @return ETrue if found, otherwise EFalse
 */
EXPORT_C TBool CIniData::FindVar(const TDesC &aKeyName, TPtrC &aResult)
{
	// Call with no section, so starts at beginning
	if (FindVar((TDesC&)KNullDesC , aKeyName, aResult))
		return(ETrue);
	else
		return(EFalse);
}

/**
 * Find a text value from given aKeyName and aSecName in the ini data file
 * @param aSectName Section containing key
 * @param aKeyName Key being searched for in aSectName
 * @param aResult On return, contains the text result 
 * @return ETrue if found, otherwise EFalse
 */
EXPORT_C TBool CIniData::FindVar(const TDesC &aSectName,const TDesC &aKeyName,TPtrC &aResult)
	{
	__ASSERT_DEBUG(aSectName.Length()<=KTokenSize,Panic(ESectionNameTooBig));
	__ASSERT_DEBUG(aKeyName.Length()<=KTokenSize,Panic(EKeyNameTooBig));

	return FindVar(aSectName,aKeyName,aResult, iPtrExpandedVars);
	}

/**
 * Find a text value from given aKeyName and aSecName in the provided ini data pointer
 * @param aSectName Section containing key
 * @param aKeyName Key being searched for in aSectName
 * @param aResult On return, contains the text result 
 * @param aIniDataPtr ini data pointer to be used to find the value
 * @return ETrue if found, otherwise EFalse
 */
TBool CIniData::FindVar(const TDesC &aSectName,const TDesC &aKeyName,TPtrC &aResult, TPtr& aIniDataPtr)
	{
	TInt posStartOfSection(0);
	TInt posEndOfSection(aIniDataPtr.Length()); // Default to the entire length of the ini data
	TPtrC SearchBuf;

	// If we have a section, set pos to section start
	TInt posI(0);	// Position in internal data Buffer
	if( aSectName.Length() > 0 )
		{
		TBool FoundSection(false);
		while ( ! FoundSection )
			{
			// Move search buffer to next area of interest
			SearchBuf.Set(aIniDataPtr.Mid(posI));

			// Make up token "[SECTIONNAME]", to search for
			TPtr sectionToken=iToken->Des();
			_LIT(sectionTokenFmtString,"[%S]");
			sectionToken.Format(sectionTokenFmtString,&aSectName);

			// Search for next occurrence of aSectName
			TInt posSB = SearchBuf.Find(sectionToken);

			if (posSB==KErrNotFound)
				return(EFalse);

			// Check this is at beginning of line (ie. non-commented)
			// ie. Check preceding char was LF
			if(posSB>0)
				{
				// Create a Buffer, starting one char before found subBuf
				TPtrC CharBefore(SearchBuf.Right(SearchBuf.Length()-posSB+1));
				// Check first char is end of prev
				if(CharBefore[0] == '\n')
					{
					FoundSection = ETrue;		// found
					posI = posI + posSB;
					}
				else
					{
					posI = posI + posSB + 1;	// try again
					}
				}
			else
				{
				FoundSection = ETrue;
				}

			}	// while ( ! FoundSection ) 

		// Set start of section, after section name, (incl '[' and ']')
		posStartOfSection = posI + aSectName.Length() + 2;

		// Set end of section, by finding begin of next section or end
		SearchBuf.Set(aIniDataPtr.Mid(posI));
		_LIT(nextSectionBuf,"\n[");
		TInt posSB = SearchBuf.Find(nextSectionBuf);
		if(posSB != KErrNotFound)
			{
			posEndOfSection = posI + posSB;
			}
		else
			{
			posEndOfSection = aIniDataPtr.Length();
			}

		}	// if( aSectName.Length() > 0 )

	// Look for key in ini file data Buffer
	posI = posStartOfSection;
	TBool FoundKey(false);
	while ( ! FoundKey )
		{
		// Search for next occurrence of aKeyName
		SearchBuf.Set(aIniDataPtr.Mid(posI,posEndOfSection-posI));
		TInt posSB = SearchBuf.Find(aKeyName);

		// If not found, return
		if (posSB==KErrNotFound)
			return(EFalse);

		// Check this is at beginning of line (ie. non-commented)
		// ie. Check preceding char was CR or LF
		if(posSB>0)
			{
			// Create a Buffer, starting one char before found subBuf
			TPtrC CharBefore(SearchBuf.Right(SearchBuf.Length()-posSB+1));
			// Check if the first char is end of prev and also check 
			// if the token found is not a substring of another string  
			TBool beginningOK = ((CharBefore[0] == '\n') || (CharBefore[0] == ' ') || (CharBefore[0] == '\t'));
			TBool endingOK = ((CharBefore[aKeyName.Length()+1] == '=') || (CharBefore[aKeyName.Length()+1] == ' ') || (CharBefore[aKeyName.Length()+1] == '\t'));
			if (beginningOK && endingOK)
				{
				FoundKey = ETrue;
				posI = posI + posSB;
				}
			else
				{
				posI = posI + posSB + 1;
				}
			}
		else
			{
			FoundKey = ETrue;
			}
		}	// while ( ! FoundKey )

	// Set pos, to just after '=' sign
	SearchBuf.Set(aIniDataPtr.Mid(posI));
	TInt posSB = SearchBuf.Locate('=');
	if(posSB==KErrNotFound)		// Illegal format, should flag this...
		return(EFalse);

	// Identify start and end of data (EOL or EOF)
	posI = posI + posSB + 1;	// 1 char after '='
	TInt posValStart = posI;
	TInt posValEnd;
	SearchBuf.Set(aIniDataPtr.Mid(posI));
	posSB = SearchBuf.Locate('\n');
	if(posSB!=KErrNotFound)
		{
     		// ini file uses LF for EOL 
            posValEnd = posI + posSB; 
		}
	else
		{
		posValEnd = aIniDataPtr.Length();
		}

	// Check we are still in the section requested
	if( aSectName.Length() > 0 )
		{
		if( posValEnd > posEndOfSection )
			{
			return(EFalse);
			}
		}
	// Parse Buffer from posn of key
	// Start one space after '='
	TLex lex(aIniDataPtr.Mid(posValStart, posValEnd-posValStart));
	lex.SkipSpaceAndMark();		// Should be at the start of the data
	aResult.Set(lex.MarkedToken().Ptr(),posValEnd-posValStart - lex.Offset() );

	TInt filterOffset;
	
	// Mark the offset value to the end of the value string
	filterOffset = aResult.Length() - 1;

	// Loop from end of the value string marked by the offset fetched by above process
    // And check to see if there is spaces, tabs or carriage returns
	while(filterOffset >= 0 &&
		(aResult[filterOffset] == '\t' ||
         aResult[filterOffset] == ' ' ||
         aResult[filterOffset] == '\r'))
		{
		// If found, reduce the offset by 1 for every space and tab during the while loop
		filterOffset--;
		}

	// The final offset value indicating end of actual value
	// within the ini data is set to the result string reference passed in
	aResult.Set(aResult.Mid(0,filterOffset + 1));

	return(ETrue);
	}

/**
 * Find an integer value from given aKeyName regardless the section in the ini data file
 * @param aKeyName Key being searched for
 * @param aResult On return, contains the TInt result 
 * @return ETrue if found, otherwise EFalse
 */
EXPORT_C TBool CIniData::FindVar(const TDesC &aKeyName, TInt &aResult)
	{
	TPtrC ptr(NULL,0);
	if (FindVar(aKeyName,ptr))
		{
		TLex lex(ptr);
		if (lex.Val(aResult)==KErrNone)
			return(ETrue);
		}
	return(EFalse);
	}

/**
 * Find an integer value from given aKeyName and aSecName in the ini data file
 * @param aSectName Section containing key
 * @param aKeyName Key being searched for  in aSectName
 * @param aResult On return, contains TInt result 
 * @return ETrue if found, otherwise EFalse
 */
EXPORT_C TBool CIniData::FindVar(const TDesC &aSection,const TDesC &aKeyName,TInt &aResult)
	{
	TPtrC ptr(NULL,0);
	if (FindVar(aSection,aKeyName,ptr))
		{
		TLex lex(ptr);
		if (lex.Val(aResult)==KErrNone)
			return(ETrue);
		}
	return(EFalse);
	}
/**
 * Find an 64-bit integer value from given aKeyName and aSecName in the ini data file
 * @param aSectName Section containing key
 * @param aKeyName Key being searched for  in aSectName
 * @param aResult On return, contains TInt64 result 
 * @return ETrue if found, otherwise EFalse
 */
EXPORT_C TBool CIniData::FindVar(const TDesC &aSection,	// Section to look under
					const TDesC &aKeyName,		// Key to look for
					TInt64 &aResult)			// Int64 ref to store result
	{
	TPtrC ptr(NULL,0);
	if (FindVar(aSection,aKeyName,ptr))
		{
		TLex lex(ptr);
		if (lex.Val(aResult)==KErrNone)
			return(ETrue);
		}
	return(EFalse);
	}
/**
 * Set a text value to given aKeyName and aSecName in the ini data file
 * @param aSectName Section containing key
 * @param aKeyName Key being set  in aSectName
 * @param aValue A text value set for the aKeyName in section ASecName
 * @return KErrNone if sucessful, otherwise system error
 */
EXPORT_C TInt CIniData::SetValue(const TDesC& aSection,const TDesC& aKeyName,const TDesC& aValue)
	{
	// First find the variable - this gives us a descriptor into the
	// ini data giving the bound of the item that has got to be replaced.
	TPtrC ptr;
	
	if (!FindVar(aSection, aKeyName, ptr, iPtr))
		return KErrNotFound;
	
	TInt pos = ptr.Ptr()-iPtr.Ptr();
	TInt size = iPtr.Length()+aValue.Length()-ptr.Length();

	if (size>iPtr.MaxLength())
		{
		size = size * sizeof(TText);
		TText* newdata = (TText*)User::ReAlloc((TUint8*)iPtr.Ptr(), size); 
		if (newdata == 0)
			return KErrNoMemory;

		iPtr.Set(newdata, iPtr.Length(), iPtr.Length()>size/(TInt)sizeof(TText)?iPtr.Length():size/(TInt)sizeof(TText));
		}
	iPtr.Replace(pos, ptr.Length(), aValue);
	return(KErrNone);
	}

/**
 * Set a text value to given aKeyName regardless of the section in the ini data file
 * @param aKeyName Key being searched for
 * @param aValue A text value set for the aKeyName 
 * @return KErrNone if sucessful, otherwise system error
 */
EXPORT_C TInt CIniData::SetValue(const TDesC& aKeyName,const TDesC& aValue)
	{
	// Call with no section, so starts at beginning
	return SetValue((TDesC&)KNullDesC, aKeyName, aValue);
	}

/**
 * Add key aKeyName to end of ini data file
 * @param aKeyName aKeyName Key being added
 * @param aValue Text value assigned to the aKeyName
 * @return KErrNone if sucessful, otherwise system error
 */
EXPORT_C TInt CIniData::AddValue(const TDesC& aKeyName,const TDesC& aValue)
	{
		// Call with no section, so starts at beginning
		return AddValue((TDesC&)KNullDesC, aKeyName, aValue);
	}

/**
 * Add key aKeyName to section aSectName
 * @param aSectName Section
 * @param aKeyName Key being added to section aSectName
 * @param aValue A text value added to the aKeyName 
 * @return KErrNone if sucessful, otherwise system error
 */
EXPORT_C TInt CIniData::AddValue(const TDesC& aSectName,const TDesC& aKeyName,const TDesC& aValue)
	{
	// The request will be denied if the variable exists. 
	// 
	// If the variable is not there then making an implementation.
	// If the Section name existed in the ini file then just append it 
	// to this section otherwise append the section, key name and 
	// variable at the end of the ini file.
	TPtrC ptr;
	
	if (FindVar(aSectName, aKeyName, ptr))  
		return KErrAlreadyExists;  // an key name exists, 
								// not allow to add value again to same key
		/////////////////////////////
	TPtr insertSection =iToken->Des();
	TInt size;
		
	TInt posEndOfSection(iPtr.Length()); // Default to the entire length of the ini data
	TPtrC SearchBuf;

	// If there is a section, set pos to section start
	TInt posI(0);	// Initiate to 0. Position in internal data Buffer
	TInt posSB(0);  
	if( aSectName.Length() > 0 )
		{
		TBool FoundSection(false);
		while ( ! FoundSection )
			{
			// Move search buffer to next area of interest
			SearchBuf.Set(iPtr.Mid(posI));

			// Make up token "[SECTIONNAME]", to search for
			TPtr sectionToken=iToken->Des();
			_LIT(sectionTokenFmtString,"[%S]");
			sectionToken.Format(sectionTokenFmtString,&aSectName);

			// Search for next occurrence of aSectName
			posSB = SearchBuf.Find(sectionToken);

			// If not found, return
			if (posSB==KErrNotFound)
			{
				FoundSection = ETrue; //Stop while searching
				_LIT(insertSectionfmt,"\r\n[%S]\r\n%S = %S\r\n");  //if there is no section existed
				insertSection.Format(insertSectionfmt,&aSectName,&aKeyName, &aValue);
			}
			

			// Check this is at beginning of line (ie. non-commented)
			// ie. Check preceding char was LF
			if(posSB>=0)
				{
				// Create a Buffer, starting one char before found subBuf
				TPtrC CharBefore(SearchBuf.Right(SearchBuf.Length()-posSB+1));
				// Check first char is end of prev
				if(CharBefore[0] == '\n' or posSB == 0)
					{
					FoundSection = ETrue;		// found
					posI = posI + posSB;
					_LIT(insertSectionfmt,"\r\n%S = %S\r\n");  //if there is no section existed
					insertSection.Format(insertSectionfmt,&aKeyName, &aValue);
					}
				else
					{
					posI = posI + posSB + 1;	// try again
					}
				}
			else
				{
				FoundSection = ETrue; //found at the beginning of the search buffer
				}

			}	// while ( ! FoundSection ) 
			
			// Set start of section, after section name, (incl '[' and ']')
			if(posSB!=KErrNotFound)
			{

				// Set end of section, by finding begin of next section or end
				SearchBuf.Set(iPtr.Mid(posI));
				_LIT(nextSectionBuf,"\n[");
				TInt posSB = SearchBuf.Find(nextSectionBuf);
				if(posSB != KErrNotFound)
					{
					posEndOfSection = posI + posSB+1;  //insert it between \n and [
					}
				else
					{
					posEndOfSection = iPtr.Length();
					}			
			}

		}	// if( aSectName.Length() > 0 )
		
	// append/insert here
	size = (iPtr.Length()+ insertSection.Length())*sizeof(TText);
	if (size>iPtr.MaxLength())
		{
		TText* newdata = (TText*)User::ReAlloc((TUint8*)iPtr.Ptr(), size); 
		if (newdata == 0)
			return KErrNoMemory;

		iPtr.Set(newdata, iPtr.Length(), size/(TInt)sizeof(TText));
		}

	iPtr.Insert(posEndOfSection,insertSection);
	return(KErrNone);
	}
	
	
/**
 * A method used to flush the data in the buffer to the file given in second-phase constructor
 */
EXPORT_C void CIniData::WriteToFileL()
	{

#ifndef __WINC__

	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();

	TAutoClose<RFile> file;
	User::LeaveIfError(file.iObj.Replace(fs.iObj,*iName,EFileStreamText|EFileShareExclusive));
	file.PushL();
	
	// convert 16 bits to 8 bits stream

	HBufC8* buf8 = HBufC8::NewL(iPtr.Length());
	TPtr8 ptr8(buf8->Des());
	ptr8.Copy(iPtr);
	User::LeaveIfError(file.iObj.Write(ptr8));

	delete buf8;

	// Refresh the iPtrExpandedVars with the new written ini file content
	// Repeat the process of expanding the ${SYSDRIVE}
	UpdateVariablesL();

	file.Pop();
	fs.Pop();
#endif
	}
