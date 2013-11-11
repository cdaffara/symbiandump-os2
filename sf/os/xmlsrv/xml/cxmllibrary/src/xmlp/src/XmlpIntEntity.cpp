/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


/* Code to handle Internal Entities */

#include "cxml_internal.h"
#include <xml/cxml/nw_xmlp_xmlreader.h>
#include <xml/cxml/nw_xmlp_xmlparser.h>
#include <xml/cxml/nw_string_string.h>

#include "cxml_xmlp_int_entity.h"

//#include <flogger.h> // Comment out this.





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

        [in]  RPointerArray <CXML_Internal_Entity_t>*  internalEntityList: Internal Entity list

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
                                            RPointerArray <CXML_Internal_Entity_t>*  internalEntityList)
 {
  NW_Uint32 length;
  NW_Uint32 nameLength;
  NW_Uint32 totalByteCount = 0;
  NW_Uint32 i =0;
  NW_Uint8* bufPointer;
  NW_Uint8* pName;
  NW_Ucs2* ucsName;
  NW_Uint32 valueByteLength;
  NW_Uint8* pValue;
  NW_Ucs2* ucsValue;
  CXML_Internal_Entity_t* internalEntity=NULL;
  NW_Ucs2 c;
  NW_Uint32 numbytes;
  TInt errorCode;
  TInt entryCount;
  NW_Status_t s;

  /* Check Entity Name Interval and Value Interval is valid */

  if ( (!NW_XML_Reader_Interval_IsWellFormed(I_entityName) ) &&
       (!NW_XML_Reader_Interval_IsWellFormed(I_entityValue) ) )
  {
   return NW_STAT_FAILURE;
  }


     /* Entity name */

    length = I_entityName->stop - I_entityName->start;
    nameLength = length; /* byte length */
    s = NW_XML_Reader_DataAddressFromBuffer(pT, I_entityName->start,
                                         &nameLength, &bufPointer);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    if (nameLength != length) {
        return NW_STAT_FAILURE;
    }
    nameLength = I_entityName->charStop - I_entityName->charStart; /* char count */

    /* pName is not NULL terminated so need to use following method. The num byte
     * will be used for both pName and pVlaue.
     */


    numbytes = NW_String_readChar( (NW_Byte*) bufPointer,&c,pT->encoding);

    /* Calculate the length of string. Also add the number of characters 
     * required for the NULL termination.
     */
        
    totalByteCount = length + numbytes;


    pName = (NW_Uint8*) NW_Mem_Malloc(totalByteCount);

    if (pName  != NULL) 
     {
      (void)NW_Mem_memcpy(pName , bufPointer, length );


       for(i=0; i < numbytes; i++)
       {
        pName[length+i] = '\0';
       }
      } /*end if (pName  != NULL)*/
     else
     {
      return NW_STAT_OUT_OF_MEMORY;
     }

   

    /* The "pName" is already NULL terminated */

    if(pT->encoding != HTTP_iso_10646_ucs_2)
    {
     s = NW_String_byteToUCS2Char(pName, length, &ucsName);

     if(s != NW_STAT_SUCCESS)
     {
      return s;
     }

     totalByteCount = 2*totalByteCount;
    }/*end if(pT->encoding != HTTP_iso_10646_ucs_2)*/
    else
    {
     ucsName = (NW_Ucs2*) NW_Mem_Malloc(totalByteCount); 
     (void)NW_Mem_memcpy(ucsName, pName, totalByteCount);

    }

    /*Now free the pName */

    if(pName != NULL)
    {
     NW_Mem_Free(pName);
    }
   
    internalEntity = (CXML_Internal_Entity_t*) NW_Mem_Malloc(sizeof(CXML_Internal_Entity_t) );

    if(internalEntity == NULL)
    {
        if(pName != NULL)
        {
         NW_Mem_Free(pName);
        }
     return NW_STAT_OUT_OF_MEMORY;
    }
 
      

            //SSS: Comment me.
/*
    RFileLogger::WriteFormat(_L("Browser"), _L("xml_core.log"), 
                         EFileLoggingModeAppend, _L("In_INT:Name=%s \n"), 
                         ucsName);

*/
  
    /* Length is including the NULL termination. */

    internalEntity->nameLen = totalByteCount; 

    /* value setup */
    length = I_entityValue->stop - I_entityValue->start;
    valueByteLength = length;
    s = NW_XML_Reader_DataAddressFromBuffer(pT, I_entityValue->start,
                                         &valueByteLength, &bufPointer);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    if (valueByteLength != length) {
        return NW_STAT_FAILURE;
    }

    /* Calculate the length of string. Also add the number of characters 
     * required for the NULL termination.
     */
        
    totalByteCount = length + numbytes;


    pValue = (NW_Uint8*) NW_Mem_Malloc(totalByteCount);

    if (pValue  != NULL) 
     {
      (void)NW_Mem_memcpy(pValue , bufPointer, length );


       for(i=0; i < numbytes; i++)
       {
        pValue[length+i] = '\0';
       }
      } /*end if (pValue  != NULL)*/  
     else
     {
      return NW_STAT_OUT_OF_MEMORY;
     }

  
    if(pT->encoding != HTTP_iso_10646_ucs_2)
    {
     s = NW_String_byteToUCS2Char(pValue, length, &ucsValue);

     if(s != NW_STAT_SUCCESS)
     {
      return s;
     }
     totalByteCount = 2*totalByteCount;
    }/*end if(pT->encoding != HTTP_iso_10646_ucs_2)*/
    else
    {
     ucsValue = (NW_Ucs2*) NW_Mem_Malloc(totalByteCount); 
     (void)NW_Mem_memcpy(ucsValue, pValue, totalByteCount);

    }
     /*Now free the pValue */

    if(pValue != NULL)
    {
     NW_Mem_Free(pValue);
    }



      //SSS: Comment me.
/*
    RFileLogger::WriteFormat(_L("Browser"), _L("xml_core.log"), 
                         EFileLoggingModeAppend, _L("In_INT:Value=%s \n"), 
                         ucsValue);
*/



     internalEntity->name  = ucsName;
     internalEntity->value = ucsValue;

    entryCount = internalEntityList->Count();

    errorCode =  internalEntityList->Insert(internalEntity,entryCount);

        if(errorCode != KErrNone)
        {
         return NW_STAT_FAILURE;
        }

        ///SSS:Comment me below.
  /*
   RFileLogger::WriteFormat(_L("Browser"), _L("xml_core.log"), 
                         EFileLoggingModeAppend, _L("In_INT:Adding:nameLen=%d name=%s valueLen=%d value=%s \n"), 
                         internalEntity->nameLen, internalEntity->name, 
                         totalByteCount, internalEntity->value );

  */
 



    return NW_STAT_SUCCESS;

 }/*end CXML_XML_Parser_Store_I_Entity*/

  /** ----------------------------------------------------------------------- **
    @function:    CXML_XML_Parser_Free_I_Entity_List
    @synopsis:    Function to free the internal entity list.
    @scope:       public      

    @parameters: void
        

    @description: Function to free the internal entity list.

    @returns:   
                  void
 -----------------------------------------------------------------------------------**/

 void CXML_XML_Parser_Free_I_Entity_List(RPointerArray <CXML_Internal_Entity_t>  internalEntityList)
 {
  TInt count = 0;
  NW_Int32 i = 0;
  CXML_Internal_Entity_t* internalEntity=NULL;


  count = internalEntityList.Count();

  for(i=0; i < count; i++)
    {
     internalEntity = (CXML_Internal_Entity_t*) internalEntityList[i]; 

     if(internalEntity != NULL)
     {
      /*   

      RFileLogger::WriteFormat(_L("Browser"), _L("xml_core.log"), 
                         EFileLoggingModeAppend, _L("Deleting: name=%s  value=%s \n"), 
                         internalEntity->name, internalEntity->value );
      */

      NW_Mem_Free(internalEntity->name);
      NW_Mem_Free(internalEntity->value);
     }
    } /*end for()*/
 /* This will destroy the internal pointers */

  internalEntityList.ResetAndDestroy();

 }/*end CXML_XML_Parser_Free_I_Entity_List(void)*/


  /** ----------------------------------------------------------------------- **
    @function:    CXML_XML_Resolve_Internal_Entity
    @synopsis:    Function to store the internal entity.
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
                                              void* entityList)
 {
  NW_Uint32 entityNameByteCnt;
  TInt count = 0;
  CXML_Int32 i = 0;
  CXML_Int32 strCmp;
  NW_Ucs2*  valBuf;
  NW_Uint32 valBufByteCnt = 0;
  CXML_Internal_Entity_t* internalEntity=NULL;
  RPointerArray <CXML_Internal_Entity_t>*  internalEntityList = (RPointerArray <CXML_Internal_Entity_t>*)  entityList;

  *entityFound = NW_FALSE;



  /* The entity name at time is always in the Ucs2 so fixed value is used below */

 
  NW_Int32 entityNameLen = NW_String_charBuffGetLength((void*) entityName,
                                              HTTP_iso_10646_ucs_2,
                                              &entityNameByteCnt);

  
  (void) entityNameLen; //To remove the warning
  count = internalEntityList->Count();

  /* Now look for entity name in  "internalEntityList" */

   for(i=0; i < count; i++)
    {
     internalEntity = (CXML_Internal_Entity_t*) (*internalEntityList)[i]; 
      

     if(internalEntity != NULL)
     {
      if(internalEntity->nameLen == entityNameByteCnt)
      {
       strCmp = CXML_Str_Stricmp( (const CXML_Ucs2 *)internalEntity->name, entityName);

       if(strCmp == 0)
       {
         
        valBuf = internalEntity->value ;
        NW_Int32 valBufLen = NW_String_charBuffGetLength( (void*)internalEntity->value,
                                                 HTTP_iso_10646_ucs_2,
                                                 &valBufByteCnt);

        if(valBuf != NULL)
        {

          
         switch(encoding)
         {
          /* write the string based on the actual encoding.*/
         case HTTP_utf_8:
             *entityValStr = (NW_Byte*) CXML_Str_CvtToAscii(valBuf);
             *entityFound = NW_TRUE;
              break;

         case HTTP_iso_8859_1:      
         case HTTP_us_ascii:
              *entityValStr = (NW_Byte*) CXML_Str_CvtToAscii(valBuf);
              *entityFound = NW_TRUE;
              break;
         case HTTP_iso_10646_ucs_2:
             *entityValStr = (CXML_Uint8*) NW_Mem_Malloc(valBufByteCnt);
             if(*entityValStr == NULL)
             {
              return NW_STAT_OUT_OF_MEMORY;
             }
             CXML_Mem_memcpy((void*)*entityValStr,internalEntity->value,valBufByteCnt);
             *entityFound = NW_TRUE;
             break;
         default:
             //Some thing wrong so break
             break;
         }/*end switch(encoding)*/
            
            if(*entityFound == NW_TRUE)
            {
             break;
            }
        }/*end if(valBuf != NULL)*/
       }/*end if(strCmp == 0)*/

      }/*end if(internalEntity->nameLen = entityNameByteCnt)*/
      
     }/*end if(internalEntity != NULL)*/
    } /*end for()*/


   if(*entityFound == NW_FALSE)
   {
    /*If it is not a internal Entity */
    *entityValStr = NULL;
   }

   ///SSS: This will be freed when Internal Entity Array is freed 
   /*
   if(valBuf != NULL)
   {
    NW_Mem_Free(valBuf);
   }
  */
   return NW_STAT_SUCCESS;
 }/*end CXML_XML_Resolve_Internal_Entity() */
