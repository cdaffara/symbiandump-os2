// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// GMXMLCOMPOSE.CPP
// @file
// This file contains the declaration of the generic CMDXMLComposer class 
// which is responsible for creating an XML file
// from a given DOM structure.
// 
//

#include <f32file.h>
#include <utf.h>

#include <gmxmlconstants.h>
#include <gmxmlnode.h>
#include <gmxmldocument.h>
#include <gmxmlelement.h>
#include <gmxmlcomposer.h>
#include <gmxmlentityconverter.h>
#include <gmxmltext.h>
#include <gmxmlprocessinginstruction.h>
#include <gmxmlcomment.h>
#include <gmxmlcdatasection.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include "gmxmldummydtd.h"
#endif

_LIT(KXmlQuotation, "&quot;");

//
// CMDXMLComposer					//
//

CMDXMLComposer::CMDXMLComposer(MMDXMLComposerObserver* aComposerObserver) : CActive(EPriorityNormal)
//
// Constructor
//
	{
	iComposerObserver = aComposerObserver;
	iOutputProlog = ETrue;
	CActiveScheduler::Add(this);
	}
	
CMDXMLComposer::CMDXMLComposer(MMDXMLComposerObserver* aComposerObserver, TBool aOutputProlog) : CActive(EPriorityNormal),
	iOutputProlog(aOutputProlog)
//
// Constructor
//
	{
	iComposerObserver = aComposerObserver;
	CActiveScheduler::Add(this);
	}
//==================================================================================

EXPORT_C CMDXMLComposer::~CMDXMLComposer()
	{
	Cancel();
	delete iEntityConverter;
	delete iCharconv;

	iXMLFile.Close();
	}

//==================================================================================

void CMDXMLComposer::BaseConstructL()
	{
	iCharconv = CCnvCharacterSetConverter::NewL();
	iCharconv->SetDowngradeForExoticLineTerminatingCharacters(
		CCnvCharacterSetConverter::EDowngradeExoticLineTerminatingCharactersToJustLineFeed);
	}

//==================================================================================

EXPORT_C CMDXMLComposer* CMDXMLComposer::NewL(MMDXMLComposerObserver* aComposerObserver)
//
// Two phase static factory function constructor
// @param aEntityStrings the string table which lists the entity references and conversion
// @return Created CMDXMLComposer
// @leave can Leave due to OOM
//
	{
	CMDXMLComposer* self = NewLC(aComposerObserver);
	CleanupStack::Pop(self);
	return self;
	}
	
//==================================================================================

EXPORT_C CMDXMLComposer* CMDXMLComposer::NewL(MMDXMLComposerObserver* aComposerObserver, TBool aOutputProlog)
//
// Two phase static factory function constructor
// @param aEntityStrings the string table which lists the entity references and conversion
// @param aOutputProlog Whether the Doctype and Version tags should be output.  This is
// provided for MMS conformance.
// @return Created CMDXMLComposer
// @leave can Leave due to OOM
//
	{
	CMDXMLComposer* self = NewLC(aComposerObserver, aOutputProlog);
	CleanupStack::Pop(self);
	return self;
	}
	
//==================================================================================

