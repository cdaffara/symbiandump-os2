/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* The Open Font rasterizer plug-in Interface Definition class.
*
*/


#ifndef OPENFONTRASTERIZER_H
#define OPENFONTRASTERIZER_H

#include <ecom/ecom.h>

class COpenFontFile;

/**
Supplied to COpenFontRasterizer::ExtendedInterface() to access the extended
API interface MOpenFontLinkedTypefaceExtension.

@see	COpenFontRasterizer::ExtendedInterface()
@see	MOpenFontLinkedTypefaceExtension
@publishedPartner
@prototype
*/
const TUid KUidLinkedTypefaceRasterizerExtension = {0x10285EAA};

/**
The Open Font rasterizer plug-in Interface Definition class. 

This interface allows the implementation of an Open Font rasterizer plug-in,
which can then be dynamically loaded at run time by the ECOM plug-in framework. 
The plug-in implementation is instantiated by calling NewL(), passing the implementation 
UID so that ECOM can instantiate the correct implementation. 

The rasterizer interface declares a single pure virtual functions, which the 
plug-in derived from this interface must implement. The function reads font files, and creates 
a COpenFontFile object if the font file is of the right type.

Writing derived classes:

Open font rasterizers should derive from this class and implement the 
NewFontFileL() function.

Derived classes should also define the factory function. It is a static function 
which takes no arguments. It creates a COpenFontRasterizer derived object on the heap, and returns it 
to the caller. This factory function, together with the implementation UID will form 
the TImplementationProxy used by the ECOM framework to instatiate the plug-in. 

The rasterizer class may also need to store an 'engine context'. This is an 
object derived from COpenFontRasterizerContext, which provides functions that 
make it easier to write the glyph bitmap during rasterization.


@publishedPartner
@released
*/
class COpenFontRasterizer: public CBase
	{
public:
	/** Creates a COpenFontFile derived object for loading font files in the 
	new format. 

	This function is called by the framework during font and bitmap server 
	startup. It creates a font file object for font files in the \\resource\\fonts 
	directory on all drives, using the default search path. The first font of 
	a given name overrides subsequent ones. The directory search path is soft 
	to hard: Y:, X:, W:, ..., C:, B:, A:, Z:. Files may also be added 
	dynamically after startup using CWsScreenDevice::AddFile(), which 
	indirectly calls this function.

	Implementations of this function should examine the file aFileName, and if 
	it is of the correct type attempt to create a COpenFontFile derived object 
	to load it. The caller is responsible for deleting the object. The function 
	must return NULL if it cannot recognise the file, and it may also leave if 
	there is an error. 

	@param aUid An ID to be used for the file. UIDs are required by the font 
	framework, so the font store allocates them for non Symbian OS-native font 
	files. This ID persists until the font is unloaded, e.g. if the device is 
	rebooted.
	@param	aFileName		The full path and filename of the file from which the COpenFontFile
							object is created, if the file is the correct type.
	@param	aFileSession	The file session owned by the Font and Bitmap server. This file session 
							should be used for any file access. If COpenFontFile objects need to keep 
							files open, aFileSession should be stored in a place that is accessible to them. 
	@return	A pointer to a new COpenFontFile derived object, or NULL if the file type is not recognised.
	@see	CWsScreenDevice::AddFile() */
	virtual COpenFontFile* NewFontFileL(TInt aUid,const TDesC& aFileName,RFs& aFileSession) = 0;
	inline static COpenFontRasterizer* NewL(TUid aInterfaceImplUid);
	inline virtual ~COpenFontRasterizer();
	IMPORT_C virtual void ExtendedInterface(TUid aUid, TAny*& aParam);
private:
	TUid iDtor_ID_Key;//ECOM identifier used during destruction
	};

/** Uses ECOM plug-in framework to instantiate the Open Font rasterizer interface 
implementation given its implementation UID. 

@param	aInterfaceImplUid	The UID of the interface implementation required

@return	COpenFontRasterizer*	A pointer to a COpenFontRasterizer object. */
inline COpenFontRasterizer* COpenFontRasterizer::NewL(TUid aInterfaceImplUid)
{
	return reinterpret_cast <COpenFontRasterizer*> (
		REComSession::CreateImplementationL(
			aInterfaceImplUid,
			_FOFF(COpenFontRasterizer, iDtor_ID_Key))); 
}

/** Default destructor */
inline COpenFontRasterizer::~COpenFontRasterizer()
{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
}

#endif /*OPENFONTRASTERIZER_H*/
