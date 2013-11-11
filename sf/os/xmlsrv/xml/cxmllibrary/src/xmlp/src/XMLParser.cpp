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

#include "cxml_internal.h"
#include <xml/cxml/nw_xmlp_xmlreader.h>
#include <xml/cxml/nw_xmlp_xmlparser.h>
#include <xml/cxml/nw_encoder_wbxmlwriter.h>


#include "cxml_xmlp_entity.h"
#include <xml/cxml/nw_string_string.h>
#include "cxml_xmlp_int_entity.h"



/* "<?" len 2 */
#define NW_XML_String_PiFormStartLength 2
static
const NW_Uint8 NW_XML_String_PiFormStart[NW_XML_String_PiFormStartLength] =
{
    '<', '?'
};

/* "?>" len 2 */
#define NW_XML_String_PiFormStopLength 2
static
const NW_Uint8 NW_XML_String_PiFormStop[NW_XML_String_PiFormStopLength] =
{
    '?', '>'
};

/* "version" len 7 */
#define NW_XML_String_VersionLength 7
static
const NW_Uint8 NW_XML_String_Version[NW_XML_String_VersionLength] =
{
    'v', 'e', 'r', 's', 'i', 'o', 'n'
};

/* "encoding" len 8 */
#define NW_XML_String_EncodingLength 8
static
const NW_Uint8 NW_XML_String_Encoding[NW_XML_String_EncodingLength] =
{
    'e', 'n', 'c', 'o', 'd', 'i', 'n', 'g'
};

/* "standalone" len 10 */
#define NW_XML_String_StandaloneLength 10
static
const NW_Uint8 NW_XML_String_Standalone[NW_XML_String_StandaloneLength] =
{
    's', 't', 'a', 'n', 'd', 'a', 'l', 'o', 'n', 'e'
};

/* "<!DOCTYPE" len 9 */
#define NW_XML_String_DoctypeStartLength 9
static
const NW_Uint8 NW_XML_String_DoctypeStart[NW_XML_String_DoctypeStartLength] =
{
    '<', '!', 'D', 'O', 'C', 'T', 'Y', 'P', 'E'
};

/* "<!ENTITY" len 8 in the DTD */

#define NW_XML_String_EntityStartLength 8
static
const NW_Uint8 NW_XML_String_EntityStart[NW_XML_String_EntityStartLength] =
{
    '<', '!', 'E', 'N', 'T', 'I', 'T', 'Y'
};

/* "<!--" len 4 comment start */
#define NW_XML_String_CommentStartLength 4
static
const NW_Uint8 NW_XML_String_CommentStart[NW_XML_String_CommentStartLength] =
{
    '<', '!', '-', '-'
};

/* "-->" len 3 comment end */
#define NW_XML_String_CommentStopLength 3
static
const NW_Uint8 NW_XML_String_CommentStop[NW_XML_String_CommentStopLength] =
{
    '-', '-', '>'
};

/* "/>" len 2 */
#define NW_XML_String_EmptyTagEndLength 2
static
const NW_Uint8 NW_XML_String_EmptyTagEnd[NW_XML_String_EmptyTagEndLength] =
{
    '/', '>'
};

/* "</" len 2 */
#define NW_XML_String_EndTagStartLength 2
static
const NW_Uint8 NW_XML_String_EndTagStart[NW_XML_String_EndTagStartLength] =
{
    '<', '/'
};

/* "<![CDATA[" len 9 */
#define NW_XML_String_CdataStartLength 9
static
const NW_Uint8 NW_XML_String_CdataStart[NW_XML_String_CdataStartLength] =
{
    '<', '!', '[', 'C', 'D', 'A', 'T', 'A', '['
};

/* "]]>" len 3 */
#define NW_XML_String_CdataEndLength 3
static
const NW_Uint8 NW_XML_String_CdataEnd[NW_XML_String_CdataEndLength] =
{
    ']', ']', '>'
};

/* All case variations of "xml" */
#define NW_XML_String_XmlNameVariationCount 8
#define NW_XML_String_XmlLength 3
static
const NW_Uint8 NW_XML_String_XmlVariations[(NW_XML_String_XmlNameVariationCount
                                        * NW_XML_String_XmlLength)] =
{
    'x', 'm', 'l', /* all lower case form must be first */
    'x', 'm', 'L',
    'x', 'M', 'l',
    'x', 'M', 'L',
    'X', 'm', 'l',
    'X', 'm', 'L',
    'X', 'M', 'l',
    'X', 'M', 'L'
};


/* Assumes position in Reader is at the first character of keyword.
returns: *pMatch = 1 if found keyword and advanced over it, 0 if no match
NOTE: Keyword match just means the string of keyword chars
exists at the read position so it does not mean that the keyword
is delimited at the end---it might be followed by more name chars. */
static
NW_Status_t
NW_XML_Parse_KeywordConsume(NW_XML_Reader_t* pT, NW_XML_Reader_Interval_t* pI,
                            NW_Uint32 l, const NW_Uint8* pKeyword,
                            NW_Uint32* pMatch)
{
    NW_Status_t s;
    NW_XML_Reader_Interval_Init(pI);
    s = NW_XML_Reader_AsciiStringMatch(pT, l, pKeyword, pMatch);
    if (NW_STAT_IS_SUCCESS(s) && *pMatch) {
        NW_XML_Reader_Interval_Start(pI, pT);
        NW_XML_Reader_AdvanceOffset(pT, l);
        NW_XML_Reader_Interval_Stop(pI, pT);
    }
    return s;
}

/*
Parses an XML Name (productions 5, 4) in Reader.
If no parse error, then *pI marks the Name.
Assumes position in Reader is at the first character of name.
returns: *pMatch = 1 if found name and advanced over it, 0 if no match
*/
static
NW_Status_t
NW_XML_Parse_NameConsume(NW_XML_Reader_t* pT, NW_XML_Reader_Interval_t* pI,
                         NW_Uint32* pMatch)
{
    /*
    [4] NameChar ::= Letter | Digit | '.' | '-' | '_' | ':'
                     | CombiningChar | Extender

                     Note: combining and extender ignored here.

    [5] Name ::= (Letter | '_' | ':') (NameChar)*
    */
    NW_Status_t sl;
    NW_Uint32 isLetter;
    NW_Status_t su;
    NW_Uint32 isUnderscore;
    NW_Status_t sc;
    NW_Uint32 isColon;
    NW_Status_t sd = NW_STAT_SUCCESS;
    NW_Uint32 isDigit = 0;
    NW_Status_t sp = NW_STAT_SUCCESS;
    NW_Uint32 isPeriod = 0;
    NW_Status_t sh = NW_STAT_SUCCESS;
    NW_Uint32 isHyphen = 0;
    NW_Status_t s = NW_STAT_SUCCESS;

    sl = NW_XML_Reader_IsLetter(pT, &isLetter);
    su = NW_XML_Reader_AsciiCharMatch(pT, '_', &isUnderscore);
    sc = NW_XML_Reader_AsciiCharMatch(pT, ':', &isColon);
    *pMatch = 0;
    NW_XML_Reader_Interval_Init(pI);
    if (NW_STAT_IS_SUCCESS(sl) && NW_STAT_IS_SUCCESS(su)
        && NW_STAT_IS_SUCCESS(sc)) {
        if (isLetter | isUnderscore | isColon) {
            NW_XML_Reader_Interval_Start(pI, pT);
            while ((NW_STAT_IS_SUCCESS(sl) && NW_STAT_IS_SUCCESS(su)
                    && NW_STAT_IS_SUCCESS(sc) && NW_STAT_IS_SUCCESS(sd)
                    && NW_STAT_IS_SUCCESS(sp) && NW_STAT_IS_SUCCESS(sh)
                    && NW_STAT_IS_SUCCESS(s))
                   && (isLetter | isDigit | isPeriod | isHyphen | isUnderscore
                      | isColon )) {
                s = NW_XML_Reader_Advance(pT);
                sl = NW_XML_Reader_IsLetter(pT, &isLetter);
                sd = NW_XML_Reader_IsDigit(pT, &isDigit);
                sp = NW_XML_Reader_AsciiCharMatch(pT, '.', &isPeriod);
                sh = NW_XML_Reader_AsciiCharMatch(pT, '-', &isHyphen);
                su = NW_XML_Reader_AsciiCharMatch(pT, '_', &isUnderscore);
                sc = NW_XML_Reader_AsciiCharMatch(pT, ':', &isColon);
            }
            NW_XML_Reader_Interval_Stop(pI, pT);
            *pMatch = 1;
        }
    }
    if (NW_STAT_IS_SUCCESS(sl) && NW_STAT_IS_SUCCESS(su)
        && NW_STAT_IS_SUCCESS(sc) && NW_STAT_IS_SUCCESS(sd)
        && NW_STAT_IS_SUCCESS(sp) && NW_STAT_IS_SUCCESS(sh)
        && NW_STAT_IS_SUCCESS(s)) {
        return NW_STAT_SUCCESS;
    }
    return NW_STAT_FAILURE;
}

/* This function reads data from the pT->pBuf and converts this data to
 * the NW_String_t. The string memory is allocated  here but it is freed
 * in the calling function.
 *
 * pT        --> Parser Structute (IN)
 * I_attrVal --> Attribute value Interval parameter (IN)
 * *dataStr   --> Output string (OUT)
 *
 * 
 */

