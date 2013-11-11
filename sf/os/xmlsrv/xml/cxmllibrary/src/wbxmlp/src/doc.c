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


/*****************************************************************
**  File: wbxml_doc.c
**  Description:
*****************************************************************/

#include "cxml_internal.h"
#include <xml/cxml/nw_wbxml_document.h>
#include <xml/cxml/nw_string_char.h>
#include <xml/cxml/nw_encoder_stringtable.h>


EXPORT_C NW_Status_t
NW_WBXML_Document_construct(NW_WBXML_Document_t * doc, NW_Uint32 default_public_id)
{
  doc->version = 0;
  doc->publicid = 0;
  doc->doc_type = NULL;
  doc->default_public_id = default_public_id;
  doc->charset = 0;
  doc->strtbl.length = 0;
  doc->strtbl.data = 0;
  doc->body_len = 0;
  doc->strtbl_extension = NW_Encoder_StringTable_new();
  if (doc->strtbl_extension == NULL){
    return NW_STAT_OUT_OF_MEMORY;
  }
  return NW_STAT_SUCCESS;
}

/* There is no document construct, since initialization is done by
 * the parser
 */

EXPORT_C void
NW_WBXML_Document_destruct (NW_WBXML_Document_t * doc)
{
  if (doc == NULL) {
    return;
  }

  if (doc->doc_type) {
    NW_String_delete (doc->doc_type);
    doc->doc_type = NULL;
  }

  NW_Encoder_StringTable_delete(doc->strtbl_extension);
}

/*
 * Safely get a string from the string table
 *
 * RETURN NULL if an error occurs and sets parser->flags accordingly
 */


EXPORT_C NW_Status_t
NW_WBXML_Document_getTableString (NW_WBXML_Document_t * doc,
                                  NW_Uint32 index,
                                  NW_String_t *string)
{

  NW_ASSERT(doc != NULL);
  NW_ASSERT(string != NULL);

  if (index >= doc->strtbl.length){
    /* See if there are extended table members */
    if(NW_Encoder_StringTable_GetSize(doc->strtbl_extension) != 0)
    {
      NW_Status_t status;
      NW_Encoder_StringTableIterator_t extTblIter;
      NW_Uint32 strTblItrByteCnt = 0;
      NW_Uint32 startingExtTblByteCnt = 0;
      NW_Uint8* indexBuf;
      NW_Uint32 extensionTblInx = index - doc->strtbl.length;
      NW_Bool found = NW_FALSE;

      /* NW_Encoder_StringTable_get(..) requires the vector item
       * index not the absolute extension string table index (as 
       * passed parameter [NW_Uint32 index]). So, itertor method 
       * is used to get the string.
       *
       */
      
      /*The following function always return success so why bother to check? */

      NW_Encoder_StringTable_StringTableIterateInit(doc->strtbl_extension,&extTblIter);

      status =  NW_Encoder_StringTable_StringTableIterateNext
                    (&extTblIter,&strTblItrByteCnt,&indexBuf);


        while(status == NW_STAT_WBXML_ITERATE_MORE)
        {

         if(startingExtTblByteCnt == extensionTblInx)
         {
          found = NW_TRUE;
          break;
         }
          
          startingExtTblByteCnt += strTblItrByteCnt;

          status =  NW_Encoder_StringTable_StringTableIterateNext
                        (&extTblIter,&strTblItrByteCnt,&indexBuf) ;
        }//end while

     
      if(found == NW_TRUE)
      {
        string->storage = indexBuf;
        string->length = strTblItrByteCnt;
        return NW_STAT_SUCCESS;
      }
    }
    return NW_STAT_WBXML_ERROR_BYTECODE;
  }

  /* Make sure string ends within string table */

  if (!NW_String_valid(doc->strtbl.data + index, doc->strtbl.length - index,
                       doc->charset))
  {
    return NW_STAT_WBXML_ERROR_BYTECODE;
  }

  return  NW_String_initialize (string, doc->strtbl.data + index, doc->charset);
}

/* String table write method
 *
 * Writing a dom tree may require adding strings to the string table,
 * if one exists, or creating a string table if none exists. We create
 * an extended table using the encoder string table write methods if
 * this is necessary. Unfortunately, there can be some duplication of
 * strings between the two tables, because there is no efficient way to
 * look up a string by name in the real string table. Therefore, any
 * string that gets added to the table gets added to the extension
 * table.  The encoder takes care of rationalizing the two tables if
 * the document gets rewritten as wbxml.
 *
 * If the string is not already in the table, this method adds it. The
 * out parameter byteOffset returns the byte offset of the string in the
 * extension table plus the size of the real string table so that
 * extension table strings always have positions higher than real table
 * strings.
 */
EXPORT_C NW_Status_t
NW_WBXML_Document_putTableString(NW_WBXML_Document_t * doc,
                                 NW_String_t *string,
                                 NW_Uint32 *byteOffset)
{
  NW_Uint32 extensionOffset;
  NW_Status_t retval =
    NW_Encoder_StringTable_append(doc->strtbl_extension,
                                 string,
                                 NW_TRUE,
                                 &extensionOffset);
  *byteOffset = doc->strtbl.length + extensionOffset;
  return retval;
}

NW_Uint8
NW_WBXML_Document_getVersion(NW_WBXML_Document_t *document)
{
  NW_ASSERT(document != NULL);
  return document->version;
}

NW_Uint32
NW_WBXML_Document_getPublicID(NW_WBXML_Document_t *document)
{
  NW_ASSERT(document != NULL);
  return document->publicid;
}

NW_String_t*
NW_WBXML_Document_getDocType(NW_WBXML_Document_t *document)
{
  NW_ASSERT(document != NULL);
  return document->doc_type;
}

NW_Uint32
NW_WBXML_Document_getEncoding(NW_WBXML_Document_t *document)
{
  NW_ASSERT(document != NULL);
  return document->charset;
}







