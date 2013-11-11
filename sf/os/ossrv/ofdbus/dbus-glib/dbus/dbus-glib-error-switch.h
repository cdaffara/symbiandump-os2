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

static gint
dbus_error_to_gerror_code (const char *derr)
{
  if (0) ; 
  else if (!strcmp (derr,  DBUS_ERROR_FAILED  )) 
    return  DBUS_GERROR_FAILED ;
  else if (!strcmp (derr,  DBUS_ERROR_NO_MEMORY  )) 
    return  DBUS_GERROR_NO_MEMORY ;
  else if (!strcmp (derr,  DBUS_ERROR_SERVICE_UNKNOWN  )) 
    return  DBUS_GERROR_SERVICE_UNKNOWN ;
  else if (!strcmp (derr,  DBUS_ERROR_NAME_HAS_NO_OWNER  )) 
    return  DBUS_GERROR_NAME_HAS_NO_OWNER ;
  else if (!strcmp (derr,  DBUS_ERROR_NO_REPLY  )) 
    return  DBUS_GERROR_NO_REPLY ;
  else if (!strcmp (derr,  DBUS_ERROR_IO_ERROR  )) 
    return  DBUS_GERROR_IO_ERROR ;
  else if (!strcmp (derr,  DBUS_ERROR_BAD_ADDRESS  )) 
    return  DBUS_GERROR_BAD_ADDRESS ;
  else if (!strcmp (derr,  DBUS_ERROR_NOT_SUPPORTED  )) 
    return  DBUS_GERROR_NOT_SUPPORTED ;
  else if (!strcmp (derr,  DBUS_ERROR_LIMITS_EXCEEDED  )) 
    return  DBUS_GERROR_LIMITS_EXCEEDED ;
  else if (!strcmp (derr,  DBUS_ERROR_ACCESS_DENIED  )) 
    return  DBUS_GERROR_ACCESS_DENIED ;
  else if (!strcmp (derr,  DBUS_ERROR_AUTH_FAILED  )) 
    return  DBUS_GERROR_AUTH_FAILED ;
  else if (!strcmp (derr,  DBUS_ERROR_NO_SERVER  )) 
    return  DBUS_GERROR_NO_SERVER ;
  else if (!strcmp (derr,  DBUS_ERROR_TIMEOUT  )) 
    return  DBUS_GERROR_TIMEOUT ;
  else if (!strcmp (derr,  DBUS_ERROR_NO_NETWORK  )) 
    return  DBUS_GERROR_NO_NETWORK ;
  else if (!strcmp (derr,  DBUS_ERROR_ADDRESS_IN_USE  )) 
    return  DBUS_GERROR_ADDRESS_IN_USE ;
  else if (!strcmp (derr,  DBUS_ERROR_DISCONNECTED  )) 
    return  DBUS_GERROR_DISCONNECTED ;
  else if (!strcmp (derr,  DBUS_ERROR_INVALID_ARGS  )) 
    return  DBUS_GERROR_INVALID_ARGS ;
  else if (!strcmp (derr,  DBUS_ERROR_FILE_NOT_FOUND  )) 
    return  DBUS_GERROR_FILE_NOT_FOUND ;
  else if (!strcmp (derr,  DBUS_ERROR_FILE_EXISTS  )) 
    return  DBUS_GERROR_FILE_EXISTS ;
  else if (!strcmp (derr,  DBUS_ERROR_UNKNOWN_METHOD  )) 
    return  DBUS_GERROR_UNKNOWN_METHOD ;
  else if (!strcmp (derr,  DBUS_ERROR_TIMED_OUT  )) 
    return  DBUS_GERROR_TIMED_OUT ;
  else if (!strcmp (derr,  DBUS_ERROR_MATCH_RULE_NOT_FOUND  )) 
    return  DBUS_GERROR_MATCH_RULE_NOT_FOUND ;
  else if (!strcmp (derr,  DBUS_ERROR_MATCH_RULE_INVALID  )) 
    return  DBUS_GERROR_MATCH_RULE_INVALID ;
  else if (!strcmp (derr,  DBUS_ERROR_SPAWN_EXEC_FAILED  )) 
    return  DBUS_GERROR_SPAWN_EXEC_FAILED ;
  else if (!strcmp (derr,  DBUS_ERROR_SPAWN_FORK_FAILED  )) 
    return  DBUS_GERROR_SPAWN_FORK_FAILED ;
  else if (!strcmp (derr,  DBUS_ERROR_SPAWN_CHILD_EXITED  )) 
    return  DBUS_GERROR_SPAWN_CHILD_EXITED ;
  else if (!strcmp (derr,  DBUS_ERROR_SPAWN_CHILD_SIGNALED  )) 
    return  DBUS_GERROR_SPAWN_CHILD_SIGNALED ;
  else if (!strcmp (derr,  DBUS_ERROR_SPAWN_FAILED  )) 
    return  DBUS_GERROR_SPAWN_FAILED ;
  else if (!strcmp (derr,  DBUS_ERROR_UNIX_PROCESS_ID_UNKNOWN  )) 
    return  DBUS_GERROR_UNIX_PROCESS_ID_UNKNOWN ;
  else if (!strcmp (derr,  DBUS_ERROR_INVALID_SIGNATURE  )) 
    return  DBUS_GERROR_INVALID_SIGNATURE ;
  else if (!strcmp (derr,  DBUS_ERROR_INVALID_FILE_CONTENT  )) 
    return  DBUS_GERROR_INVALID_FILE_CONTENT ;
  else if (!strcmp (derr,  DBUS_ERROR_SELINUX_SECURITY_CONTEXT_UNKNOWN  )) 
    return  DBUS_GERROR_SELINUX_SECURITY_CONTEXT_UNKNOWN ;
  else
    return DBUS_GERROR_REMOTE_EXCEPTION;
}
