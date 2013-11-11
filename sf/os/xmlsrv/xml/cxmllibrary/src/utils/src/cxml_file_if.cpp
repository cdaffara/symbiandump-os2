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



#include <f32file.h>
#include <xml/cxml/cxml_file_if.h>
#include <xml/cxml/nw_string_char.h>


//
//CXML_Read_From_File()
//

EXPORT_C
NW_Status_t CXML_Read_From_File(
				   NW_Byte* input_file,NW_Uint8** buffer, NW_Int32* bufferLen)
{
  RFile file;
  RFs aSession;
  TInt fileErr;
  CXML_Uint32 fileNameByteCnt;
  TInt fileNameLen; 
  NW_Status_t retVal =  NW_STAT_SUCCESS;
  TUint16* fileName;
  TInt i;
  TInt err = KErrNone;
  HBufC8*  inBuf;
  CXML_Uint8* bufPtr;
  TPtr8 inBufDes(NULL,0);


  if( (input_file == NULL) || (buffer == NULL) || (bufferLen == NULL) )
  {
	retVal = NW_STAT_FAILURE;
	return retVal;
  }

  *bufferLen = -1; //Default value

  err = aSession.Connect();
  if(KErrNone != err)
  {
	retVal = NW_STAT_FAILURE;
	return retVal;  	
  }
  fileNameLen = NW_String_charBuffGetLength(input_file,HTTP_us_ascii,&fileNameByteCnt);

  
  //This is required for TPtrC

  fileName = new TUint16[fileNameLen];

  if(fileName == NULL)
  {
   retVal = NW_STAT_OUT_OF_MEMORY;
   return retVal;
  }


  for(i=0; i < fileNameLen; i++)
  {
	  fileName[i] = input_file[i];
  }

  TPtrC inFileN(fileName,fileNameLen);

  fileErr = file.Open(aSession,inFileN,EFileRead);

  if ( fileErr != KErrNone )	
  {
    retVal = NW_STAT_FILE_NOT_OPEN; 
	delete[] fileName;
	return retVal;
  }
  

    TInt fileSize;
	
  //Get File Size

  fileErr = file.Size( fileSize );

  if(fileErr != KErrNone )	
  {
    retVal = NW_STAT_UNEXPECTED_ERROR;
	delete[] fileName;
	file.Close();
	return retVal;
  }

  *bufferLen = fileSize;

  // Allocate buffer to read the cXML FILE contents

   inBuf = HBufC8::New(fileSize);

   //Get the pointer to heap object

   inBufDes.Set(inBuf->Des()) ;

  if(inBuf)
  {
	  
	  //Now read the file

      fileErr = file.Read(inBufDes);

	  
     if(fileErr != KErrNone )	
	 {
      delete inBuf;  //TODO: Change to more meaningful error
	  delete[] fileName;
	  file.Close();
	  *bufferLen = -1;
	  retVal = NW_STAT_FILE_READ_ERROR;
	  return retVal;	
	 }
  }
  else
  {
	delete[] fileName;
	file.Close();
	*bufferLen = -1;
	retVal = NW_STAT_FAILURE;
	return retVal;
  }

  
  //Allocate the buffer and copy the contents to this

  *buffer = new CXML_Byte[fileSize];


  if(*buffer == NULL)
  {
   retVal = NW_STAT_OUT_OF_MEMORY;
   return retVal;
  }


  bufPtr = (CXML_Byte*) inBuf->Ptr(); //Get pointer to the data

   for(i=0; i < fileSize; i++)
	{
	  (*buffer)[i] = bufPtr[i];
	}

  delete inBuf;
  delete[] fileName;
  file.Close();
  aSession.Close();
  return retVal;
 
}//end CXMLTestParser::CXML_Read_From_File()