static NW_Status_t NW_XML_Data_to_String(NW_XML_Reader_t* pT, NW_XML_Reader_Interval_t* I_attrVal,
                             NW_String_t* dataStr)
{
 NW_Uint8* pData;
 NW_Uint32 numbytes = 0;
 NW_Uint32 totalByteCount = 0;
 CXML_Uint8* tempAttrValBuf;
 CXML_Uint32 tempBufLen = 0;
 NW_Uint32 length;
 NW_Uint32 i =0;
 NW_Ucs2 c;
 NW_Status_t s;

     length = I_attrVal->stop - I_attrVal->start; 
     tempBufLen = length; //Desired bytes need to read.

     s = NW_XML_Reader_DataAddressFromBuffer(pT,I_attrVal->start,
                                             &tempBufLen,
                                             &tempAttrValBuf);

         if (NW_STAT_IS_FAILURE(s))
         {
            return s;
         }

        if (tempBufLen != length) 
        {
            return NW_STAT_FAILURE;
        }


 
    /* pData is not NULL terminated so need to use following method. The num byte
     * will be used for both pName and pVlaue.
     */


    numbytes = NW_String_readChar( (NW_Byte*) tempAttrValBuf,&c,pT->encoding);

    /* Calculate the length of string. Also add the number of characters 
     * required for the NULL termination.
     */
        
    totalByteCount = length + numbytes;

    pData = (NW_Uint8*) NW_Mem_Malloc(totalByteCount);

    if (pData != NULL) 
     {
      (void)NW_Mem_memcpy(pData , tempAttrValBuf, length );


       for(i=0; i < numbytes; i++)
       {
        pData[length+i] = '\0';
       }
      } /*end if (pName  != NULL)*/
     else
     {
      return NW_STAT_OUT_OF_MEMORY;
     }

     s = NW_String_initialize( dataStr , pData, pT->encoding);

     if (NW_STAT_IS_FAILURE(s))
     {
      return s;
     }

     /* This will the storage of the dataStr by the NW_String_delete() */

     dataStr->length |= 0x80000000;

 return NW_STAT_SUCCESS;

}/*end NW_XML_Data_to_String()*/

/* Assumes position in Reader is at the opening quote character for value. 
* BUG not yet spec compliant.
* The following function is called for Process Instruction and Element
* attribute consume. The predefined entities will be handled in 
* element attributes only. So, the "entity" parameter is used to 
* distinguish between these two cases.
* Careful about the "entity" parameter passed. This is used both as 
* IN/OUT paramter.

*  IN --> if(entity == CXML_TRUE): Then parse attribute value for the
*         entities.

*  OUT --> (entity=CXML_TRUE): The entity is found in the attribute value.

*/

static
NW_Status_t
NW_XML_Parse_ValueConsume(NW_XML_Reader_t* pT, NW_XML_Reader_Interval_t* pI,
						  CXML_Bool* entityCheck, NW_String_t* attrValStr,
                          const RPointerArray <CXML_Internal_Entity_t>*  internalEntityList)
{
    /*
    Literal data is any quoted string not containing the quotation mark
    used as a delimiter for that string.

    [10] AttValue ::=   '"' ([^<&"] | Reference)* '"'
                      | "'" ([^<&'] | Reference)* "'"
    */

    /* BUG this only pays attention to the quote chars not the value
    so it ignores [<&]. */
    NW_Status_t s;
    NW_Status_t ssq;
    NW_Uint32 isOpenSingleQuote;
    NW_Uint32 isCloseSingleQuote = 0;
    NW_Status_t sdq;
    NW_Uint32 isOpenDoubleQuote;
    NW_Uint32 isCloseDoubleQuote = 0;
    NW_Status_t slt;
    NW_Uint32 isLessThan;

	NW_Uint32 prevIndex;
	NW_Uint32 prevCharIndex;
	NW_XML_Reader_LineColumn_t prevLineColumn;
    CXML_Uint8* intEntityValStr = NULL;
    CXML_Uint32 tempBufLen = 0;
	NW_String_t* tempStr = NULL;
	NW_String_t entityValStr;
    CXML_Bool entityFoundLevel_2 =  NW_FALSE;
	CXML_Bool entityFoundOnce = NW_FALSE;
	NW_String_initialize (&entityValStr, NULL, 0);
	



    NW_XML_Reader_Interval_Init(pI);
    ssq = NW_XML_Reader_AsciiCharMatch(pT, '\'', &isOpenSingleQuote);
    sdq = NW_XML_Reader_AsciiCharMatch(pT, '\"', &isOpenDoubleQuote);
    if (NW_STAT_IS_FAILURE(sdq) || NW_STAT_IS_FAILURE(ssq)
        || (!isOpenSingleQuote && !isOpenDoubleQuote)) {
        return NW_STAT_FAILURE;
    }
    /* xor, mutually exclusive */
    NW_ASSERT(isOpenSingleQuote ^ isOpenDoubleQuote);
    s = NW_XML_Reader_Advance(pT);
    if (NW_STAT_IS_FAILURE(s)) {
        return NW_STAT_FAILURE;
    }
    NW_XML_Reader_Interval_Start(pI, pT);
    for (;;) {

        /* Check for the closing quotes. If this is empty attribute value 
         * then no need for the checking the entities.
         */

        if (isOpenSingleQuote) {
            ssq = NW_XML_Reader_AsciiCharMatch(pT, '\'', &isCloseSingleQuote);
        }
        if (isOpenDoubleQuote) {
            sdq = NW_XML_Reader_AsciiCharMatch(pT, '\"', &isCloseDoubleQuote);
        }
        slt = NW_XML_Reader_AsciiCharMatch(pT, '<', &isLessThan);
        if (NW_STAT_IS_FAILURE(ssq) || NW_STAT_IS_FAILURE(sdq)
            || NW_STAT_IS_FAILURE(slt) || isLessThan) {
            return NW_STAT_FAILURE;
        }


        if( (*entityCheck == CXML_TRUE) && !isCloseSingleQuote && !isCloseDoubleQuote)
        {
            NW_Uint32 match;

            s = NW_XML_Reader_AsciiCharMatch(pT, '&', &match); 

		    if (NW_STAT_IS_FAILURE(s))
		    {
                return NW_STAT_FAILURE;
            }
            if (match) 
		    {
              NW_XML_Reader_Interval_t I_entityData;
		      NW_Bool entityFoundLevel_1 =  NW_FALSE; //If end of entity (;) found
		      NW_Uint32 entityVal = 0;
		      NW_XML_Reader_Interval_t* I_attrVal = pI;
              


		      entityFoundLevel_2 =  NW_FALSE;
              NW_XML_Reader_Interval_Stop(I_attrVal, pT); //Contents before entity
             
			  //Will back if not a valid entity
			  NW_XML_Reader_GetPosition(pT,
                              &prevIndex, &prevCharIndex, &prevLineColumn);

		      s = CXML_XML_Parser_Entity(pT,&I_entityData,&entityFoundLevel_1);

                if (NW_STAT_IS_FAILURE(s))
		        {
                    return NW_STAT_FAILURE;
                }
               /*
                if(entityFoundLevel_1 == NW_FALSE)
                {
                  return NW_STAT_XHTML_BAD_CONTENT;
                }
                */
		      if (entityFoundLevel_1) 
              {
			    // Validate the entity
                // The following function checks for the character,
                // predefined and Internal Entities.

                if( (I_entityData.stop - I_entityData.start) > 0)
                {
			    s = CXML_XML_Handle_entity(pT,
                                           &I_entityData,&entityVal,&intEntityValStr,
                                           &entityFoundLevel_2,
                                           (void*) internalEntityList);
                }
                else
                {
                 //Not a valid entity e.g. "&&;" test case
                 entityFoundLevel_2 = NW_FALSE;
                }

			    if (NW_STAT_IS_FAILURE(s)) 
			    {
			     return NW_STAT_FAILURE;
			    }

                if(entityFoundLevel_2 == CXML_TRUE)
			    {
                 /* Read contents before entity*/

                   tempBufLen = I_attrVal->stop - I_attrVal->start; 

				   if(tempBufLen > 0)
				   {

                    if(tempStr == NULL)
					 {
					  tempStr = NW_String_new();
					  if(tempStr == NULL)
					  {
					   return NW_STAT_OUT_OF_MEMORY;
                      }
					  
                    } /*end if(tempStr == NULL)*/

                    s = NW_XML_Data_to_String (pT,I_attrVal,tempStr);
                                                          
                    if (NW_STAT_IS_FAILURE(s))
                    {
                     return s;
                    }

					 s = NW_String_concatenate(attrValStr,tempStr,pT->encoding);


					 if (NW_STAT_IS_FAILURE(s))
                     {
                      return s;
                     }
                    
                    // Do some clean up

					if(tempStr != NULL)
					{
					 NW_String_delete(tempStr);
					 tempStr = NULL;
					}

				   }//end if(tempBufLen > 0)

                 // Write the entity content now. There are two possibilities for entities.
                 //
                 // 1) If it is character or decimal or predefined entities. 
                 //    In this case, intEntityValStr = NULL.
                 //
                 // 2) If it is "Internal Entity" then In this case, entityVal = 0;

                   if (intEntityValStr == NULL)
                   {

                    /* convert contents of the character/predfined entity to string */    

				     s = NW_String_entityToString(entityVal,&entityValStr,pT->encoding);

					 if (NW_STAT_IS_FAILURE(s))
                     {
                      return s;
                     }
                   }/*end if (intEntityValStr == NULL)*/
                   else if(entityVal == 0)
                   {
                        /* This is a internal entity string */

                         s = NW_String_initialize(&entityValStr,intEntityValStr,pT->encoding);
                         if (NW_STAT_IS_FAILURE(s))
                         {
                          return s;
                         }
                   } /*end else if(entityVal == 0)*/

                     /* Add this entity value to the string */
			
					 s = NW_String_concatenate(attrValStr,&entityValStr,pT->encoding);

					 if (NW_STAT_IS_FAILURE(s))
                     {
                      return s;
                     }
                  

				
					if(entityValStr.storage != NULL)
                    {
                     NW_Mem_Free (entityValStr.storage);
                    }
                    // Initialize the entity string for next entity 

					NW_String_initialize (&entityValStr, NULL, 0);

                   //Again start the top level interval

                    NW_XML_Reader_Interval_Init(pI);
                    NW_XML_Reader_Interval_Start(pI, pT);

					/*Check for the closing quotes after entity parsing */

					if (isOpenSingleQuote) {
						ssq = NW_XML_Reader_AsciiCharMatch(pT, '\'', &isCloseSingleQuote);
					}
					if (isOpenDoubleQuote) {
						sdq = NW_XML_Reader_AsciiCharMatch(pT, '\"', &isCloseDoubleQuote);
					}
					slt = NW_XML_Reader_AsciiCharMatch(pT, '<', &isLessThan);
					if (NW_STAT_IS_FAILURE(ssq) || NW_STAT_IS_FAILURE(sdq)
						|| NW_STAT_IS_FAILURE(slt) || isLessThan) {
						return NW_STAT_FAILURE;
					}
					entityFoundOnce = CXML_TRUE;
			    }//endif(entityFoundLevel_2 == CXML_TRUE)
                else
                {
                 /* If it is here, the entity is not well formed or a entity is
                  * not supported. But, it is error for now. 
                  */
                /*
                   return NW_STAT_XHTML_BAD_CONTENT;*/

                //No valid entity found. Parse as the normal string
                 NW_XML_Reader_SetPosition(pT,
                                        prevIndex,
                                        prevCharIndex,
                                        &prevLineColumn);
                }
                
             } //end if (entityFound && inContent)
			else
			{
			 //No valid entity found. Parse as the normal string
			  NW_XML_Reader_SetPosition(pT,
                                        prevIndex,
                                        prevCharIndex,
                                        &prevLineColumn);
			}//end else
		  } //end match
        } //end if( !isCloseSingleQuote && !isCloseDoubleQuote)



        if ((isOpenSingleQuote & isCloseSingleQuote)
            | (isOpenDoubleQuote & isCloseDoubleQuote)) {
            break;
        }

		if(entityFoundLevel_2 != CXML_TRUE)
		{
         s = NW_XML_Reader_Advance(pT);
		}
		else
		{
		 entityFoundLevel_2 = CXML_FALSE;
		}
    }//end for (;;)
    NW_XML_Reader_Interval_Stop(pI, pT);
    s = NW_XML_Reader_Advance(pT);

    
    if( (*entityCheck == CXML_TRUE) && 
		( (entityFoundLevel_2 == CXML_TRUE) || (entityFoundOnce == CXML_TRUE) ) )
    {
      /* Get rest of the attribute value contents */
     
		
       tempBufLen = pI->stop - pI->start; 

		if(tempBufLen > 0)
		{
         if(tempStr == NULL)
		 {
		  tempStr = NW_String_new();
		  if(tempStr == NULL)
		  {
		   return NW_STAT_OUT_OF_MEMORY;
          }
		  
         } /*end if(tempStr == NULL)*/
        s = NW_XML_Data_to_String (pT,pI,tempStr);

        if (NW_STAT_IS_FAILURE(s))
         {
          return s;
         }


			s = NW_String_concatenate(attrValStr,tempStr,pT->encoding);

			if (NW_STAT_IS_FAILURE(s))
             {
              return s;
             }

            // Do some clean up

			if(tempStr != NULL)
			{
			 NW_String_delete(tempStr);
			 tempStr = NULL;
			}

		}//endif(tempBufLen > 0)

    }//end if( (*entityCheck == CXML_TRUE) && (entityFoundLevel_2 == CXML_TRUE) )
	else
	{
	 *entityCheck = CXML_FALSE;
	}

    return s;
}

