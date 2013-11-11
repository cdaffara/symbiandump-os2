/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*/
// HTI service functions
//gsoap ns1 service name: HtiAudio
//gsoap ns1 service namespace: urn:hti
//gsoap ns1 service style: rpc
//gsoap ns1 service encoding: literal
//gsoap ns1 service location: http://localhost:2000

typedef char*			xsd__string;
typedef wchar_t*		xsd__string_;
typedef int				xsd__int;
typedef unsigned char	xsd__unsignedByte;
typedef unsigned short	xsd__unsignedShort;


struct ArrayOfAudioFiles
{
    xsd__string	*__ptr;
    xsd__int	__size;
};
//gsoap ns1 service method-action: listAudioFiles "HtiAudio"
int ns1__listAudioFiles(
    xsd__string					directory,
    struct ArrayOfAudioFiles	*audiofiles
);

//enum ns__audioSettings {Default, GeneralMusic, RingTonePreview};

// synchronous service
//gsoap ns1 service method-action: playAudioFile "HtiAudio"
int ns1__playAudioFile(
    xsd__string				fileName,
    xsd__unsignedByte		volume,
    xsd__int				startPosition,
    xsd__int				stopPosition,
    xsd__unsignedByte		nrOfRepeats,
    xsd__int				silenceBetweenRepeats,
	xsd__string				audioSettings,
	xsd__int				timeout, // we cannot calculate timeout from file/parameters in all cases
    struct ns1__playAudioFileResponse{} *out // empty response
);
// asynchronous service
//gsoap ns1 service method-action: startPlayAudioFile "HtiAudio"
int ns1__startPlayAudioFile(
    xsd__string				fileName,
    xsd__unsignedByte		volume,
    xsd__int				startPosition,
    xsd__int				stopPosition,
    xsd__unsignedByte		nrOfRepeats,
    xsd__int				silenceBetweenRepeats,
	xsd__string				audioSettings,
    struct ns1__startPlayAudioFileResponse{} *out // empty response
);

// synchronous service
//gsoap ns1 service method-action: playTone "HtiAudio"
int ns1__playTone(
    xsd__unsignedShort		frequency,
    xsd__int				duration,
    xsd__unsignedByte		volume,
    xsd__unsignedByte		nrOfRepeats,
    xsd__int				silenceBetweenRepeats,
    struct ns1__playToneResponse{} *out // empty response
);
// asynchronous service
//gsoap ns1 service method-action: startPlayTone "HtiAudio"
int ns1__startPlayTone(
    xsd__unsignedShort		frequency,
    xsd__int				duration,
    xsd__unsignedByte		volume,
    xsd__unsignedByte		nrOfRepeats,
    xsd__int				silenceBetweenRepeats,
    struct ns1__startPlayToneResponse{} *out // empty response
);

// synchronous service
//gsoap ns1 service method-action: playDtmf "HtiAudio"
int ns1__playDtmf(
    xsd__string				dtmfString,
    xsd__int				toneLength,
    xsd__int				gapLength,
    xsd__unsignedByte		volume,
    xsd__unsignedByte		nrOfRepeats,
    xsd__int				silenceBetweenRepeats,
    struct ns1__playDtmfResponse{} *out // empty response
);
// asynchronous service
//gsoap ns1 service method-action: startPlayDtmf "HtiAudio"
int ns1__startPlayDtmf(
    xsd__string				dtmfString,
    xsd__int				toneLength,
    xsd__int				gapLength,
    xsd__unsignedByte		volume,
    xsd__unsignedByte		nrOfRepeats,
    xsd__int				silenceBetweenRepeats,
    struct ns1__startPlayDtmfResponse{} *out // empty response
);
//gsoap ns1 service method-action: getPlayStatus "HtiAudio"
int ns1__getPlayStatus(
    xsd__string		type,
	xsd__string		&status
);
//gsoap ns1 service method-action: stopPlayback "HtiAudio"
int ns1__stopPlayback(
    void *_, // no parameters
    struct ns1__stopPlaybackResponse{} *out // empty response
);
//gsoap ns1 service method-action: getDuration "HtiAudio"
int ns1__getDuration(
    xsd__string			fileName,
    xsd__int			&duration
);
//gsoap ns1 service method-action: getMaxVolume "HtiAudio"
int ns1__getMaxVolume(
    xsd__string			fileName,
    xsd__unsignedByte	&volume
);
//gsoap ns1 service method-action: setVolume "HtiAudio"
int ns1__setVolume(
    xsd__unsignedByte	volume,
    xsd__unsignedByte	&volumeSet
);

