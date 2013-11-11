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
#include <xml/cxml/nw_string_string.h>
#include "cxml_xmlp_entity.h"
#include "cxml_xmlp_int_entity.h"

/*IMPORTANT NOTE: This array must be sorted by entity name.  The lookup function
 *				  does a binary search.
 *
 *                When you update this table make sure the constant 
 *				  CXML_Num_CaseInsensitive_Entries which is the count of 
 *				  caseinsensitive entries is updated correctly 
 */

#define CXML_Num_CaseInsensitive_Entries 126



static
const CXML_EntitySet_Entry_t CXML_EntitySet_Array[] = 
{
   // CaseSensitive entries
  {(CXML_Uint8*) "AElig", 198 },
  {(CXML_Uint8*) "Aacute", 193 },
  {(CXML_Uint8*) "Acirc", 194 },
  {(CXML_Uint8*) "Agrave", 192 },
  {(CXML_Uint8*) "Alpha", 913 },
  {(CXML_Uint8*) "Aring", 197 },
  {(CXML_Uint8*) "Atilde", 195 },
  {(CXML_Uint8*) "Auml", 196 },
  {(CXML_Uint8*) "Beta", 914 },
  {(CXML_Uint8*) "Ccedil", 199 },
  {(CXML_Uint8*) "Chi", 935 },
  {(CXML_Uint8*) "Dagger", 8225 },
  {(CXML_Uint8*) "Delta", 916 },
  {(CXML_Uint8*) "ETH", 208 },
  {(CXML_Uint8*) "Eacute", 201 },
  {(CXML_Uint8*) "Ecirc", 202 },
  {(CXML_Uint8*) "Egrave", 200 },
  {(CXML_Uint8*) "Epsilon", 917 },
  {(CXML_Uint8*) "Eta", 919 },
  {(CXML_Uint8*) "Euml", 203 },
  {(CXML_Uint8*) "Gamma", 915 },
  {(CXML_Uint8*) "Iacute", 205 },
  {(CXML_Uint8*) "Icirc", 206 },
  {(CXML_Uint8*) "Igrave", 204 },
  {(CXML_Uint8*) "Iota", 921 },
  {(CXML_Uint8*) "Iuml", 207 },
  {(CXML_Uint8*) "Kappa", 922 },
  {(CXML_Uint8*) "Lambda", 923 },
  {(CXML_Uint8*) "Mu", 924 },
  {(CXML_Uint8*) "Ntilde", 209 },
  {(CXML_Uint8*) "Nu", 925 },
  {(CXML_Uint8*) "OElig", 338 },
  {(CXML_Uint8*) "Oacute", 211 },
  {(CXML_Uint8*) "Ocirc", 212 },
  {(CXML_Uint8*) "Ograve", 210 },
  {(CXML_Uint8*) "Omega", 937 },
  {(CXML_Uint8*) "Omicron", 927 },
  {(CXML_Uint8*) "Oslash", 216 },
  {(CXML_Uint8*) "Otilde", 213 },
  {(CXML_Uint8*) "Ouml", 214 },
  {(CXML_Uint8*) "Phi", 934 },
  {(CXML_Uint8*) "Pi", 928 },
  {(CXML_Uint8*) "Prime", 8243 },
  {(CXML_Uint8*) "Psi", 936 },
  {(CXML_Uint8*) "Rho", 929 },
  {(CXML_Uint8*) "Scaron", 352 },
  {(CXML_Uint8*) "Sigma", 931 },
  {(CXML_Uint8*) "THORN", 222 },
  {(CXML_Uint8*) "Tau", 932 },
  {(CXML_Uint8*) "Theta", 920 },
  {(CXML_Uint8*) "Uacute", 218 },
  {(CXML_Uint8*) "Ucirc", 219 },
  {(CXML_Uint8*) "Ugrave", 217 },
  {(CXML_Uint8*) "Upsilon", 933 },
  {(CXML_Uint8*) "Uuml", 220 },
  {(CXML_Uint8*) "Xi", 926 },
  {(CXML_Uint8*) "Yacute", 221 },
  {(CXML_Uint8*) "Yuml", 376 },
  {(CXML_Uint8*) "Zeta", 918 },
  {(CXML_Uint8*) "aacute", 225 },
  {(CXML_Uint8*) "acirc", 226 },
  {(CXML_Uint8*) "acute", 180 },
  {(CXML_Uint8*) "aelig", 230 },
  {(CXML_Uint8*) "agrave", 224 },
  {(CXML_Uint8*) "alpha", 945 },
  {(CXML_Uint8*) "atilde", 227 },
  {(CXML_Uint8*) "auml", 228 },
  {(CXML_Uint8*) "beta", 946 },
  {(CXML_Uint8*) "ccedil", 231 },
  {(CXML_Uint8*) "chi", 967 },
  {(CXML_Uint8*) "dArr", 8659 },
  {(CXML_Uint8*) "dagger", 8224 },
  {(CXML_Uint8*) "darr", 8595 },
  {(CXML_Uint8*) "delta", 948 },
  {(CXML_Uint8*) "eacute", 233 },
  {(CXML_Uint8*) "ecirc", 234 },
  {(CXML_Uint8*) "egrave", 232 },
  {(CXML_Uint8*) "epsilon", 949 },
  {(CXML_Uint8*) "eta", 951 },
  {(CXML_Uint8*) "euml", 235 },
  {(CXML_Uint8*) "gamma", 947 },
  {(CXML_Uint8*) "hArr", 8660 },
  {(CXML_Uint8*) "harr", 8596 },
  {(CXML_Uint8*) "iacute", 237 },
  {(CXML_Uint8*) "icirc", 238 },
  {(CXML_Uint8*) "igrave", 236 },
  {(CXML_Uint8*) "iota", 953 },
  {(CXML_Uint8*) "iuml", 239 },
  {(CXML_Uint8*) "kappa", 954 },
  {(CXML_Uint8*) "lArr", 8656 },
  {(CXML_Uint8*) "lambda", 955 },
  {(CXML_Uint8*) "larr", 8592 },
  {(CXML_Uint8*) "mu", 956 },
  {(CXML_Uint8*) "ntilde", 241 },
  {(CXML_Uint8*) "nu", 957 },
  {(CXML_Uint8*) "oacute", 243 },
  {(CXML_Uint8*) "ocirc", 244 },
  {(CXML_Uint8*) "oelig", 339 },
  {(CXML_Uint8*) "ograve", 242 },
  {(CXML_Uint8*) "omega", 969 },
  {(CXML_Uint8*) "omicron", 959 },
  {(CXML_Uint8*) "oslash", 248 },
  {(CXML_Uint8*) "otilde", 245 },
  {(CXML_Uint8*) "otimes", 8855 },
  {(CXML_Uint8*) "ouml", 246 },
  {(CXML_Uint8*) "phi", 966 },
  {(CXML_Uint8*) "pi", 960 },
  {(CXML_Uint8*) "psi", 968 },
  {(CXML_Uint8*) "rArr", 8658 },
  {(CXML_Uint8*) "rarr", 8594 },
  {(CXML_Uint8*) "rho", 961 },
  {(CXML_Uint8*) "scaron", 353 },
  {(CXML_Uint8*) "sigma", 963 },
  {(CXML_Uint8*) "tau", 964 },
  {(CXML_Uint8*) "theta", 952 },
  {(CXML_Uint8*) "thorn", 254 },
  {(CXML_Uint8*) "uArr", 8657 },
  {(CXML_Uint8*) "uacute", 250 },
  {(CXML_Uint8*) "uarr", 8593 },
  {(CXML_Uint8*) "ucirc", 251 },
  {(CXML_Uint8*) "ugrave", 249 },
  {(CXML_Uint8*) "upsilon", 965 },
  {(CXML_Uint8*) "uuml", 252 },
  {(CXML_Uint8*) "xi", 958 },
  {(CXML_Uint8*) "yacute", 253 },
  {(CXML_Uint8*) "yuml", 255 },
  {(CXML_Uint8*) "zeta", 950 },
  {(CXML_Uint8*) "zwj", 8205 },
  {(CXML_Uint8*) "zwnj", 8204 },
// Case Insensitive entries
  {(CXML_Uint8*) "alefsym", 8501 },
  {(CXML_Uint8*) "amp", 38 },
  {(CXML_Uint8*) "and", 8743 },
  {(CXML_Uint8*) "ang", 8736 },
  {(CXML_Uint8*) "apos", 39 },
  {(CXML_Uint8*) "aring", 229 },
  {(CXML_Uint8*) "asymp", 8776 },
  {(CXML_Uint8*) "bdquo", 8222 },
  {(CXML_Uint8*) "brvbar", 166 },
  {(CXML_Uint8*) "bull", 8226 },
  {(CXML_Uint8*) "cap", 8745 },
  {(CXML_Uint8*) "cedil", 184 },
  {(CXML_Uint8*) "cent", 162 },
  {(CXML_Uint8*) "circ", 710 },
  {(CXML_Uint8*) "clubs", 9827 },
  {(CXML_Uint8*) "cong", 8773 },
  {(CXML_Uint8*) "copy", 169 },
  {(CXML_Uint8*) "crarr", 8629 },
  {(CXML_Uint8*) "cup", 8746 },
  {(CXML_Uint8*) "curren", 164 },
  {(CXML_Uint8*) "deg", 176 },
  {(CXML_Uint8*) "diams", 9830 },
  {(CXML_Uint8*) "divide", 247 },
  {(CXML_Uint8*) "empty", 8709 },
  {(CXML_Uint8*) "emsp", 8195 },
  {(CXML_Uint8*) "ensp", 8194 },
  {(CXML_Uint8*) "equiv", 8801 },
  {(CXML_Uint8*) "eth", 240 },
  {(CXML_Uint8*) "euro", 8364 },
  {(CXML_Uint8*) "exist", 8707 },
  {(CXML_Uint8*) "fnof", 402 },
  {(CXML_Uint8*) "forall", 8704 },
  {(CXML_Uint8*) "frac12", 189 },
  {(CXML_Uint8*) "frac14", 188 },
  {(CXML_Uint8*) "frac34", 190 },
  {(CXML_Uint8*) "frasl", 8260 },
  {(CXML_Uint8*) "ge", 8805 },
  {(CXML_Uint8*) "gt", 62 },
  {(CXML_Uint8*) "hearts", 9829 },
  {(CXML_Uint8*) "hellip", 8230 },
  {(CXML_Uint8*) "iexcl", 161 },
  {(CXML_Uint8*) "image", 8465 },
  {(CXML_Uint8*) "infin", 8734 },
  {(CXML_Uint8*) "int", 8747 },
  {(CXML_Uint8*) "iquest", 191 },
  {(CXML_Uint8*) "isin", 8712 },
  {(CXML_Uint8*) "lang", 9001 },
  {(CXML_Uint8*) "laquo", 171 },
  {(CXML_Uint8*) "lceil", 8968 },
  {(CXML_Uint8*) "ldquo", 8220 },
  {(CXML_Uint8*) "le", 8804 },
  {(CXML_Uint8*) "lfloor", 8970 },
  {(CXML_Uint8*) "lowast", 8727 },
  {(CXML_Uint8*) "loz", 9674 },
  {(CXML_Uint8*) "lrm", 8206 },
  {(CXML_Uint8*) "lsaquo", 8249 },
  {(CXML_Uint8*) "lsquo", 8216 },
  {(CXML_Uint8*) "lt", 60 },
  {(CXML_Uint8*) "macr", 175 },
  {(CXML_Uint8*) "mdash", 8212 },
  {(CXML_Uint8*) "micro", 181 },
  {(CXML_Uint8*) "middot", 183 },
  {(CXML_Uint8*) "minus", 8722 },
  {(CXML_Uint8*) "nabla", 8711 },
  {(CXML_Uint8*) "nbsp", 160 },
  {(CXML_Uint8*) "ndash", 8211 },
  {(CXML_Uint8*) "ne", 8800 },
  {(CXML_Uint8*) "ni", 8715 },
  {(CXML_Uint8*) "not", 172 },
  {(CXML_Uint8*) "notin", 8713 },
  {(CXML_Uint8*) "nsub", 8836 },
  {(CXML_Uint8*) "oline", 8254 },
  {(CXML_Uint8*) "oplus", 8853 },
  {(CXML_Uint8*) "or", 8744 },
  {(CXML_Uint8*) "ordf", 170 },
  {(CXML_Uint8*) "ordm", 186 },
  {(CXML_Uint8*) "para", 182 },
  {(CXML_Uint8*) "part", 8706 },
  {(CXML_Uint8*) "permil", 8240 },
  {(CXML_Uint8*) "perp", 8869 },
  {(CXML_Uint8*) "piv", 982 },
  {(CXML_Uint8*) "plusmn", 177 },
  {(CXML_Uint8*) "pound", 163 },
  {(CXML_Uint8*) "prime", 8242 },
  {(CXML_Uint8*) "prod", 8719 },
  {(CXML_Uint8*) "prop", 8733 },
  {(CXML_Uint8*) "quot", 34 },
  {(CXML_Uint8*) "radic", 8730 },
  {(CXML_Uint8*) "rang", 9002 },
  {(CXML_Uint8*) "raquo", 187 },
  {(CXML_Uint8*) "rceil", 8969 },
  {(CXML_Uint8*) "rdquo", 8221 },
  {(CXML_Uint8*) "real", 8476 },
  {(CXML_Uint8*) "reg", 174 },
  {(CXML_Uint8*) "rfloor", 8971 },
  {(CXML_Uint8*) "rlm", 8207 },
  {(CXML_Uint8*) "rsaquo", 8250 },
  {(CXML_Uint8*) "rsquo", 8217 },
  {(CXML_Uint8*) "sbquo", 8218 },
  {(CXML_Uint8*) "sdot", 8901 },
  {(CXML_Uint8*) "sect", 167 },
  {(CXML_Uint8*) "shy", 173 },
  {(CXML_Uint8*) "sigmaf", 962 },
  {(CXML_Uint8*) "sim", 8764 },
  {(CXML_Uint8*) "spades", 9824 },
  {(CXML_Uint8*) "sub", 8834 },
  {(CXML_Uint8*) "sube", 8838 },
  {(CXML_Uint8*) "sum", 8721 },
  {(CXML_Uint8*) "sup", 8835 },
  {(CXML_Uint8*) "sup1", 185 },
  {(CXML_Uint8*) "sup2", 178 },
  {(CXML_Uint8*) "sup3", 179 },
  {(CXML_Uint8*) "supe", 8839 },
  {(CXML_Uint8*) "szlig", 223 },
  {(CXML_Uint8*) "there4", 8756 },
  {(CXML_Uint8*) "thetasym", 977 },
  {(CXML_Uint8*) "thinsp", 8201 },
  {(CXML_Uint8*) "tilde", 732 },
  {(CXML_Uint8*) "times", 215 },
  {(CXML_Uint8*) "trade", 8482 },
  {(CXML_Uint8*)"uml", 168 },
  {(CXML_Uint8*) "upsih", 978 },
  {(CXML_Uint8*) "weierp", 8472 },
  {(CXML_Uint8*) "yen", 165 },
  {(CXML_Uint8*) "zwj", 8205 },
  {(CXML_Uint8*) "zwnj", 8204 },
};

