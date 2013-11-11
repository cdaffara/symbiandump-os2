// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <string>
#include <sstream>
#if !defined (STLPORT) || !defined (_STLP_USE_NO_IOSTREAMS)
#  include <fstream>
#  include <locale>

#  include "cppunit/cppunit_proxy.h"

#  if !defined (STLPORT) || defined(_STLP_USE_NAMESPACES)
using namespace std;
#  endif

//
// TestCase class
//
class CodecvtTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(CodecvtTest);
#if defined (STLPORT) && defined (_STLP_NO_MEMBER_TEMPLATES)
  CPPUNIT_IGNORE;
#endif
  CPPUNIT_TEST(variable_encoding);
  CPPUNIT_TEST(locale_cov1);
  CPPUNIT_TEST(locale_cov2);
  CPPUNIT_TEST(locale_cov3);
  CPPUNIT_TEST(locale_cov4);
  CPPUNIT_TEST(locale_cov5);
  CPPUNIT_TEST(locale_cov6);
  CPPUNIT_TEST(locale_cov7);
  CPPUNIT_TEST_SUITE_END();

protected:
  void variable_encoding();
  void locale_cov1();
  void locale_cov2();
  void locale_cov3();
  void locale_cov4();
  void locale_cov5();
  void locale_cov6();
  void locale_cov7();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CodecvtTest);

#if defined (STLPORT)
#  define __NO_THROW _STLP_NOTHROW
#else
#  define __NO_THROW throw()
#endif


/* Codecvt facet eating some characters from the external buffer.
 * Transform '01' in 'a'
 */
struct eater_codecvt : public codecvt<char, char, mbstate_t> {
  typedef codecvt<char,char,mbstate_t> base;

  explicit eater_codecvt(size_t refs = 0) : base(refs) {}

  // primitive conversion
  virtual base::result
  do_in(mbstate_t& mb,
        const char* ebegin, const char* eend, const char*& ecur,
        char* ibegin, char* iend, char*& icur) const __NO_THROW {
      char *state = (char*)&mb;
      ecur = ebegin;
      icur = ibegin;

      while (ecur != eend) {
          if (icur == iend)
              return partial;
          if (*ecur == '0' || *state == 1) {
            if (*state != 1) {
              ++ecur;
            }
            if (ecur == eend) {
              *state = 1;
              return ok;
            }

            if (*ecur == '1') {
              *icur = 'a';
            }
            else {
              *(icur++) = '0';
              if (icur == iend) {
                if (*state != 1) {
                  --ecur;
                }
                return partial;
              }
              *icur = *ecur;
            }
          }
          else {
            *icur = *ecur;
          }

          *state = 0;
          ++icur;
          ++ecur;
      }

      return ok;
  }

  // claim it's not a null-conversion
  virtual bool do_always_noconv() const __NO_THROW
  { return false; }

  // claim it doesn't have a fixed-length encoding
  virtual int do_encoding() const __NO_THROW
  { return 0; }

  // implemented for consistency with do_in overload
  virtual int do_length(const mbstate_t &state,
                        const char *efrom, const char *eend, size_t m) const {
    char *ibegin = new char[m];
    const char *ecur = efrom;
    char *icur = ibegin;
    mbstate_t tmp = state;
    do_in(tmp, efrom, eend, ecur, ibegin, ibegin + m, icur);
    delete[] ibegin;
    return ecur - efrom;
  }

  virtual int do_max_length() const __NO_THROW
  { return 2; }
};

/* Codecvt facet generating more characters than the ones read from the
 * external buffer, transform '01' in 'abc'
 * This kind of facet do not allow systematical positionning in the external
 * buffer (tellg -> -1), when you just read a 'a' you are at an undefined
 * external buffer position.
 */
struct generator_codecvt : public codecvt<char, char, mbstate_t> {
  typedef codecvt<char,char,mbstate_t> base;

  explicit generator_codecvt(size_t refs = 0) : base(refs) {}