/*
Parses an XML attribute (production ???) in Reader.
If no parse error, then ti_name and ti_attvalue mark the two items.
Allows for leading whitespace.  If l is > 0 then p is a "string" of
length l that is the name that must match (parse error if doesn't).
Assumes position in Reader is at whitespace before or first character
of attribute.
return: *pMatch = 1 if keyword or attribute name found

 Careful about the "entity" parameter passed. This is used both as 
 IN/OUT paramter.

  IN --> if(entity == CXML_TRUE): Then parse attribute value for the
         entities.

  OUT --> (entity=CXML_TRUE): The entity is found in the attribute value.
*/


static
NW_Status_t
NW_XML_Parse_AttributeValueConsume(NW_XML_Reader_t* pT,
                                   NW_XML_Reader_Interval_t* pI_name,
                                   NW_XML_Reader_Interval_t* pI_attvalue,
                                   NW_Uint32 l, const NW_Uint8* pKeyword,
                                   NW_Uint32* pMatch,
								   CXML_Bool* entity, NW_String_t*  attrValStr,
                                   const RPointerArray <CXML_Internal_Entity_t>*  internalEntityList)
{
    /*
    [41] Attribute ::= Name Eq AttValue
    where Name is possibly a keyword that must match
    */
    NW_Status_t s;
    NW_Uint32 match;
    NW_Uint32 prevIndex0;
    NW_Uint32 prevIndex1;
    NW_Uint32 prevCharIndex0;
    NW_Uint32 prevCharIndex1;
    NW_XML_Reader_LineColumn_t prevLineColumn0;
    NW_XML_Reader_LineColumn_t prevLineColumn1;

    *pMatch = 0;

    /* S mandatory */
    NW_XML_Reader_GetPosition(pT, &prevIndex0, &prevCharIndex0, &prevLineColumn0);
    s = NW_XML_Reader_SkipSpace(pT);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    NW_XML_Reader_GetPosition(pT, &prevIndex1, &prevCharIndex1, &prevLineColumn1);
    if (prevIndex0 == prevIndex1) {
        return NW_STAT_FAILURE;
    }
    /* Name */
    if ((l != 0U) && (pKeyword != NULL)) {
        s = NW_XML_Parse_KeywordConsume(pT, pI_name, l, pKeyword, pMatch);
        if (NW_STAT_IS_SUCCESS(s) && !*pMatch) {
            /* backup so mandatory space not consumed */
            NW_XML_Reader_SetPosition(pT, prevIndex0, prevCharIndex0, &prevLineColumn0);
        }
    } else {
        s = NW_XML_Parse_NameConsume(pT, pI_name, pMatch);
    }
    if (NW_STAT_IS_FAILURE(s) || !*pMatch) {
        return NW_STAT_FAILURE;
    }
    /* S */
    s = NW_XML_Reader_SkipSpace(pT);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    /* = */
    s = NW_XML_Reader_AsciiCharMatch(pT, '=', &match);
    if (NW_STAT_IS_FAILURE(s) || !match) {
        return NW_STAT_FAILURE;
    }
    s = NW_XML_Reader_Advance(pT);
    if (NW_STAT_IS_FAILURE(s)) {
        return NW_STAT_FAILURE;
    }
    /* S */
    s = NW_XML_Reader_SkipSpace(pT);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    /* attValue */

   if(*entity == CXML_FALSE)
   {
    return NW_XML_Parse_ValueConsume(pT, pI_attvalue,entity,NULL, internalEntityList);  
   }
   else
   {
    return NW_XML_Parse_ValueConsume(pT, pI_attvalue,entity,attrValStr, internalEntityList);
   }
}

/*
Parses an XML Comment (production 15) in Reader.
If no parse error, then ti marks the Comment--all chars
between the start and end marks including spaces.
Assumes position in Reader is the character after "<!--".
*/
static
NW_Status_t
NW_XML_Parse_CommentConsume(NW_XML_Reader_t* pT, NW_XML_Reader_Interval_t* pI,
                            const struct NW_XML_Parser_EventCallbacks_s* pE)
{
    /*
    [15] Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'

    Note: the pattern with (Char - '-') means that the comment cannot
    end with '--->' although it can begin with '<!--...'.

    Also, '--' may not appear in a comment.
    BUG not yet implemented
    */
    NW_Status_t s;
    NW_Uint32 match;

    NW_XML_Reader_Interval_Start(pI, pT);
    for (;;) {
        s = NW_XML_Reader_AsciiStringMatch(pT, NW_XML_String_CommentStopLength,
                                        NW_XML_String_CommentStop, &match);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
        if (match) {
            break;
        }
        s = NW_XML_Reader_Advance(pT);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
    }
    NW_XML_Reader_Interval_Stop(pI, pT);
    s = NW_XML_Reader_AdvanceOffset(pT, NW_XML_String_CommentStopLength);
    NW_ASSERT(NW_STAT_IS_SUCCESS(s)); /* should never fail */
    if (pE->Comment_CB != NULL) {
        s = (*(pE->Comment_CB))(pT, pI, pE->pClientPointer);
    }
    return s;
}

