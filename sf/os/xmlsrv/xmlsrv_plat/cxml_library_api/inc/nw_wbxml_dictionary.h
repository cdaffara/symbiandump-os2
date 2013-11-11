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


/** ----------------------------------------------------------------------- **
    @package:     NW_WBXML

    @synopsis:    default

    @description: Wbxml token definitions

 ** ----------------------------------------------------------------------- **/

#ifndef NW_PARSER_WBXML_DICTIONARY_H
#define NW_PARSER_WBXML_DICTIONARY_H

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_string_string.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*Different feature flags for cXML Library internal use only. */

#define CXML_DTD_SUPPORT_ON  0x0000001

/** ----------------------------------------------------------------------- **
    @struct:      NW_WBXML_DictEntry

    @synopsis:    Entry

    @scope:       public
    @variables:
       NW_Byte token
                  The token.

       NW_String_UCS2Buff_t* name
                  The name.

    @description: Generic dictionary entry.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_WBXML_DictEntry_s{
  NW_Byte token;
  NW_String_UCS2Buff_t* name;
}NW_WBXML_DictEntry_t;


/** ----------------------------------------------------------------------- **
    @struct:      NW_WBXML_Codepage

    @synopsis:    Codepage

    @scope:       public
    @variables:
       NW_Byte count
                  The number of tokens.

       NW_WBXML_DictEntry_t* tokens
                  Ordered by token.

       NW_Byte* names
                  Ordered by name, stored as offsets into the token list.

    @description:  Codepages should be small and have efficient access
                  both by token and name. They are currently stored in
                  packed lists of token-string pairs.  While this
                  requires one more NW_Byte per entry than a sparse
                  array, they are still guaranteed to be smaller than
                  sparse arrays for tables with less.  than ~200
                  entries. Since the biggest table we know about (wbxml
                  attributes) has only 109 entries this seems like a
                  reasonable tradeoff.

 ** ----------------------------------------------------------------------- **/
typedef struct NW_WBXML_Codepage_s{
  NW_Byte count;
  NW_WBXML_DictEntry_t* tokens;   
  NW_Byte* names;
}NW_WBXML_Codepage_t;


/** ----------------------------------------------------------------------- **
    @struct:      NW_WBXML_Dictionary

    @synopsis:    Dictionary

    @scope:       public
    @variables:
       NW_Uint32 public_id
                  The public ID.

       NW_Ucs2* doc_type
                  The document type.

       NW_Int32 tag_page_count
                  Number of tag pages.

       NW_WBXML_Codepage_t* tag_pages
                  Array of pages.

       NW_Int32 attr_page_count
                  Number of attribute pages.

       NW_WBXML_Codepage_t* attr_pages
                  Array of pages.

    @description: This structure stores the basic definition of a dictionary.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_WBXML_Dictionary_s {
  /* Dictionary identifiers */
  NW_Uint32 public_id;
  NW_Ucs2* doc_type;
  /* Tag code pages */
  NW_Int32 tag_page_count;
  NW_WBXML_Codepage_t* tag_pages;
  /* Attribute code pages */
  NW_Int32 attr_page_count;
  NW_WBXML_Codepage_t* attr_pages;
}NW_WBXML_Dictionary_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_initialize

    @synopsis:    Initialize dictionary.

    @scope:       public

    @parameters:
       [in] NW_Int32 n
                  Number of dictionaries.

       [in] NW_WBXML_Dictionary_t* d[]
                  The array of dictionaries.

    @description: Dictionary initialization. 

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Dictionaries added to active list

       [NW_STAT_FAILURE]
                  List already full.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_WBXML_Dictionary_initialize(NW_Int32 n, NW_WBXML_Dictionary_t* d[]);

/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_add

    @synopsis:    add dictionary(s).

    @scope:       public

    @parameters:
       [in] NW_Int32 n
                  Number of dictionaries.

       [in] NW_WBXML_Dictionary_t* d[]
                  The array of dictionaries.

    @description: add an array of dictionaries. 

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Dictionaries added to active list

       [NW_STAT_FAILURE]
                  List already full.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_WBXML_Dictionary_add(NW_Int32 n, NW_WBXML_Dictionary_t* d[]);

/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_destroy

    @synopsis:    Destroy dictionary.

    @scope:       public

    @parameters:
       [in] void
                

       [in-out] 
                

    @description: Dictionary destruction.

    @returns:     NW_Status_t
                  default

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_WBXML_Dictionary_destroy ();

/* ----------------------------------------------------------------------- **
   Dictionary lookup
** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getIndexByPublicId

    @synopsis:    Get dictionary index using publicID.

    @scope:       public

    @parameters:
       [in] NW_Uint32 public_id
                  The publid ID to search for.

    @description: Get dictionary index using publicID.

    @returns:     NW_Uint32
                  1 - based index or 0 if not found.

 ** ----------------------------------------------------------------------- **/
