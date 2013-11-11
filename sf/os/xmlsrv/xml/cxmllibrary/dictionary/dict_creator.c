/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Author:       Frank Richichi
* Created:      Thu Apr 25 10:34:06 2002
* Modified:     Thu Apr 25 11:12:35 2002 (Frank Richichi) richichi@D5250215
* Language:     C
* Subsystem:    N/A
* RCS:          $Id$
*
*/


/* ****************************************************************
**          Copyright 2000 - Nokia Corporation  All rights reserved.
**          Nokia Americas
**          6000 Connection Drive
**          Irving, Texas 75039
**
**          Restricted Rights: Use, duplication, or disclosure by the
**          U.S. Government is subject to restrictions as set forth in
**          subparagraph (c)(1)(ii) of DFARS 252.227-7013, or in FAR
**          52.227-19, or in FAR 52.227-14 Alt. III, as applicable.
**
**          This software is proprietary to and embodies the confidential
**          technology of Nokia  Possession, use, or copying of this software 
**          and media is authorized only pursuant to a valid written license
**          from Nokia or an authorized sublicensor.
**
**          Nokia  - Wireless Software Solutions
*****************************************************************/

#include <stdio.h>
#include <io.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>


/* ----------------------------------------------------------------------- **
   The following strings are used to identify the beginning of a tag or 
   attribute list in the input file.
** ----------------------------------------------------------------------- **/

#define TAG_TABLE_HEADING   "TAGS_FOR_CODE_PAGE"
#define ATTR_TABLE_HEADING  "ATTRIBUTES_FOR_CODE_PAGE"
#define NO_STRINGS "NO_STRINGS"

/* ----------------------------------------------------------------------- **
   Types for various tables.
** ----------------------------------------------------------------------- **/
#define NAME_TABLE_T    "NW_Byte"
#define TOKEN_TABLE_T   "NW_WBXML_DictEntry_t"
#define CODEPAGE_TABLE_T  "NW_WBXML_Codepage_t"
#define DICTIONARY_TABLE_T  "NW_WBXML_Dictionary_t"
#define NAMES_T     "NW_String_UCS2Buff_t"
#define ELEMENT_T "NW_%s_Element"
#define ATTRIBUTE_T "NW_%s_Attribute"
#define ELEMENT_TOKEN_T "NW_%s_ElementToken"
#define ATTRIBUTE_TOKEN_T "NW_%s_AttributeToken"

/* ----------------------------------------------------------------------- **
   Misc array limits
** ----------------------------------------------------------------------- **/
#define TABLE_SIZE      255
#define MAX_LINE        255
#define MAX_NAME        255
#define MAX_TOKEN       4
#define MAX_DICT_NAME   255
#define MAX_PUBLIC_ID   255
#define MAX_DOC_TYPE    255
#define MAX_START_NAME  255
#define MAX_TYPE_NAME   255

/* ----------------------------------------------------------------------- **
  Names used in the generated tables
** ----------------------------------------------------------------------- **/
#define TAG_NAME    "tag"
#define ATTR_NAME   "attribute"


/* ----------------------------------------------------------------------- **
   Define a type for codepage storage
** ----------------------------------------------------------------------- **/
typedef struct codepage_s {
  int   num;    /* the code page number */
  int   size;   /* the number of entries in table */
} codepage_t;


/* ----------------------------------------------------------------------- **
   Header information
** ----------------------------------------------------------------------- **/
char dict_name[MAX_DICT_NAME];
char doc_type[MAX_DOC_TYPE];
char public_id[10];

/* ----------------------------------------------------------------------- **
   Scratch tables
** ----------------------------------------------------------------------- **/
char * tokens[TABLE_SIZE];
char * names[TABLE_SIZE];
int sorted_indexes[TABLE_SIZE];
codepage_t tag_codepages[TABLE_SIZE];
codepage_t attr_codepages[TABLE_SIZE];


/* ----------------------------------------------------------------------- **
   Records time of program execution and command line arguments
** ----------------------------------------------------------------------- **/
static time_t timestamp;
static int main_argc = 1;
static char** main_argv;

/* ----------------------------------------------------------------------- **
   Globals so error clean up is easy
** ----------------------------------------------------------------------- **/
static FILE* input_dict;
static FILE* output_c;
static FILE* output_h;

