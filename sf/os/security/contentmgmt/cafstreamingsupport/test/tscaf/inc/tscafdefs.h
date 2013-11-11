// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Defines the common constant definitions for the Streaming CAF test harness
// 
//

/**
 @file 
 @internalComponent
 @test
*/

#ifndef TSCAFDEFS_H_
#define TSCAFDEFS_H_

//Names of the test RO files
_LIT(KServiceProtectedRo,"roserviceprotected.dr");
_LIT(KProgramProtectedRo,"roprogramprotected.dr");
_LIT(KSimpleRoFileName,"rosimple.dr");
_LIT(KExpiredRoFileName,"roexpired.dr");
_LIT(KRecordableRoFileName,"rorecordable.dr");
_LIT(KPostAcquisitionRoFileName,"postacquisitiontest.dr");

/** The private folder of the Streaming Test Agent. */
_LIT(KStaPrivateFolder,"!:\\private\\1028588b\\");

/** The private folder of the Test Server. */
_LIT(KTestServerPrivateFolder,"!:\\private\\1028588c\\");

/** The private folder of the Reference Test Agent. */
_LIT(KRtaPrivateFolder,"!:\\private\\10205ea5\\");

/** The path where data files live. */
_LIT(KDataFilesPath,"z:\\tcaf\\tscaf\\data\\");

/** The test folder of the Reference Test Agent. */
_LIT(KRtaTestFolder,"!:\\rta\\");

/** Maximum length of the Rights Object's content. */
const TInt KMaxRightsObjectLength = 255;

/** Maximum length of a Mime type. */
const TInt KMaxMimeTypeLength = 50;

// ******* INI file configuration constants  *******

// Out of Memory Test attributes
_LIT(KScafConfigOOMTest,"OOMTest");

// Rights object filename attribute
_LIT(KScafConfigRoName,"RoName_%d");

// Key Stream Decoder Attribute Identifiers
_LIT(KScafConfigIsServiceProtected,"IsServiceProtected%d");
_LIT(KScafConfigIsProgramProtected,"IsProgramProtected%d");
_LIT(KScafConfigCanExport,"CanExport%d");
_LIT(KScafConfigMustProtectIfRecording,"MustProtectIfRecording%d");
_LIT(KScafConfigCanPlay,"CanPlay%d");
_LIT(KScafConfigContentId,"ContentId%d");
_LIT(KScafConfigRightsIssuerUri,"RightsIssuerUri%d");

// Single Process Stream Agent Specific Attributes
_LIT(KScafConfigSingleProcessAgent,"SingleProcessAgent%d");
_LIT(KScafConfigPrivateFolderPath,"PrivateFolderPath%d");

// SDP Construction Attributes
_LIT(KSdpMedia,"Media%d");
_LIT(KSdpPort,"Port%d");
_LIT(KSdpProtocol,"Protocol%d");
_LIT(KSdpFormat,"Format%d");
_LIT(KSdpConnAddr,"ConnectionAddress%d");
_LIT(KSdpAttributeNum,"AttributeNum%d");
_LIT(KSdpAttributeName,"AttributeName%d%d");
_LIT(KSdpAttributeValue,"AttributeValue%d%d");
_LIT(KSdpAttributeType,"AttributeType%d%d");

// Constants for Media types
_LIT(KSdpMediaAudio,"Audio");
_LIT(KSdpMediaVideo,"Video");
_LIT(KSdpMediaData,"Data");

//Constants for Protocol types
_LIT(KSdpProtocolUdp,"Udp");
_LIT(KSdpProtocolTcp,"Tcp");
_LIT(KSdpProtocolRtp,"Rtp");

//Whether the output file is created by the test harness
//ETrue=Test Harness creates output files
//EFalse=The agent creates output files
_LIT(KCreateOutputFiles, "CreateOutputFiles");

// ********************************************

#endif /*TSCAFDEFS_H_*/
