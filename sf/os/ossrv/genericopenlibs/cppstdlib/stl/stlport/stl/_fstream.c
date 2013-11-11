/*
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1999
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */
#ifndef _STLP_FSTREAM_C
#define _STLP_FSTREAM_C

#ifndef _STLP_INTERNAL_FSTREAM_H
#  include <stl/_fstream.h>
#endif

#ifndef _STLP_INTERNAL_LIMITS
#  include <stl/_limits.h>
#endif

_STLP_BEGIN_NAMESPACE

# if defined ( _STLP_NESTED_TYPE_PARAM_BUG )
// no wchar_t is supported for this mode
# define __BF_int_type__ int
# define __BF_pos_type__ streampos
# define __BF_off_type__ streamoff
# else
# define __BF_int_type__ _STLP_TYPENAME_ON_RETURN_TYPE basic_filebuf<_CharT, _Traits>::int_type
# define __BF_pos_type__ _STLP_TYPENAME_ON_RETURN_TYPE basic_filebuf<_CharT, _Traits>::pos_type
# define __BF_off_type__ _STLP_TYPENAME_ON_RETURN_TYPE basic_filebuf<_CharT, _Traits>::off_type
# endif


//----------------------------------------------------------------------
// Public basic_filebuf<> member functions

template <class _CharT, class _Traits>
basic_filebuf<_CharT, _Traits>::basic_filebuf()
     :  basic_streambuf<_CharT, _Traits>(), _M_base(),
    _M_constant_width(false), _M_always_noconv(false),
    _M_int_buf_dynamic(false),
    _M_in_input_mode(false), _M_in_output_mode(false),
    _M_in_error_mode(false), _M_in_putback_mode(false),
    _M_int_buf(0), _M_int_buf_EOS(0),
    _M_ext_buf(0), _M_ext_buf_EOS(0),
    _M_ext_buf_converted(0), _M_ext_buf_end(0),
    _M_state(_STLP_DEFAULT_CONSTRUCTED(_State_type)),
    _M_end_state(_STLP_DEFAULT_CONSTRUCTED(_State_type)),
    _M_mmap_base(0), _M_mmap_len(0),
    _M_saved_eback(0), _M_saved_gptr(0), _M_saved_egptr(0),
    _M_codecvt(0),
    _M_width(1), _M_max_width(1)
{
  this->_M_setup_codecvt(locale(), false);
}


template <class _CharT, class _Traits>
basic_filebuf<_CharT, _Traits>*
basic_filebuf<_CharT, _Traits>::close() {
  bool __ok = this->is_open();

  if (_M_in_output_mode) {
    __ok = __ok && !_Traits::eq_int_type(this->overflow(traits_type::eof()),
                                         traits_type::eof());
    __ok == __ok && this->_M_unshift();
  }
  else if (_M_in_input_mode)
      this->_M_exit_input_mode();

  // Note order of arguments.  We close the file even if __ok is false.
  __ok = _M_base._M_close() && __ok;

  // Restore the initial state, except that we don't deallocate the buffer
  // or mess with the cached codecvt information.
  _M_state = _M_end_state = _State_type();
  _M_ext_buf_converted = _M_ext_buf_end = 0;

  _M_mmap_base = 0;
  _M_mmap_len = 0;

  this->setg(0, 0, 0);
  this->setp(0, 0);

  _M_saved_eback = _M_saved_gptr = _M_saved_egptr = 0;

  _M_in_input_mode = _M_in_output_mode = _M_in_error_mode = _M_in_putback_mode
    = false;

  return __ok ? this : 0;
}

// This member function is called whenever we exit input mode.
// It unmaps the memory-mapped file, if any, and sets
// _M_in_input_mode to false.
template <class _CharT, class _Traits>
void basic_filebuf<_CharT, _Traits>::_M_exit_input_mode() {
   if (_M_mmap_base != 0)
     _M_base._M_unmap(_M_mmap_base, _M_mmap_len);
   _M_in_input_mode = false;
   _M_mmap_base = 0;
}



//----------------------------------------------------------------------
// basic_filebuf<> helper functions.

//----------------------------------------
// Helper functions for switching between modes.

