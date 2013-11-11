/*
* Copyright (c) 2001 Nokia Corporation and/or its subsidiary(-ies).
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


/*
** WARNING
**
** DO NOT EDIT - THIS CODE IS AUTOMATICALLY GENERATED
**               FROM A DATA FILE BY THE DICTIONARY CREATION PROGRAM
**
** This file generated on Thu Jun  7 15:00:08 2001
**                        (coordinated universal time)
**
** Command line: dict_creator syncml_1.0.dict syncml_1_0_dict.c syncml_1_0_dict.h
*/

/*
 * Dictionary = SyncML_1_0
 * Public id = 0x0FD1
 * Doc Type = -//SYNCML//DTD SyncML v1.0//EN
 */

#include <xml/cxml/nw_wbxml_dictionary.h>


static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Add[] = {'A','d','d','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Alert[] = {'A','l','e','r','t','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Archive[] = {'A','r','c','h','i','v','e','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Atomic[] = {'A','t','o','m','i','c','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Chal[] = {'C','h','a','l','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Cmd[] = {'C','m','d','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_CmdID[] = {'C','m','d','I','D','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_CmdRef[] = {'C','m','d','R','e','f','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Copy[] = {'C','o','p','y','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Cred[] = {'C','r','e','d','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Data[] = {'D','a','t','a','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Delete[] = {'D','e','l','e','t','e','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Exec[] = {'E','x','e','c','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Final[] = {'F','i','n','a','l','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Get[] = {'G','e','t','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Item[] = {'I','t','e','m','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Lang[] = {'L','a','n','g','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_LocName[] = {'L','o','c','N','a','m','e','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_LocURI[] = {'L','o','c','U','R','I','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Map[] = {'M','a','p','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_MapItem[] = {'M','a','p','I','t','e','m','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Meta[] = {'M','e','t','a','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_MsgID[] = {'M','s','g','I','D','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_MsgRef[] = {'M','s','g','R','e','f','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_NoResp[] = {'N','o','R','e','s','p','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_NoResults[] = {'N','o','R','e','s','u','l','t','s','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Put[] = {'P','u','t','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Replace[] = {'R','e','p','l','a','c','e','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_RespURI[] = {'R','e','s','p','U','R','I','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Results[] = {'R','e','s','u','l','t','s','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Search[] = {'S','e','a','r','c','h','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Sequence[] = {'S','e','q','u','e','n','c','e','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_SessionID[] = {'S','e','s','s','i','o','n','I','D','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_SftDel[] = {'S','f','t','D','e','l','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Source[] = {'S','o','u','r','c','e','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_SourceRef[] = {'S','o','u','r','c','e','R','e','f','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Status[] = {'S','t','a','t','u','s','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Sync[] = {'S','y','n','c','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_SyncBody[] = {'S','y','n','c','B','o','d','y','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_SyncHdr[] = {'S','y','n','c','H','d','r','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_SyncML[] = {'S','y','n','c','M','L','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Target[] = {'T','a','r','g','e','t','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_TargetRef[] = {'T','a','r','g','e','t','R','e','f','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Reserved[] = {'R','e','s','e','r','v','e','d','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_VerDTD[] = {'V','e','r','D','T','D','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_VerProto[] = {'V','e','r','P','r','o','t','o','\0'};

/*
** WARNING
**
** DO NOT EDIT - THIS CODE IS AUTOMATICALLY GENERATED
**               FROM A DATA FILE BY THE DICTIONARY CREATION PROGRAM
**
** This file generated on Thu Jun  7 15:00:08 2001
**                        (coordinated universal time)
**
** Command line: dict_creator syncml_1.0.dict syncml_1_0_dict.c syncml_1_0_dict.h
*/

/*
 * tag entries - sorted by token
 */
static const
NW_WBXML_DictEntry_t NW_SyncML_1_0_tag_token_0[46] = {
	{0x05, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Add},
	{0x06, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Alert},
	{0x07, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Archive},
	{0x08, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Atomic},
	{0x09, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Chal},
	{0x0a, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Cmd},
	{0x0b, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_CmdID},
	{0x0c, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_CmdRef},
	{0x0d, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Copy},
	{0x0e, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Cred},
	{0x0f, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Data},
	{0x10, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Delete},
	{0x11, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Exec},
	{0x12, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Final},
	{0x13, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Get},
	{0x14, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Item},
	{0x15, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Lang},
	{0x16, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_LocName},
	{0x17, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_LocURI},
	{0x18, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Map},
	{0x19, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_MapItem},
	{0x1a, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Meta},
	{0x1b, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_MsgID},
	{0x1c, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_MsgRef},
	{0x1d, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_NoResp},
	{0x1e, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_NoResults},
	{0x1f, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Put},
	{0x20, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Replace},
	{0x21, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_RespURI},
	{0x22, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Results},
	{0x23, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Search},
	{0x24, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Sequence},
	{0x25, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_SessionID},
	{0x26, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_SftDel},
	{0x27, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Source},
	{0x28, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_SourceRef},
	{0x29, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Status},
	{0x2a, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Sync},
	{0x2b, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_SyncBody},
	{0x2c, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_SyncHdr},
	{0x2d, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_SyncML},
	{0x2e, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Target},
	{0x2f, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_TargetRef},
	{0x30, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Reserved},
	{0x31, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_VerDTD},
	{0x32, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_VerProto}
};

/*
** WARNING
**
** DO NOT EDIT - THIS CODE IS AUTOMATICALLY GENERATED
**               FROM A DATA FILE BY THE DICTIONARY CREATION PROGRAM
**
** This file generated on Thu Jun  7 15:00:08 2001
**                        (coordinated universal time)
**
** Command line: dict_creator syncml_1.0.dict syncml_1_0_dict.c syncml_1_0_dict.h
*/

/*
 * tag entries - sorted by name
 */
static const
NW_Byte NW_SyncML_1_0_tag_name_0[46] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	43,
	28,
	29,
	30,
	31,
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	41,
	42,
	44,
	45,
};

static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Anchor[] = {'A','n','c','h','o','r','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_EMI[] = {'E','M','I','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Format[] = {'F','o','r','m','a','t','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_FreeID[] = {'F','r','e','e','I','D','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_FreeMem[] = {'F','r','e','e','M','e','m','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Last[] = {'L','a','s','t','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Mark[] = {'M','a','r','k','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_MaxMsgSize[] = {'M','a','x','M','s','g','S','i','z','e','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Mem[] = {'M','e','m','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_MetInf[] = {'M','e','t','I','n','f','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Next[] = {'N','e','x','t','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_NextNonce[] = {'N','e','x','t','N','o','n','c','e','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_SharedMem[] = {'S','h','a','r','e','d','M','e','m','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Size[] = {'S','i','z','e','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Type[] = {'T','y','p','e','\0'};
static const NW_Ucs2 NW_SyncML_1_0_ElementTag_Version[] = {'V','e','r','s','i','o','n','\0'};

/*
** WARNING
**
** DO NOT EDIT - THIS CODE IS AUTOMATICALLY GENERATED
**               FROM A DATA FILE BY THE DICTIONARY CREATION PROGRAM
**
** This file generated on Thu Jun  7 15:00:08 2001
**                        (coordinated universal time)
**
** Command line: dict_creator syncml_1.0.dict syncml_1_0_dict.c syncml_1_0_dict.h
*/

/*
 * tag entries - sorted by token
 */
static const
NW_WBXML_DictEntry_t NW_SyncML_1_0_tag_token_1[16] = {
	{0x05, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Anchor},
	{0x06, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_EMI},
	{0x07, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Format},
	{0x08, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_FreeID},
	{0x09, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_FreeMem},
	{0x0a, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Last},
	{0x0b, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Mark},
	{0x0c, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_MaxMsgSize},
	{0x0d, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Mem},
	{0x0e, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_MetInf},
	{0x0f, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Next},
	{0x10, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_NextNonce},
	{0x11, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_SharedMem},
	{0x12, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Size},
	{0x13, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Type},
	{0x14, (NW_String_UCS2Buff_t *) NW_SyncML_1_0_ElementTag_Version}
};

/*
** WARNING
**
** DO NOT EDIT - THIS CODE IS AUTOMATICALLY GENERATED
**               FROM A DATA FILE BY THE DICTIONARY CREATION PROGRAM
**
** This file generated on Thu Jun  7 15:00:08 2001
**                        (coordinated universal time)
**
** Command line: dict_creator syncml_1.0.dict syncml_1_0_dict.c syncml_1_0_dict.h
*/

/*
 * tag entries - sorted by name
 */
static const
NW_Byte NW_SyncML_1_0_tag_name_1[16] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
};

/*
 * Tag codepage table
 */
static const
NW_WBXML_Codepage_t NW_SyncML_1_0_tag_codepages[2] = {
	{46, (NW_WBXML_DictEntry_t*)&NW_SyncML_1_0_tag_token_0[0], (NW_Byte *)&NW_SyncML_1_0_tag_name_0[0]},
	{16, (NW_WBXML_DictEntry_t*)&NW_SyncML_1_0_tag_token_1[0], (NW_Byte *)&NW_SyncML_1_0_tag_name_1[0]},
};

/*
 * Attribute codepage table
 */

static const NW_Ucs2 NW_SyncML_1_0_docType[] = {'-','/','/','S','Y','N','C','M','L','/','/','D','T','D',' ','S','y','n','c','M','L',' ','v','1','.','0','/','/','E','N','\0'};

/*
 * Dictionary
 */
const NW_WBXML_Dictionary_t NW_SyncML_1_0_WBXMLDictionary = {
	0x0FD1,
	(NW_Ucs2 *)NW_SyncML_1_0_docType,
	2, (NW_WBXML_Codepage_t*)&NW_SyncML_1_0_tag_codepages[0],
	0, 0,
};

/*
** WARNING
**
** DO NOT EDIT - THIS CODE IS AUTOMATICALLY GENERATED
**               FROM A DATA FILE BY THE DICTIONARY CREATION PROGRAM
**
** This file generated on Thu Jun  7 15:00:08 2001
**                        (coordinated universal time)
**
** Command line: dict_creator syncml_1.0.dict syncml_1_0_dict.c syncml_1_0_dict.h
*/
