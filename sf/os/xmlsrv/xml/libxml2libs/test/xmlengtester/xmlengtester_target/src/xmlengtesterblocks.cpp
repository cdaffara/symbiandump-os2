// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//


// INCLUDE FILES
#include "TestContentHandler.h"
#include "XMLOutputContentHandler.h"
#include "xmlengtester.h"

#include <e32svr.h>
#include <e32math.h>
#include <f32file.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include "xmlengtesterdef.h"
#include <Xml\Parser.h>
#include <Xml\ParserFeature.h>

#include <libc\string.h>
#include <charconv.h>
#include <UTF.H>

using namespace Xml;

// -----------------------------------------------------------------------------
// CLibxml2Tester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CLibxml2Tester::Delete() 
    {
    
    }
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
         
        // test
        ENTRY( "CompareFiles", CLibxml2Tester::CompareFilesL ),        
        
        // sax
        ENTRY( "SAXParseFile", CLibxml2Tester::ParseFileSAXL ),
        ENTRY( "SAXParseBuffer", CLibxml2Tester::ParseBufSAXL ),
        ENTRY( "SAXParseFileWithNameSpaceMapping", CLibxml2Tester::ParseFileSAXNSL ),
        ENTRY( "SAXParseFileWithTagToLowerCase", CLibxml2Tester::ParseFileSAXLowerCaseL ),
        ENTRY( "SAXParseFileWithExternalSubsetFeature", CLibxml2Tester::ParseFileSAXExternalSubsetL ),
               
        // dom       
        ENTRY( "ParseFile", CLibxml2Tester::ParseFileDomL ),
        ENTRY( "SaveXmlToFile", CLibxml2Tester::SaveXmlToFileDomL ),
        ENTRY( "CheckOuterXml", CLibxml2Tester::CheckOuterXmlL ),
        ENTRY( "CheckInnerXml", CLibxml2Tester::CheckInnerXmlL ),
        ENTRY( "CreateDocument", CLibxml2Tester::CreateDocumentL ),
        ENTRY( "CheckDocumentParameters", CLibxml2Tester::CheckDocumentParametersL ),
        ENTRY( "CreateDocumentElement", CLibxml2Tester::CreateDocumentElementL ),
        ENTRY( "CreateDocumentFromNode", CLibxml2Tester::CreateDocumentFromNodeL ),
        ENTRY( "SaveXmlToUTF16LE", CLibxml2Tester::SaveXmlToBufferUTF16L ),
        ENTRY( "TestNodeFilter1", CLibxml2Tester::TestNodeFilterL ),
        ENTRY( "CloneDocument", CLibxml2Tester::CloneDocumentL ),       
        ENTRY( "CheckNotNullElement", CLibxml2Tester::CheckNotNullElementL ),
        ENTRY( "CheckSiblingFunctions", CLibxml2Tester::CheckSiblingFunctionsL ),
        ENTRY( "CheckNodeParameters", CLibxml2Tester::CheckNodeParametersL ),
        ENTRY( "ParentNodes", CLibxml2Tester::ParentNodesL ),
        //ENTRY( "NameModification", CLibxml2Tester::NameModificationL ),
        ENTRY( "CheckNamespaces", CLibxml2Tester::CheckNamespacesL ),       
        ENTRY( "CheckProcessingInstruction", CLibxml2Tester::CheckProcessingInstructionL ),
        ENTRY( "CheckComments", CLibxml2Tester::CheckCommentsL ),       
        ENTRY( "CheckCDATA", CLibxml2Tester::CheckCDATAL ),
        ENTRY( "CheckTextNode", CLibxml2Tester::CheckTextNodeL ),
        //ENTRY( "CheckDocumentFragment", CLibxml2Tester::CheckDocumentFragmentL ),
        ENTRY( "NodeNamespacePocessing", CLibxml2Tester::NodeNamespacePocessingL ),       
        ENTRY( "CheckEntityRef", CLibxml2Tester::CheckEntityRefL ),
        ENTRY( "NodeTreeModification", CLibxml2Tester::NodeTreeModificationL ),       
        ENTRY( "NodeTreeDeleting", CLibxml2Tester::NodeTreeDeletingL ),
        ENTRY( "CheckNodeList", CLibxml2Tester::CheckNodeListL ),       
        ENTRY( "ElementNamespacePocessing", CLibxml2Tester::ElementNamespacePocessingL ),
        ENTRY( "CheckAttributes", CLibxml2Tester::CheckAttributesL ),       
        ENTRY( "CheckElementAttributes", CLibxml2Tester::CheckElementAttributesL ),
        ENTRY( "CheckModifAttributes", CLibxml2Tester::CheckModifAttributesL ),       
        ENTRY( "AddingAttributes", CLibxml2Tester::AddingAttributesL ),
        ENTRY( "AddingElements", CLibxml2Tester::AddingElementsL ),  
        ENTRY( "CheckModifElement", CLibxml2Tester::CheckModifElementL ), 
        ENTRY( "SetElementText", CLibxml2Tester::SetElementTextL ),   
        ENTRY( "WHAT", CLibxml2Tester::CheckNextL ),  
        ENTRY( "AddXmlId", CLibxml2Tester::AddXmlIdL ),
        ENTRY( "FindXmlId", CLibxml2Tester::FindXmlIdL ),
        ENTRY( "SetTextNoEncTest", CLibxml2Tester::SetTextNoEncTestL),
        ENTRY( "AppendTextNoEncTest", CLibxml2Tester::AppendTextNoEncTestL),
        ENTRY( "SetValueNoEncTest", CLibxml2Tester::SetValueNoEncTestL),
        ENTRY( "CreateDocument2", CLibxml2Tester::CreateDocument2L),
        ENTRY( "CreateEntityReferenceTest", CLibxml2Tester::CreateEntityReferenceTestL),
        ENTRY( "ParseFileDom2", CLibxml2Tester::ParseFileDom2L),
        ENTRY( "ParseFileDom3", CLibxml2Tester::ParseFileDom3L),
        ENTRY( "LookupNamespaceByPrefixTest", CLibxml2Tester::LookupNamespaceByPrefixTestL),
        ENTRY( "LookupNamespaceByUriTest", CLibxml2Tester::LookupNamespaceByUriTestL),
        ENTRY( "LookupNamespaceByPrefixTest2", CLibxml2Tester::LookupNamespaceByPrefixTest2L),
        ENTRY( "LookupNamespaceByUriTest2", CLibxml2Tester::LookupNamespaceByUriTest2L),
        ENTRY( "RemoveChildrenTest", CLibxml2Tester::RemoveChildrenTestL),
        ENTRY( "UserDataTest", CLibxml2Tester::UserDataTestL),
        ENTRY( "BaseUriTest", CLibxml2Tester::BaseUriTestL),
        ENTRY( "AddXmlIdTest", CLibxml2Tester::AddXmlIdTestL),
        ENTRY( "AddXmlId2", CLibxml2Tester::AddXmlId2L),
        ENTRY( "AddXmlId3", CLibxml2Tester::AddXmlId3L),
        
        //chunkparser
        ENTRY( "ParseFileChunk", CLibxml2Tester::ParseFileChunkL ),
        
        //chunkcontainer    
        ENTRY("ChunkContainerGetList",CLibxml2Tester::ChunkContainer_GetListL),
        ENTRY("ChunkContainerCid",CLibxml2Tester::ChunkContainer_CidL),
        ENTRY("ChunkContainerCidError",CLibxml2Tester::ChunkContainer_CidErrorL),
        ENTRY("ChunkContainerChunk",CLibxml2Tester::ChunkContainer_ChunkL),
        ENTRY("ChunkContainerOffset",CLibxml2Tester::ChunkContainer_OffsetL),
        ENTRY("ChunkContainerSize",CLibxml2Tester::ChunkContainer_SizeL),
        ENTRY("ChunkContainerNodeType",CLibxml2Tester::ChunkContainer_NodeTypeL),
        ENTRY("ChunkContainerRemove",CLibxml2Tester::ChunkContainer_RemoveL),
        ENTRY("ChunkContainerRemove2",CLibxml2Tester::ChunkContainer_Remove2L),
        ENTRY("ChunkContainerRemove3",CLibxml2Tester::ChunkContainer_Remove3L),
        ENTRY("ChunkContainerUnlink",CLibxml2Tester::ChunkContainer_UnlinkL),
        ENTRY("ChunkContainerReplaceWith",CLibxml2Tester::ChunkContainer_ReplaceWithL),
        ENTRY("ChunkContainerSubstituteFor",CLibxml2Tester::ChunkContainer_SubstituteForL),
        ENTRY("ChunkContainerCopy",CLibxml2Tester::ChunkContainer_CopyL),
        ENTRY("ChunkContainerCopy2",CLibxml2Tester::ChunkContainer_Copy2L),  
        ENTRY("ChunkContainerCopy3",CLibxml2Tester::ChunkContainer_Copy3L),       
        ENTRY("ChunkContainerCopy4",CLibxml2Tester::ChunkContainer_Copy4L),   
        ENTRY("ChunkContainerCopyTo",CLibxml2Tester::ChunkContainer_CopyToL),       
        ENTRY("ChunkContainerClone",CLibxml2Tester::ChunkContainer_CloneL),
        ENTRY("ChunkContainerAdoptNode",CLibxml2Tester::ChunkContainer_AdoptNodeL),        
        ENTRY("ChunkContainerSetAsFirstSibling",CLibxml2Tester::ChunkContainer_SetAsFirstSiblingL),
        ENTRY("ChunkContainerSetAsLastSibling",CLibxml2Tester::ChunkContainer_SetAsLastSiblingL),
        ENTRY("ChunkContainerMoveBeforeSibling",CLibxml2Tester::ChunkContainer_MoveBeforeSiblingL),
        ENTRY("ChunkContainerMoveAfterSibling",CLibxml2Tester::ChunkContainer_MoveAfterSiblingL),
        ENTRY("ChunkContainerMoveTo",CLibxml2Tester::ChunkContainer_MoveToL),
        ENTRY("ChunkContainerReconcileNamespaces",CLibxml2Tester::ChunkContainer_ReconcileNamespacesL),
        
        //filecontainer     
        ENTRY("FileContainerGetList",CLibxml2Tester::FileContainer_GetListL),
        ENTRY("FileContainerCid",CLibxml2Tester::FileContainer_CidL),
        ENTRY("FileContainerCidError",CLibxml2Tester::FileContainer_CidErrorL),
        ENTRY("FileContainerSize",CLibxml2Tester::FileContainer_SizeL),
        ENTRY("FileContainerFile",CLibxml2Tester::FileContainer_FileL),
        ENTRY("FileContainerNodeType",CLibxml2Tester::FileContainer_NodeTypeL),
        ENTRY("FileContainerRemove",CLibxml2Tester::FileContainer_RemoveL),
        ENTRY("FileContainerRemove2",CLibxml2Tester::FileContainer_Remove2L),
        ENTRY("FileContainerRemove3",CLibxml2Tester::FileContainer_Remove3L),
        ENTRY("FileContainerUnlink",CLibxml2Tester::FileContainer_UnlinkL),
        ENTRY("FileContainerReplaceWith",CLibxml2Tester::FileContainer_ReplaceWithL),
        ENTRY("FileContainerSubstituteFor",CLibxml2Tester::FileContainer_SubstituteForL),
        ENTRY("FileContainerCopy",CLibxml2Tester::FileContainer_CopyL),
        ENTRY("FileContainerCopy2",CLibxml2Tester::FileContainer_Copy2L),  
        ENTRY("FileContainerCopy3",CLibxml2Tester::FileContainer_Copy3L),       
        ENTRY("FileContainerCopy4",CLibxml2Tester::FileContainer_Copy4L),   
        ENTRY("FileContainerCopyTo",CLibxml2Tester::FileContainer_CopyToL),       
        ENTRY("FileContainerClone",CLibxml2Tester::FileContainer_CloneL),
        ENTRY("FileContainerAdoptNode",CLibxml2Tester::FileContainer_AdoptNodeL),        
        ENTRY("FileContainerSetAsFirstSibling",CLibxml2Tester::FileContainer_SetAsFirstSiblingL),
        ENTRY("FileContainerSetAsLastSibling",CLibxml2Tester::FileContainer_SetAsLastSiblingL),
        ENTRY("FileContainerMoveBeforeSibling",CLibxml2Tester::FileContainer_MoveBeforeSiblingL),
        ENTRY("FileContainerMoveAfterSibling",CLibxml2Tester::FileContainer_MoveAfterSiblingL),
        ENTRY("FileContainerMoveTo",CLibxml2Tester::FileContainer_MoveToL),
        ENTRY("FileContainerReconcileNamespaces",CLibxml2Tester::FileContainer_ReconcileNamespacesL),
        
        // Binary Container
        ENTRY( "BinaryContainer", CLibxml2Tester::BinaryContainerL),
        ENTRY( "PartialBinaryContainer", CLibxml2Tester::PartialBinaryContainerL ),
        ENTRY( "TestAppendContents", CLibxml2Tester::TestAppendContentsL),
        ENTRY("BinaryContainerGetList",CLibxml2Tester::BinaryContainer_GetListL),
        ENTRY("BinaryContainerGetList2",CLibxml2Tester::BinaryContainer_GetList2L),
        ENTRY("BinaryContainerCidError",CLibxml2Tester::BinaryContainer_CidErrorL),
        ENTRY("BinaryContainerRemove",CLibxml2Tester::BinaryContainer_RemoveL),
        ENTRY("BinaryContainerRemove2",CLibxml2Tester::BinaryContainer_Remove2L),
        ENTRY("BinaryContainerRemove3",CLibxml2Tester::BinaryContainer_Remove3L),
        ENTRY("BinaryContainerUnlink",CLibxml2Tester::BinaryContainer_UnlinkL),
        ENTRY("BinaryContainerReplaceWith",CLibxml2Tester::BinaryContainer_ReplaceWithL),
        ENTRY("BinaryContainerSubstituteFor",CLibxml2Tester::BinaryContainer_SubstituteForL),
        ENTRY("BinaryContainerCopy",CLibxml2Tester::BinaryContainer_CopyL),
        ENTRY("BinaryContainerCopy2",CLibxml2Tester::BinaryContainer_Copy2L),  
        ENTRY("BinaryContainerCopy3",CLibxml2Tester::BinaryContainer_Copy3L),       
        ENTRY("BinaryContainerCopy4",CLibxml2Tester::BinaryContainer_Copy4L),   
        ENTRY("BinaryContainerCopyTo",CLibxml2Tester::BinaryContainer_CopyToL),       
        ENTRY("BinaryContainerClone",CLibxml2Tester::BinaryContainer_CloneL),
        ENTRY("BinaryContainerAdoptNode",CLibxml2Tester::BinaryContainer_AdoptNodeL),        
        ENTRY("BinaryContainerSetAsFirstSibling",CLibxml2Tester::BinaryContainer_SetAsFirstSiblingL),
        ENTRY("BinaryContainerSetAsLastSibling",CLibxml2Tester::BinaryContainer_SetAsLastSiblingL),
        ENTRY("BinaryContainerMoveBeforeSibling",CLibxml2Tester::BinaryContainer_MoveBeforeSiblingL),
        ENTRY("BinaryContainerMoveAfterSibling",CLibxml2Tester::BinaryContainer_MoveAfterSiblingL),
        ENTRY("BinaryContainerMoveTo",CLibxml2Tester::BinaryContainer_MoveToL),
        ENTRY("BinaryContainerReconcileNamespaces",CLibxml2Tester::BinaryContainer_ReconcileNamespacesL),

        // XPath
        ENTRY( "Evaluate", CLibxml2Tester::EvaluateExpressionL ),   
        ENTRY( "SaveResult", CLibxml2Tester::SaveResultL ), 
        ENTRY( "ToNumber", CLibxml2Tester::ToNumberL ), 
        ENTRY( "ToBool", CLibxml2Tester::ToBoolL ),
        ENTRY( "ToString", CLibxml2Tester::ToStringL ),
        ENTRY( "Instances", CLibxml2Tester::InstanceL ),
        ENTRY( "ExtVector", CLibxml2Tester::ExtensionVectorL ),
        ENTRY( "ExtFunction", CLibxml2Tester::ExtensionFunctionL ),
        
        // Utils       
        ENTRY( "EscapeForXmlValue", CLibxml2Tester::UtilsEscapeForXmlValueL ),
        ENTRY( "XmlCharFromDes", CLibxml2Tester::UtilsXmlCharFromDesL ),
        ENTRY( "XmlCharFromDes8", CLibxml2Tester::UtilsXmlCharFromDes8L ),
       
        // Serialize Default
        ENTRY( "SerializeDefaultFile", CLibxml2Tester::SerializeDefaultFileL),
        
        // Serialize XOP
        ENTRY( "SerializeXOP", CLibxml2Tester::SerializeXOPL),
        ENTRY( "DirtySerializeXOP", CLibxml2Tester::DirtySerializeXOPL),
        ENTRY( "SerializeXopSoap", CLibxml2Tester::SerializeXopSoapL),
        ENTRY( "MultiSerializeXopSoap", CLibxml2Tester::MultiSerializeXopSoapL),
        ENTRY( "SerializeXopFile", CLibxml2Tester::SerializeXopFileL),
        ENTRY( "SerializeXopFileDirty", CLibxml2Tester::SerializeXopFileDirtyL),
        ENTRY( "SerializeXopDirtySoap", CLibxml2Tester::SerializeXopDirtySoapL),
        ENTRY( "SerializeXopBuffer", CLibxml2Tester::SerializeXopBufferL),
        ENTRY( "Serxop3diffCont", CLibxml2Tester::Serxop3diffContL),
        
        //Serialize GZIP
        ENTRY( "SerializeGZIP", CLibxml2Tester::SerializeGZIPL),
        ENTRY( "DecompressGZIP", CLibxml2Tester::DecompressGZIPL),
        ENTRY( "DecompressBufferGZIP", CLibxml2Tester::DecompressBufferGZIPL),
        ENTRY( "CompressBufferGZIP", CLibxml2Tester::CompressBufferGZIPL),
        ENTRY( "SerializeDirtyGzip", CLibxml2Tester::SerializeDirtyGzipL),
        
        //Serializer
        ENTRY( "TestSerializer", CLibxml2Tester::TestSerializerL),
        ENTRY( "TestSetOutputBuffer", CLibxml2Tester::TestSetOutputBufferL),
        ENTRY( "TestSetOutputFile", CLibxml2Tester::TestSetOutputFileL),
        ENTRY( "TestSetOutputStream", CLibxml2Tester::TestSetOutputStreamL),
        ENTRY( "TestSerializationOptions", CLibxml2Tester::TestSerializationOptionsL),
        ENTRY( "TestSerializeFile1", CLibxml2Tester::TestSerializeFile1L),
        ENTRY( "TestSerializeFile2", CLibxml2Tester::TestSerializeFile2L),
        ENTRY( "TestSerializeFile3", CLibxml2Tester::TestSerializeFile3L),
        ENTRY( "TestSerializeBuffer", CLibxml2Tester::TestSerializeBufferL),
        ENTRY( "TestSerializePartTree", CLibxml2Tester::TestSerializePartTreeL),
        ENTRY( "TestSerializeFileBuffer", CLibxml2Tester::TestSerializeFileBufferL),
        ENTRY( "TestSerializeChilds", CLibxml2Tester::TestSerializeChildsL),
        ENTRY( "TestSerializeFileConvinience", CLibxml2Tester::TestSerializeFileConvinienceL),
        ENTRY( "TestSerializeBufferConvinience", CLibxml2Tester::TestSerializeBufferConvinienceL),
        ENTRY( "TestMultipleSerializeToBuffer", CLibxml2Tester::TestMultipleSerializeToBufferL),
        ENTRY( "TestSerializationOptionsBuffer", CLibxml2Tester::TestSerializationOptionsBufferL),
        ENTRY( "TestContentType", CLibxml2Tester::TestContentTypeL),
     
        //Deserialize
        ENTRY( "Deserialize", CLibxml2Tester::DeserializeL),
        ENTRY( "Deserialize2", CLibxml2Tester::Deserialize2L),
        ENTRY( "DeserializeFromFile", CLibxml2Tester::DeserializeFromFileL),
        ENTRY( "DeserializeFromBuffer", CLibxml2Tester::DeserializeFromBufferL),
        ENTRY( "TestDeserializer", CLibxml2Tester::TestDeserializerL),
        ENTRY( "TestSetInputFile", CLibxml2Tester::TestSetInputFileL),
        ENTRY( "TestSetInputBuffer", CLibxml2Tester::TestSetInputBufferL),
        ENTRY( "TestSetContentHandler", CLibxml2Tester::TestSetContentHandlerL),
        ENTRY( "TestEnableFeature", CLibxml2Tester::TestEnableFeatureL),
        ENTRY( "TestDisableFeature", CLibxml2Tester::TestDisableFeatureL),
        ENTRY( "TestIsFeatureEnabled", CLibxml2Tester::TestIsFeatureEnabledL),
        ENTRY( "TestUseExternalData", CLibxml2Tester::TestUseExternalDataL),
        ENTRY( "TestExternalData", CLibxml2Tester::TestExternalDataL),
        ENTRY( "TestDesFileConvinience", CLibxml2Tester::TestDesFileConvinienceL),
        ENTRY( "TestDesBufferConvinience", CLibxml2Tester::TestDesBufferConvinienceL),
        ENTRY( "TestDeserializerBuffer", CLibxml2Tester::TestDeserializerBufferL),
        ENTRY( "TestSetParsingOptions1", CLibxml2Tester::TestSetParsingOptions1L),
        ENTRY( "DeserializeInfoset", CLibxml2Tester::DeserializeInfosetL),
        ENTRY( "Deserialize2Infoset", CLibxml2Tester::Deserialize2InfosetL),
        ENTRY( "DeserializeFromFileInfoset", CLibxml2Tester::DeserializeFromFileInfosetL),
        ENTRY( "DeserializeFromBufferInfoset", CLibxml2Tester::DeserializeFromBufferInfosetL),
        ENTRY( "TestDeserializerInfoset", CLibxml2Tester::TestDeserializerInfosetL),
        ENTRY( "TestSetInputFileInfoset", CLibxml2Tester::TestSetInputFileInfosetL),
        ENTRY( "TestSetInputBufferInfoset", CLibxml2Tester::TestSetInputBufferInfosetL),
        ENTRY( "TestSetContentHandlerInfoset", CLibxml2Tester::TestSetContentHandlerInfosetL),
        ENTRY( "TestEnableFeatureInfoset", CLibxml2Tester::TestEnableFeatureInfosetL),
        ENTRY( "TestDisableFeatureInfoset", CLibxml2Tester::TestDisableFeatureInfosetL),
        ENTRY( "TestIsFeatureEnabledInfoset", CLibxml2Tester::TestIsFeatureEnabledInfosetL),
        ENTRY( "TestUseExternalDataInfoset", CLibxml2Tester::TestUseExternalDataInfosetL),
        ENTRY( "TestExternalDataInfoset", CLibxml2Tester::TestExternalDataInfosetL),
        ENTRY( "TestDesFileConvinienceInfoset", CLibxml2Tester::TestDesFileConvinienceInfosetL),
        ENTRY( "TestDesBufferConvinienceInfoset", CLibxml2Tester::TestDesBufferConvinienceInfosetL),
        ENTRY( "TestDeserializerBufferInfoset", CLibxml2Tester::TestDeserializerBufferInfosetL),
        
        //DOMDeserialize
        ENTRY( "DOMDeserialize", CLibxml2Tester::DOMDeserializeL),
        ENTRY( "DOMDeserialize2", CLibxml2Tester::DOMDeserialize2L),
        ENTRY( "DOMDeserializeFromFile", CLibxml2Tester::DOMDeserializeFromFileL),
        ENTRY( "DOMDeserializeFromFile2", CLibxml2Tester::DOMDeserializeFromFile2L),
        ENTRY( "DOMDeserializeFromBuffer", CLibxml2Tester::DOMDeserializeFromBufferL),
        ENTRY( "TestDOMDeserializer", CLibxml2Tester::TestDOMDeserializerL),
        ENTRY( "TestDOMSetInputFile", CLibxml2Tester::TestDOMSetInputFileL),
        ENTRY( "TestDOMSetInputBuffer", CLibxml2Tester::TestDOMSetInputBufferL),
        ENTRY( "TestDOMUseExternalData", CLibxml2Tester::TestDOMUseExternalDataL),
        ENTRY( "TestDOMExternalData", CLibxml2Tester::TestDOMExternalDataL)
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

