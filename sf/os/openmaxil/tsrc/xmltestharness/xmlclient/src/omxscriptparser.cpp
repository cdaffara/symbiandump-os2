/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "omxscriptparser.h"
#include <xml/parser.h>
#include <utf.h>
#include "parsemap.h"
//#include <openmax/il/khronos/v1_x/OMX_CoreExt_Nokia.h>

#if defined(NCP_COMMON_BRIDGE_FAMILY) && !defined(__WINSCW__)
// These definitions are from /epoc32/include/osi/video/VFM_Types.h
// We can't include the header itself because it does not obey symbian foundation
// rules on header file inclusion paths.
const TInt OMX_COLOR_FormatSTYUV420PackedSemiPlanarMB = (OMX_COLOR_FormatMax-1);
const TInt OMX_COLOR_FormatSTYUV422PackedSemiPlanarMB = (OMX_COLOR_FormatMax-2);
#endif

///////////////////////////////////////////////////////////////////////////////
// Temporary work-around for floating point issues on HREF 8500 ED hardware
// Using this macro, code that uses TReal will be #defined out if running on ED.
// TODO: Remove HREF_ED_WITHOUT_FLOATING_POINT and all of its conditional code
// once the floating point issue has been solved.
#if defined(NCP_COMMON_BRIDGE_FAMILY) && !defined(__WINSCW__)
#define HREF_ED_WITHOUT_FLOATING_POINT
#endif

using Xml::CParser;

COmxScriptParser* COmxScriptParser::NewL(RFs& aFs, const TDesC& aFilename, MOmxScriptIf& aCallback)
	{
	COmxScriptParser* self = new(ELeave) COmxScriptParser(aFs, aCallback);
	CleanupStack::PushL(self);
	self->ConstructL(aFilename);
	CleanupStack::Pop(self);
	return self;
	}

COmxScriptParser::~COmxScriptParser()
	{
	iFilename.Close();
	}
	
COmxScriptParser::COmxScriptParser(RFs& aFs, MOmxScriptIf& aCallback):
iFs(aFs), iCallback(aCallback)
	{

	}

void COmxScriptParser::ConstructL(const TDesC& aFilename)
	{
	User::LeaveIfError(iFilename.Create(aFilename.Length()));
	iFilename = aFilename;
	}
	
TBool COmxScriptParser::RunScriptL(const TDesC& aSectionName)
	{
	CParser* parser = CParser::NewLC(_L8("text/xml"), *this);
	iSectionName = &aSectionName;
	iInTest = EFalse;
	iFoundTest = EFalse;
	iCallbackAborted = EFalse;
	TInt error = KErrNone;
	TInt size = iFilename.Length();
	
	TRAP(error, Xml::ParseL(*parser, iFs, iFilename));
	CleanupStack::PopAndDestroy(parser);
	
	if(error == KErrAbort && iCallbackAborted)
		{		
		return EFalse;
		}
	User::LeaveIfError(error);
	if(!iFoundTest)
		{
		User::Leave(KErrNotFound);
		}
	return ETrue;
	}

void COmxScriptParser::OnStartDocumentL(const RDocumentParameters& /*aDocParam*/, TInt /*aErrorCode*/)
	{
	// do nothing
	}

void COmxScriptParser::OnEndDocumentL(TInt /*aErrorCode*/)
	{
	// do nothing
	}

void COmxScriptParser::OnContentL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
	{
	// do nothing
	}

void COmxScriptParser::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, 
		TInt /*aErrorCode*/)
	{
	// do nothing
	}

void COmxScriptParser::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt /*aErrorCode*/)
	{
	// do nothing
	}

void COmxScriptParser::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
	{
	// do nothing
	}

void COmxScriptParser::OnSkippedEntityL(const RString& /*aName*/, TInt /*aErrorCode*/)
	{
	User::Invariant();
	}

void COmxScriptParser::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, 
		TInt /*aErrorCode*/)
	{
	// do nothing
	}

void COmxScriptParser::OnError(TInt aErrorCode)
	{
	(void) aErrorCode;	// avoid unused parameter warning. useful to give parameter a name when debugging. 
	User::Invariant();
	}

TAny* COmxScriptParser::GetExtendedInterface(const TInt32 /*aUid*/)
	{
	return 0;
	}

