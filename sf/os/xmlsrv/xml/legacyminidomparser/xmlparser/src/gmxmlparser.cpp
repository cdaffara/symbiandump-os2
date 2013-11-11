// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// This file contains the definition of the generic CMDXMLParser class
// which is responsible for creating a DOM structure
// from a given XML file.
// 
//

#include <f32file.h>
#include <utf.h>
#include <bautils.h>

#include <gmxmlconstants.h>
#include <gmxmlcomposer.h>
#include <gmxmlentityconverter.h>
#include <gmxmlnode.h>
#include <gmxmldocument.h>
#include <gmxmlelement.h>
#include <gmxmlparser.h>
#include <gmxmlprocessinginstruction.h>
#include <gmxmlcomment.h>
#include <gmxmlcdatasection.h>
#include <gmxmltext.h>

#include "GMXMLFileDataSource.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include "gmxmldummydtd.h"
#endif

const TInt KGMXMLDefaultTextBufferSize = 1024;
const TInt KUTF8EdgeBufferLen = 6;

//
// Global functions					//
//


//==================================================================================

//
//CMDXParser						//
//
//==================================================================================

void CMDXMLParser::Panic(TPanicCode aReason) const
	{
	_LIT(KClassName, "CMDXMLParser");
	User::Panic(KClassName, aReason);
	}