/*
Parses things that start with "<?".  These are XMLDecl, TextDecl and
the Processing Instruction.  On return the formType distinguishes which one
was consumed.  If it was a PI then ti_name marks the PITarget and
ti_content marks the text "argument" of the PI.  If it was an XMLDecl
(or TextDecl) then examine the booleans to see if various attributes appeared.
Note that XMLDecl must have VersionInfo and a TextDecl must not have an SDDecl.
Neither XMLDecl or TextDecl should have additional content
(i.e., *ti_contentValid should be 0).
Assumes position in Reader is the character after "<?".
*/
static
NW_Status_t
NW_XML_Parse_PiFormConsume(NW_XML_Reader_t* pT, NW_PiFormTypeTag_t* pFormType,
                           NW_Uint32* pNameValid,
                           NW_XML_Reader_Interval_t* pI_name,
                           NW_Uint32* pVersionValid,
                           NW_XML_Reader_Interval_t* pI_version,
                           NW_Uint32* pEncodingValid,
                           NW_XML_Reader_Interval_t* pI_encoding,
                           NW_Uint32* pStandaloneValid,
                           NW_XML_Reader_Interval_t* pI_standalone,
                           NW_Uint32* pContentValid,
                           NW_XML_Reader_Interval_t* pI_content,
                           const RPointerArray <CXML_Internal_Entity_t>*  internalEntityList)
{
    /*
    [16] PI ::= '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'

    [17] PITarget ::= Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))

    when name is an XMLDecl or TextDecl then parse according to

    [23] XMLDecl ::= '<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'

    [77] TextDecl ::= '<?xml' VersionInfo? EncodingDecl S? '?>'

    [24] VersionInfo ::= S 'version' Eq ("'" VersionNum "'"
                         | '"' VersionNum '"')

    [26] VersionNum ::= ([a-zA-Z0-9_.:] | '-')+

    [80] EncodingDecl ::= S 'encoding' Eq
                          ('"' EncName '"' | "'" EncName "'" )

    [81] EncName ::= [A-Za-z] ([A-Za-z0-9._] | '-')*

    [32] SDDecl ::= S 'standalone' Eq (("'" ('yes' | 'no') "'")
                    | ('"' ('yes' | 'no') '"'))

    */
    NW_Status_t s;
    NW_XML_Reader_Interval_t I_keyword;
    NW_Uint32 prevIndex;
    NW_Uint32 prevCharIndex;
    NW_XML_Reader_LineColumn_t prevLineColumn;
    NW_Uint32 match;
    NW_Uint32 i;
	CXML_Bool entity = CXML_FALSE;


    *pNameValid = *pVersionValid = *pEncodingValid = 0;
    *pStandaloneValid = *pContentValid = 0;
    *pFormType = UNKNOWFORM;


    s = NW_STAT_FAILURE;
    match = 0;
    /* Will back up if "xml" is part of a longer name like "xml-stylesheet". */
    NW_XML_Reader_GetPosition(pT,
                              &prevIndex, &prevCharIndex, &prevLineColumn);
    for (i = 0; i < NW_XML_String_XmlNameVariationCount; i++) {
        s = NW_XML_Parse_KeywordConsume(pT, pI_name,
                                        NW_XML_String_XmlLength,
                                        (NW_XML_String_XmlVariations
                                         + NW_XML_String_XmlLength * i),
                                        &match);
        if (NW_STAT_IS_SUCCESS(s) && match) {
            /* Make sure that the "xml" variant isn't part of a longer name */
            NW_Uint32 m;
            /* look for end of piform */
            s = NW_XML_Reader_AsciiStringMatch(pT, NW_XML_String_PiFormStopLength,
                                            NW_XML_String_PiFormStop, &m);
            if (NW_STAT_IS_SUCCESS(s) && m) {
                break;
            }
            /* look for whitespace */
            s = NW_XML_Reader_IsSpace(pT, &m);
            if (NW_STAT_IS_SUCCESS(s) && m) {
                break;
            }
            /* if get here then "xml" variant is part of a longer name,
            fail match, back up and shortcut out of loop */
            match = 0;
            NW_XML_Reader_SetPosition(pT,
                                      prevIndex,
                                      prevCharIndex,
                                      &prevLineColumn);
            i = NW_XML_String_XmlNameVariationCount;
            break;
        }
    }

    if (NW_STAT_IS_SUCCESS(s) && !match) {
        s = NW_XML_Parse_NameConsume(pT, pI_name, &match);
    }
    if (NW_STAT_IS_FAILURE(s) || !match) {
        return NW_STAT_FAILURE;
    }
    *pNameValid = 1;

    /* Support all the XML declaration starting with any possible combination. */

    if (i < NW_XML_String_XmlNameVariationCount) 
    { 
        /* BUG there is currently no checking of legal values according
        to productions 26, 81 and 32 */
        *pFormType = XMLDECL;
        s = NW_XML_Parse_AttributeValueConsume(pT, &I_keyword, pI_version,
                                               NW_XML_String_VersionLength,
                                               NW_XML_String_Version, &match,
											   &entity,NULL,
                                               internalEntityList);
        if (NW_STAT_IS_FAILURE(s)) {
            if (match) {
                return NW_STAT_FAILURE; /* couldn't parse version value */
            }
        } else {
            if (match) {
                *pVersionValid = 1;
            }
        }
        s = NW_XML_Parse_AttributeValueConsume(pT, &I_keyword, pI_encoding,
                                               NW_XML_String_EncodingLength,
                                               NW_XML_String_Encoding, &match,
											   &entity,NULL,internalEntityList);
        if (NW_STAT_IS_FAILURE(s)) {
            if (match) {
                return NW_STAT_FAILURE; /* couldn't parse encoding value */
            }
        } else {
            if (match) {
                *pEncodingValid = 1;
            }
        }
        s = NW_XML_Parse_AttributeValueConsume(pT, &I_keyword, pI_standalone,
                                               NW_XML_String_StandaloneLength,
                                               NW_XML_String_Standalone, &match,
											   &entity,NULL,internalEntityList);
        if (NW_STAT_IS_FAILURE(s)) {
            if (match) {
                return NW_STAT_FAILURE; /* couldn't parse standalone value */
            }
        } else {
            if (match) {
                *pStandaloneValid = 1;
            }
        }
    } else {
        *pFormType = PI;
    }
    s = NW_XML_Reader_SkipSpace(pT);
    if (NW_STAT_IS_FAILURE(s)) {
        return NW_STAT_FAILURE;
    }
    NW_XML_Reader_Interval_Start(pI_content, pT);
    for (;;) {
        s = NW_XML_Reader_AsciiStringMatch(pT, NW_XML_String_PiFormStopLength,
                                        NW_XML_String_PiFormStop, &match);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
        if (match) {
            break;
        }
        s = NW_XML_Reader_Advance(pT);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
    }
    NW_XML_Reader_Interval_Stop(pI_content, pT);
    if (pI_content->start == pI_content->stop) {
        *pContentValid = 0;
    } else {
        *pContentValid = 1;
    }
    s = NW_XML_Reader_AdvanceOffset(pT, NW_XML_String_PiFormStopLength);
    NW_ASSERT(NW_STAT_IS_SUCCESS(s)); /* should never fail */
    return s;
}

/* Assumes position is either at whitespace or beginning of text. */
static
NW_Status_t
NW_XML_Parse_MiscConsume(NW_XML_Reader_t* pT,
                         const struct NW_XML_Parser_EventCallbacks_s* pE,
                         const RPointerArray <CXML_Internal_Entity_t>*  internalEntityList)
{
    /*
    [27] Misc ::= Comment | PI | S

    Note: All invocations of this production take the form "Misc*"
    */
    NW_Status_t s;
    NW_XML_Reader_Interval_t I;
    NW_Uint32 match;
    NW_Uint32 prevIndex0; /* will do while there is advancement thru Reader */
    NW_Uint32 prevIndex1; /* will do while there is advancement thru Reader */
    NW_Uint32 prevCharIndex; /* ignored */
    NW_XML_Reader_LineColumn_t prevLineColumn; /* ignored */
    do {
        NW_XML_Reader_GetPosition(pT, &prevIndex0, &prevCharIndex, &prevLineColumn);
        s = NW_XML_Reader_SkipSpace(pT);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }

        if(pT->end){
            break;
        }
        s = NW_XML_Parse_KeywordConsume(pT, &I,
                                        NW_XML_String_CommentStartLength,
                                        NW_XML_String_CommentStart, &match);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
        if (match) {
            s = NW_XML_Parse_CommentConsume(pT, &I, pE);
            if (NW_STAT_IS_FAILURE(s)) {
                return NW_STAT_FAILURE;
            }
        } else {
            s = NW_XML_Parse_KeywordConsume(pT, &I,
                                            NW_XML_String_PiFormStartLength,
                                            NW_XML_String_PiFormStart, &match);
            if (NW_STAT_IS_FAILURE(s)) {
                return NW_STAT_FAILURE;
            }
            if (match) {
                NW_PiFormTypeTag_t pi_type;
                NW_XML_Reader_Interval_t I_name, I_version, I_encoding, I_standalone;
                NW_XML_Reader_Interval_t I_content;
                NW_Uint32 nameValid, versionValid, encodingValid, standaloneValid;
                NW_Uint32 contentValid;
                s = NW_XML_Parse_PiFormConsume(pT, &pi_type,
                                               &nameValid,
                                               &I_name,
                                               &versionValid,
                                               &I_version,
                                               &encodingValid,
                                               &I_encoding,
                                               &standaloneValid,
                                               &I_standalone,
                                               &contentValid,
                                               &I_content,
                                               internalEntityList);
                if (NW_STAT_IS_FAILURE(s) || !nameValid) {
                    return NW_STAT_FAILURE;
                }
                if (pi_type != PI) {
                    /* BUG Is this really an illegal case? */
                    return NW_STAT_FAILURE;
                }
                if (pE->PiForm_CB != NULL) {
                    s = (*(pE->PiForm_CB))(pT, pi_type,
                                           (nameValid ? &I_name : NULL),
                                           (versionValid ? &I_version : NULL),
                                           (encodingValid ? &I_encoding : NULL),
                                           (standaloneValid ? &I_standalone : NULL),
                                           (contentValid ? &I_content : NULL),
                                           pE->pClientPointer);
                }
            }
        }
        NW_XML_Reader_GetPosition(pT, &prevIndex1, &prevCharIndex, &prevLineColumn);
    } while (prevIndex0 != prevIndex1); /* do while */
    return NW_STAT_SUCCESS;
}


