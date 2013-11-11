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



#ifndef _CXML_XMLP_INT_ENTITY_H_
#define _CXML_XMLP_INT_ENTITY_H_
#include <e32std.h>
#include <xml/cxml/cxml_proj.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct CXML_Internal_Entity_s {
   NW_Ucs2 * name;
   NW_Ucs2 * value;
   NW_Uint32 nameLen;
}CXML_Internal_Entity_t;


/** ----------------------------------------------------------------------- **
    @function:    CXML_XML_Parser_Store_I_Entity
    @synopsis:    Function to store the internal entity.
    @scope:       public      

    @parameters: 
        [in] entityVal
                  

        [in] NW_XML_Reader_Interval_t* I_entityName
                  Internal Entity Name


        [in] NW_XML_Reader_Interval_t* I_entityValue
                  Internal Entity Value

    @description: This extracts the Internal Entity name from the XML buffer
                  and stores in the Link list.

    @returns:   
                  In case of file  related failures:  Error codes is returned defined in 
				                                      nwx_status.h
                  NW_STAT_FAILURE : For any general error and entity is not predefined 
                                    entity.
                  NW_STAT_SUCCESS: For success
 -----------------------------------------------------------------------------------**/

 NW_Status_t CXML_XML_Parser_Store_I_Entity(NW_XML_Reader_t* pT, 
                                            NW_XML_Reader_Interval_t* I_entityName,
                                            NW_XML_Reader_Interval_t* I_entityValue,
                                            RPointerArray <CXML_Internal_Entity_t>*  internalEntityList);

 /** ----------------------------------------------------------------------- **
    @function:    CXML_XML_Parser_Free_I_Entity_List
    @synopsis:    Function to free the internal entity list.
    @scope:       public      

    @parameters: void
        

    @description: Function to free the internal entity list.

    @returns:   
                  void
 -----------------------------------------------------------------------------------**/

 void CXML_XML_Parser_Free_I_Entity_List(RPointerArray <CXML_Internal_Entity_t>  internalEntityList);

 /** ----------------------------------------------------------------------- **
    @function:    CXML_XML_Resolve_Internal_Entity
    @synopsis:    Function to resolve the internal entity.
    @scope:       public      

    @parameters: 
        [in] NW_Ucs2* entityName: Name of Entity
                  

        [out] CXML_Uint8** entityValStr
                           If the entity name is the internal entity then this 
                           contains the entiy value string otherwise NULL.
                  


        [out] NW_Bool* entityFound
                  If it is internal entity then it is true otherwise false.
        [in]  NW_Uint32 encoding:
                   Original encoding of the buffer so that entityValStr contain 
                   correct.
        [in]  void* internalEntityList: Internal Entity list

    @description: This extracts the Internal Entity name from the XML buffer
                  and stores in the Link list.

    @returns:   
                  In case of file  related failures:  Error codes is returned defined in 
				                                      nwx_status.h
                  NW_STAT_FAILURE : For any general error and entity is not predefined 
                                    entity.
                  NW_STAT_SUCCESS: For success
 -----------------------------------------------------------------------------------**/

 NW_Status_t CXML_XML_Resolve_Internal_Entity(NW_Ucs2* entityName,
                                              CXML_Uint8** entityValStr,
                                              NW_Bool* entityFound,
                                              NW_Uint32 encoding,
                                              void* internalEntityList);
                                            



#ifdef __cplusplus 
} // extern "C" {
#endif /* __cplusplus */

#endif //_CXML_XMLP_INT_ENTITY_H_