EXPORT_C CMDXMLParser* CMDXMLParser::NewL(MMDXMLParserObserver* aParserObserver)
//
// Two phase static factory function constructor
// @return Created CMDXMLParser
// @leave can Leave due to OOM
//
	{
	CMDXMLParser* self = NewLC(aParserObserver);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CMDXMLParser* CMDXMLParser::NewL(MMDXMLParserObserver* aParserObserver, MXMLDtd* aDtdRepresentation)
//
// Two phase static factory function constructor
// @return Created CMDXMLParser
// @param aDtdRepresentation specid DTD represention class to be used for validation
// @leave can Leave due to OOM
//
	{
	CMDXMLParser* self = NewLC(aParserObserver, aDtdRepresentation);
	CleanupStack::Pop();
	return self;
	}


//==================================================================================

EXPORT_C CMDXMLParser* CMDXMLParser::NewLC(MMDXMLParserObserver* aParserObserver)
//
// Two phase static factory function constructor
// @return Created CMDXMLParser
// @leave can Leave due to OOM
//
	{
	CMDXMLParser* self = new (ELeave) CMDXMLParser(aParserObserver);
	CleanupStack::PushL(self);
	// This overload of NewLC doesn't take a MXMLDtd*, but we need to provide one to
	// ConstructL where ownership is taken if we do have one.  Just pass NULL.
	self->ConstructL(NULL);
	return self;
	}

EXPORT_C CMDXMLParser* CMDXMLParser::NewLC(MMDXMLParserObserver* aParserObserver, MXMLDtd* aDtdRepresentation)
//
// Two phase static factory function constructor
// @return Created CMDXMLParser
// @param aDtdRepresentation specid DTD represention class to be used for validation
// @leave can Leave due to OOM
//
	{
	CMDXMLParser* self = new (ELeave) CMDXMLParser(aParserObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aDtdRepresentation);
	return self;
	}


//==================================================================================

void CMDXMLParser::ConstructL(MXMLDtd* aDtdRepresentation)
//
// Second stage constructor
// @param aDtdRepresentation The DTD to be used for validation
// @leave can Leave due to OOM
//
	{
	CMDXMLEntityConverter* entityConverter = new(ELeave) CMDXMLEntityConverter();
	SetEntityConverter(entityConverter);

	// This doesn't leave, but if CMDXMLParser::NewLC() Leaves after taking ownership
	// of this we'll get a double deletion as the caller will have pushed 
	// aDtdRepresentation onto the CleanupStack.  As such we can only take ownership
	// once we are sure we aren't going to leave.
	iDtdRepresentation = aDtdRepresentation;
	}

CMDXMLParser::CMDXMLParser(MMDXMLParserObserver* aParserObserver)
	: CActive(EPriorityNormal)
//
// Constructor
//
	{
	iParserObserver = aParserObserver;
	iStoreInvalid = ETrue;
	CActiveScheduler::Add(this);
	}

//==================================================================================

EXPORT_C CMDXMLParser::~CMDXMLParser()
	{
	Cancel();
	delete iBomBuffer;
	if( iFileSource == NULL )
		{
		// iFileSource has not been allocated yet, so the file path or file
		// handle are still owned
		if( iFileToParse!=NULL )
			delete iFileToParse;
		else
			iFileHandleToParse.Close();
		}
	else
		{
		delete iFileSource;
		}
	delete iUTF8EdgeBuffer;
	delete iXMLDoc;
	delete iEntityConverter;
	delete iElementTag;
	delete iDtdRepresentation;
	delete iText;
	}

//==================================================================================
//Defect fix for INC036136- Enable the use of custom entity converters in GMXML
EXPORT_C void CMDXMLParser::SetEntityConverter(CMDXMLEntityConverter* aEntityConverter)
/*
	 * Sets the entity converter to be used
	 * and  take ownership of the passed entity converter
	 * @param aEntityConverter The entity converter to be used
	 */
	{
	delete iEntityConverter;
	iEntityConverter = aEntityConverter;
	}
//End Defect fix for INC036136
//==================================================================================

EXPORT_C void CMDXMLParser::SetStoreInvalid(TBool aStoreInvalid)
	{
	iStoreInvalid = aStoreInvalid;
	}
	
//==================================================================================
// Defect fix for INC105134 - GmXML consumes whitespace characters 
//==================================================================================	
EXPORT_C void CMDXMLParser::SetWhiteSpaceHandlingMode(TBool aPreserve)
    {
    iPreserve = aPreserve;
    }

// End Defect fix for INC105134
//==================================================================================
EXPORT_C CMDXMLDocument* CMDXMLParser::DetachXMLDoc()
//
// @return CMDXMLDocument* to the created DOM, should be called after the
// conclusion of the parser process.  Note that internal variable pointing to
// the document is set to NULL so this function can only be called once per file
// parse.  Client application must take ownership of document for cleanup purposes.
//
	{
	CMDXMLDocument* returnDoc = iXMLDoc;
	iXMLDoc = NULL;
	return returnDoc;
	}

//==================================================================================

CMDXMLEntityConverter* CMDXMLParser::EntityConverter()
//
// @return the CMDXMLEntityConverter for use in converting built in entity
// and character entities back to their original format
//
	{
	return iEntityConverter;
	}

//==================================================================================

EXPORT_C TInt CMDXMLParser::ParseFile(RFs aRFs, const TDesC& aFileToParse)
//
// ParseFile opens a file ready for parsing
// @param aRFs a resource file session used for file I/O
// @param aFileToParse the file name to parse
// @return KErrNone if all OK or file read error code
//
	{
	//Find out whether the file exists. If not dont start the active object
	if(!BaflUtils::FileExists(aRFs,aFileToParse))
		{
		return KErrNotFound;
		}
	else
		{	
		//Check whether the file is locked by any other process				
		RFile tempFile;
		TInt err=tempFile.Open(aRFs, aFileToParse, EFileRead | EFileShareReadersOnly);		
		if(err!=KErrNone)
			{
			return err;					
			}
		tempFile.Close();	
		}	
	Cancel();
	iSuspiciousCharacter = KErrNotFound;
	iError = KErrNone;
	iSeverity = EXMLNone;
	iDocTypeSet = EFalse;
	iVersionSet = EFalse;

	/* We need to open our file in a leave-safe place as it involves
	   a heap alloc, and so we'll set up the AO to do that when it runs next.
	*/
	delete iFileToParse;
	iFileToParse = aFileToParse.Alloc();
	if(iFileToParse == NULL)
		{
		return KErrNoMemory;
		}

	iRFs = aRFs;
	iState = KInitFromFile;

	SetActive();
	TRequestStatus* s=&iStatus;
	User::RequestComplete(s, KErrNone);

	return KErrNone;
	}
	
/** 
Parses a specified XML file into a DOM object tree.

Parses a specified XML file into a DOM object tree using an open file handle. The 
parser takes ownership of the open file handle and will close handle when completed.

@param aFileHandleToParse An open file handle for the file to parse. Ownership of the
	   file handle is passed.
@return KErrNone if successful.
*/
EXPORT_C TInt CMDXMLParser::ParseFile(RFile& aFileHandleToParse)
	{	
	iFileHandleToParse = aFileHandleToParse;

	Cancel();
	iSuspiciousCharacter = KErrNotFound;
	iError = KErrNone;
	iSeverity = EXMLNone;
	iDocTypeSet = EFalse;
	iVersionSet = EFalse;

	iState = KInitFromFile;

	iStatus = KRequestPending;
	SetActive();
	TRequestStatus* s=&iStatus;
	User::RequestComplete(s, KErrNone);

	return KErrNone;
	}
//==================================================================================

EXPORT_C void CMDXMLParser::ParseSourceL(MMDXMLParserDataProvider *aSource)
	{
	iSuspiciousCharacter = KErrNotFound;
	iError = KErrNone;
	iSeverity = EXMLNone;
	iDocTypeSet = EFalse;
	iVersionSet = EFalse;

	iDataSource = aSource;
	PrepareForReuseL();
	GetMoreData();
	}

//==================================================================================

void CMDXMLParser::PrepareForReuseL()
	{
	Cancel();

	iError = KErrNone;
	iSeverity = EXMLNone;
	iDocTypeSet = EFalse;
	iVersionSet = EFalse;
	iBytesPerChar = 0;
	iNextChar = 0;
	iInputBytesRemaining = 0;
	iOpened = EFalse;
	iClosed = EFalse;
	iNewElement = NULL;
	iParentElement = NULL;
	delete iUTF8EdgeBuffer;
	iUTF8EdgeBuffer = NULL;
	delete iBomBuffer;
	iBomBuffer = NULL;

	CreateDocumentL();
	iState = KDetermineCharset;
	}

//==================================================================================
TBool CMDXMLParser::DetectFileType()
//
// Detects the type of a data source - can be Unicode, UTF-8 or ASCII (because ASCII is
// a subset of UTF-8).
// If the file is empty it is assumed to be Utf-8.
	{
	TBuf8<3> bom;

	// Read the first 3 bytes of the file.  These contain any BOM present.  If it turns out
	// there's not a bom we leave the pointer untouched so we can parse these bytes
	// as usual.  
	if(iInputBufferPtr.Length() < 3)
		return EFalse;
	else
		bom.Copy(iInputBufferPtr.Left(3));

	TInt hichar = (CEditableText::EByteOrderMark & 0xFF00)>>8;
	TInt lochar = CEditableText::EByteOrderMark & 0xFF;

	if((bom[0] == 0xEF) && (bom[1] == 0xBB) && (bom[2] == 0xBF))
		{
		// Utf-8 with a bom.  We don't want to parse the bom, so add 3 bytes offset to the read pos.
		iBytesPerChar = 1;
		iNextChar = 3;
		}
	else
		{
		if((bom[0] == lochar) && (bom[1] == hichar))
			{
			// Little Endian Unicode.  Move the read position on 2 bytes to ignore the bom.
			iBytesPerChar = 2;
			// would normally skip first 2 characters
			iNextChar = 2;
			}
		else if((bom[0] == hichar) && (bom[1] == lochar))
			{
			// We have a bom, but it indicates endianess opposite to that of the platform.  We
			// don't currently support this so set an error.
			SetError(KErrNotSupported, EXMLFatal);
			}
		else
			{
			// Default to Utf-8
			iBytesPerChar = 1;
			}
		}
	return ETrue;
	}

//==================================================================================

EXPORT_C void CMDXMLParser::DoCancel()
//
// DoCancel function inherited from CActive base class
//
	{
	if (iDataSource)
	  {
	  iDataSource->Disconnect();
	  iDataSource = NULL;
	  }
	}

//==================================================================================

EXPORT_C TInt CMDXMLParser::RunError(TInt aError)
//
// RunError function inherited from CActive base class - intercepts any Leave from
// the RunL() function, sets an appropriate errorcode and calls ParseFileCompleteL
//
	{
	if(iDataSource)
		{
		iDataSource->Disconnect();
		iDataSource = NULL;
		}

	iSeverity = EXMLFatal;
	iError = aError;
	if (iFileToParse)
		{
		delete iFileToParse;
		iFileToParse = NULL;
		}
	else if( iFileSource==NULL )
		{
		// iFileSource is not set so the ownership of the file handle has not been passed
		iFileHandleToParse.Close();
		}

	__ASSERT_DEBUG(iParserObserver != NULL, Panic(ENullMemVarParserObserver));
	TRAPD(err, iParserObserver->ParseFileCompleteL());
	return err;
	}

//==================================================================================

TBool CMDXMLParser::DoParseLoopL()
//
// RunL function inherited from CActive base class - carries out the actual parsing.
// @leave can Leave due to OOM
//
	{
	TBuf<1> singleChar;
	TGetCharReturn getCharReturn = KError;
	TInt error;

	while(iSeverity != EXMLFatal && (getCharReturn = GetChar(singleChar), getCharReturn == KCharReturned) )
		{

#if 0	// THIS NEEDS TO BE REMOVED - IT'S A VERY HANDY DEBUGGING TOOL WHEN YOU'RE MANUALLY DOING 
		// BYTE ORDERING ON MISALIGNED STREAMS THOUGH...
		{
#define DES_AS_8_BIT(str) (TPtrC8((TText8*)((str).Ptr()), (str).Size()))

		RFs aRFs;
		RFile aFile;
		_LIT(KFileName, "c:\\documents\\SMIL_Test_Files\\echoOutput.txt");
		TPtrC file;
		TInt err;

		file.Set(KFileName);

		aRFs.Connect();

		err = aFile.Open(aRFs, file, EFileWrite);
		if(err != KErrNone)
			err = aFile.Create(aRFs, file, EFileWrite);

		err = 0;
		aFile.Seek(ESeekEnd, err);
		aFile.Write(DES_AS_8_BIT(singleChar));
		aFile.Close();
		aRFs.Close();
		}
#endif
		if((!iOpened) && singleChar != KXMLStartTag)
			{
			HandleTextL(singleChar);
			}

		if((iOpened) || (singleChar == KXMLStartTag))			
			{
			if(singleChar == KXMLStartTag)
				{
				if(iOpened)
					{
					if((iSuspiciousCharacter == KErrNotFound))
						{
						iSuspiciousCharacter = iElementTag->Length();
						}
					}
				else
					{
					AddTextL(iParentElement);
					iOpened = ETrue;
					}
				}
			else if(singleChar == KXMLEndTag)
				{
				if(iSuspiciousCharacter != KErrNotFound)
					{
					TPtrC suspiciousSection = iElementTag->Mid(iSuspiciousCharacter);
					if( CheckForStartCData(suspiciousSection) == 0 )
						{
						TInt endCDataLen = TPtrC(KXMLEndCDataSection).Length();
						// The suspicious character begins a CDataSection.  Check if
						// this End Tag is closing it.
						if( suspiciousSection.Right(endCDataLen - 1) 
							== TPtrC(KXMLEndCDataSection).Left(endCDataLen - 1) )
							{
							// Any dodgy characters began the CDataSection or were in it
							iSuspiciousCharacter = KErrNotFound;
							}
						}
					else if( suspiciousSection.Find(KXMLStartComment) == 0 )
						{
						// The suspicious character begins a comment.  Check if
						// this End Tag is closing it.
						TInt endCommentLen = TPtrC(KXMLEndComment).Length();
						if( suspiciousSection.Right(endCommentLen - 1) 
							== TPtrC(KXMLEndComment).Left(endCommentLen - 1) )
							{
							// Any dodgy characters began the comment or were in it
							iSuspiciousCharacter = KErrNotFound;
							}
						}
					else if((CheckForStartCData(*iElementTag) == 0) || (iElementTag->Find(KXMLStartComment) == 0))
						{
						// this tag is a CDataSection or comment, we're allowed <
						iSuspiciousCharacter = KErrNotFound;
						iClosed = ETrue;
						}
					else
						{
						// The < was spurious, set an error and close the tag as normal
						SetError(KErrXMLIllegalCharacter, EXMLWorkable);
						iClosed = ETrue;
						iSuspiciousCharacter = KErrNotFound;
						}
					}
				else
					{
					iClosed = ETrue;
					}
				}

			// ensure descriptor doesn't overflow end and panics
			if(iElementTag->Length() == iElementTag->Des().MaxLength())
				{
				iElementTag = iElementTag->ReAllocL(iElementTag->Length() + KNominalTagLength);
				}

			iElementTag->Des().Append(singleChar);

			// if tag is complete and needs adding to the DOM?
			if(iClosed)
				{
				if(		!CommentL(iParentElement)
					&&	!CDataSectionL(iParentElement)
					&&	!VersionIDL()
					&&	!DocTypeL()
					&&	!ProcessingInstructionL(iParentElement) )
					{
					// is this a regular closing tag
					if
						(iElementTag->Left(2) == KXMLStartEndTag)
						{
						error = ParseElementEndTag(*iParentElement, iElementTag->Des());
						if(error == KErrNone)
							{
							if(iParentElement->ParentNode() == NULL)
								{
								SetError(KErrXMLBadNesting, EXMLIndeterminate);
								}
							else
								{
								iParentElement = (CMDXMLElement*) iParentElement->ParentNode();
								}
							}
						else if(error == KErrNotFound)
							{
							CMDXMLElement* tempElement = (CMDXMLElement*) iParentElement->ParentNode();
							TInt searchResult = KErrNotFound;

							while(tempElement != NULL &&
									searchResult == KErrNotFound &&
									tempElement->NodeName() != KXMLDocumentElementNodeName)
								{
								searchResult = ParseElementEndTag(*tempElement,iElementTag->Des());	
								if(searchResult == KErrNone)
									{
									iParentElement = tempElement;
									SetError(KErrXMLBadNesting, EXMLIndeterminate);
									}
								else
									{
									tempElement = (CMDXMLElement*) tempElement->ParentNode();
									}
								}
							if(searchResult != KErrNone)
								{
								SetError(KErrXMLBadNesting, EXMLIndeterminate);
								}
							}
						else
							{
							SetError(error, EXMLIndeterminate);
							}
						}
					// if a new element start tag or start/end tag
					else
						{
						// NOTE ParseStartTagL destroys iElementTag
						// so following check must be done first

						// if not single tag with close
						if(!(iElementTag->Right(2) == KXMLEndStartTag))
							{
							iNewElement = ParseStartTagL();
							CleanupStack::PushL(iNewElement);
							error = iParentElement->AppendChild(iNewElement);

							if(error == KErrNone)
								{
								CleanupStack::Pop(); // iNewElement
								iParentElement = (CMDXMLElement*) iParentElement->LastChild();
								}
							else
								{
								SetError(error, EXMLWorkable);
								CleanupStack::PopAndDestroy(iNewElement); // iNewElement
								}
							}
						else
							{
							iNewElement = ParseStartTagL();
							CleanupStack::PushL(iNewElement);
							error = iParentElement->AppendChild(iNewElement);

							if(error == KErrNone)
								{
								CleanupStack::Pop(iNewElement); // iNewElement
								}
							else
								{
								SetError(error, EXMLWorkable);
								CleanupStack::PopAndDestroy(iNewElement); // iNewElement
								}
							}
						}
						iEndOfTag = ETrue;
					}

				if(iEndOfTag)
					{
					iEndOfTag = EFalse;
					iOpened = iClosed = EFalse;
					iElementTag->Des().Zero();

					// reduce size of ElementTag if increased beyond normal limits on last pass
					if(iElementTag->Des().MaxLength() > KNominalTagLength)
						{
						iElementTag = iElementTag->ReAllocL(KNominalTagLength);
						}
					}
				}
			}
		}	// END OF WHILE LOOP

	if(getCharReturn == KError)
		{
		return EFalse;
		}
	else
		{
		//	GetChar returned KWEaitForChar
		// GetChar handles pushing the state and requesting more data for us, so we just go active.
		return ETrue;
		}
	}




//==================================================================================

void CMDXMLParser::RunL()
	{
	TRequestStatus* s=&iStatus;
	TInt err = s->Int();

	switch(iState)
		{
	case KInitFromFile:
		delete iFileSource;
		iFileSource = NULL;
		if( iFileToParse == NULL )
			{
			// iFileToParse is not set, file was passed by open file handle.
			iFileSource = CMDXMLParserFileDataSource::NewL(iFileHandleToParse);			
			}
		else
			{
			iFileSource = CMDXMLParserFileDataSource::NewL(iRFs, *iFileToParse);

			delete iFileToParse;
			iFileToParse = NULL;
			}

		ParseSource(iFileSource);	// will go active itself 
		break;

	case KDetermineCharset:
		if(!iBytesPerChar)
			{
			if(DetectFileType())
				{
				iState = KParseData;
				SetActive();
				User::RequestComplete(s, KErrNone);
				}
			else
				{
				if(!iBomBuffer)
					{
					iBomBuffer = HBufC8::NewL(KUTF8EdgeBufferLen);
					}

				TPtr8 bomDes(iBomBuffer->Des());
				TInt newLength = bomDes.Length() + iCurrentInputBufferLen;
				iBomBuffer->ReAlloc(newLength);
				
				bomDes.Append(iInputBufferPtr);
				if(iBomBuffer->Length() >=3)
					{
					iInputBufferPtr.Set(bomDes);
					iCurrentInputBufferLen = newLength;

					SetActive();
					User::RequestComplete(s, KErrNone);
					}
				else
					{
					GetMoreData();
					}
				}
			}
		else
			{
			iState = KParseData;
			SetActive();
			User::RequestComplete(s, KErrNone);
			}
		break;

	case KWaitingForData:
		switch(err)
			{
		case MMDXMLParserDataProvider::KMoreData:
			// We got more data this time, make sure all the parameters are correct.
			iCurrentInputBufferLen = iInputBufferPtr.Length();				

			iState = iPreviousState;
			SetActive();
			User::RequestComplete(s, KErrNone);
			break;

		case MMDXMLParserDataProvider::KDataStreamEnd:
			iState = KFinished;
			SetActive();
			User::RequestComplete(s, KErrNone);
			break;

		default:
		case MMDXMLParserDataProvider::KDataStreamError:
			User::Leave(KErrCorrupt);
			break;
			}

		break;

	case KParseData:
		if(!iParentElement)	// initialise the parsing
			{
			iOpened = EFalse;
			iClosed = EFalse;
			// If we're going through the tag and find a < we don't know whether
			// it will be valid (eg it starts a CDataSection) or whether it's an
			// illegal character.  Store its index so that when we're a bit further
			// along in the string we can check whether it was allowed and if not 
			// set an Illegal Character error.
			delete iNewElement;
			iNewElement = NULL;
			
			__ASSERT_DEBUG(iXMLDoc != NULL, Panic(ENullMemVarXMLDoc));
			iParentElement = iXMLDoc->DocumentElement();

			delete iElementTag;
			iElementTag = NULL;
			iElementTag = HBufC::NewL(KNominalTagLength);
			}

		if(!iBytesPerChar)
			{
			// assume ascii/UTF8
			iBytesPerChar = 1;
			}

		if(!DoParseLoopL())
			iState = KFinished;

			break;

	case KFinished:
		// Check for any errors that we can pick up now, like missing doctype, or incomplete content
		CheckForErrors();

		// we want to leave this instance in a safe state where it can be restarted again with a single call to ParseSource.
		// cannot delete this element, as it belongs to the document...
		iParentElement = NULL;
		iState = KDetermineCharset;

		iDataSource->Disconnect();
		iDataSource=NULL;
		__ASSERT_DEBUG(iParserObserver != NULL, Panic(ENullMemVarParserObserver));
		iParserObserver->ParseFileCompleteL();
		break;

	default:
		User::Leave(KErrUnknown);
		break;
		}
	}

void CMDXMLParser::CheckForErrors()
	{
	if(iError == KErrNone)
		{
		if(iParentElement)
			{
			// if iParentElement is not pointing to dummy root node, there has been a problem
			if( (iParentElement == NULL) || (iParentElement->NodeName() != KXMLDocumentElementNodeName) )
				{
				SetError(KErrXMLIncomplete, EXMLWorkable);
				}
			else if(!iParentElement->CheckChildren())
				{
				SetError(KErrXMLInvalidChild, EXMLWorkable);
				}
			else if(iParentElement->FirstChild() != NULL)
				{
				// multiple real (not dummy) root elements
				TInt count = 0;
				CMDXMLNode* iterator = iParentElement->FirstChild();
				do
					{
					if(iterator->NodeType() == CMDXMLNode::EElementNode)
						{
						count++;
						}
					iterator = iterator->NextSibling();
					}
				while(iterator != NULL);

				if(count != 1)
					{
					SetError(KErrXMLMultipleRootElements, EXMLWorkable);
					}
				}
			}
		}
	if(iError == KErrNone && !iDocTypeSet)
		{
		SetError(KErrXMLMissingDocTypeTag, EXMLWorkable);
		}

	if(iError == KErrNone && !iVersionSet)
		{
		SetError(KErrXMLMissingVersionTag, EXMLWorkable);
		}
	}


void CMDXMLParser::CreateDocumentL()
//
// Creates a generic or DTD-specific document object
// @leave can Leave due to OOM
//
	{
	delete iXMLDoc;
	iXMLDoc = NULL;

	if (iDtdRepresentation != NULL)
		iXMLDoc = CMDXMLDocument::NewL(*iDtdRepresentation);
	else
		iXMLDoc = CMDXMLDocument::NewL();
	}


//==================================================================================

TBool CMDXMLParser::DocTypeL()
//
// @return Returns true if the current tag is a doctype tag and sets the
// Document DocType member accordingly on the first pass of this function.
//
	{
	TBool returnValue = EFalse;
	TInt tagIdLen = TPtrC(KXMLDocumentTypes).Length();

	__ASSERT_DEBUG(iElementTag != NULL, Panic(ENullMemVarElementTag));

	if(iElementTag->Length() > tagIdLen
			&& iElementTag->Left(tagIdLen) == KXMLDocumentTypes)
		{
		if(iDocTypeSet)
			{
			SetError(KErrXMLDuplicateDocTypeTags, EXMLWorkable);
			}
		else
			{
			iXMLDoc->SetDocTypeTagL(iElementTag->Des());
			iDocTypeSet = ETrue;
			}
		returnValue = ETrue;
		iEndOfTag = ETrue;
		}

	return returnValue;
	}

//==================================================================================

TBool CMDXMLParser::ProcessingInstructionL(CMDXMLElement* aParentElement)
//
// creates a new processing instruction if necessary and adds to document
// @return Returns true if the current tag is a processing instruction
//
	{
	TBool returnValue = EFalse;
	TInt startPILen = TPtrC(KXMLStartProcessingInstruction).Length();
	TInt endPILen = TPtrC(KXMLEndProcessingInstruction).Length();

	__ASSERT_DEBUG(iElementTag != NULL, Panic(ENullMemVarElementTag));

	if((iElementTag->Left(startPILen) == KXMLStartProcessingInstruction) &&
		(iElementTag->Right(endPILen) == KXMLEndProcessingInstruction))
		{
		if(aParentElement != NULL)
			{
			CMDXMLProcessingInstruction* inst = CMDXMLProcessingInstruction::NewLC(iXMLDoc);

			TPtrC instStr = iElementTag->Des().Mid(startPILen,
				iElementTag->Length() - (startPILen + endPILen));
			inst->SetDataL(instStr);

			__ASSERT_DEBUG(aParentElement != NULL, Panic(ENullParameterParentElement));
			TInt error = aParentElement->AppendChild(inst);
			CleanupStack::Pop(inst);

			if(error != KErrNone)
				{
				SetError(error, EXMLWorkable);
				}
			}

		returnValue = ETrue;
		iEndOfTag = ETrue;
		}

	return returnValue;
	}


//==================================================================================

TBool CMDXMLParser::CDataSectionL(CMDXMLElement* aParentElement)
//
// creates a new CDataSection if necessary and adds to document
// @return Returns true if the current tag is a CDataSection
//
	{
	TBool returnValue = EFalse;
	TInt instLen = TPtrC(KXMLStartCDataSection).Length();
	TInt endCDataLen = TPtrC(KXMLEndCDataSection).Length();

	__ASSERT_DEBUG(iElementTag != NULL, Panic(ENullMemVarElementTag));

	if (iElementTag->Left(instLen) == KXMLStartCDataSection) 
		{
		returnValue = ETrue;
		if ((iElementTag->Right(endCDataLen) == KXMLEndCDataSection) && (aParentElement != NULL))
			{
			CMDXMLCDATASection* inst = CMDXMLCDATASection::NewLC(iXMLDoc);

			TPtrC instStr = iElementTag->Des().Mid(instLen,
				iElementTag->Length() - (instLen + endCDataLen));
			inst->SetDataL(instStr);

			__ASSERT_DEBUG(aParentElement != NULL, Panic(ENullParameterParentElement));
			TInt error = aParentElement->AppendChild(inst);
			CleanupStack::Pop(); // inst

			if(error != KErrNone)
				{
				SetError(error, EXMLWorkable);
				}
			}
		iEndOfTag = ETrue;
		}

	return returnValue;
	}


//==================================================================================

TBool CMDXMLParser::VersionIDL()
//
// @return returns true if the current tag is a version id tag and sets the
// Document Version member accordingly on the first pass of this function.
//
	{
	TBool returnValue = EFalse;
	TInt tagIdLen = TPtrC(KXMLVersion).Length();

	__ASSERT_DEBUG(iElementTag != NULL, Panic(ENullMemVarElementTag));

	if(iElementTag->Length() > tagIdLen
			&& iElementTag->Left(tagIdLen) == KXMLVersion)
		{
		if(iVersionSet)
			{
			SetError(KErrXMLDuplicateVersionTags, EXMLWorkable);
			}
		else
			{
			iXMLDoc->SetVersionTagL(iElementTag->Des());
			iVersionSet = ETrue;
			}
		returnValue = ETrue;
		iEndOfTag = ETrue;
		}

	return returnValue;
	}

//==================================================================================

TBool CMDXMLParser::CommentL(CMDXMLElement* aParentElement)
//
// creates a new comment if necessary and adds to document
// @return returns true if the current tag is a comment tag
//
	{
	TBool returnValue = EFalse;
	TInt commentLen = TPtrC(KXMLStartComment).Length();
	TInt endCommentLen = TPtrC(KXMLEndComment).Length();

	__ASSERT_DEBUG(iElementTag != NULL, Panic(ENullMemVarElementTag));

	if (iElementTag->Left(commentLen) == KXMLStartComment) 
		{
		returnValue = ETrue;
		if ((aParentElement != NULL) && (iElementTag->Right(endCommentLen) == KXMLEndComment))
			{
			CMDXMLComment* comment = CMDXMLComment::NewLC(iXMLDoc);

			TPtrC commentStr = iElementTag->Des().Mid(commentLen,
				iElementTag->Length() - (commentLen + endCommentLen));
			comment->SetDataL(commentStr);

			__ASSERT_DEBUG(aParentElement != NULL, Panic(ENullParameterParentElement));
			TInt error  = aParentElement->AppendChild(comment);
			CleanupStack::Pop(); // comment

			if(error != KErrNone)
				{
				SetError(error, EXMLWorkable);
				}
			iEndOfTag = ETrue;
			}
		}

	return returnValue;
	}

//==================================================================================

EXPORT_C void CMDXMLParser::SetSourceCharacterWidth(TMDXMLParserInputCharWidth aWidth)
	{
	iBytesPerChar = aWidth;
	}

//==================================================================================

void CMDXMLParser::GetMoreData()
	{

	// Prepare these variables.
	iNextChar = 0;					// reading from the start of the buffer
	iCurrentInputBufferLen = 0;		// we have no characters in the buffer
	iUnicodeConversion.Zero();		// we have no characters in our UTF8->Unicode Conversion buffer.
	iUnicodeConversionLen = 0;
	iUnicodeReadPos = 0;

	// Request more data from the data provider
	__ASSERT_DEBUG(iDataSource != NULL, Panic(ENullMemVarDataSource));
	iDataSource->GetData(iInputBufferPtr, iStatus);
	SetActive();
	iPreviousState = iState;
	iState = KWaitingForData;
	}

//==================================================================================

CMDXMLParser::TGetCharReturn CMDXMLParser::GetDoubleByteChar(TDes& aChar)
	// when inputing we have a pointer to an 8 bit buffer (iInputBufferPtr), for unicode
	// input we point a 16 bit descriptor (tempUnicodeInput) at the 8 bit buffer to
	// enable us to read the 2 x 8 bit chars as a single 16 bit char.
	// However it isn't always this simple as the data provider interface makes no guarantees
	// on the alignment of this data. It's perfectly possible for it to end up with a unicode
	// character where the high byte comes from the previous buffer and the low byte comes from
	// the current one.  This will put the rest of the current buffer out of line, and also all
	// subsequent buffers unless an odd length buffer is provided.  Hopfully this won't happen often.
	{
	aChar.Zero();

	if(iUnicodeInputMisaligned)
		{
		TUint16 tempOut;
		TUint8* tempRead;
		tempRead = (TUint8*)(iInputBufferPtr.Ptr()) + iNextChar;

		if(iCurrentInputBufferLen - iNextChar >=1)
		{
			// if we saved a byte last time, lets use that first
			if(iSpareChar.Length())
				{
				tempOut = iSpareChar[0];
				iSpareChar.Zero();
				}
			else if(iCurrentInputBufferLen - iNextChar >=2)
				{
				// we didn't save a byte, so we read from the stream.
				tempOut = *tempRead;
				tempRead++;
				iNextChar++;
				}
			else
				{
				// our input stream must have been an odd length - this might cause alignment problems, 
				// so we need to start reading bytewise for a while
				iUnicodeInputMisaligned = ETrue;

				TUint8* tempRead = (TUint8*)(iInputBufferPtr.Ptr()) + iNextChar;
				TUint16 tempVal = (TUint16)(*tempRead << 8);
				iSpareChar.Copy(&tempVal,1);

				GetMoreData();
				return KWaitForChar;

				}

			// second byte (high byte) of our output comes from the input stream in all cases.
			tempOut |= ((*tempRead & 0xFF) << 8);
			iNextChar++;

			TPtrC16 readDes(&tempOut, 1);
			aChar = readDes.Left(1);
			}
		}
	else if(iCurrentInputBufferLen - iNextChar >= 2)
		{
		// we may be in a position where we don't know we're going to lose a byte
		// so we'll test for that ahead of time, and then handle that in the normal way

		// if we execute this, it means that we have two bytes available to read.
		const TUint16* word = reinterpret_cast<const TUint16*>((iInputBufferPtr.Ptr() + iNextChar));
		TPtrC16 tempUnicodeInput(word, 2);
		aChar = tempUnicodeInput.Left(1);
		iNextChar+=2;
		return KCharReturned;
		}
	
	TInt bytesRemaining = iCurrentInputBufferLen - iNextChar;

	switch(bytesRemaining)
		{
		case 1:
			{
			// our input stream must have been an odd length - this might cause alignment problems, 
			// so we need to start reading bytewise for a while

			TUint8* tempRead = (TUint8*)(iInputBufferPtr.Ptr()) + iNextChar;
			TUint16 tempVal = *tempRead;
			iSpareChar.Copy(&tempVal,1);
			iUnicodeInputMisaligned = ETrue;
			iNextChar++;
			if(!aChar.Length())
				{
				GetMoreData();
				return KWaitForChar;
				}
			}
			break;
		case 0:
			{
			// we're at the end of this block, and it's turned out to be re-aligned.
			// we can read subsequent blocks in 16-bit chunks.
			iUnicodeInputMisaligned = EFalse;
			}
			break;
		}

	return KCharReturned;
	}
	
//==================================================================================

CMDXMLParser::TGetCharReturn CMDXMLParser::GetSingleByteChar(TDes& aChar)
	{
	// We have UTF8/ASCII Source, and we must need to convert some more if we got here.
	iUnicodeConversion.Zero();
	iUnicodeConversionLen = 0;
	iUnicodeReadPos = 0;

	// if we are not operating out of the edge buffer yet, work on the real one
	if(!iUTF8EdgeBuffer)
		{
		// This is an 8 bit encoding, probably UTF-8, but could be ASCII.  Because
		// ASCII is valid UTF-8 we can just convert what we have to Unicode.
		// We're going to convert a number of characters at a time here.
		TInt inputBytesRemaining = iCurrentInputBufferLen - iNextChar;
		TInt convResult;
		TPtrC8 tempPtr( (TUint8*)(iInputBufferPtr.Ptr()) + iNextChar, inputBytesRemaining );

		convResult = CnvUtfConverter::ConvertToUnicodeFromUtf8(iUnicodeConversion, tempPtr);
		if((convResult >= 0) || (convResult == KErrCorrupt))
			{
			// Sometimes the UTF8 decoder might return corrupt if it only gets a single character
			// in this case we ignore the error and report that we have converted 0 characters
			if (convResult == KErrCorrupt)
				convResult = tempPtr.Length();

			// This is the number of bytes converted.
			// Keep an eye out in case there is no change in the character consumed count.
			TInt bytesConverted = inputBytesRemaining - convResult;

			// We consumed characters.  Make our input buffer read position correct.
			iNextChar += bytesConverted;

			// Make our intermediate buffering correct and return the first character out of our buffer
			// subsequent calls will just return characters from this buffer.
			iUnicodeConversionLen = iUnicodeConversion.Length();
			aChar = iUnicodeConversion.Left(1);
			iUnicodeReadPos = 1;

			if(convResult && convResult < KUTF8EdgeBufferLen)
				{
				TUint8* multiByteCheck = (TUint8*)(iInputBufferPtr.Ptr()) + iNextChar;

				// There is a possibility that we've got an edge case here
				//check if our left over character is in fact UTF8.
				if((0x80 & *multiByteCheck) != 0)
					{
					// Shift 'convResult' characters off into the edge buffer.
					delete iUTF8EdgeBuffer;
					iUTF8EdgeBuffer = HBufC8::New(KUTF8EdgeBufferLen);
					*iUTF8EdgeBuffer = iInputBufferPtr.Right(convResult);

					TUint8 bitMask = 0x80;
					TInt   byteCount = 0;
					while(bitMask && (bitMask & (iUTF8EdgeBuffer->Des()[0])) != 0)
						{
						bitMask >>= 1;
						byteCount++;
						};

					if(!bitMask)
						{
						// the utf8 stream appears to be corrupt.
						SetError(KError, EXMLFatal);
						return KError;
						}
					// we need to find byteCount characters to make up the character currently stored in the edge
					// buffer.
					iRequiredUTF8Bytes = byteCount - iUTF8EdgeBuffer->Length();

					// set the variables up so that we return any converted characters, and then begin work
					// on the edge buffer (where we've already cached the remaining bytes if any)
					// NOTE: We will return all the characters which we preconverted into iUnicodeConversion *before*
					// we begin dealing with the edge buffer, because of the structure of this function.
					iNextChar = iCurrentInputBufferLen;

					if (bytesConverted == 0)
						// If no bytes were converted then there is nothing to return,
						// we need to wait for more data and then conbine it with what we have
						// just put on the edge buffer.
						{
						// need more bytes to finish this character.
						GetMoreData();
						return KWaitForChar;			
						}
					}
				}
			}

		else
			{
			return KError;	// something failed in the UTF8 Converter.
			}
		}
	else
		{
		// We are converting the UTF8 Edge Buffer.  We know that in it's current state, it 
		// can't be converted to Unicode.
		// Decide if we have enough characters in our current input stream to convert to an
		// output character (or two) yet.
		
		if(iUTF8EdgeBuffer->Length() >= KUTF8EdgeBufferLen)
			{
			// our edge buffer reached the maximum length for a UTF8 character
			// and we haven't managed to convert a unicode output.
			// this means that the input stream is corrupt.
			delete iUTF8EdgeBuffer;
			iUTF8EdgeBuffer = NULL;

			// Report a fatal error.
			SetError(KError, EXMLFatal);
			return KError;
			}
		else
			{
			TInt convResult;

			// we know how many bytes are required in order to complete the utf8 buffer
			TInt bytesAvailable = iCurrentInputBufferLen - iNextChar;

			if(bytesAvailable >= iRequiredUTF8Bytes)
				{
				// we have enough bytes to complete this character.
				// Go ahead and convert then return it.
				iUTF8EdgeBuffer->Des().Append(iInputBufferPtr.Mid(iNextChar, iRequiredUTF8Bytes));
				iUnicodeConversion.Zero();
				convResult = CnvUtfConverter::ConvertToUnicodeFromUtf8(iUnicodeConversion, iUTF8EdgeBuffer->Des());
				
				// regardless if we managed to convert this buffer or not, we don't need it any more.
				delete iUTF8EdgeBuffer;
				iUTF8EdgeBuffer = NULL;

				// make sure we report any error in the conversion
				if(convResult != 0)
				{
					// we either incorrectly calculated the required number of bytes or the
					// stream is corrupt.  Either way we have a fatal error.
					SetError(KError, EXMLFatal);
					return KError;
				}

				// Make our intermediate buffering correct and return the first character out of our buffer
				// subsequent calls will just return characters from this buffer.
				iUnicodeConversionLen = iUnicodeConversion.Length();
				aChar = iUnicodeConversion.Left(1);
				iUnicodeReadPos = 1;

				// set up the main input buffers so that the next char comes from the input stream.
				iNextChar += iRequiredUTF8Bytes;
				iRequiredUTF8Bytes = 0;
				}
			else
				{
				// we haven't got enough bytes to complete this character, store the 
				// available byte(s) and request more data.
				iUTF8EdgeBuffer->Des().Append(iInputBufferPtr.Mid(iNextChar, bytesAvailable));

				// Move the next character index on for as many bytes as we have just added to the edge buffer
				iNextChar += bytesAvailable;
				// We can reduce the number of bytes require by the number of bytes added to the edge buffer
				iRequiredUTF8Bytes -= bytesAvailable;

				// need more bytes to finish this character.
				GetMoreData();
				return KWaitForChar;			
				}
			}
		}

	return KCharReturned;
	}

//==================================================================================

CMDXMLParser::TGetCharReturn CMDXMLParser::GetChar(TDes& aChar)
//
// Fetch one character from the input file
// @param aChar the returned character.
// @return returns true if a character returned or false if the file is finished
//
	{
	// first test - see if we're providing preconverted characters.
	if(iUnicodeConversionLen && iUnicodeReadPos < iUnicodeConversionLen)
		{
		// return one of the preconverted chars.
		aChar = iUnicodeConversion.Mid(iUnicodeReadPos, 1);
		iUnicodeReadPos++;

		return KCharReturned;
		}
	
	// Second test - see if we require more data.  If we have converted data and we require
	// more data, this code is not intelligent enough to request data from the provider
	// early, but that's ok.

	// Buffer length held as a member variable for performance reasons
	// this function will be accessed thousands of times
	if(iCurrentInputBufferLen <= iNextChar)
		{
		GetMoreData();
		return KWaitForChar;	
		}

	// return the character, handling any of the buffer shuffling we need to do.
	if(iBytesPerChar == 2)
		return GetDoubleByteChar(aChar);
	else	
		return GetSingleByteChar(aChar);
	}

//==================================================================================

CMDXMLElement* CMDXMLParser::ParseStartTagL()
//
// Parse a start of element tag and create an element with attributes set.
// @return Returns a pointer to the created element
// @leave can Leave due to OOM
//
	{
	__ASSERT_DEBUG(iElementTag != NULL, Panic(ENullMemVarElementTag));

	// there must be at least two angle brackets and a single character to be meaningful
	if(iElementTag->Length() < 3)
		return NULL;

	CMDXMLElement* newElement = NULL;
	TPtr elementTagPtr = iElementTag->Des();

	// remove the angle brackets and trim white space
	if(iElementTag->Right(TPtrC(KXMLEndStartTag).Length()) == KXMLEndStartTag)
		elementTagPtr = iElementTag->Left(iElementTag->Length() - TPtrC(KXMLEndStartTag).Length());
	else
		elementTagPtr = iElementTag->Left(iElementTag->Length() - TPtrC(KXMLEndTag).Length());

	elementTagPtr = iElementTag->Right(iElementTag->Length() - TPtrC(KXMLStartTag).Length());
	elementTagPtr.Trim();

	// find out where the name ends and the attributes begin
	TLex16 element(elementTagPtr);
	element.SkipCharacters();
	TInt endOfName = element.Offset();

	// separate out the name from the attributes
	HBufC* elementName = (iElementTag->Left(endOfName)).AllocLC();
	TPtr elementNamePtr = elementName->Des();
	elementNamePtr.TrimRight();

	TInt error = KErrNone;

	__ASSERT_DEBUG(iXMLDoc != NULL, Panic(ENullMemVarXMLDoc));
	
	TBool validElement = iXMLDoc->ValidElementNameL(elementNamePtr);
	if(validElement || iStoreInvalid)
		{
		// remove the actual name from the tag so we only pass on the attributes
		elementTagPtr = iElementTag->Right(iElementTag->Length() - endOfName);
		elementTagPtr.TrimLeft();
		newElement = CMDXMLElement::NewLC(iXMLDoc->CanElementHaveChildren(elementNamePtr), iXMLDoc, elementNamePtr);

		error = ParseElementAttributesL(*newElement, elementTagPtr);
		CleanupStack::Pop();
		}
	if(!validElement)
		{
		error = KErrXMLInvalidElement;
		}

	CleanupStack::PopAndDestroy(elementName); // elementName

	if(error != KErrNone)
		{
		SetError(error, EXMLWorkable);
		}

	return newElement;

	}


TInt CMDXMLParser::ParseElementAttributesL(CMDXMLElement& aElement, TDes& aTagToParse)
//
// This function is used to parse the attributes.
// @param aElement The element to which the attributes belong
// @param aTagToParse The tag to be parsed
// @return Returns KErrNone if both attribute name & value are valid 
// KErrXMLBadAttributeName if attribute name is invalid or KErrXMLBadAttributeValue is invalid
// @leave can Leave due to OOM
//
	{
	TInt error = KErrNone;
	TInt attributeError = KErrNone;
	HBufC* attributeName = NULL;
	HBufC* attributeValue = NULL;
	TBuf<1> attributeDelimiter; // may be " or '
	TInt offset = KErrNone;

	offset = aTagToParse.Find(KEqualSign);
	while(offset != KErrNotFound)
		{
		attributeName = TPtrC(aTagToParse.Left(offset)).AllocLC();
		TPtr attributeNamePtr = attributeName->Des();
		attributeNamePtr.TrimRight(); // remove white space that existed between name and equal sign

		// remove current attribute name and equal sign from string
		aTagToParse = aTagToParse.Right(aTagToParse.Length() - (offset + 1));
		aTagToParse.TrimLeft(); // remove white space that existed between equal sign and delimiter

		if(error == KErrNone && aTagToParse.Length() < 2) // a name must be at followed by at least 2 delimiters
			{
			error = KErrXMLBadAttributeName;
			// In this case, there is insufficient tag left to contain any more attributes
			}
		else if(error == KErrNone && aElement.IsAttributeSpecified(*attributeName))
			{
			error = KErrXMLDuplicateAttributeName;
			// We need to remove the attribute value from the tag string or it will be
			// picked up as part of the next attribute name.
			attributeDelimiter = aTagToParse.Left(1);

			// Check in case we've got a missing " at the beginning of the attribute.  If
			// we do, we need to try a different strategy to find the end of this attribute
			if (attributeDelimiter != KQuotation && attributeDelimiter != KApostrophe)
				{
				offset = LocateNextAttribute(aTagToParse);
				}
			else
				{
				// remove start delimiter then search for next one (end delimiter)
				aTagToParse = aTagToParse.Right(aTagToParse.Length() - 1);
				offset = FindDelimiter(aTagToParse, attributeDelimiter);
				}

			if(offset != KErrNotFound)
				{
				// remove current attribute value and delimiter
				aTagToParse = aTagToParse.Right(aTagToParse.Length() - (offset + 1));
				aTagToParse.TrimLeft(); // remove white space that existed between delimiter and next name
				}
			}
		else
			{
			attributeDelimiter = aTagToParse.Left(1);

			if (attributeDelimiter != KQuotation && attributeDelimiter != KApostrophe)
				{
				// This attribute doesn't have a valid delimiter.  Try and find the beginning of the next
				// attribute and just cut this one.
				TInt nextAttribute = LocateNextAttribute(aTagToParse);
				if(nextAttribute > 0)
					{
					// Add one to next attribute because the offset includes the whitespace before it
					aTagToParse = aTagToParse.Right(aTagToParse.Length() - (nextAttribute + 1));
					}

				if (error == KErrNone)
					{
					error = KErrXMLBadAttributeValue;
					}
				}
			else
				{
				// remove start delimiter then search for next one (end delimiter)
				aTagToParse = aTagToParse.Right(aTagToParse.Length() - 1);

				offset = FindDelimiter(aTagToParse, attributeDelimiter);
				if(offset != KErrNotFound)
					{
					attributeValue = TPtrC(aTagToParse.Left(offset)).AllocLC();
					TPtr attributeValuePtr = attributeValue->Des();
					attributeValuePtr.TrimRight(); // remove white space that existed between value and delimiter

					// remove current attribute value and delimiter
					aTagToParse = aTagToParse.Right(aTagToParse.Length() - (offset + 1));
					aTagToParse.TrimLeft(); // remove white space that existed between delimiter and next name

					// Entity convert this attribute
					attributeError = ParseSingleAttributeL(attributeValuePtr);
					if( attributeError != KErrNone && error == KErrNone)
						{
						error = attributeError;
						}

					attributeError = aElement.SetAttributeL(*attributeName, *attributeValue, iStoreInvalid);
					if( attributeError != KErrNone && error == KErrNone)
						{
						error = KErrXMLInvalidAttribute;
						}
					CleanupStack::PopAndDestroy(attributeValue); //attributeValue
					}
				else if(error == KErrNone)
					{
					error = KErrXMLBadAttributeValue;
					}
				}
			}

		// next attribute pair
		offset = aTagToParse.Find(KEqualSign);
		CleanupStack::PopAndDestroy(attributeName); //attributeName
		}

	if(error == KErrNone && aTagToParse.Length() != 0)
		{
		error = KErrXMLBadAttributeValue;
		}

	return error;
	}

TInt CMDXMLParser::LocateNextAttribute(const TDesC& aTagToParse)
	{
	// Find the next attribute by looking for an = then search back for a ' '.
	// This is useful when you've hit rubbish parsing the content of a start tag
	// and are looking for somewhere sensible to start.
	TInt nextAttribute = KErrNotFound;
	TInt offset = aTagToParse.Find(KEqualSign);

	// If the = is the first character then there isn't space for a ' ' so
	// don't bother looking
	if(offset > 0)
		{
		TPtrC invalidText = aTagToParse.Left(offset);
		nextAttribute = invalidText.LocateReverse(' ');
		}

	return nextAttribute;
	}

TInt CMDXMLParser::ParseElementEndTag(CMDXMLElement& aElement, const TDesC& aTagToParse)
//
// Parses an end tag.  In fact, at this point the end tag must match
// the tag name of the start tag.  
// @param aTagToParse Text of the end tag.
// @return Returns KErrNone if the end tag matches the start tag or KErrNotFound if there is a mismatch.
//
	{
	// The tag should be of the form '</tag>' where tag is the name of this element so we will
	// check and strip off the surrounding </  > and then compare the remains with this #
	// node name.
	TInt retVal = KErrNone;
	if( aTagToParse.Length() != (aElement.NodeName().Length()+3))
		{
		retVal = KErrNotFound;
		}
	else
		{
		TInt startEndTagLen = TPtrC(KXMLStartEndTag).Length();
		TInt endTagLen = TPtrC(KXMLEndTag).Length();

		if((aTagToParse.Left(startEndTagLen).Compare(KXMLStartEndTag) == 0) &&
				(aTagToParse.Right(endTagLen).Compare(KXMLEndTag) == 0))
			{
			if(aElement.NodeName().Compare(aTagToParse.Mid(2,
						aTagToParse.Length() - (startEndTagLen + endTagLen))) != 0)
				{
				retVal = KErrNotFound;
				}
			}
		}
	return retVal;
	}


//==================================================================================

EXPORT_C void CMDXMLParser::SetError(const TInt aErrorCode, const TXMLErrorCodeSeverity aSeverity)
//
// Sets iError to new errorcode if more serious than any error so far encountered
//
	{
	if(iSeverity > aSeverity)
		{
		iSeverity = aSeverity;
		iError = aErrorCode;
		}
	}

//==================================================================================

EXPORT_C TInt CMDXMLParser::Error() const
	{
	return iError;
	}

//==================================================================================

EXPORT_C TXMLErrorCodeSeverity CMDXMLParser::ErrorSeverity() const
	{
	return iSeverity;
	}

//==================================================================================

void CMDXMLParser::HandleTextL(TDes& aChar)
//
// Called when a character is read in and found to bo outside of an element tag
//
	{
	// Save the text in a buffer.
	// This text will get added as as a child element when the next tag is encounted.
	if (iText == NULL)
		iText = HBufC::NewL(KGMXMLDefaultTextBufferSize);
	
	if (iText->Length() == iText->Des().MaxLength())
		// The buffer will overflow if we add another character.
		// Need to reallocate.
		{
		iText = iText->ReAllocL(iText->Des().MaxLength() + KGMXMLDefaultTextBufferSize);
		}

	iText->Des().Append(aChar);
	}

TBool CMDXMLParser::EndOfCDataSection()
	{
	TBool endOfCData = EFalse;
	TPtrC cdataEndSection(KXMLEndCDataSection);
	TInt instLen = TPtrC(KXMLEndCDataSection).Length()-1;

	__ASSERT_DEBUG(iElementTag != NULL, Panic(ENullMemVarElementTag));
	if(iElementTag->Right(instLen) == cdataEndSection.Left(instLen))
		{
		if(iElementTag->Left(instLen) == KXMLStartCDataSection)
			endOfCData = ETrue;
		}

	return endOfCData;
	}

TInt CMDXMLParser::CheckForStartCData(const TDesC& aTextToCheck)
	{
	TInt index;
	index = aTextToCheck.Find(KXMLStartCDataSection);
	return index;
	}

TInt CMDXMLParser::FindDelimiter(TDesC& aDataToSearch, TDesC& aDelimiterToFind)
	{
	TInt currentOffset = 0;
	TInt nextDelimiter = KErrNotFound;
	TBool valid = EFalse;
	TPtrC unsearchedData(aDataToSearch);

	while (!valid && ((nextDelimiter = unsearchedData.Find(aDelimiterToFind)) != KErrNotFound))
		{
		// If this isn't the first time round the loop (When currentOffset == 0) we're moved
		// our attention to the character after the delimiter we found, so add one to currentOffset
		if(currentOffset != 0)
			{
			currentOffset += 1;
			}
		// We have a delimiter, add the position of this to currentOffset
		currentOffset += nextDelimiter;

		// Check whether this delimiter is in a CDataSection, it's valid if it isn't
		TPtrC delimiterToCheck = aDataToSearch.Left(currentOffset);
		valid = !InCDataSection(delimiterToCheck);

		// Move on to the next section of text in case this one wasn't valid
		unsearchedData.Set(aDataToSearch.Mid(currentOffset + 1));
		}

	if ((nextDelimiter == KErrNotFound) && (!valid))
		{
		return KErrNotFound;
		}
	else
		{
		return currentOffset;
		}
	}

void CMDXMLParser::AddTextL(CMDXMLElement* aParentElement)
	{
	if ((aParentElement != NULL) && (iText != NULL))
	// Add any buffered text to the parent element unless it contains only whitespace
		{
		// Strip off any leading whitespace
		TInt stripCounter = 0;
		
		if (!iPreserve) // GmXML consumes whitespace characters 
			{
			TBool endOfWhitespace = EFalse;
			while ((stripCounter < iText->Length()) && (!endOfWhitespace))
				{
				// If character is not 0x20 (space) and not between 0x09 and 0x0d 
				// it isn't whitespace 
				if( ((*iText)[stripCounter] != 0x20) &&
					!((*iText)[stripCounter] >= 0x09 && (*iText)[stripCounter] <= 0x0d))
					{
					endOfWhitespace = ETrue;
					}
				else
					{
					stripCounter++;
					}
				}
			}

		HBufC* strippedText = TPtrC(iText->Right(iText->Length() - stripCounter)).AllocLC();

		if (strippedText->Length() > 0)
			// If there is anything left of the stripped text then entity convert and add it.
			{
			TPtr toConvert = strippedText->Des();
			TInt error = iEntityConverter->EntityToTextL(toConvert);
			if( error != KErrNone )
				{
				SetError(error, EXMLIndeterminate);
				}
			CMDXMLText* textElement = CMDXMLText::NewLC(iXMLDoc);
			textElement->SetDataL(*strippedText);
			CleanupStack::Pop(textElement);
			TInt err = aParentElement->AppendChild(textElement);
			if(err != KErrNone)
				{
				SetError(err, EXMLWorkable);
				}			
			}

		CleanupStack::PopAndDestroy(strippedText);
		iText->Des().Zero();
		}
	}

TBool CMDXMLParser::InCDataSection(TDesC& aDataToSearch)
	{
	TBool inCDataSection = EFalse;
	TInt startCData = CheckForStartCData(aDataToSearch);
	TInt endCData = 0;

	while ((startCData != KErrNotFound) && !inCDataSection)
		{
		// We only want to look for the end of the CDataSection in the part of
		// aDataToSearch after the start of the CDataSection.  We know that the 
		// first (TPtrC)KXMLStartCDataSection.Length() of the data we're looking
		// at won't match because it's the start tag, but it's probably more 
		// efficient to check the extra few characters than to work out the 
		// length of the tag so we can ignore them.
		startCData += endCData;
		TPtrC afterStart = aDataToSearch.Mid(startCData);
		endCData = afterStart.Find(KXMLEndCDataSection);
		if (endCData == KErrNotFound)
			{
			// We haven't found a match for the start of the CDataSection so
			// we must still be in it -> "<" is valid.
			inCDataSection = ETrue;
			}
		else 
			{
			// We found a match for the start of the CDataSection.  Check to
			// see if another one has started since then.
			endCData += startCData;
			TPtrC afterEnd = aDataToSearch.Mid(endCData);
			startCData = CheckForStartCData(afterEnd);
			}
		}

	return inCDataSection;
	}

TInt CMDXMLParser::ParseSingleAttributeL(TDes& aAttributeValue)
	{
	TInt error = KErrNone;
	TInt beginSection = 0;
	TInt endSection = aAttributeValue.Find(KXMLStartCDataSection);
	
	// We've found at least one CDataSection
	while(endSection != KErrNotFound)
		{
		// Entity convert this plain text section
		HBufC* textToConvert = TPtrC(aAttributeValue.Mid(beginSection, endSection)).AllocLC();
		TPtr toConvert = textToConvert->Des();
		error = iEntityConverter->EntityToTextL(toConvert);
		aAttributeValue.Replace(beginSection, endSection, *textToConvert);
		CleanupStack::PopAndDestroy(textToConvert);

		// Move on our markers.  We start the new section at the end of the old one.
		beginSection += endSection;
		// The end of this new section is the end of the CDataSection
		endSection = TPtrC(aAttributeValue.Mid(beginSection)).Find(KXMLEndCDataSection);

		if(endSection != KErrNotFound)
			{
			// Now move on our markers again.  Start at the end of the CDataSection, 
			// plus the length of the end tag, and continue to the beginning of the next one.
			beginSection += endSection + TPtrC(KXMLEndCDataSection).Length();
			endSection = TPtrC(aAttributeValue.Mid(beginSection)).Find(KXMLStartCDataSection);
			}
		else
			{
			// There's an unterminated CDataSection in our attribute
			error = KErrXMLBadAttributeValue;
			}
		}

	// There are no more CDataSections, entity convert the rest of the string
	if(!error)
		{
		HBufC* textToConvert = TPtrC(aAttributeValue.Mid(beginSection)).AllocLC();
		TPtr toConvert = textToConvert->Des();
		error = iEntityConverter->EntityToTextL(toConvert);
		aAttributeValue.Replace(beginSection, (aAttributeValue.Length()-beginSection), *textToConvert);
		CleanupStack::PopAndDestroy(textToConvert);
		}

	return error;
	}


EXPORT_C void CMDXMLParser::PlaceholderForRemovedExport1(MMDXMLParserObserver* /*aParserObserver*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}

EXPORT_C void CMDXMLParser::PlaceholderForRemovedExport2(MMDXMLParserObserver* /*aParserObserver*/, MXMLDtd* /*aDtdRepresentation*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}

EXPORT_C void CMDXMLParser::PlaceholderForRemovedExport3()
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}

// End of File
