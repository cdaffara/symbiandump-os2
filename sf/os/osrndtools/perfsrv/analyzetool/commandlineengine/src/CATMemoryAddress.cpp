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
* Description:  Class representing a memory address and its details.
*
*/


#include "../inc/CATMemoryAddress.h"
#include "../inc/CATBase.h"

// -----------------------------------------------------------------------------
// CATMemoryAddress::CATMemoryAddress
// Constructor
// -----------------------------------------------------------------------------
CATMemoryAddress::CATMemoryAddress(string& sAddress, unsigned long iOffSet)
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::CATMemoryAddress");
	m_sAddress = sAddress;
	m_sFileName = "";
	m_sFunctionName = "";
	m_sModuleName = "";
	m_iAddress = CATBase::_httoi( sAddress.c_str() );
	m_iDllLoadinfoIndex = -1;
	m_iModuleStartAddress = 0;
	m_iOffSetFromModuleStart = 0;
	m_iExactLineNumber = -1;
	m_iFunctionLineNumber = -1;
	m_ePinPointState = OUT_OF_PROCESS;
	m_iOffSet = iOffSet;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::~CATMemoryAddress
// Destructor.
// -----------------------------------------------------------------------------
CATMemoryAddress::~CATMemoryAddress()
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::~CATMemoryAddress");
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::FindSetModuleName
// Find which binary this address belongs to.
// Sets also the offsetfrommodulestart.
// -----------------------------------------------------------------------------
bool CATMemoryAddress::FindSetModuleName(vector<DLL_LOAD_INFO>* vDlls)
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::FindSetModuleName");
	vector<DLL_LOAD_INFO>::iterator it;
	for (  it = vDlls->begin() ;
		it != vDlls->end(); it++ )
	{
		// Is modules load time defined?
		if ( (*it).iLoadTime > 0 )
		{
			// Check that load time is earlier or same as allocation
			if ( m_iTime >= (*it).iLoadTime
				&& m_iAddress >= (*it).iStartAddress 
				&& m_iAddress < (*it).iEndAddress )
			{
				// Module is loaded until process end.
				if ( (*it).iUnloadTime == 0 )
					break;
				// Check is allocation done before module was unloaded.
				else if ( (*it).iUnloadTime >= m_iTime )
					break;
			}
		}
		// Module has no time defined use only code segments.
		else
		{
			if ( m_iAddress >= (*it).iStartAddress 
				&& m_iAddress < (*it).iEndAddress )
				break;
		}
	}

	// Did we not find module where address is?
	if ( it == vDlls->end() )
		return false;

	m_ePinPointState = OUT_OF_RANGE;
	m_sModuleName = (*it).sModuleName;
	m_iModuleStartAddress = (*it).iStartAddress;
	m_iOffSetFromModuleStart = m_iAddress - m_iModuleStartAddress;
	m_iOffSetFromModuleStart += m_iOffSet;
	m_iDllLoadinfoIndex = distance( vDlls->begin(), it ) ;
	return true;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::SetModuleStartAddress
// Set start address of the binary in which address resides.
// Note, this also sets the offset from start value.
// -----------------------------------------------------------------------------
void CATMemoryAddress::SetModuleStartAddress(unsigned long iAddress)
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::SetModuleStartAddress");
	m_iModuleStartAddress = iAddress;
	m_iOffSetFromModuleStart = m_iAddress - m_iModuleStartAddress;
	m_iOffSetFromModuleStart += m_iOffSet;
}


// -----------------------------------------------------------------------------
// CATMemoryAddress::GetOffSetFromModuleStart
// Note return value includes the set offset.
// So this value is not binary start - address.
// Instead it is.
// memory address - binary start address + offset
// -----------------------------------------------------------------------------
unsigned long CATMemoryAddress::GetOffSetFromModuleStart()
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::GetOffSetFromModuleStart");
	return m_iOffSetFromModuleStart;
}

