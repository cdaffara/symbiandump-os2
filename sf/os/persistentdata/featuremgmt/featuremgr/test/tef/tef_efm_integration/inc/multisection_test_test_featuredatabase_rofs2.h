// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @internalComponent
 @released
*/
namespace NFeature {

	/**
	Feature = TestCompROMUid1  
	This feature is defined as "unsupported" in the first ROFS section and as "supported" in both second and third ROFS sections.
	*/
	const TUid KTestCompROMUid1 = {0x1};

	/**
	Feature = TestCompROMUid10,  
	This feature is defined "supported" in the xml file both second and third ROFS sections
	The feature data is set to "0" in the IBY file for the second ROFS section
	The feature data is set to "1" in the IBY file for the third ROFS section
	*/
	const TUid KTestCompROMUid10 = {0x10};

	/**
	Feature = TestCompROMUid2
	This feature is defined as "supported" in the first and second ROFS sections and as "unsupported" in the third ROFS section
	*/
	const TUid KTestCompROMUid2 = {0x2};

	/**
	Feature = TestCompROMUid3  
	This feature is defined as "supported" in the xml files for the second and third ROFS sections,
	and it is marked as "included" in the IBY files for the second and third ROFS sections
	*/
	const TUid KTestCompROMUid3 = {0x3};

	/**
	Feature = TestCompROMUid4  
	This feature is defined as "unsupported" in the xml file for the second ROFS section, and
	as "supported" in the xml file for the third ROFS section
	*/
	const TUid KTestCompROMUid4 = {0x4};

	/**
	Feature = TestCompROMUid5,  
	This feature is defined as "supported" in the xml file for the second ROFS section, and
	as "unsupported" in the xml file for the third ROFS section
	*/
	const TUid KTestCompROMUid5 = {0x5};

	/**
	Feature = TestCompROMUid6, 
	This feature is defined as "supported and blacklisted" in the xml file for the second ROFS section, and
	as "unsupported" in the xml file for the third ROFS section
	*/
	const TUid KTestCompROMUid6 = {0x6};

	/**
	Feature = TestCompROMUid7,  
	This feature is defined "unsupported and  blacklisted" in the xml file for the second ROFS section, and
	as "supported" in the xml file for the third ROFS section
	*/
	const TUid KTestCompROMUid7 = {0x7};

	/**
	Feature = TestCompROMUid8,  
	This feature is defined as "unsupported" in the xml file for the second ROFS section, and
	then it is set to "blacklisted" in the IBY file forthe second ROFS section. Then this feature is set
	to"supported" in the xml file for the third ROFS section
	*/
	const TUid KTestCompROMUid8 = {0x8};

	/**
	Feature = TestCompROMUid9,  
	This feature is defined as "supported" in the xml file for both the second and third ROFS sections
	The feature data is set to "0" in the xml file for the second ROFS section
	The feature data is set to "1" in the xml file for the third ROFS section
	*/
	const TUid KTestCompROMUid9 = {0x9};

}
