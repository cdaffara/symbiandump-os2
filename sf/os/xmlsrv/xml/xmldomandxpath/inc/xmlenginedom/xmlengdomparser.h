/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

/** @file
@publishedAll
@released
*/
#ifndef XMLENGDOMPARSER_H
#define XMLENGDOMPARSER_H

#include <f32file.h>

class RXmlEngDOMImplementation;
class RXmlEngDocument;

/** 
This class implements methods for parsing XML data.  XML data may be parsed
from a chunk, file, or memory buffer.

Sample code for parsing from buffer:
@code
     RXmlEngDOMImplementation domImpl;
     domImpl.OpenL();              // opening DOM implementation object 
     RXmlEngDOMParser parser;
     parser.Open( domImpl );   // opening parser object
     RXmlEngDocument iDoc;
     iDoc =parser.ParseL( *aInput );   // parsing aInput - buffer  
     iDoc.Close();               // closing all opened objects
     parser.Close();
     domImpl.Close();
@endcode 

Sample code for parsing from file:
@code
     RXmlEngDOMImplementation domImpl;
     domImpl.OpenL();              // opening DOM implementation object 
     RXmlEngDOMParser parser;
     parser.Open( domImpl );   // opening parser object
     RXmlEngDocument iDoc;
     iDoc =parser.ParseFileL( aFileName );   // parsing from file  
     iDoc.Close();               // closing all openend objects
     parser.Close();
     domImpl.Close();
@endcode 
*/
class RXmlEngDOMParser
{
public:
    /** Default constructor */
    IMPORT_C RXmlEngDOMParser();
	
    /** 
	Opens the parser.  The RXmlEngDOMImplementation object passed as an
	argument may be used by multiple RXmlEngDOMParser objects.

	@param aDOMImpl DOM implementation object previously opened without error.
    @return KErrNone if successful, system wide error code otherwise
    */
    IMPORT_C TInt Open(RXmlEngDOMImplementation& aDOMImpl);
    
    /** Closes the parser. */
    IMPORT_C void Close();

	/** 
	Parses a chunk of XML data from a memory buffer and builds an internal DOM
	tree.  The DOM tree can be accessed by calling FinishL() to obtain a
	RXmlEngDocument.

	@see FinishL()
    @param aBuffer XML data buffer
	@see GetLastParsingError()
    @leave KXmlEngErrParsing Parsing error
	@leave KXmlEngErrWrongUseOfAPI OpenL() not previously called
	@leave - One of the system-wide error codes
    */
    IMPORT_C void ParseChunkL(const TDesC8& aBuffer);

	/** 
	Creates a document from chunks of data previously parsed by ParseChunkL().
	Should be called after parsing all chunks.  Ownership of the returned
	RXmlEngDocument object is transferred to the caller of the method.
	RXmlEngDocument::Close() must be called when the document is no longer
	required.

	@see ParseChunkL()
    @return The created document
	@see GetLastParsingError()
    @leave KXmlEngErrParsing Parsing error
	@leave KXmlEngErrWrongUseOfAPI OpenL() or ParseChunkL() not previously 
	called
	@leave - One of the system-wide error codes
    */
    IMPORT_C RXmlEngDocument FinishL();
    
    /** 
	Parses XML file and builds a DOM RXmlEngDocument.  Ownership of the
	returned RXmlEngDocument object is transferred to the caller of the method.
	RXmlEngDocument::Close() must be called when the document is no longer
	required.

	@param aRFs Open file server session
    @param aFileName File name
	@param aChunkSize The number of bytes to parse from the file at a time, or 0
	if the whole file should be parsed at once.
    @return The created document
	@see GetLastParsingError()
    @leave KXmlEngErrParsing Parsing error
	@leave KXmlEngErrWrongUseOfAPI OpenL() not previously called
	@leave - One of the system-wide error codes
	*/
    IMPORT_C RXmlEngDocument ParseFileL(RFs &aRFs, const TDesC& aFileName, TUint aChunkSize = 0);

    /** 
    Parses XML file and builds a DOM RXmlEngDocument.  Ownership of the
	returned RXmlEngDocument object is transferred to the caller of the method.
	RXmlEngDocument::Close() must be called when the document is no longer
	required.

	@param aFileName File name
	@param aChunkSize The number of bytes to parse from the file at a time, or 0
	if the whole file should be parsed at once.
    @return The created document
	@see GetLastParsingError()
    @leave KXmlEngErrParsing Parsing error
	@leave KXmlEngErrWrongUseOfAPI OpenL() not previously called
	@leave - One of the system-wide error codes
    */
    IMPORT_C RXmlEngDocument ParseFileL(const TDesC& aFileName, TUint aChunkSize = 0);

	/** 
	Parses XML data from a memory buffer that holds the entire XML structure
	and builds a DOM RXmlEngDocument.  Ownership of the returned
	RXmlEngDocument object is transferred to the caller of the method.
	RXmlEngDocument::Close() must be called when the document is no longer
	required.

	@see ParseChunkL()
	@param aBuffer XML data buffer
    @return The created document
	@see GetLastParsingError()
    @leave KXmlEngErrParsing Parsing error
	@leave KXmlEngErrWrongUseOfAPI OpenL() not previously called
	@leave - One of the system-wide error codes
    */
    IMPORT_C RXmlEngDocument ParseL(const TDesC8& aBuffer);  

    /** 
    Return last parsing error code.  Error codes are positive numbers.
	@see xmlengerrors.h
	@return The last error returned by the parser or KErrNone if none
    */
    IMPORT_C TInt GetLastParsingError();

private:
    RXmlEngDocument ParseFileWithoutChunksL(RFs& aRFs, const TDesC& aFileName);
    void Cleanup();

private:
    void* iInternal;
    TInt iError;
    RXmlEngDOMImplementation* iImpl;
};

#endif /* XMLENGDOMPARSER_H */