/******************************  TEST  **************************************/    

TBool CLibxml2Tester::CompareDescAndChar(const char* aString,TDesC8& aDesc)
{
	TInt strLen = strlen(aString);
	TInt desLen = aDesc.Length();
	if (strLen != desLen)
	{
		return FALSE;	
	}
	else
	{
		for(TInt i = 0; i < strLen; i++)
		{
			if (aString[i] != aDesc[i])
				return FALSE;
		}
	}	
	return TRUE;
}

TBool CLibxml2Tester::CompareDescAndChar(const TUint8* aString,TDesC8& aDesc)
{
	TInt strLen = strlen((char*)aString);
	TInt desLen = aDesc.Length();
	if (strLen != desLen)
	{
		return FALSE;	
	}
	else
	{
		for(TInt i = 0; i < strLen; i++)
		{
			if (aString[i] != aDesc[i])
				return FALSE;
		}
	}	
	return TRUE;
}

void CLibxml2Tester::SaveNode(RFile aFile,TXmlEngNode aNode)
	{
	_LIT8(KInfoOnStartEle,"Element: %S \r\n");
	_LIT8(KInfoOnAttribute,"\tAttribute \tname: %S \t value: %S \r\n");
	_LIT8(KInfoOnAttribute2,"Attribute \tname: %S \t value: %S \r\n");
	_LIT8(KInfoOnText,"Text: %S\r\n");

	TBuf8<256> info;
	
	if(aNode.NodeType() == TXmlEngNode::EElement)
		{
		TPtrC8 wsk = aNode.Name();
		info.Format(KInfoOnStartEle,&wsk);	
		aFile.Write(info);
			
		RXmlEngNodeList <TXmlEngAttr> array;
		if(aNode.HasAttributes())
			{
			aNode.AsElement().GetAttributes(array);
			while(array.HasNext())
				{
				TXmlEngAttr attr = array.Next();
				TPtrC8 wsk1 = attr.Name();
				TPtrC8 wsk2 = attr.Value();
				info.Format(KInfoOnAttribute,&wsk1,&wsk2);
				aFile.Write(info);
				}
		    array.Close();
			}
		}
	else if(aNode.NodeType() == TXmlEngNode::EAttribute)
		{
		TPtrC8 wsk1 = aNode.Name();
		TPtrC8 wsk2 = aNode.Value();
		info.Format(KInfoOnAttribute2,&wsk1,&wsk2);
		aFile.Write(info);
		}
	else if(aNode.NodeType() == TXmlEngNode::EText)
		{
		TPtrC8 wsk1 = aNode.AsText().Contents();
		info.Format(KInfoOnText,&wsk1);
		aFile.Write(info);
		}
	}

