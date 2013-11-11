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
* Name        : typeinfo_winscw.h
* Part of     : standard c++ library.
* Descritopn  : This file provides the typeinfo structure to match the CW
* runtime.
* 
*
*/



#ifndef __TYPEINFO_WINSCW_
#define __TYPEINFO_WINSCW_

namespace std {

  	class type_info {
  		
	public :
		bool operator== (const type_info& rhs) const {return static_cast<bool>(strcmp(tname, rhs.tname) == 0);}
		bool operator!= (const type_info& rhs) const {return static_cast<bool>(strcmp(tname, rhs.tname) != 0);}
		bool before (const type_info& rhs) const {return static_cast<bool>(strcmp(tname, rhs.tname) < 0);}
		const char* name () const {return tname;}
			
		 
		private :
		const char  *tname;
		const void  *tbase;

		type_info ( const type_info & rhs );
	    type_info& operator=(const type_info&);
	
	
	};  	
  	
	/* Compiler Runtime specific stuff ends here.
	 **/
	} //end of std namespace

	
inline std::bad_cast& std::bad_cast::operator=(const std::bad_cast&) __NO_THROW
	{
	return *this;
	}
inline const char* std::bad_cast::what() const __NO_THROW
	{
	return "bad_cast";
	}

inline std::bad_typeid& std::bad_typeid::operator=(const std::bad_typeid&) __NO_THROW 
	{
	return *this;
	}

inline const char* std::bad_typeid::what() const __NO_THROW 
	{
	return "bad_typeid";
	}
	
#endif //__TYPEINFO_WINSCW_
