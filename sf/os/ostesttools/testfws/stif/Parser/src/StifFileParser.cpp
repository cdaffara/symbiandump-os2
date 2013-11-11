/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This module contains implementation of CStifParser 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "StifFileParser.h"
#include "StifTestInterface.h"
#include "ParserTracing.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CStifFileParser

    Method: CStifFileParser

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: TCommentType aCommentType: in: Comment type's indication

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CStifFileParser::CStifFileParser(CStifParser::TCommentType aCommentType)
	{
    iCommentType = aCommentType;
	}

/*
-------------------------------------------------------------------------------

    Class: CStifFileParser

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Sets variables.

    Parameters: RFs& aFs:        in: Handle to valid file server
                RFile& aFile:    in: Handle to the source file
                TBool aIsUnicode in: Is file in unicode format

    Return Values: None

    Errors/Exceptions:  None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CStifFileParser::ConstructL(RFs& aFs,
                                 RFile& aFile,
                                 TBool aIsUnicode)
	{
	//Initialization
	iFileServer = aFs;
	iBaseFile = aFile;
	iCurrentFile = &aFile;
	iIsUnicode = aIsUnicode;
	iBytesPerChar = iIsUnicode ? 2 : 1;

	//Create file stack (INCLUDE feature)
	iFileStack = new (ELeave) CStackDeprecated<RFile, EFalse>;

	//Add base file to file names array
	TFileName fn;
	iCurrentFile->FullName(fn);
	HBufC* newFile = fn.AllocLC();
	User::LeaveIfError(iFileNames.Append(newFile));
	CleanupStack::Pop(newFile);
	}

/*
-------------------------------------------------------------------------------

    Class: CStifFileParser

    Method: NewL

    Description: Two-phased constructor.

    Starting creating parser with path and file information.

    Parameters: RFs& aFs:                  in: Handle to file server
                RFile& aFile:              in: Source path definition
                TBool aIsUnicode           in: Is file in unicode format
                TCommentType aCommentType: in: Comment type's indication

    Return Values: CStifFileParser* : pointer to CStifFileParser object

    Errors/Exceptions: Leaves if ConstructL leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
CStifFileParser* CStifFileParser::NewL(RFs& aFs,
                                       RFile& aFile,
                                       TBool aIsUnicode,
                                       CStifParser::TCommentType aCommentType)
	{
    // Create CStifParser object
    CStifFileParser* parser = new (ELeave) CStifFileParser(aCommentType);

    CleanupStack::PushL(parser);
    parser->ConstructL(aFs, aFile, aIsUnicode);
    CleanupStack::Pop(parser);

    return parser;
	}

/*
-------------------------------------------------------------------------------

    Class: CStifFileParser

    Method: ~CStifFileParser

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CStifFileParser::~CStifFileParser()
	{
	//Close file stack
	ClearFileStack();
	delete iFileStack;

	//Close section lines array
	ClearSectionLinesArray();
	iSectionLines.Close();
	
	//Close fila names array
	while(iFileNames.Count() > 0)
		{
		delete iFileNames[0];
		iFileNames.Remove(0);
		}
	iFileNames.Close();
	}

/*
-------------------------------------------------------------------------------

    Class: CStifFileParser

    Method: ReadLineL

    Description: Reads line from source file

    Parameters: TPtr& aLineBuffer:      in: Descriptor in which line loads
                TPtr& aEndOfLineBuffer: in: Descriptor in which end of line sequence is loaded (0x0A or 0x0D 0x0A)

    Return Values: TBool: determines whether line was readed or not

    Errors/Exceptions:  Leaves if seek command leaves
    					Leaves if buffer is too small

    Status: Proposal

-------------------------------------------------------------------------------
*/
TBool CStifFileParser::ReadLineL(TPtr& aLineBuffer,
                                 TPtr& aEndOfLineBuffer)
	{
	//Variables
	TBuf8<128> buf8;
	TBuf16<128> buf16;
	TPtrC16 buf;
	TInt pos;
	TInt offset;
	TChar char0x0A = 0x000A;
	TChar char0x0D = 0x000D;

	TBuf<1> b0x0A;
	b0x0A.Append(char0x0A);

	//Reset buffers
	aLineBuffer.Zero();
	aEndOfLineBuffer.Zero();

	//Read from source
	User::LeaveIfError(iCurrentFile->Read(buf8));
	buf16.Copy(buf8);
	if(iIsUnicode)
		buf.Set((TUint16 *)(buf8.Ptr()), buf8.Length() / 2);
	else
		buf.Set(buf16.Ptr(), buf16.Length());

	while(buf.Length())
		{
		//Search for end of line char
		pos = buf.Find(b0x0A);

		//If found, append readed data to output descriptor and move back file offset to correct position
		if(pos >= 0)
			{
			offset = -((buf.Length() - pos - 1) * iBytesPerChar);
			User::LeaveIfError(iCurrentFile->Seek(ESeekCurrent, offset));
			buf.Set(buf.Ptr(), pos + 1);
			aLineBuffer.Append(buf);
			break;
			}
		//Otherwise, append whole buffer to output descriptor
		else
			{
			aLineBuffer.Append(buf);
			}
		//Read next part of data
		User::LeaveIfError(iCurrentFile->Read(buf8));
		buf16.Copy(buf8);
		if(iIsUnicode)
			{
			buf.Set((TUint16 *)(buf8.Ptr()), buf8.Length() / 2);
			}
		else
			{
			buf.Set(buf16.Ptr(), buf16.Length());
			}
		}

	//Set correct end of line buffer
	if(buf.Length() > 1)
		{
		if(buf[buf.Length() - 2] == char0x0D)
			{
			aEndOfLineBuffer.Append(char0x0D);
			}
		}
	if(buf.Length() > 0)
		{
		if(buf[buf.Length() - 1] == char0x0A)
			{
			aEndOfLineBuffer.Append(char0x0A);
			}
		else
			{
			aEndOfLineBuffer.Zero();
			}
		}
	if(aEndOfLineBuffer.Length())
		{
		aLineBuffer.SetLength(aLineBuffer.Length() - aEndOfLineBuffer.Length());
		}

	//If no data was found, try to get previous file from stack
	if(aLineBuffer.Length() + aEndOfLineBuffer.Length() == 0)
		{
		//Pop file from stack. If stack is empty, then we achieved end of base file
		if(!iFileStack->IsEmpty())
			{
			PopFromFileStack();
			aEndOfLineBuffer.Copy(iEolBuf);
			return aEndOfLineBuffer.Length();
			}
		}
	//Check if this is include line
	else
		{
		if(aLineBuffer.Find(KIncludeKeyword) == 0)
			{
			TFileName fn;
			TLex lex(aLineBuffer);

			fn.Copy(lex.NextToken()); //get INCLUDE keyword
			fn.Copy(lex.NextToken()); //get cfg file name
			if(fn.Length() > 0)
				{
				TStifUtil::CorrectFilePathL( fn );
				PushFileToStackL(fn);
				iEolBuf.Copy(aEndOfLineBuffer);
				}
			else
				{
				__TRACE(KError, (_L("No filename was given after INCLUDE. Ignoring")));
				}

			//Read next line
			return ReadLineL(aLineBuffer, aEndOfLineBuffer);
			}
		}
	
	return aLineBuffer.Length() + aEndOfLineBuffer.Length();
	}

