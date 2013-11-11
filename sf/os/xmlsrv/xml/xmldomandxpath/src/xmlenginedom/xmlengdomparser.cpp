// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements methods for parsing XML data
//

#include <stdlib.h>

#include <xml/dom/xmlengdomparser.h>
#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengerrors.h>
#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengxestd.h>
#include <stdapis/libxml2/libxml2_parser.h>
#define PARSER_CTXT (static_cast<xmlParserCtxtPtr>(iInternal))

// ---------------------------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngDOMParser::RXmlEngDOMParser(): iInternal(NULL), iError(0), iImpl(NULL)
	{
	}

// ---------------------------------------------------------------------------------------------
// Opens parser
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TInt RXmlEngDOMParser::Open(RXmlEngDOMImplementation& aDOMImpl)
    {
    iImpl = &aDOMImpl;
    return KErrNone;
    };

// ---------------------------------------------------------------------------------------------
// Frees all allocated by parser resources.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void RXmlEngDOMParser::Close()
    {
    Cleanup();
    iImpl = NULL;
    }
// ---------------------------------------------------------------------------------------------
// Builds DOM document from file. File is read by blocks of specified size, thus,
// avoiding keeping in memory both file and DOM tree for it.
//
// @note Use RDOMPushParser for parsing large files.
// @param aRFs File server session
// @param aFileName File name
// @return The created document
// @see GetLastParsingError()
// @leave KXmlEngErrParsing Parsing error
// @leave KXmlEngErrWrongUseOfAPI OpenL() not previously called
// @leave - One of the system-wide error codes
// ---------------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngDocument RXmlEngDOMParser::ParseFileL(
    RFs &aRFs,
    const TDesC& aFileName,
    TUint aChunkSize )
    {
    if  ( aChunkSize == 0 )
    	{
    	return ParseFileWithoutChunksL( aRFs, aFileName );
    	}
    
    RFile xmlFile;
    TInt res;
    
    User::LeaveIfError(xmlFile.Open(aRFs, aFileName, EFileRead | EFileShareReadersOnly));
    CleanupClosePushL(xmlFile);
    
    RBuf8 buffer;
    if (aChunkSize < (KMaxTInt/2))
        {
        buffer.CreateL(aChunkSize);
        }
    else
        {
        User::Leave(KErrArgument);
        }
    CleanupClosePushL(buffer);
    
   	User::LeaveIfError(xmlFile.Read(buffer,aChunkSize));
	TUint length;
	
	do
		{
		ParseChunkL(buffer);
		res = xmlFile.Read(buffer, aChunkSize);
		if(res)
		    {
		    Close();
		    User::Leave(res);
		    }
		length = buffer.Length();
		} while (length);
		
	CleanupStack::PopAndDestroy(&buffer);
    CleanupStack::PopAndDestroy(&xmlFile);
    return FinishL();
    }
// ---------------------------------------------------------------------------------------------
// Builds DOM document from file.
// @see ParseFileL(RFs,TDesC&,TUint)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngDocument RXmlEngDOMParser::ParseFileL(
    const TDesC& aFileName,
    TUint aChunkSize )
    {
    RFs aRFs;
    User::LeaveIfError(aRFs.Connect());
    CleanupClosePushL(aRFs);
    RXmlEngDocument doc = ParseFileL(aRFs, aFileName, aChunkSize);
    CleanupStack::PopAndDestroy(); // aRFs
    return doc;
    }
// ---------------------------------------------------------------------------------------------
// Pushes new data from buffer to DOM parser.
// Parsing is performed synchronously; method returns as soon as block is parsed.
//
// Buffer contents is not needed for further parsing after this method.
// Method may leave with KErrXMLSyntax code or one of general codes (e.g. KErrNoMemory)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void RXmlEngDOMParser::ParseChunkL(
    const TDesC8& aBuffer )
    {
    if(!iImpl)
        {
        User::Leave(KXmlEngErrWrongUseOfAPI);
        }
    
    xmlParserCtxtPtr ctxt = PARSER_CTXT;
    if(!ctxt)
        {
        iError = KErrNone;
        // First call after reset or parser creation
        ctxt = xmlCreatePushParserCtxt(
                            NULL , /* default set of callbacks is used (for building tree) */
                            NULL , /* save as userData */
                            (const char*)aBuffer.Ptr(),
                            aBuffer.Size(),
                            NULL /* filename or URI */);
        if(!ctxt)
            {
            XmlEngOOMTestL();
            };
        iInternal = ctxt;    
        xmlCtxtUseOptions(ctxt, XML_PARSE_DTDLOAD | XML_PARSE_NOENT | XML_PARSE_NODICT);
        iError = xmlParseChunk(ctxt,
                      ""   , /* char* data */
                      0    , /* int size */
                      0);    /* flag: 0 - not the last chunk */
        }
    else
        {
        iError = xmlParseChunk(ctxt,
                      (char*)aBuffer.Ptr(), /* char* data */
                      (TInt)aBuffer.Size(), /* int size */
                      0);                   /* flag: 0 - not the last chunk */
        }
    if(iError != XML_ERR_OK && ctxt->lastError.level == XML_ERR_FATAL)
        {
        Cleanup();        
        XmlEngOOMTestL();
        User::Leave(KXmlEngErrParsing);
        }
    }

