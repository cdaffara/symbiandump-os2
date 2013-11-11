// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#include "inputdata.h"

#if defined(__LIGHTLOGGER_ENABLED)
#include "lightlogger.h" 
#endif

/**Default constructor.
*/
EXPORT_C CInputData::CInputData(unsigned int aMinPackageSize)
{
	iMinPackageSize = aMinPackageSize;
}

/**Destructor.
*/
EXPORT_C CInputData::~CInputData()
{
}

/**Function creates new control data chunk (with EOL).

@param aCommand A pointer to command literal.
@param aCommandSize Size of data poitned by aCommand.
@return Pointer to ControlData type is returned when chunk was successfully created, otherwise NULL.
*/
EXPORT_C ControlData* CInputData::CreatePackage(void* aCommand, signed long aCommandSize)
{
	unsigned long trueSize = aCommandSize+SIZE_OF_EOL; 
	unsigned long size = trueSize;
	if(size<iMinPackageSize)
		size = iMinPackageSize;

	//create package
	ControlData *package = new ControlData[size]; //minimum package
	if(!package)
		return NULL;
	
	//add command 
	memcpy((void*)package, aCommand, aCommandSize);
	
	//apend padding character if necessary (between last string and EOL)
	long padd = 0;
	if(trueSize < iMinPackageSize)
	{
	padd = iMinPackageSize-trueSize;
		for(int i=0; i<padd; i++) //skip separator
			memcpy((void*)(package+aCommandSize+i), (void*)&DATA_SEPARATOR, 1);
	}
	
	//add EOL
	memcpy((void*)(package+aCommandSize+padd), (void*)&CD_EOL, SIZE_OF_EOL);
	
	return package;
}

//only for internal use
unsigned long CInputData::FindAvailablePlace(const ControlData *aDataPtr)
{
	unsigned long currentSize = this->GetSize(aDataPtr);
	unsigned long endPtr = 0;
	
	while(true)
	{
		if((*(aDataPtr+endPtr))==CD_CR)
			break;
		
		if((*(aDataPtr+endPtr)) == DATA_SEPARATOR)
			if(((*(aDataPtr+endPtr+1)) == CD_CR) || ((*(aDataPtr+endPtr+1)) == DATA_SEPARATOR))
				break; //offset
		++endPtr;
	}
	
	return endPtr+1;
}

//only for internal use
unsigned long CInputData::CalcNumberOfPaddingChars(const ControlData *aDataPtr)
{
	unsigned long padd = 0;
	int i=1;
	
	while(true)
	{
		if((*(aDataPtr+i))==DATA_SEPARATOR)
			if((*(aDataPtr+i+1))==DATA_SEPARATOR)
				++padd;
		if((*(aDataPtr+i))==CD_CR)
			break;
		++i;
	}
	
	return padd;
}

/**Function returns size of control data chunk given as an argument.
aDataPtr must be ended with EOL (end if line) sequence.

@param aDataPtr A poiter to existing control data.
@return Unsigned long number describing current size of control data.
*/
EXPORT_C unsigned long CInputData::GetSize(const ControlData *aDataPtr)
{
	long i=0;
	while(true)
		{
		if((*(aDataPtr+i)) == CD_CR)
			if((*(aDataPtr+i+1)) == CD_LF)
				return i+SIZE_OF_EOL;
		++i;
		}
}


/**Function returns number of chunks in current control data package.
 
@param aDataPtr A poiter to existing control data chunk.
@return Number of chunks in current control data.
*/
EXPORT_C unsigned long CInputData::GetChunksCount(const ControlData *aDataPtr)
{
	unsigned long counter = 0;
	unsigned long size = this->GetSize(aDataPtr);
	unsigned long tmp = 0;
	
	while(tmp < size-1)
	{
		if( (*(aDataPtr+tmp) != DATA_SEPARATOR) && (*(aDataPtr+tmp) != CD_CR) && (*(aDataPtr+tmp) != CD_LF) )
			if( (*(aDataPtr+tmp+1) == DATA_SEPARATOR) || (*(aDataPtr+tmp+1) == CD_CR) || (*(aDataPtr+tmp+1) == CD_LF) )
				++counter;
		++tmp;
	}	
	return counter;
}


/**Function parses control data chunk and returns pointer to data chunk described in aChunkNumber argument and update 
aChunkSize argument passed as a reference.
 
@param aDataPtr A poiter to existing control data.
@param aChunkNumber Number of desired data chunk.
@param aChunkSize Reference to unsigned long variable, where size of returned chunk will be written.
@return A pointer to data or NULL.
*/
EXPORT_C const void* CInputData::GetChunk(const ControlData* aDataPtr, unsigned long aChunkNumber, unsigned long &aChunkSize)
{
	aChunkSize = 0;
	unsigned long counter = 0;
	unsigned long size = this->GetSize(aDataPtr);
	unsigned long tmp = 0;
	
	while(tmp < size)
	{
		if(counter == aChunkNumber)
		{
			unsigned long chunkStart = tmp;
			while((*(aDataPtr+tmp)!=DATA_SEPARATOR) && (*(aDataPtr+tmp)!=CD_CR))
				{++tmp;}
			aChunkSize = tmp-chunkStart;
			const void* ret = aDataPtr+chunkStart;
			return ret;
		}
		else if(*(aDataPtr+tmp)==DATA_SEPARATOR)
				++counter;
		++tmp;
	}
	
	return NULL;
}

/**Functions appends data to existing control data. All operations, like updating size and appending separators, are done internally.

@param aDataPtr A reference to poiter to existing control data.
@param aAddData A pointer to data that should be appended to current payload.
@param aAddDataSize A size of data that should be appended.
@return A new size of control data chunk (size after 'append' operation).
*/
EXPORT_C unsigned long CInputData::AppendNewData(ControlData *&aDataPtr, const void *aAddData, unsigned long aAddDataSize)
{	
	unsigned long currentSize = this->GetSize(aDataPtr); //with EOL
	unsigned long nextPayOffest = this->FindAvailablePlace(aDataPtr);
	unsigned long paddChars = this->CalcNumberOfPaddingChars(aDataPtr);
	
	//calculate new size
	unsigned long newSize = 0;
	if(paddChars >= aAddDataSize)
		newSize = currentSize;	
		else
			newSize = currentSize+aAddDataSize+1; //1 char for separator

	//reallocate 
	ControlData *newPtr = new ControlData[newSize];
	if(newPtr == NULL)
		{
		delete [] aDataPtr;
		return NULL;
		}

	//fill with padding characters
	for(int i=0; i<newSize; i++)
		memcpy((void*)(newPtr+i), (void*)&DATA_SEPARATOR, 1);
	
	//copy old content to new location
	memcpy((void*)newPtr, (void*)aDataPtr, currentSize-SIZE_OF_EOL); //except EOL
	delete [] aDataPtr;
	
	//add new Data
	memcpy((void*)(newPtr+nextPayOffest), (void*)aAddData, aAddDataSize);
	
	//add EOL (end of package indicator)
	memcpy((void*)(newPtr+newSize-SIZE_OF_EOL), (void*)&CD_EOL, SIZE_OF_EOL);

	//assign new data
	aDataPtr = newPtr;
	
	return newSize;
}


