/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
 @file
 @internalAll
 @released
*/

#ifndef __REFTESTAGENTCONSTANTS_H__
#define __REFTESTAGENTCONSTANTS_H__

_LIT8(KRtaMimeContent, "application/x-rta.drm.content");
_LIT8(KRtaMimeContentRights, "application/x-rta.drm.contentrights");
_LIT8(KRtaMimeRights, "application/x-rta.drm.rights");
_LIT8(KRtaMimePostAcquisitionRights, "application/x-rta.drm.postacqrights");
_LIT8(KRtaMimePostAcquisitionContent, "application/x-rta.drm.postacqcontent");

_LIT(KRtaExtensionContent, ".content");
_LIT(KRtaExtensionContentRights, ".contentrights");
_LIT(KRtaExtensionRights, ".rights");
_LIT(KRtaExtensionPostAcquisitionRights, ".dr");

_LIT(KRtaDefaultOutputDirectory, "c:\\");
_LIT(KRtaDefaultOutputFile, "output");

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
      
// Asf header object GUID.
_LIT8(KRtaAsfHeaderObject, "75B22630668E11CFA6D900AA0062CE6C");
_LIT8(KRtaWmaContentType, "audio/x-ms-wma");
_LIT(KRtaWmdrmFileExtension, ".asf");
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
      

// combine KRtaDefaultOutputDirectory, KRtaDefaultOutputFile and
// KRtaExtensionContent
_LIT(KRtaDefaultOutputPath, "c:\\output.content");

_LIT8(KContentXml,"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n\
<DRMFiles>\n\r\
\t<content cid=\"PostAcquisitionTest\">\r\n\
\t\t<filename>\r\n\
\t\tmaketheparserhappy\r\n\
\t\t</filename>\r\n\
\t\t<name>\r\n\
\t\tmaketheparserhappy\r\n\
\t\t</name>\r\n\
\t\t<uniqueid>\r\n\
\t\tmaketheparserhappy\r\n\
\t\t</uniqueid>\r\n\
\t\t<stringattribute name=\"mimetype\">\r\n\
\t\ttext/plain\r\n\
\t\t</stringattribute>\r\n\
\t</content>\r\n\
</DRMFiles>\r\n\r\n\r\n");
_LIT8(KMimeTypeRecordedContent, "text/plain");
#endif