static void exit_error(int exit_code) {
  static char errmsg[]
  = "#error \"Bad input dictionary data cannot complete code generation\"\n";
  if (input_dict) fclose(input_dict);
  if (output_c) {
    fprintf(output_c, errmsg);
    fclose(output_c);
  }
  if (output_h) {
    fprintf(output_h, errmsg);
    fclose(output_h);
  }
  exit(exit_code);
}

void print_usage(char* progname)
{
  fprintf(stderr,
  "Dictionary creator - creates a .c and .h source file for a\n"
  "NW_Wbxml_Dictionary_t structure from a dictionary data file.\n"
  "\n"
  "Usage: %s data_input_file dot_c_output_file dot_h_output_file\n"
  "\n"
  " - all args are required\n"
  "\n"
  "Comments are defined to be:\n"
  "\n"
  " - A line whose first non-whitespace char is '#'\n"
  " - A blank line or line with only whitespace\n"
  " Note: Comments are NOT permitted at the end of other lines.\n"
  "\n"
  "Input format:\n"
  "\n"
  " - Dictionary name = the first non-comment line.  The dictionary\n"
  "   that will be created will use this string in the dictionary \n"
  "   name.  For example, if the dictionary name is \"wml\", the\n"
  "   following dictionary will be created:\n"
  "\n"
  "     NW_Wbxml_Dictionary_t NW_wml_dictionary = { ... }\n"
  "\n"
  " - Public id = the second non-comment line.  This is the Public \n"
  "   Identifier as specified in section 7.2 of the WBML spec.  If\n"
  "   a public id has not been defined, use \"1\".\n"
  "\n"
  " - Doc Type = the third non-comment line.  This is the source\n"
  "   document's Document Type Declaration.\n"
  "\n"
  " - The beginning of a Tag table has the following syntax:\n"
  "\n"
  "     TAGS_FOR_CODE_PAGE <codepage_number> [NO_STRINGS]\n"
  "\n"
  "     <codepage_number> is required and is a base 10 number\n"
  "\n"
  "     [NO_STRINGS] is an optional keyword which means output only\n"
  "     the token structures, treat all token names as empty strings\n"
  "\n"
  " - The beginning of an Attribute table has the following syntax:\n"
  "\n"
  "     ATTRIBUTES_FOR_CODE_PAGE <codepage_number> [NO_STRINGS]\n"
  "     \n"
  "     <codepage_number> is required and is a base 10 number\n"
  "\n"
  "     [NO_STRINGS] is an optional keyword which means output only\n"
  "     the token structures, treat all token names as empty strings\n"
  "\n"
  " - The syntax for an entry is the same for both Tag and Attribute \n"
  "   tables\n"
  "\n"
  "     <token> <name>\n"
  "\n"
  "     <token> must be a hexadecimal number written as 0x?? (e.g., 0x07)\n"
  "     NOTE: Tokens are sorted as strings so 0x7 MUST be entered as 0x07\n"
  "           or the sorting won't work!  It is ok to mix case as all hex\n"
  "           digits are converted to lower case before the sort.\n"
  "\n"
  "     <name> is any sequence of printable characters without whitespace\n"
  "\n"
  " - The input tables do NOT have to be sorted \n"
  "\n"
  " - If a language has \"Attribute Value Tokens\", they should be included\n"
  "   in the Attribute table\n"
  "\n"
  "Example input file (example.dict):\n"
  "\n"
  "  #\n"
  "  # Sample dictionary for the WML language\n"
  "  #\n"
  "  wml\n"
  "  #\n"
  "  # WML version 1.2 has a public id of 9\n"
  "  #\n"
  "  9\n"
  "  #\n"
  "  # WML 1.2 doc type\n"
  "  #\n"
  "  -//WAPFORUM//DTD WML 1.1//EN\n"
  "  #\n"
  "  # Tags\n"
  "  #\n"
  "  TAGS_FOR_CODE_PAGE 0\n"
  "  0x2b go\n"
  "  0x1d td\n"
  "  #\n"
  "  # Attributes\n"
  "  #\n"
  "  ATTRIBUTES_FOR_CODE_PAGE 0\n"
  "  0x12 format\n"
  "  0xA0 wrap\n"
  "  0x23 newcontext=true\n"
  "  0x8F http://www.\n"
  "\n"
  "Example run command:\n"
  "\n"
  "  %s example.dict example.c example.h\n",
  progname, progname);
}