/* on entry position should be just after '<!DOCTYPE' */

static
NW_Status_t
NW_XML_Parse_InternalEntity(NW_XML_Reader_t* pT,
                           RPointerArray <CXML_Internal_Entity_t>*  internalEntityList)
{
 NW_Status_t s;
 NW_XML_Reader_Interval_t I_EntityName;
 NW_Uint32 match;
 NW_XML_Reader_Interval_t I_EntityValue;
 NW_Uint32 quoteChar = 0; /* 0 = double quote, 1 = single quote */
 NW_Uint32 matchDoubleQuote;
 NW_Uint32 matchSingleQuote;
 NW_Uint32 matchPercentSign;
 NW_Status_t statusDoubleQuote;
 NW_Status_t statusSingleQuote;
 

 

    s = NW_XML_Reader_SkipSpace(pT);
    if (NW_STAT_IS_FAILURE(s)) 
    {
        return NW_STAT_FAILURE;
    }

	/* The Internal Parameteric Entities has "%" symbol in the beginning of the entity.
	 * Check for parametric entity has skip the symbol.
	 */

	s =  NW_XML_Reader_AsciiCharMatch(pT, '%', &matchPercentSign);

    if (NW_STAT_IS_FAILURE(s)) 
    {
        return NW_STAT_FAILURE;
    }

	if(matchPercentSign)
	{
		/*Advance from '%' sign */

        s = NW_XML_Reader_Advance(pT);
        if (NW_STAT_IS_FAILURE(s)) 
        {
            return NW_STAT_FAILURE;
        }
      
		s = NW_XML_Reader_SkipSpace(pT);

		if (NW_STAT_IS_FAILURE(s)) 
		{
			return NW_STAT_FAILURE;
		}

	} /*end if(matchPercentSign)*/

    /* Name */

    s = NW_XML_Parse_NameConsume(pT, &I_EntityName, &match);
    if (NW_STAT_IS_FAILURE(s)) 
    {
        return NW_STAT_FAILURE;
    }

    if (!match) 
    {
        /* FATAL bad tag */
        return NW_STAT_FAILURE;
    }

    /* S, space may be there after the Entry Name */

    s = NW_XML_Reader_SkipSpace(pT);
    if (NW_STAT_IS_FAILURE(s)) 
    {
        return NW_STAT_FAILURE;
    }

    /* Look for opening single or double quote. Let the closing '>'
     * be handled in the DTD function.
     */

      statusDoubleQuote = NW_XML_Reader_AsciiCharMatch(pT, '\"', &matchDoubleQuote);
      statusSingleQuote = NW_XML_Reader_AsciiCharMatch(pT, '\'', &matchSingleQuote);

      if (NW_STAT_IS_FAILURE(statusDoubleQuote)
            || NW_STAT_IS_FAILURE(statusSingleQuote) ) 
        {
            return NW_STAT_FAILURE;
        }
        
      if(matchDoubleQuote || matchSingleQuote)
      {

        if(matchSingleQuote) /* Look for corresponding quote */
        {
         quoteChar = 1; /* 0 = double quote, 1 = single quote */
        }
        /*Advance from the Quote */

        s = NW_XML_Reader_Advance(pT);
        if (NW_STAT_IS_FAILURE(s)) 
        {
            return NW_STAT_FAILURE;
        }

       /* Start the Internal for the name space */

        NW_XML_Reader_Interval_Start(&I_EntityValue, pT);

       for(; ;)
       {
        statusDoubleQuote = NW_XML_Reader_AsciiCharMatch(pT, '\"', &matchDoubleQuote);
        statusSingleQuote = NW_XML_Reader_AsciiCharMatch(pT, '\'', &matchSingleQuote);

        if(matchDoubleQuote || matchSingleQuote)
        {
         if(!quoteChar && matchDoubleQuote)
         {
          NW_XML_Reader_Interval_Stop(&I_EntityValue, pT);
          break;
         }
         else if(quoteChar && matchSingleQuote)
         {
          NW_XML_Reader_Interval_Stop(&I_EntityValue, pT);
          break;
         }
        }/* if(matchDoubleQuote || matchSingleQuote)*/

        /*Read next character */

        s = NW_XML_Reader_Advance(pT);

        if (NW_STAT_IS_FAILURE(s)) 
        {
            return NW_STAT_FAILURE;
        }

       }/*end for */
      }/*end if(statusDoubleQuote || statusSingleQuote)*/


      s = CXML_XML_Parser_Store_I_Entity(pT,&I_EntityName,&I_EntityValue,internalEntityList);

      if(s != NW_STAT_SUCCESS ) 
        {
         return s;
        }


 return NW_STAT_SUCCESS;

}/*end NW_XML_Parse_InternalEntity() */

/* on entry position should be just after '<!DOCTYPE' */
static
NW_Status_t
NW_XML_Parse_DtdConsume(NW_XML_Reader_t* pT,
                        RPointerArray <CXML_Internal_Entity_t>*  internalEntityList)
{
    NW_Status_t s;
    NW_XML_Reader_Interval_t I_name;
    NW_Uint32 match;
    NW_XML_Reader_Interval_t I_keyword;
    NW_Uint32 inString = 0;
    NW_Uint32 quoteChar = 0; /* 0 = double quote, 1 = single quote */
    NW_Uint32 balance = 1; /* already read opening greater-than sign */

    /* S */
    s = NW_XML_Reader_SkipSpace(pT);
    if (NW_STAT_IS_FAILURE(s)) {
        return NW_STAT_FAILURE;
    }
    /* Name */
    s = NW_XML_Parse_NameConsume(pT, &I_name, &match);
    if (NW_STAT_IS_FAILURE(s)) {
        return NW_STAT_FAILURE;
    }
    if (!match) {
        /* FATAL bad tag */
        return NW_STAT_FAILURE;
    }
    /* You can put quoted '<' and '>' in literal value strings which must be ignored. */
    while (balance) {
        NW_Uint32 matchDoubleQuote;
        NW_Uint32 matchSingleQuote;
        NW_Uint32 matchGreaterThan;
        NW_Uint32 matchLessThan;
        NW_Status_t statusDoubleQuote;
        NW_Status_t statusSingleQuote;
        NW_Status_t statusGreaterThan;
        NW_Status_t statusLessThan;

        statusDoubleQuote = NW_XML_Reader_AsciiCharMatch(pT, '\"', &matchDoubleQuote);
        statusSingleQuote = NW_XML_Reader_AsciiCharMatch(pT, '\'', &matchSingleQuote);
        statusGreaterThan = NW_XML_Reader_AsciiCharMatch(pT, '>', &matchGreaterThan);
        statusLessThan    = NW_XML_Reader_AsciiCharMatch(pT, '<', &matchLessThan);

        if (NW_STAT_IS_FAILURE(statusDoubleQuote)
            || NW_STAT_IS_FAILURE(statusSingleQuote)
            || NW_STAT_IS_FAILURE(statusGreaterThan)
            || NW_STAT_IS_FAILURE(statusLessThan)) {
            return NW_STAT_FAILURE;
        }

        if (inString) {
            if (quoteChar) {
                if (matchSingleQuote) {
                    inString = 0;
                }
            } else if (matchDoubleQuote) {
                inString = 0;
            }
        } else 
            {
              if (matchGreaterThan) 
              {
                NW_ASSERT(balance);
                balance--;
               } else if (matchLessThan) 
                {
                    balance++;
                     /* '<!ENTITY' */
                    s = NW_XML_Parse_KeywordConsume(pT, &I_keyword,
                                    NW_XML_String_EntityStartLength,
                                    NW_XML_String_EntityStart, &match);
                    if (NW_STAT_IS_FAILURE(s)) 
                    {
                     return NW_STAT_FAILURE;
                    }
                    if (match) 
                    {
                        s = NW_XML_Parse_InternalEntity(pT,internalEntityList);
                        if (NW_STAT_IS_FAILURE(s))
                        {
                            return NW_STAT_FAILURE;
                        }
                    }
                } else if (matchSingleQuote) 
                {
                 quoteChar = 1;
                 inString = 1;
                }
                else if (matchDoubleQuote) 
                {
                 quoteChar = 0;
                 inString = 1;
                }
            } /*end else */
        s = NW_XML_Reader_Advance(pT);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
    }
    return NW_STAT_SUCCESS;
}