EXPORT_C CMDXMLComposer* CMDXMLComposer::NewLC(MMDXMLComposerObserver* aComposerObserver)
//
// Two phase static factory function constructor
// @param aEntityStrings the string table which lists the entity references and conversion
// @return Created CMDXMLComposer
// @leave can Leave due to OOM
//
	{
	CMDXMLComposer* self = new (ELeave) CMDXMLComposer(aComposerObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
//==================================================================================

EXPORT_C CMDXMLComposer* CMDXMLComposer::NewLC(MMDXMLComposerObserver* aComposerObserver, TBool aOutputProlog)
//
// Two phase static factory function constructor
// @param aEntityStrings the string table which lists the entity references and conversion
// @param aOutputProlog Whether the Doctype and Version tags should be output.  This is
// provided for MMS conformance.
// @return Created CMDXMLComposer
// @leave can Leave due to OOM
//
	{
	CMDXMLComposer* self = new (ELeave) CMDXMLComposer(aComposerObserver, aOutputProlog);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
//==================================================================================

void CMDXMLComposer::ConstructL()
//
// Second stage constructor
// @param aEntityStrings the string table which lists the entity references and conversion
//
	{
	BaseConstructL();
	CMDXMLEntityConverter* entityConverter = new(ELeave) CMDXMLEntityConverter();
	SetEntityConverter(entityConverter);
	}


//==================================================================================

void CMDXMLComposer::InitialiseCompose(CMDXMLDocument* aDocument, TXMLFileType aFileType)
	{
	Cancel();
	
	iError = KErrNone;
	iSeverity = EXMLNone;
	iOutputBuffer.Zero();

	iXMLDoc = aDocument;
	iFileType = aFileType;

	iOnlyCalculatingSize = EFalse;
	iSizeTally = 0;
	}

EXPORT_C TInt CMDXMLComposer::ComposeFile(RFs aRFs, const TDesC& aFileToCompose, CMDXMLDocument* aDocument, TXMLFileType aFileType)
/** Starts file composition.

This function can not be called if there is an outstanding size calculation or compose
operation in progress. If it is necessary to run two asynchronous operations in parallel
then two instances of the CMDXMLComposer are needed.

@param aRFS File system to use
@param aFileToCompose Name of file to create
@param aDocument The document object to compose to the file
@return KERRNone if successful
**/
	{
	InitialiseCompose(aDocument, aFileType);
	iRFs = aRFs;
	TInt error = KErrNone;

	if(iXMLDoc == NULL)
		{
		error = KErrNotSupported;
		}
	else
		{
		switch(iFileType)
			{
			case EAscii:
				{
				error = iXMLFile.Replace(aRFs, aFileToCompose, EFileWrite | EFileStream);
				break;
				}
			case EUnicode:
				error = iXMLFile.Replace(aRFs, aFileToCompose, EFileWrite | EFileStream);
				if(error == KErrNone)
					{
					// append Unicode File identifier to start of output text
					iOutputBuffer.Append(CEditableText::EByteOrderMark);
					}
				break;
			case EUtf8:
				{
				error = iXMLFile.Replace(aRFs, aFileToCompose, EFileWrite | EFileStream);
				break;
				}
			default:
				error = KErrNotSupported;
				break;
			}
		}

	if(error == KErrNone)
		{
		SetActive();
		TRequestStatus* s=&iStatus;
		User::RequestComplete(s, KErrNone);
		}
	else
		{
		SetError(error, EXMLFatal);
		}

	return error;
	}
	
/** Starts file composition to an open file handle.

This function must not be called when file sizing is in progress. If it is necessary to calulate
the size and generate the XML simultaneously then two instances of the composer should be used,
one for sizing and one for composition.

@param aFileHandleToCompose An open file handle to write to. Ownership of the file handle is passed even if an error occurs.
@param aDocument The document object to compose to the file.
@param aFileType Type of the output file.
@return KErrNone if successful.
*/
EXPORT_C TInt CMDXMLComposer::ComposeFile(RFile& aFileHandleToCompose, CMDXMLDocument* aDocument, TXMLFileType aFileType)
	{
	InitialiseCompose(aDocument, aFileType);
	iXMLFile = aFileHandleToCompose;
	TInt error = KErrNone;

	if(iXMLDoc == NULL)
		{
		error = KErrNotSupported;
		}
	else
		{
		switch(iFileType)
			{
			case EUnicode:
				{
				// append Unicode File identifier to start of output text
				iOutputBuffer.Append(CEditableText::EByteOrderMark);
				}
			case EAscii:
			case EUtf8:
				break;
			
			default:
				error = KErrNotSupported;
				break;
			}
		}
	
	if(error == KErrNone)
		{
		iStatus = KRequestPending;
		SetActive();
		TRequestStatus* s=&iStatus;
		User::RequestComplete(s, KErrNone);
		}
	else
		{
		iXMLFile.Close();
		SetError(error, EXMLFatal);
		}

	return error;
	}

//==================================================================================

/** Starts calculating the size of the XML output without actually writing it to the file.

This process is asyncronous, the size value is only updated when ComposeFileComplete
is called on the MMDXMLComposerObserver passed in in the NewL.

This function can not be called if there is an outstanding size calculation or compose
operation in progress. If it is necessary to run two asynchronous operations in parallel
then two instances of CMDXMLComposer are needed.

@param aSize Will be set to the size of the XML document when composition has completed.
@param aDocument The document object to size
@param aFileType Type of the output file, required because it will affect the size of the XML
@return KErrNone if successful */

EXPORT_C TInt CMDXMLComposer::CalculateFileSize(TInt& aSize, CMDXMLDocument* aDocument, TXMLFileType aFileType)
	{
	Cancel();
	
	iError = KErrNone;
	iSeverity = EXMLNone;
	iOutputBuffer.Zero();

	iXMLDoc = aDocument;
	iFileType = aFileType;
	TInt error = KErrNone;

	if (iFileType == EUnicode)
		{
		// The size tally must be incremented by two characters because the unicode byte marker
		// gets added in the ComposeFile function that does not get called when we are calculating
		// the size.
		iSizeTally = 2;
		}
	else
		{
		iSizeTally = 0;
		}

	iSize = &aSize;
	iOnlyCalculatingSize = ETrue;

	if(iXMLDoc == NULL)
		{
		error = KErrNotSupported;
		}

	if(error == KErrNone)
		{
		SetActive();
		TRequestStatus* s=&iStatus;
		User::RequestComplete(s, KErrNone);
		}
	else
		{
		SetError(error, EXMLFatal);
		}

	return error;
	}

//==================================================================================

EXPORT_C TInt CMDXMLComposer::Error() const
	{
	return iError;
	}

//==================================================================================

EXPORT_C TXMLErrorCodeSeverity CMDXMLComposer::ErrorSeverity() const
	{
	return iSeverity;
	}

//==================================================================================
	
EXPORT_C CMDXMLEntityConverter* CMDXMLComposer::EntityConverter() const
	{
	return iEntityConverter;
	}

//==================================================================================

TInt CMDXMLComposer::OutputCommentL(const TDesC& aComment)
//
// Output a comment
// @param aComment the comment text to output
// @return KERRNone if successful, otherwise a file writing error.
//
	{
	TInt error = KErrNone;

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KNewLine);
		}

#ifdef _DEBUG
	for(TInt loopIndex = 0; loopIndex < iIndentationLevel; loopIndex++)
		{
		if(iSeverity != EXMLFatal)
			{
			error = WriteFileL(KTab);
			}
		}
#endif

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KXMLStartComment);
		}

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(aComment);
		}

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KXMLEndComment);
		}

	return error;
	}

