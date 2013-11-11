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


#ifndef OMXSCRIPTPARSER_H_
#define OMXSCRIPTPARSER_H_

#include <f32file.h>
#include <xml/contenthandler.h>
#include <xml/documentparameters.h>

#include <openmax/il/khronos/v1_x/OMX_Component.h>


// a bit naughty, bit it will do for now
using namespace Xml;

enum TVideoFitMode
	{
	EVideoFitCentre,
	EVideoFitScaleAndCentre,
	EVideoFitRotateScaleAndCentre
	};

enum TTransitionOrder
	{
	ELoadOrder,
	EAutoOrder
	};

/**
 * High-level elementary test actions. This interface can be driven by the COmxScriptParser.
 */
class MOmxScriptIf
	{
public:
	virtual TBool MosLoadComponentL(const TDesC8& aComp, const TDesC8& aName, TBool aBaseProfile, const TDesC8* aBaseImpl, TBool aLoadInCoreServerThread) = 0;
	virtual TBool MosSetupTunnel(const TDesC8& aSourceComp, TInt aSourcePort,
	                             const TDesC8& aSinkComp, TInt aSinkPort, OMX_ERRORTYPE aExpectedError) = 0;
	virtual TBool MosSetupNonTunnel(const TDesC8& aSourceComp, TInt aSourcePort,
                                  const TDesC8& aSinkComp, TInt aSinkPort,
                                  OMX_BUFFERSUPPLIERTYPE aSupplier) = 0;
	virtual TBool MosSetWindowL(const TDesC8& aComp) = 0;
	virtual TBool MosAllTransitionL(OMX_STATETYPE aState,OMX_ERRORTYPE aExpectedError, TTransitionOrder order) = 0;
	virtual TBool MosTransition(const TDesC8& aComp, OMX_STATETYPE aState, TBool aAsync) = 0;	
	virtual TBool MosFailingTransition(const TDesC8& aComp, OMX_STATETYPE aState, OMX_ERRORTYPE aExpectedError) = 0;
	virtual TBool MosWaitEOS() = 0;
	virtual TBool MosSetFilename(const TDesC8& aComp, const TDesC& aFilename) = 0;
	//meena
	virtual TBool MosSetFilename_bellagio(const TDesC8& aComp, char *aFilename) = 0;
	
	virtual	TBool MosGetFilename(const TDesC8& aComp, TDesC& aFilename, OMX_ERRORTYPE aExpectedError) = 0;
	virtual	TBool MosSetBadFilename(const TDesC8& aComp) = 0;

	virtual TBool MosSetBufferCount(const TDesC8& aComp, TInt aPortIndex, TInt aCount, OMX_ERRORTYPE aExpectedError) = 0;
	virtual TBool MosCompareFilesL(const TDesC& aFileOne, const TDesC& aFileTwo) = 0;
    virtual TBool MosFilterAndCompareFilesL(const TDesC& aFileOne, const TDesC8& aFile1Filter1, const TDesC8& aFile1Filter2, const TDesC8& aFile1Filter3,
                                                const TDesC& aFileTwo, const TDesC8& aFile2Filter1, const TDesC8& aFile2Filter2, const TDesC8& aFile2Filter3) = 0;	
	virtual TBool MosBufferSupplierOverrideL(const TDesC8& aSourceComp, TInt aSourcePort,
			                                 const TDesC8& aSinkComp, TInt aSinkPort,
			                                 OMX_BUFFERSUPPLIERTYPE aSupplier, 
			                                 OMX_ERRORTYPE aExpectedSourceError, OMX_ERRORTYPE aExpectedSinkError) = 0;
	virtual TBool MosSetVideoPortDefL(const TDesC8& aComp, TInt aPortIndex, TInt aWidth, TInt aHeight, OMX_COLOR_FORMATTYPE* aColorFormat, OMX_VIDEO_CODINGTYPE* aCodingType, TInt aStride, TReal aFps, OMX_ERRORTYPE aExpectedError) = 0;
	virtual TBool MosSetCameraOneShotL(const TDesC8& aComp, TInt aIsOneShot, OMX_ERRORTYPE aExpectedError) = 0;
	virtual TBool MosSetCameraCaptureL(const TDesC8& aComp, TInt aPortIndex, TInt aIsCapturing, OMX_ERRORTYPE aExpectedError) = 0;
	virtual void MosParseError(const TDesC& aError) = 0;
	virtual TBool MosWaitL(TTimeIntervalMicroSeconds32 aDelay) = 0;
	virtual TBool MosExpectEventL(const TDesC8& aComp, OMX_EVENTTYPE aEvent, TUint32 nData1, TUint32 nData2) = 0;
	virtual TBool MosWaitForAllEventsL() = 0;
	virtual TBool MosCheckStateL(const TDesC8& aComp, OMX_STATETYPE aState) = 0;
	virtual TBool MosCheckVideoPortDefL(const TDesC8& aComp, TInt aPortIndex, TInt aWidth, TInt aHeight, OMX_VIDEO_CODINGTYPE aCoding, OMX_COLOR_FORMATTYPE aColorFormat) = 0;
	virtual TBool MosCheckMetaDataL(const TDesC8& aComp, TInt aPortIndex, OMX_METADATASCOPETYPE aScope, const TDesC8& aAtomType, TUint32 aAtomIndex, const TDesC8& aData) = 0;
	virtual TBool MosGetParameterUnknownIndexTypeL(const TDesC8& aComp, TInt aPortIndex, OMX_METADATASCOPETYPE aScope, const TDesC8& aAtomType, TUint32 aAtomIndex, const TDesC8& aData) = 0;
	virtual TBool MosSetParameterUnknownIndexTypeL(const TDesC8& aComp, TInt aPortIndex, OMX_METADATASCOPETYPE aScope, const TDesC8& aAtomType, TUint32 aAtomIndex, const TDesC8& aData) = 0;

	virtual TBool MosDisablePort(const TDesC8& aComp, TInt aPortIndex) = 0;
	virtual TBool MosEnablePort(const TDesC8& aComp, TInt aPortIndex) = 0;
	virtual TBool MosIgnoreEventL(const TDesC8& aComp, OMX_EVENTTYPE aEvent, TUint32 nData1, TUint32 nData2) = 0;
	virtual TBool MosSetPcmAudioPortDefL(const TDesC8& aComp, TInt aPortIndex, TInt aNumChannels, TInt aSamplingRate, TInt aBitsperSample, OMX_NUMERICALDATATYPE aNumData, OMX_ENDIANTYPE aEndian, OMX_BOOL* aInterleaved, const TDesC8* aEncoding) = 0;
	virtual TBool MosSetConfigAudioVolumeL(const TDesC8& aComp, TInt aPortIndex, TBool aLinear, TInt aMinVolume, TInt aMaxVolume, TInt aVolume, OMX_ERRORTYPE aExpectedError) = 0;
	virtual TBool MosCheckConfigAudioVolumeL(const TDesC8& aComp, TInt aPortIndex, TBool aLinear, TInt aMinVolume, TInt aMaxVolume, TInt aVolume) = 0;
	virtual TBool MosCheckConfigAudioMuteL(const TDesC8& aComp, TInt aPortIndex, TBool aMute) = 0;
	virtual TBool MosSetConfigAudioMuteL(const TDesC8& aComp, TInt aPortIndex, TBool aMute) = 0;
	virtual TBool MosSetAacAudioPortDefL(const TDesC8& aComp, TInt aPortIndex, TInt aNumChannels, TInt aSamplingRate, TInt aBitRate, TInt aAudioBandwidth, TInt aFrameLength, TInt aAacTools, TInt aAacErTools, TInt aProfile, TInt aStreamFormat, TInt aChannelMode) = 0;
	virtual TBool MosSetAudioPortDefL(const TDesC8& aComp, TInt aPortIndex, OMX_AUDIO_CODINGTYPE* aCodingType, OMX_ERRORTYPE aExpectedError) = 0;
	virtual TBool MosSetRefClockTypeL(const TDesC8& aComp, OMX_TIME_REFCLOCKTYPE aRefClockType) = 0;
	virtual TBool MosSetClockTimeScale(const TDesC8& aComp, OMX_S32 aScale) = 0;
	virtual TBool MosSetClockStateL(const TDesC8& aComp, OMX_TIME_CLOCKSTATE aClockState, TInt aStartTime, TInt aOffset, TUint32 aWaitMask) = 0;
	virtual TBool MosCheckClockStateL(const TDesC8& aComp, OMX_TIME_CLOCKSTATE aClockState) = 0;
	virtual TBool MosCheckTimePositionL(const TDesC8& aComp, TInt aPortIndex, TInt aTimestamp) = 0;
	virtual TBool MosSetTimePositionL(const TDesC8& aComp, TInt aPortIndex, TInt aTimestamp) = 0;
	virtual TBool MosSetClientStartTimeL(const TDesC8& aComp, TInt aPortIndex, TInt aTimestamp) = 0;
	virtual TBool MosSetCurrentAudioReferenceTimeL(const TDesC8& aComp, TInt aPortIndex, TInt aTimestamp) = 0;
	virtual TBool MosSetVideoFitModeL(TVideoFitMode aVideoFitMode) = 0;
	virtual TBool MosNegativeSetDataChunk(const TDesC8& aComp, OMX_ERRORTYPE aExpectedError, TInt aExpectedSystemError) = 0;
	virtual TBool MosSetBufferSize(const TDesC8& aComp, TInt aPortIndex, TInt aCount, OMX_ERRORTYPE aExpectedError) = 0;
	virtual TBool MosGetAndCompareBufferCount(const TDesC8& aComp, TInt aPortIndex, TInt aDataToCompare) = 0;
	virtual TBool MosGetAndCompareBufferSize(const TDesC8& aComp, TInt aPortIndex, TInt aDataToCompare) = 0;
	virtual TBool MosFlushBuffer(const TDesC8& aComp, TInt aPortIndex, OMX_ERRORTYPE aExpectedError) = 0;
	virtual TBool MosForceBufferSourceFlushBufferL(const TDesC8& aComp, TInt aPortIndex, OMX_ERRORTYPE aExpectedError) = 0;
	virtual TBool MosForceBufferSinkFlushBuffer(const TDesC8& aComp, TInt aPortIndex, OMX_ERRORTYPE aExpectedError) = 0;
	virtual TBool MosSetActiveStream(const TDesC8& aComp, TUint32 aPortIndex)= 0;
	virtual TBool MosGetActiveStream(const TDesC8& aComp, TUint32 aPortIndex) = 0;
	virtual TBool MosSetDroppedFrameEvent(const TDesC8& aComp, const TDesC8& aAction) = 0;
	virtual TBool MosCheckFrameCountL(const TDesC& aFilename, TInt aCount) = 0;
	virtual TBool MosMarkBuffer(const TDesC8& aComp, TInt aPortIndex, const TDesC8& aTargetComp, TInt markData) = 0;
	// mpeg4 encoder component
    virtual TBool MosSetVideoEncQuantL(const TDesC8& aComp, TInt aPortIndex, TInt aQpb) = 0;
    virtual TBool MosSetVideoEncMotionVectL(const TDesC8& aComp, TInt aPortIndex, TInt aAccuracy, TInt aSxSearchrange, TInt aSySearchrange, OMX_ERRORTYPE aExpectedError) = 0;
    virtual TBool MosSetVideoEncMpeg4TypeL(const TDesC8& aComp, TInt aPortIndex, OMX_VIDEO_MPEG4PROFILETYPE aMpeg4Profile, OMX_VIDEO_MPEG4LEVELTYPE aMpeg4Level, OMX_ERRORTYPE aExpectedError) = 0;
	virtual TBool MosSetVideoEncBitRateL(const TDesC8& aComp, TInt aPortIndex, OMX_VIDEO_CONTROLRATETYPE aControlRate, TInt aTargetBitrate, OMX_ERRORTYPE aExpectedError) = 0;
    virtual TBool MosChangeFilledBufferLength(const TDesC8& aComp, TInt aValue) = 0;
    virtual TBool MosSetOMX_SymbianIndexParamBufferMsgQueueData(const TDesC8& aComp, OMX_ERRORTYPE aExpectedError) = 0;
    virtual TBool MosGetExtensionIndex(const TDesC8& aComp, const TDesC8& aParameterName, OMX_ERRORTYPE aExpectedError) = 0;

    virtual TBool MosCheckTimeClockState(const TDesC8& aComp, OMX_TIME_CLOCKSTATE aExpectedState) = 0;
    virtual TBool MosCheckMediaTime(const TDesC8& aComp, TInt aPortIndex, OMX_TICKS aMediaTime, TBool aMoreThan) = 0;
    
    // Buffer component handler
	virtual TBool InitialiseBufferHandlerL(const TDesC8& aComp, TDesC& aName, const TDesC8* aType) = 0;
	virtual void StartBufferHandler() = 0;
	virtual void StopBufferHandler() = 0;
	virtual TBool InitialiseBufferSinkHandlerL(const TDesC8& aComp, TDesC& aName) = 0;
    virtual void StartBufferSinkHandler() = 0;
    virtual void StopBufferSinkHandler() = 0;
    virtual void SendInvalidBufferId(TInt aInvalidId) = 0;

	virtual TBool MosWaitEOS(const TDesC8& aComp) = 0;

	virtual TBool MosSetAACProfileL(const TDesC8& aComp, TInt aPortIndex, TInt aChannels, TInt aSamplingRate, TInt aBitRate, TInt aAudioBandwidth, TInt aFrameLength, TInt aAACTools, TInt aAACERTools, OMX_AUDIO_AACPROFILETYPE aProfile, OMX_AUDIO_AACSTREAMFORMATTYPE aStreamFormat, OMX_AUDIO_CHANNELMODETYPE aChannelMode) = 0;
	virtual TBool MosSetupBufferForPortL(const TDesC8& comp,TInt aPortIndex, const TDesC& aFileName,TInt aHeaderLength,OMX_BUFFERSUPPLIERTYPE aSupplier) = 0;
	
	// Camera

	virtual TBool MosSetSensorModeTypeL(const TDesC8& aComp, TInt aPort, TInt aFrameRate, TBool aOneShot, TInt aWidth, TInt aHeight) = 0;
	virtual TBool MosSetCaptureModeTypeL(const TDesC8& aComp, TInt aPort, TBool aContinuous, TBool aFrameLimited, TInt aFrameLimit) = 0;
	
	
	// Debugging/Utility functions.
	virtual TBool MosDeleteFileL(const TDesC& aFileName, TBool aFileMustExist) = 0;
	virtual TBool MosLogAllEventsL() = 0;

	// Methods supporting generic Base Profile communications support with components
	virtual TBool MosBaseSupportPortL(const TDesC8& aComp, TInt aPortIndex) = 0;
	virtual TBool MosBaseSetAutonomous(const TDesC8& aComp, TInt aPortIndex, TBool aEnabled) = 0;
	virtual TBool MosBaseAllocateBuffersL(const TDesC8& aComp, TInt aPortIndex, TInt aNumberBuffers) = 0;
	virtual TBool MosBaseFreeAllocatedBuffersL(const TDesC8& aComp) = 0;
	virtual TBool MosBaseSetBufSupplier(const TDesC8& aComp, TInt aPortIndex, TBool aComponentSupplier) = 0;
	virtual TBool MosBaseFillThisBuffer(const TDesC8& aComp, TInt aPortIndex, TInt aPortRelBufIndex) = 0;
    virtual TBool MosBaseEmptyThisBuffer(const TDesC8& aComp, TInt aPortIndex, TInt aPortRelBufIndex) = 0;
	virtual TBool MosBaseWaitForBuffer(const TDesC8& aComp, TInt aPortIndex, TInt aPortRelBufIndex) = 0;	

	// Methods supporting the Timestamp checking implementation for Base Profile testing
	virtual TBool MosBaseTimestampPassClock(const TDesC8& aCompReceiving, const TDesC8& aClockCompToPass) = 0;
	virtual TBool MosBaseTimestampCheckTimestampL(const TDesC8& aComp, TInt aPortIndex, TUint aExpectedTime, TUint aTolerance) = 0;
	virtual TBool MosBaseTimestampCompareWithRefClockL(const TDesC8& aComp, TInt aPortIndex, TUint aTolerance) = 0;
	
	virtual TBool MosStartBuffersforPort(  const TDesC8& aComp, TInt aPortIndex) = 0;
#ifdef OLD_ADPCM_EXTENSION
	virtual TBool MosSetAdPcmAudioPortDefL(const TDesC8& aComp, TInt aPortIndex, TInt aNumChannels, TInt aSamplingRate, TInt aBitsperSample) = 0;
    virtual TBool MosConfigAdPcmDecoderBlockAlign(const TDesC8& aComp, OMX_S32 aBlockAlign) = 0;
#endif
	};

