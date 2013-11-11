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
* Name        : wsd_locales.h
* Part of     : standard c++ library.
* 
*
*/




#ifndef WSD_LOCALES_H_
#define WSD_LOCALES_H_

//locale_impl.cpp  - function static in  _Locale_impl::Init::_M_count() 
inline std::_Refcount_Base& get_Loc_init_S_count()
	{
	return *(get_libcpp_wsd()._Loc_init_S_count);
	}

//_locale.h
inline size_t& std::locale::id::get_locale_id_S_max()
	{
	return get_libcpp_wsd().locale_id_S_max;
	}

//locale.cpp - function static in _Stl_loc_get_index()
inline __stl_atomic_t& get_S_index()
	{
	return get_libcpp_wsd()._S_index;
	}

//locale.cpp - function static in _Stl_loc_get_index()
inline std::_STLP_STATIC_MUTEX& get_locale_Index_lock()
	{
	return get_libcpp_wsd().locale_Index_lock;
	}

//monetary.cpp - global variable
inline std::string& get_monetary_S_empty_string()
	{
	return get_libcpp_wsd().monetary_S_empty_string;
	}
# ifndef _STLP_NO_WCHAR_T
//monetary.cpp - global variable
inline std::wstring& get_monetary_S_empty_wstring()
	{
	return get_libcpp_wsd().monetary_S_empty_wstring;
	}
#endif //_STLP_NO_WCHAR_T

//locale_impl.cpp - global
inline std::locale *get_Stl_classic_locale()
	{
	return get_libcpp_wsd()._Stl_classic_locale;
	}

//locale_impl.cpp - global
inline std::locale *get_Stl_global_locale()
	{
	return get_libcpp_wsd()._Stl_global_locale;
	}

//locale_impl.cpp - global
inline void set_Stl_classic_locale(std::locale *loc)
	{
	get_libcpp_wsd()._Stl_classic_locale = loc;
	}

//locale_impl.cpp - global
inline void set_Stl_global_locale(std::locale *loc)
	{
	get_libcpp_wsd()._Stl_global_locale = loc;
	}

//locale_catalog.cpp - global
inline std::_STLP_STATIC_MUTEX& 
get_locale_catalog_category_hash_lock()
	{
	return get_libcpp_wsd().locale_catalog_category_hash_lock;
	}


#endif /*WSD_LOCALES_H_*/