void COmxScriptParser::OnStartElementL(const RTagInfo& aElement,
		                               const RAttributeArray& aAttributes, 
		                               TInt /*aErrorCode*/)
	{
	const TDesC8& elemName = aElement.LocalName().DesC();
	if(!iInTest)
		{
		if(elemName == _L8("Test"))
			{
			const TDesC8* testName8Bit = FindAttribute(aAttributes, _L8("name"));
			if(testName8Bit)
				{
				HBufC* testName = HBufC::NewLC(testName8Bit->Length());
				testName->Des().Copy(*testName8Bit);
				if(*testName == *iSectionName)
					{
					__ASSERT_ALWAYS(!iFoundTest, User::Invariant());
					iFoundTest = ETrue;
					iInTest = ETrue;
					}
				CleanupStack::PopAndDestroy(testName);
				}
			}
		}
	else
		{
			// to trace out which command has been passed from xml file
			TBuf<32> elemNameCopy;
			elemNameCopy.Copy(elemName);
			TBuf<44> message;
			message.Append(_L("command: "));
			message.Append(elemNameCopy);
			iCallback.MosParseError(message);
						
		if(elemName == _L8("LoadComponent"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& name = FindAttributeL(aAttributes, _L8("name"));
			
			TBool baseEnabled = EFalse;
			const TDesC8* baseImpl = NULL;
			const TDesC8* baseEnabledAttr = FindAttribute(aAttributes, _L8("baseprofilesupport"));
			if (baseEnabledAttr != NULL)
			    {
			    baseEnabled = ParseBoolean(*baseEnabledAttr);
			    baseImpl = &(FindAttributeL(aAttributes, _L8("baseprofileimpl")));
			    }
			
			TBool loadInCoreServerThread = EFalse;
			const TDesC8* loadInCoreServerThreadAttr = FindAttribute(aAttributes, _L8("loadincoreserverthread"));
			if(loadInCoreServerThreadAttr != NULL)
			    {
			    loadInCoreServerThread = ParseBoolean(*loadInCoreServerThreadAttr);
			    }
			
			CheckForAbortL(iCallback.MosLoadComponentL(comp, name, baseEnabled, baseImpl, loadInCoreServerThread));
			}
		else if(elemName == _L8("LogAllEvents")) //utility
		    {
		    CheckForAbortL(iCallback.MosLogAllEventsL());
		    }

		else if(elemName == _L8("SetSensorModeType")) //camera
            {
            const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
            
            TPtrC8 comp;
            TInt port = 0;
            ParseCompPortL(compPort, comp, port);

            TInt frameRate = ParseOptionalIntL(aAttributes, _L8("frameRate"), -1);
            
            const TDesC8& oneShotStr = FindAttributeL(aAttributes, _L8("oneShot"));
            TBool oneShot = ParseBooleanL(oneShotStr);
            TInt width = ParseOptionalIntL(aAttributes, _L8("width"), -1);
            TInt height = ParseOptionalIntL(aAttributes, _L8("height"), -1);
            
            CheckForAbortL(iCallback.MosSetSensorModeTypeL(comp, port, frameRate, oneShot, width, height));
            }
		else if(elemName == _L8("DeleteFile")) //utility
            {
			const TDesC8& fileName8 = FindAttributeL(aAttributes, _L8("filename"));

			HBufC* fileName = HBufC::NewLC(fileName8.Length() + 1);
			fileName->Des().Copy(fileName8);
			fileName->Des().ZeroTerminate();

			TBool fileMustExist = EFalse;
			const TDesC8* fileMustExistAttr = FindAttribute(aAttributes, _L8("fileMustExist"));
			if (fileMustExistAttr != NULL)
				{
				fileMustExist = ParseBoolean(*fileMustExistAttr);
				}

			CheckForAbortL(iCallback.MosDeleteFileL(*fileName, fileMustExist));
			CleanupStack::PopAndDestroy(fileName);
			}
		else if(elemName == _L8("SetupTunnel"))
			{
			const TDesC8& source = FindAttributeL(aAttributes, _L8("output"));
			const TDesC8& sink = FindAttributeL(aAttributes, _L8("input"));

			const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectedomxerr"));
		    OMX_ERRORTYPE expectedErrorInt = OMX_ErrorNone;
			if (expectedError)
			    {
			    expectedErrorInt = ParseOmxErrorCode(*expectedError);
			    }

			TPtrC8 sourceComp;
			TPtrC8 sinkComp;
			TInt sourcePort = 0;
			TInt sinkPort = 0;
			_LIT8(KTemp, "");
			const TDesC8& temp = KTemp;
			if (source.Compare(temp))
			    {
			    ParseCompPortL(source, sourceComp, sourcePort);
			    }
			if(sink.Compare(temp))
			    {
			    ParseCompPortL(sink, sinkComp, sinkPort);
			    }			
			CheckForAbortL(iCallback.MosSetupTunnel(sourceComp, sourcePort, sinkComp, sinkPort, expectedErrorInt));
			}

		else if (elemName == _L8("SetupNonTunnel"))
			{
			const TDesC8& source = FindAttributeL(aAttributes, _L8("output"));
			const TDesC8& sink = FindAttributeL(aAttributes, _L8("input"));
			const TDesC8& supplierDes = FindAttributeL(aAttributes, _L8("supplier"));
			TPtrC8 sourceComp;
			TPtrC8 sinkComp;
			TInt sourcePort;
			TInt sinkPort;
			ParseCompPortL(source, sourceComp, sourcePort);
			ParseCompPortL(sink, sinkComp, sinkPort);
			OMX_BUFFERSUPPLIERTYPE supplier = ParseOmxSupplierL(supplierDes, ETrue);
			CheckForAbortL(iCallback.MosSetupNonTunnel(sourceComp, sourcePort, sinkComp, sinkPort, supplier));
			}
		else if (elemName == _L8("SetBufferForPort"))
		    {
		    const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
            TPtrC8 comp;
            TInt port;
            ParseCompPortL(compPort, comp, port);
            
            const TDesC8& filename8Bit = FindAttributeL(aAttributes, _L8("filename"));
            HBufC* filename = HBufC::NewLC(filename8Bit.Length());
            filename->Des().Copy(filename8Bit);
            TInt headerlength = KErrNone;
            headerlength = ParseOptionalIntL(aAttributes, _L8("headerlength"), -1);
             
            const TDesC8* supplierDes = FindAttribute(aAttributes, _L8("supplier"));
            OMX_BUFFERSUPPLIERTYPE supplier = OMX_BufferSupplyUnspecified;
            if(supplierDes)
                {
                supplier = ParseOmxSupplierL(*supplierDes);
                }
            CheckForAbortL(iCallback.MosSetupBufferForPortL(comp,port,*filename,headerlength,supplier));
            CleanupStack::PopAndDestroy(filename);
		    }
		else if(elemName == _L8("SetWindow"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			CheckForAbortL(iCallback.MosSetWindowL(comp));
			}
		else if(elemName == _L8("AllTransition"))
			{
			const TDesC8& stateDes = FindAttributeL(aAttributes, _L8("state"));
			OMX_STATETYPE state = ParseOmxStateL(stateDes);
			const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectedomxerr"));
			OMX_ERRORTYPE expectedErrorInt = OMX_ErrorNone;
			const TDesC8* orderDes = FindAttribute(aAttributes, _L8("order"));
			TTransitionOrder order = ELoadOrder;
			if(orderDes != NULL)
				{
				if(*orderDes == _L8("auto"))
					{
					order = EAutoOrder;
					}
				else
					{
					User::Leave(KErrArgument);
					}
				}
			if (expectedError)
				{
				expectedErrorInt = ParseOmxErrorCode(*expectedError);
				}
			CheckForAbortL(iCallback.MosAllTransitionL(state, expectedErrorInt, order));
			}
		else if(elemName == _L8("Transition"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& stateDes = FindAttributeL(aAttributes, _L8("state"));
			TBool async_behaviour = ParseOptionalBooleanL(aAttributes, _L8("async"), EFalse);
			OMX_STATETYPE state = ParseOmxStateL(stateDes);
			CheckForAbortL(iCallback.MosTransition(comp, state, async_behaviour));
			}
		else if(elemName == _L8("FailingTransition"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& stateDes = FindAttributeL(aAttributes, _L8("state"));
			const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectederr"));
			OMX_ERRORTYPE expectedErrorInt = OMX_ErrorNone;
			if (expectedError)
			    {
			    expectedErrorInt = ParseOmxErrorCode(*expectedError);
			    }
			OMX_STATETYPE state = ParseOmxStateL(stateDes);
			CheckForAbortL(iCallback.MosFailingTransition(comp, state, expectedErrorInt));
			}
		else if(elemName == _L8("WaitEOS"))
			{
			const TDesC8* comp = FindAttribute(aAttributes, _L8("comp"));
			if(comp == NULL)
				{
				CheckForAbortL(iCallback.MosWaitEOS());
				}
			else
				{
				CheckForAbortL(iCallback.MosWaitEOS(*comp));
				}
			}
		else if(elemName == _L8("Wait"))
			{
			const TDesC8& delay = FindAttributeL(aAttributes, _L8("delaytime"));
			TLex8 lex(delay);
			TInt delayInt;
			User::LeaveIfError(lex.Val(delayInt));			
			CheckForAbortL(iCallback.MosWaitL(delayInt));
			}
		else if(elemName == _L8("WaitForAllEvents"))
			{
			CheckForAbortL(iCallback.MosWaitForAllEventsL());
			}
		else if(elemName == _L8("SetFilename"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& filename8Bit = FindAttributeL(aAttributes, _L8("filename"));
			HBufC* filename = HBufC::NewLC(filename8Bit.Length());
			filename->Des().Copy(filename8Bit);
			CheckForAbortL(iCallback.MosSetFilename(comp, *filename));
			CleanupStack::PopAndDestroy(filename);
			}
	   else if(elemName == _L8("SetFilename_Bellagio"))
        {
		   const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
           const TDesC8& filename8Bit = FindAttributeL(aAttributes, _L8("filename"));
           char filepath[100];
           int i;
           for(i=0;i<filename8Bit.Length();i++)
            filepath[i]=filename8Bit.Ptr()[i];
            filepath[i]='\0';
           CheckForAbortL(iCallback.MosSetFilename_bellagio(comp, filepath));
          //CleanupStack::PopAndDestroy(filename8Bit);
         }
		 else if(elemName == _L8("SetDroppedFrameEvent"))
		    {
		    const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
		    const TDesC8& action = FindAttributeL(aAttributes, _L8("action"));
		                 
		    CheckForAbortL(iCallback.MosSetDroppedFrameEvent(comp, action));
		    }
		else if(elemName == _L8("SetBadFilename"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));	
			CheckForAbortL(iCallback.MosSetBadFilename(comp));

			}
		else if(elemName == _L8("GetFilename"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& filename8Bit = FindAttributeL(aAttributes, _L8("filename"));
			HBufC* filename = HBufC::NewLC(filename8Bit.Length());
			filename->Des().Copy(filename8Bit);

			const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectedomxerr"));
			OMX_ERRORTYPE expectedErrorInt = OMX_ErrorNone;
			if (expectedError)
			    {
			    expectedErrorInt = ParseOmxErrorCode(*expectedError);
			    }
			CheckForAbortL(iCallback.MosGetFilename(comp, *filename, expectedErrorInt));
			CleanupStack::PopAndDestroy(filename);
			}
		
		else if(elemName == _L8("SetBufferCount"))
			{
			const TDesC8& port = FindAttributeL(aAttributes, _L8("port"));
			const TDesC8& count = FindAttributeL(aAttributes, _L8("count"));
			const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectedomxerr"));
		    OMX_ERRORTYPE expectedErrorInt = OMX_ErrorNone;
			if (expectedError)
			    {
			    expectedErrorInt = ParseOmxErrorCode(*expectedError);
			    }
			
			TPtrC8 comp;
			TInt portIndex = 0;
			ParseCompPortL(port, comp, portIndex);
			TLex8 lex(count);
			TInt countInt;
			User::LeaveIfError(lex.Val(countInt));
			CheckForAbortL(iCallback.MosSetBufferCount(comp, portIndex, countInt, expectedErrorInt));
			}
		else if(elemName == _L8("CompareFiles"))
			{
			const TDesC8& file18Bit = FindAttributeL(aAttributes, _L8("file1"));
			const TDesC8& file28Bit = FindAttributeL(aAttributes, _L8("file2"));
			TBuf<64> file1, file2;
			file1.Copy(file18Bit);
			file2.Copy(file28Bit);
			CheckForAbortL(iCallback.MosCompareFilesL(file1, file2));
			}
        else if(elemName == _L8("FilterAndCompareFiles"))
            {
            
            const TDesC8& file18Bit = FindAttributeL(aAttributes, _L8("file1"));
            const TDesC8& file28Bit = FindAttributeL(aAttributes, _L8("file2"));
            TBuf<64> file1, file2;
            file1.Copy(file18Bit);
            file2.Copy(file28Bit);
            
            const TDesC8* f1filter1 = FindAttribute(aAttributes, _L8("file1filter1"));
            const TDesC8* f1filter2 = FindAttribute(aAttributes, _L8("file1filter2"));
            const TDesC8* f1filter3 = FindAttribute(aAttributes, _L8("file1filter3"));

            const TDesC8* f2filter1 = FindAttribute(aAttributes, _L8("file2filter1"));
            const TDesC8* f2filter2 = FindAttribute(aAttributes, _L8("file2filter2"));
            const TDesC8* f2filter3 = FindAttribute(aAttributes, _L8("file2filter3"));
            
            CheckForAbortL(
                    iCallback.MosFilterAndCompareFilesL(file1, f1filter1 ? *f1filter1 : KNullDesC8, f1filter2 ? *f1filter2 : KNullDesC8, f1filter3 ? *f1filter3 : KNullDesC8,
                                                file2, f2filter1 ? *f2filter1 : KNullDesC8, f2filter2 ? *f2filter2 : KNullDesC8, f2filter3 ? *f2filter3 : KNullDesC8)
                );
            }		
		else if(elemName == _L8("BufferSupplierOverride"))
			{
			const TDesC8& output = FindAttributeL(aAttributes, _L8("output"));
			const TDesC8& input = FindAttributeL(aAttributes, _L8("input"));
			const TDesC8& supplierDes = FindAttributeL(aAttributes, _L8("supplier"));
			const TDesC8* expectedError1 = FindAttribute(aAttributes, _L8("expectedomxerr1"));
		    OMX_ERRORTYPE expectedError1Int = OMX_ErrorNone;
			if (expectedError1)
			    {
			    expectedError1Int = ParseOmxErrorCode(*expectedError1);
			    }
			const TDesC8* expectedError2 = FindAttribute(aAttributes, _L8("expectedomxerr2"));
			OMX_ERRORTYPE expectedError2Int = OMX_ErrorNone;
			if (expectedError2)
			    {
			    expectedError2Int = ParseOmxErrorCode(*expectedError2);
			    }
			TPtrC8 sourceComp;
			TPtrC8 sinkComp;
			TInt sourcePort = 0;
			TInt sinkPort = 0;
			ParseCompPortL(output, sourceComp, sourcePort);
			ParseCompPortL(input, sinkComp, sinkPort);
			OMX_BUFFERSUPPLIERTYPE supplier = ParseOmxSupplierL(supplierDes);
			CheckForAbortL(iCallback.MosBufferSupplierOverrideL(sourceComp, sourcePort, sinkComp, sinkPort, supplier, expectedError1Int, expectedError2Int));
			}
        else if(elemName == _L8("SetCameraOneShot"))
            {
            //TODO DL
            const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
            TInt isOneShot = ParseOptionalIntL(aAttributes, _L8("isoneshot"), 1);
            OMX_ERRORTYPE expectedErrorInt = OMX_ErrorNone;
            CheckForAbortL(iCallback.MosSetCameraOneShotL(comp, isOneShot, expectedErrorInt));
            }
        else if(elemName == _L8("SetCameraCapture"))
            {
            //TODO DL
            const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
            TInt isCapturing = ParseOptionalIntL(aAttributes, _L8("iscapturing"), 1);
            const TDesC8& port = FindAttributeL(aAttributes, _L8("port"));
            TPtrC8 comp1;
            TInt portIndex = 0;
            ParseCompPortL(port, comp1, portIndex);
            
            OMX_ERRORTYPE expectedErrorInt = OMX_ErrorNone;
            CheckForAbortL(iCallback.MosSetCameraCaptureL(comp, portIndex, isCapturing, expectedErrorInt));
            }
		else if(elemName == _L8("SetVideoPortDef"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
			TInt width = ParseOptionalIntL(aAttributes, _L8("width"), -1);
			TInt height = ParseOptionalIntL(aAttributes, _L8("height"), -1);
			OMX_COLOR_FORMATTYPE colorFormat = OMX_COLOR_FormatMax;
			OMX_COLOR_FORMATTYPE* colorFormatPtr = NULL;
			const TDesC8* colorFormatDes = FindAttribute(aAttributes, _L8("colorFormat"));
			if(colorFormatDes)
				{
				colorFormat = ParseOmxColorFormatL(*colorFormatDes);
				colorFormatPtr = &colorFormat;
				}
			OMX_VIDEO_CODINGTYPE codingType = OMX_VIDEO_CodingMax;
			OMX_VIDEO_CODINGTYPE* codingTypePtr = NULL;
			const TDesC8* codingDes = FindAttribute(aAttributes, _L8("codingType"));
			if(codingDes)
				{
				codingType = ParseOmxVideoCodingL(*codingDes);
				codingTypePtr = &codingType;
				}
			TInt stride = ParseOptionalIntL(aAttributes, _L8("stride"), -1);

			const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectedomxerr"));
		    OMX_ERRORTYPE expectedErrorInt = OMX_ErrorNone;
			if (expectedError)
			    {
			    expectedErrorInt = ParseOmxErrorCode(*expectedError);
			    }
			
#ifdef HREF_ED_WITHOUT_FLOATING_POINT
			CheckForAbortL(iCallback.MosSetVideoPortDefL(comp, port, width, height, colorFormatPtr, codingTypePtr, stride, 0, expectedErrorInt));
#else
			TReal32 fps = ParseOptionalRealL(aAttributes, _L8("fps"), -1);
			CheckForAbortL(iCallback.MosSetVideoPortDefL(comp, port, width, height, colorFormatPtr, codingTypePtr, stride, fps, expectedErrorInt));
#endif //HREF_ED_WITHOUT_FLOATING_POINT
			}
		else if(elemName == _L8("ExpectEvent"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& eventDes = FindAttributeL(aAttributes, _L8("event"));
			const TDesC8& nData1Des = FindAttributeL(aAttributes, _L8("nData1"));
			const TDesC8& nData2Des = FindAttributeL(aAttributes, _L8("nData2"));
			TLex8 lex(nData1Des);
			TUint32 nData1;
			OMX_EVENTTYPE event = ParseOmxEventL(eventDes);
			switch(event)
				{
			// ParseOmxErrorL and ParseOmxCommandL will also parse literal integers
			case OMX_EventError:
				nData1 = static_cast<TUint32>(ParseOmxErrorL(nData1Des));
				break;
			case OMX_EventCmdComplete:
				nData1 = static_cast<TUint32>(ParseOmxCommandL(nData1Des));
				break;
			default:
				nData1 = ParseUint32L(nData1Des);
				break;
				}
			TUint32 nData2 = ParseUint32L(nData2Des);
			CheckForAbortL(iCallback.MosExpectEventL(comp, event, nData1, nData2));
			}
		else if(elemName == _L8("CheckState"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& stateDes = FindAttributeL(aAttributes, _L8("state"));
			OMX_STATETYPE state = ParseOmxStateL(stateDes);
			CheckForAbortL(iCallback.MosCheckStateL(comp, state));
			}
		else if(elemName == _L8("CheckVideoPortDef"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
			const TDesC8& widthDes = FindAttributeL(aAttributes, _L8("width"));
			const TDesC8& heightDes = FindAttributeL(aAttributes, _L8("height"));
			TInt width;
			TInt height;
			TLex8 lex(widthDes);
			User::LeaveIfError(lex.Val(width));
			lex = TLex8(heightDes);
			User::LeaveIfError(lex.Val(height));
			const TDesC8& codingDes = FindAttributeL(aAttributes, _L8("coding"));
			OMX_VIDEO_CODINGTYPE coding = ParseOmxVideoCodingL(codingDes);
			const TDesC8& colorFormatDes = FindAttributeL(aAttributes, _L8("colorFormat"));
			OMX_COLOR_FORMATTYPE colorFormat = ParseOmxColorFormatL(colorFormatDes);
			CheckForAbortL(iCallback.MosCheckVideoPortDefL(comp, port, width, height, coding, colorFormat));
			}
		else if(elemName == _L8("CheckMetaData"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
			const TDesC8& scopeDes = FindAttributeL(aAttributes, _L8("scope"));			
			OMX_METADATASCOPETYPE scope = ParseOmxScopeTypeL(scopeDes);
			const TDesC8& atomType = FindAttributeL(aAttributes, _L8("atomType"));						
			const TDesC8& atomIndexDes = FindAttributeL(aAttributes, _L8("atomIndex"));
			TLex8 lex(atomIndexDes);
			TUint32 atomIndex = ParseUint32L(atomIndexDes);				
			const TDesC8& data = FindAttributeL(aAttributes, _L8("data"));
			CheckForAbortL(iCallback.MosCheckMetaDataL(comp, port, scope, atomType, atomIndex, data));			
			}
		else if(elemName == _L8("GetParameterUnknownType"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
			const TDesC8& scopeDes = FindAttributeL(aAttributes, _L8("scope"));			
			OMX_METADATASCOPETYPE scope = ParseOmxScopeTypeL(scopeDes);
			const TDesC8& atomType = FindAttributeL(aAttributes, _L8("atomType"));						
			const TDesC8& atomIndexDes = FindAttributeL(aAttributes, _L8("atomIndex"));
			TLex8 lex(atomIndexDes);
			TUint32 atomIndex = ParseUint32L(atomIndexDes);				
			const TDesC8& data = FindAttributeL(aAttributes, _L8("data"));
			CheckForAbortL(iCallback.MosGetParameterUnknownIndexTypeL(comp, port, scope, atomType, atomIndex, data));			
			}		
		else if(elemName == _L8("SetParameterUnknownType"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
			const TDesC8& scopeDes = FindAttributeL(aAttributes, _L8("scope"));			
			OMX_METADATASCOPETYPE scope = ParseOmxScopeTypeL(scopeDes);
			const TDesC8& atomType = FindAttributeL(aAttributes, _L8("atomType"));						
			const TDesC8& atomIndexDes = FindAttributeL(aAttributes, _L8("atomIndex"));
			TLex8 lex(atomIndexDes);
			TUint32 atomIndex = ParseUint32L(atomIndexDes);				
			const TDesC8& data = FindAttributeL(aAttributes, _L8("data"));
			CheckForAbortL(iCallback.MosSetParameterUnknownIndexTypeL(comp, port, scope, atomType, atomIndex, data));			
			}						
		else if(elemName == _L8("DisablePort"))
			{
			const TDesC8& port = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt portIndex;
			ParseCompPortL(port, comp, portIndex);
			CheckForAbortL(iCallback.MosDisablePort(comp, portIndex));
			}
		else if(elemName == _L8("EnablePort"))
			{
			const TDesC8& port = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt portIndex;
			ParseCompPortL(port, comp, portIndex);
			CheckForAbortL(iCallback.MosEnablePort(comp, portIndex));
			}
		else if(elemName == _L8("IgnoreEvent"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& eventDes = FindAttributeL(aAttributes, _L8("event"));
			const TDesC8& nData1Des = FindAttributeL(aAttributes, _L8("nData1"));
			const TDesC8& nData2Des = FindAttributeL(aAttributes, _L8("nData2"));
			TLex8 lex(nData1Des);
			TUint32 nData1;
			OMX_EVENTTYPE event = ParseOmxEventL(eventDes);
			switch(event)
				{
			// ParseOmxErrorL and ParseOmxCommandL will also parse literal integers
			case OMX_EventError:
				nData1 = static_cast<TUint32>(ParseOmxErrorL(nData1Des));
				break;
			case OMX_EventCmdComplete:
				nData1 = static_cast<TUint32>(ParseOmxCommandL(nData1Des));
				break;
			default:
				nData1 = ParseUint32L(nData1Des);
				break;
				}
			TUint32 nData2 = ParseUint32L(nData2Des);
			CheckForAbortL(iCallback.MosIgnoreEventL(comp, event, nData1, nData2));
			}
		else if(elemName == _L8("SetAudioPortDef"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
			OMX_AUDIO_CODINGTYPE codingType = OMX_AUDIO_CodingMax;
			OMX_AUDIO_CODINGTYPE* codingTypePtr = NULL;
			const TDesC8* codingDes = FindAttribute(aAttributes, _L8("codingType"));
			if(codingDes)
				{
				codingType = ParseOmxAudioCodingL(*codingDes);
				codingTypePtr = &codingType;
				}

			const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectedomxerr"));
			OMX_ERRORTYPE expectedErrorInt = OMX_ErrorNone;
			if (expectedError)
			    {
			    expectedErrorInt = ParseOmxErrorCode(*expectedError);
			    }

			CheckForAbortL(iCallback.MosSetAudioPortDefL(comp, port, codingTypePtr, expectedErrorInt));
			}
		else if(elemName == _L8("SetAACProfile"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
			// TODO allow some values to be unspecified (preserved from existing settings)
			TInt channels = ParseUint32L(FindAttributeL(aAttributes, _L8("channels")));
			TInt samplingrate = ParseUint32L(FindAttributeL(aAttributes, _L8("samplingrate")));
			TInt bitrate = ParseUint32L(FindAttributeL(aAttributes, _L8("bitrate")));
			TInt audioBandwidth = ParseUint32L(FindAttributeL(aAttributes, _L8("bandwidth")));
			TInt frameLength = ParseUint32L(FindAttributeL(aAttributes, _L8("frameLength")));
			// TODO allow multiple flags in disjunctive form
			TInt aacTools = ParseUint32L(FindAttributeL(aAttributes, _L8("aacTools")));
			TInt aacerTools = ParseUint32L(FindAttributeL(aAttributes, _L8("aacerTools")));
			OMX_AUDIO_AACPROFILETYPE profile = ParseOmxAACProfileL(FindAttributeL(aAttributes, _L8("profile")));
			OMX_AUDIO_AACSTREAMFORMATTYPE streamFormat = ParseOmxAACStreamFormatL(FindAttributeL(aAttributes, _L8("streamFormat")));
			OMX_AUDIO_CHANNELMODETYPE channelMode = ParseOmxAudioChannelModeL(FindAttributeL(aAttributes, _L8("channelMode")));
			
			CheckForAbortL(iCallback.MosSetAACProfileL(comp, port, channels, samplingrate, bitrate, audioBandwidth, frameLength, aacTools, aacerTools, profile, streamFormat, channelMode));
			}
		else if(elemName == _L8("SetPcmAudioPortDef"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
			TInt channels = ParseOptionalIntL(aAttributes, _L8("channels"), -1);
			TInt samplingrate = ParseOptionalIntL(aAttributes, _L8("samplingrate"), -1);
			TInt bitspersample = ParseOptionalIntL(aAttributes, _L8("bitspersample"), -1);
			OMX_NUMERICALDATATYPE numData = OMX_NumercialDataMax;
 			const TDesC8* des = FindAttribute(aAttributes, _L8("numericalData"));
 			if(des != NULL)
 				{
 				numData = ParseNumericalDataL(*des);
 				}
			else
			    {
			    numData = static_cast<OMX_NUMERICALDATATYPE>(-1);
			    }
 			OMX_ENDIANTYPE endian = OMX_EndianMax;
 			des = FindAttribute(aAttributes, _L8("endian"));
 			if(des != NULL)
 				{
 				endian = ParseEndianL(*des);
			    }
			else
			   {
			   endian = static_cast<OMX_ENDIANTYPE>(-1);
 				}
 			OMX_BOOL* interleaved = NULL;
 			OMX_BOOL interleavedData;
 			des = FindAttribute(aAttributes, _L8("interleaved"));
 			if(des != NULL)
 				{
 				interleavedData = ParseBoolL(*des);
 				interleaved = &interleavedData;
 				}
            const TDesC8* encoding = FindAttribute(aAttributes, _L8("encoding"));
 			CheckForAbortL(iCallback.MosSetPcmAudioPortDefL(comp, port, channels, samplingrate, bitspersample, numData, endian, interleaved, encoding));
			}
		else if(elemName == _L8("SetAudioMute"))
		    {
		    const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
		    TPtrC8 comp;
		    TInt port;
		    ParseCompPortL(compPort, comp, port);
		    TBool mute = EFalse;
		    const TDesC8* muteAttr = FindAttribute(aAttributes, _L8("mute"));
		    if (muteAttr != NULL)
		        {
		        mute = ParseBooleanL(*muteAttr);
		        }
		    else
		        {
		        User::Leave(KErrArgument);
		        }
		    CheckForAbortL(iCallback.MosSetConfigAudioMuteL(comp, port, mute));
		    }
		else if(elemName == _L8("CheckAudioMute"))
		    {
		    const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
		    TPtrC8 comp;
		    TInt port;
		    ParseCompPortL(compPort, comp, port);
		    TBool mute = EFalse;
		    const TDesC8* muteAttr = FindAttribute(aAttributes, _L8("mute"));
		    if (muteAttr != NULL)
		        {
		        mute = ParseBooleanL(*muteAttr);
		        }
		    else
		        {
		        User::Leave(KErrArgument);
		        }
		    CheckForAbortL(iCallback.MosCheckConfigAudioMuteL(comp, port, mute));
		    }   
		else if(elemName == _L8("SetAudioVolume"))
            {
            const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
            TPtrC8 comp;
            TInt port;
            ParseCompPortL(compPort, comp, port);
            TBool linear = EFalse;
            const TDesC8* linearScaleAttr = FindAttribute(aAttributes, _L8("linearscale"));
            if (linearScaleAttr != NULL)
                {
                linear = ParseBooleanL(*linearScaleAttr);
                }
            TInt minVolume = ParseOptionalIntL(aAttributes, _L8("min"), -1);
            TInt maxVolume = ParseOptionalIntL(aAttributes, _L8("max"), -1);
            TInt volume = ParseOptionalIntL(aAttributes, _L8("volume"), -1);
            const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectedomxerr"));
            OMX_ERRORTYPE expectedErrorInt = OMX_ErrorNone;
            if (expectedError)
                {
                expectedErrorInt = ParseOmxErrorCode(*expectedError);
                }
            
            CheckForAbortL(iCallback.MosSetConfigAudioVolumeL(comp, port, linear, minVolume, maxVolume, volume, expectedErrorInt));
            }
		else if(elemName == _L8("CheckAudioVolume"))
		    {
		    const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
		    TPtrC8 comp;
		    TInt port;
		    ParseCompPortL(compPort, comp, port);
		    TBool linear = EFalse;
		    const TDesC8* linearScaleAttr = FindAttribute(aAttributes, _L8("linearscale"));
		    if (linearScaleAttr != NULL)
		        {
		        linear = ParseBooleanL(*linearScaleAttr);
		        }
		    TInt minVolume = ParseOptionalIntL(aAttributes, _L8("min"), -1);
		    TInt maxVolume = ParseOptionalIntL(aAttributes, _L8("max"), -1);
		    TInt volume = ParseOptionalIntL(aAttributes, _L8("volume"), -1);
		    CheckForAbortL(iCallback.MosCheckConfigAudioVolumeL(comp, port, linear, minVolume, maxVolume, volume));
		    }
		else if(elemName == _L8("SetAacAudioPortDef"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
			TInt channels = ParseOptionalIntL(aAttributes, _L8("channels"), -1);
			TInt samplingRate = ParseOptionalIntL(aAttributes, _L8("samplingrate"), -1);
			TInt bitRate = ParseOptionalIntL(aAttributes, _L8("bitrate"), -1);
			TInt audioBandwidth = ParseOptionalIntL(aAttributes, _L8("audiobandwidth"), -1);
			TInt frameLength = ParseOptionalIntL(aAttributes, _L8("framelength"), -1);
			TInt aacTools = ParseOptionalIntL(aAttributes, _L8("aactools"), -1);
			TInt aacErTools = ParseOptionalIntL(aAttributes, _L8("aacertools"), -1);
			
			OMX_AUDIO_AACPROFILETYPE profile;
			const TDesC8* attbval = FindAttribute(aAttributes, _L8("profile"));
			            if ( NULL != attbval )
			                {
			                profile = ParseOmxAACProfileL( *attbval );
			                }
			            else
			                {
			                profile = static_cast <OMX_AUDIO_AACPROFILETYPE>(-1);
			                }
			OMX_AUDIO_AACSTREAMFORMATTYPE streamFormat = ParseOmxAACStreamFormatL(FindAttributeL(aAttributes, _L8("streamFormat")));
			OMX_AUDIO_CHANNELMODETYPE channelMode = ParseOmxAudioChannelModeL(FindAttributeL(aAttributes, _L8("channelMode")));

			CheckForAbortL(iCallback.MosSetAacAudioPortDefL(comp, port, channels, samplingRate, bitRate, audioBandwidth, frameLength, aacTools, aacErTools, profile, streamFormat, channelMode));
			}
		else if(elemName == _L8("SetClockReference"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& refDes = FindAttributeL(aAttributes, _L8("ref"));
			OMX_TIME_REFCLOCKTYPE refClockType = ParseOmxRefClockTypeL(refDes);
			CheckForAbortL(iCallback.MosSetRefClockTypeL(comp, refClockType));
			}
		else if(elemName == _L8("SetClockState"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& stateDes = FindAttributeL(aAttributes, _L8("state"));
			OMX_TIME_CLOCKSTATE clockState = ParseOmxClockStateL(stateDes);
			const TDesC8& maskDes = FindAttributeL(aAttributes, _L8("mask"));
			TUint32 mask = ParseUint32L(maskDes);
			TInt startTime = ParseOptionalIntL(aAttributes, _L8("start"), 0);
			TInt offset = ParseOptionalIntL(aAttributes, _L8("offset"), 0);
			CheckForAbortL(iCallback.MosSetClockStateL(comp, clockState, startTime, offset, mask));
			}
		else if(elemName == _L8("SetVideoFitMode"))
			{
			const TDesC8& modeDes = FindAttributeL(aAttributes, _L8("mode"));
			TVideoFitMode mode = ParseVideoFitModeL(modeDes);
			CheckForAbortL(iCallback.MosSetVideoFitModeL(mode));
			}
		else if(elemName == _L8("SetActiveStream"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& id = FindAttributeL(aAttributes, _L8("id"));
			TLex8 lex(id);
			TInt streamId;
			User::LeaveIfError(lex.Val(streamId));
			CheckForAbortL(iCallback.MosSetActiveStream(comp, streamId));
			}
		else if(elemName == _L8("GetActiveStream"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& id = FindAttributeL(aAttributes, _L8("id"));
			TLex8 lex(id);
			TInt streamId;
			User::LeaveIfError(lex.Val(streamId));
			CheckForAbortL(iCallback.MosGetActiveStream(comp, streamId));
			}
		else if(elemName == _L8("LoadBufferHandler"))
			{
			// Load buffer component handler
			// Buffer component tests
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& filename = FindAttributeL(aAttributes, _L8("filename"));
			const TDesC8* type = FindAttribute(aAttributes, _L8("type"));
			TBuf<KMaxFileName> name;
			User::LeaveIfError(CnvUtfConverter::ConvertToUnicodeFromUtf8(name,filename));
			
			// Call test script to create buffer handler
			CheckForAbortL(iCallback.InitialiseBufferHandlerL(comp, name, type));
			}
		else if(elemName == _L8("StartBufferHandler"))
			{
			iCallback.StartBufferHandler();
			}
		else if (elemName == _L8("SendInvalidBufferId"))
			{
			const TDesC8& id = FindAttributeL(aAttributes, _L8("id"));
			TInt idValue = ParseUint32L(id);
			iCallback.SendInvalidBufferId(idValue);
			}
		else if(elemName == _L8("EndBufferHandler"))
			{
			iCallback.StopBufferHandler();
			}
        else if(elemName == _L8("LoadBufferSinkHandler"))
			{
			// Load buffer component handler
			// Buffer component tests
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& filename = FindAttributeL(aAttributes, _L8("filename"));
			TBuf<KMaxFileName> name;
			User::LeaveIfError(CnvUtfConverter::ConvertToUnicodeFromUtf8(name,filename));
			
			// Call test script to create buffer handler
			CheckForAbortL(iCallback.InitialiseBufferSinkHandlerL(comp, name));
			}
		else if (elemName == _L8("StartBufferSinkHandler"))
		    {
		    iCallback.StartBufferSinkHandler();
		    }
		else if (elemName == _L8("EndBufferSinkHandler"))
		    {
		    iCallback.StopBufferSinkHandler();
		    }
        else if(elemName == _L8("NegativeSetDataChunk"))
		    {
		    const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
		   		
		    const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectedomxerr"));
		    OMX_ERRORTYPE expectedErrorInt = OMX_ErrorNone;
			if (expectedError)
			    {
			    expectedErrorInt = ParseOmxErrorCode(*expectedError);
			    }
			
			const TDesC8* expectedSystemError = FindAttribute(aAttributes, _L8("expectederror"));
		    TInt expectedSystemErrorInt = KErrNone;
			if (expectedSystemError)
			    {
			    expectedSystemErrorInt = ParseSystemErrorCode(*expectedSystemError);
			    }   
	
			CheckForAbortL(iCallback.MosNegativeSetDataChunk(comp, expectedErrorInt, expectedSystemErrorInt));
		    }
		else if(elemName == _L8("SetBufferSize"))
			{
			const TDesC8& port = FindAttributeL(aAttributes, _L8("port"));
			const TDesC8& value = FindAttributeL(aAttributes, _L8("size"));
			const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectedomxerr"));
		    OMX_ERRORTYPE expectedErrorInt = OMX_ErrorNone;
			if (expectedError)
			    {
			    expectedErrorInt = ParseOmxErrorCode(*expectedError);
			    }
			TInt portIndex = 0;
			TPtrC8 comp;
			ParseCompPortL(port, comp, portIndex);
			TLex8 lex(value);
			TInt valInt = 0;
			User::LeaveIfError(lex.Val(valInt));
			CheckForAbortL(iCallback.MosSetBufferSize(comp, portIndex, valInt, expectedErrorInt));
			}
		else if(elemName == _L8("GetAndCompareBufferCount"))
			{
			const TDesC8& port = FindAttributeL(aAttributes, _L8("port"));
			const TDesC8& value = FindAttributeL(aAttributes, _L8("count"));
			TInt portIndex = 0;
			TPtrC8 comp;
			ParseCompPortL(port, comp, portIndex);
			TLex8 lex(value);
			TInt countInt = 0;
			User::LeaveIfError(lex.Val(countInt));
		    CheckForAbortL(iCallback.MosGetAndCompareBufferCount(comp, portIndex, countInt));
			}
		else if(elemName == _L8("GetAndCompareBufferSize"))
			{
			const TDesC8& port = FindAttributeL(aAttributes, _L8("port"));
			const TDesC8& value = FindAttributeL(aAttributes, _L8("size"));
			TInt portIndex = 0;
			TPtrC8 comp;
			ParseCompPortL(port, comp, portIndex);
			TLex8 lex(value);
			TInt valInt = 0;
			User::LeaveIfError(lex.Val(valInt));
		    CheckForAbortL(iCallback.MosGetAndCompareBufferSize(comp, portIndex, valInt));
			}
		else if(elemName == _L8("FlushBuffer"))
			{
			const TDesC8& expectedError = FindAttributeL(aAttributes, _L8("expectedomxerr"));
			OMX_ERRORTYPE expectedErrorInt = ParseOmxErrorCode(expectedError);

			const TDesC8& port = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt portIndex = 0;
			ParseCompPortL(port, comp, portIndex);
			CheckForAbortL(iCallback.MosFlushBuffer(comp, portIndex, expectedErrorInt));
			}
		else if(elemName == _L8("ForceBufferSourceFlushBuffer"))
			{
			const TDesC8& expectedError = FindAttributeL(aAttributes, _L8("expectedomxerr"));
			OMX_ERRORTYPE expectedErrorInt = ParseOmxErrorCode(expectedError);

			const TDesC8& port = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt portIndex = 0;
			ParseCompPortL(port, comp, portIndex);
			CheckForAbortL(iCallback.MosForceBufferSourceFlushBufferL(comp, portIndex, expectedErrorInt));
			}
		else if(elemName == _L8("ForceBufferSinkFlushBuffer"))
			{
			const TDesC8& expectedError = FindAttributeL(aAttributes, _L8("expectedomxerr"));
			OMX_ERRORTYPE expectedErrorInt = ParseOmxErrorCode(expectedError);

			const TDesC8& port = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt portIndex = 0;
			ParseCompPortL(port, comp, portIndex);
			CheckForAbortL(iCallback.MosForceBufferSinkFlushBuffer(comp, portIndex, expectedErrorInt));
			}
		else if(elemName == _L8("SetVideoEncQuant"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
		
			TInt qpb = ParseOptionalIntL(aAttributes, _L8("qpb"), -1);
			
			CheckForAbortL(iCallback.MosSetVideoEncQuantL(comp, port,qpb));
			}
		else if(elemName == _L8("SetVideoEncMotionVect"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
			
			TInt accuracy = ParseOptionalIntL(aAttributes, _L8("accuracy"), -1);			
			TInt sxsearchrange = ParseOptionalIntL(aAttributes, _L8("sxsearchrange"), -1);
			TInt sysearchrange = ParseOptionalIntL(aAttributes, _L8("sysearchrange"), -1);
			
			const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectedomxerr"));
		    OMX_ERRORTYPE expectedOmxError = OMX_ErrorNone;
			if (expectedError)
			    {
			    expectedOmxError = ParseOmxErrorCode(*expectedError);
			    }
			    
			CheckForAbortL(iCallback.MosSetVideoEncMotionVectL(comp, port, accuracy, sxsearchrange, sysearchrange, expectedOmxError));
			}
		else if(elemName == _L8("SetVideoEncMpeg4Type"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
			const TDesC8* mpeg4Profile = FindAttribute(aAttributes, _L8("mpeg4profile"));
			OMX_VIDEO_MPEG4PROFILETYPE profile = OMX_VIDEO_MPEG4ProfileMax;
			if (mpeg4Profile)
				{
				profile = ParseOmxMpeg4ProfileL(*mpeg4Profile);
				}
			
			OMX_VIDEO_MPEG4LEVELTYPE level = OMX_VIDEO_MPEG4LevelMax;
			const TDesC8* mpeg4Level = FindAttribute(aAttributes, _L8("mpeg4level"));
			if (mpeg4Level)
				{
				level = ParseOmxMpeg4LevelL(*mpeg4Level);
				}
			const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectedomxerr"));
		    OMX_ERRORTYPE expectedOmxError = OMX_ErrorNone;
			if (expectedError)
			    {
			    expectedOmxError = ParseOmxErrorCode(*expectedError);
			    }

			CheckForAbortL(iCallback.MosSetVideoEncMpeg4TypeL(comp, port, profile, level, expectedOmxError));
			}
		else if(elemName == _L8("SetVideoEncBitRate"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
			
			const TDesC8* controlRate = FindAttribute(aAttributes, _L8("controlrate"));
			OMX_VIDEO_CONTROLRATETYPE omxControlRate = OMX_Video_ControlRateMax;
			if (controlRate)
				{
				omxControlRate = ParseOmxControlRateL(*controlRate);
				}
			
			TInt targetBitrate = ParseOptionalIntL(aAttributes, _L8("targetbitrate"), -1);
			
			const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectedomxerr"));
		    OMX_ERRORTYPE expectedOmxError = OMX_ErrorNone;
			if (expectedError)
			    {
			    expectedOmxError = ParseOmxErrorCode(*expectedError);
			    }

			CheckForAbortL(iCallback.MosSetVideoEncBitRateL(comp, port, omxControlRate, targetBitrate, expectedOmxError));
			}
		else if(elemName == _L8("ChangeFilledBufferLength"))
		    {
		    const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
		    const TDesC8& value = FindAttributeL(aAttributes, _L8("data"));
		    TLex8 lex(value);
			TInt valInt = 0;
			User::LeaveIfError(lex.Val(valInt));
			CheckForAbortL(iCallback.MosChangeFilledBufferLength(comp, valInt));
		    }
		else if(elemName == _L8("SetOMX_SymbianIndexParamBufferMsgQueueData"))
		    {
		    const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
		    const TDesC8& expectedError = FindAttributeL(aAttributes, _L8("expectedomxerr"));
			OMX_ERRORTYPE expectedErrorInt = ParseOmxErrorCode(expectedError);
            
            CheckForAbortL(iCallback.MosSetOMX_SymbianIndexParamBufferMsgQueueData(comp, expectedErrorInt));
		    }
		else if(elemName == _L8("GetExtensionIndex"))
		    {
		    const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
		    const TDesC8& paramName = FindAttributeL(aAttributes, _L8("parametername"));
		    
		    const TDesC8* expectedError = FindAttribute(aAttributes, _L8("expectedomxerr"));
		    OMX_ERRORTYPE expectedErrorInt = OMX_ErrorNone;
			if (expectedError)
			    {
			    expectedErrorInt = ParseOmxErrorCode(*expectedError);
			    }
            
            CheckForAbortL(iCallback.MosGetExtensionIndex(comp, paramName, expectedErrorInt));
		    }
		else if(elemName == _L8("SetCaptureModeType"))
		    {
		    const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
		    
		    TPtrC8 comp;
		    TInt port = 0;
		    ParseCompPortL(compPort, comp, port);
		    
		    const TDesC8& continuousStr = FindAttributeL(aAttributes, _L8("continuous"));
		    TBool continuous = ParseBooleanL(continuousStr);
		    const TDesC8& framelimitedStr = FindAttributeL(aAttributes, _L8("framelimited"));
		    TBool framelimited = ParseBooleanL(framelimitedStr);
		    TInt framelimit = ParseOptionalIntL(aAttributes, _L8("framelimit"), -1);
		    
		    CheckForAbortL(iCallback.MosSetCaptureModeTypeL(comp, port, continuous, framelimited, framelimit));
		    }

		else if(elemName == _L8("GetTimeClockState"))
		    {
		    const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
		    const TDesC8& expectedState = FindAttributeL(aAttributes, _L8("expectedstate"));
		    OMX_TIME_CLOCKSTATE expectedStateInt = ParseOmxClockStateL(expectedState);

		    CheckForAbortL(iCallback.MosCheckTimeClockState(comp, expectedStateInt));
		    }
		else if(elemName == _L8("CheckMediaTime"))
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
	
			ParseCompPortL(compPort, comp, port);
	
			TBool moreThan = ETrue;
			const TDesC8* mediatime = FindAttribute(aAttributes, _L8("mediatimelessthan"));
			
			if (mediatime)
			  {
			    moreThan = EFalse;
			  }
			else
			  {
			    const TDesC8& mTime = FindAttributeL(aAttributes, _L8("mediatimemorethan"));
			    mediatime = &mTime;
			  }


			OMX_TICKS mediatimeTick;
			TLex8 lex(*mediatime);
			User::LeaveIfError(lex.Val(mediatimeTick));		 
			CheckForAbortL(iCallback.MosCheckMediaTime(comp, port, mediatimeTick, moreThan));
			}
		else if(elemName == _L8("SetClockTimeScale"))
			{
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& scale = FindAttributeL(aAttributes, _L8("scale"));
	
			TReal32 xscale = ParseReal32L(scale);
	
			CheckForAbortL(iCallback.MosSetClockTimeScale(comp,xscale * 65536));
			}
		else if(elemName == _L8("Base_AddPortSupport"))
		    {
		    const TDesC8& portValue = FindAttributeL(aAttributes, _L8("port"));
		    TPtrC8 comp;
		    TInt portIndex;
		    ParseCompPortL(portValue, comp, portIndex);
		    CheckForAbortL(iCallback.MosBaseSupportPortL(comp, portIndex));
		    }
        else if(elemName == _L8("Base_SetAutonomous"))
            {
            const TDesC8& portValue = FindAttributeL(aAttributes, _L8("port"));
            TPtrC8 comp;
            TInt portIndex;
            ParseCompPortL(portValue, comp, portIndex);
            const TDesC8& enabledValue = FindAttributeL(aAttributes, _L8("enabled"));
            TBool enabled = ParseBooleanL(enabledValue);
            CheckForAbortL(iCallback.MosBaseSetAutonomous(comp, portIndex, enabled));
            }		
        else if(elemName == _L8("Base_AllocateBuffers"))
            {
            const TDesC8& portValue = FindAttributeL(aAttributes, _L8("port"));
            TPtrC8 comp;
            TInt portIndex;
            ParseCompPortL(portValue, comp, portIndex);
            
            const TDesC8& numberValue = FindAttributeL(aAttributes, _L8("number"));
            TInt numberBuffs;
            TLex8 lex(numberValue);
            User::LeaveIfError(lex.Val(numberBuffs));
            
            CheckForAbortL(iCallback.MosBaseAllocateBuffersL(comp, portIndex, numberBuffs));
            }
        else if(elemName == _L8("Base_FreeAllocatedBuffers"))
            {
			const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
            
            CheckForAbortL(iCallback.MosBaseFreeAllocatedBuffersL(comp));
            }			
        else if(elemName == _L8("Base_SetBufSupplierPref"))
            {
            const TDesC8& portValue = FindAttributeL(aAttributes, _L8("port"));
            TPtrC8 comp;
            TInt portIndex;
            ParseCompPortL(portValue, comp, portIndex);
            
            const TDesC8& componentSupplierValue = FindAttributeL(aAttributes, _L8("iscomponentsupplier"));
            TBool componentSupplier = ParseBooleanL(componentSupplierValue);
            CheckForAbortL(iCallback.MosBaseSetBufSupplier(comp, portIndex, componentSupplier));
            }
        else if(elemName == _L8("Base_FillThisBuffer"))
            {
            const TDesC8& portValue = FindAttributeL(aAttributes, _L8("port"));
            TPtrC8 comp;
            TInt portIndex;
            ParseCompPortL(portValue, comp, portIndex);
            
            const TDesC8& bufIndexValue = FindAttributeL(aAttributes, _L8("portrelativebufferindex"));
            TInt bufIndex;
            TLex8 lex(bufIndexValue);
            User::LeaveIfError(lex.Val(bufIndex));
            
            CheckForAbortL(iCallback.MosBaseFillThisBuffer(comp, portIndex, bufIndex));         
            }
        else if(elemName == _L8("Base_EmptyThisBuffer"))
            {
            const TDesC8& portValue = FindAttributeL(aAttributes, _L8("port"));
            TPtrC8 comp;
            TInt portIndex;
            ParseCompPortL(portValue, comp, portIndex);
            
            const TDesC8& bufIndexValue = FindAttributeL(aAttributes, _L8("portrelativebufferindex"));
            TInt bufIndex;
            TLex8 lex(bufIndexValue);
            User::LeaveIfError(lex.Val(bufIndex));
            
            CheckForAbortL(iCallback.MosBaseEmptyThisBuffer(comp, portIndex, bufIndex));         
            }   
        else if(elemName == _L8("Base_WaitForBuffer"))
            {            
            const TDesC8& portValue = FindAttributeL(aAttributes, _L8("port"));
            TPtrC8 comp;
            TInt portIndex;
            ParseCompPortL(portValue, comp, portIndex);

            const TDesC8& bufIndexValue = FindAttributeL(aAttributes, _L8("bufferindexinport"));
            TInt bufIndex;
            TLex8 lex(bufIndexValue);
            User::LeaveIfError(lex.Val(bufIndex));            
            
            CheckForAbortL(iCallback.MosBaseWaitForBuffer(comp, portIndex, bufIndex));         
            }		
        else if(elemName == _L8("BaseTimestamp_PassClockHandle"))
            {
            const TDesC8& receivingComp = FindAttributeL(aAttributes, _L8("receiver"));
            const TDesC8& clockToPass = FindAttributeL(aAttributes, _L8("handle"));
            
            CheckForAbortL(iCallback.MosBaseTimestampPassClock(receivingComp, clockToPass));
            }
        else if(elemName == _L8("BaseTimestamp_CheckTimestamp"))
            {
            const TDesC8& portValue = FindAttributeL(aAttributes, _L8("port"));
            TPtrC8 comp;
            TInt portIndex;
            ParseCompPortL(portValue, comp, portIndex);
                        
            const TDesC8& timeValue = FindAttributeL(aAttributes, _L8("time"));
            TUint time;
            TLex8 lex(timeValue);
            User::LeaveIfError(lex.Val(time));
                        
            const TDesC8& toleranceValue = FindAttributeL(aAttributes, _L8("tolerance"));
            TUint tolerance;
            lex.Assign(toleranceValue);
            User::LeaveIfError(lex.Val(tolerance));            
            
            CheckForAbortL(iCallback.MosBaseTimestampCheckTimestampL(comp, portIndex, time, tolerance));         
            }
        else if(elemName == _L8("BaseTimestamp_CompareWithRefClock"))
            {
            const TDesC8& portValue = FindAttributeL(aAttributes, _L8("port"));
            TPtrC8 comp;
            TInt portIndex;
            ParseCompPortL(portValue, comp, portIndex);
                                    
            const TDesC8& toleranceValue = FindAttributeL(aAttributes, _L8("tolerance"));
            TUint tolerance;
            TLex8 lex(toleranceValue);
            User::LeaveIfError(lex.Val(tolerance));            
                        
            CheckForAbortL(iCallback.MosBaseTimestampCompareWithRefClockL(comp, portIndex, tolerance));             
            }
		else if(elemName == _L8("CheckClockState"))
            {
            const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
            const TDesC8& stateDes = FindAttributeL(aAttributes, _L8("clockstate"));
            OMX_TIME_CLOCKSTATE clockState = ParseOmxClockStateL(stateDes);            
            CheckForAbortL(iCallback.MosCheckClockStateL(comp, clockState));
            }
	      else if(elemName == _L8("CheckTimePosition"))
            {
			// Use with caution: increments position as a side-effect.
            const TDesC8& port = FindAttributeL(aAttributes, _L8("port"));
            TPtrC8 comp;
            TInt portIndex;
            ParseCompPortL(port, comp, portIndex);
            
            TInt timestamp = ParseOptionalIntL(aAttributes, _L8("timestamp"), 0);
            
            CheckForAbortL(iCallback.MosCheckTimePositionL(comp, portIndex, timestamp));
            }
        else if(elemName == _L8("SetTimePosition"))
            {
            const TDesC8& port = FindAttributeL(aAttributes, _L8("port"));
            TPtrC8 comp;
            TInt portIndex;
            ParseCompPortL(port, comp, portIndex);
            
            TInt timestamp = ParseOptionalIntL(aAttributes, _L8("timestamp"), 0);
            
            CheckForAbortL(iCallback.MosSetTimePositionL(comp, portIndex, timestamp));
			}
		else if(elemName == _L8("StartBuffersforPort"))
            {       
            const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));          
            TPtrC8 comp;
            TInt port;
            ParseCompPortL(compPort, comp, port);

            CheckForAbortL(iCallback.MosStartBuffersforPort( comp,port));
		    }
       else if(elemName == _L8("CheckFrameCount"))
            {
            const TDesC8& fileName8 = FindAttributeL(aAttributes, _L8("filename"));
            TInt count = ParseOptionalIntL(aAttributes, _L8("count"), 0);

            HBufC* fileName = HBufC::NewLC(fileName8.Length());
            fileName->Des().Copy(fileName8);
            CheckForAbortL(iCallback.MosCheckFrameCountL(*fileName, count));
            CleanupStack::PopAndDestroy(fileName);
            }
	
		else if(elemName == _L8("MarkBuffer"))
			{
			const TDesC8& port = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt portIndex;
			ParseCompPortL(port, comp, portIndex);
			const TDesC8& targetComp = FindAttributeL(aAttributes, _L8("targetComp"));
			TInt markData = ParseOptionalIntL(aAttributes, _L8("markData"), 0);
			CheckForAbortL(iCallback.MosMarkBuffer(comp, portIndex, targetComp, markData));
			}
#ifdef OLD_ADPCM_EXTENSION
		else if (elemName == _L8("SetAdPcmAudioPortDef"))	
			{
			const TDesC8& compPort = FindAttributeL(aAttributes, _L8("port"));
			TPtrC8 comp;
			TInt port;
			ParseCompPortL(compPort, comp, port);
			TInt channels = ParseOptionalIntL(aAttributes, _L8("channels"), -1);
			TInt samplingrate = ParseOptionalIntL(aAttributes, _L8("samplingrate"), -1);
			TInt bitspersample = ParseOptionalIntL(aAttributes, _L8("bitspersample"), -1);
			CheckForAbortL(iCallback.MosSetAdPcmAudioPortDefL(comp, port, channels, samplingrate, bitspersample));
			}
		else if (elemName == _L8("SetAdPcmDecoderBlockAlign"))
			{
            const TDesC8& comp = FindAttributeL(aAttributes, _L8("comp"));
			const TDesC8& blockAlign = FindAttributeL(aAttributes, _L8("blockalign"));
			TLex8 lex(blockAlign);
			TInt blockAlignValue;
			User::LeaveIfError(lex.Val(blockAlignValue));	
            CheckForAbortL(iCallback.MosConfigAdPcmDecoderBlockAlign(comp, blockAlignValue));			
			}
#endif	
		else
			{
			// element name is not recognized
			TBuf<32> elemNameCopy;
			elemNameCopy.Copy(elemName);
			TBuf<44> message;
			message.Append(_L("Unrecognized command: "));
			message.Append(elemNameCopy);
			iCallback.MosParseError(message);
			User::Leave(KErrArgument);
			}
		}
	}
	
TInt COmxScriptParser::ParseSystemErrorCode(const TDesC8& aErrorCode)
    {
    TInt error = KErrNone;
    
    if (aErrorCode == _L8("KErrNotFound"))
        {
        error = KErrNotFound;
        }
    
    // TODO: the other error code will be added in demand
    
    return error;
    }

// TODO duplication with ParseOmxErrorL
OMX_ERRORTYPE COmxScriptParser::ParseOmxErrorCode(const TDesC8& aErrorCode)
    {
    #define DEFERR(e) if(aErrorCode == _L8(#e)) return e;
    
		DEFERR(OMX_ErrorNone);
 		DEFERR(OMX_ErrorInsufficientResources);
		DEFERR(OMX_ErrorUndefined);
		DEFERR(OMX_ErrorInvalidComponentName);
		DEFERR(OMX_ErrorComponentNotFound);
		DEFERR(OMX_ErrorInvalidComponent);
		DEFERR(OMX_ErrorBadParameter);
		DEFERR(OMX_ErrorNotImplemented);
		DEFERR(OMX_ErrorUnderflow);
		DEFERR(OMX_ErrorOverflow);
		DEFERR(OMX_ErrorHardware);
		DEFERR(OMX_ErrorInvalidState);
		DEFERR(OMX_ErrorStreamCorrupt);
		DEFERR(OMX_ErrorPortsNotCompatible);
		DEFERR(OMX_ErrorResourcesLost);
		DEFERR(OMX_ErrorNoMore);
		DEFERR(OMX_ErrorVersionMismatch);
		DEFERR(OMX_ErrorNotReady);
		DEFERR(OMX_ErrorTimeout);
		DEFERR(OMX_ErrorSameState);
		DEFERR(OMX_ErrorResourcesPreempted);
		DEFERR(OMX_ErrorPortUnresponsiveDuringAllocation);
		DEFERR(OMX_ErrorPortUnresponsiveDuringDeallocation);
		DEFERR(OMX_ErrorPortUnresponsiveDuringStop);
		DEFERR(OMX_ErrorIncorrectStateTransition);
		DEFERR(OMX_ErrorIncorrectStateOperation);
		DEFERR(OMX_ErrorUnsupportedSetting);
		DEFERR(OMX_ErrorUnsupportedIndex);
		DEFERR(OMX_ErrorBadPortIndex);
		DEFERR(OMX_ErrorPortUnpopulated);
		DEFERR(OMX_ErrorComponentSuspended);
		DEFERR(OMX_ErrorDynamicResourcesUnavailable);
		DEFERR(OMX_ErrorMbErrorsInFrame);
		DEFERR(OMX_ErrorFormatNotDetected);
		DEFERR(OMX_ErrorContentPipeOpenFailed);
		DEFERR(OMX_ErrorContentPipeCreationFailed);
		DEFERR(OMX_ErrorSeperateTablesUsed);
		DEFERR(OMX_ErrorTunnelingUnsupported);
		DEFERR(OMX_ErrorKhronosExtensions);
		DEFERR(OMX_ErrorVendorStartUnused);
	#undef DEFERR

	return OMX_ErrorMax;
    }
    
void COmxScriptParser::OnEndElementL(const RTagInfo& aElement, TInt /*aErrorCode*/)
	{
	if(iInTest)
		{
		const TDesC8& elemName = aElement.LocalName().DesC();
		if(elemName == _L8("Test"))
			{
			iInTest = EFalse;
			}
		}
	}

const TDesC8* COmxScriptParser::FindAttribute(const RArray<RAttribute>& aArray, const TDesC8& aAttribName)
	{
	for(TInt index = 0, count = aArray.Count(); index < count; index++)
		{
		const RAttribute& attribute = aArray[index];
		const TDesC8& name = attribute.Attribute().LocalName().DesC();
		if(name == aAttribName)
			{
			return &(attribute.Value().DesC());
			}
		}
	return NULL;
	}

const TDesC8& COmxScriptParser::FindAttributeL(const RArray<RAttribute>& aArray, const TDesC8& aAttribName)
	{
	const TDesC8* result = FindAttribute(aArray, aAttribName);
	if(!result)
		{
		TBuf<32> nameCopy;
		nameCopy.Copy(aAttribName);
		TBuf<52> msg;
		msg.Append(_L("Attribute "));
		msg.Append(nameCopy);
		msg.Append(_L(" not found"));
		iCallback.MosParseError(msg);
		User::Leave(KErrNotFound);
		}
	return *result;
	}

void COmxScriptParser::ParseCompPortL(const TDesC8& aInput, TPtrC8& aNameOut, TInt& aPortOut)
    {
    TInt offset = aInput.Locate(':');
    User::LeaveIfError(offset);
    aNameOut.Set(aInput.Left(offset));
    TPtrC8 port = aInput.Right(aInput.Length() - offset - 1);
    if (port == _L8("all"))
        {
        aPortOut = static_cast<TInt>(OMX_ALL);
        }
    else
        {
        TLex8 lex(port);
        User::LeaveIfError(lex.Val(aPortOut));
        }
    }


// The order of these entries does not matter, but for clarity's sake please
// maintain alphabetical order

PARSE_MAP_START(OMX_AUDIO_AACPROFILETYPE)
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACObject, ERLC),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACObject, HE),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACObject, HE_PS),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACObject, LC),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACObject, LD),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACObject, LTP),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACObject, Main),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACObject, Null),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACObject, Scalable),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACObject, SSR)    
PARSE_MAP_END(OMX_AUDIO_AACPROFILETYPE)

PARSE_MAP_START(OMX_AUDIO_AACSTREAMFORMATTYPE)
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACStreamFormat, ADIF),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACStreamFormat, MP2ADTS),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACStreamFormat, MP4ADTS),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACStreamFormat, MP4FF),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACStreamFormat, MP4LATM),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACStreamFormat, MP4LOAS),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_AACStreamFormat, RAW)
PARSE_MAP_END(OMX_AUDIO_AACSTREAMFORMATTYPE)

PARSE_MAP_START(OMX_AUDIO_CHANNELMODETYPE)
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_ChannelMode, Dual), 
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_ChannelMode, JointStereo), 
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_ChannelMode, Mono),
	PARSE_MAP_PREFIXENTRY(OMX_AUDIO_ChannelMode, Stereo) 
PARSE_MAP_END(OMX_AUDIO_CHANNELMODETYPE)

PARSE_MAP_START(OMX_COMMANDTYPE)
	PARSE_MAP_PREFIXENTRY(OMX_Command, StateSet),
	PARSE_MAP_PREFIXENTRY(OMX_Command, Flush),
	PARSE_MAP_PREFIXENTRY(OMX_Command, PortDisable),
	PARSE_MAP_PREFIXENTRY(OMX_Command, PortEnable),
	PARSE_MAP_PREFIXENTRY(OMX_Command, MarkBuffer)
PARSE_MAP_END(OMX_COMMANDTYPE)

PARSE_MAP_START(OMX_COLOR_FORMATTYPE)
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, Unused),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, Max),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, 12bitRGB444),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, 16bitRGB565),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, 24bitBGR888),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, 32bitARGB8888),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, 32bitBGRA8888),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, CbYCrY),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, YCbYCr),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, YCrYCb),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, YUV420PackedPlanar),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, YUV422PackedPlanar),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, YUV420Planar),
#if defined(NCP_COMMON_BRIDGE_FAMILY) && !defined(__WINSCW__)
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, YUV422Planar),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, STYUV420PackedSemiPlanarMB),
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, STYUV422PackedSemiPlanarMB)
#else
	PARSE_MAP_PREFIXENTRY(OMX_COLOR_Format, YUV422Planar)