//==================================================================================

TInt CMDXMLComposer::OutputProcessingInstructionL(const TDesC& aInstruction)
//
// Output a Processing Instruction
// @param aInstruction the Processing Instruction text to output
// @return KERRNone if successful, otherwise a file writing error.
//
	{
	TInt error = KErrNone;

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KNewLine);
		}

#ifdef _DEBUG
	for(TInt loopIndex = 0; loopIndex < iIndentationLevel; loopIndex++)
		{
		if(iSeverity != EXMLFatal)
			{
			error = WriteFileL(KTab);
			}
		}
#endif

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KXMLStartProcessingInstruction);
		}

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(aInstruction);
		}

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KXMLEndProcessingInstruction);
		}

	return error;
	}


//==================================================================================

TInt CMDXMLComposer::OutputCDataSectionL(const TDesC& aCDataSection)
//
// Output a CDataSection
// @param aCDataSection the data section text to output
// @return KERRNone if successful, otherwise a file writing error.
//
	{
	TInt error = KErrNone;
	
#ifdef _DEBUG
	for(TInt loopIndex = 0; loopIndex < iIndentationLevel; loopIndex++)
		{
		if(iSeverity != EXMLFatal)
			{
			error = WriteFileL(KTab);
			}
		}
#endif

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KXMLStartCDataSection);
		}

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(aCDataSection);
		}

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KXMLEndCDataSection);
		}

	return error;
	}

//==================================================================================
//Defect fix for INC036136- Enable the use of custom entity converters in GMXML

EXPORT_C TInt CMDXMLComposer::OutputDataL(const TDesC& aData)
//
// Output raw data
// it's only intended to be used from within a custom entity converter as
// it relies on a Composer sesssion already being in progress
// @param the data to be output
// @return KERRNone if successful, otherwise a file writing error.
//
	{
	TInt error = KErrNone;

	if(iSeverity != EXMLFatal)
		{
		if(aData.Find(KQuotation) != KErrNotFound)
			{
			error = ReplaceXmlCharactersL(aData, KQuotation);
			}
		else
			{
			error = WriteFileL(aData);	
			}		
		}

	return error;
	}