static
NW_Status_t
NW_XML_Parse_PrologConsume(NW_XML_Reader_t* pT,
                           const struct NW_XML_Parser_EventCallbacks_s* pE,
                           RPointerArray <CXML_Internal_Entity_t>*  internalEntityList)
{
    /*
    [22] prolog ::= XMLDecl? Misc* (doctypedecl Misc*)?

    [23] XMLDecl ::= '<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'

    [24] VersionInfo ::= S 'version' Eq ("'" VersionNum "'"
                                         | '"' VersionNum '"')

    [26] VersionNum ::= ([a-zA-Z0-9_.:] | '-')+
    */

    /* '<?' */
    NW_Status_t s;
    NW_Uint32 match;
    NW_PiFormTypeTag_t pi_type;
    NW_Uint32 nameValid;
    NW_XML_Reader_Interval_t I_name;
    NW_Uint32 versionValid;
    NW_XML_Reader_Interval_t I_version;
    NW_Uint32 encodingValid;
    NW_XML_Reader_Interval_t I_encoding;
    NW_Uint32 standaloneValid;
    NW_XML_Reader_Interval_t I_standalone;
    NW_Uint32 contentValid;
    NW_XML_Reader_Interval_t I_content;
    NW_XML_Reader_Interval_t I_keyword;
    NW_Uint32 prevIndex;
    NW_Uint32 prevCharIndex;
    NW_XML_Reader_LineColumn_t prevLineColumn;

    /* Might back up if PiForm is from Misc */
    NW_XML_Reader_GetPosition(pT, &prevIndex, &prevCharIndex, &prevLineColumn);

    s = NW_XML_Parse_KeywordConsume(pT, &I_keyword,
                                    NW_XML_String_PiFormStartLength,
                                    NW_XML_String_PiFormStart, &match);
    if (NW_STAT_IS_FAILURE(s)) {
        return NW_STAT_FAILURE;
    }
    if (match) {
        NW_Uint32 error = 0;
        /* XMLDecl */
        s = NW_XML_Parse_PiFormConsume(pT, &pi_type,
                                       &nameValid,
                                       &I_name,
                                       &versionValid,
                                       &I_version,
                                       &encodingValid,
                                       &I_encoding,
                                       &standaloneValid,
                                       &I_standalone,
                                       &contentValid,
                                       &I_content,
                                       internalEntityList);
        if (NW_STAT_IS_FAILURE(s) || !nameValid) {
            return NW_STAT_FAILURE;
        }
        switch (pi_type) {
        case XMLDECL:
            /* BUG not checking for legal values of version,
            encoding and standalone */
            if (!versionValid) {
                /* version is mandatory */
                error = 1;
            }
            if (contentValid) {
                /* no other attribute info is legal */
                error = 1;
            }
            if (!error && (pE->PiForm_CB != NULL)) {
                s = (*(pE->PiForm_CB))(pT, pi_type,
                                       (nameValid ? &I_name : NULL),
                                       (versionValid ? &I_version : NULL),
                                       (encodingValid ? &I_encoding : NULL),
                                       (standaloneValid ? &I_standalone : NULL),
                                       (contentValid ? &I_content : NULL),
                                       pE->pClientPointer);
            }
            break;
        case PI:
            /* encountered a PI from Misc, rewind and continue */
            NW_XML_Reader_SetPosition(pT, prevIndex, prevCharIndex, &prevLineColumn);
            break;
        default:
            error = 1;
            break;
        }
        if (error) {
            if (pE->Exception_CB != NULL) {
                s = (*(pE->Exception_CB))(pT, pE->pClientPointer);
            }
            return NW_STAT_FAILURE;
        }
    }
    /* MISC */
    s = NW_XML_Parse_MiscConsume(pT, pE,internalEntityList);
    if (NW_STAT_IS_FAILURE(s)) {
        return NW_STAT_FAILURE;
    }
    /* '<!DOCTYPE' */
    s = NW_XML_Parse_KeywordConsume(pT, &I_keyword,
                                    NW_XML_String_DoctypeStartLength,
                                    NW_XML_String_DoctypeStart, &match);
    if (NW_STAT_IS_FAILURE(s)) {
        return NW_STAT_FAILURE;
    }
    if (match) {
        s = NW_XML_Parse_DtdConsume(pT,internalEntityList);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
    }
    /* MISC */
    s = NW_XML_Parse_MiscConsume(pT, pE,internalEntityList);
    if (NW_STAT_IS_FAILURE(s)) {
        return NW_STAT_FAILURE;
    }
    return NW_STAT_SUCCESS;
}

/* Assumes read position is at '<' on entry.
return: *pIsEmptyElement = 1 if it is an EmptyElemTag. */
static
NW_Status_t
NW_XML_Parse_ElementOpeningMarkupConsume(NW_XML_Reader_t* pT,
                                         NW_Uint32* pIsEmptyElement,
                                         const struct
                                         NW_XML_Parser_EventCallbacks_s* pE,
                                         const RPointerArray <CXML_Internal_Entity_t>*  internalEntityList)
{
    /*
    [40] STag ::= '<' Name (S Attribute)* S? '>'

    [41] Attribute ::= Name Eq AttValue

    [44] EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'
    */

    NW_Status_t s;
    NW_XML_Reader_Interval_t I_name;
    NW_XML_Reader_Interval_t I_attName;
    NW_XML_Reader_Interval_t I_attValue;
    NW_Uint32 match;
    NW_Uint32 attributeCount;
	CXML_Bool entity = CXML_TRUE;
	NW_String_t  attrValStr;
	NW_Byte* finalStr = NULL;
	NW_Uint32 finalStrLen = 0;
    NW_String_initialize (&attrValStr, NULL, 0);


    *pIsEmptyElement = 0;
    /* '<' */
    s = NW_XML_Reader_AsciiCharMatch(pT, '<', &match);
    if (NW_STAT_IS_FAILURE(s) || !match) {
        return NW_STAT_FAILURE;
    }
    s = NW_XML_Reader_Advance(pT);
    if (NW_STAT_IS_FAILURE(s)) {
        return NW_STAT_FAILURE;
    }
    /* Name */
    s = NW_XML_Parse_NameConsume(pT, &I_name, &match);
    if (NW_STAT_IS_FAILURE(s) || !match) {
        return NW_STAT_FAILURE;
    }
    if (pE->Tag_Start_CB != NULL) {
        s = (*(pE->Tag_Start_CB))(pT, &I_name, pE->pClientPointer);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
    }
    /* Do attribute value consume and look for closing markup. */
    for (attributeCount = 0;;) {
        entity = CXML_TRUE;
        s = NW_XML_Parse_AttributeValueConsume(pT, &I_attName, &I_attValue,
                                               0, NULL, &match,
											   &entity,&attrValStr,internalEntityList);

        if (NW_STAT_IS_SUCCESS(s) && match && (entity != CXML_TRUE) ) {
            attributeCount++;
            if (pE->Attr_Start_CB != NULL) {
                s = (*(pE->Attr_Start_CB))(pT, &I_attName, pE->pClientPointer);
                if (NW_STAT_IS_SUCCESS(s)) {
                    if (pE->Attr_VarVal_CB != NULL) {
                        s = (*(pE->Attr_VarVal_CB))(pT, &I_attName, &I_attValue,
                                                    pE->pClientPointer);
                    }
                }
                if (NW_STAT_IS_FAILURE(s)) {
                    return NW_STAT_FAILURE;
                }
                if(attrValStr.storage != NULL)
                {
                 NW_Mem_Free(attrValStr.storage);
                }
                NW_String_initialize (&attrValStr, NULL, 0);
            }
            continue;
        }
		else if(NW_STAT_IS_SUCCESS(s) && match && (entity == CXML_TRUE) )
		{
		    attributeCount++;
            if (pE->Attr_Start_CB != NULL) {
                s = (*(pE->Attr_Start_CB))(pT, &I_attName, pE->pClientPointer);
                if (NW_STAT_IS_SUCCESS(s)) {
                    if (pE->Attr_Entity_VarVal_CB != NULL) {
						/* The MSB of NW_Byte length if set represents that the 
							string is from storage buffer, so BufferOwns String. So,
						    find exact length.
						*/
						finalStrLen = attrValStr.length & 0x7fffffff;
                        
                        /* This string is with the NULL terminate character. But,
                         * the callback function Attr_Entity_VarVal_CB(..) requires
                         * with out this. So, take out NULL termination character 
                         * length depending on the Encoding.
                         */

                        if ( (pT->encoding == HTTP_iso_10646_ucs_2) &&
                             (finalStrLen > 1) &&
                             (attrValStr.storage[(finalStrLen-1)] == 0) && 
                             (attrValStr.storage[(finalStrLen-2)] == 0) ) 
                        {
                            finalStrLen--;
                            finalStrLen--;
                         }
                        else if ((pT->encoding == HTTP_utf_8) ||
                                (pT->encoding == HTTP_us_ascii) ||
                                (pT->encoding== HTTP_iso_8859_1)) 
                            {
                                if ( (finalStrLen > 0) && (attrValStr.storage[finalStrLen-1] == 0) )
                                {
                                 finalStrLen--;
                                }
                            }

						finalStr = (NW_Byte*) NW_Mem_Malloc(finalStrLen);
						if(finalStr == NULL)
						{
						 return NW_STAT_OUT_OF_MEMORY;
						}

						CXML_Mem_memcpy(finalStr,attrValStr.storage,finalStrLen);

                        s = (*(pE->Attr_Entity_VarVal_CB))(pT, &I_attName,
							   finalStr,finalStrLen,pE->pClientPointer);
						if(finalStr != NULL)
						{
						 NW_Mem_Free(finalStr);
						 finalStr = NULL;
						}
                    }
                }
                if (NW_STAT_IS_FAILURE(s)) {
                    return NW_STAT_FAILURE;
                }

                if(attrValStr.storage != NULL)
                {
                 NW_Mem_Free(attrValStr.storage);
                }
                NW_String_initialize (&attrValStr, NULL, 0);
            }
            continue;
		 
		}//end else if(NW_STAT_IS_SUCCESS(s) && match && (entity != CXML_TRUE) )
        if (NW_STAT_IS_FAILURE(s) && match) {
            return NW_STAT_FAILURE;
        }
        s = NW_XML_Reader_AsciiCharMatch(pT, '>', &match);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
        if (match) {
            s = NW_XML_Reader_Advance(pT);
            if (NW_STAT_IS_FAILURE(s)) {
                return NW_STAT_FAILURE;
            }
            if (pE->Attributes_End_CB != NULL) {
                s = (*(pE->Attributes_End_CB))(pT, attributeCount,
                                               pE->pClientPointer);
                if (NW_STAT_IS_FAILURE(s)) {
                    return NW_STAT_FAILURE;
                }
            }
            break;
        }
        s = NW_XML_Reader_AsciiStringMatch(pT, NW_XML_String_EmptyTagEndLength,
                                        NW_XML_String_EmptyTagEnd, &match);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
        if (match) {
            s = NW_XML_Reader_AdvanceOffset(pT, NW_XML_String_EmptyTagEndLength);
            if (NW_STAT_IS_FAILURE(s)) {
                return NW_STAT_FAILURE;
            }
            if (pE->Attributes_End_CB != NULL) {
                s = (*(pE->Attributes_End_CB))(pT, attributeCount,
                                               pE->pClientPointer);
                if (NW_STAT_IS_FAILURE(s)) {
                    return NW_STAT_FAILURE;
                }
            }
            *pIsEmptyElement = 1;
            if (pE->Tag_End_CB != NULL) {
                s = (*(pE->Tag_End_CB))(pT, &I_name,
                                        1, /* empty tag*/
                                        pE->pClientPointer);
                if (NW_STAT_IS_FAILURE(s)) {
                    return NW_STAT_FAILURE;
                }
            }
            break;
        }
        return NW_STAT_FAILURE;
    }
    return NW_STAT_SUCCESS;
}