#endif //NCP_COMMON_BRIDGE_FAMILY	
PARSE_MAP_END(OMX_COLOR_FORMATTYPE)

PARSE_MAP_START(OMX_ERRORTYPE)
	PARSE_MAP_PREFIXENTRY(OMX_Error, BadParameter),
	PARSE_MAP_PREFIXENTRY(OMX_Error, BadPortIndex),
	PARSE_MAP_PREFIXENTRY(OMX_Error, ContentPipeOpenFailed),
	PARSE_MAP_PREFIXENTRY(OMX_Error, Hardware),
	PARSE_MAP_PREFIXENTRY(OMX_Error, IncorrectStateOperation),
	PARSE_MAP_PREFIXENTRY(OMX_Error, None),
	PARSE_MAP_PREFIXENTRY(OMX_Error, NotReady),
	PARSE_MAP_PREFIXENTRY(OMX_Error, PortsNotCompatible),
	PARSE_MAP_PREFIXENTRY(OMX_Error, Underflow),
	PARSE_MAP_PREFIXENTRY(OMX_Error, UnsupportedIndex),
	PARSE_MAP_PREFIXENTRY(OMX_Error, UnsupportedSetting)
PARSE_MAP_END(OMX_ERRORTYPE)

PARSE_MAP_START(OMX_EVENTTYPE)
	PARSE_MAP_PREFIXENTRY(OMX_Event, BufferFlag),
	PARSE_MAP_PREFIXENTRY(OMX_Event, CmdComplete),
	PARSE_MAP_PREFIXENTRY(OMX_Event, Error),
	PARSE_MAP_PREFIXENTRY(OMX_Event, Mark),
	PARSE_MAP_PREFIXENTRY(OMX_Event, PortFormatDetected),
	PARSE_MAP_PREFIXENTRY(OMX_Event, PortSettingsChanged),
