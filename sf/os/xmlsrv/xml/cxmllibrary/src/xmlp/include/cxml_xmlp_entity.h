/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides a single header for the cXML Parser
*
*/



#ifndef _CXML_ENTITY_H_
#define _CXML_ENTITY_H_

#include <xml/cxml/cxml_proj.h>

#ifdef __cplusplus
extern "C" {
#endif

/* As per WBXML Format Specification (WAP-192-WBXML-20010725-a), 
 * section 5.8.4.3, the entities in WBXML can be represented either 
 * as String token (STR_I) or the ENTITY Token. The numeric character 
 * entities are reprented as ENTITY token and all other as STR_I.
 * 
 * In our implementation, all XML/HTML is represented as the numeric 
 * character entities. The numeric character entities is allocated four
 * bytes (NW_Uint32). The highest order byte will distinguish between the
 * different types of entities e.g. between predefined or character numeric 
 * entities.
 *
 */

  
// The maximum length of the entities can't exceed than 
// 9 bytes in current implementation. Provides safty
// for the infinite loop.
	
enum { CXML_ATTR_VAL_STR = 512,
       CXML_ENTITY_VAL_LEN =4
};

typedef struct CXML_EntitySet_Entry_s {
   CXML_Uint8* name;
   CXML_Ucs2 value;
}CXML_EntitySet_Entry_t;




/** ----------------------------------------------------------------------- **
    @function:    CXML_XML_Handle_entity
    @synopsis:    Resolve the entities.
    @scope:       public      

    @parameters: 
        [in] NW_XML_Reader_t* pT
                  

        [in] NW_XML_Reader_Interval_t* pI_entityData
                  Entity name 


        [out] NW_Uint32* entityVal
                       If it is Character hex entry, Character decimal entry or
                       Prefined entry then it contains the hex value of entity.
                       In case of Internal entities it is zero.
                        
		[out] CXML_Uint8** entityStr
                       In case of Internal Entities, it is string corresponding 
                       to the entity name defined in 
                       NW_XML_Reader_Interval_t* pI_entityData. 

        [out] NW_Bool* entityFound 
                       NW_TRUE: If valid type of entity is defined.
        [in] RPointerArray <CXML_Internal_Entity_t>  internalEntityList 
                       List of Internal Entities for search. This is
                       casted to (void*) to avoid c++ related error in
                       <e32std.h>

    @description:  * The following type of entities are handled
                 * 1) Character hex entry
                 * 2) Character decimal entry
                 * 3) Prefined entry
                 * 4) Internal Entities defined in the DTD.
                 *
                 * 
                 


    @returns:   
                  In case of file  related failures:  Error codes is returned defined in 
				                                      nwx_status.h
                  NW_STAT_FAILURE : For any general error and entity is not predefined 
                                    entity.
                  NW_STAT_SUCCESS: For success
 -----------------------------------------------------------------------------------**/

NW_Status_t CXML_XML_Handle_entity(NW_XML_Reader_t* pT,
								   NW_XML_Reader_Interval_t* pI_entityData,
								   NW_Uint32* entityVal,
                                   CXML_Uint8** entityStr,
								   NW_Bool* entityFound,
                                   void* internalEntityList);




NW_Status_t CXML_XML_Parser_Entity(NW_XML_Reader_t* pT,
							    NW_XML_Reader_Interval_t* I_entityData,
							    NW_Bool* entityFound);

/** ----------------------------------------------------------------------- **
    @function:    CXML_XML_Get_Entity
    @synopsis:    Converts the entity value from CXML_Uint32 to CXML_Byte string.
    @scope:       public      

    @parameters: 
        [in] entityVal
                  

        [out] CXML_Byte* entityStr
                  Entity value in the string.


        [out] CXML_Uint32* strLen
                       Length of the entity string. 
					   0: In case of error.

    @description: Converts the entity value from CXML_Uint32 to CXML_Byte string.

    @returns:   
                  In case of file  related failures:  Error codes is returned defined in 
				                                      nwx_status.h
                  NW_STAT_FAILURE : For any general error and entity is not predefined 
                                    entity.
                  NW_STAT_SUCCESS: For success
 -----------------------------------------------------------------------------------**/

IMPORT_C
NW_Status_t CXML_XML_Get_Entity(CXML_Uint32 entityVal,
								CXML_Byte* entityStr,
								CXML_Uint32* strLen);

#ifdef __cplusplus 
} // extern "C" {
#endif /* __cplusplus */

#endif //_CXML_ENTITY_H_