// This member function is called if we're performing the first I/O
// operation on a filebuf, or if we're performing an input operation
// immediately after a seek.
template <class _CharT, class _Traits>
bool basic_filebuf<_CharT, _Traits>::_M_switch_to_input_mode() {
  if (this->is_open() && (((int)_M_base.__o_mode() & (int)ios_base::in) != 0)
      && (_M_in_output_mode == 0) && (_M_in_error_mode == 0)) {
    if (!_M_int_buf && !_M_allocate_buffers())
      return false;

    _M_ext_buf_converted = _M_ext_buf;
    _M_ext_buf_end       = _M_ext_buf;

    _M_end_state    = _M_state;

    _M_in_input_mode = true;
    return true;
  }

  return false;
}


// This member function is called if we're performing the first I/O
// operation on a filebuf, or if we're performing an output operation
// immediately after a seek.
template <class _CharT, class _Traits>
bool basic_filebuf<_CharT, _Traits>::_M_switch_to_output_mode() {
  if (this->is_open() && (_M_base.__o_mode() & (int)ios_base::out) &&
      _M_in_input_mode == 0 && _M_in_error_mode == 0) {

    if (!_M_int_buf && !_M_allocate_buffers())
      return false;

    // In append mode, every write does an implicit seek to the end
    // of the file.  Whenever leaving output mode, the end of file
    // get put in the initial shift state.
    if (_M_base.__o_mode() & ios_base::app)
      _M_state = _State_type();

    this->setp(_M_int_buf, _M_int_buf_EOS - 1);
    _M_in_output_mode = true;
    return true;
  }

  return false;
}


//----------------------------------------
// Helper functions for input

// This member function is called if there is an error during input.
// It puts the filebuf in error mode, clear the get area buffer, and
// returns eof.
// returns eof.  Error mode is sticky; it is cleared only by close or
// seek.

template <class _CharT, class _Traits>
__BF_int_type__
basic_filebuf<_CharT, _Traits>::_M_input_error() {
   this->_M_exit_input_mode();
  _M_in_output_mode = false;
  _M_in_error_mode = true;
  this->setg(0, 0, 0);
  return traits_type::eof();
}

template <class _CharT, class _Traits>
__BF_int_type__
basic_filebuf<_CharT, _Traits>::_M_underflow_aux() {
  // We have the state and file position from the end of the internal
  // buffer.  This round, they become the beginning of the internal buffer.
  _M_state    = _M_end_state;

  // Fill the external buffer.  Start with any leftover characters that
  // didn't get converted last time.
  if (_M_ext_buf_end > _M_ext_buf_converted)

    _M_ext_buf_end = copy(_M_ext_buf_converted, _M_ext_buf_end, _M_ext_buf);
    // boris : copy_backward did not work
    //_M_ext_buf_end = copy_backward(_M_ext_buf_converted, _M_ext_buf_end,
    //_M_ext_buf+ (_M_ext_buf_end - _M_ext_buf_converted));
  else
    _M_ext_buf_end = _M_ext_buf;

  // Now fill the external buffer with characters from the file.  This is
  // a loop because occasionally we don't get enough external characters
  // to make progress.
  for (;;) {
    ptrdiff_t __n = _M_base._M_read(_M_ext_buf_end, _M_ext_buf_EOS - _M_ext_buf_end);

    // Don't enter error mode for a failed read.  Error mode is sticky,
    // and we might succeed if we try again.
    if (__n <= 0)
      return traits_type::eof();

    // Convert the external buffer to internal characters.
    _M_ext_buf_end += __n;
    const char*   __enext;
    _CharT* __inext;

    typename _Codecvt::result __status
      = _M_codecvt->in(_M_end_state,
                       _M_ext_buf, _M_ext_buf_end, __enext,
                       _M_int_buf, _M_int_buf_EOS, __inext);

    // Error conditions: (1) Return value of error.  (2) Producing internal
    // characters without consuming external characters.  (3) In fixed-width
    // encodings, producing an internal sequence whose length is inconsistent
    // with that of the internal sequence.  (4) Failure to produce any
    // characters if we have enough characters in the external buffer, where
    // "enough" means the largest possible width of a single character.
    if (__status == _Codecvt::noconv)
      return _Noconv_input<_Traits>::_M_doit(this);
    else if (__status == _Codecvt::error ||
             (__inext != _M_int_buf && __enext == _M_ext_buf) ||
             (_M_constant_width &&
              //         __inext - _M_int_buf != _M_width * (__enext - _M_ext_buf)) ||
              (__inext - _M_int_buf) *  _M_width != (__enext - _M_ext_buf)) ||
             (__inext == _M_int_buf && __enext - _M_ext_buf >= _M_max_width))
      return _M_input_error();
    else if (__inext != _M_int_buf) {
      _M_ext_buf_converted = _M_ext_buf + (__enext - _M_ext_buf);
      this->setg(_M_int_buf, _M_int_buf, __inext);
      return traits_type::to_int_type(*_M_int_buf);
    }
    // We need to go around the loop again to get more external characters.
  }
}