  // primitive conversion
  virtual base::result
  do_in(mbstate_t& mb,
        const char* ebegin, const char* eend, const char*& ecur,
        char* ibegin, char* iend, char*& icur) const __NO_THROW {
      //Access the mbstate information in a portable way:
      char *state = (char*)&mb;
      ecur = ebegin;
      icur = ibegin;

      if (icur == iend) return ok;

      if (*state == 2) {
        *(icur++) = 'b';
        if (icur == iend) {
          *state = 3;
          return ok;
        }
        *(icur++) = 'c';
        *state = 0;
      }
      else if (*state == 3) {
        *(icur++) = 'c';
        *state = 0;
      }

      while (ecur != eend) {
          if (icur == iend)
              return ok;
          if (*ecur == '0' || *state == 1) {
            if (*state != 1) {
              ++ecur;
            }
            if (ecur == eend) {
              *state = 1;
              return partial;
            }

            if (*ecur == '1') {
              *(icur++) = 'a';
              if (icur == iend) {
                *state = 2;
                return ok;
              }
              *(icur++) = 'b';
              if (icur == iend) {
                *state = 3;
                return ok;
              }
              *icur = 'c';
            }
            else {
              *(icur++) = '0';
              if (icur == iend) {
                if (*state != 1) {
                  --ecur;
                }
                return ok;
              }
              *icur = *ecur;
            }
          }
          else {
            *icur = *ecur;
          }

          *state = 0;
          ++icur;
          ++ecur;
      }

      return ok;
  }

  // claim it's not a null-conversion
  virtual bool do_always_noconv() const __NO_THROW
  { return false; }

  // claim it doesn't have a fixed-length encoding
  virtual int do_encoding() const __NO_THROW
  { return 0; }

  // implemented for consistency with do_in overload
  virtual int do_length(const mbstate_t &mb,
                        const char *efrom, const char *eend, size_t m) const {
    const char *state = (const char*)&mb;
    int offset = 0;
    if (*state == 2)
      offset = 2;
    else if (*state == 3)
      offset = 1;

    char *ibegin = new char[m + offset];
    const char *ecur = efrom;
    char *icur = ibegin;
    mbstate_t tmpState = mb;
    do_in(tmpState, efrom, eend, ecur, ibegin, ibegin + m + offset, icur);
    /*
    char *state = (char*)&tmpState;
    if (*state != 0) {
      if (*state == 1)
        --ecur;
      else if (*state == 2 || *state == 3) {
        //Undefined position, we return -1:
        ecur = efrom - 1;
      }
    }
    else {
      if (*((char*)&mb) != 0) {
        //We take into account the character that hasn't been counted yet in
        //the previous decoding step:
        ecur++;
      }
    }
    */
    delete[] ibegin;
    return (int)min((size_t)(ecur - efrom), m);
  }

  virtual int do_max_length() const __NO_THROW
  { return 0; }
};

//
// tests implementation
//
void CodecvtTest::variable_encoding()
{
#if !defined (STLPORT) || !defined (_STLP_NO_MEMBER_TEMPLATES)
  //We first generate the file used for test:
  const char* fileName = "c:\\test_file.txt";
  {
    ofstream ostr(fileName);
    //Maybe we simply do not have write access to repository
    CPPUNIT_ASSERT( ostr.good() );
    for (int i = 0; i < 2048; ++i) {
      ostr << "0123456789";
    }
    CPPUNIT_ASSERT( ostr.good() );
  }

  {
    ifstream istr(fileName);
    CPPUNIT_ASSERT( istr.good() );
    CPPUNIT_ASSERT( !istr.eof() );

    eater_codecvt codec(1);
    locale loc(locale::classic(), &codec);

    istr.imbue(loc);
    CPPUNIT_ASSERT( istr.good() );
    CPPUNIT_ASSERT( (int)istr.tellg() == 0 );

    int theoricalPos = 0;
    do {
      signed char c = (signed char)istr.get();
      if (c == char_traits<char>::eof()) {
        break;
      }
      ++theoricalPos;
      if (c == 'a') {
        ++theoricalPos;
      }
     CPPUNIT_ASSERT( (int)istr.tellg() == theoricalPos );
    }
    while (!istr.eof());

    CPPUNIT_ASSERT( istr.eof() );
  }

#  if 0
  /* This test is broken, not sure if it is really possible to get a position in
   * a locale having a codecvt such as generator_codecvt. Maybe generator_codecvt
   * is not a valid theorical example of codecvt implementation. */
  {
    ifstream istr(fileName);
    CPPUNIT_ASSERT( istr.good() );
    CPPUNIT_ASSERT( !istr.eof() );

    generator_codecvt codec(1);
    locale loc(locale::classic(), &codec);

    istr.imbue(loc);
    CPPUNIT_ASSERT( istr.good() );
    CPPUNIT_ASSERT( (int)istr.tellg() == 0 );

    int theoricalPos = 0;
    int theoricalTellg;
    do {
      char c = istr.get();
      if (c == char_traits<char>::eof()) {
        break;
      }
      switch (c) {
        case 'a':
        case 'b':
          theoricalTellg = -1;
          break;
        case 'c':
          ++theoricalPos;
        default:
          ++theoricalPos;
          theoricalTellg = theoricalPos;
          break;
      }

      if ((int)istr.tellg() != theoricalTellg) {
        CPPUNIT_ASSERT( (int)istr.tellg() == theoricalTellg );
      }
    }
    while (!istr.eof());

    CPPUNIT_ASSERT( istr.eof() );
  }
#  endif
#endif
}