//==================================================================================
/* Before writing to xml file, search for special character like quotation (").
   if it exist then replace it with &quot; and then write it to xml file.
   Refer section 5 of below URL for more information.
   http://www.xmlnews.org/docs/xml-basics.html
   @param aXmlData the data to be output
   @param aString the special character to search in aXmlData
   @return KERRNone if successful, otherwise a file writing error.	*/
   
TInt CMDXMLComposer::ReplaceXmlCharactersL(const TDesC16& aXmlData, const TDesC& aString)
	{
	TInt xmlDataIndex;
	const TDesC& quot = KXmlQuotation;
	HBufC16* xmlData = aXmlData.AllocL();
				
	while((xmlDataIndex = (xmlData->Des()).Find(aString)) != KErrNotFound)
		{
		HBufC16* temp = HBufC::NewLC(((xmlData->Des()).Length() - 1) + quot.Length());
		TPtr16 tempPtr = temp->Des();
		tempPtr.Append((xmlData->Des()).Left(xmlDataIndex));
		tempPtr.Append(quot);
		tempPtr.Append((xmlData->Des()).Right((xmlData->Des()).Length() - (xmlDataIndex + 1)));
		delete xmlData;
		xmlData = NULL;
		xmlData = tempPtr.AllocL();
		CleanupStack::PopAndDestroy();	// temp
		}
	TInt error = WriteFileL((xmlData->Des()));
	delete xmlData;
	xmlData = NULL;
	return error;
	}

//==================================================================================

EXPORT_C void CMDXMLComposer::SetEntityConverter(CMDXMLEntityConverter* aEntityConverter)
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

EXPORT_C TInt CMDXMLComposer::OutputStartOfElementTagL(const TDesC& aElementName)
//
// Output a start of element tag
// @param aElementName the name of the tag to output
// @return KERRNone if successful, otherwise a file writing error.
//
	{
	TInt error = KErrNone;

#ifdef _DEBUG
	for(TInt loopIndex = 0; loopIndex < iIndentationLevel; loopIndex++)
		{
		if(iSeverity != EXMLFatal)
			{
			error = WriteFileL(KTab);
			}
		}
	iIndentationLevel++;
#endif

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KXMLStartTag);
		}

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(aElementName);
		}

	return error;
	}

//==================================================================================

EXPORT_C TInt CMDXMLComposer::OutputEndOfElementTagL(const TBool aHasChildren)
//
// Output an end of element start tag
// @param aHasChildren true if the element has children
// @return KERRNone if successful, otherwise a file writing error.
//
	{
	TInt error = KErrNone;

	if(iSeverity != EXMLFatal)
		{
		if(aHasChildren)
			{
			error = WriteFileL(KXMLEndTag);
			}
		else
			{
			error = WriteFileL(KXMLEndStartTag);

	#ifdef _DEBUG
			iIndentationLevel--;
	#endif
			}
		}

	return error;
	}

//==================================================================================

TInt CMDXMLComposer::OutputEndTagL(const TDesC& aElementName)
//
// Output an end of element tag
// @param aElementName the name of the tag to output
// @return KERRNone if successful, otherwise a file writing error.
//
	{
	TInt error = KErrNone;

#ifdef _DEBUG
	iIndentationLevel--;
	for(TInt loopIndex = 0; loopIndex < iIndentationLevel; loopIndex++)
		{
		if(iSeverity != EXMLFatal)
			{
			error = WriteFileL(KTab);
			}
		}
#endif
	
	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KXMLStartEndTag);
		}

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(aElementName);
		}

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KXMLEndTag);
		}

	return error;
	}





//==================================================================================