static void print_automaticallyGeneratedCodeWarning(FILE *f)
{
  int i;
  static char automaticallyGeneratedCodeWarning[] =
  "/*\n"
  "** WARNING\n"
  "**\n"
  "** DO NOT EDIT - THIS CODE IS AUTOMATICALLY GENERATED\n"
  "**               FROM A DATA FILE BY THE DICTIONARY CREATION PROGRAM";

  fprintf(f, "%s\n", automaticallyGeneratedCodeWarning);
  fprintf(f,
          "**\n"
          "** This file generated on %s"
          "**                        (coordinated universal time)\n"
          "**\n"
          "** Command line: ",
          asctime(gmtime(&timestamp))); /* asctime() generates a newline at the end */
  fprintf(f, "%s", main_argv[0]);
  for (i = 1; i < main_argc; i++) {
    fprintf(f, " %s", main_argv[i]);
  }
  fprintf(f, "\n*/");
}

static void print_copyright(FILE *f)
{
  struct tm* tm_time;
  static const char copyright[] =
  "/* ****************************************************************\n"
  "**          Copyright %d - Nokia Corporation  All rights reserved.\n"
  "**          Nokia Americas\n"
  "**          6000 Connection Drive\n"
  "**          Irving, Texas 75039\n"
  "**\n"
  "**          Restricted Rights: Use, duplication, or disclosure by the\n"
  "**          U.S. Government is subject to restrictions as set forth in\n"
  "**          subparagraph (c)(1)(ii) of DFARS 252.227-7013, or in FAR\n"
  "**          52.227-19, or in FAR 52.227-14 Alt. III, as applicable.\n"
  "**\n"
  "**          This software is proprietary to and embodies the confidential\n"
  "**          technology of Nokia  Possession, use, or copying of this software\n"
  "**          and media is authorized only pursuant to a valid written license\n"
  "**          from Nokia or an authorized sublicensor.\n"
  "**\n"
  "**          Nokia  - Wireless Software Solutions\n"
  "*****************************************************************/";

  tm_time = gmtime(&timestamp);
  fprintf(f, copyright,
          tm_time->tm_year + 1900);
}

static FILE * open_file(char* fn, char *perms){
  FILE* f;
  
  if ((fn == 0)||((f = fopen(fn, perms)) == 0)){    
    return 0;
  }else{    
    return f;
  }
  
}

static init() {
  int i;

  for (i=0; i < TABLE_SIZE; i++) {
    tag_codepages[i].num = -1;
    attr_codepages[i].num = -1;
  }
}


/* ----------------------------------------------------------------------- **
  Read the next line of input and store it in s.  
 
  NOTE: s will have the new-line character stripped
 
  RETURN: 1 for success and 0 for EOF or failure 
** ----------------------------------------------------------------------- **/
static int get_line(FILE *f, char s[], int n){
  int i = 0, j, indx, len;
  char line[MAX_LINE];

  for(;;) {
    if ((fgets(line, n, f)) == 0) {
      /* Either EOF or an error occurred */
      return 0;
    }
    len = (int) strlen(line);
    /* Skip any preceeding whitespace */
    for (i=0; i < len; i++) {
      if (isspace(line[i]))
      continue;
      break;
    }
    if (i >= len || line[i] == '#')
      continue;
    break;
  }

  /* Fill in s */
  for (j=i, indx = 0; j < (int) strlen(line); i++, j++, indx++) {
    if (line[j] == '\n') {
      break;
    }
    if (line[j] == '\t') {
      /* convert tab to space for isprint() test */
      line[j] = ' ';
    }
    if (!isprint(line[j])) {
      /* stops on bogus char */
      fprintf(stderr,
              "ERROR: Illegal character (may be control char) in input text "
              "near file byte offset %ld\n",
              ftell(f));
      exit_error(1);
    }
    s[indx] = line[j];
    if (indx + 1 == MAX_LINE) break;
  }

  if (indx > 0)
    s[indx] = '\000';

  return 1;
}