const CXML_Uint32 CXML_ALL_PREDEFINE_ENTITIES = (sizeof(CXML_EntitySet_Array)
   / sizeof(CXML_EntitySet_Entry_t));

/* It assumes the parser at the begginning i.e '&' symbol. 
 * The entity name stops at (;) or at '>'. If it is stopped
 * at (;) then it is a valid entity.
 * If it stops at '>' then it is not a valid entity.
 */


NW_Status_t CXML_XML_Parser_Entity(NW_XML_Reader_t* pT,
							    NW_XML_Reader_Interval_t* I_entityData,
							    NW_Bool* entityFound)
{
 NW_Status_t s;
 NW_Bool endFound = NW_FALSE;
 //NW_Uint32 isSpace;
 NW_Uint32 cnt=0;
 NW_Uint32 match;

 s = NW_XML_Reader_Advance(pT); //Pass over the '&' symbol

 if (NW_STAT_IS_FAILURE(s))
 {
  return NW_STAT_FAILURE;
 }

 NW_XML_Reader_Interval_Start(I_entityData, pT);

 // Just check for false entity or error in entity e.g. If entity is not 
 // terminated by (;)


 for(cnt=0; ; )
 {

  cnt++;

  //check for terminating entity character

  s = NW_XML_Reader_AsciiCharMatch(pT, ';', &match); 

  if (NW_STAT_IS_FAILURE(s))
  {
    return NW_STAT_FAILURE;
  }

  if(match)
  {
   endFound = NW_TRUE;
   break;
  }

  //Check condition if it is malformed entity. Exit at least 
  //at the end of attribute or contents.

  s = NW_XML_Reader_AsciiCharMatch(pT, '>', &match); 

  if (NW_STAT_IS_FAILURE(s))
  {
    return NW_STAT_FAILURE;
  }

    if(match)
    {
     endFound = NW_FALSE;
     break;
    }

   //Move to next character

   s = NW_XML_Reader_Advance(pT); 

   if (NW_STAT_IS_FAILURE(s))
   {
    return NW_STAT_FAILURE;
   }
 }//end for

 if(endFound == NW_TRUE)
 {
	 NW_XML_Reader_Interval_Stop(I_entityData, pT);
	 *entityFound = NW_TRUE;

    //Move over end of entity i.e. (;) character.

    s = NW_XML_Reader_Advance(pT); 

    if (NW_STAT_IS_FAILURE(s))
    {
     return NW_STAT_FAILURE;
    }
 }
 else
 {
	*entityFound = NW_FALSE;
	s = NW_STAT_SUCCESS;
 }


 return s;
}//end CXML_Parser_Entity(...)