EXPORT_C TInt CMDXMLComposer::OutputAttributeL(const TDesC& aAttributeName, const TDesC& aAttributeValue)
//
// Output an attribute - name and value.
// @param aAttributeName the name of the attribute to output
// @param aAttributeValue the text of the attribute value to output
// @return KERRNone if successful, otherwise a file writing error.
//
	{
	TInt error = KErrNone;

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KSingleSpace);
		}

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(aAttributeName);
		}

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KEqualSign);
		}

	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KQuotation);
		}

	if(iSeverity != EXMLFatal)
		{
		// Work along the attribute value in sections.  We have two markers in the attribute,
		// one at the beginning of the section we're working on and one at the end.  Initially
		// beginSection is the start of the string, 0, the end of the section is the first
		// CDataSection we find.
		// beginSection is an offset within the string, endSection is an offset from that
		TInt beginSection = 0;
		TInt endSection = aAttributeValue.Find(KXMLStartCDataSection);
		
		// We've found at least one CDataSection
		while(endSection != KErrNotFound)
			{
			// Entity convert this plain text section
			TPtrC plainText = aAttributeValue.Mid(beginSection, endSection);
			error = iEntityConverter->OutputComposedTextL(this, plainText);

			// Move on our markers.  We start the new section at the end of the old one.
			beginSection += endSection;
			// The end of this section is the end of the CDataSection
			endSection = TPtrC(aAttributeValue.Mid(beginSection)).Find(KXMLEndCDataSection);
			if(endSection != KErrNotFound)
				{
				// The CDataSection ends at the beginning of the end tag, so we need to add
				// on the length of the end tag before outputting it without conversion
				endSection += TPtrC(KXMLEndCDataSection).Length();
				OutputDataL(aAttributeValue.Mid(beginSection, endSection));

				// Now move on our markers again.  Start at the end of the CDataSection, and
				// continue to the beginning of the next one.
				beginSection += endSection;
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
			error = iEntityConverter->OutputComposedTextL(this, aAttributeValue.Mid(beginSection));
			}
		}
	
	if(iSeverity != EXMLFatal)
		{
		error = WriteFileL(KQuotation);
		}

	return error;
	}

//==================================================================================

EXPORT_C TInt CMDXMLComposer::RunError(TInt aError)
//
// RunError function inherited from CActive base class - intercepts any Leave from
// the RunL() function, sets an appropriate errorcode and calls ComposeFileCompleteL
//
	{
	iSeverity = EXMLFatal;
	iError = aError;
	iXMLFile.Close();
	TRAPD(err, iComposerObserver->ComposeFileCompleteL());
	return err;
	}

//==================================================================================

EXPORT_C void CMDXMLComposer::DoCancel()
//
// DoCancel function inherited from CActive base class
//
	{
	iXMLFile.Close();
	}

//==================================================================================

void CMDXMLComposer::RunL()
//
// RunL function inherited from CActive base class - does the actual composition
// @leave can Leave due to OOM
//
	{
	// If this is an ASCII file then check that we can load the character set converter
	if(iSeverity != EXMLFatal)
		{
		if(iFileType == EAscii)
			{
			if (iCharconv->PrepareToConvertToOrFromL(KCharacterSetIdentifierAscii, iRFs)!=
				CCnvCharacterSetConverter::EAvailable)
				{
				SetError( KErrNotSupported, EXMLFatal);
				}
			}
		}

	if(iSeverity != EXMLFatal)
		{
	#ifdef _DEBUG
		iIndentationLevel = 0;
	#endif
		
		CMDXMLElement* parentElement = iXMLDoc->DocumentElement();

		TInt error = KErrNone;

		if(!parentElement->CheckChildren())
			{
			error = KErrNotSupported;
			SetError(KErrXMLInvalidChild, EXMLWorkable);
			}
		
		if(iOutputProlog)
			{
			// Output document level information if required.  MMS conformance requires no Version
			// or DocType tag, so we may not want to output them.
			if(iSeverity != EXMLFatal)
				{
				WriteFileL(iXMLDoc->VersionTag());
				}
			if(iSeverity != EXMLFatal)
				{
				WriteFileL(KNewLine);
				}
			if(iSeverity != EXMLFatal)
				{
				WriteFileL(iXMLDoc->DocTypeTag());
				}
			if(iSeverity != EXMLFatal)
				{
				WriteFileL(KNewLine);
				}
			}
			
		// Output the contents of the DOM tree
		if(iSeverity != EXMLFatal)
			{
			error = ComposeL();
			if(iSeverity != EXMLFatal)
				{
				if(error != KErrNone)
					{
					SetError(error, EXMLIndeterminate);
					}

				WriteFileL(KNewLine);
				}
			}

		if(iSeverity != EXMLFatal)
			{
			error = FlushOutputBufferL();

			if(error != KErrNone)
				{
				SetError(error, EXMLIndeterminate);
				}
			}
		}

	iXMLFile.Close();

	if (iOnlyCalculatingSize)
		{
		*iSize = iSizeTally;
		}

	iComposerObserver->ComposeFileCompleteL();
	}