/* ----------------------------------------------------------------------- **
   Read the a line of input and break it into three items:
   For table header
      1. Table type name
      2. Code page number
      3. Optional "NO_STRINGS"
   For table entry
      1. 0x?? token value
      2. token string (optional, extends from first nonspace to eol)
      3. null
 
  RETURN: 1 for success and 0 for EOF or failure
 ** ----------------------------------------------------------------------- **/
static int get_tuple(FILE *f, char item1[], char item2[], char item3[])
{
  char line[MAX_LINE];
  int i;
  int j;
  int len;

  if ((get_line(f, line, MAX_LINE)) != 1) {
    return 0;
  }

  len = (int)strlen(line);

  /* tablename or token hex value */
  j = 0;
  for (i = 0; i < len; i++) {
    if (isspace(line[i])) break;
    if (item1[0] == '0' && j == MAX_TOKEN) {
      /* have to check for leading '0' because this func is called to process
      TAG_TABLE_HEADING lines too where length could be longer than MAX_TOKEN */
      fprintf(stderr, "ERROR: token value too long near input file byte offset %ld\n",
              ftell(f));
      exit_error(1);
      break;
    }
    item1[j++] = line[i];
  }
  item1[j] = '\0';

  /* whitespace */
  for (; i < len; i++) {
    if (!isspace(line[i])) break;
  }

  /* codepage or token string */
  j = 0;
  for (/* continue with i */; i < len; i++) {
    if (item1[0] != '0' && isspace(line[i])) break;
    if (j == MAX_NAME) {
      fprintf(stderr, "ERROR: token string too long near input file byte offset %ld\n",
              ftell(f));
      exit_error(1);
      break;
    }
    item2[j++] = line[i];
  }
  item2[j] = '\0';

  /* whitespace */
  for (; i < len; i++) {
    if (!isspace(line[i])) break;
  }

  /* nostrings or nothing */
  j = 0;
  for (/* continue with i */; i < len; i++) {
    item3[j++] = line[i];
  }
  item3[j] = '\0';

  return 1;
}

static void process_header(FILE *f)
{
  if ((get_line(f, dict_name, MAX_DICT_NAME)) != 1) {
    fprintf(stderr, "ERROR: reading dictionary name\n");
    exit_error(1);
  }

  if ((get_line(f, public_id, MAX_PUBLIC_ID)) != 1) {
    fprintf(stderr, "ERROR: reading public id\n");
    exit_error(1);
  }

  if ((get_line(f, doc_type, MAX_DOC_TYPE)) != 1) {
    fprintf(stderr, "ERROR: reading doc type\n");
    exit_error(1);
  }
}

static void print_file_header(FILE *output, FILE *outputHeader)
{
  fprintf(output, "/*\n");
  fprintf(output, " * Dictionary = %s\n", dict_name);
  fprintf(output, " * Public id = %s\n", public_id);
  fprintf(output, " * Doc Type = %s\n", doc_type);
  fprintf(output, " */\n");
  fprintf(output, "\n");

  fprintf(output, "#include \"%s\"\n", main_argv[3]);
  fprintf(output, "#include \"xml\/cxml\/nw_wbxml_dictionary.h\"\n");

  fprintf(output, "\n");

  /* Including the typedef for public id */
  fprintf(outputHeader, "#define NW_%s_PublicId %s\n", dict_name, public_id);
}

static void print_table_header(FILE *output, char * table_type, char *type, char *key, char *code_page, int n)
{
  print_automaticallyGeneratedCodeWarning(output);
  fprintf(output, "\n\n");
  fprintf(output, "/*\n");
  fprintf(output, " * %s entries - sorted by %s\n", type, key);
  fprintf(output, " */\n");
  fprintf(output, "static const\n");
  fprintf(output, "%s NW_%s_%s_%s_%s[%d] = {\n", table_type, dict_name, type, key, code_page, n);
}

static void process_entry(int i, char *token, char *name) 
{
  tokens[i] = strdup(token);
  names[i] = strdup(name);
  sorted_indexes[i] = i;
}


/* ----------------------------------------------------------------------- **
   Print the table of tokens and names - sorted by token
   Also create a list of the items, sorted by name 
 ** ----------------------------------------------------------------------- **/
