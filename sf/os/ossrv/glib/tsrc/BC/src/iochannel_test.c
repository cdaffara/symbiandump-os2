/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
* Description:
*
*/


#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "glib.h"

#define BUFFER_SIZE 1024

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

int iochannel_test()
{
	GIOChannel *gio_r, *gio_w ;
    GError *gerr = NULL;
    GString *buffer;
    char *filename;
    char *srcdir = getenv ("srcdir");
    const gchar encoding[] = "ISO-8859-5";
    GIOStatus status;
    GIOFlags flags;
	GIOChannelError gio_error;
	GIOCondition gio_condition;
	const gchar *line_term;
	GIOChannel *temp;
	char *buf;
	gsize size_read,size_written;
	GIOError io_error;
	gunichar thechar;
	
	// This call should set gio_error to G_IO_CHANNEL_ERROR_INVAL
	gio_error = g_io_channel_error_from_errno(EINVAL);
	
	g_assert(gio_error == G_IO_CHANNEL_ERROR_INVAL);
	
	if (!srcdir)
      srcdir = "c:";
    filename = g_strconcat (srcdir, G_DIR_SEPARATOR_S, "iochannel-test-infile", NULL);
    
    gio_r = g_io_channel_new_file (filename, "r", &gerr);
    if (gerr)
      {
        g_warning ("Unable to open file %s: %s", filename, gerr->message);
        g_error_free (gerr);
        return 1;
      }
    gio_w = g_io_channel_new_file ("c:\\iochannel-test-outfile", "w", &gerr);
    if (gerr)
      {
        g_warning ("Unable to open file %s: %s", "iochannel-test-outfile", gerr->message);
        g_error_free (gerr);
        return 1;
      }

	g_io_channel_set_encoding (gio_r, NULL, &gerr);
	
	if (gerr)
      {
        g_warning (gerr->message);
        g_error_free (gerr);
        return 1;
      }
	
	g_io_channel_set_buffered(gio_r,TRUE);
	
	g_assert(gio_r->use_buffer == TRUE);
	
    g_io_channel_set_encoding (gio_r, encoding, &gerr);
    if (gerr)
      {
        g_warning (gerr->message);
        g_error_free (gerr);
        return 1;
      }
    
    g_io_channel_set_buffer_size (gio_r, BUFFER_SIZE);
    
    // check g_io_channel_get_buffer_size
    g_assert(g_io_channel_get_buffer_size(gio_r) == BUFFER_SIZE);
    
    // check g_io_channel_get_buffered
    g_assert(g_io_channel_get_buffered(gio_r) == TRUE);
    
    //g_io_channel_get_close_on_unref
    g_assert(g_io_channel_get_close_on_unref(gio_r) == TRUE);
    
    //check g_io_channel_get_encoding
    g_assert(!strcmp(g_io_channel_get_encoding(gio_r),encoding));
    
    line_term = g_io_channel_get_line_term(gio_r,NULL);
    
    //check g_io_channel_get_line_term
    g_assert(line_term == NULL);
    
    temp = g_io_channel_ref(gio_r);
    
    //check g_io_channel_ref
    g_assert(temp == gio_r && temp->ref_count == 2);
    
    g_io_channel_unref(gio_r);
    
    temp = NULL;
    
    status = g_io_channel_set_flags (gio_r, G_IO_FLAG_NONBLOCK, &gerr);
    if (status == G_IO_STATUS_ERROR)
      {
        g_warning (gerr->message);
        g_error_free (gerr);
        gerr = NULL;
      }
    flags = g_io_channel_get_flags (gio_r);
    buffer = g_string_sized_new (BUFFER_SIZE);
    
    status = g_io_channel_read_to_end(gio_r,&buf,&size_read,&gerr);
    
    // checks g_io_channel_read_to_end
    g_assert(status == G_IO_STATUS_NORMAL && gerr == NULL);
    
    status = g_io_channel_write_chars(gio_w,buf,size_read,&size_written,&gerr);
    
    // checks g_io_channel_write_chars
    g_assert(status == G_IO_STATUS_NORMAL && size_read == size_written && gerr == NULL);
    
    switch (status)
      {
        case G_IO_STATUS_NORMAL:
          break;
        case G_IO_STATUS_ERROR:
          g_warning (gerr->message);
          g_error_free (gerr);
          gerr = NULL;
          break;
        default:
          g_warning ("Abnormal exit from write loop.");
          break;
      }
    
    gio_condition = g_io_channel_get_buffer_condition(gio_w);
    
    //checks g_io_channel_get_buffer_condition
    g_assert(gio_condition == G_IO_OUT);
    
    do
      status = g_io_channel_flush (gio_w, &gerr);
    while (status == G_IO_STATUS_AGAIN);

    if (status == G_IO_STATUS_ERROR)
      {
        g_warning (gerr->message);
        g_error_free (gerr);
        gerr = NULL;
      }

	
	io_error = g_io_channel_seek(gio_r,0,G_SEEK_SET);
	
	// check g_io_channel_seek
	g_assert(io_error == G_IO_ERROR_NONE);
	
	status = g_io_channel_read_unichar(gio_r,&thechar,&gerr);
	
	//check g_io_channel_read_unichar
	g_assert(status == G_IO_STATUS_NORMAL && gerr == NULL);
	
	status = g_io_channel_write_unichar(gio_w,thechar,&gerr);
	
	//check g_io_channel_write_unichar
	g_assert(status == G_IO_STATUS_NORMAL && gerr == NULL);
	
	g_io_channel_set_line_term(gio_r,"L",-1);
	
	status = g_io_channel_read_line_string (gio_r, buffer, NULL, &gerr);
	
	//checks g_io_channel_set_line_term
	g_assert(buffer->str[strlen(buffer->str)-1] == 'L');
	
	do
      status = g_io_channel_flush (gio_w, &gerr);
    while (status == G_IO_STATUS_AGAIN);

	
	g_io_channel_unref(gio_r);
    g_io_channel_unref(gio_w);
    
    return 0;

}

int main()
{
	int retval;
	#ifdef SYMBIAN


	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	#endif /*SYMBIAN*/
	
	retval = iochannel_test();
	
	#ifdef SYMBIAN
  	testResultXml("iochannel_test");
  	#endif /* EMULATOR */
  	
  	return retval;
}