TInt CMDXMLComposer::ComposeL()
//
// Function to output the contents of the node as XML to some output stream.
// @param aComposer The composer to use - provides access to output and entity conversion
// @return Returns KerrNone if successful or a file write error
// @leave Can Leave due to OOM.
//
	{
	CMDXMLNode* nextPtr;
	TInt error = KErrNone;
	nextPtr = iXMLDoc->DocumentElement()->FirstChild();


	while(error == KErrNone && nextPtr != NULL)
		{
		// compose the start tag of the current element
		error = ComposeStartTagL(*nextPtr);

		if(error == KErrNone)
			{
			// move to the first child is there is one
			if(nextPtr->FirstChild() != NULL)
				{
				nextPtr = nextPtr->FirstChild();
				}
			else
				{
				// EndTag is only output if HasChildNodes() == TRUE
				error = ComposeEndTagL(*nextPtr);

				// move to the next sibling if exists
				if(nextPtr->NextSibling() != NULL)
					{
					nextPtr = nextPtr->NextSibling();
					}

				// no siblings, move back to parent and close parent tag
				else
					{
					// might need to go up several layers so WHILE rather than IF
					while(nextPtr != NULL && nextPtr->NextSibling() == NULL && error == KErrNone)
						{
						nextPtr = nextPtr->ParentNode();
						if(nextPtr != NULL)
							{
							error = ComposeEndTagL(*nextPtr);
							}
						}

					// if we've stopped at an element with further siblings
					if(nextPtr != NULL && nextPtr->NextSibling() != NULL)
						{
						nextPtr = nextPtr->NextSibling();
						}
					}
				}
			}
		}

	return error;
	}


EXPORT_C TInt CMDXMLComposer::ComposeStartTagL(CMDXMLNode& aNode)
 //
 // Outputs a start tag for the node which includes the
 // tag name and all attribute name value pairs currently
 // specified.  If the node is an empty node then it
 // makes the tag an empty node tag, otherwise it creates
 // a start tag.
 // @param aNode The Node for which the start tag is being written
 // @return Returns KerrNone if successful or a file write error
 // 
	{
	TInt error = KErrNone;

	switch (aNode.NodeType())
		{
		case CMDXMLNode::ETextNode:
			error = iEntityConverter->OutputComposedTextL(this, ((CMDXMLText&)aNode).Data());
			break;
		case CMDXMLNode::ECDATASectionNode:
			error = OutputCDataSectionL(((CMDXMLCDATASection&)aNode).Data());
			// does nothing
			break;
		case CMDXMLNode::EProcessingInstructionNode:
			error = OutputProcessingInstructionL(((CMDXMLProcessingInstruction&)aNode).Data());
			break;
		case CMDXMLNode::ECommentNode:
			error = OutputCommentL(((CMDXMLComment&)aNode).Data());
			break;
		case CMDXMLNode::EDocumentNode:
			// does nothing
			break;
		case CMDXMLNode::EElementNode:
			error = OutputStartOfElementTagL(aNode.NodeName());
			TInt attIndex;
			// Output all attributes in a loop
			for(attIndex = 0 ; (error == KErrNone && attIndex < ((CMDXMLElement&)aNode).NumAttributes()); attIndex ++ )
				{
				TPtrC attributeValue;	
				TPtrC attributeName;
				error = ((CMDXMLElement&)aNode).AttributeDetails(attIndex, attributeName, attributeValue);
				if(error == KErrNone)
					error = OutputAttributeL(attributeName, attributeValue);
				}


			if( error == KErrNone )
				{
				error = OutputEndOfElementTagL(aNode.HasChildNodes());
				}
			break;
		default:
			// does nothing
			break;


		}

	return error;
	}

EXPORT_C TInt CMDXMLComposer::ComposeEndTagL(CMDXMLNode& aNode)
//
// Outputs an end tag for the node.
// @param aNode the node for which the tag is being written.
// @return Returns KerrNone if successful or a file write error
//
	{
	TInt error = KErrNone;

	if (aNode.NodeType() == CMDXMLNode::EElementNode && aNode.NodeName() != KXMLDocumentElementNodeName)
		{
		if(aNode.HasChildNodes())
			{
			error = OutputEndTagL(aNode.NodeName());
			}
		}
		

	return error;
	}