/**
 * This parses an XML file to drive the operation of a MOmxScriptIf.
 */
NONSHARABLE_CLASS(COmxScriptParser) : public CBase, public Xml::MContentHandler
	{
public:
	static COmxScriptParser* NewL(RFs& aFs, const TDesC& aFilename, MOmxScriptIf& aCallback);
	~COmxScriptParser();
	
	TBool RunScriptL(const TDesC& aSectionName);

	// Xml::MContentHandler interface
	void OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode);
	void OnEndDocumentL(TInt aErrorCode);
	void OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, 
			TInt aErrorCode);
	void OnEndElementL(const RTagInfo& aElement, TInt aErrorCode);
	void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
	void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, 
			TInt aErrorCode);
	void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
	void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
	void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
	void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, 
			TInt aErrorCode);
	void OnError(TInt aErrorCode);
	TAny* GetExtendedInterface(const TInt32 aUid);

protected:
	COmxScriptParser(RFs& aFs, MOmxScriptIf& aCallback);
	void ConstructL(const TDesC& aFilename);
	
private:
	static const TDesC8* FindAttribute(const RArray<RAttribute>& aArray, const TDesC8& aAttribName);
	const TDesC8& FindAttributeL(const RArray<RAttribute>& aArray, const TDesC8& aAttribName);
	static void ParseCompPortL(const TDesC8& aInput, TPtrC8& aNameOut, TInt& aPortOut);
	static OMX_STATETYPE ParseOmxStateL(const TDesC8& aStateDes);
	static OMX_BUFFERSUPPLIERTYPE ParseOmxSupplierL(const TDesC8& aSupplierDes, TBool aAllowUnspecified = EFalse);
	void CheckForAbortL(TBool success);
	static TInt ParseOptionalIntL(const RArray<RAttribute>& aArray, const TDesC8& aAttribName, TInt aDefaultValue);
	static TReal ParseOptionalRealL(const RArray<RAttribute>& aArray, const TDesC8& aAttribName, TReal aDefaultValue);
	static OMX_COLOR_FORMATTYPE ParseOmxColorFormatL(const TDesC8& aDes);
	static OMX_EVENTTYPE ParseOmxEventL(const TDesC8& aEventDes);
	static TUint32 ParseUint32L(const TDesC8& aDes);
	static TReal32 ParseReal32L(const TDesC8& aDes);
	static OMX_VIDEO_CODINGTYPE ParseOmxVideoCodingL(const TDesC8& aDes);
	static TBool ParseOptionalBooleanL(const RArray<RAttribute>& aArray, const TDesC8& aAttribName, TBool aDefaultValue);
	static TBool ParseBooleanL(const TDesC8& aBool);

	static OMX_METADATASCOPETYPE ParseOmxScopeTypeL(const TDesC8& aScopeDes);
	static OMX_AUDIO_CODINGTYPE ParseOmxAudioCodingL(const TDesC8& aDes);
	static OMX_TIME_REFCLOCKTYPE ParseOmxRefClockTypeL(const TDesC8& aDes);
	static OMX_TIME_CLOCKSTATE ParseOmxClockStateL(const TDesC8& aDes);
	static TVideoFitMode ParseVideoFitModeL(const TDesC8& aMode);
	static OMX_COMMANDTYPE ParseOmxCommandL(const TDesC8& aDes);
	static OMX_ERRORTYPE ParseOmxErrorL(const TDesC8& aDes);
	static TBool ParseBoolean(const TDesC8& aDes);

    static OMX_ERRORTYPE ParseOmxErrorCode(const TDesC8& aErrorCode);
    static OMX_VIDEO_MPEG4PROFILETYPE ParseOmxMpeg4ProfileL(const TDesC8& aDes);
    static OMX_VIDEO_MPEG4LEVELTYPE ParseOmxMpeg4LevelL(const TDesC8& aDes);
    static OMX_VIDEO_CONTROLRATETYPE ParseOmxControlRateL(const TDesC8& aDes);
	static OMX_AUDIO_AACPROFILETYPE ParseOmxAACProfileL(const TDesC8& aProfileDes);
	static OMX_AUDIO_AACSTREAMFORMATTYPE ParseOmxAACStreamFormatL(const TDesC8& aFormatDes);
	static OMX_AUDIO_CHANNELMODETYPE ParseOmxAudioChannelModeL(const TDesC8& aChannelModeDes);
    static TInt ParseSystemErrorCode(const TDesC8& aErrorCode);
    static OMX_AUDIO_AACSTREAMFORMATTYPE ParseAacStreamFormatL(const TDesC8& aStreamFormatStr);
	static OMX_NUMERICALDATATYPE ParseNumericalDataL(const TDesC8& aDes);
	static OMX_ENDIANTYPE ParseEndianL(const TDesC8& aDes);
	static OMX_BOOL ParseBoolL(const TDesC8& aDes);

private:
	RFs& iFs;
	RBuf iFilename;
	MOmxScriptIf& iCallback;
	
	// state to allow filtering for a specific section within the XML document
	const TDesC* iSectionName;	// not owned
	TBool iInTest;
	TBool iFoundTest;
	TBool iCallbackAborted;
	};
	
#endif /*OMXSCRIPTPARSER_H_*/
