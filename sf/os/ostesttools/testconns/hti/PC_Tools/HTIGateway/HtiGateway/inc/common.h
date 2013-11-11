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
*     This file contains the headers of the Data and ParameterList classes.
*/

#ifndef COMMON_H
#define COMMON_H

#pragma warning ( disable : 4786 )

#include <windows.h>

//**********************************************************************************
// Class Data
// 
// This class provides data encapsulattion for DataGateway
//**********************************************************************************

class Data
{
public:
	//type of data
	enum DataType { EEmpty = 0, EControl, EError, EData };
public:
	Data(const void* data_in = NULL, DWORD length = 0, DataType type = EEmpty);
	Data(const void* data_in, DWORD length, DWORD offset, DataType type);
	Data(const Data& d);
	virtual ~Data();
	/*
	 * returns data in Data object given parameter
	 */
	virtual void* GetData(void* data_out = NULL) const;
	/*
	 * returns length of data 
	 */	
	virtual DWORD GetLength() const;
	/*
	 * returns type of data
	 * can be one of the following: EEmpty, EControl, EError, EData
	 */	
	DataType GetType() const;
	/*
	 * This method copies up to length bytes from given data object to class member data object
	 */
	bool SetData(const void* data_in, DWORD length, DataType type);
	/*
	 * This method copies up to length bytes(starting from offset) from given data object to class member data object
	 */	
	bool SetData(const void* data_in, DWORD length, DWORD offset, DataType type);
	Data& operator=(const Data& data);
	/*
	 * Used to combine different messages into one.
	 * This method appends new data at the end of already excisting data.
	 * If new data isn't same type as excisting data it isn't appended.
	 * Returns bool value of whether the method has succeeded to append the data or not.
	 */
	virtual bool AppendData(Data* aData);
	/*
	 * This method is used to free data and set data type to EEmpty
	 */
	void FreeData();
private:
	//actual data
	void* m_pData;
	//length of data
	DWORD m_dwLength;
	//type of data
	DataType m_Type;
};

const BYTE ControlPhonePowered = 0x01;

const Data CtrlMessagePhonePowered((void*)&ControlPhonePowered, 1, 0, Data::EControl);

const BYTE ErrorSendingData = 0x81;

const Data ErrorMessageSendingData((void*)&ErrorSendingData, 1, 0, Data::EError);

#define CREATE_CONTROL_MESSAGE(str) new Data( (str), strlen(str)+1, Data::EControl)

//**********************************************************************************
// Class ParameterList
//
//**********************************************************************************

#include <map>
#include <string>

using namespace std;

class ParameterList
{
public:
	void Add(const string& key, const void* value);
	const void* Get(const string& key);
	ParameterList();
	~ParameterList();
private:
	map<const string,const void*> m_Map;
};

#endif

// End of the file