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
// This file contains the declaration of the generic CMDXMLParser class
// which is responsible for creating a DOM structure
// from a given XML file.
// 
//

/**
 @file
*/

#ifndef __GMXMLPARSER_H__
#define __GMXMLPARSER_H__

#include <e32std.h>
#include <txtetext.h>
#include <gmxmlconstants.h>
#include <f32file.h>

//forward reference
class CMDXMLDocument;
class CMDXMLEntityConverter;
class CMDXMLElement;
class MXMLDtd;



class MMDXMLParserObserver
/** Abstract observer interface for notification when XML parsing is complete.

It should be implemented by users of CMDXMLParser
@publishedAll 
@released*/
	{
public:
	/**
	Call back function used to inform a client of the Parser when a parsing operation completes.
	 */
	virtual void ParseFileCompleteL() = 0;
	};

class MMDXMLParserDataProvider
/** Abstract data source interface for XML data source.

The user of CMDXMLParser must build one of these to encapsulate the data source
that they wish to parse.  CMDXMLParser implements a file-based data source to
implement the functionality of the ParseFile function.

@publishedAll 
@released*/
	{
public:
	/** Status codes returned by GetData() implementations. */
	enum TDataProviderResults
		{
		KMoreData,		//< Returned by the interface implementation when it is returning more data.
		KDataStreamError,	//< Returned by the interface when an unrecoverable error prevents obtaining more data.  A recoverable error should be represented by KDataNotReady.
		KDataStreamEnd	//< Returned by the interface when there is no more data to come.
		};

public:
	/** 
	The XML Parser calls this on a specific data provider to get more data
	when required.

	Note that the TPtrC supplied may be used by the parser at any time
	between the return of this call and the next call that the parser
	makes out.

	Your data provider must not move the data pointed to until the
	parser has indicated that it's done with that block by asking for
	another.

	Ownership of the data pointed to remains with the data provider.


	General comments on efficiency
	------------------------------

	The parser is designed such that it processes the whole data block
	provided in one go.  It will automatically become asynchronous when
	another block is required - the data provider only needs to supply
	data.

	Because of this design, it allows the data provider to indirectly
	control the amount of processing time that will be needed
	in a single block.

	It is a good idea to balance the need for the fastest possible 
	processing with the need for client application responsiveness by
	ensuring that the amount of data passed in a single block is not 
	too large.	However, it is worth bearing in mind that the parser
	will convert UTF8 data streams in blocks of 32 characters, and
	supplying blocks of smaller length than this will result in a
	slight loss of efficiency.

	@param aPtr On return, the data provided
	@param aStatus Asynchronous status to be completed by the function with a 
	TDataProviderResults value
	*/
	virtual void GetData(TPtrC8 &aPtr, TRequestStatus &aStatus) = 0;
	/**
	Called to indicate that use of the data source is complete.
	*/
	virtual void Disconnect() = 0;
	};

class CMDXMLParserFileDataSource;

