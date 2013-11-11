// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the declaration of the generic CMDXMLComposer class
// which is responsible for creating an XML file
// from a given DOM structure.
// 
//

/**
 @file
*/


#ifndef __GMXMLCOMPOSER_H__
#define __GMXMLCOMPOSER_H__

#include <e32base.h>
#include <f32file.h>
#include <gmxmlconstants.h>
#include <charconv.h>

//forward reference
class CMDXMLDocument;
class CMDXMLEntityConverter;
class CMDXMLNode;

class MMDXMLComposerObserver
/** Abstract observer interface for notification when XML composition is complete.

It should be implemented by users of CMDXMLComposer.

@publishedPartner
@released
*/
	{
public:
	/**
	 Call back function used to inform a client of the composer that the composer has completed.
	 */
	virtual void ComposeFileCompleteL() = 0;
	};


class CMDXMLComposer: public CActive
/** Creates an XML file from a given DOM structure.
	@publishedPartner
    @released
 */
	{
public:
	/** Destructor. */
	IMPORT_C ~CMDXMLComposer();

	/** Allocates and constructs a new XML composer.
	
	@param aComposerObserver Composer observer
	@leave KErrNoMemory Out of memory
	@return New XML composer */
	IMPORT_C static CMDXMLComposer* NewL(MMDXMLComposerObserver* aComposerObserver);
	
	/** Allocates and constructs a new XML composer.
	
	@param aComposerObserver Composer observer
	@param aOutputProlog Whether to output Version and Doctype tags.
	@leave KErrNoMemory Out of memory
	@return New XML composer */
	IMPORT_C static CMDXMLComposer* NewL(MMDXMLComposerObserver* aComposerObserver, TBool aOutputProlog);


	/** Allocates and constructs a new XML composer, leaving the object on the cleanup 
	stack.
	
	@leave KErrNoMemory Out of memory
	@param aComposerObserver Composer observer
	@return New XML composer */
	IMPORT_C static CMDXMLComposer* NewLC(MMDXMLComposerObserver* aComposerObserver);
	
	/** Allocates and constructs a new XML composer, leaving the object on the cleanup 
	stack.
	
	@leave KErrNoMemory Out of memory
	@param aComposerObserver Composer observer
	@param aOutputProlog Whether to output Version and Doctype tags.
	@return New XML composer */
	IMPORT_C static CMDXMLComposer* NewLC(MMDXMLComposerObserver* aComposerObserver, TBool aOutputProlog);

	/** Starts file composition.
	
	This function must not be called when file sizing is in progress. If it is necessary to calulate
	the size and generate the XML simultaneously then two instances of the composer should be used,
	one for sizing and one for composition.
  
	@param aRFs A file server session
	@param aFileToCompose Name of the file to create
	@param aDocument The document object to compose to the file
	@param aFileType Type of the output file
	@return KErrNone if successful */
	IMPORT_C TInt ComposeFile(RFs aRFs, const TDesC& aFileToCompose, CMDXMLDocument* aDocument, TXMLFileType aFileType);

	IMPORT_C TInt ComposeFile(RFile& aFileHandleToCompose, CMDXMLDocument* aDocument, TXMLFileType aFileType);

	/** Starts calculating the size of the XML output without actually writing it to the file.

	File size calculation is asyncronous, the size value is only updated when ComposeFileComplete
	is called on the MMDXMLComposerObserver passed in in the NewL.

	This function must not be called when file composition or another sizing operation is
	in progress. If it is necessary to calulate the size and generate the XML simultaneously
	then two instances of the composer should be used, one for sizing and one for composition.

	@param aSize Will be set to the size of the XML document when composition has completed.
	@param aDocument The document object to size
	@param aFileType Type of the output file, required because it will affect the size of the XML
	@return KErrNone if successful */

	IMPORT_C TInt CalculateFileSize(TInt& aSize, CMDXMLDocument* aDocument, TXMLFileType aFileType);

	/** Gets the composer's last error.
	
	@return Error code */
	IMPORT_C TInt Error() const;

	/**
	 Get the severity of the most severe error found.
	 @return the maximum error severity
	 */
	IMPORT_C TXMLErrorCodeSeverity ErrorSeverity() const; 

	//Defect fix for INC036136 - Enable the use of custom entity converters in GMXML
	/** Outputs raw data.
	    it's only intended to be used from within a custom entity converter as
        it relies on a Composer sesssion already being in progress
	
	   @param aData Data to output
	   @return KErrNone if successful, otherwise a file writing error. 
	 */
	IMPORT_C TInt OutputDataL(const TDesC& aData);

	/**
	 * Sets the entity converter to be used
	 * and  take ownership of the passed entity converter
	 * @param aEntityConverter The entity converter to be used
	 */
	IMPORT_C void SetEntityConverter(CMDXMLEntityConverter* aEntityConverter); 
	//End Defect fix for INC036136

public: // public functions used by other classes within the .dll, not for Export.
	/** Gets the entity converter used by the composer.
	
	@return The entity converter used by the composer. */
	IMPORT_C CMDXMLEntityConverter* EntityConverter() const;

	/** Outputs a comment.
	
	@param aComment Comment to output
	@return KErrNone if successful, otherwise a file writing error. */
	TInt OutputCommentL(const TDesC& aComment);

	/** Outputs a processing instruction.
	
	@param aInstruction Processing instruction text to output
	@return KErrNone if successful, otherwise a file writing error. */
	TInt OutputProcessingInstructionL(const TDesC& aInstruction);

	/** Outputs a CDATA section.
	
	@param aCDataSection CDATA section to output
	@return KErrNone if successful, otherwise a file writing error. */
	TInt OutputCDataSectionL(const TDesC& aCDataSection);


	/** Outputs a start of element tag.
	
	@param aElementName The name of the tag to output
	@return KErrNone if successful, otherwise a file writing error. */
	IMPORT_C TInt OutputStartOfElementTagL(const TDesC& aElementName);

	/** Outputs an end of element start tag (</).
	
	@param aHasChildren True if the element has children
	@return KErrNone if successful, otherwise a file writing error. */
	IMPORT_C TInt OutputEndOfElementTagL(const TBool aHasChildren);

	/** Output an end of element tag.
	
	@param aElementName The name of the tag to output
	@return KErrNone if successful, otherwise a file writing error. */
	TInt OutputEndTagL(const TDesC& aElementName);

	/** Outputs an attribute name and value. 
	
	@param aAttributeName Attribute name
	@param aAttributeValue Attribute value
	@return KErrNone if successful, otherwise a file writing error. */
	IMPORT_C TInt OutputAttributeL(const TDesC& aAttributeName, const TDesC& aAttributeValue);

private:
	/*
	 * BaseConstructL function to be called by dervived classes during their construction
	 */
	void BaseConstructL();

	/*
	 * RunError function inherited from CActive base class - intercepts any Leave from
	 * the RunL() function, sets an appropriate errorcode and calls ComposeFileCompleteL
	 */
	IMPORT_C TInt RunError(TInt aError);

	 /*
	 * DoCancel function inherited from CActive base class
	 */
	IMPORT_C virtual void DoCancel();

	/*
	 * RunL function inherited from CActive base class - does the actual composition
	 * @leave can Leave due to OOM
	 */
	virtual void RunL();

	/*
	 * Function to write string to required file format - handles format conversion
	 * @param aStringToWrite the string to output
	 * @return returns KERRNone if successful or a file write error.
	 */
	virtual TInt WriteFileL(const TDesC& aStringToWrite);

	/*
	 * Function to flush output string to required file format - handles format conversion
	 * @return returns KERRNone if successful or a file write error.
	 */
	virtual TInt FlushOutputBufferL();

	/*
	 * Constructor
	 */
	CMDXMLComposer(MMDXMLComposerObserver* aComposerObserver);

	/*
	 * Constructor
	 */
	CMDXMLComposer(MMDXMLComposerObserver* aComposerObserver, TBool aOutputProlog);


	/*
	 * Sets iError to new errorcode if more serious than any error so far encountered
	 */
	IMPORT_C void SetError(const TInt aErrorCode, const TXMLErrorCodeSeverity aSeverity);


	/*
     * Outputs a start tag for the node which includes the
     * tag name and all attribute name value pairs currently
     * specified.  If the node is an empty node then it
     * makes the tag an empty node tag, otherwise it creates
     * a start tag.
     * @param aNode The Node for which the start tag is being written
     * @return Returns KerrNone if successful or a file write error
     */
	IMPORT_C TInt ComposeStartTagL(CMDXMLNode& aNode);

	/*
     * Outputs an end tag for the node.
     * @param aNode the node for which the tag is being written.
     * @return Returns KerrNone if successful or a file write error
     */


	IMPORT_C TInt ComposeEndTagL(CMDXMLNode& aNode);

	/*
	 * Second stage constructor
	 * @param aEntityStrings the string table which lists the entity references and conversion
	 */
	void ConstructL();
	TInt ComposeL();
	
	void InitialiseCompose(CMDXMLDocument* aDocument, TXMLFileType aFileType);

	IMPORT_C void PlaceholderForRemovedExport1(MMDXMLComposerObserver* aComposerObserver);
	IMPORT_C void PlaceholderForRemovedExport2();
	IMPORT_C void PlaceholderForRemovedExport3();
	TInt ReplaceXmlCharactersL(const TDesC16& aXmlData, const TDesC& aString);

private:
	MMDXMLComposerObserver* iComposerObserver;
	CMDXMLEntityConverter* iEntityConverter;	// Entity converter to use
	CMDXMLDocument* iXMLDoc;					// XML document being composed
	RFile iXMLFile;								// File being composed
	TXMLFileType iFileType;						// Type of file being composed
	TInt iError;								// Current error
	TXMLErrorCodeSeverity iSeverity;			// ErrorCode severity
	CCnvCharacterSetConverter* iCharconv;
	TBuf<KWriteBufferLen> iOutputBuffer;
	RFs iRFs;									// File system to use
	TBool iOutputProlog;							// Whether to output Version and Doctype tags

	TInt iSizeTally;
	TInt* iSize;
	TBool iOnlyCalculatingSize;

#ifdef _DEBUG
	TInt iIndentationLevel;
#endif
	};

#endif