//==================================================================================

TInt CMDXMLComposer::WriteFileL(const TDesC& aStringToWrite)
//
// Function to write string to required file format - handles format conversion
// @param aStringToWrite the string to output
// @return returns KERRNone if successful or a file write error.
//
	{
	TInt error = KErrNone;
	TInt outputBufferLength = iOutputBuffer.Length();
	TInt additionalLength = aStringToWrite.Length();

	if(outputBufferLength + additionalLength <= KWriteBufferLen)
		{
		iOutputBuffer.Append(aStringToWrite);
		}
	else
		{
		iOutputBuffer.Append(aStringToWrite.Left(KWriteBufferLen - outputBufferLength));
		error = FlushOutputBufferL();
		iOutputBuffer.Zero();
		WriteFileL(aStringToWrite.Right(additionalLength - (KWriteBufferLen - outputBufferLength)));
		}		

	return error;
	}

//==================================================================================
#define DES_AS_8_BIT(str) (TPtrC8((TText8*)((str).Ptr()), (str).Size()))

TInt CMDXMLComposer::FlushOutputBufferL()
//
// Function to write string to required file format - handles format conversion
// @param aStringToWrite the string to output
// @return returns KERRNone if successful or a file write or conversion error.
//
	{
	TInt error = KErrNone;

	if (!iOnlyCalculatingSize)
		{
		if(iXMLFile.SubSessionHandle() == KNullHandle)
			{
			return KErrBadHandle;
			}
		}

	// All text is processed internally as unicode.  If we've been asked to output
	// another format (Ascii or Utf-8) we need to translate it to that format using
	// charconv before writing the file.  
	switch(iFileType)
		{
		case EAscii:
		case EUtf8:
			{
			// We need to convert in a loop as charconv only allows 25 untranslatable
			// characters per block of text.  convResult tells us how many characters
			// were left untranslated at the end of iOutputBuffer so we only look at
			// that many characters on the right hand of iOutputBuffer.  Initially we
			// don't have any characters translated so convResult is the length of
			// iOutputBuffer.  Each time through the loop write out the chunk we've
			// converted.  If there's an error writing the file or doing the character
			// conversion we give up.
			TInt convResult = iOutputBuffer.Length();
			while((convResult > 0) && (error == KErrNone))
				{
				HBufC8* narrow = HBufC8::NewLC(convResult);
				TPtr8 narrowPtr = narrow->Des();
				if(iFileType == EAscii)
					{
					convResult = iCharconv->ConvertFromUnicode(narrowPtr, iOutputBuffer.Right(convResult));
					}
				else // iFileType == EUtf8
					{
					convResult = CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowPtr, iOutputBuffer.Right(convResult));
					}

				if(convResult >= 0)
					{
					iSizeTally += narrow->Size();

					if (!iOnlyCalculatingSize)
						{
						error = iXMLFile.Write(narrow->Des());
						}
					}
				else
					{
					error = convResult;
					}
				CleanupStack::PopAndDestroy(narrow);
				} 
			break;
			}

		case EUnicode:
			{
			TPtrC  output16 = iOutputBuffer;
			TPtrC8 output = DES_AS_8_BIT(output16);
			
			iSizeTally += output.Size();

			if (!iOnlyCalculatingSize)
				{
				error = iXMLFile.Write(output);
				}

			break;
			}

		default:
			error = KErrNotSupported;
			break;
		}

	if(error != KErrNone)
		{
		SetError(error, EXMLFatal);
		}

	return error;	
	}

//==================================================================================

EXPORT_C void CMDXMLComposer::SetError(const TInt aErrorCode, const TXMLErrorCodeSeverity aSeverity)
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

EXPORT_C void CMDXMLComposer::PlaceholderForRemovedExport1(MMDXMLComposerObserver* /*aComposerObserver*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}

EXPORT_C void CMDXMLComposer::PlaceholderForRemovedExport2()
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}

EXPORT_C void CMDXMLComposer::PlaceholderForRemovedExport3()
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}

// End of File