/* Assumes position is at '<' on entry. */
static
NW_Status_t
NW_XML_Parse_ElementConsume(NW_XML_Reader_t* pT,
                            const struct NW_XML_Parser_EventCallbacks_s* pE,
                            const RPointerArray <CXML_Internal_Entity_t>*  internalEntityList)
{
    /*
    [39] element ::= EmptyElemTag | STag content ETag

    [40] STag ::= '<' Name (S Attribute)* S? '>'

    [41] Attribute ::= Name Eq AttValue

    [42] ETag ::= '</' Name S? '>'

    [43] content ::= CharData?
                     ((element | Reference | CDSect | PI | Comment)
                     CharData?)*

    [44] EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'
    */
    NW_Status_t s;
    NW_Uint32 match;
    NW_Uint32 isEmptyElement;
    NW_Uint32 nestingCount;
    NW_Uint32 inContent;
    NW_Uint32 prevIndex;
	NW_Uint32 prevCharIndex;
    CXML_Uint8* intEntityValStr = NULL;
    NW_Uint32 contentLen = 0;
	NW_XML_Reader_LineColumn_t prevLineColumn;
	NW_Bool entityFoundLevel_2 =  NW_FALSE;
    NW_XML_Reader_Interval_t I_elementContent;
    NW_XML_Reader_Interval_Init(&I_elementContent);	


    s = NW_XML_Reader_SkipSpace(pT);
    if (NW_STAT_IS_FAILURE(s)) 
    {
        return NW_STAT_FAILURE;
    }

    /* Look for initial element markup. */
    s = NW_XML_Parse_ElementOpeningMarkupConsume(pT, &isEmptyElement, pE, internalEntityList);
    if (NW_STAT_IS_FAILURE(s)) {
        return NW_STAT_FAILURE;
    }
    if (isEmptyElement) {
        return NW_STAT_SUCCESS;
    }
    inContent = 0;
    for (nestingCount = 1; nestingCount;) {
        /* test for ETag */
        s = NW_XML_Reader_AsciiStringMatch(pT, NW_XML_String_EndTagStartLength,
                                        NW_XML_String_EndTagStart, &match);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
        if (match) 
        {
            NW_XML_Reader_Interval_t I_name;
            if (inContent) 
            {
                inContent = 0;
                NW_XML_Reader_Interval_Stop(&I_elementContent, pT);
                
                /*Write the contents only if I_elementContent have some contents*/
            
                if(NW_XML_Reader_Interval_IsWellFormed(&I_elementContent) )
                {
                  if (pE->Content_CB != NULL) 
                  {
                    s = (*(pE->Content_CB))(pT, &I_elementContent, pE->pClientPointer);
                    if (NW_STAT_IS_FAILURE(s)) {
                        return NW_STAT_FAILURE;
                    }
                  }/*end if (pE->Content_CB != NULL)*/
                }
            }/*end if(inContent)*/

            s = NW_XML_Reader_AdvanceOffset(pT, NW_XML_String_EndTagStartLength);
            if (NW_STAT_IS_FAILURE(s)) {
                return NW_STAT_FAILURE;
            }
            /* Name */
            s = NW_XML_Parse_NameConsume(pT, &I_name, &match);
            if (NW_STAT_IS_FAILURE(s) || !match) {
                return NW_STAT_FAILURE;
            }
            /* S */
            s = NW_XML_Reader_SkipSpace(pT);
            if (NW_STAT_IS_FAILURE(s)) {
                return NW_STAT_FAILURE;
            }
            /* '>' */
            s = NW_XML_Reader_AsciiCharMatch(pT, '>', &match);
            if (NW_STAT_IS_FAILURE(s) || !match) {
                return NW_STAT_FAILURE;
            }
            s = NW_XML_Reader_Advance(pT);
            if (NW_STAT_IS_FAILURE(s)) {
                return NW_STAT_FAILURE;
            }
            /* FUTURE could check that Etag name matches STag name */
            nestingCount--;
            if (pE->Tag_End_CB != NULL) {
                s = (*(pE->Tag_End_CB))(pT, &I_name,
                                        0, /* not empty tag */
                                        pE->pClientPointer);
                if (NW_STAT_IS_FAILURE(s)) {
                    return NW_STAT_FAILURE;
                }
                /*Just now element (tag) is consumned so contenst will start
                 *after this. So, init the content pointer.
                 */

                NW_XML_Reader_Interval_Start(&I_elementContent, pT);
            }
            continue;
        } /*end if(match) */


        /* test for PI */
        s = NW_XML_Reader_AsciiStringMatch(pT, NW_XML_String_PiFormStartLength,
                                        NW_XML_String_PiFormStart, &match);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
        if (match) {
            NW_PiFormTypeTag_t pi_type;
            NW_XML_Reader_Interval_t I_name, I_version, I_encoding, I_standalone;
            NW_XML_Reader_Interval_t I_content;
            NW_Uint32 nameValid, versionValid, encodingValid, standaloneValid;
            NW_Uint32 contentValid;
            if (inContent) {
                inContent = 0;
                NW_XML_Reader_Interval_Stop(&I_elementContent, pT);
                if (pE->Content_CB != NULL) {
                    s = (*(pE->Content_CB))(pT, &I_elementContent, pE->pClientPointer);
                    if (NW_STAT_IS_FAILURE(s)) {
                        return NW_STAT_FAILURE;
                    }
                }
            }
            s = NW_XML_Reader_AdvanceOffset(pT, NW_XML_String_PiFormStartLength);
            if (NW_STAT_IS_FAILURE(s)) {
                return NW_STAT_FAILURE;
            }
            s = NW_XML_Parse_PiFormConsume(pT, &pi_type,
                                           &nameValid,
                                           &I_name,
                                           &versionValid,
                                           &I_version,
                                           &encodingValid,
                                           &I_encoding,
                                           &standaloneValid,
                                           &I_standalone,
                                           &contentValid,
                                           &I_content,
                                           internalEntityList);
            if (NW_STAT_IS_FAILURE(s) || !nameValid) {
                return NW_STAT_FAILURE;
            }
            if (pi_type == PI && (pE->PiForm_CB != NULL)) {
                s = (*(pE->PiForm_CB))(pT, pi_type,
                                       (nameValid ? &I_name : NULL),
                                       (versionValid ? &I_version : NULL),
                                       (encodingValid ? &I_encoding : NULL),
                                       (standaloneValid ? &I_standalone : NULL),
                                       (contentValid ? &I_content : NULL),
                                       pE->pClientPointer);
                if (NW_STAT_IS_FAILURE(s)) {
                    return NW_STAT_FAILURE;
                }
            }
            if (pi_type != PI) {
                /* BUG is this correct? */
                return NW_STAT_FAILURE;
            }
            continue;
        }
        /* test for Comment */
        s = NW_XML_Reader_AsciiStringMatch(pT, NW_XML_String_CommentStartLength,
                                        NW_XML_String_CommentStart, &match);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
        if (match) {
            NW_XML_Reader_Interval_t I_comment;
            if (inContent) {
                inContent = 0;
                NW_XML_Reader_Interval_Stop(&I_elementContent, pT);
                if (pE->Content_CB != NULL) {
                    s = (*(pE->Content_CB))(pT, &I_elementContent, pE->pClientPointer);
                    if (NW_STAT_IS_FAILURE(s)) {
                        return NW_STAT_FAILURE;
                    }
                }
            }
            s = NW_XML_Reader_AdvanceOffset(pT, NW_XML_String_CommentStartLength);
            if (NW_STAT_IS_FAILURE(s)) {
                return NW_STAT_FAILURE;
            }
            s = NW_XML_Parse_CommentConsume(pT, &I_comment, pE);
            if (NW_STAT_IS_FAILURE(s)) {
                return NW_STAT_FAILURE;
            }
            continue;
        }
        /* test for CDSect */
        s = NW_XML_Reader_AsciiStringMatch(pT, NW_XML_String_CdataStartLength,
                                        NW_XML_String_CdataStart, &match);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
        if (match) {
            NW_XML_Reader_Interval_t I_cdata;
            if (inContent) {
                inContent = 0;
                NW_XML_Reader_Interval_Stop(&I_elementContent, pT);
                if (pE->Content_CB != NULL) {
                    s = (*(pE->Content_CB))(pT, &I_elementContent, pE->pClientPointer);
                    if (NW_STAT_IS_FAILURE(s)) {
                        return NW_STAT_FAILURE;
                    }
                }
            }
            s = NW_XML_Reader_AdvanceOffset(pT, NW_XML_String_CdataStartLength);
            if (NW_STAT_IS_FAILURE(s)) {
                return NW_STAT_FAILURE;
            }
            NW_XML_Reader_Interval_Start(&I_cdata, pT);
            for (;;) {
                s = NW_XML_Reader_AsciiStringMatch(pT,
                                                NW_XML_String_CdataEndLength,
                                                NW_XML_String_CdataEnd,
                                                &match);
                if (NW_STAT_IS_FAILURE(s)) {
                    NW_XML_Reader_Interval_Stop(&I_cdata, pT);
                    return NW_STAT_FAILURE;
                }
                if (match) {
                    NW_XML_Reader_Interval_Stop(&I_cdata, pT);
                    s = NW_XML_Reader_AdvanceOffset(pT,
                                                 NW_XML_String_CdataEndLength);
                    if (NW_STAT_IS_FAILURE(s)) {
                        return NW_STAT_FAILURE;
                    }
                    /* BUG Is this spec compliant parsing of CDATA? */
                    if (pE->Cdata_CB != NULL) {
                        s = (*(pE->Cdata_CB))(pT, &I_cdata, pE->pClientPointer);
                        if (NW_STAT_IS_FAILURE(s)) {
                            return NW_STAT_FAILURE;
                        }
                    }
                    break;
                }
                s = NW_XML_Reader_Advance(pT);
                if (NW_STAT_IS_FAILURE(s)) {
                    NW_XML_Reader_Interval_Stop(&I_cdata, pT);
                    return NW_STAT_FAILURE;
                }
            }
            continue;
        }
        /* test for illegal '<' or nested element */
        s = NW_XML_Reader_AsciiCharMatch(pT, '<', &match);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
        if (match) {
            if (inContent) {
                inContent = 0;
                NW_XML_Reader_Interval_Stop(&I_elementContent, pT);

                if (pE->Content_CB != NULL) 
                {
                    contentLen = I_elementContent.stop - I_elementContent.start;
                    if(contentLen > 0)
                    {
                     s = (*(pE->Content_CB))(pT, &I_elementContent, pE->pClientPointer);
                     if (NW_STAT_IS_FAILURE(s)) 
                     {
                        return NW_STAT_FAILURE;
                     }
                    }//end if(contentLen > 0)
                }// end if(pE->Content_CB != NULL)
            }
            s = NW_XML_Parse_ElementOpeningMarkupConsume(pT, &isEmptyElement,
                                                         pE, internalEntityList);
            if (NW_STAT_IS_SUCCESS(s)) {
                if (!isEmptyElement) {
                    nestingCount++;
                }

                /*Just now element (tag) is consumned so contenst will start
                 *after this. So, init the content pointer.
                 */

                NW_XML_Reader_Interval_Start(&I_elementContent, pT);
                 continue;
            }
            return NW_STAT_FAILURE;
        }


       /* Test for predefined entities and character entries*/

		s = NW_XML_Reader_AsciiCharMatch(pT, '&', &match); 

		if (NW_STAT_IS_FAILURE(s))
		{
            return NW_STAT_FAILURE;
        }


        if (match) 
		{
         NW_XML_Reader_Interval_t I_entityData;
		 NW_Bool entityFoundLevel_1 =  NW_FALSE; //If end of entity (;) found
		 NW_Uint32 entityVal = 0;
		 NW_XML_Reader_Interval_t* I_content = &I_elementContent;

         //Will back off if not a valid entity

			  NW_XML_Reader_GetPosition(pT,
                              &prevIndex, &prevCharIndex, &prevLineColumn);

         /* If it is here then this must in the contents. The entity can appear
          * at the beginning of content.
          */

         inContent = NW_TRUE;
         entityFoundLevel_2 =  NW_FALSE;
         NW_XML_Reader_Interval_Stop(I_content, pT); //Contents before entity

		 s = CXML_XML_Parser_Entity(pT,&I_entityData,&entityFoundLevel_1);

         if (NW_STAT_IS_FAILURE(s))
		 {
            return NW_STAT_FAILURE;
         }

		 if (entityFoundLevel_1 && inContent) 
		 {
			 //Validate the entity
            
             //The following function checks for the character,
             //predefined entities and Internal Entities.

            contentLen = I_entityData.stop - I_entityData.start;

            if(contentLen > 0)
            {
			s = CXML_XML_Handle_entity(pT,
                                       &I_entityData,&entityVal,
                                       &intEntityValStr,&entityFoundLevel_2,
                                       (void*) internalEntityList);
            }
            else
            {
             //Not a valid entity e.g. "&&;" test case
             entityFoundLevel_2 = NW_FALSE;
            }

			if (NW_STAT_IS_FAILURE(s)) 
			{
			 return NW_STAT_FAILURE;
			}

            if(entityFoundLevel_2 == CXML_TRUE)
			{
                inContent = NW_FALSE;

				//Now write content before entity first


                if (pE->Content_CB != NULL) {
                    contentLen = I_content->stop - I_content->start;
                    if(contentLen > 0)
                    {
                     s = (*(pE->Content_CB))(pT, I_content, pE->pClientPointer);
                     if (NW_STAT_IS_FAILURE(s)) {
                         return NW_STAT_FAILURE;
                     }
                    }
                }
			
			    // Write the entity content now. There are two possibilities for
                // entities.
                //
                // 1) If it is character or decimal or predefined entities. 
                //    In this case, intEntityValStr = NULL.
                //
                // 2) If it is "Internal Entity" then intEntityValStr != NULL.
                //     So, this is directly written to WBXML encoder as inline string.
                //    In this case, entityVal = 0;


             if (pE->Entity_CB != NULL )
				{
                    s = (*(pE->Entity_CB))(pT, entityVal, pE->pClientPointer,intEntityValStr);
                    
  if(intEntityValStr != NULL)
                    {
                     NW_Mem_Free(intEntityValStr);
                    }
if (NW_STAT_IS_FAILURE(s)) 
                    {
                        return NW_STAT_FAILURE;
                    }
                  
                }/*end if(pE->Entity_CB != NULL) */
			}//endif(entityFoundLevel_2 == CXML_TRUE)
            else
            {
                //No valid entity found. Parse as the normal string
                 NW_XML_Reader_SetPosition(pT,
                                        prevIndex,
                                        prevCharIndex,
                                        &prevLineColumn);
            }
         } //end if (entityFoundLevel_1 && inContent)
         else
         {
             //No valid entity found. Parse as the normal string

                 NW_XML_Reader_SetPosition(pT,
                                        prevIndex,
                                        prevCharIndex,
                                        &prevLineColumn);
         }
		} //end match


        /* BUG no reference parsing and also permits illegal char data */
        if (!inContent) 
		{
            NW_XML_Reader_Interval_Start(&I_elementContent, pT);
            inContent = 1;
			if(entityFoundLevel_2 == NW_TRUE)
			{
			 //Don't advance if it is entity as we did already
			 entityFoundLevel_2 = NW_FALSE;
			 continue;
			}
        }

        s = NW_XML_Reader_Advance(pT);
        if (NW_STAT_IS_FAILURE(s)) {
            return NW_STAT_FAILURE;
        }
    }/*end for(..) */
    return NW_STAT_SUCCESS;
}

