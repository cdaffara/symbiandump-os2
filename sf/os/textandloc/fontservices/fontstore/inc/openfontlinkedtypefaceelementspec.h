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
* Specification of a single typeface element within a linked typeface specification.
*
*/


#ifndef OPENFONTLINKEDTYPEFACEELEMENTSPEC_H
#define OPENFONTLINKEDTYPEFACEELEMENTSPEC_H

/**
Class for specifying a single typeface element within a linked typeface specification.
Its features determine how the typeface is rendered within the linked fonts that use it.

This class is used to communicate between the server and the rasterizer.

Another similar but separate class, CLinkedTypefaceElementSpec, will be used client side
and to communicate between the server and the rasterizer.

WARNING: Class for internal and partner use ONLY.  Compatibility is not guaranteed in future releases.

@see CLinkedTypefaceElementSpec

@publishedAll
@released
*/
NONSHARABLE_CLASS(COpenFontLinkedTypefaceElementSpec) : public CBase
	{
public:
	/**
	Create a new linked typeface element specification. The name specified must be an existing typeface
	that should be added to the linked typeface. The group ID links this typeface to a linked typeface
	group.
	
	@see CLinkedTypefaceSpecification::Group()
	
	@param aTypefaceName The typeface name for this element
	@param aGroupId The ID of the linked font group this element should belong to
	
	@return The newly created element spec
	*/
	IMPORT_C static COpenFontLinkedTypefaceElementSpec* NewLC(const TDesC& aTypefaceName, TInt aGroupId);

	/**
	Returns the typeface that this element represents within the linked typeface.
	
	@return	The element typeface name
	*/
	IMPORT_C TPtrC ElementName() const;
	
	/**
	Set whether the element is the canonical (metrics) font. 
	This is the font that provides all the metrics for the linked typeface.
	
	@param aIsCanonical Whether this element is canonical or not
	*/
	IMPORT_C void SetCanonical(TBool aIsCanonical);
	/**
	Returns the element canonical setting.
	@return	The element canonical setting
	*/
	IMPORT_C TBool Canonical() const;
	
	/**
	Returns the group Id specified at construction.
	@return The group Id specified at construction
	 */
	IMPORT_C TInt GroupId() const;
	
	/**
	Returns the path and filename of file containing the font this element represents.
	@return The full path and filename for the font this element represents.
	*/
	IMPORT_C TPtrC FileName() const;
	
	/**
	Default destructor
	*/
	IMPORT_C ~COpenFontLinkedTypefaceElementSpec();

	/**
	Sets the path and filename of file containing the font this element represents.

	WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.
	
	@param aFileName Path and filename of this element
	*/
	IMPORT_C void SetFileNameL(const TDesC& aFileName);
private:
	/**
	Constructor
	*/
	COpenFontLinkedTypefaceElementSpec(TInt aGroupId);
	void ConstructL(const TDesC& aTypefaceName);
private:
	HBufC*	iElementName;	//The Typeface Name
	HBufC*  iFileName;		//The file containing this typeface
	TBool	iIsCanonical;
	TInt	iGroupId;
	};

#endif /*OPENFONTLINKEDTYPEFACEELEMENTSPEC_H*/