PARSE_MAP_END(OMX_EVENTTYPE)

//	PARSE_MAP_PREFIXENTRY(OMX_EventNokia, FirstFrameDisplayed),
//	PARSE_MAP_PREFIXENTRY(OMX_EventNokia, DroppedFrame)


PARSE_MAP_START(OMX_STATETYPE)
	PARSE_MAP_PREFIXENTRY(OMX_State, Loaded),
	PARSE_MAP_PREFIXENTRY(OMX_State, Idle),
	PARSE_MAP_PREFIXENTRY(OMX_State, Executing),
	PARSE_MAP_PREFIXENTRY(OMX_State, Pause),
	PARSE_MAP_PREFIXENTRY(OMX_State, WaitForResources),
	PARSE_MAP_PREFIXENTRY(OMX_State, Invalid)
PARSE_MAP_END(OMX_STATETYPE)

/**
 * Templated wrapper to a plain-C function generated by PARSE_MAP
 * Accepts a descriptor as input and leaves with KErrArgument if parse fails.
 */
template<typename T> T ParseL(const TDesC8& aDes, TInt (*parseFunc)(const char*, T*))
	{
	if(aDes.Length() >= 64)
		{
		User::Leave(KErrArgument);
		}
	TBuf8<64> buf = aDes;
	T result = (T) 0;
	TInt success = parseFunc((char*) buf.PtrZ(), &result);
	if(!success)
		{
		// value not recognized
		// maybe add it to the PARSE_MAP ?
		__BREAKPOINT();
		User::Leave(KErrArgument);
		}
	return result;
	}

