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
*
*/
#include "../inc/ATCommonDefines.h"
#include "../inc/catallocs.h"

void CATAllocs::Alloc( const string& sAllocString )
{
	LOG_LOW_FUNC_ENTRY("CATAllocs::Alloc");
	string sAlloc( sAllocString );
	CATAlloc alloc;
	string sAddress = GetStringUntilNextSpace( sAlloc, true );
	alloc.m_sTime = GetStringUntilNextSpace( sAlloc, true );
	alloc.m_sSize = GetStringUntilNextSpace( sAlloc, true );
	alloc.m_vCallStack.insert( pair<unsigned long,string>(1, sAlloc) );
	// Add allocation
	pair< map<string,CATAlloc>::iterator, bool> ret;
	ret = m_vAllocs.insert( pair<string, CATAlloc>( sAddress, alloc ) );
	if( ret.second == false )
	{
		// Allocation to this memory address was already added.
		LOG_STRING( "CATAllocs: Allocating same address again, address: " << sAddress );
	}
}

void CATAllocs::AllocH( const string& sAllocHString, const string& aTimeString )
{
	LOG_LOW_FUNC_ENTRY("CATAllocs::AllocH");
	string sAllocH( sAllocHString );
	// Parse alloc & create new allocation.
	CATAlloc alloc;
	alloc.m_sTime = aTimeString;
	string sAddress = GetStringUntilNextSpace( sAllocH, true );
	alloc.m_sSize = GetStringUntilNextSpace( sAllocH, true );
	alloc.m_iThreadId = GetStringUntilNextSpace ( sAllocH, true );
	alloc.m_iCSCount = _httoi( string( GetStringUntilNextSpace( sAllocH, true ) ).c_str() );
	// Insert call stack fragment as "first" 1 because we are header.
	if ( alloc.m_iCSCount > 0 )
		alloc.m_vCallStack.insert( pair<unsigned long,string>( 0, sAllocH ) );
	// Add allocation
	pair< map<string,CATAlloc>::iterator, bool> ret;
	ret = m_vAllocs.insert( pair<string, CATAlloc>( sAddress, alloc ) );
	if( ret.second == false )
	{
		// Allocation to this memory address was already added.
		LOG_STRING( "CATAllocs: Allocating same address again, address: " << sAddress );
	}
}

void CATAllocs::AllocF( const string& sAllocFString, const string& aTimeString )
{
	LOG_LOW_FUNC_ENTRY("CATAllocs::AllocF");
	string sAllocF( sAllocFString );
	string sAddress = GetStringUntilNextSpace( sAllocF, true );
	string sTime = aTimeString;
	unsigned long iNumber = _httoi( string( GetStringUntilNextSpace( sAllocF, true ) ).c_str() );
	string sCallSstack = sAllocF;
	// Find correct allocation into which add call stack fragment.
	map<string, CATAlloc>::iterator it;
	it = m_vAllocs.find( sAddress );
	// TODO: If cannot find, create new in cache.
	if ( it == m_vAllocs.end() )
	{
		LOG_STRING( "CATAllocs: Allocate fragment without header: " << sAddress );
		return;
	}
	pair< map<unsigned long,string>::iterator, bool> ret;
	// Add call stack to it.
	ret = it->second.m_vCallStack.insert( pair<unsigned long,string>( iNumber, sCallSstack ) );
	if( ret.second == false )
	{
		LOG_STRING( "CATAllocs: Same allocation fragment again: " << sAddress );
	}
}

// ReallocH.
void CATAllocs::ReallocH( const string& sReallocHString, const string& aTimeString )
{
	LOG_LOW_FUNC_ENTRY("CATAllocs::ReallocH");
	string sAllocH( sReallocHString );
	// Parse alloc & create new allocation.
	CATAlloc alloc;
	string sAddress = GetStringUntilNextSpace( sAllocH, true );
	alloc.m_sTime = aTimeString;
	alloc.m_sSize = GetStringUntilNextSpace( sAllocH, true );
	alloc.m_iCSCount = _httoi( string( GetStringUntilNextSpace( sAllocH, true ) ).c_str() );
	// Insert call stack fragment as "first" 1 because we are header.
	if ( alloc.m_iCSCount > 0 )
		alloc.m_vCallStack.insert( pair<unsigned long,string>( 0, sAllocH ) );
	// Add allocation
	pair< map<string,CATAlloc>::iterator, bool> ret;
	ret = m_vAllocs.insert( pair<string, CATAlloc>( sAddress, alloc ) );
	if( ret.second == false )
	{
		// Allocation to this memory address was already added.
		LOG_STRING( "CATAllocs: Allocating same address again, address: " << sAddress );
	}
}