static void print_token_table(FILE *output, FILE* outputHeader,
                              char *type, char *key, char *code_page, int n,
                              int use_strings)
{
  static char tagTypeString[MAX_TYPE_NAME];
  static char tokenTypeString[MAX_TYPE_NAME];

  /* Must first sort by token */
  int i, j;
  char *tmp_token;
  char *tmp_name;
  char *tagType = &tagTypeString[0];

  if (strcmp("tag", type) == 0)
  {
    (void)sprintf(tagTypeString, ELEMENT_T, dict_name);
    (void)sprintf(tokenTypeString, ELEMENT_TOKEN_T, dict_name);
  }
  else if (strcmp("attribute", type) == 0)
  {
    (void)sprintf(tagTypeString, ATTRIBUTE_T, dict_name);
    (void)sprintf(tokenTypeString, ATTRIBUTE_TOKEN_T, dict_name);
  }
  else {
    fprintf(stderr, "ERROR: internal error\n");
    exit_error(1);
  }

  /* make all the tokens lower case */
  for (i=0; i < n; i++) {
    int l = (int)strlen(tokens[i]);
    if (l != 4) {
      fprintf(stderr, 
              "ERROR: Badly formatted token %s\n"
              "All token values must be in the form 0x?? (e.g., 0x07) "
              "or the sorting algorithm won't work.\n"
              , tokens[i]);
      exit_error(1);
    }
    for (j=0; j < l; j++) {
      if (isupper(tokens[i][j])) {
        tokens[i][j] = (char)tolower(tokens[i][j]);
      }
      if ((tokens[i][j] != 'x') && isalpha(tokens[i][j])) {
        if ((tokens[i][j] < 'a') || (tokens[i][j] > 'f')) {
          fprintf(stderr, 
                  "ERROR: Illegal hex digit in token %s\n"
                  , tokens[i]);
          exit_error(1);
        }
      }
    }
  }

  for (i=0; i < n-1; i++) {
    for (j=0; j < n-1; j++) {
      /* 
       * Sort by token 
       */
      if ((strcmp(tokens[j], tokens[j+1])) > 0) {
        /* Swap the two elements */
        tmp_token = tokens[j];
        tmp_name = names[j];
        tokens[j] = tokens[j+1];
        names[j] = names[j+1];
        tokens[j+1] = tmp_token;
        names[j+1] = tmp_name;
      }
    }
  }

  /*
   * print variables
   */
  fprintf(output, "\n");
  for (i=0; i < n; i++) {
    char tempName[MAX_NAME+1];
    int l;
    if (isdigit(names[i][0])) {
      tempName[0] = '_';
      strcpy(tempName+1, names[i]);
    } else {
      strcpy(tempName, names[i]);
    }
    l = (int)strlen(tempName);
    for (j = 0; j < l; j++) {
      if (isdigit(tempName[j])
          || isalpha(tempName[j])
          || (tempName[j] == '_')) {
        continue;
      }
      tempName[j] = '_';
    }
    if (use_strings) {
      fprintf(output, 
              "static const NW_Ucs2 %sTag_%s[] = {"
              , tagType, tempName);
      for (j=0; j < (int) strlen(names[i]); j++) {
        if (names[i][j] == '\\') {
          fprintf(output, "\'\\\\\',");
        } else {
          fprintf(output, "\'%c\',", names[i][j]);
        }
      }
      fprintf(output, "\'\\0\'};\n");
    }
  }
  if (!use_strings) {
    fprintf(output,
            "static const NW_Ucs2 %sTag_emptyString_%s[] = { \'\\0\' };\n"
            , tagType, code_page);
  }
  fprintf(output, "\n");

  print_table_header(output, TOKEN_TABLE_T, type, key, code_page, n);
  if (use_strings) {
    fprintf(outputHeader, 
            "\ntypedef enum %sToken_%s_e{\n", 
            tagType, code_page);
  }
 /*
  * Print the table
  */
  for (i=0; i < n; i++) {
    char tempName[MAX_NAME+1] ;
    char tempToken[6] ;
    char *token;
    int l;
    if (isdigit(names[i][0])) {
      tempName[0] = '_';
      strcpy(tempName+1, names[i]);
    } else {
      strcpy(tempName, names[i]);
    }
    l = (int)strlen(tempName);
    for (j = 0; j < l; j++) {
      if (isdigit(tempName[j])
          || isalpha(tempName[j])
          || (tempName[j] == '_')) {
        continue;
      }
      tempName[j] = '_';
    }
    strcpy(tempToken, tokens[i]);
    token = strchr(tempToken, 'x');
    token++;
    if (use_strings) {
      fprintf(output, "\t{%s, (%s *) %sTag_%s", tokens[i], NAMES_T, tagType, tempName);
      fprintf(outputHeader, "\t%s_%s = 0x0%s%s", tokenTypeString, tempName, code_page, token);
    } else {
      fprintf(output, "\t{%s, &%sTag_emptyString_%s", tokens[i], tagType, code_page);
    }
    
    if (i == (n-1))
    {
      fprintf(output, "}\n");
      if (use_strings) {
        fprintf(outputHeader, "\n");
      }
    }
    else
    {
      fprintf(output, "},\n");
      if (use_strings) {
        fprintf(outputHeader, ",\n");
      }
    }
  }
  fprintf(output, "};\n\n");
  if (use_strings) {
    fprintf(outputHeader, "}%sToken_%s_t;\n\n", tagType, code_page);
  }

  if (use_strings) {
    /*
    * Create an array of the names sorted by index
    */
    for (i=0; i < n-1; i++) {
      for (j=0; j < n-1; j++) {
        /*
        * Since we will need an array of the names sorted by index,
        * generate that arrary now.
        */
        if ((strcmp(names[j], names[j+1])) > 0) {
          /* Swap the two names */
          int tmp_token;
          tmp_name = names[j];
          tmp_token = sorted_indexes[j];
          names[j] = names[j+1];
          names[j+1] = tmp_name;
          sorted_indexes[j] = sorted_indexes[j+1];
          sorted_indexes[j+1] = tmp_token;
        }
      }
    }
  }
}