OMX_AUDIO_AACPROFILETYPE COmxScriptParser::ParseOmxAACProfileL(const TDesC8& aProfileDes)
	{
	return ParseL(aProfileDes, parse_OMX_AUDIO_AACPROFILETYPE);
	}

OMX_AUDIO_AACSTREAMFORMATTYPE COmxScriptParser::ParseOmxAACStreamFormatL(const TDesC8& aFormatDes)
	{
	return ParseL(aFormatDes, parse_OMX_AUDIO_AACSTREAMFORMATTYPE);
	}

OMX_AUDIO_CHANNELMODETYPE COmxScriptParser::ParseOmxAudioChannelModeL(const TDesC8& aChannelModeDes)
	{
	return ParseL(aChannelModeDes, parse_OMX_AUDIO_CHANNELMODETYPE);
	}

OMX_STATETYPE COmxScriptParser::ParseOmxStateL(const TDesC8& aStateDes)
	{
	return ParseL(aStateDes, parse_OMX_STATETYPE);
	}

OMX_METADATASCOPETYPE COmxScriptParser::ParseOmxScopeTypeL(const TDesC8& aScopeDes)
	{
	if(aScopeDes == _L8("all"))
		{
		return OMX_MetadataScopeAllLevels;
		}
	else if(aScopeDes == _L8("top"))
		{
		return OMX_MetadataScopeTopLevel;
		}
	else if(aScopeDes == _L8("port"))
		{
		return OMX_MetadataScopePortLevel;
		}
	else
		{
		User::Leave(KErrArgument);
		return OMX_MetadataScopeTypeMax;	// unreachable, prevents compiler warning
		}
	}

