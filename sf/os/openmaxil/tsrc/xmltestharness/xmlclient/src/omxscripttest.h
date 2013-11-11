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


#ifndef OMXSCRIPTTEST_H_
#define OMXSCRIPTTEST_H_

#include "omxscriptparser.h"
#include "asbreakeventhandler.h"
#include "videotesttimer.h"
#include <e32msgqueue.h>
#include "omxthreadrequest.h"
#include "omx_xml_script.h"
//#include <multimedia/mmfbuffershared.h>
#include <openmax/il/khronos/v1_x/OMX_IVCommon.h>
#include <openmax/il/khronos/v1_x/OMX_Types.h>
#include "portbufferhandler.h"

class CNonTunneledHandler;
class CVideoBufferHandler;
class CVideoBufferSinkHandler;
class CBaseProfileHandler;
class CWindowManager;

const OMX_VERSIONTYPE KOmxVersion = { OMX_VERSION_MAJOR, OMX_VERSION_MINOR, OMX_VERSION_REVISION, OMX_VERSION_STEP };

/**
 * Class to parse and execute an OMX test case specified in
 * an XML file.
 */
NONSHARABLE_CLASS(ROmxScriptTest) : public MOmxScriptIf, public MASBreakCallback, public MTimerObserver
	{
public:
    
    NONSHARABLE_CLASS(CComponentInfo) : public CBase
        {
    public:
        static CComponentInfo* NewL(ROmxScriptTest& aTestController);
        ~CComponentInfo();
    
    private:
        CComponentInfo(ROmxScriptTest& aTestController);
        void ConstructL();
        
    public:
        HBufC8* iShortName;
        HBufC8* iComponentName;
        OMX_COMPONENTTYPE* iComponent;
        CBaseProfileHandler* iBaseHandler;    //NULL if not being used via OMX IL Base Profile
        COmxThreadRequest* iThreadRequest;
        
    private:
        ROmxScriptTest& iTestController;        
        };
        
    class TExpectedEvent
        {
    public:
        OMX_COMPONENTTYPE* iComponent;
        OMX_EVENTTYPE iEvent;
        TUint32 iData1;
        TUint32 iData2;
        TAny* iEventData;
        };
    typedef void (*THashFilter)(RBuf8&);
    THashFilter GetHashFilterByName(const TDesC8& aName);
    
	ROmxScriptTest(MOmxScriptTestLogger& aLogger);
	void Close();
	
	void RunTestL(const TDesC& aScriptFilename, const TDesC& aScriptSection);
	
	// MOmxScriptIf
	TBool MosLoadComponentL(const TDesC8& aComp, const TDesC8& aName, TBool aBaseProfile, const TDesC8* aBaseImpl, TBool aLoadInCoreServerThread);
	TBool MosSetupTunnel(const TDesC8& aSourceComp, TInt aSourcePort,
			             const TDesC8& aSinkComp, TInt aSinkPort, OMX_ERRORTYPE aExpectedError);
	TBool MosSetupNonTunnel(const TDesC8& aSourceComp, TInt aSourcePort,
                          const TDesC8& aSinkComp, TInt aSinkPort,
                          OMX_BUFFERSUPPLIERTYPE aSupplier);
	TBool MosSetWindowL(const TDesC8& aComp);
	TBool MosSetDroppedFrameEvent(const TDesC8& aComp, const TDesC8& aAction);
	TBool MosAllTransitionL(OMX_STATETYPE aState,OMX_ERRORTYPE aExpectedError, TTransitionOrder aOrder);
	TBool MosWaitEOS();
	TBool MosWaitEOS(const TDesC8& aComp);
	TBool MosSetFilename(const TDesC8& aComp, const TDesC& aFilename);

	TBool MosSetFilename_bellagio(const TDesC8& aComp, char *filepath);
	
	TBool MosSetBadFilename(const TDesC8& aComp);
	TBool MosGetFilename(const TDesC8& aComp, TDesC& aFilename, OMX_ERRORTYPE aExpectedError);

	TBool MosSetBufferCount(const TDesC8& aComp, TInt aPortIndex, TInt aCount, OMX_ERRORTYPE aExpectedError);
	TBool MosCompareFilesL(const TDesC& aFileOne, const TDesC& aFileTwo);
    TBool MosFilterAndCompareFilesL(
            const TDesC& aFileOne, const TDesC8& aFile1Filter1, const TDesC8& aFile1Filter2, const TDesC8& aFile1Filter3,
            const TDesC& aFileTwo, const TDesC8& aFile2Filter1, const TDesC8& aFile2Filter2, const TDesC8& aFile2Filter3);
	TBool MosBufferSupplierOverrideL(const TDesC8& aSourceComp, TInt aSourcePort,
				                     const TDesC8& aSinkComp, TInt aSinkPort,
				                     OMX_BUFFERSUPPLIERTYPE aSupplier, 
				                     OMX_ERRORTYPE aExpectedSourceError, OMX_ERRORTYPE aExpectedSinkError);
	TBool MosSetVideoPortDefL(const TDesC8& aComp, TInt aPortIndex, TInt aWidth, TInt aHeight, OMX_COLOR_FORMATTYPE* aColorFormat, OMX_VIDEO_CODINGTYPE* aCodingType, TInt aStride, TReal aFps, OMX_ERRORTYPE aExpectedError);
	TBool MosSetCameraOneShotL(const TDesC8& aComp, TInt aIsOneShot, OMX_ERRORTYPE aExpectedError);
	TBool MosSetCameraCaptureL(const TDesC8& aComp, TInt aPortIndex, TInt aIsCapturing, OMX_ERRORTYPE aExpectedError);
	void MosParseError(const TDesC& aError);
	TBool MosWaitL(TTimeIntervalMicroSeconds32 aDelay);
	TBool MosExpectEventL(const TDesC8& aComp, OMX_EVENTTYPE aEvent, TUint32 nData1, TUint32 nData2);
	TBool MosWaitForAllEventsL();
	TBool MosTransition(const TDesC8& aComp, OMX_STATETYPE aState, TBool aAsync);
	TBool MosFailingTransition(const TDesC8& aComp, OMX_STATETYPE aState,OMX_ERRORTYPE aExpectedError);
	TBool MosCheckStateL(const TDesC8& aComp, OMX_STATETYPE aState);	
	TBool MosCheckVideoPortDefL(const TDesC8& aComp, TInt aPortIndex, TInt aWidth, TInt aHeight, OMX_VIDEO_CODINGTYPE aCoding, OMX_COLOR_FORMATTYPE aColorFormat);
	TBool MosCheckMetaDataL(const TDesC8& aComp, TInt aPortIndex, OMX_METADATASCOPETYPE aScope, const TDesC8& aAtomType, TUint32 aAtomIndex, const TDesC8& aData);
	TBool MosGetParameterUnknownIndexTypeL(const TDesC8& aComp, TInt aPortIndex, OMX_METADATASCOPETYPE aScope, const TDesC8& aAtomType, TUint32 aAtomIndex, const TDesC8& aData);
	TBool MosSetParameterUnknownIndexTypeL(const TDesC8& aComp, TInt aPortIndex, OMX_METADATASCOPETYPE aScope, const TDesC8& aAtomType, TUint32 aAtomIndex, const TDesC8& aData);

	TBool MosDisablePort(const TDesC8& aComp, TInt aPortIndex);
	TBool MosEnablePort(const TDesC8& aComp, TInt aPortIndex);
	TBool MosIgnoreEventL(const TDesC8& aComp, OMX_EVENTTYPE aEvent, TUint32 nData1, TUint32 nData2);
	TBool MosSetPcmAudioPortDefL(const TDesC8& aComp, TInt aPortIndex, TInt aNumChannels, TInt aSamplingRate, TInt aBitsperSample, OMX_NUMERICALDATATYPE aNumData, OMX_ENDIANTYPE aEndian, OMX_BOOL* aInterleaved, const TDesC8* aEncoding);
	TBool MosSetConfigAudioVolumeL(const TDesC8& aComp, TInt aPortIndex, TBool aLinear, TInt aMinVolume, TInt aMaxVolume, TInt aVolume, OMX_ERRORTYPE aExpectedError);
	TBool MosCheckConfigAudioVolumeL(const TDesC8& aComp, TInt aPortIndex, TBool aLinear, TInt aMinVolume, TInt aMaxVolume, TInt aVolume);
	TBool MosCheckConfigAudioMuteL(const TDesC8& aComp, TInt aPortIndex, TBool aMute);
	TBool MosSetConfigAudioMuteL(const TDesC8& aComp, TInt aPortIndex, TBool aMute);
	TBool MosSetAacAudioPortDefL(const TDesC8& aComp, TInt aPortIndex, TInt aNumChannels, TInt aSamplingRate, TInt aBitRate, TInt aAudioBandwidth, TInt aFrameLength, TInt aAacTools, TInt aAacErTools, TInt aProfile, TInt aStreamFormat, TInt aChannelMode);
	TBool MosSetAudioPortDefL(const TDesC8& aComp, TInt aPortIndex, OMX_AUDIO_CODINGTYPE* aCodingType, OMX_ERRORTYPE aExpectedError);
	TBool MosSetRefClockTypeL(const TDesC8& aComp, OMX_TIME_REFCLOCKTYPE aRefClockType);
	TBool MosSetClockTimeScale(const TDesC8& aComp, OMX_S32 aScale);
	TBool MosSetClockStateL(const TDesC8& aComp, OMX_TIME_CLOCKSTATE aClockState, TInt aStartTime, TInt aOffset, TUint32 aWaitMask);
	TBool MosCheckClockStateL(const TDesC8& aComp, OMX_TIME_CLOCKSTATE aClockState);
	TBool MosCheckTimePositionL(const TDesC8& aComp, TInt aPortIndex, TInt aTimestamp);
	TBool MosSetTimePositionL(const TDesC8& aComp, TInt aPortIndex, TInt aTimestamp);
	TBool MosSetClientStartTimeL(const TDesC8& aComp, TInt aPortIndex, TInt aTimestamp);
	TBool MosSetCurrentAudioReferenceTimeL(const TDesC8& aComp, TInt aPortIndex, TInt aTimestamp);
	TBool MosSetVideoFitModeL(TVideoFitMode aVideoFitMode);
	TBool MosNegativeSetDataChunk(const TDesC8& aComp, OMX_ERRORTYPE aExpectedError, TInt aExpectedSystemError);
	TBool MosCheckFrameCountL(const TDesC& aFilename, TInt aCount);
    TBool InitialiseBufferHandlerL(const TDesC8& aComp, TDesC& aName, const TDesC8* aType);
	void StartBufferHandler();
	void StopBufferHandler();
	void SendInvalidBufferId(TInt aInvalidId);
	TBool InitialiseBufferSinkHandlerL(const TDesC8& aComp, TDesC& aName);
    void StartBufferSinkHandler();
    void StopBufferSinkHandler();

	TBool MosSetBufferSize(const TDesC8& aComp, TInt aPortIndex, TInt aCount, OMX_ERRORTYPE aExpectedError);
	TBool MosGetAndCompareBufferCount(const TDesC8& aComp, TInt aPortIndex, TInt aDataToCompare);
	TBool MosGetAndCompareBufferSize(const TDesC8& aComp, TInt aPortIndex, TInt aDataToCompare);
	TBool MosFlushBuffer(const TDesC8& aComp, TInt aPortIndex, OMX_ERRORTYPE aExpectedError);
	TBool MosForceFlushBuffer(const TDesC8& aComp, TInt aPortIndex, OMX_ERRORTYPE aExpectedError);
	TBool MosSetActiveStream(const TDesC8& aComp, TUint32 aPortIndex);
	TBool MosGetActiveStream(const TDesC8& aComp, TUint32 aPortIndex);
	TBool MosChangeFilledBufferLength(const TDesC8& aComp, TInt aValue);
	TBool MosSetOMX_SymbianIndexParamBufferMsgQueueData(const TDesC8& aComp, OMX_ERRORTYPE aExpectedError);
	TBool MosGetExtensionIndex(const TDesC8& aComp, const TDesC8& aParameterName, OMX_ERRORTYPE aExpectedError);

	TBool MosCheckTimeClockState(const TDesC8& aComp, OMX_TIME_CLOCKSTATE aExpectedState);
	TBool MosCheckMediaTime(const TDesC8& aComp, TInt aPortIndex, OMX_TICKS aMediaTime, TBool aMoreThan);
	TBool MosBaseSupportPortL(const TDesC8& aComp, TInt aPortIndex);
	TBool MosBaseSetAutonomous(const TDesC8& aComp, TInt aPortIndex, TBool aEnabled);
	TBool MosBaseAllocateBuffersL(const TDesC8& aComp, TInt aPortIndex, TInt aNumberBuffers);
	TBool MosBaseFreeAllocatedBuffersL(const TDesC8& aComp);
	TBool MosBaseSetBufSupplier(const TDesC8& aComp, TInt aPortIndex, TBool aComponentSupplier);
	TBool MosBaseFillThisBuffer(const TDesC8& aComp, TInt aPortIndex, TInt aPortRelBufIndex);
    TBool MosBaseEmptyThisBuffer(const TDesC8& aComp, TInt aPortIndex, TInt aPortRelBufIndex);
    TBool MosBaseWaitForBuffer(const TDesC8& aComp, TInt aPortIndex, TInt aPortRelBufIndex);
    TBool MosBaseTimestampPassClock(const TDesC8& aCompReceiving, const TDesC8& aClockCompToPass);
	TBool MosBaseTimestampCheckTimestampL(const TDesC8& aComp, TInt aPortIndex, TUint aExpectedTime, TUint aTolerance);
	TBool MosBaseTimestampCompareWithRefClockL(const TDesC8& aComp, TInt aPortIndex, TUint aTolerance);
	TBool MosMarkBuffer(const TDesC8& aComp, TInt aPortIndex, const TDesC8& aTargetComp, TInt markData);
	
	// mpeg4 encoder component
	TBool MosSetVideoEncQuantL(const TDesC8& aComp, TInt aPortIndex, TInt aQpb);
    TBool MosSetVideoEncMotionVectL(const TDesC8& aComp, TInt aPortIndex, TInt aAccuracy, TInt aSxSearchrange, TInt aSySearchrange, OMX_ERRORTYPE aExpectedError);
    TBool MosSetVideoEncMpeg4TypeL(const TDesC8& aComp, TInt aPortIndex, OMX_VIDEO_MPEG4PROFILETYPE aMpeg4Profile, OMX_VIDEO_MPEG4LEVELTYPE aMpeg4Level, OMX_ERRORTYPE aExpectedError);
	TBool MosSetVideoEncBitRateL(const TDesC8& aComp, TInt aPortIndex, OMX_VIDEO_CONTROLRATETYPE aControlRate, TInt aTargetBitrate, OMX_ERRORTYPE aExpectedError);
	
	TBool MosSetAACProfileL(const TDesC8& aComp, TInt aPortIndex, TInt aChannels, TInt aSamplingRate, TInt aBitRate, TInt aAudioBandwidth, TInt aFrameLength, TInt aAACTools, TInt aAACERTools, OMX_AUDIO_AACPROFILETYPE aProfile, OMX_AUDIO_AACSTREAMFORMATTYPE aStreamFormat, OMX_AUDIO_CHANNELMODETYPE aChannelMode);
	
	// Camera
	
	TBool MosSetSensorModeTypeL(const TDesC8& aComp, TInt aPort, TInt aFrameRate, TBool aOneShot, TInt aWidth, TInt aHeight);
	TBool MosSetCaptureModeTypeL(const TDesC8& aComp, TInt aPort, TBool aContinuous, TBool aFrameLimited, TInt aFrameLimit);


	// Debugging/Utility functions.
	TBool MosDeleteFileL(const TDesC& aFileName, TBool aFileMustExist);
	TBool MosLogAllEventsL();

	// MASBreakCallback
	void AllComponentsEOS();
	void AllComponentsTransitioned(OMX_STATETYPE aNewState,
				                   OMX_STATETYPE aOldState);
	void EventReceived(OMX_COMPONENTTYPE* aComponent, OMX_EVENTTYPE aEvent, TUint32 aData1, TUint32 aData2, TAny* aEventData);
	void ComponentTransitioned(OMX_STATETYPE aNewState,
				                   OMX_STATETYPE aOldState);
	void BufferDone(OMX_COMPONENTTYPE* aComponent, OMX_BUFFERHEADERTYPE* aBufHdr, TBool aSource);
				                   
	// MTimerObserver
	void TimerExpired();
	
	TBool MosSetupBufferForPortL(const TDesC8& comp,TInt aPortIndex, const TDesC& aFileName,TInt aHeaderLength,OMX_BUFFERSUPPLIERTYPE aSupplier);
	TBool MosStartBuffersforPort(const TDesC8& aComp, TInt aPortIndex);
#ifdef OLD_ADPCM_EXTENSION
	TBool MosSetAdPcmAudioPortDefL(const TDesC8& aComp, TInt aPortIndex, TInt aNumChannels, TInt aSamplingRate, TInt aBitsperSample);
    TBool MosConfigAdPcmDecoderBlockAlign(const TDesC8& aComp, OMX_S32 aBlockAlign);  
#endif
	
public:
	enum TOmxScriptTestVerdict
		{
		EPass, EFail, EPending
		};

	TInt Reason() const;
	TOmxScriptTestVerdict Verdict() const;
    void FailWithOmxError(const TDesC& aOperation, OMX_ERRORTYPE aError);   
    void StopTest(TOmxScriptTestVerdict aVerdict);
    void StopTest(TInt aError, TOmxScriptTestVerdict aVerdict);
    void FailTest(const TDesC& aErrorMsg);   

	TBool MosForceBufferSourceFlushBufferL(const TDesC8& aComp, TInt aPortIndex, OMX_ERRORTYPE aExpectedError);
	TBool MosForceBufferSinkFlushBuffer(const TDesC8& aComp, TInt aPortIndex, OMX_ERRORTYPE aExpectedError);

    static TBool ShortNameMatchComparison(const TDesC8* aShortName, const ROmxScriptTest::CComponentInfo& aComponentInfo);
    
    void BeginWait();
    void EndWait();
		
private:
    TBool ComponentLogicalIL(const TDesC8& aName);
    TBool ComponentPhysicalIL(const TDesC8& aName);
	OMX_COMPONENTTYPE* ComponentByName(const TDesC8& aName);
	void GetComponentName(const OMX_COMPONENTTYPE* aHandle, TDes& aName);
	void FailWithOmxError(const TDesC8& aComponent, const TDesC& aOperation, OMX_ERRORTYPE aError);
	// static void CleanupOmxComponent(TAny*);	
	static void FormatHex(const TDesC8& bin, TDes& hex);
	void WriteInt32(TUint8* aPtr, TInt32 aData);
	void HashFileL(const TDesC& aFileName, TDes8& aHash, void (*aFilter1)(RBuf8&) = NULL, void (*aFilter2)(RBuf8&) = NULL, void (*aFilter3)(RBuf8&) = NULL );	
	TBool HandleNonTunneledBuffers(OMX_STATETYPE aNewState, OMX_COMPONENTTYPE* aComp);
	TBool HandlePortBufferHandler(OMX_STATETYPE aNewState, OMX_COMPONENTTYPE* aComp);

	void LogExtra(const TText8* aFile, TInt aLine, TOmxScriptSeverity aSeverity,
			TRefByValue<const TDesC16> aFmt,...);
	void RegisterTunnel(OMX_COMPONENTTYPE* aSourceComp, TInt aSourcePort, OMX_COMPONENTTYPE* aSinkComp, TInt aSinkPort);
	
	class TComponentInfo
		{
	public:
		HBufC8* iShortName;
		HBufC8* iComponentName;
		OMX_COMPONENTTYPE* iComponent;
		};
class TFilledBufferHeaderV2
	{
public:
	/** Amount of buffer filled with actual data */
	TUint iFilledLength; 
	/** Any timestamp associated with the buffer */
	TInt64 iTimeStamp;
	/** Combination of OMX_BUFFERFLAG_*, specified at page 68 in the OpenMAX spec. */
	TUint iFlags; 
	/** offset **/
	TUint32 iOffset;
	};
	class TTunnelInfo
		{
	public:
		OMX_COMPONENTTYPE* iSourceComponent;
		TInt iSourcePort;
		OMX_COMPONENTTYPE* iSinkComponent;
		TInt iSinkPort;
		};
	
	RArray<TTunnelInfo> iTunnels;
	typedef RArray<TTunnelInfo> RTunnelRelations;
	void FindTransitionOrderL(const RPointerArray<CComponentInfo>& aComponents,
								  const RTunnelRelations& aTunnelRelations,
								  RArray<TInt>& aOrder);

private:
	TBool iCoreLoaded;
	RPointerArray<CComponentInfo> iComponents;
	RASBreakEventHandler iEventHandler;
	OMX_STATETYPE iState;
	
	// Symbian graphic sink specialisation
	CWindowManager* iWindowManager;
	TVideoFitMode iVideoFitMode;
	OMX_INDEXTYPE iExtSurfaceConfigIndex;
	OMX_COMPONENTTYPE* iGraphicSink;
	
	CVideoTestTimer			*iTimer;
	
	RArray<TExpectedEvent> iExpectedEvents;
	TBool iWaitingForEvents;
	
	CNonTunneledHandler* iNonTunneledHandler;
	RArray<TExpectedEvent> iIgnoredEvents;

    // Buffer component handler
	CVideoBufferHandler* iBufferHandler;
	CVideoBufferSinkHandler* iBufferSinkHandler;
	
	TInt iReason;
	TOmxScriptTestVerdict iVerdict;
	MOmxScriptTestLogger& iLogger;
	
	CAsyncCallBack* iStopSchedulerCallback;
		
 	RMsgQueue<TFilledBufferHeaderV2> iMsgQueue;
	CPortBufferHandler* iPortBufferHandler;
	};

#endif /*OMXSCRIPTTEST_H_*/