/* Function to verify the digit depending on its base */

static
CXML_Int32 CXML_XML_IsValidDigit (const CXML_Ucs2 ch, 
                       NW_Int32 base) 
{
  switch (base) {
    case 10: 
      return CXML_Str_Isdigit (ch);

    case 16:
      return CXML_Str_Isxdigit (ch);

    default:
      return 0;
  }
}


/* 
 * Function converts the entity string to the entity value.
 */

static
NW_Bool CXML_XML_GetNumEntityChar (const CXML_Ucs2* instring, 
                           CXML_Ucs2 *retchar, 
                           CXML_Int32 base) 
{
  CXML_Int32 result = 0;
  CXML_Int32 prevResult;
  CXML_Uint32 currDigit = 0;
  
  if (*instring == 0) {
    return NW_FALSE;
  }

  while (*instring != 0) {
    if (!(CXML_XML_IsValidDigit (*instring, base)))
      return NW_FALSE;
  
    if (CXML_Str_Isdigit (*instring)) {
      currDigit = *instring - CXML_ASCII_0;
      instring++;
    } 
    else {
      currDigit = CXML_Str_ToUpper (*instring) - CXML_ASCII_UPPER_A + 10;
      instring++;
    }

    prevResult = result;
    result = result * base + currDigit;
    if (result < prevResult) {
      return NW_FALSE;
    }
  }
  if (result > 0xffff || result < 0x0) 
    return NW_FALSE;

  *retchar = (NW_Ucs2) result;
  return NW_TRUE;
}//end NW_Bool CXML_XML_GetNumEntityChar(..)


