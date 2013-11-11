/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @internalComponent
*/
enum TWordPanic
	{
	//
	// File related errors
	EFileOpenIntegrityError,
	EDefaultTemplateNotPresent,
	EWordSaveReadOnlyFile,
	ENewFileNullStore,
	//
	//
	EInvalidCommandParameter,
	//
	// Zoom setting dialog
	EWordZeroZoomFactor,
	//
	// Style gallery panics
	EWordGalleryInvalidStyle,
	EStyleIntegrityError,
	//
	// Spell related panics
	EWordIllegalDocLength,
	//
	// Object format dialog
	EWordIllegalCrop,
	//
	// New Template panics
	ETemplateNotPresent,
	EWordNewFileRequired,
	//picture factory panic
	EPictureTypeNotSupported
	};

/**
@internalComponent
*/
GLREF_C void Panic(TWordPanic aPanic);