int CATMemoryAddress::GetDllLoadInfoIndex()
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::GetDllLoadInfo");
	return m_iDllLoadinfoIndex;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::SetTime
// -----------------------------------------------------------------------------
void CATMemoryAddress::SetTime( unsigned long long& ullTime )
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::SetTime");
	m_iTime = ullTime;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::GetTIme
// -----------------------------------------------------------------------------
unsigned long long CATMemoryAddress::GetTime()
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::GetTime");
	return m_iTime;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::SetAddress
// -----------------------------------------------------------------------------
void CATMemoryAddress::SetAddress(string& sAddress)
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::SetAddress");
	m_sAddress = sAddress;
	m_iAddress = CATBase::_httoi( sAddress.c_str() );
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::GetAddressString
// -----------------------------------------------------------------------------
string CATMemoryAddress::GetAddressString()
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::GetAddressString");
	return m_sAddress;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::SetAddress
// -----------------------------------------------------------------------------
void CATMemoryAddress::SetAddress(unsigned long iAddress)
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::SetAddress");
	m_iAddress = iAddress;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::GetAddress
// -----------------------------------------------------------------------------
unsigned long CATMemoryAddress::GetAddress()
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::GetAddress");
	return m_iAddress;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::SetModuleName
// -----------------------------------------------------------------------------
void CATMemoryAddress::SetModuleName(string& sModuleName)
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::SetModuleName");
	m_sModuleName = sModuleName;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::GetModuleName
// -----------------------------------------------------------------------------
string CATMemoryAddress::GetModuleName()
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::GetModuleName");
	return m_sModuleName;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::SetAddressToLineState
// -----------------------------------------------------------------------------
void CATMemoryAddress::SetAddressToLineState( ADDRESS_TO_LINE_STATE eState )
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::SetAddressToLineState");
	m_ePinPointState = eState;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::GetAddressToLineState
// -----------------------------------------------------------------------------
int CATMemoryAddress::GetAddressToLineState()
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::GetAddressToLineState");
	return m_ePinPointState;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::SetFileName
// -----------------------------------------------------------------------------
void CATMemoryAddress::SetFileName(string& sFileName)
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::SetFileName");
	m_sFileName = sFileName;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::GetFileName
// -----------------------------------------------------------------------------
string CATMemoryAddress::GetFileName()
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::GetFileName");
	return m_sFileName;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::SetFunctionName
// -----------------------------------------------------------------------------
void CATMemoryAddress::SetFunctionName(string& sFunctionName)
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::SetFunctionName");
	m_sFunctionName = sFunctionName;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::GetFunctionName
// -----------------------------------------------------------------------------
string CATMemoryAddress::GetFunctionName()
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::GetFunctionName");
	return m_sFunctionName;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::SetFunctionLineNumber
// -----------------------------------------------------------------------------
void CATMemoryAddress::SetFunctionLineNumber(int iFunctionLineNumber)
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::SetFunctionLineNumber");
	m_iFunctionLineNumber = iFunctionLineNumber;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::GetFunctionLineNumber
// -----------------------------------------------------------------------------
int CATMemoryAddress::GetFunctionLineNumber()
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::GetFunctionLineNumber");
	return m_iFunctionLineNumber;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::SetExactLineNumber
// -----------------------------------------------------------------------------
void CATMemoryAddress::SetExactLineNumber(int iExactLineNumber)
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::SetExactLineNumber");
	m_iExactLineNumber = iExactLineNumber;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::GetExactLineNumber
// -----------------------------------------------------------------------------
int CATMemoryAddress::GetExactLineNumber()
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::GetExactLineNumber");
	return m_iExactLineNumber;
}

// -----------------------------------------------------------------------------
// CATMemoryAddress::GetModuleStartAddress
// -----------------------------------------------------------------------------
unsigned long CATMemoryAddress::GetModuleStartAddress() const
{
	LOG_LOW_FUNC_ENTRY("CATMemoryAddress::GetModuleStartAddress");
	return m_iModuleStartAddress;
}

//EOF