static void cache_codepage(codepage_t table[], char *cp_num, int n)
{
  int num = atoi(cp_num);

  if (num >= TABLE_SIZE) {
    fprintf(stderr, "ERROR: Codepage '%d' is too large!\n", num);
    exit(1);
  }

  table[num].num = num; 
  table[num].size = n; 
}

/*
 * Print a table of the names 
 */
static void print_name_table(FILE *output, char *type, char *key, char *code_page, int n,
                             int use_strings)
{
  int i;

  print_table_header(output, NAME_TABLE_T, type, key, code_page, n);

  for (i=0; i < n; i++) {
    if (use_strings) {
      fprintf(output, "\t%d,\n", sorted_indexes[i]);
    } else {
      fprintf(output, "\t0,\n");
    }
  }

  fprintf(output, "};\n");
}

static process_content(FILE *f, FILE *output, FILE* outputHeader)
{
  char token[MAX_START_NAME+1]; /* Must be big enough to hold a 
                                   tag/attr start string */
  char name[MAX_NAME+1];
  char optional[MAX_LINE+1];
  char *tag_code_page = "";
  char *attr_code_page = "";
  char processing_tag = 2;  /* processing state: 1 = tag, 0 = attribute, 2 = init */
  int n = 0;
  int use_strings = 1;

  for(;;) {

    if ((get_tuple(f, token, name, optional)) != 1) {
      break;
    }

    if (!strcmp(token, TAG_TABLE_HEADING)) {
      use_strings = strcmp(optional, NO_STRINGS);
      if (processing_tag == 1) {
        /* Process the current tag table */
        print_token_table(output, outputHeader, TAG_NAME, "token", tag_code_page, n, use_strings);
        print_name_table(output, TAG_NAME, "name", tag_code_page, n, use_strings);
        cache_codepage(tag_codepages, tag_code_page, n);
        n = 0;
      } else if (processing_tag == 0) {
        /* Process the current attribute table */
        print_token_table(output, outputHeader, ATTR_NAME, "token", attr_code_page, n, use_strings);
        print_name_table(output, ATTR_NAME, "name", attr_code_page, n, use_strings);
        cache_codepage(attr_codepages, attr_code_page, n);
        n = 0;
      }
      tag_code_page = strdup(name);
      processing_tag = 1;
    }
    else if (!strcmp(token, ATTR_TABLE_HEADING)) {
      use_strings = strcmp(optional, NO_STRINGS);
      if (processing_tag == 1) {
        /* Process the current tag table */
        print_token_table(output, outputHeader, TAG_NAME, "token", tag_code_page, n, use_strings);
        print_name_table(output, TAG_NAME, "name", tag_code_page, n, use_strings);
        cache_codepage(tag_codepages, tag_code_page, n);
        n = 0;
      } else if (processing_tag == 0) {
        /* Process the current attribute table */
        print_token_table(output, outputHeader, ATTR_NAME, "token", attr_code_page, n, use_strings);
        print_name_table(output, ATTR_NAME, "name", attr_code_page, n, use_strings);
        cache_codepage(attr_codepages, attr_code_page, n);
        n = 0;
      }
      attr_code_page = strdup(name);
      processing_tag = 0;
    } else {
      process_entry(n, token, name);
      n++;
    }
  }

  if (processing_tag == 2) {
    fprintf(stderr, 
            "ERROR: Could not find tag or attribute table starts in file.\n"
            "       Input file syntax has changed.\n"
            "       See usage by executing this program with no arguments.\n");
    exit_error(1);
  }

  /*
   * If anything is left, process it
   */
  if (n > 0) {
    if (processing_tag == 1) {
      print_token_table(output, outputHeader, TAG_NAME, "token", tag_code_page, n, use_strings);
      print_name_table(output, TAG_NAME, "name", tag_code_page, n, use_strings);
      cache_codepage(tag_codepages, tag_code_page, n);
    } else if (processing_tag == 0) {
      print_token_table(output, outputHeader, ATTR_NAME, "token", attr_code_page, n, use_strings);
      print_name_table(output, ATTR_NAME, "name", attr_code_page, n, use_strings);
      cache_codepage(attr_codepages, attr_code_page, n);
    }
  }
}