//----------------------------------------
// Helper functions for output

// This member function is called if there is an error during output.
// It puts the filebuf in error mode, clear the put area buffer, and
// returns eof.  Error mode is sticky; it is cleared only by close or
// seek.
template <class _CharT, class _Traits>
__BF_int_type__
basic_filebuf<_CharT, _Traits>::_M_output_error() {
  _M_in_output_mode = false;
  _M_in_input_mode = false;
  _M_in_error_mode = true;
  this->setp(0, 0);
  return traits_type::eof();
}


// Write whatever sequence of characters is necessary to get back to
// the initial shift state.  This function overwrites the external
// buffer, changes the external file position, and changes the state.
// Precondition: the internal buffer is empty.
template <class _CharT, class _Traits>
bool basic_filebuf<_CharT, _Traits>::_M_unshift() {
  if (_M_in_output_mode && !_M_constant_width) {
    typename _Codecvt::result __status;
    do {
      char* __enext = _M_ext_buf;
      __status = _M_codecvt->unshift(_M_state,
                                     _M_ext_buf, _M_ext_buf_EOS, __enext);
      if (__status == _Codecvt::noconv ||
          (__enext == _M_ext_buf && __status == _Codecvt::ok))
        return true;
      else if (__status == _Codecvt::error)
        return false;
      else if (!_M_write(_M_ext_buf, __enext - _M_ext_buf))
        return false;
    } while (__status == _Codecvt::partial);
  }

  return true;
}


//----------------------------------------
// Helper functions for buffer allocation and deallocation

// This member function is called when we're initializing a filebuf's
// internal and external buffers.  The argument is the size of the
// internal buffer; the external buffer is sized using the character
// width in the current encoding.  Preconditions: the buffers are currently
// null.  __n >= 1.  __buf is either a null pointer or a pointer to an
// array show size is at least __n.

// We need __n >= 1 for two different reasons.  For input, the base
// class always needs a buffer because of the semantics of underflow().
// For output, we want to have an internal buffer that's larger by one
// element than the buffer that the base class knows about.  (See
// basic_filebuf<>::overflow() for the reason.)
template <class _CharT, class _Traits>
bool basic_filebuf<_CharT, _Traits>::_M_allocate_buffers(_CharT* __buf, streamsize __n) {
  //The major hypothesis in the following implementation is that size_t is unsigned.
  //We also need streamsize byte representation to be larger or equal to the int
  //representation to correctly store the encoding information.
  _STLP_STATIC_ASSERT(!numeric_limits<size_t>::is_signed &&
                      sizeof(streamsize) >= sizeof(int))

  if (__buf == 0) {
    streamsize __bufsize = __n * sizeof(_CharT);
    //We first check that the streamsize representation can't overflow a size_t one.
    //If it can, we check that __bufsize is not higher than the size_t max value.
    if ((sizeof(streamsize) > sizeof(size_t)) &&
        (__bufsize > __STATIC_CAST(streamsize, (numeric_limits<size_t>::max)())))
      return false;
    _M_int_buf = __STATIC_CAST(_CharT*, malloc(__STATIC_CAST(size_t, __bufsize)));
    if (!_M_int_buf)
      return false;
    _M_int_buf_dynamic = true;
  }
  else {
    _M_int_buf = __buf;
    _M_int_buf_dynamic = false;
  }

  streamsize __ebufsiz = (max)(__n * __STATIC_CAST(streamsize, _M_width),
                               __STATIC_CAST(streamsize, _M_codecvt->max_length()));
  _M_ext_buf = 0;
  if ((sizeof(streamsize) < sizeof(size_t)) ||
      ((sizeof(streamsize) == sizeof(size_t)) && numeric_limits<streamsize>::is_signed) ||
      (__ebufsiz <= __STATIC_CAST(streamsize, (numeric_limits<size_t>::max)()))) {
    _M_ext_buf = __STATIC_CAST(char*, malloc(__STATIC_CAST(size_t, __ebufsiz)));
  }

  if (!_M_ext_buf) {
    _M_deallocate_buffers();
    return false;
  }

  _M_int_buf_EOS = _M_int_buf + __STATIC_CAST(ptrdiff_t, __n);
  _M_ext_buf_EOS = _M_ext_buf + __STATIC_CAST(ptrdiff_t, __ebufsiz);
  return true;
}

