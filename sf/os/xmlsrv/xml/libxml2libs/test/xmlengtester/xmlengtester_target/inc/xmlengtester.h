// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file for Libxml2Tester
//


#ifndef XML_TEST_H
#define XML_TEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <f32file.h>
#include <Xml\XmlParserErrors.h>

#include <xml/dom/xmlengdom.h>
#include <xml/dom/xmlengerrors.h>
// This include is not required anymore.
// #include <XmlEngXPathObject.h>
#include <xml/dom/xmlengdomparser.h>

#include <xml/dom/xmlengdeserializer.h>
#include <xml/dom/xmlengdeserializerdom.h>
#include <xml/dom/xmlengserializer.h>
#include <xml/dom/xmlengxpathresult.h>
 
// CONSTANTS

// MACROS
// Logging path
_LIT( KXML_TestLogPath, "\\logs\\Libxml2Tester\\" ); 
// Log file
_LIT( KXML_TestLogFile, "Libxml2Tester.txt" ); 

_LIT( KTEST_NAME, "Libxml2Tester" );

_LIT8(KXmlMimeType, "text/xml");

// parser implementation
_LIT8(KImplementationID, "");
// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CXML_Test;
class RXmlEngXPathResult;

// DATA TYPES

// CLASS DECLARATION

/**
*  CXML_Test test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CLibxml2Tester) : public CScriptBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CLibxml2Tester* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Destructor.
        */
        virtual ~CLibxml2Tester();

    public: // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        
    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
                
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        
    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        
    private:

        /**
        * C++ default constructor.
        */
        CLibxml2Tester( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        
        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();
        
        /**
        * Test methods are listed below. 
        */
/**************************** TEST *************************************/        
        /**
        * comparing two files ( skip or not skip white space ).
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        TInt CompareFilesL( CStifItemParser& aItem );
        
        /**
        * comparing two strings;
        * @since ?Series60_version
        * @param aString string.
        * @param aDesc string.
        * @return true if strings are the same.
        */
        TBool CompareDescAndChar(const char* aString, TDesC8& aDesc);
        
        /**
        * comparing two strings;
        * @since ?Series60_version
        * @param aString string.
        * @param aDesc string.
        * @return true if strings are the same.
        */
        TBool CompareDescAndChar(const TUint8* aString,TDesC8& aDesc);
        
        /**
        * comparing two strings;
        * @since ?Series60_version
        * @param aString1 string.
        * @param aString2 string.
        * @return true if strings are the same.
        */
        TBool CompareChars(const char* aString1,const char* aString2);
            
        
/****************************** SAX *************************************/        
        /**
        * Parsing xml file using SAX.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ParseFileSAXL( CStifItemParser& aItem );
        
        /**
        * Parsing xml file using SAX with name space reporting.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ParseFileSAXNSL( CStifItemParser& aItem );
        
        /**
        * Parsing xml file using SAX and convert tags name to lower case.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ParseFileSAXLowerCaseL( CStifItemParser& aItem );
        
        /**
        * Parsing xml file using SAX with skipping entities from external subset feature on.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ParseFileSAXExternalSubsetL( CStifItemParser& aItem );        
        
        /**
        * Parsing xml from buffer using SAX.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ParseBufSAXL( CStifItemParser& aItem );
        
/******************************* DOM *************************************/       
        /**
        * Parsing xml file using SAX. As output is generated xml file.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ParseFileDomL( CStifItemParser& aItem );
        
        /**
        * Parsing xml file using SAX. As output is generated xml file.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SaveXmlToFileDomL( CStifItemParser& aItem );
        
        virtual TInt CreateDocumentL( CStifItemParser& aItem );
        
        virtual TInt CheckInnerXmlL( CStifItemParser& );
        
        virtual TInt CheckOuterXmlL( CStifItemParser& );
        
        virtual TInt CreateDocumentFromNodeL( CStifItemParser& aItem );
        
        virtual TInt CheckDocumentParametersL( CStifItemParser& aItem );
        
        virtual TInt CreateDocumentElementL( CStifItemParser& aItem );
        
        virtual TInt SaveXmlToBufferUTF16L( CStifItemParser& aItem );
        
        virtual TInt TestNodeFilterL( CStifItemParser& aItem );
        
        virtual TInt CloneDocumentL( CStifItemParser& aItem );
        
        virtual TInt CheckNotNullElementL( CStifItemParser& aItem );
        
        virtual TInt CheckSiblingFunctionsL( CStifItemParser& aItem );
        
        virtual TInt CheckNodeParametersL( CStifItemParser& aItem);
        
        virtual TInt ParentNodesL( CStifItemParser& aItem );
        
        //virtual TInt NameModificationL( CStifItemParser& aItem );
        
        virtual TInt CheckNamespacesL( CStifItemParser& aItem );
        
        virtual TInt CheckProcessingInstructionL( CStifItemParser& aItem );
        
        virtual TInt CheckCommentsL( CStifItemParser& aItem );
        
        virtual TInt CheckCDATAL( CStifItemParser& aItem);
        
        virtual TInt CheckTextNodeL( CStifItemParser& aItem);
        
        //virtual TInt CheckDocumentFragmentL( CStifItemParser& aItem );
        
        virtual TInt NodeNamespacePocessingL( CStifItemParser& aItem );
        
        virtual TInt CheckEntityRefL( CStifItemParser& aItem );
        
        virtual TInt NodeTreeModificationL( CStifItemParser& aItem);
        
        virtual TInt NodeTreeDeletingL( CStifItemParser& aItem);
        
        virtual TInt CheckNodeListL( CStifItemParser& aItem );
        
        virtual TInt ElementNamespacePocessingL( CStifItemParser& aItem );
        
        virtual TInt CheckAttributesL( CStifItemParser& aItem );
        
        virtual TInt CheckElementAttributesL( CStifItemParser& aItem );
        
        virtual TInt CheckModifAttributesL( CStifItemParser& aItem );
        
        virtual TInt AddingAttributesL( CStifItemParser& aItem );
        
        virtual TInt AddingElementsL( CStifItemParser& aItem );
        
        virtual TInt CheckModifElementL( CStifItemParser& aItem );
        
        virtual TInt SetElementTextL( CStifItemParser& aItem );
        
        virtual TInt CheckNextL( CStifItemParser& aItem );
                
        virtual TInt AddXmlIdL( CStifItemParser& aItem );                
        virtual TInt FindXmlIdL( CStifItemParser& aItem );                
        virtual TInt SetValueNoEncTestL(CStifItemParser& aItem);     
        virtual TInt CreateDocument2L( CStifItemParser& aItem ); 
        virtual TInt CreateEntityReferenceTestL(CStifItemParser& aItem);   
        virtual TInt ParseFileDom2L( CStifItemParser& aItem );
        virtual TInt ParseFileDom3L( CStifItemParser& aItem );
        virtual TInt LookupNamespaceByPrefixTestL( CStifItemParser& aItem );   
        virtual TInt LookupNamespaceByUriTestL( CStifItemParser& aItem );
        virtual TInt LookupNamespaceByPrefixTest2L( CStifItemParser& aItem );   
        virtual TInt LookupNamespaceByUriTest2L( CStifItemParser& aItem );
        virtual TInt RemoveChildrenTestL( CStifItemParser& aItem);
        virtual TInt UserDataTestL( CStifItemParser& aItem );
        virtual TInt BaseUriTestL( CStifItemParser& /*aItem*/ );
        virtual TInt AddXmlIdTestL( CStifItemParser& aItem);
        virtual TInt AddXmlId2L( CStifItemParser& aItem );
        virtual TInt AddXmlId3L( CStifItemParser& aItem );
        /******************************* BinCont ************************************/			
        /**
        * Test binary container function
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt BinaryContainerL(CStifItemParser& aItem);
        virtual TInt PartialBinaryContainerL(CStifItemParser& aItem);
        virtual TInt TestCreateBinaryContainerL( TPtrC8 aCid, TPtrC8 binaryDataPtr, TPtrC pOut);
        virtual TInt TestSizeOfBinaryContainerL( RXmlEngDocument& iDocument, TPtrC8 aCid,TPtrC8 binaryDataPtr, TInt size );
        virtual TInt TestBinaryContainerNodeTypeL( RXmlEngDocument& iDocument, TPtrC8 aCid,TPtrC8 bufferPtr );
        virtual TInt TestContentsOfBinaryContainerL( RXmlEngDocument& iDocument, TPtrC8 aCid,TPtrC8 binaryDataPtr, TPtrC pOut1);
        virtual TInt TestMoveBinaryContainerL( RXmlEngDocument& iDoc1, RXmlEngDocument& iDoc2, TPtrC8 aCid, TPtrC8 binaryDataPtr, TPtrC pOut1, TPtrC pOut2);
        virtual TInt TestRemoveBinaryContainerL( RXmlEngDocument& iDocument, TPtrC8 aCid,TPtrC8 binaryDataPtr, TPtrC pOut1, TPtrC pOut2 );
        virtual TInt TestCopyBinaryContainerL( RXmlEngDocument& iDocument, TPtrC8 aCid, TPtrC8 binaryDataPtr, TPtrC pOut1, TPtrC pOut2 );
        virtual TInt TestReplaceBinaryContainerL( RXmlEngDocument& iDoc1, RXmlEngDocument& iDoc2, 
													TPtrC8 aCid, TPtrC8 binaryDataPtr, TPtrC pOut1, TPtrC pOut2);													
        virtual TInt TestAppendContentsL(CStifItemParser& aItem);
        virtual TInt BinaryContainer_GetListL( CStifItemParser& aItem );  
        virtual TInt BinaryContainer_GetList2L( CStifItemParser& aItem );  
        virtual TInt BinaryContainer_CidErrorL( CStifItemParser& aItem );
        virtual TInt BinaryContainer_RemoveL( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_Remove2L( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_Remove3L( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_UnlinkL( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_ReplaceWithL( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_SubstituteForL( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_CopyL( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_Copy2L( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_Copy3L( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_Copy4L( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_CopyToL( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_CloneL( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_AdoptNodeL( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_SetAsFirstSiblingL( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_SetAsLastSiblingL( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_MoveBeforeSiblingL( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_MoveAfterSiblingL( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_MoveToL( CStifItemParser& /*aItem*/ );
        virtual TInt BinaryContainer_ReconcileNamespacesL( CStifItemParser& /*aItem*/ );

        /******************************* CHUNK *************************************/    
        virtual TInt ParseFileChunkL( CStifItemParser& aItem );      
        virtual TInt ChunkContainer_GetListL( CStifItemParser& aItem );  
        virtual TInt ChunkContainer_CidL( CStifItemParser& aItem );
        virtual TInt ChunkContainer_CidErrorL( CStifItemParser& aItem );
        virtual TInt ChunkContainer_ChunkL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_OffsetL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_SizeL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_NodeTypeL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_RemoveL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_Remove2L( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_Remove3L( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_UnlinkL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_ReplaceWithL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_SubstituteForL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_CopyL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_Copy2L( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_Copy3L( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_Copy4L( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_CopyToL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_CloneL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_AdoptNodeL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_SetAsFirstSiblingL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_SetAsLastSiblingL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_MoveBeforeSiblingL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_MoveAfterSiblingL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_MoveToL( CStifItemParser& /*aItem*/ );
        virtual TInt ChunkContainer_ReconcileNamespacesL( CStifItemParser& /*aItem*/ );
    /******************************* FILE CONTAINER *************************************/        
        virtual TInt FileContainer_GetListL( CStifItemParser& /*aItem*/  );  
        virtual TInt FileContainer_CidL( CStifItemParser& /*aItem*/  );
        virtual TInt FileContainer_CidErrorL( CStifItemParser& /*aItem*/  );
        virtual TInt FileContainer_SizeL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_FileL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_NodeTypeL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_RemoveL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_Remove2L( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_Remove3L( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_UnlinkL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_ReplaceWithL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_SubstituteForL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_CopyL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_Copy2L( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_Copy3L( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_Copy4L( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_CopyToL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_CloneL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_AdoptNodeL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_SetAsFirstSiblingL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_SetAsLastSiblingL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_MoveBeforeSiblingL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_MoveAfterSiblingL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_MoveToL( CStifItemParser& /*aItem*/ );
        virtual TInt FileContainer_ReconcileNamespacesL( CStifItemParser& /*aItem*/ );

        
        virtual TInt SetTextNoEncTestL(CStifItemParser& aItem);
        virtual TInt AppendTextNoEncTestL(CStifItemParser& aItem);
/******************************* UTIL *************************************/       
        /**
        * Convert const char* to char* 
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt UtilsEscapeForXmlValueL(CStifItemParser& aItem);
        
        /**
        * Generate char* from TDes
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt UtilsXmlCharFromDesL( CStifItemParser& aItem );
        
        /**
        * Generate char* from TDes8
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt UtilsXmlCharFromDes8L( CStifItemParser& aItem );        
        
/******************************* XPATH ***********************************/         
        
        virtual TInt EvaluateExpressionL( CStifItemParser& aItem );
        virtual TInt SaveResultL( CStifItemParser& aItem);
        virtual TInt ToNumberL( CStifItemParser& aItem);
        virtual TInt ToBoolL( CStifItemParser& aItem);
        virtual TInt ToStringL( CStifItemParser& aItem);
        virtual TInt InstanceL( CStifItemParser& aItem);
        virtual TInt ExtensionFunctionL( CStifItemParser& aItem);
        virtual TInt ExtensionVectorL( CStifItemParser& aItem);     
   
     /* 
     *Write file from RBuf8
     *
     *@ returns void
     */
     
	 void WriteFileFromBufferL(TDesC& aFile, RBuf8& buffer);
     
     /* 
     *Read aFile to HBufC8
     *
     *@ returns buffer
     */
     
     HBufC8* ReadFileToBufferL(TDesC& aFile);
        
	/**
     * Generates random CID
     *
     * @since S60 v3.2
	 * @param aCid randomly generated CID string
     */
	 void GenerateRandomCid(TDes8& aCid);

    
        
        /** 
        * Saving node in XML document
        *
        * @param aCString a c-style string .
        */
	void SaveNode(RFile aFile,TXmlEngNode aNode);    
               
        /** 
        * A function to use with TCleanupItem to push c-style arrays
        * on cleanup stack. It simply calls delete[] aCArray;
        * @param aCString a c-style string .
        */
        static void CArrayCleanup( TAny* aCArray );  
        
         /** 
        * Finding node in XML document
        *
        * @param aCString a c-style string .
        */  
        TXmlEngNode FindNodeL(RXmlEngDocument document, TPtrC8 pName8);		// defect
        
        TInt FindElement(const TDesC8& aElementName, const TXmlEngElement& aParent, TXmlEngElement& aResult);	// defect(?)
        
        	/**
        * Saves XML document 
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */	
	virtual TInt SaveDocumentL(RXmlEngDocument& iDocument, TPtrC& pOut);
		
        /**
        * returns HBufC8 pointer to content of file with TDesC filename 
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */		
        virtual TInt GetHexFromPointerL(TPtrC pSerOpt);
        
        /**
        * returns TInt from TPtrC(pointer to hexadecimal value)  
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual HBufC8* ReadFileL(const TDesC& aFileName);
        
        /**
        * returns serializer / deserializer type
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        
        TXmlEngSerializerType RetSerializerType(TPtrC& type);
		TXmlEngDeserializerType RetDeserializerType(TPtrC& type);

		 /**
        * creating DOM document
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */		
				
		void SetupDocumentL(  );

		
/******************************* Serializer ************************************/
		 /**
        * Test serializer
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
		
		
		virtual TInt TestSerializerL(CStifItemParser& aItem);
		virtual TInt TestSetOutputFileL(CStifItemParser& aItem);
		virtual TInt TestSetOutputBufferL(CStifItemParser& aItem);
        virtual TInt TestSetOutputStreamL(CStifItemParser& aItem);
        virtual TInt TestSerializationOptionsL(CStifItemParser& aItem);
		virtual TInt TestSerializeFile1L(CStifItemParser& aItem);
		virtual TInt TestSerializeFile2L(CStifItemParser& aItem);
		virtual TInt TestSerializeFile3L(CStifItemParser& aItem);
		virtual TInt TestSerializeBufferL(CStifItemParser& aItem);
		virtual TInt TestSerializePartTreeL(CStifItemParser& aItem);
		virtual TInt TestSerializeFileBufferL(CStifItemParser& aItem);
		virtual TInt TestSerializeChildsL(CStifItemParser& aItem);
		virtual TInt TestSerializeFileConvinienceL(CStifItemParser& aItem);
		virtual TInt TestSerializeBufferConvinienceL(CStifItemParser& aItem);
		virtual TInt TestMultipleSerializeToBufferL(CStifItemParser& aItem);
		virtual TInt TestSerializationOptionsBufferL(CStifItemParser& aItem);
		virtual TInt TestContentTypeL(CStifItemParser& aItem);

		
/******************************* SerXOP ************************************/
        /**
        * Test serialize XOP function
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */		
	virtual TInt SerializeXOPL(CStifItemParser& aItem);
	virtual TInt DirtySerializeXOPL(CStifItemParser& aItem);
	virtual TInt SerializeXopSoapL(CStifItemParser& aItem);
	virtual TInt MultiSerializeXopSoapL(CStifItemParser& aItem);
	virtual TInt SerializeXopFileDirtyL(CStifItemParser& aItem);
	virtual TInt SerializeXopDirtySoapL(CStifItemParser& aItem);
	virtual TInt SerializeXopBufferL(CStifItemParser& aItem);
	virtual TInt SerializeXopFileL(CStifItemParser& aItem);
	virtual TInt Serxop3diffContL(CStifItemParser& aItem);
	
		
        /**
        * Function tests, serialize XOP
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
	virtual TInt TestSerialize1paraToFileL( RXmlEngDocument& iDocument, TPtrC pXml, 
									TPtrC pSerOpt, TXmlEngSerializerType serializerType = ESerializerDefault );
	virtual TInt TestSerialize3paraToFileL( RXmlEngDocument& iDocument, TPtrC pXml,
									TPtrC pSerOpt,  TXmlEngSerializerType serializerType = ESerializerDefault  );
	virtual TInt TestSerializeRFsL( RXmlEngDocument& iDocument, TPtrC pXml, 
									TPtrC pSerOpt, TXmlEngSerializerType serializerType = ESerializerDefault  );
	virtual TInt TestSerializeToBufferL( RXmlEngDocument& iDocument, TPtrC pXml, 
									TPtrC pSerOpt, TXmlEngSerializerType serializerType = ESerializerDefault  );	
        /**
        * Saves XML document with added binary data
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SaveXmlWithBinaryDataL(RXmlEngDocument& iDocument, TPtrC serOptions, 
											TPtrC pData, TPtrC pXml, TPtrC pOut, TBool multiNode = FALSE);
		virtual TInt SerializeDefaultFileL( CStifItemParser& aItem );
/******************************* SerGZIP ************************************/		
        /**
        * Function tests, serialize GZIP
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SerializeGZIPL(CStifItemParser& aItem);
        virtual TInt DecompressGZIPL(CStifItemParser& aItem);
		virtual TInt DecompressBufferGZIPL(CStifItemParser& aItem);
		virtual TInt CompressBufferGZIPL(CStifItemParser& aItem);
		virtual TInt SerializeDirtyGzipL(CStifItemParser& aItem);


/******************************* Deserialize ************************************/
        virtual TInt DeserializeL( CStifItemParser& aItem);
        virtual TInt Deserialize2L( CStifItemParser& aItem);
        virtual TInt DeserializeFromFileL( CStifItemParser& aItem);
        virtual TInt DeserializeFromBufferL(CStifItemParser& aItem);
		virtual TInt TestDeserializerL(CStifItemParser& aItem);
		virtual TInt TestSetInputFileL(CStifItemParser& aItem);
		virtual TInt TestSetInputBufferL(CStifItemParser& aItem);
		virtual TInt TestSetContentHandlerL(CStifItemParser& aItem);
		virtual TInt TestEnableFeatureL(CStifItemParser& aItem);
		virtual TInt TestDisableFeatureL(CStifItemParser& aItem);
		virtual TInt TestIsFeatureEnabledL(CStifItemParser& aItem);
		virtual TInt TestUseExternalDataL(CStifItemParser& aItem);
		virtual TInt TestExternalDataL(CStifItemParser& aItem);
		virtual TInt TestDesFileConvinienceL(CStifItemParser& aItem);
		virtual TInt TestDesBufferConvinienceL(CStifItemParser& aItem);
		virtual TInt TestDeserializerBufferL(CStifItemParser& aItem);
		virtual TInt TestSetParsingOptions1L(CStifItemParser& aItem);
		
		virtual TInt DeserializeInfosetL(CStifItemParser& aItem);
        virtual TInt Deserialize2InfosetL(CStifItemParser& aItem);
        virtual TInt DeserializeFromFileInfosetL(CStifItemParser& aItem);
        virtual TInt DeserializeFromBufferInfosetL(CStifItemParser& aItem);
		virtual TInt TestDeserializerInfosetL(CStifItemParser& aItem);
		virtual TInt TestSetInputFileInfosetL(CStifItemParser& aItem);
		virtual TInt TestSetInputBufferInfosetL(CStifItemParser& aItem);
		virtual TInt TestSetContentHandlerInfosetL(CStifItemParser& aItem);
		virtual TInt TestEnableFeatureInfosetL(CStifItemParser& aItem);
		virtual TInt TestDisableFeatureInfosetL(CStifItemParser& aItem);
		virtual TInt TestIsFeatureEnabledInfosetL(CStifItemParser& aItem);
		virtual TInt TestUseExternalDataInfosetL(CStifItemParser& aItem);
		virtual TInt TestExternalDataInfosetL(CStifItemParser& aItem);
		virtual TInt TestDesFileConvinienceInfosetL(CStifItemParser& aItem);
		virtual TInt TestDesBufferConvinienceInfosetL(CStifItemParser& aItem);
		virtual TInt TestDeserializerBufferInfosetL(CStifItemParser& aItem);
		
/******************************* DeserializeDOM ************************************/
        virtual TInt DOMDeserializeL( CStifItemParser& aItem);
        virtual TInt DOMDeserialize2L( CStifItemParser& aItem);
        virtual TInt DOMDeserializeFromFileL( CStifItemParser& aItem);
        virtual TInt DOMDeserializeFromFile2L( CStifItemParser& aItem);
        virtual TInt DOMDeserializeFromBufferL(CStifItemParser& aItem);

        virtual TInt TestDOMDeserializerL(CStifItemParser& aItem);
        virtual TInt TestDOMSetInputFileL(CStifItemParser& aItem);
        virtual TInt TestDOMSetInputBufferL(CStifItemParser& aItem);
        virtual TInt TestDOMUseExternalDataL(CStifItemParser& aItem);
        virtual TInt TestDOMExternalDataL(CStifItemParser& aItem);

/**************************** Test function end **************************/

    public:     // Data
    
    protected:  // Data
 
    private:    // Data
    
    // DOM implementation for dom tests
	RXmlEngDOMImplementation DOM_impl;

	// Dom parser
	RXmlEngDOMParser parser;
	
	// XML document ( dom tree )
	RXmlEngDocument iDoc;
	
	// XML node ( dom tree )
	TXmlEngNode iNode;
	
	// XML element
	TXmlEngElement iElement;
	
	// XPath result
	RXmlEngXPathResult iResult;
	
	// Log info number
	TInt infoNum;
	
	// Last error number
	TInt iLastError;
	
		// cheap resize 
	RHeap* he;
	RHeap* oldHeap;
	CTrapCleanup* cleanup;
	
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };



#endif      // XML_TEST_H

           
// End of File