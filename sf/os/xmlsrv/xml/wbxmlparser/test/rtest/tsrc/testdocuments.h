// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTDOCUMENTS_H__
#define __TESTDOCUMENTS_H__

#include <e32test.h>


// Encoding XML documents
_LIT(KXmlEngUtf,	"c:\\system\\XmlTest\\Encodings\\doc_eng_utf.xml");
_LIT(KXmlChBig5,	"c:\\system\\XmlTest\\Encodings\\doc_ch_b5.xml");
_LIT(KXmlChUtf,		"c:\\system\\XmlTest\\Encodings\\doc_ch_utf.xml");
_LIT(KXmlFrHtml,	"c:\\system\\XmlTest\\Encodings\\doc_fr_html.xml");
_LIT(KXmlFrLatin1,	"c:\\system\\XmlTest\\Encodings\\doc_fr_latin1.xml");
_LIT(KXmlFrUtf,		"c:\\system\\XmlTest\\Encodings\\doc_fr_utf.xml");
_LIT(KXmlJpEuc,		"c:\\system\\XmlTest\\Encodings\\doc_jp_euc.xml");
_LIT(KXmlJpSjis,	"c:\\system\\XmlTest\\Encodings\\doc_jp_sjis.xml");
_LIT(KXmlJpUtf,		"c:\\system\\XmlTest\\Encodings\\doc_jp_utf.xml");


// SyncML XML/WBXML documents directory.
// This contains a list of dirctories that contains the actual documents for test.
// NOTE there are NO trailing back slash.
_LIT(KSyncML1_1Directory,		"c:\\system\\XmlTest\\SyncML\\1.1");
_LIT(KSyncML1_2Directory,		"c:\\system\\XmlTest\\SyncML\\1.2");
_LIT(KWml1_1Directory,			"c:\\system\\XmlTest\\Wml\\1.1");
_LIT(KSi1_0Directory,			"c:\\system\\XmlTest\\ServiceIndication\\1.0");
_LIT(KOpaqueDirectory, 			"c:\\system\\XmlTest\\Opaque");

_LIT8(KWbxmlParserDataType, "text/wbxml");

_LIT8(KSyncmlStringDictionaryUri, "-//SYNCML//DTD SyncML 1.1//EN~0");
_LIT8(KBlankStringDictionaryUri, "");

_LIT(KDirSeperator, "\\");
_LIT(KUnderscore, "_");

_LIT(KXmlExt,	".xml");
_LIT(KWbxmlExt,	".wbxml");
_LIT(KWmlcExt,	".wmlc");
_LIT(KSiExt, 	".bdy");

// The size of chunks to parse. Allows for boundary testing.
const TInt KMinChunkSize = 1;
const TInt KMidChunkSize = 5;
const TInt KMaxChunkSize = 300;


#endif // __TESTDOCUMENTS_H__
