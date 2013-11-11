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
* Interface for handling, creating and modifying linked typefaces.
*
*/


#ifndef OPENFONTLINKEDTYPEFACEEXTENSION_H
#define OPENFONTLINKEDTYPEFACEEXTENSION_H

class COpenFontLinkedTypefaceSpecification;

/**
Interface for handling, creating and modifying linked typefaces.

The creation method sends an object containing the entire linked typeface specification.
In return, a pointer to an object conforming to COpenFontFile is returned.
This can subsequently be handed as though it were a normal COpenFontFile.

@publishedPartner
@prototype
*/
class MOpenFontLinkedTypefaceExtension
	{
public:
	/**
	Create a linked typeface from the specification. A path and filename is provided to the rasterizer.
	For example "c:\linkedfonts\MyLinkedTypeface" the rasterizer should append the file extension
	e.g. ".ltf" to aFileName.

	@param aLinkedTypefaceSpec The specification for the linked typeface
	@param aFileName The path and filename where the rasterizer will save the linked typeface
	@return	KErrNone if successful, otherwise one of the system wide error codes.
	*/
	virtual void CreateLinkedTypefaceL(const COpenFontLinkedTypefaceSpecification& aLinkedTypefaceSpec, TFileName& aFilename) = 0;

	/**
	Get the specification of a linked typeface from  the rasterizer.

	The primary purpose of this function is to allow client side code to retrieve the existing
	defintion of a linked typeface, prior to making changes to it and resubmitting the revised
	definition to the rasterizer using UpdatelinkedTypeface().

	@param	aOpenFontLinkedFont	A pointer to the existing COpenFontFile object for which the specification is required
	@param	aLinkedTypefaceSpec	On return will point to the specification of the existing linked typeface. Ownership
	 							of that specification object is transferred to the caller.
	@return	KErrNone if everything succeeded; otherwise an appropriate standard error code

	@see	MOpenFontLinkedTypefaceExtension::UpdatelinkedTypeface().
	*/
	virtual void GetLinkedTypefaceSpecificationL(const TFileName& aFilename, COpenFontLinkedTypefaceSpecification& aLinkedTypefaceSpec) = 0;

	/**
	To allow future extensions while maintaining BC.
	*/
	virtual void LinkedTypefaceRequestExtendedInterface(TUid aUid, TAny*& aParam) = 0;

private:
	//Reserved functions for future functionality.
	IMPORT_C virtual void Reserved1() = 0;
	IMPORT_C virtual void Reserved2() = 0; 
	IMPORT_C virtual void Reserved3() = 0; 
	IMPORT_C virtual void Reserved4() = 0; 
	IMPORT_C virtual void Reserved5() = 0; 
	IMPORT_C virtual void Reserved6() = 0;
	IMPORT_C virtual void Reserved7() = 0;
	IMPORT_C virtual void Reserved8() = 0;
	};

#endif /*OPENFONTLINKEDTYPEFACEEXTENSION_H*/