OMX_BUFFERSUPPLIERTYPE COmxScriptParser::ParseOmxSupplierL(const TDesC8& aSupplierDes, TBool aAllowUnspecified)
	{
	if(aSupplierDes == _L8("input"))
		{
		return OMX_BufferSupplyInput;
		}
	else if(aSupplierDes == _L8("output"))
		{
		return OMX_BufferSupplyOutput;
		}
	else if (aAllowUnspecified && aSupplierDes == _L8("unspecified"))
		{
		return OMX_BufferSupplyUnspecified;
		}
	else
		{
		User::Leave(KErrArgument);
		return OMX_BufferSupplyInput;	// unreachable, prevents compiler warning
		}
	}

void COmxScriptParser::CheckForAbortL(TBool success)
	{
	if(!success)
		{
		iCallbackAborted = ETrue;
		User::Leave(KErrAbort);
		}
	}

TInt COmxScriptParser::ParseOptionalIntL(const RArray<RAttribute>& aArray, const TDesC8& aAttribName, TInt aDefaultValue)
	{
	const TDesC8* des = FindAttribute(aArray, aAttribName);
	if(des == NULL)
		{
		return aDefaultValue;
		}
	else
		{
		TInt result;
		TLex8 lex(*des);
		User::LeaveIfError(lex.Val(result));
		return result;
		}
	}