void CodecvtTest::locale_cov1()
	{
	locale loc ( "fr_FR.ISO-8859-1" );
	locale loc1 ( "en_US.ISO-8859-1" );
	bool result1,result2;
		{
	    result1 = isalnum ( 'L', loc);
		result2 = isalnum ( '@', loc);
		CPPUNIT_ASSERT( result1 == true );
		CPPUNIT_ASSERT( result2 == false);
		
		result1 = isalnum ( 'L', loc1);
		result2 = isalnum ( '@', loc1);
		CPPUNIT_ASSERT( result1 == true );
		CPPUNIT_ASSERT( result2 == false);
		}
		{
	    result1 = isalpha ( 'L', loc);
		result2 = isalpha ( '@', loc);
		CPPUNIT_ASSERT( result1 == true );
		CPPUNIT_ASSERT( result2 == false);
		
		result1 = isalpha ( 'L', loc1);
		result2 = isalpha ( '@', loc1);
		CPPUNIT_ASSERT( result1 == true );
		CPPUNIT_ASSERT( result2 == false);
		}
		{
	    result1 = iscntrl ( 'L', loc);
		result2 = iscntrl ( '\n', loc);
		CPPUNIT_ASSERT( result1 == false);
		CPPUNIT_ASSERT( result2 == true );
		
		result1 = iscntrl ( 'L', loc1);
		result2 = iscntrl ( '\n', loc1);
		CPPUNIT_ASSERT( result1 == false);
		CPPUNIT_ASSERT( result2 == true );
		}
		{
	    result1 = isdigit ( 'L', loc);
		result2 = isdigit ( '3', loc);
		CPPUNIT_ASSERT( result1 == false);
		CPPUNIT_ASSERT( result2 == true );
		
	    result1 = isdigit ( 'L', loc1);
		result2 = isdigit ( '3', loc1);
		CPPUNIT_ASSERT( result1 == false);
		CPPUNIT_ASSERT( result2 == true );
		}
	}