static void get_num_codepages(codepage_t cp[], int *n, int *max)
{
  int i;
  /* Determine the number of codepages */
  for (i=0; i < TABLE_SIZE; i++) {
    if (cp[i].num != -1) {
      *max = i;
      (*n)++;
    }
  }
}

static void print_codepage_table(FILE *output, codepage_t cp[], int n, int max, char *type)
{
  int i;

  if (n == 0)
  return;

  fprintf(output, "static const\n");
  fprintf(output, "%s NW_%s_%s_codepages[%d] = {\n", CODEPAGE_TABLE_T, 
      dict_name, type, max + 1);

  for (i=0; i <= max && i < TABLE_SIZE; i++) {
    if (cp[i].num == -1) {
      fprintf(output, "\t{0, 0, 0},\n");
    } else {
      fprintf(output, "\t{%d, (%s*)&NW_%s_%s_token_%d[0], ", 
      cp[i].size, TOKEN_TABLE_T, dict_name, type, cp[i].num);
      fprintf(output, "(NW_Byte *)&NW_%s_%s_name_%d[0]},\n", 
      dict_name, type, cp[i].num);
    }
  }
  fprintf(output, "};\n");
}

static void print_codepage_tables(FILE *output) 
{
  int n=0, max=0;

  /* Tag tables */
  fprintf(output, 
          "\n"
          "/*\n"
          " * Tag codepage table\n"
          " */\n"
          );

  get_num_codepages(tag_codepages, &n, &max);
  print_codepage_table(output, tag_codepages, n, max, TAG_NAME);

  /* Attr tables */
  fprintf(output, 
          "\n"
          "/*\n"
          " * Attribute codepage table\n"
          " */\n"
          );

  n = max = 0;
  get_num_codepages(attr_codepages, &n, &max);
  print_codepage_table(output, attr_codepages, n, max, ATTR_NAME);
}

static void add_codepage(FILE *output, codepage_t cp[], char *type)
{
  int n=0, max=0;

  get_num_codepages(cp, &n, &max);

  if (n == 0)
    fprintf(output, "\t0, 0,\n");
  else
    fprintf(output, "\t%d, (%s*)&NW_%s_%s_codepages[0],\n", max + 1, CODEPAGE_TABLE_T, dict_name, type);
}