TReal COmxScriptParser::ParseOptionalRealL(const RArray<RAttribute>& aArray, const TDesC8& aAttribName, TReal aDefaultValue)
	{
	const TDesC8* des = FindAttribute(aArray, aAttribName);
	if(des == NULL)
		{
		return aDefaultValue;
		}
	else
		{
		TReal result;
		TLex8 lex(*des);
		User::LeaveIfError(lex.Val(result));
		return result;
		}
	}

OMX_COLOR_FORMATTYPE COmxScriptParser::ParseOmxColorFormatL(const TDesC8& aDes)
	{
	return ParseL(aDes, parse_OMX_COLOR_FORMATTYPE);
	}


OMX_COMMANDTYPE COmxScriptParser::ParseOmxCommandL(const TDesC8& aDes)
	{
	return ParseL(aDes, parse_OMX_COMMANDTYPE);
	}

OMX_ERRORTYPE COmxScriptParser::ParseOmxErrorL(const TDesC8& aDes)
	{
	return ParseL(aDes, parse_OMX_ERRORTYPE);
	}

OMX_EVENTTYPE COmxScriptParser::ParseOmxEventL(const TDesC8& aDes)
	{
	return ParseL(aDes, parse_OMX_EVENTTYPE);
	}

TBool COmxScriptParser::ParseOptionalBooleanL(const RArray<RAttribute>& aArray, const TDesC8& aAttribName, TBool aDefaultValue)
	{
	const TDesC8* des = FindAttribute(aArray, aAttribName);
	if(des == NULL)
		{
		return aDefaultValue;
		}

	return ParseBooleanL(*des);
	}