/*
-------------------------------------------------------------------------------

    Class: CStifFileParser

    Method: ReplaceCommentsLineL

    Description: Replaces comments with spaces from line and copy it to destination buffer

    Parameters: TPtr& aSrcBufPtr:        in:  Source line with comments
                TPtr& aDstBufPtr:       out:  Destination line without comments
                TWhatToFind& aFind:  in out:  Determines what method currently is looking for

    Return Values: None

    Errors/Exceptions:  None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CStifFileParser::ReplaceCommentsLineL(TPtr& aSrcBufPtr,
                                           TPtr& aDstBufPtr,
										   TWhatToFind& aFind)
	{
	//Variables
	TLex lex(aSrcBufPtr);
	TChar ch;
	TChar chSpace(' ');
	TChar chSlash('/');
	TChar chQuota('\"');
	TChar chMulti('*');
	TChar chHash('#');

	//Clean buffer
	aDstBufPtr.Zero();

	while(!lex.Eos())
		{
		ch = lex.Get();

		//If the end of line, exit
		if(!ch)
			{
			break;
			}

		//We're looking for start od quoted text or comment
		if(aFind == EStart)
			{
			//Special character readed
			if(ch == chQuota)
				{
				aFind = EQuota;
				aDstBufPtr.Append(chSpace);
				}
			//Possible start of comment
			else if(ch == chSlash)
				{
				//Comment to the end of line found
				if(lex.Peek() == chSlash)
					{
					break;
					}
				//Beginning of a comment found
				else if(lex.Peek() == chMulti)
					{
					aFind = EEndOfComment;
					ch = lex.Get();
					aDstBufPtr.Append(chSpace);
					aDstBufPtr.Append(chSpace);
					}
				//No start of comment - add read slash
				else
					{
					aDstBufPtr.Append(ch);
					}
				}
			//Start of hash comment (to the end of line)
			else if(ch == chHash)
				{
				break;
				}
			//Append readed character to the destination buffer
			else
				{
				aDstBufPtr.Append(ch);
				}
			}
		//We're looking for the end of quoted text
		else if(aFind == EQuota)
			{
			if(ch == chQuota)
				{
				aFind = EStart;
				aDstBufPtr.Append(chSpace);
				}
			else
				{
				aDstBufPtr.Append(chSpace);
				}
			}
		//We're looking for the end of comment
		else if(aFind == EEndOfComment)
			{
			//It may be the end of a comment
			if(ch == chMulti)
				{
				ch = lex.Peek();
				//It is the end of a comment
				if(ch == chSlash)
					{
					aFind = EStart;
					ch = lex.Get();
					aDstBufPtr.Append(chSpace);
					aDstBufPtr.Append(chSpace);
					}
				//It is not the end of a comment, add this character to the destinaton buffer
				else
					{
					aDstBufPtr.Append(chSpace);
					}
				}
			//It is not the end of a comment, add this character to the destinaton buffer
			else
				{
				aDstBufPtr.Append(chSpace);
				}
			}
		}
	}

/*
-------------------------------------------------------------------------------

    Class: CStifFileParser

    Method: ReplaceHashCommentsLineL

    Description: Copy line to destination buffer and deletes #-style comments

    Parameters: TPtr& aSrcBufPtr:     in:  Source line with comments
                TPtr& aDstBufPtr:    out: Destination line without comments

    Return Values: None

    Errors/Exceptions:  None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CStifFileParser::ReplaceHashCommentsLineL(TPtr& aSrcBufPtr,
                                               TPtr& aDstBufPtr)
	{
	//Variables
	TLex lex(aSrcBufPtr);
	TChar ch;
	TChar chHash('#');

	//Prepare destination buffer
	aDstBufPtr.Zero();

	//Copy source line to destination until # char is found
	while(!lex.Eos())
		{
		ch = lex.Get();

		//If the end of line, exit
		if(!ch)
			{
			break;
			}

		if(ch == chHash)
			{
			break;
			}
		else
			{
			aDstBufPtr.Append(ch);
			}
		}
	}

/*
-------------------------------------------------------------------------------

    Class: CStifFileParser

    Method: NextSectionL

    Description: Finds n-th (aSeeked) section in file starting from m-th (aOffset) position

    Parameters: const TDesc& aStartTag: in:     Starting tag of a section
                const TDesc& aEndTag:   in:     Ending tag of a section
                TInt& aOffset:          in out: Current offset in file (*)
                TInt aSeeked:           in:     Which section is to be found
    
    Notification: aOffset has different meaning than before adding INCLUDE functionality.
                  If it has 0 value, that means that we need to remove files stack and go
                  back to base file to the beginning. Otherwise we don't change current
                  file read-handler position.

    Return Values: HBufC *: address of a descriptor containing section. Section is returned without tags.
                            Caller must take care about the freeing descriptor.

    Errors/Exceptions:  Leaves if aSeeked is less than 1 or aOffset is negative
    					Leaves if seek command leaves
    					Leaves if cannot allocate buffers
    					Leaves if cannot allocate section
    					Leaves if length of readed line exceeds KMaxLineLength constant

    Status: Proposal

-------------------------------------------------------------------------------
*/
HBufC* CStifFileParser::NextSectionL(const TDesC& aStartTag,
                                     const TDesC& aEndTag,
                                     TInt& aOffset,
                                     TInt aSeeked)
	{
	// Check arguments
	if(aSeeked < 1)
		{		
		User::Leave(KErrArgument);
		}
	if(aOffset < 0)
		{		
		User::Leave(KErrArgument);
		}

	TInt foundSection = 0;
	TInt ret;

	// Alloc buffers to read line
	const TInt KMaxLineLength = 4096; //If length of readed line exceeds this constant, method will leave

	HBufC* buf = HBufC::NewL(KMaxLineLength);
	CleanupStack::PushL(buf);
	TPtr bufPtr(buf->Des());

	HBufC* withoutCommentsBuf = HBufC::NewL(KMaxLineLength);
	CleanupStack::PushL(withoutCommentsBuf);
	TPtr withoutCommentsBufPtr(withoutCommentsBuf->Des());

	HBufC* endOfLine = HBufC::NewL(8);  //After reading a line it contains 0D0A or 0A or null (how readed line is ended)
	CleanupStack::PushL(endOfLine);
	TPtr endOfLinePtr(endOfLine->Des());

	//Set valid position in file
	//but only if offset shows to 0. Otherwise keep previus position
	if(aOffset == 0)
		{
		User::LeaveIfError(iBaseFile.Seek(ESeekStart, aOffset));
		ClearFileStack();
		aOffset = 1;
		}

	//Prepare to read lines
	TBool validSectionBeginFound = EFalse;
	TBool validSectionEndFound = EFalse;
	TSectionFind whatToFindSection = ESectionStart;
	//If no start tag is given start to find end tag immediatly
	if(aStartTag.Length() == 0)
		{
		foundSection++;
		whatToFindSection = ESectionEnd;
		validSectionBeginFound = ETrue;
		}
	if(aEndTag.Length() == 0)
		{
		validSectionEndFound = ETrue;
		}
	TWhatToFind whatToFind = EStart;

	//Perform reading file
	while(ReadLineL(bufPtr, endOfLinePtr))
		{
		if(iCommentType == CStifParser::ECStyleComments)
			{				
			ReplaceCommentsLineL(bufPtr, withoutCommentsBufPtr, whatToFind);
			}
		else
			{
			ReplaceHashCommentsLineL(bufPtr, withoutCommentsBufPtr);
			}
			if(whatToFindSection == ESectionStart)
			{
			//Find in line star tag (if start tag is not given, behave like we've found it)
			if(aStartTag.Length() == 0)
				{
				ret = 0;
				}
			else
				{
				ret = withoutCommentsBuf->Find(aStartTag);
				}
			//If found remember position, move offset of file to actual position
			if(ret >= 0)
				{
				whatToFindSection = ESectionEnd;
				TInt offset = -(bufPtr.Length() + endOfLinePtr.Length() - ret - aStartTag.Length()) * iBytesPerChar;
				User::LeaveIfError(iCurrentFile->Seek(ESeekCurrent, offset));

				whatToFind = EStart; //reset marker, because if we've found tag, we couldn't be in the middle of comment or quota
				foundSection++;
				//Add this line to section lines array
				if(foundSection == aSeeked)
					{
					validSectionBeginFound = ETrue;
					}
				continue;
				}
			}
		else if(whatToFindSection == ESectionEnd)
			{
			//Find in line end tag (if end tag is not given, behave like we've found it)
			if(aEndTag.Length() == 0)
				{
				ret = KErrNotFound;
				}
			else
				{
				ret = withoutCommentsBuf->Find(aEndTag);
				}
				//If found check if this is the one we're looking for
			if(ret >= 0)
				{
				whatToFindSection = ESectionStart;
				TInt offset = -(bufPtr.Length() + endOfLinePtr.Length() - ret - aEndTag.Length()) * iBytesPerChar;
				User::LeaveIfError(iCurrentFile->Seek(ESeekCurrent, offset));

				whatToFind = EStart; //reset marker, because if we've found tag, we couldn't be in the middle of comment or quota
				if(foundSection == aSeeked)
					{
					//Add this line to section lines array
					HBufC* line = HBufC::NewLC(bufPtr.Length());
					TPtr linePtr(line->Des());
					linePtr.Copy(bufPtr.MidTPtr(0, ret));
					User::LeaveIfError(iSectionLines.Append(line));
					CleanupStack::Pop(line);
					validSectionEndFound = ETrue;
					break;
					}
				else
					{
					continue;
					}
				}
			else
				{
				//If we're in section we are looking for, add line to array
				if(foundSection == aSeeked)
					{
					HBufC* line = HBufC::NewLC(bufPtr.Length() + endOfLinePtr.Length());
					TPtr linePtr(line->Des());
					linePtr.Copy(bufPtr);
					linePtr.Append(endOfLinePtr);
					User::LeaveIfError(iSectionLines.Append(line));
					CleanupStack::Pop(line);
					}
				}
			}
		}

	//Clean data
	CleanupStack::PopAndDestroy(endOfLine);
	CleanupStack::PopAndDestroy(withoutCommentsBuf);
	CleanupStack::PopAndDestroy(buf);
	
	//Load into section if found
	HBufC* section = NULL;

	if(validSectionBeginFound && validSectionEndFound)
		{
		//Count amount of memory needed for section
		TInt i;
		TInt size = 0;
		for(i = 0; i < iSectionLines.Count(); i++)
			size += iSectionLines[i]->Length();
		
		//Copy section from array to buffer
		section = HBufC::NewL(size);
		CleanupStack::PushL(section);
		TPtr sectionPtr(section->Des());

		for(i = 0; i < iSectionLines.Count(); i++)
			sectionPtr.Append(*iSectionLines[i]);

		ClearSectionLinesArray();
		
		//Clean local data
		CleanupStack::Pop(section);
		
		return section;
		}
	
	ClearSectionLinesArray();

	//If section was not found, then for compability with CSectionParser leave when not first section was seeking, return NULL when first section was seeking
	if(foundSection != aSeeked)
		{
		if( aSeeked - foundSection > 1)
			{			
			User::Leave(KErrNotFound);
			}
		}

    return section;
	}