// ReallocF.
void CATAllocs::ReallocF( const string& sReallocFString, const string& aTimeString )
{
	LOG_LOW_FUNC_ENTRY("CATAllocs::ReallocF");
	string sAllocF( sReallocFString );
	string sAddress = GetStringUntilNextSpace( sAllocF, true );
	string sTime = aTimeString;
	unsigned long iNumber = _httoi( string( GetStringUntilNextSpace( sAllocF, true ) ).c_str() );
	string sCallSstack = sAllocF;
	// Find correct allocation into which add call stack fragment.
	map<string, CATAlloc>::iterator it;
	it = m_vAllocs.find( sAddress );
	// TODO: If cannot find, create new in cache.
	if ( it == m_vAllocs.end() )
	{
		LOG_STRING( "CATAllocs: Allocate fragment without header: " << sAddress );
		return;
	}
	pair< map<unsigned long,string>::iterator, bool> ret;
	// Add call stack to it.
	ret = it->second.m_vCallStack.insert( pair<unsigned long,string>( iNumber, sCallSstack ) );
	if( ret.second == false )
	{
		LOG_STRING( "CATAllocs: Same allocation fragment again: " << sAddress );
	}
}

// Free message.
void CATAllocs::Free( const string& sFreeString )
{
	LOG_LOW_FUNC_ENTRY("CATAllocs::Free");
	string sFree( sFreeString );
	string sKey = GetStringUntilNextSpace( sFree );
	// Find related allocation.
	map<string, CATAlloc>::iterator it;
	it = m_vAllocs.find( sKey );
	if ( it == m_vAllocs.end() )
	{
		LOG_STRING( "CATAllocs: Free message which has no alloc pair: " << sKey );
		return;
	}
	// Delete it.
	m_vAllocs.erase( it );
}

// FreeH.
void CATAllocs::FreeH( const string& sFreeH, const string& aTimeString )
{
	LOG_LOW_FUNC_ENTRY("CATAllocs::FreeH");
	// Current implementation does not use call stack of
	// free message to anything.
	string sFree( sFreeH );
	string sKey = GetStringUntilNextSpace( sFree );
	// Time stamp. (not used currently)
	// Thread ID. (not used currently)
	// Call stack count. (not used currently)
	// Call stack data. (not used currently)

	// Find related allocation.
	map<string, CATAlloc>::iterator it;
	it = m_vAllocs.find( sKey );
	if ( it == m_vAllocs.end() )
	{
		LOG_STRING( "CATAllocs: FreeH message which has no alloc pair: " << sKey );
		return;
	}
	// Delete it.
	m_vAllocs.erase( it );
}

// FreeF.
void CATAllocs::FreeF( const string& /* sFreeF */, const string& /* aTimeString */ )
{
	LOG_LOW_FUNC_ENTRY("CATAllocs::FreeF");
	// Fragments are currently ignored.
	// Memory address. (not used currently)
	// Time stamp. (not used currently)
	// Packet number. (not used currently)
	// Call stack data. (not used currently)
}

// Get "leak" list ordered by allocation time.
void CATAllocs::GetLeakList( vector<string>& vLeakList )
{
	LOG_LOW_FUNC_ENTRY("CATAllocs::GetLeakList");

	// Create multimap where key is timestamp.
	// Add allocations there so they will go ascending order.
	multimap<string,string> mmap;
	multimap<string,string>::iterator itMmap;
	
	map<string, CATAlloc>::iterator it;
	for( it = m_vAllocs.begin(); it != m_vAllocs.end() ; it++ )
	{
		string sTime = it->second.m_sTime;
		
		string sLine = it->first;
		sLine.append(" ");
		sLine.append( it->second.GetAllocString() );

		mmap.insert( pair<string,string>( sTime, sLine ) );
	}
	
	// Iterate list to parameter vector.
	vLeakList.clear();
	for ( itMmap = mmap.begin(); itMmap != mmap.end(); itMmap++ )
		vLeakList.push_back( itMmap->second );
}

// Clear alloc data.
void CATAllocs::ClearAllocs( void )
{
	LOG_LOW_FUNC_ENTRY("CATAllocs::ClearAllocs");
	m_vAllocs.clear();
}

//EOF