// ---------------------------------------------------------------------------------------------
// Returns constructed document after the last block of XML document is parsed.
// Ownership over returned RXmlEngDocument object is transferred to the caller of the method.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngDocument RXmlEngDOMParser::FinishL()
    {
    if(!iImpl)
        {
        User::Leave(KXmlEngErrWrongUseOfAPI);
        }
    
    xmlDocPtr doc;
    xmlParserCtxtPtr ctxt = PARSER_CTXT;
    
    if(!ctxt)
        {
        User::Leave(KXmlEngErrWrongUseOfAPI);
        }
    
    iError = xmlParseChunk(ctxt,
                        NULL /* char* data */,
                        0    /* int size */,
                        1    /* flag: 1 - the last chunk */);
    
    if(iError != XML_ERR_OK  && ctxt->lastError.level == XML_ERR_FATAL)
        {
        Cleanup();
        XmlEngOOMTestL();        
        User::Leave(KXmlEngErrParsing);
        }
        
    doc = ctxt->myDoc;
    ctxt->myDoc = NULL;
    xmlFreeParserCtxt(ctxt);
    iInternal = NULL;
    RXmlEngDocument rdoc;
    rdoc.OpenL(*iImpl,doc);
    return rdoc;
    }

// ---------------------------------------------------------------------------------------------
// Parses XML data from memory buffer and builds DOM RXmlEngDocument
//
// May leave with KXmlEngErrParsing code
// ---------------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngDocument RXmlEngDOMParser::ParseL(
    const TDesC8& aBuffer )
    {
    if(!iImpl)
        {
        User::Leave(KXmlEngErrWrongUseOfAPI);
        }
    
    TInt code = 0;
    xmlDocPtr tree = xmlSAXParseDoc(
                        NULL, 
                        (xmlChar*) aBuffer.Ptr(), 
                        aBuffer.Size(), 
                        0, 
                        &code /* errorCode */);
    TEST_OOM_FLAG;
    iError = code;
    if(!tree)
        XmlEngLeaveL(KXmlEngErrParsing);
    RXmlEngDocument doc;
    doc.OpenL(*iImpl,tree);
    return doc;
    }
// ---------------------------------------------------------------------------------------------
// Return last parsing error
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TInt RXmlEngDOMParser::GetLastParsingError()
    {
    return iError;
    }
// ---------------------------------------------------------------------------------------------
// Parses XML file and builds DOM RXmlEngDocument without usage of chunks
//
// May leave with KXmlEngErrParsing code (besides system I/O error codes)
// ---------------------------------------------------------------------------------------------
//
RXmlEngDocument RXmlEngDOMParser::ParseFileWithoutChunksL(
    RFs &aRFs, 
    const TDesC& aFileName )
    {
    TInt size;
    RFile xmlFile;
        
    // Read file
    User::LeaveIfError(xmlFile.Open(aRFs, aFileName, EFileRead | EFileShareReadersOnly));
    CleanupClosePushL(xmlFile);
    User::LeaveIfError(xmlFile.Size(size));
    HBufC8* buffer = HBufC8::NewLC(size);
    TPtr8 bufferPtr = buffer->Des();
    User::LeaveIfError(xmlFile.Read(bufferPtr, size));

    RXmlEngDocument doc = ParseL(bufferPtr);

    CleanupStack::PopAndDestroy(buffer);
    CleanupStack::PopAndDestroy(&xmlFile);
    return doc;
    }

// ---------------------------------------------------------------------------------------------
// Cleanup internal data
// ---------------------------------------------------------------------------------------------
//
void RXmlEngDOMParser::Cleanup()
    {
    xmlParserCtxtPtr ctxt = PARSER_CTXT;
    // free all allocated resources
    if(ctxt)
        {
        if(ctxt->myDoc)
            xmlFreeDoc(ctxt->myDoc);
        xmlFreeParserCtxt(ctxt);
        iInternal = NULL;
        }
    }