/*
-------------------------------------------------------------------------------

    Class: CStifFileParser

    Method: ClearFileStack

    Description: Closes all files on file stack and clears the stack

    Parameters: None

    Return Values: None

    Errors/Exceptions:  None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CStifFileParser::ClearFileStack(void)
	{
	while(!iFileStack->IsEmpty())
		{
		PopFromFileStack();
		}
	}

/*
-------------------------------------------------------------------------------

    Class: CStifFileParser

    Method: PopFromFileStack

    Description: Pops RFile handle from file stack and sets correct current file handle

    Parameters: None

    Return Values: None

    Errors/Exceptions:  None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CStifFileParser::PopFromFileStack(void)
	{
	if(!iFileStack->IsEmpty())
		{
		//Pop from stack
		iCurrentFile = iFileStack->Pop();

		TFileName fn;
		iCurrentFile->FullName(fn);

		//And remove from file names array
		for(TInt i = iFileNames.Count() - 1; i >= 0; i--)
			{
			if(fn.CompareF(iFileNames[i]->Des()) == KErrNone)
				{
				delete iFileNames[i];
				iFileNames.Remove(i);
				break;
				}
			}
		__TRACE(KInfo, (_L("Closing file [%S]"), &fn));

		//Close file
		iCurrentFile->Close();
		delete iCurrentFile;

		//Set correct current file
		if(iFileStack->IsEmpty())
			{
			iCurrentFile = &iBaseFile; //base file, because stack is empty
			}
		else
			{
			iCurrentFile = iFileStack->Last();
			}
		}
	}

/*
-------------------------------------------------------------------------------

    Class: CStifFileParser

    Method: PushFileToStack

    Description: Opens file and pushes it to stack

    Parameters: TDesc& aFileName   in: name of file to open and add to stack

    Return Values: None

    Errors/Exceptions:  None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CStifFileParser::PushFileToStackL(const TDesC& aFileName)
	{

	//First check if file is not already included
	for(TInt i = 0; i < iFileNames.Count(); i++)
		{
		if(aFileName.CompareF(iFileNames[i]->Des()) == KErrNone)
			{
			__TRACE(KError, (_L("File [%S] was already included. Ignoring"), &aFileName));
			return;
			}
		}

	//Open and add file to stack
	RFile *nf = new RFile();

	__TRACE(KInfo, (_L("Including file [%S]"), &aFileName));
	TInt r = nf->Open(iFileServer, aFileName, EFileRead | EFileShareAny);
	if(r == KErrNone)
		{
		//Add to stack
		iFileStack->PushL(nf);

		//And add to file names array
		HBufC* newFile = aFileName.AllocLC();
		User::LeaveIfError(iFileNames.Append(newFile));
		CleanupStack::Pop(newFile);

		//Set valid pointer of current file
		iCurrentFile = nf;
		}
	else
		{
		__TRACE(KError, (_L("Could not open file [%S]. Error %d. Ignoring"), &aFileName, r));
		}
	}

/*
-------------------------------------------------------------------------------

    Class: CStifFileParser

    Method: ClearSectionLinesArray

    Description: Deletes all descriptors assigned to array and empties array

    Parameters: None

    Return Values: None

    Errors/Exceptions:  None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CStifFileParser::ClearSectionLinesArray(void)
	{
	while(iSectionLines.Count() > 0)
		{
		delete iSectionLines[0];
		iSectionLines.Remove(0);
		}
	}

// End of File