class CMDXMLParser: public CActive
/** Creates a DOM structure from a given XML file.

The parsing operation is asynchronous and is initiated by a call to ParseFile(). 
On completion, the created DOM document can be retrieved through DetachXMLDoc().

Note the following ownership rules for the DOM document:

1. calling DetachXMLDoc() transfers ownership of the document to the client

2. if the parser is asked to parse a new file while it still owns an existing 
DOM document, it will delete the old document.

@publishedAll
@released
*/
	{
public:
	/** Allocates and constructs a new XML parser, specifying a DTD.
	
	@param aParserObserver XML parser observer
	@leave KErrNoMemory Out of memory
	@return New XML parser */
	IMPORT_C static CMDXMLParser* NewL(MMDXMLParserObserver* aParserObserver);

	/** Allocates and constructs a new XML parser, specifying a DTD.
	
	@param aParserObserver XML parser observer
	@param aDtdRepresentation DTD validator
	@leave KErrNoMemory Out of memory
	@return New XML parser */
	IMPORT_C static CMDXMLParser* NewL(MMDXMLParserObserver* aParserObserver, MXMLDtd* aDtdRepresentation);

	/** Allocates and constructs a new XML parser, leaving the object on the cleanup 
	stack.
	
	@param aParserObserver XML parser observer
	@leave KErrNoMemory Out of memory
	@return New XML parser */
	IMPORT_C static CMDXMLParser* NewLC(MMDXMLParserObserver* aParserObserver);

	/** Allocates and constructs a new XML parser, leaving the object on the cleanup 
	stack.
	
	@param aParserObserver XML parser observer
	@param aDtdRepresentation DTD validator
	@leave KErrNoMemory Out of memory
	@return New XML parser */
	IMPORT_C static CMDXMLParser* NewLC(MMDXMLParserObserver* aParserObserver, MXMLDtd* aDtdRepresentation);


	/** Destructor. */
	IMPORT_C ~CMDXMLParser();

	/** Gets the last error found by the parser.
	
	@return Error code
	 */
	IMPORT_C TInt Error() const;

	/**
	 Get the severity of the most severe error found.
	 @return the maximum error severity
	 */
	IMPORT_C TXMLErrorCodeSeverity ErrorSeverity() const; 

	/** Gets the created DOM.
	
	This should be called after the conclusion of the parser process.
	
	Note that the function sets the internal variable pointing to the document 
	to NULL, so this function can only be called once per file parse. The caller 
	takes ownership of the document, and must delete it when its use is complete.
	
	@return The created DOM */
	IMPORT_C CMDXMLDocument* DetachXMLDoc();

	/** Parses a specified XML file into a DOM object tree.
	
	@param aRFs File server session
	@param aFileToParse The file name to parse
	@return KErrNone if success or a file read error code */
	IMPORT_C TInt ParseFile(RFs aRFs, const TDesC& aFileToParse);
	
	IMPORT_C TInt ParseFile(RFile& aFileHandleToParse);

	/** Parses a specified XML Data Source into a DOM object tree.
	Use ParseSourceL() function in preference to ParseSource()
	@param aSource MMDXMLParserDataProvider pointer 
	*/
	inline void ParseSource(MMDXMLParserDataProvider *aSource)
		{
		TRAP_IGNORE(ParseSourceL(aSource));
		} 
				
	/** Parses a specified XML Data Source into a DOM object tree.	
	@param aSource MMDXMLParserDataProvider pointer 
	*/
	IMPORT_C void ParseSourceL(MMDXMLParserDataProvider *aSource);

	/** Defines input stream character widths. */
	enum TMDXMLParserInputCharWidth
		{
		EAscii = 0x01, //< ASCII
		EUnicode = 0x02 //<Unicode
		};
	
	/** Sets the input stream character width.
	 *
	 * @param aWidth Character width for incoming stream.  Possible values are EAscii and EUnicode (representing Ascii/UTF8 and Unicode respectively).
	 *
	 */
	IMPORT_C void SetSourceCharacterWidth(TMDXMLParserInputCharWidth aWidth);

	//Defect fix for INC036136- Enable the use of custom entity converters in GMXML
	/**
	 * Sets the entity converter to be used for parsing.
	 * and  take ownership of the passed entity converter
	 * @param aEntityConverter the entity converter to be used.
	 */
	IMPORT_C void SetEntityConverter(CMDXMLEntityConverter* aEntityConverter);
	//End Defect fix for INC036136

	/**
	 Controls whether invalid elements and attributes are added to the DOM.
	 @param aStoreInvalid ETrue if invalid content should be stored, EFalse otherwise.
	 */
	IMPORT_C void SetStoreInvalid(TBool aStoreInvalid);
	
	/**
	 Controls whether whitespaces are handled by XML parser or by client.
	 @param aPreserve ETrue if all whitespaces should be preserved (handled by client), EFalse otherwise.
	 */
	IMPORT_C void SetWhiteSpaceHandlingMode(TBool aPreserve);

public: // public functions used by other classes within the .dll, not for Export.
	/** Gets the entity converter.
	
	@return The entity converter */
	CMDXMLEntityConverter* EntityConverter();

private:
	IMPORT_C virtual void DoCancel();

	/*
	 * RunL function inherited from CActive base class - carries out the actual parsing.
	 * @leave can Leave due to OOM
	 */
	virtual void RunL();

	/*
	 * Helper function that does the parsing - called from inside RunL
	 */
	TBool DoParseLoopL();

	/*
	 * RunError function inherited from CActive base class - intercepts any Leave from
	 * the RunL() function, sets an appropriate errorcode and calls ParseFileCompleteL
	 */
	IMPORT_C TInt RunError(TInt aError);

	/*
	 * Constructors
	 */
	CMDXMLParser(MMDXMLParserObserver* aParserObserver);

	CMDXMLParser(MMDXMLParserObserver* aParserObserver, MXMLDtd* aDtdRepresentation);

	/*
	 * Called when a character is read in and found to bo outside of an element tag
	 */
	virtual void HandleTextL(TDes& aChar);

	enum TGetCharReturn
		{
		KError = 0x00,			// GetChar detected an error
		KCharReturned,	// GetChar returned a character
		KWaitForChar	// GetChar couldn't return a character this time, but might next time.
		};

	/*
	 * Fetch one character from the input file
	 * @param aChar the returned character.
	 * @return returns one of the values of TCharReturn
	 */
	TGetCharReturn GetChar(TDes& aChar);

	/* utility functions, called from GetChar to deal with the
	 * 2 types of input stream
	 */
	TGetCharReturn GetDoubleByteChar(TDes& aChar);
	TGetCharReturn GetSingleByteChar(TDes& aChar);

	/*
	 * Fetch some more data from the data provider
	 * @return returns one of the values of TCharReturn
	 */
	void GetMoreData();

	/*
	 * @return Returns true if the current tag is a doctype tag and sets the
	 * Document DocType member accordingly on the first pass of this function.
	 */
	TBool DocTypeL();

	/*
	 * creates a new processing instruction if necessary and adds to document
	 * @return Returns true if the current tag is a processing instruction
	 */
	TBool ProcessingInstructionL(CMDXMLElement* aParentElement);

	/*
	 * creates a new CDataSection if necessary and adds to document
	 * @return Returns true if the current tag is a processing instruction
	 */
	TBool CDataSectionL(CMDXMLElement* aParentElement);
	TBool EndOfCDataSection();

	/*
	 * @return returns true if the current tag is a version id tag and sets the
	 * Document Version member accordingly on the first pass of this function.
	 */
	TBool VersionIDL();

	/*
	 * creates a new comment if necessary and adds to document
	 * @return returns true if the current tag is a comment tag
	 */
	TBool CommentL(CMDXMLElement* aParentElement);

	/*
	 * Parse a start of element tag and create an element with attributes set.
	 * @return Returns a pointer to the created element
	 * @leave can Leave due to OOM
	 */
	virtual CMDXMLElement* ParseStartTagL();

	/*
	 * Detects the type of a file - can be Unicode or UTF-8
	 */
	TBool DetectFileType();

	/*
	 * Creates a generic or DTD-specific document object
	 * @leave can Leave due to OOM
	 */
	virtual void CreateDocumentL();

	/*
	 * Sets iError to new errorcode if more serious than any error so far encountered
	 */
	IMPORT_C void SetError(const TInt aErrorCode, const TXMLErrorCodeSeverity aSeverity);

	/*
	 * This function is used to parse the attributes.
     * @param aElement The element to which the attributes belong
     * @param aTagToParse The tag to be parsed
     * @return Returns KErrNone if both attribute name & value are valid 
	 * KErrXMLBadAttributeName if attribute name is invalid or KErrXMLBadAttributeValue is invalid
     * @leave can Leave due to OOM
	 */
	TInt ParseElementAttributesL(CMDXMLElement& aElement, TDes& aTagToParse);

	/** 
	  This function locates the next attribute in the tag.
	  @param aTagToParse the tag to find the attribute in
	  @return the offset of the next attribute
	 */
	TInt LocateNextAttribute(const TDesC& aTagToParse);

    /*
     * Parses an end tag.  In fact, at this point the end tag must match
     * the tag name of the start tag.  
     * @param aTagToParse Text of the end tag.
     * @return Returns KErrNone if the end tag matches the start tag or KErrNotFound if there is a mismatch.
     */
	TInt ParseElementEndTag(CMDXMLElement& aElement, const TDesC& aTagToParse);

	TInt CheckForStartCData(const TDesC& aTextToCheck);
	TInt FindDelimiter(TDesC& aDataToSearch, TDesC& aDelimiterToFind);

	/*
	 * Second stage constructor
	 */
	void ConstructL(MXMLDtd* aDtdRepresentation);
	void AddTextL(CMDXMLElement* aParentElement);

	/*
	 * Checks whether the end of this tag is in a CDataSection.
	 * @param aDataToSearch The data to check
	 * @return Returns ETrue if the tag contains an unclosed CDataSection
	 */
	TBool InCDataSection(TDesC& aDataToSearch);

	/*
	 * Entity converts the sections of one attribute value that are not within a CDataSection.
	 * @param aAttributeValue one attribute value
	 * @return Returns an error if entity conversion did not successfully complete, otherwise KErrNone
	 */
	TInt ParseSingleAttributeL(TDes& aAttributeValue);

	/*
	 * Prepares this class for use on another file.
	 *
	 */
	void PrepareForReuseL();

	/**
	 This should be called when parsing has been completed, before calling ParseFileCompleteL().
	 It checks for errors that can only be determined at the end of parsing, eg missing doctype or 
	 incomplete content.
	 */
	void CheckForErrors();

	IMPORT_C void PlaceholderForRemovedExport1(MMDXMLParserObserver* aParserObserver);
	IMPORT_C void PlaceholderForRemovedExport2(MMDXMLParserObserver* aParserObserver, MXMLDtd* aDtdRepresentation);
	IMPORT_C void PlaceholderForRemovedExport3();


private:
	enum TPanicCode {	ENullMemVarDataSource, 
						ENullMemVarParserObserver, 
						ENullMemVarXMLDoc, 
						ENullMemVarElementTag, 
						ENullParameterParentElement };
	void Panic(TPanicCode aReason) const;

private:
	MMDXMLParserObserver* iParserObserver;
	MXMLDtd* iDtdRepresentation;
	TInt iError;								// Current error
	TXMLErrorCodeSeverity iSeverity;			// ErrorCode severity
	CMDXMLDocument* iXMLDoc;					// Document created by the parser
	CMDXMLEntityConverter* iEntityConverter;	// Entity converter used by the parser
	HBufC* iElementTag;							// Currently processed element tag
	TBool iDocTypeSet;
	TBool iVersionSet;
	TInt iBytesPerChar;

	/* member variables dealing with access to source data */
	TPtrC8 iInputBufferPtr;						// set during a call to get more data
	TInt iCurrentInputBufferLen;				// current length of the data block available
	TInt iNextChar;								// read position in the data block
	TInt iInputBytesRemaining;					// number of bytes remaining to read.
	HBufC8 *iUTF8EdgeBuffer;					// buffer to hold up to 6 bytes so that UTF8 parsing can span edges of data blocks
	HBufC8 *iBomBuffer;							// buffer to hold data at the start of the stream so we may determine charset
	TInt iRequiredUTF8Bytes;					// number of bytes required to complete the character held in the edge buffer
	TBool iUnicodeInputMisaligned;				// Set to ETrue if the unicode input stream is not aligned to 16-bit boundaries
	MMDXMLParserDataProvider* iDataSource;		// XML Data Source being parsed.
	CMDXMLParserFileDataSource* iFileSource;	// We own this, and need to free it when we are done. Only used when we're providing the data source object to wrap a local file.

	/* member variables dealing with chunked conversion into unicode output */
	TBuf<32> iUnicodeConversion;				// buffer to temporarily hold the results of conversion from UTF8 to Unicode
	TInt iUnicodeConversionLen;					// number of characters stored in our intermediate buffer
	TInt iUnicodeReadPos;						// next character to send from our intermediate buffer
	TBuf<1> iSpareChar;

	/* member variables used when parsing a local file */
	TDesC *iFileToParse;
	RFs iRFs;
	RFile iFileHandleToParse;

	TBool iEndOfTag;
	
	/* member variables used in DoParseLoopL() */
	TBool iOpened;
	TBool iClosed;
	CMDXMLElement* iNewElement;
	CMDXMLElement* iParentElement;
	HBufC* iText;
	enum EParserStates
		{
		KInitFromFile,
		KDetermineCharset,
		KWaitingForData,
		KParseData,
		KSpanDataGap,
		KFinished
		};

	EParserStates iState;
	EParserStates iPreviousState;
	TInt iSuspiciousCharacter;
	TBool iStoreInvalid;						// controls whether invalid elements and attributes are stored in the DOM.
	TBool iPreserve;

	};

#endif
