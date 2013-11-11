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
    @package:     NW_XML

    @synopsis:    default

    @description: default

 ** ----------------------------------------------------------------------- **/

#ifndef NW_XMLPARSER_H
#define NW_XMLPARSER_H

#include <xml/cxml/nw_xmlp_xmlreader.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/** ----------------------------------------------------------------------- **
    @enum:        NW_XML_PiFormTypeTag

    @synopsis:    Process instruction type definitions.

    @scope:       public
    @names:
       [UNKNOWFORM]
                  Don't know what type it is.

       [PI]
                  Process instruction.

       [XMLDECL]
                   May be a TextDecl

       [XMLRESERVED]
                  XML reserved.

    @description: Process instruction type definitions.
 ** ----------------------------------------------------------------------- **/
typedef enum NW_XML_PiFormTypeTag_e {
    UNKNOWFORM,
    PI,
    XMLDECL,
    XMLRESERVED
} NW_XML_PiFormTypeTag_t;

/* Deprecated - Lext in for backward compatibility. */
typedef enum NW_XML_PiFormTypeTag_e NW_PiFormTypeTag_t;

/* RME documentation tools do not support function calls as variables yet */  

/* * ----------------------------------------------------------------------- **
    @struct:      NW_XML_Parser_EventCallbacks

    @synopsis:    default

    @scope:       public
    @variables:
       NW_Status_t (*StartDocument_CB) (NW_XML_Reader_t*, void*)
                  default

       NW_Status_t (*EndDocument_CB) (NW_XML_Reader_t*, void*)
                  default

       NW_Status_t (*PiForm_CB) (NW_XML_Reader_t*,
                                NW_XML_PiFormTypeTag_t piTypeTag,
                                const NW_XML_Reader_Interval_t* pI_name,
                                const NW_XML_Reader_Interval_t* pI_version,
                                const NW_XML_Reader_Interval_t* pI_encoding,
                                const NW_XML_Reader_Interval_t* pI_standalone,
                                const NW_XML_Reader_Interval_t* pI_content,
                                void*)
                  default

       NW_Status_t (*Tag_Start_CB) (NW_XML_Reader_t*,
                                    const NW_XML_Reader_Interval_t* pI_name,
                                    void*)
                  default

       NW_Status_t (*Tag_End_CB) (NW_XML_Reader_t*,
                                 const NW_XML_Reader_Interval_t* pI_name,
                                 NW_Uint32 emptyTagFlag,
                                 void*)
                  default

       NW_Status_t (*Attr_Start_CB) (NW_XML_Reader_t*,
                                    const NW_XML_Reader_Interval_t* pI_name,
                                    void*)
                  default

       NW_Status_t (*Attr_VarVal_CB) (NW_XML_Reader_t*,
                                      const NW_XML_Reader_Interval_t* pI_name,
                                      const NW_XML_Reader_Interval_t* pI_value,
                                      void*)
                  default

       NW_Status_t (*Attributes_End_CB) (NW_XML_Reader_t*, NW_Uint32 attributeCount,
                                    void*)
                  default

       NW_Status_t (*Cdata_CB) (NW_XML_Reader_t*,
                               const NW_XML_Reader_Interval_t* pI_cdata,
                               void*)
                  default

       NW_Status_t (*Content_CB) (NW_XML_Reader_t*,
                                 const NW_XML_Reader_Interval_t* pI_cdata,
                                 void*)
                  default

       NW_Status_t (*Comment_CB) (NW_XML_Reader_t*,
                                 const NW_XML_Reader_Interval_t* pI_comment,
                                 void*)
                  default

       NW_Status_t (*Extension_CB) (NW_XML_Reader_t*, void*)
                  default

       NW_Status_t (*Exception_CB) (NW_XML_Reader_t*, void*)
                  default

       void* pClientPointer
                  Passed back in each callback.

    @description: default
 ** ----------------------------------------------------------------------- **/
struct NW_XML_Parser_EventCallbacks_s
{
  NW_Status_t (*StartDocument_CB) (NW_XML_Reader_t*, void*);
  NW_Status_t (*EndDocument_CB) (NW_XML_Reader_t*, void*);
  NW_Status_t (*PiForm_CB) (NW_XML_Reader_t*,
                            NW_XML_PiFormTypeTag_t piTypeTag,
                            const NW_XML_Reader_Interval_t* pI_name,
                            const NW_XML_Reader_Interval_t* pI_version,
                            const NW_XML_Reader_Interval_t* pI_encoding,
                            const NW_XML_Reader_Interval_t* pI_standalone,
                            const NW_XML_Reader_Interval_t* pI_content,
                            void*);
  NW_Status_t (*Tag_Start_CB) (NW_XML_Reader_t*,
                               const NW_XML_Reader_Interval_t* pI_name,
                               void*);
  NW_Status_t (*Tag_End_CB) (NW_XML_Reader_t*,
                             const NW_XML_Reader_Interval_t* pI_name,
                             NW_Uint32 emptyTagFlag,
                             void*);
  NW_Status_t (*Attr_Start_CB) (NW_XML_Reader_t*,
                                const NW_XML_Reader_Interval_t* pI_name,
                                void*);
  NW_Status_t (*Attr_VarVal_CB) (NW_XML_Reader_t*,
                                 const NW_XML_Reader_Interval_t* pI_name,
                                 const NW_XML_Reader_Interval_t* pI_value,
                                 void*);
  NW_Status_t (*Attributes_End_CB) (NW_XML_Reader_t*, NW_Uint32 attributeCount,
                                    void*);
  NW_Status_t (*Cdata_CB) (NW_XML_Reader_t*,
                           const NW_XML_Reader_Interval_t* pI_cdata,
                           void*);
  NW_Status_t (*Content_CB) (NW_XML_Reader_t*,
                             const NW_XML_Reader_Interval_t* pI_cdata,
                             void*);
  NW_Status_t (*Comment_CB) (NW_XML_Reader_t*,
                             const NW_XML_Reader_Interval_t* pI_comment,
                             void*);
  NW_Status_t (*Extension_CB) (NW_XML_Reader_t*, void*);

  NW_Status_t (*Exception_CB) (NW_XML_Reader_t*, void*);

  NW_Status_t (*Entity_CB) (NW_XML_Reader_t*, NW_Uint32 numEntity,void* ,CXML_Uint8* str);

  NW_Status_t (*Attr_Entity_VarVal_CB) (NW_XML_Reader_t* pT,
                                        const NW_XML_Reader_Interval_t* pI_name,
                                        NW_Uint8* pValue,
										NW_Uint32 valueByteLength,
                                        void* pV);

  void* pClientPointer; /* passed back in each callback*/
};


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Parse

    @synopsis:    The XML main parser.

    @scope:       public

    @parameters:
       [in] NW_XML_Reader_t* t
                  The XML reader.

       [in] const struct NW_XML_Parser_EventCallbacks_s* pE
                  The callbacks.

    @description: The XML main parser. Parses the entire document.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Parsed the document.

       [NW_STAT_FAILURE]
                  Failed somewhere along the way.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_XML_Parse(NW_XML_Reader_t* t, const struct NW_XML_Parser_EventCallbacks_s* pE);


#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */


#endif  /* NW_XMLPARSER_H */