TBool COmxScriptParser::ParseBooleanL(const TDesC8& aBool)
    {
    if ((aBool == _L8("true")) ||
        (aBool == _L8("yes")) ||
        (aBool == _L8("1")) ||
        (aBool == _L8("ETrue")) ||
        (aBool == _L8("OMX_TRUE")))
        {
        return ETrue;
        }
        
    if ((aBool == _L8("false")) ||
        (aBool == _L8("no")) ||
        (aBool == _L8("0")) ||
        (aBool == _L8("EFalse")) ||
        (aBool == _L8("OMX_FALSE")))
        {
        return EFalse;
        }
        
    User::Leave(KErrArgument);
    return EFalse;
    }


TUint32 COmxScriptParser::ParseUint32L(const TDesC8& aDes)
	{
	TUint32 result;
	if(aDes.Find(_L8("0x")) == 0)
		{
		TLex8 lex(aDes.Mid(2));
		User::LeaveIfError(lex.Val(result, EHex));
		}
	else
		{
		TLex8 lex(aDes);
		User::LeaveIfError(lex.Val(result, EDecimal));
		}
	return result;
	}
	
TBool COmxScriptParser::ParseBoolean(const TDesC8& aDes)
	{
	if (aDes == _L8("true"))
		{
		return ETrue;
		}
	return EFalse;	
	}

TReal32 COmxScriptParser::ParseReal32L(const TDesC8& aDes)
	{
	TReal32 result;
	TLex8 lex(aDes);
	User::LeaveIfError(lex.Val(result));
	return result;
	}	
	
OMX_VIDEO_CODINGTYPE COmxScriptParser::ParseOmxVideoCodingL(const TDesC8& aDes)
	{
	if(aDes == _L8("unused"))
		{
		return OMX_VIDEO_CodingUnused;
		}
	else if (aDes == _L8("autodetect"))
		{
		return OMX_VIDEO_CodingAutoDetect;
		}
	else if(aDes == _L8("mpeg4"))
		{
		return OMX_VIDEO_CodingMPEG4;
		}
	else if(aDes == _L8("avc"))
		{
		return OMX_VIDEO_CodingAVC;
		}
	else if(aDes == _L8("max"))
		{
		return OMX_VIDEO_CodingMax;
		}
	else
		{
		__BREAKPOINT();
		User::Leave(KErrArgument);
		}

	return OMX_VIDEO_CodingUnused;
	}

OMX_AUDIO_CODINGTYPE COmxScriptParser::ParseOmxAudioCodingL(const TDesC8& aDes)
	{
	if(aDes == _L8("unused"))
		{
		return OMX_AUDIO_CodingUnused;
		}
	else if (aDes == _L8("autodetect"))
		{
		return OMX_AUDIO_CodingAutoDetect;
		}
	else if(aDes == _L8("pcm"))
		{
		return OMX_AUDIO_CodingPCM;
		}
	else if(aDes == _L8("aac"))
		{
		return OMX_AUDIO_CodingAAC;
		}	
	else if(aDes == _L8("max"))
		{
		return OMX_AUDIO_CodingMax;
		}
	else
		{
		__BREAKPOINT();
		User::Leave(KErrArgument);
		}

	return OMX_AUDIO_CodingUnused;
	}

OMX_TIME_REFCLOCKTYPE COmxScriptParser::ParseOmxRefClockTypeL(const TDesC8& aDes)
	{
	if(aDes == _L8("none"))
		{
		return OMX_TIME_RefClockNone;
		}
	else if (aDes == _L8("audio"))
		{
		return OMX_TIME_RefClockAudio;
		}
	else if(aDes == _L8("video"))
		{
		return OMX_TIME_RefClockVideo;
		}
	else
		{
		__BREAKPOINT();
		User::Leave(KErrArgument);
		}

	return OMX_TIME_RefClockNone;
	}

OMX_TIME_CLOCKSTATE COmxScriptParser::ParseOmxClockStateL(const TDesC8& aDes)
	{
	if(aDes == _L8("stopped"))
		{
		return OMX_TIME_ClockStateStopped;
		}
	else if (aDes == _L8("waiting"))
		{
		return OMX_TIME_ClockStateWaitingForStartTime;
		}
	else if(aDes == _L8("running"))
		{
		return OMX_TIME_ClockStateRunning;
		}
	else
		{
		__BREAKPOINT();
		User::Leave(KErrArgument);
		return OMX_TIME_ClockStateMax;	// unreachable, to prevent compiler warning		
		}
	}

TVideoFitMode COmxScriptParser::ParseVideoFitModeL(const TDesC8& aMode)
	{
	if(aMode == _L8("centre"))
		{
		return EVideoFitCentre;
		}
	else if(aMode == _L8("scaleAndCentre"))
		{
		return EVideoFitScaleAndCentre;
		}
	else if(aMode == _L8("rotateScaleAndCentre"))
		{
		return EVideoFitRotateScaleAndCentre;
		}
	else
		{
		__BREAKPOINT();
		User::Leave(KErrArgument);
		return EVideoFitCentre;	// unreachable, prevent compiler warning
		}
	}

OMX_VIDEO_MPEG4PROFILETYPE COmxScriptParser::ParseOmxMpeg4ProfileL(const TDesC8& aDes)
	{
	if(aDes == _L8("simple"))
		{
		return OMX_VIDEO_MPEG4ProfileSimple;
		}
	else if(aDes == _L8("main"))
		{
		return OMX_VIDEO_MPEG4ProfileMain;
		}
	else if (aDes == _L8("advancedrealtime"))
		{
 		return OMX_VIDEO_MPEG4ProfileAdvancedRealTime;
 		}
 	else if (aDes == _L8("advancedcoding"))
 		{
 		return OMX_VIDEO_MPEG4ProfileAdvancedCoding;
 		}
 	else if (aDes == _L8("core"))
 		{
 		return OMX_VIDEO_MPEG4ProfileCore;
 		}
	else
		{
		__BREAKPOINT();
		User::Leave(KErrArgument);
		}
	return OMX_VIDEO_MPEG4ProfileMax;	// unreachable, prevents compiler warning
	}
	
OMX_VIDEO_MPEG4LEVELTYPE COmxScriptParser::ParseOmxMpeg4LevelL(const TDesC8& aDes)
	{
	if(aDes == _L8("level0"))
		{
		return OMX_VIDEO_MPEG4Level0;
		}
	else if(aDes == _L8("level0b"))
		{
		return OMX_VIDEO_MPEG4Level0b;
		}
	else if (aDes == _L8("level1"))
		{
 		return OMX_VIDEO_MPEG4Level1;
 		}
 	else if (aDes == _L8("level2"))
 		{
 		return OMX_VIDEO_MPEG4Level2;
 		}
 	else if (aDes == _L8("level3"))
 		{
 		return OMX_VIDEO_MPEG4Level3;
 		}
	else if(aDes == _L8("level4"))
		{
		return OMX_VIDEO_MPEG4Level4;
		}
	else if (aDes == _L8("level5"))
 		{
 		return OMX_VIDEO_MPEG4Level5;
 		}
	else
		{
		__BREAKPOINT();
		User::Leave(KErrArgument);
		}
	return OMX_VIDEO_MPEG4LevelMax;	// unreachable, prevents compiler warning
	}	

OMX_VIDEO_CONTROLRATETYPE COmxScriptParser::ParseOmxControlRateL(const TDesC8& aDes)
	{
	if(aDes == _L8("disable"))
		{
		return OMX_Video_ControlRateDisable;
		}
	else if(aDes == _L8("variable"))
		{
		return OMX_Video_ControlRateVariable;
		}
	else if (aDes == _L8("constant"))
		{
 		return OMX_Video_ControlRateConstant;
 		}
	else
		{
		__BREAKPOINT();
		User::Leave(KErrArgument);
		}
	return OMX_Video_ControlRateMax;	// unreachable, prevents compiler warning
	}	
	
OMX_AUDIO_AACSTREAMFORMATTYPE COmxScriptParser::ParseAacStreamFormatL(const TDesC8& aStreamFormatStr)
 	{
 	if(aStreamFormatStr == _L8("mp2adts"))
 		{
 		return OMX_AUDIO_AACStreamFormatMP2ADTS;
 		}
 	else if(aStreamFormatStr == _L8("mp4adts"))
 		{
 		return OMX_AUDIO_AACStreamFormatMP4ADTS;
		}
 	else if(aStreamFormatStr == _L8("mp4loas"))
 		{
 		return OMX_AUDIO_AACStreamFormatMP4LOAS;
 		}
 	else if(aStreamFormatStr == _L8("mp4latm"))
 		{
 		return OMX_AUDIO_AACStreamFormatMP4LATM;
 		}
 	else if(aStreamFormatStr == _L8("adif"))
 		{
 		return OMX_AUDIO_AACStreamFormatADIF;
 		}
 	else if(aStreamFormatStr == _L8("mp4ff"))
 		{
 		return OMX_AUDIO_AACStreamFormatMP4FF;
 		}
 	else if(aStreamFormatStr == _L8("raw"))
 		{
 		return OMX_AUDIO_AACStreamFormatRAW;
 		}
 	else
 		{
 		User::Leave(KErrArgument);
 		return OMX_AUDIO_AACStreamFormatMax;
 		}
 	}
 
 OMX_NUMERICALDATATYPE COmxScriptParser::ParseNumericalDataL(const TDesC8& aDes)
 	{
 	if(aDes == _L8("signed"))
 		{
 		return OMX_NumericalDataSigned;
 		}
 	else if(aDes == _L8("unsigned"))
 		{
 		return OMX_NumericalDataUnsigned;
 		}
 	else
 		{
 		User::Leave(KErrArgument);
 		return OMX_NumercialDataMax;
 		}
 	}
 
 OMX_ENDIANTYPE COmxScriptParser::ParseEndianL(const TDesC8& aDes)
 	{
 	if(aDes == _L8("big"))
 		{
 		return OMX_EndianBig;
 		}
 	else if(aDes == _L8("little"))
 		{
 		return OMX_EndianLittle;
 		}
 	else
 		{
		User::Leave(KErrArgument);
 		return OMX_EndianMax;
 		}
 	}
 
 OMX_BOOL COmxScriptParser::ParseBoolL(const TDesC8& aDes)
 	{
 	if(aDes == _L8("true"))
 		{
 		return OMX_TRUE;
 		}
 	else if(aDes == _L8("false"))
 		{
 		return OMX_FALSE;
 		}
 	else
 		{
 		User::Leave(KErrArgument);
 		return OMX_FALSE;
 		}
 	}