/* This function uses the binary search to find the predefined
 * entity names and corresponding value if found in the array.
 */

static NW_Ucs2
CXML_Get_Entity_Val (NW_Ucs2 *name)
{
  CXML_Int32 index;
  NW_Status_t s = NW_STAT_SUCCESS;
  const CXML_EntitySet_Entry_t* entry;
  NW_Ucs2* ucs2TempStr = NULL;
  CXML_Uint32 entityLength = 0;

  /* First do a binary search search in the case sensitive part of the array */
  CXML_Int32 low = 0;
  CXML_Int32 high = CXML_ALL_PREDEFINE_ENTITIES - CXML_Num_CaseInsensitive_Entries - 1;
  CXML_Int32 res = 0;

  while (low <= high ) {
    index = (high + low) / 2;
    entry = & (CXML_EntitySet_Array[index]);
    entityLength = CXML_Asc_strlen((CXML_Int8 *)entry->name);

    s = NW_String_byteToUCS2Char(entry->name,entityLength,&ucs2TempStr);

	if (NW_STAT_IS_FAILURE(s)) 
	 {
	  return 0;
	 }

    // do a case insensitive string comparison
    
    res = CXML_Str_StrcmpConst( name, ucs2TempStr );

    if(ucs2TempStr != NULL)
	 {
	  NW_Mem_Free(ucs2TempStr);
      ucs2TempStr = NULL;
	 }

    if ( res > 0 ) {
      /* name is ahead of this slot.  Increase low bound. */
      low = index + 1;
    } else if ( res < 0 ) {
      /* name is behind this slot.  Decrease high bound. */
      high = index - 1;
    } else {
      /* Found the entity name.  Return its value. */
      return entry->value;
    }
  }

  /* if no match was found search in the case insensitive part of the table. */

  low = CXML_ALL_PREDEFINE_ENTITIES - CXML_Num_CaseInsensitive_Entries;
  high = CXML_ALL_PREDEFINE_ENTITIES - 1;
  res = 0;
  while (low <= high )
  {
    index = (high + low) / 2;
    entry = &(CXML_EntitySet_Array[index]);
    entityLength = CXML_Asc_strlen((CXML_Int8 *)entry->name);

    s = NW_String_byteToUCS2Char(entry->name,entityLength,&ucs2TempStr);

	if (NW_STAT_IS_FAILURE(s)) 
	 {
	  return 0;
	 }

    // do a case insensitive string comparison
    
    res = CXML_Str_Stricmp( name, ucs2TempStr );

    if(ucs2TempStr != NULL)
	 {
	  NW_Mem_Free(ucs2TempStr);
      ucs2TempStr = NULL;
	 }
	  

    if ( res > 0 ) {
      /* name is ahead of this slot.  Increase low bound. */
      low = index + 1;
    } else if ( res < 0 ) {
      /* name is behind this slot.  Decrease high bound. */
      high = index - 1;
    } else {
      /* Found the entity name.  Return its value. */
      return entry->value;
    }
  }/*end while */

  /* if no match were found we return 0 */
  return 0;
}