// Abbreviation for the most common case.
template <class _CharT, class _Traits>
bool basic_filebuf<_CharT, _Traits>::_M_allocate_buffers() {
  // Choose a buffer that's at least 4096 characters long and that's a
  // multiple of the page size.
  streamsize __default_bufsiz =
    ((_M_base.__page_size() + 4095UL) / _M_base.__page_size()) * _M_base.__page_size();
  return _M_allocate_buffers(0, __default_bufsiz);
}

template <class _CharT, class _Traits>
void basic_filebuf<_CharT, _Traits>::_M_deallocate_buffers() {
  if (_M_int_buf_dynamic)
    free(_M_int_buf);
  free(_M_ext_buf);
  _M_int_buf     = 0;
  _M_int_buf_EOS = 0;
  _M_ext_buf     = 0;
  _M_ext_buf_EOS = 0;
}


//----------------------------------------
// Helper functiosn for seek and imbue

template <class _CharT, class _Traits>
bool basic_filebuf<_CharT, _Traits>::_M_seek_init(bool __do_unshift) {
  // If we're in error mode, leave it.
   _M_in_error_mode = false;

  // Flush the output buffer if we're in output mode, and (conditionally)
  // emit an unshift sequence.
  if (_M_in_output_mode) {
    bool __ok = !traits_type::eq_int_type(this->overflow(traits_type::eof()),
                                          traits_type::eof());
    if (__do_unshift)
      __ok = __ok && this->_M_unshift();
    if (!__ok) {
      _M_in_output_mode = false;
      _M_in_error_mode = true;
      this->setp(0, 0);
      return false;
    }
  }

  // Discard putback characters, if any.
  if (_M_in_input_mode && _M_in_putback_mode)
    _M_exit_putback_mode();

  return true;
}


/* Change the filebuf's locale.  This member function has no effect
 * unless it is called before any I/O is performed on the stream.
 * This function is called on construction and on an imbue call. In the
 * case of the construction the codecvt facet might be a custom one if
 * the basic_filebuf user has instanciate it with a custom char_traits.
 * The user will have to call imbue before any I/O operation.
 */
template <class _CharT, class _Traits>
void basic_filebuf<_CharT, _Traits>::_M_setup_codecvt(const locale& __loc, bool __on_imbue) {
  if (has_facet<_Codecvt>(__loc)) {
    _M_codecvt = &use_facet<_Codecvt>(__loc) ;
    int __encoding    = _M_codecvt->encoding();

    _M_width          = (max)(__encoding, 1);
    _M_max_width      = _M_codecvt->max_length();
    _M_constant_width = __encoding > 0;
    _M_always_noconv  = _M_codecvt->always_noconv();
  }
  else {
    _M_codecvt = 0;
    _M_width = _M_max_width = 1;
    _M_constant_width = _M_always_noconv  = false;
    if (__on_imbue) {
      //This call will generate an exception reporting the problem.
      use_facet<_Codecvt>(__loc);
    }
  }
}

_STLP_END_NAMESPACE

# undef __BF_int_type__
# undef __BF_pos_type__
# undef __BF_off_type__

#endif /* _STLP_FSTREAM_C */

// Local Variables:
// mode:C++
// End:
