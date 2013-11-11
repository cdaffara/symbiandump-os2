/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : wsd_streams.h
* Part of     : standard c++ library.
* 
*
*/



#ifndef WSD_STREAMS_H_
#define WSD_STREAMS_H_



/********************** stream related ****************/
inline long& std::ios_base::Init::get_ios_base_Init_S_count()
	{
		return get_libcpp_wsd().ios_base_Init_S_count;
	}

//_ios_base.h
inline bool& std::ios_base::get_ios_base_S_was_synced()
	{
		return get_libcpp_wsd().ios_base_S_was_synced;
	}
//ios.cpp - function static in ios_base::xalloc()
inline int& get_ios_base_xalloc_S_index()
	{
	return get_libcpp_wsd().ios_base_xalloc_S_index;
	}

//ios.cpp - function static in ios_base::xalloc()
inline std::_STLP_STATIC_MUTEX& get_ios_base_xalloc_lock()
	{
	return get_libcpp_wsd().ios_base_xalloc_lock;
	}


#endif /*WSD_STREAMS_H_*/