void CodecvtTest::locale_cov2()
	{
	locale loc ( "fr_FR.ISO-8859-1" );
	locale loc1 ( "en_US.ISO-8859-1" );
	bool result1,result2;
		{
	    result1 = isgraph ( ' ', loc);
		result2 = isgraph ( '.', loc);
		CPPUNIT_ASSERT( result1 == false);
		CPPUNIT_ASSERT( result2 == true );
		
		result1 = isgraph ( ' ', loc1);
		result2 = isgraph ( '.', loc1);
		CPPUNIT_ASSERT( result1 == false);
		CPPUNIT_ASSERT( result2 == true );
		}
		{
	    result1 = islower ( 'L', loc);
		result2 = islower ( 'v', loc);
		CPPUNIT_ASSERT( result1 == false);
		CPPUNIT_ASSERT( result2 == true );
		
		result1 = islower ( 'L', loc1);
		result2 = islower ( 'v', loc1);
		CPPUNIT_ASSERT( result1 == false);
		CPPUNIT_ASSERT( result2 == true );
		}
		{
	    result1 = isprint ( '\n', loc);
		result2 = isprint ( 't', loc);
		CPPUNIT_ASSERT( result1 == false);
		CPPUNIT_ASSERT( result2 == true );
		
		result1 = isprint ( '\n', loc1);
		result2 = isprint ( 't', loc1);
		CPPUNIT_ASSERT( result1 == false);
		CPPUNIT_ASSERT( result2 == true );
		}
		{
	    result1 = ispunct ( 'L', loc);
		result2 = ispunct ( ';', loc);
		CPPUNIT_ASSERT( result1 == false);
		CPPUNIT_ASSERT( result2 == true );
		
		result1 = ispunct ( 'L', loc1);
		result2 = ispunct ( ';', loc1);
		CPPUNIT_ASSERT( result1 == false);
		CPPUNIT_ASSERT( result2 == true );
		}
	}
void CodecvtTest::locale_cov3()
	{
	locale loc ( "fr_FR.ISO-8859-1" );
	locale loc1 ( "en_US.ISO-8859-1" );
	bool result1,result2,result3;
		{
		result2 = isspace ( '\n', loc);
		result3 = isspace ( 'x', loc);
		CPPUNIT_ASSERT( result2 == true );
		CPPUNIT_ASSERT( result3 == false );
		
		result2 = isspace ( '\n', loc1);
		result3 = isspace ( 'x', loc1);
		CPPUNIT_ASSERT( result2 == true );
		CPPUNIT_ASSERT( result3 == false );
		}
		{
	    result1 = isupper ( 'L', loc);
		result2 = isupper ( ';', loc);
		CPPUNIT_ASSERT( result1 == true );
		CPPUNIT_ASSERT( result2 == false );
		
		result1 = isupper ( 'L', loc1);
		result2 = isupper ( ';', loc1);
		CPPUNIT_ASSERT( result1 == true );
		CPPUNIT_ASSERT( result2 == false );
		}
		{
	    result1 = isxdigit ( 'f', loc);
		result2 = isxdigit ( 'd', loc);
		result3 = isxdigit ( 'q', loc);
		CPPUNIT_ASSERT( result1 == true );
		CPPUNIT_ASSERT( result2 == true );
		CPPUNIT_ASSERT( result3 == false );
		
		result1 = isxdigit ( 'f', loc1);
		result2 = isxdigit ( 'd', loc1);
		result3 = isxdigit ( 'q', loc1);
		CPPUNIT_ASSERT( result1 == true );
		CPPUNIT_ASSERT( result2 == true );
		CPPUNIT_ASSERT( result3 == false );
		}
	}
void CodecvtTest::locale_cov4()
	{
	locale loc ( "fr_FR.ISO-8859-1" );
	locale loc1 ( "en_US.ISO-8859-1" );
	char cresult1;
		{
		cresult1 = tolower ( 'H', loc );
		CPPUNIT_ASSERT( cresult1 == 'h' );
		cresult1 = tolower ( 'h', loc );
		CPPUNIT_ASSERT( cresult1 == 'h' );
		cresult1 = tolower ( '$', loc );
		CPPUNIT_ASSERT( cresult1 == '$' );
		
		cresult1 = tolower ( 'H', loc1 );
		CPPUNIT_ASSERT( cresult1 == 'h' );
		cresult1 = tolower ( 'h', loc1 );
		CPPUNIT_ASSERT( cresult1 == 'h' );
		cresult1 = tolower ( '$', loc1 );
		CPPUNIT_ASSERT( cresult1 == '$' );
		}
		{
		cresult1 = toupper ( 'H', loc );
		CPPUNIT_ASSERT( cresult1 == 'H' );
		cresult1 = toupper ( 'h', loc );
		CPPUNIT_ASSERT( cresult1 == 'H' );
		cresult1 = toupper ( '$', loc );
		CPPUNIT_ASSERT( cresult1 == '$' );
		
		cresult1 = toupper ( 'H', loc1 );
		CPPUNIT_ASSERT( cresult1 == 'H' );
		cresult1 = toupper ( 'h', loc1 );
		CPPUNIT_ASSERT( cresult1 == 'H' );
		cresult1 = toupper ( '$', loc1 );
		CPPUNIT_ASSERT( cresult1 == '$' );
		}
	}