EXPORT_C NW_Status_t
NW_XML_Parse(NW_XML_Reader_t* pT, const struct NW_XML_Parser_EventCallbacks_s* pE)
{
    /*
    [1] document ::= prolog element Misc*
    */
    NW_Status_t s = NW_STAT_SUCCESS;
    RPointerArray <CXML_Internal_Entity_t>  internalEntityList;

    if (pE->StartDocument_CB != NULL) {
        s = (*(pE->StartDocument_CB))(pT, pE->pClientPointer);
        if (NW_STAT_IS_FAILURE(s)) {
            s =  NW_STAT_FAILURE;
            goto nw_xml_parser_final;
        }
    }

    s = NW_XML_Parse_PrologConsume(pT, pE,&internalEntityList);
    if (NW_STAT_IS_FAILURE(s)) {
        s = NW_STAT_FAILURE;
        goto nw_xml_parser_final;
    }

    s = NW_XML_Parse_ElementConsume(pT, pE, &internalEntityList);
    if (NW_STAT_IS_FAILURE(s)) {
        s =  NW_STAT_FAILURE;
        goto nw_xml_parser_final;
    }

    s = NW_XML_Parse_MiscConsume(pT, pE, &internalEntityList);
    if (NW_STAT_IS_FAILURE(s) && !NW_XML_Reader_AtEnd(pT)) {
        s = NW_STAT_FAILURE;
        goto nw_xml_parser_final;
    }

    if (!NW_XML_Reader_AtEnd(pT)) {
        s = NW_STAT_FAILURE;
        goto nw_xml_parser_final;
    }

    if (pE->EndDocument_CB != NULL) {
        s = (*(pE->EndDocument_CB))(pT, pE->pClientPointer);
        if (NW_STAT_IS_FAILURE(s)) {
            goto nw_xml_parser_final;
        }
    }

nw_xml_parser_final:

    CXML_XML_Parser_Free_I_Entity_List(internalEntityList);
    return s;
}

