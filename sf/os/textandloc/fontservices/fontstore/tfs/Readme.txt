//
// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

Manual Test
-----------
LargeDummyFontFile.gdr is an auto-generated dummy font file for manually testing the OS with a font containing a large number of glyphs ( > 20000 CHARS).

Test steps:
1) Place LargeDummyFontFile.gdr in z:\system\fonts and start the emulator.
2) Start the Word application in emulator and change font to 'LargeDummy'.
3) Enter a character towards the end of the font range by:
	press ctrl key + enter char code in decimal (eg 20500 ) + release ctrl key.  
4) If emulator does not crash the test is successful.  

 