void CodecvtTest::locale_cov5()
	{
		{
		char* str = "stdcpp with pips";
		mbstate_t state = {0};
		locale loc("C");
		int res = use_facet<codecvt<wchar_t, char, mbstate_t> > ( loc ).length( state,str, &str[strlen(str)], 50 );
		CPPUNIT_ASSERT( res == 16 );
		res = use_facet<codecvt<char, char, mbstate_t> >( loc ).max_length( );
		}
		{
		char* str = "stdcpp with pips";
		wchar_t wstr [50];
		memset(&wstr[0], 0, (sizeof(wchar_t))*(50));
		const char* pszNext;
		wchar_t* pwszNext;
		mbstate_t state = {0};
		locale loc("C");
		int res = use_facet<codecvt<wchar_t, char, mbstate_t> >( loc ).in( state,str, &str[strlen(str)], pszNext,wstr, &wstr[strlen(str)], pwszNext );
		wstr[strlen(str)] = 0; 
		CPPUNIT_ASSERT(res!=codecvt_base::error);
		}
		{
		locale loc ( "fr_FR.ISO-8859-1" );
		int result1 = use_facet<codecvt<char, char, mbstate_t> > ( loc ).encoding ( );
		CPPUNIT_ASSERT(result1 == 1);
		bool result2 = use_facet<codecvt<char, char, mbstate_t> >( loc ).always_noconv( );
		CPPUNIT_ASSERT(result2 == true);
		result2 = use_facet<codecvt<wchar_t, char, mbstate_t> >( loc ).always_noconv( );
		CPPUNIT_ASSERT(result2 == false);
		}
	}
void CodecvtTest::locale_cov6()
	{
		{
		locale loc( "en_US.ISO-8859-1" );
		const numpunct < char> &npunct = use_facet <numpunct <char> >( loc );
		string str = npunct.truename();
		CPPUNIT_ASSERT(str == "true");
		str = npunct.falsename();
		CPPUNIT_ASSERT(str == "false");
		CPPUNIT_ASSERT(npunct.thousands_sep( ) == ',');
		
		const numpunct < wchar_t> &npunct1 = use_facet <numpunct <wchar_t> >( loc );
		wstring str1 = npunct1.truename();
		CPPUNIT_ASSERT(str1 == L"true");
		str1 = npunct1.falsename();
		CPPUNIT_ASSERT(str1 == L"false");
		CPPUNIT_ASSERT(npunct1.thousands_sep( ) == L',');
		CPPUNIT_ASSERT(npunct1.decimal_point( ) == L'.');
		}
	}
void CodecvtTest::locale_cov7()
	{
		{
		locale loc( "en_US.ISO-8859-1" );
		const moneypunct <char, true> &mpunct = use_facet <moneypunct <char, true > >(loc);
		CPPUNIT_ASSERT(mpunct.thousands_sep( ) == ',');
		string str = mpunct.positive_sign( );
		str = mpunct.negative_sign( );
		char x = mpunct.neg_format().field[0];
		x = mpunct.neg_format().field[1];
		x = mpunct.neg_format().field[2];
		x = mpunct.neg_format().field[3];
		CPPUNIT_ASSERT(mpunct.decimal_point( ) == '.');
		str = mpunct.curr_symbol( );
		
		const moneypunct <char, false> &mpunct2 = use_facet <moneypunct <char, false> >(loc);
		CPPUNIT_ASSERT(mpunct2.thousands_sep( ) == ',');
		str = mpunct2.positive_sign( );
		str = mpunct2.negative_sign( );
		x = mpunct2.neg_format().field[0];
		x = mpunct2.neg_format().field[1];
		x = mpunct2.neg_format().field[2];
		x = mpunct2.neg_format().field[3];
		CPPUNIT_ASSERT(mpunct2.decimal_point( ) == '.');
		str = mpunct2.curr_symbol( );
		}
	}
#endif
