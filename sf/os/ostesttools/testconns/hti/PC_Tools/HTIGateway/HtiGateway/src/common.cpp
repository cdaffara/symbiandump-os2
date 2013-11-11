/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description:
*     This file contains implementation file of the Data and ParameterList classes.
*/

// INCLUDES
#include "common.h"
#include "util.h"
#include <crtdbg.h>

// Maximum time to wait
long g_MaximumShutdownWaitTime;

DWORD g_ErrorCode = NO_ERRORS;

//**********************************************************************************
// Class Data
//
// This class provides data encapsulattion for DataGateway
//**********************************************************************************

Data::Data(const void* data_in, DWORD length, DataType type)
{
	////_RPT1(_CRT_WARN,"Data::Data %x\n", data_in);
	m_pData = NULL;
	SetData(data_in, length, 0, type);
}


Data::Data(const void* data_in, DWORD length, DWORD offset, DataType type)
{
	////_RPT1(_CRT_WARN,"Data::Data %x\n", data_in);
	m_pData = NULL;
	SetData(data_in, length, offset, type);
}

Data::Data(const Data& data)
{
	m_pData = NULL;
	SetData(data.GetData(), data.GetLength(), 0, data.GetType());
}

Data::~Data()
{
	if (m_pData != NULL)
	{
		//_RPT1(_CRT_WARN,"~Data %x\n", m_pData);
		free( m_pData );
		m_pData = NULL;
	}
}

/*
 * This method is used to free data and set data type to EEmpty
 */
void Data::FreeData()
{
	if (m_pData != NULL)
	{
		//_RPT1(_CRT_WARN,"FreeData %x\n", m_pData);
		free( m_pData );
		m_pData = NULL;
		m_Type = EEmpty;
	}
}
/*
 * returns data in Data object given parameter
 */
void* Data::GetData(void *data_out) const
{
	if (data_out != NULL)
	{
		data_out = m_pData;
	}
	return m_pData;
}

/*
 * returns length of data 
 */
DWORD Data::GetLength() const
{
	return m_dwLength;
}

/*
 * returns type of data
 * can be one of the following: EEmpty, EControl, EError, EData
 */
Data::DataType Data::GetType() const
{
	return m_Type;
}

Data& Data::operator=(const Data& data)
{
	if ( this != &data )
	{
		FreeData();
		SetData( data.GetData(), data.GetLength(), data.GetType() );
	}
	return *this;
}

/*
 * This method copies up to length bytes from given data object to class member data object
 */
bool Data::SetData(const void* data_in, DWORD length, DataType type)
{
	////_RPT1(_CRT_WARN,"Data::SetData %x\n", data_in);
	return SetData(data_in, length, 0, type);
}

/*
 * This method copies up to length bytes(starting from offset) from given data object to class member data object
 */
bool Data::SetData(const void* data_in, DWORD length, DWORD offset, DataType type)
{
	//_RPT1(_CRT_WARN,"Data::SetData %x\n", data_in);
	m_Type = type;
	if (length == 0 || data_in == NULL || offset >= length)
	{
		m_pData = NULL;
		m_dwLength = 0;
		return true;
	}
	m_dwLength = (length - offset);
	//_ASSERTE(m_pData==NULL);
	m_pData = (BYTE*)malloc( m_dwLength );
	//_RPT2(_CRT_WARN,"Data::SetData m_pData %x <%d>\n", m_pData, m_dwLength);
	if (m_pData == NULL)
	{
		Util::Error("Error allocating memory to data object");
		m_Type = EEmpty;
		return false;
	}
	memset(m_pData, 0, m_dwLength);
	memcpy(m_pData, &reinterpret_cast<const char*>(data_in)[offset], m_dwLength);
	return false;
}

/*
 * Used to combine different messages into one.
 * This method appends new data at the end of already excisting data.
 * If new data isn't same type as excisting data it isn't appended.
 * Returns bool value of whether the method has succeeded to append the data or not.
 */
bool Data::AppendData(Data* aData)
{
	if ( m_Type == aData->m_Type )
	{
		//realloc memory
		BYTE* newBlock = (BYTE*) realloc( m_pData, m_dwLength + aData->m_dwLength );
		if ( newBlock != NULL )
		{
			m_pData = newBlock;
			//copy data
			memcpy( ((BYTE*)m_pData) + m_dwLength,
					aData->GetData(),
					aData->GetLength() );
			//update length
			m_dwLength += aData->GetLength();
			return true;
		}
	}
	return false;
}

//**********************************************************************************
// Class ParameterList
//
//**********************************************************************************
ParameterList::ParameterList()
{
}

ParameterList::~ParameterList()
{
	m_Map.clear();
}

void ParameterList::Add(const string& key, const void* value)
{
	m_Map[key] = value;
}

const void* ParameterList::Get(const string& key)
{
	return m_Map[key];
}

// End of the file