TBool CLibxml2Tester::CompareChars(const char* aString1,const char* aString2)
{
	TInt strLen = strlen(aString1);
	TInt desLen = strlen(aString2);
	if (strLen != desLen)
	{
		return FALSE;	
	}
	else
	{
		for(TInt i = 0; i < strLen; i++)
		{
			if (aString1[i] != aString2[i])
				return FALSE;			
		}
	}	
	return TRUE;
}

// -----------------------------------------------------------------------------
// CLibxml2Tester::CompareFilesL
// Comapare two files return true if the same
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::CompareFilesL( CStifItemParser& aItem )
{
    _LIT(KSourceFileErrorName,"No source file name.");
    _LIT(KPatternFileErrorName,"No pattern file name.");
    _LIT(KSkipWhiteSpace,"Skipping white spaces in files.");
    _LIT(KCompareFilesInfo,"Comparing files: %S and %S");
    _LIT(KSourceFileError,"Source file error.");
    _LIT(KPatternFileError,"Pattern file error.");
    _LIT(KComparePassed,"Files compare test PASSED.");
    _LIT(KCompareFailed,"Files compare test FAILED.");
    
    TInt nResult = KErrNone;
    
    TBool skipWhite = FALSE;
    TBool foundRes = FALSE;
    TBool foundRef = FALSE;
    
    RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
    
    RFile srcFile;
    RFile referenceFile;

    TFileName srcFileName;
	TFileName referenceFileName;
    
    TPtrC SrcFile;
    if ( aItem.GetNextString(SrcFile) != KErrNone )
    {
        TestModuleIf().Printf( infoNum++, KTEST_NAME, 
                                KSourceFileErrorName);
     	iLog->Log((TDesC) KSourceFileErrorName);
     	nResult = KErrGeneral;
    }
        
    TPtrC ReferenceFile;
    if ( aItem.GetNextString(ReferenceFile) != KErrNone )
    {
        TestModuleIf().Printf( infoNum++, KTEST_NAME, 
                                KPatternFileErrorName);
     	iLog->Log((TDesC)KPatternFileErrorName);
        nResult = KErrGeneral;
    }
        
    TPtrC string;
    if ( aItem.GetNextString(string) == KErrNone )
    {
        TestModuleIf().Printf( infoNum++, KTEST_NAME, 
                                KSkipWhiteSpace);
     	iLog->Log((TDesC)KSkipWhiteSpace);
        skipWhite = TRUE;
    }
    
    if ( nResult == KErrNone )
	{
		srcFileName.Copy(SrcFile);
		referenceFileName.Copy(ReferenceFile);

		TestModuleIf().Printf( infoNum++, KTEST_NAME, 
    						KCompareFilesInfo, &SrcFile, &ReferenceFile);	
		iLog->Log(KCompareFilesInfo, &SrcFile, &ReferenceFile);
		
	    if(srcFile.Open(fs, srcFileName, EFileStream|EFileRead) == KErrNone)
		{
			// Does reference file exist.
			if(referenceFile.Open(fs, referenceFileName, EFileStream|EFileRead) == KErrNone)
		    {
		        // Integer variables for compare to length of files (result and reference).
		        TInt resSize;
		        TInt refSize;

		        srcFile.Size(resSize);
		        referenceFile.Size(refSize);

	            // Next compare one letter at the time, but only if files have same length.
	            if(skipWhite)
		        {
			        TBuf8<1> resBuf;
			        TBuf8<1> refBuf;
			        nResult = KErrNone;
			        TInt j = 0;
			        TInt i = 0;
			        
			        //for(TInt i = 0; i < Size; i++)
			        while (TRUE)
				    {
				    	foundRes = FALSE;
				    	foundRef = FALSE;
				        // Read result file
				        while(i < (resSize + 1))
				        {
				        	i++;
				        	srcFile.Read(resBuf);
				        	resBuf.Trim();	
				        	if ( resBuf.Length() > 0)
				        	{
				        		foundRes = TRUE;
				        		break;
				        	}
				        }
						
						// Read reference file
				        while(j < (refSize + 1))
				        {
				        	j++;
				        	referenceFile.Read(refBuf);
				        	refBuf.Trim();
				        	if ( refBuf.Length() > 0)
				        	{
				        		foundRef = TRUE;
				        		break;
				        	}
				        }
				        
				        // Compare single letter at the time.
				        if( ( i < resSize ) && ( j < refSize ) && (resBuf[0] != refBuf[0]) )
				        {
					        nResult = KErrGeneral;
					        break;
					    }
					    if( (i == (resSize + 1)) && (j < refSize) && foundRef)
				    	{
					    	nResult = KErrGeneral;
					        break;
				    	}
					    if( (i < resSize) && (j == (refSize + 1)) && foundRes)
				    	{
					    	nResult = KErrGeneral;
					        break;
				    	}
				    	if ((i > resSize) && (j > refSize))
				    		break;
				    }
			    }
		        else
			    {
			        if (resSize != refSize)
			       		nResult = KErrGeneral;
			        else
		        	{
				        TBuf8<1> resBuf;
			        	TBuf8<1> refBuf;
			        	nResult = KErrNone;
			        	for(TInt i = 0; i < resSize; i++)
			        	{
				        	// Read result file
				        	srcFile.Read(resBuf);

					        // Read reference file
					        referenceFile.Read(refBuf);

					        // Compare single letter at the time.

					        if(resBuf[0] != refBuf[0])
					        {
						        nResult = KErrGeneral;
					    	    break;
				        	}
			        	}
		        	}
			    }
	            referenceFile.Close();
	            srcFile.Close();
			}
			else
			{
				nResult = KErrGeneral;
				TestModuleIf().Printf( infoNum++, KTEST_NAME, KPatternFileError);
	     		iLog->Log((TDesC)KPatternFileError);
			}
			srcFile.Close();
		}
		else
		{
			nResult = KErrGeneral;
			TestModuleIf().Printf( infoNum++, KTEST_NAME, KSourceFileError);
	     	iLog->Log((TDesC)KSourceFileError);		    
		}
			
	}
	
	CleanupStack::PopAndDestroy(&fs);
	
	if ( nResult == KErrNone)
		{
		TestModuleIf().Printf( infoNum++, KTEST_NAME,KComparePassed);
     	iLog->Log((TDesC)KComparePassed);		    
		}
	else
		{
		TestModuleIf().Printf( infoNum++, KTEST_NAME, KCompareFailed);
     	iLog->Log((TDesC)KCompareFailed);		    
		}

	return nResult;
    }    