/* This function is called when entity of valid syntax is found. The 
 * entity validity is checked here.
 *
 * E.g. &ggg; is valid syntax entity but not a valid entity. In this 
 *             case continue parsing the entity as it is. Create this
 *             as normal contents not as the entity.
 *
 * entityContent ==> Entity Name (e.g. "amp")
 * length        ==> Length of Entity Name.
 * entityVal     ==> This parameter will contain the entity value.
 *                   if it is character entites (decimal, hex or predefined)
 *
 * entityFound  ==> TRUE if it is valid entity of any kind.
 * encoding     ==> Encoding of input string.
 *
 * Resolve the following type of entities 
 * 1) Character hex entry
 * 2) Character decimal entry
 * 3) Prefined entry
 * 4) Internal Entities defined in the DTD.
 *
 */


static
NW_Status_t CXML_XML_Resolve_Entity( CXML_Uint8* entityContent,
									CXML_Uint32 length,
									NW_Uint32* entityVal,
                                    CXML_Uint8** entityValStr,
									NW_Bool* entityFound,
                                    NW_Uint32 encoding,
                                    void* internalEntityList)
{
 NW_Status_t s = NW_STAT_SUCCESS;
 NW_Ucs2 entityChar = 0;
 NW_Ucs2* ucs2Str = NULL;
 NW_Bool entityGot = NW_FALSE;


    
    if( encoding == HTTP_iso_10646_ucs_2 )
    {
     /* Extra two bytes for NULL termination as UCS should be always 
      * even bytes.
      */
     ucs2Str = (NW_Ucs2*) NW_Mem_Malloc(length+2);
     if(ucs2Str != NULL)
     {
      (void)NW_Mem_memcpy(ucs2Str, entityContent, length );
      /* Null-terminate the string */
       ucs2Str[length/2] = '\000';
     }
     else
     {
      s = NW_STAT_OUT_OF_MEMORY;
     }
    }
    else
    {
     s = NW_String_byteToUCS2Char(entityContent,length,&ucs2Str);
    }

    
	 if (NW_STAT_IS_FAILURE(s)) 
	 {
          if(ucs2Str != NULL)
          {
           NW_Mem_Free(ucs2Str);
          }
	  return s;
	 }

 /*Check and validate for Hex/Decimal numeric character entry */

 if(ucs2Str[0] == '#')
 {
	entityGot = CXML_TRUE;
	if( (ucs2Str[1] == 'x') || (ucs2Str[1] == 'X') )  //Hex entity
	{
      
	  if(CXML_XML_GetNumEntityChar(ucs2Str + 2,&entityChar,16) != NW_TRUE)
	  {
       /* Entity syntax is valid, but entity value is wrong so continue parsing it as
        * normal syntax.
        */
       *entityFound = CXML_FALSE;
         if(ucs2Str != NULL)
          {
           NW_Mem_Free(ucs2Str);
          }
	   return NW_STAT_SUCCESS;
	  }
      *entityVal = (entityChar);
      *entityValStr = NULL;
	}
  else
	{
      /*Check and validate for Decimal numeric character entry */ 
	  if(CXML_XML_GetNumEntityChar(ucs2Str + 1,&entityChar,10) != NW_TRUE)
	  {
       /* Entity syntax is valid, but entity value is wrong so continue parsing it as
        * normal syntax.
        */
       *entityFound = CXML_FALSE;
         if(ucs2Str != NULL)
          {
           NW_Mem_Free(ucs2Str);
          }
	   return NW_STAT_SUCCESS;
	  }
      *entityVal = (entityChar);
      *entityValStr = NULL;
	}
   *entityFound = CXML_TRUE;
 } //end if(ucs2Str[0] == '#')

 /* Check for only predefined entities */

 if(entityGot != CXML_TRUE)
 {	
	 if( ( entityChar = CXML_Get_Entity_Val(ucs2Str) ) != 0)
	 {
	  entityGot = NW_TRUE;
	 }

	  
	 if(entityGot == NW_TRUE)
	 {
	  *entityVal =  entityChar;
      *entityValStr = NULL;
	  *entityFound = CXML_TRUE;
	 }
 } //end if(..)

 /*Check for the Internal Entity */

 
 if(entityGot != CXML_TRUE)
 {	
  s = CXML_XML_Resolve_Internal_Entity(ucs2Str, entityValStr, entityFound, encoding, internalEntityList);  
  *entityVal =  0;
 }
 

 if(ucs2Str != NULL)
 {
  NW_Mem_Free(ucs2Str);
 }

 return NW_STAT_SUCCESS;
}//end CXML_XML_Resolve_Entity()