static void print_dictionary_table(FILE *output, FILE *outputHeader)
{
  int i;
  if (strlen(doc_type) > 0)
  {
    fprintf(output, "\nstatic const NW_Ucs2 NW_%s_docType[] = {", dict_name);
    for (i=0; i < (int) strlen(doc_type); i++)
      fprintf(output, "\'%c\',", doc_type[i]);
    fprintf(output, "\'\\0\'};\n");
  }

  fprintf(output, 
          "\n"
          "/*\n"
          " * Dictionary\n"
          " */\n"
          );

  fprintf(outputHeader, "extern const %s NW_%s_WBXMLDictionary\n",
          DICTIONARY_TABLE_T, dict_name);
  fprintf(output, "%s NW_%s_WBXMLDictionary = {\n", DICTIONARY_TABLE_T, dict_name);
  fprintf(output, "\tNW_%s_PublicId,\n", dict_name);

  /* Print the doc type as a UCS2 string */
  fprintf(output, "\t(%s *)NW_%s_docType,\n", "NW_Ucs2", dict_name);

  /* Add the tag and attribute code page */
  add_codepage(output, tag_codepages, TAG_NAME);
  add_codepage(output, attr_codepages, ATTR_NAME);

  fprintf(output, "};\n");
}

static void process_file(FILE *f, FILE *output, FILE* outputHeader)
{
  process_header(f);

  print_file_header(output, outputHeader);

  process_content(f, output, outputHeader);

  print_codepage_tables(output);

  print_dictionary_table(output, outputHeader);
}

int main(int argc, char ** argv){
  if (argc < 4) {
    print_usage(argv[0]);
    exit_error(1);
  }

  init();

  input_dict = NULL;
  output_c = NULL;
  output_h = NULL;

  /* ----------------------------------------------------------------------- **
     Save info used in print_automaticallyGeneratedCodeWarning()   
  ** ----------------------------------------------------------------------- **/
  (void)time(&timestamp);
  main_argc = argc;
  main_argv = argv;


  /* ----------------------------------------------------------------------- **
     Work on files in text mode to ease end-of-line processing in DOS.  
  ** ----------------------------------------------------------------------- **/
  if ((input_dict = open_file(argv[1], "rt")) == NULL) {
    fprintf(stderr, "ERROR: Input File '%s' could NOT be opened!\n", argv[1]);
    exit_error(1);
  }

  if ((output_c = open_file(argv[2], "wt")) == NULL) {
    fprintf(stderr, "ERROR: Output .c File '%s' could NOT be opened!\n", argv[2]);
    exit_error(1);
  }

  if ((output_h = open_file(argv[3], "wt")) == NULL) {
    fprintf(stderr, "ERROR: Output .h File '%s' could NOT be opened!\n", argv[3]);
    exit_error(1);
  }

  print_copyright(output_c);
  fprintf(output_c, "\n\n");
  print_automaticallyGeneratedCodeWarning(output_c);
  fprintf(output_c, "\n\n");

  print_copyright(output_h);
  fprintf(output_h, "\n\n");
  print_automaticallyGeneratedCodeWarning(output_h);
  fprintf(output_h, "\n\n");

  {
    int l;
    char* p = strrchr(argv[3], '.');
    l = (p == NULL) ? (int)strlen(argv[3]) : p - argv[3];

    fprintf(output_h, "#ifndef HEADER_GUARD_%.*s", l, argv[3]);
    if (p != NULL) fprintf(output_h, "_%s", p+1);
    fprintf(output_h,
            "\n"
            "#define HEADER_GUARD_%.*s"
            , l, argv[3]);
    if (p != NULL) fprintf(output_h, "_%s", p+1);
    fprintf(output_h, 
            "\n"
            "\n"
            "#ifdef __cplusplus\n"
            "extern \"C\"\n"
            "{\n"
            "#endif\n"
            "\n"
            "\n"
            );
  }

  /* process input */
  process_file(input_dict, output_c, output_h);

  fprintf(output_c, "\n");
  print_automaticallyGeneratedCodeWarning(output_c);

  fprintf(output_c, "\n");
  fprintf(output_h, "\n");
  print_automaticallyGeneratedCodeWarning(output_h);

  fprintf(output_h,
          "\n"
          "\n"
          "#ifdef __cplusplus\n"
          "} /* extern \"C\" */\n"
          "#endif\n"
          "\n"
          "#endif\n"
          );
  
  /* close files */
  fclose(input_dict);
  fclose(output_c);
  fclose(output_h);

  return 0;
}