NW_Uint32
NW_WBXML_Dictionary_getIndexByPublicId (NW_Uint32 public_id);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getIndexByDocType

    @synopsis:    Get dictionary index using docType.

    @scope:       public

    @parameters:
       [out] NW_String_t* doc_type
                  Returned document type.

       [in] NW_Uint32 encoding
                  Requested encoding.

    @description: Get dictionary index using docType.

    @returns:     NW_Uint32
                  1 based index if found, otherwise 0.

 ** ----------------------------------------------------------------------- **/
NW_Uint32 
NW_WBXML_Dictionary_getIndexByDocType(NW_String_t* doc_type, NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getByPublicId

    @synopsis:    Get dictionary using publicId.

    @scope:       public

    @parameters:
       [in] NW_Uint32 publicId
                  Public ID.

    @description: Look up a dictionary using the document public ID. The 
                  public ID can be gotten from the document header 
                  (NMXB_Wbxml_Document_t).

                  Documents contain either a public ID or a document type 
                  string. Use this function for documents containing a public ID.

    @returns:     NW_WBXML_Dictionary_t*
                  Pointer to dictionary or NULL if not found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_WBXML_Dictionary_t* 
NW_WBXML_Dictionary_getByPublicId (NW_Uint32 publicId);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getByDocType

    @synopsis:    Get dictionary using docType.

    @scope:       public

    @parameters:
       [in] NW_String_t* docType
                  A UCS-2 encoded string representing the document type.

       [in] NW_Uint32 encoding
                  The encoding.

    @description: Look up a dictionary using the document type
                  string. The document type can be gotten from the
                  document header (NW_Wbxml_Document_t). The string
                  contained in the header will be encoded in the
                  document encoding. Note that if the encoding is not
                  UCS-2, this string must be converted to a UCS-2 string
                  before being passed to this function.  Documents
                  contain either a public ID or a document type
                  string. Use this functions for documents containing a
                  document type string

    @returns:     NW_WBXML_Dictionary_t* 
                  Pointer to dictionary or NULL if not found.

 ** ----------------------------------------------------------------------- **/
NW_WBXML_Dictionary_t* 
NW_WBXML_Dictionary_getByDocType (NW_String_t* docType, NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getByIndex

    @synopsis:    Get dictionary using index.

    @scope:       public

    @parameters:
       [in] NW_Uint32 dictIndex
                  The index.

    @description: Get dictionary using index.

    @returns:     NW_WBXML_Dictionary_t*
                  Pointer to dictionary or NULL if not found.

 ** ----------------------------------------------------------------------- **/
NW_WBXML_Dictionary_t* 
NW_WBXML_Dictionary_getByIndex(NW_Uint32 dictIndex);


/* ----------------------------------------------------------------------- **
   Lookups by token
** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getTagByFqToken

    @synopsis:    Get tag using fully qualified token.

    @scope:       public

    @parameters:
       [in] NW_Uint32 fq_token
                  The fully qualified token.

    @description: Gets the string represented by a token. The fq_token
                  parameter is a token that was returned by the parser
                  via the FQToken callback. Such token values are "fully
                  qualified" since they contain not only the token value
                  itself, but also information that specifies the
                  dictionary, the code page, and whether the token is
                  part of the attribute or token code space. Fully
                  qualified token values should be treated as opaque
                  data. Get tag using fully qualified token.

    @returns:     NW_String_UCS2Buff_t*
                  Tag name or NULL if not found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_String_UCS2Buff_t*
NW_WBXML_Dictionary_getTagByFqToken (NW_Uint32 fq_token);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getElementNameByToken

    @synopsis:    Get element name using token.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Dictionary_t* dictionary
                  The dictionary to search.

       [in] NW_Uint16 token
                  The token.

    @description: Get element name using token.

    @returns:     NW_String_UCS2Buff_t*
                  Element name or NULL if not found.

 ** ----------------------------------------------------------------------- **/
NW_String_UCS2Buff_t*
NW_WBXML_Dictionary_getElementNameByToken (NW_WBXML_Dictionary_t* dictionary,
                                           NW_Uint16 token);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getAttributeNameByToken

    @synopsis:    Get attribute name using token.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Dictionary_t* dictionary
                  The dictionary.

       [in] NW_Uint16 token
                  The token.

    @description: Get attribute name using token.

    @returns:     NW_String_UCS2Buff_t*
                  Attribute name or NULL if not found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_String_UCS2Buff_t*
NW_WBXML_Dictionary_getAttributeNameByToken (NW_WBXML_Dictionary_t* dictionary,
                                             NW_Uint16 token);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_resolveLiteralToken

    @synopsis:    Returns literal token.

    @scope:       public

    @parameters:
       [in-out] NW_Uint32* token
                  The token.

       [in] NW_String_t* name
                  Name to lookup.

       [in] NW_Bool is_tag
                  NW_TRUE if tag token otherwise NW_FALSE if attribute .

       [in] NW_Uint32 encoding
                  The encoding of this string.

       [in] NW_Bool matchCase
                  If NW_TRUE do case sensitive compare.

    @description: Given a tag or attribute token, if the token is a literal,
                  use the given name to lookup the tag/attribute's "real" 
                  token and return that token.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Token resolved.

       [NW_STAT_FAILURE]
                  Token lookup fails (NOT fatal)
         
       [NW_STAT_OUT_OF_MEMORY]
                  Out of memory.
 
 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_WBXML_Dictionary_resolveLiteralToken(NW_Uint32* token, 
                                        NW_String_t* name,
                                        NW_Bool is_tag, 
                                        NW_Uint32 encoding,
                                        NW_Bool matchCase);

/* ----------------------------------------------------------------------- **
   Lookups by name  
** ----------------------------------------------------------------------- **/


#define NW_WBXML_MASK_CODEPAGE       0x0000FF00
#define NW_WBXML_MASK_TOKEN          0x000000FF
/* mask off the page and tag leaving content and attribute bits out */
#define NW_WBXML_MASK_FQTOKEN_TAG    0x0000FF3F

#define NW_WBXML_Dictionary_extractPage(x) \
    ((NW_Uint8)(((x) & NW_WBXML_MASK_CODEPAGE) >> 8))

#define NW_WBXML_Dictionary_extractToken(x) \
    ((NW_Uint8)((x) & NW_WBXML_MASK_TOKEN))


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getAttributeToken

    @synopsis:    Get attribute token using name.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Dictionary_t* dictionary
                  The dictionary.

       [in] const NW_String_t* name
                  The attribute name.

       [in] NW_Uint32 encoding
                  The encoding of the name.

       [in] NW_Bool matchCase
                  Case sensitive comparison if NW_TRUE.

    @description: Look up an attribute token in a dictionary. The lower
                  two bytes of the returned value represent the code
                  page and token. The token value can be calculated as
                  value&0xff ; the code page can be calculated as
                  (value>>8)&0xff.)

                  A fully qualified token constructed from the return
                  value violates the goal that fully qualified tokens be
                  opaque. Attribute tokens can either be attribute start
                  tokens or attribute value tokens. Attribute start
                  tokens may contain both a name and a value or a name
                  alone. To look up an attribute start token containing
                  a name and value, the string argument should be of the
                  form "name=value". Since attributes may be encoded as
                  either a single attribute start token, or an attribute
                  start token and one or more attribute value tokens,
                  dictionaries may contain multiple entries for a
                  particular attribute name. The caller may need to do
                  several lookups to convert a particular attribute
                  string into a set of tokens, especially if the caller
                  wants to find the smallest set of tokens to represent
                  a particular string.

                  These return the lower 16 bits of the fully qualified
                  token i.e., the token and code page. The rest of the
                  token can be constructed by the caller if needed. We
                  don't use all 32 bits in order to be able to return a
                  signed quantity to indicate failure.

    @returns:     NW_Int16
                  Either the token or -1 if the token is not found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Int16 
NW_WBXML_Dictionary_getAttributeToken (NW_WBXML_Dictionary_t* dictionary,
                                       const NW_String_t* name,
                                       NW_Uint32 encoding,
                                       NW_Bool matchCase);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getTagToken

    @synopsis:    Get tag token using name.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Dictionary_t* dictionary
                  The dictionary to look in.

       [in] NW_String_UCS2Buff_t* name
                  The name to search for.

       [in] NW_Bool matchCase
                  Case sensitive match if NW_TRUE.

    @description: Look up a tag token in a dictionary. The lower two
                  bytes of the returned value represent the code page
                  and token. The token value can be calculated as
                  value&0xff and the code page can be calculated as
                  (value>>8)&0xff.

                  A fully qualified token is constructed from the return
                  value. This violates the goal that fully qualified
                  tokens be opaque.

                  These return the lower 16 bits of the fully qualified
                  token i.e., the token and code page. The rest of the
                  token can be constructed by the caller if needed. We
                  don't use all 32 bits in order to be able to return a
                  signed quantity to indicate failure.

    @returns:     NW_Int16
                  Either the token or -1 if the token is not found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Int16
NW_WBXML_Dictionary_getTagToken(NW_WBXML_Dictionary_t* dictionary, 
                                NW_String_UCS2Buff_t* name,
                                NW_Bool matchCase);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getAttributeToken2

    @synopsis:    Get attribute token using name (alt.)

    @scope:       public

    @parameters:
       [in] NW_WBXML_Dictionary_t* dictionary
                  The dictionary to look in.

       [in] NW_Uint32 encoding
                  The encoding of name.

       [in] NW_Uint32 charCount
                  Length of name in characters.

       [in] NW_Uint8* pBuf
                  The characters.

       [out] NW_Uint16* pPageToken
                  The token.

       [in] NW_Bool isName
                  NW_TRUE for attribute name, NW_FALSE for  attribute value.

    @description: Get attribute token using name (alt.) Boolean flag determines
                  whether to return attribute name (NW_TRUE) or attribute value
                  (NW_FALSE). A cleaner alternative is to use the more specific
                  functions which do not require the boolean argument.

                  These return the lower 16 bits of the fully qualified
                  token i.e., the token and code page. The rest of the
                  token can be constructed by the caller if needed. We
                  don't use all 32 bits in order to be able to return a
                  signed quantity to indicate failure.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Token returned.

       [NW_STAT_FAILURE]
                  Token not found or required parameter is NULL.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Dictionary_getAttributeToken2(NW_WBXML_Dictionary_t* dictionary,
                                       NW_Uint32 encoding,
                                       NW_Uint32 charCount,
                                       NW_Uint8* pBuf,
                                       NW_Uint16* pPageToken,
                                       NW_Bool isName);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getAttributeNameToken

    @synopsis:    Get attribute name token using name.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Dictionary_t* dictionary
                  The dictionary.

       [in] NW_Uint32 encoding
                  The encoding.

       [in] NW_Uint32 charCount
                  Length of name in characters.

       [in] NW_Uint8* pBuf
                  The name.

       [out] NW_Uint16* pPageToken
                  Returned token.

    @description: Get attribute name token using name.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Token returned.

       [NW_STAT_FAILURE]
                  Token not found or required parameter is NULL.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Dictionary_getAttributeNameToken(NW_WBXML_Dictionary_t* dictionary,
                                          NW_Uint32 encoding,
                                          NW_Uint32 charCount,
                                          NW_Uint8* pBuf,
                                          NW_Uint16* pPageToken);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getAttributeValueToken

    @synopsis:    Get attribute value token using name.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Dictionary_t* dictionary
                  The dictionary.

       [in] NW_Uint32 encoding
                  The encoding.

       [in] NW_Uint32 charCount
                  Length of attribute value.

       [in] NW_Uint8* pBuf
                  The attribute value.

       [out] NW_Uint16* pPageToken
                  The returned token.

    @description: Get attribute value token using name.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Token returned.

       [NW_STAT_FAILURE]
                  Token not found or required parameter is NULL.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Dictionary_getAttributeValueToken(NW_WBXML_Dictionary_t* dictionary,
                                           NW_Uint32 encoding,
                                           NW_Uint32 charCount,
                                           NW_Uint8* pBuf,
                                           NW_Uint16* pPageToken);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Dictionary_getTagToken2

    @synopsis:    Get tag token using name (alt.)

    @scope:       public

    @parameters:
       [in] NW_WBXML_Dictionary_t* dictionary
                  The dictionary.

       [in] NW_Uint32 encoding
                  The encoding.

       [in] NW_Uint32 charCount
                  Length of value.

       [in] NW_Uint8* pBuf
                  The value.

       [out] NW_Uint16* pPageToken
                  The returned token.

    @description: Get tag token using name (alt.)

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Token returned.

       [NW_STAT_FAILURE]
                  Token not found or required parameter is NULL.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Dictionary_getTagToken2(NW_WBXML_Dictionary_t* dictionary,
                                 NW_Uint32 encoding,
                                 NW_Uint32 charCount,
                                 NW_Uint8* pBuf,
                                 NW_Uint16* pPageToken);

/* 1. returns success, oom, or failure
   2. if oom or failure, no leaks and no residual memory allocations
   3. if oom or failure on return *ppDocType is NULL
   4  if success on return *ppDocType is a valid string or NULL if
   no matching dictionary or dictionary did not have a doc_type string. */
NW_Status_t
NW_WBXML_Dictionary_publicId_to_doctypeString(NW_Uint32 publicId,
                                              NW_String_t** ppDocType);



/** ----------------------------------------------------------------------- **
    @function:    CXML_Additional_Feature_Supprted

    @synopsis:    To decide about differnt. This helps in the debugging the 
                  cXML Library. It is not for any client. So, this function 
                  may not be used.

    @scope:       public

    @parameters:
       
    @description: There are bit masks defined for different features and for
                  internal use only.
              
    @returns:     

             Bit mask of features supported. If not feature is supported 
             then this function returns zero.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
CXML_Int32 CXML_Additional_Feature_Supprted();



#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_PARSER_WBXML_DICTIONARY_H */