/* Reads the entity data. Decide about the entity. */


NW_Status_t CXML_XML_Handle_entity(NW_XML_Reader_t* pT,
								   NW_XML_Reader_Interval_t* pI_entityData,
								   NW_Uint32* entityVal,
                                   NW_Uint8** entityValStr,
								   NW_Bool* entityFound,
                                   void* internalEntityList)
{
   NW_Status_t s;
   NW_Uint32 length;
   NW_Uint32 byteLength;
   NW_Uint8* pContent;

	if (!NW_XML_Reader_Interval_IsWellFormed(pI_entityData)) {
        return NW_STAT_FAILURE;
    }

    length = pI_entityData->stop - pI_entityData->start;
    byteLength = length;

    s = NW_XML_Reader_DataAddressFromBuffer(pT, pI_entityData->start,
                                            &byteLength,
                                            &pContent);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }

    if (byteLength != length) {
        return NW_STAT_FAILURE;
    }

	s = CXML_XML_Resolve_Entity(pContent,length,entityVal,entityValStr,entityFound,
                                pT->encoding, internalEntityList);

return s;
}//end CXML_XML_Handle_entity(..)

 /* The following function converts the numeric entities to the 
  * predefined entity.
  */

static
NW_Status_t CXML_XML_Entity_to_Ascii(CXML_Uint32 entityVal,
									CXML_Byte* entityStr,
									CXML_Uint32* strLen)
{
 NW_Uint32 i = 0, j=0;
 CXML_Uint32 entityLength = 0;
 NW_Status_t s = NW_STAT_FAILURE;

 for(i=0; i < CXML_ALL_PREDEFINE_ENTITIES; i++)
 {
  if(CXML_EntitySet_Array[i].value == entityVal)
  {
   entityStr[0] = '&'; //Starting of the entity

   entityLength = CXML_Asc_strlen((CXML_Int8 *)CXML_EntitySet_Array[i].name);

   for(j=0; j < entityLength; j++)
   {
    entityStr[j+1] = CXML_EntitySet_Array[i].name[j];
   }
   
    entityStr[j + 1] = ';' ; //end of entity
    *strLen = entityLength + 2;
    entityStr[*strLen] = '\0';
	s = NW_STAT_SUCCESS;
	break;
  }//endif
 }//end for(..)

 return s;
}//end CXML_XML_Ascii_to_Entity(..)

/* This function assumes that predefined entities. The predefined
 * entities are defined in the CXML_EntitySet_Array[].
 * 
 */

EXPORT_C
NW_Status_t CXML_XML_Get_Entity(CXML_Uint32 entityVal,
								CXML_Byte* entityStr,
								CXML_Uint32* strLen)
{
 NW_Status_t s = NW_STAT_SUCCESS;
 *strLen = 0;

 
  s = CXML_XML_Entity_to_Ascii( entityVal,entityStr,strLen);

 if(s == NW_STAT_FAILURE)
 {
  *entityStr = NULL;
  *strLen    = NULL;
 }

 return s;
}//end CXML_XML_Get_Entity()

