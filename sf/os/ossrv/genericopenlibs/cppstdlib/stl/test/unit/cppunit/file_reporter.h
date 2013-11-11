
/*
 * Copyright (c) 2003, 2004
 * Zdenek Nemec
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
 *----------------------------------------------------------------------------------------------
 *© Portions copyright (c) 2009 Nokia Corporation and/or its subsidiaries.  All rights reserved.
 *----------------------------------------------------------------------------------------------
 */

/* $Id: file_reporter.h 2421 2006-06-09 19:27:06Z dums $ */

#ifndef _CPPUNITMINIFILEREPORTERINTERFACE_H_
#define _CPPUNITMINIFILEREPORTERINTERFACE_H_

#include <stdio.h>

#include "cppunit_timer.h"

//
// CppUnit mini file(stream) reporter
//
class FileReporter : public CPPUNIT_NS::Reporter {
private:
  FileReporter(const FileReporter&);
  FileReporter& operator=(const FileReporter&);
public:
  // reporting to stderr
  explicit FileReporter(bool doMonitor = false):
      m_numErrors(0), m_numIgnores(0), m_numTests(0), _myStream(false),
      m_failed(false), m_error(false), m_doMonitor(doMonitor)
  { _file = stderr; }

  // reporting to the file with the given name
  explicit FileReporter(const char* file, bool doMonitor = false):
      m_numErrors(0), m_numIgnores(0), m_numTests(0), _myStream(true),
      m_failed(false), m_error(false), m_doMonitor(doMonitor)
  {  
#ifndef _STLP_USE_SAFE_STRING_FUNCTIONS
    _file = fopen(file, "a+");
#else
    fopen_s(&_file, file, "w");
#endif
  }

  // reporting to the given file
  explicit FileReporter(FILE* stream, bool doMonitor = false):
      m_numErrors(0), m_numIgnores(0), m_numTests(0), _myStream(false),
      m_failed(false), m_error(false), m_doMonitor(doMonitor)
  { _file = stream; }

  virtual ~FileReporter() {
    if (_myStream)
      fclose(_file);
    else
      fflush(_file);
  }

  virtual void error(const char *in_macroName, const char *in_macro, const char *in_file, int in_line) {
    ++m_numErrors;
    m_error = true;
    fprintf(_file, "\n\r\n\r%s(%d) : %s(%s);", in_file, in_line, in_macroName, in_macro);
  }

  virtual void failure(const char *in_macroName, const char *in_macro, const char *in_file, int in_line) {
    m_failed = true;
    m_error = true;
    fprintf(_file, "\n\r\n\r%s(%d) : %s(%s);", in_file, in_line, in_macroName, in_macro);
  }

  virtual void message( const char *msg )
  { fprintf(_file, "\n\r\t%s", msg ); }

  virtual void progress(const char *in_className, const char *in_shortTestName, bool ignoring) {  
    if (firstStep()) {
	  fprintf(_file, "\n\r\n\r@SYMTestCaseID:OPENENV-STDCPP-%s-0001 \n\r\n\r", in_className);
	  setFirstStep(false);
	}  
    if (m_doMonitor) {
      m_globalTimer.restart();
      m_testTimer.start();
    }
    ++m_numTests;    
       
    if (m_failed) {
      //Previous test experimented a failure:
      ++m_numErrors;
      m_failed = false;
    }
    
    if (m_error) {
        fprintf(_file, "\n\rRTEST: Checkpoint-fail \n\r");
        m_error = false;
    }
    else {
    	fprintf(_file, "\n\rRTEST: SUCCESS ");
    }
    
    if (ignoring)
      ++m_numIgnores;
    fprintf(_file, "%s::%s", in_className, in_shortTestName);
    if (ignoring)
      fprintf(_file, " IGNORED");
    
  }
  virtual void end() {
    if (m_doMonitor) {
      m_globalTimer.stop();
      m_testTimer.stop();
      fprintf(_file, " %f msec", m_testTimer.elapsedMilliseconds());
    }
    fprintf(_file, "\n\r");
  }
  virtual void printSummary() {   
    if (m_failed) {
      ++m_numErrors;
      m_failed = false;
    }
    if (m_numErrors > 0) {
      fprintf(_file, "\n\rThere were errors! %d of %d tests", m_numErrors, m_numTests);
    }
    else {
      fprintf(_file, "\n\rOK %d tests", m_numTests);      
    }
    if (m_numIgnores > 0) {
      fprintf(_file, ", %d ignored", m_numIgnores);
    }
    if (m_doMonitor) {
      fprintf(_file, " %f msec", m_globalTimer.elapsedMilliseconds());
    }
    fprintf(_file, "\n\r\n\r");    
  }
private:
  int m_numErrors;
  int m_numIgnores;
  int m_numTests;
  // flag whether we own '_file' and are thus responsible for releasing it in the destructor
  bool  _myStream;
  bool m_failed;  
  bool m_doMonitor;
  Timer m_globalTimer, m_testTimer;
  bool m_error;
  FILE* _file;  
};

#endif /*_CPPUNITMINIFILEREPORTERINTERFACE_H_*/
