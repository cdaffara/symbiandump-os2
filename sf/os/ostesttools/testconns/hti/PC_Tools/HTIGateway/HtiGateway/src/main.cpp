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
*    This module contains the UI operations of DataGateway.
*/

#pragma warning ( disable : 4786 )

#include "Socket.h"

#include <windows.h>
#include <process.h>
#include <string>
#include <map>

#include "util.h"
#include "datagateway.h"
#include "common.h"

// GLOBAL DEFINES
// Command line switches
#define PARAM_SWITCH_PORT       "-port"
#define PARAM_SWITCH_BUFSIZE    "-bufsize"
#define PARAM_SWITCH_COMMPLUGIN "-commchannel"
#define PARAM_SWITCH_STAYALIVE  "-stayalive"
#define PARAM_SWITCH_CCLATEINIT "-cclateinit"
#define PARAM_SWITCH_SWT        "-swt"
#define PARAM_SWITCH_VERBOSE    "-v"
#define PARAM_SWITCH_TIMESTAMP  "-t"

#define VERSION "1.68.0"
#define INFO "HtiGateway %s - Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.\n\n"

// Event object to catch console signal
Event g_ConsoleCloseEvent;

// Event object to wait datagateway shutdown
Event g_DataGatewayClosedEvent;

// Maximum time to wait
extern long g_MaximumShutdownWaitTime;

extern DWORD g_ErrorCode;

// Global parameters to be used in datagateway and all plugins
map<string, string> g_parameters;

// FUNCTION PROTOTYPES
// Prints usage of DataGateway
void usage();

// Initializes phoenixtool default values
void init(map<string, string>&, map<string, string>&);

// Catches console control signals
BOOL WINAPI ConsoleControlHandler(DWORD);

int main(int argv, char *args[])
{
    printf(INFO, VERSION);
    if (argv == 2 && strcmp("-h", args[1]) == 0)
    {
        usage();
        return 0;
    }
    map<string, string> props;

    try {
        Util::ReadProperties("HtiGateway.ini", props);

        // Sets values from properties file
        init(props, g_parameters);

        if (argv > 1)
        {
            // Overrides properties file values
            // if specified in command line
            Util::ParseCmdLine(argv, args, g_parameters);
            Util::SetTimestamp(g_parameters[PARAM_SWITCH_TIMESTAMP]);
        }
        Util::SetVerboseLevel(g_parameters[PARAM_SWITCH_VERBOSE]);

        bool stayalive = (strcmp("true", g_parameters[PARAM_SWITCH_STAYALIVE].c_str()) == 0
                            ? true
                            : false);
        bool cclateinit = (strcmp("true", g_parameters[PARAM_SWITCH_CCLATEINIT].c_str()) == 0
                            ? true
                            : false);
        int port = atoi(g_parameters[PARAM_SWITCH_PORT].c_str());
        long bufsize = atol(g_parameters[PARAM_SWITCH_BUFSIZE].c_str());
        g_MaximumShutdownWaitTime = atol(g_parameters[PARAM_SWITCH_SWT].c_str());
        string ch = Util::ToUpper(g_parameters[PARAM_SWITCH_COMMPLUGIN]);

        DataGateway dg(port, bufsize, ch, stayalive, cclateinit);

        // Installs console control handler for catching
        // signals
        if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleControlHandler, TRUE))
        {
            Util::Error("Error registering the console control handler");
            return ERR_DG_CONSOLEHANDLER;
        }

        dg.Start();
        HANDLE handles[2];
        handles[0] = dg.ThreadHandle();
        handles[1] = g_ConsoleCloseEvent.EventHandle();
        DWORD dwResult = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        switch (dwResult)
        {
            case WAIT_OBJECT_0 + 0:
            {
                Util::Debug("main() HtiGateway thread stopped");
            }
            break;
            case WAIT_OBJECT_0 + 1:
            {
                dg.Stop();
                if (g_MaximumShutdownWaitTime > 0)
                {
                    Util::Info("[HtiGateway] Waiting HtiGateway to shutdown");
                    WaitForSingleObject(dg.ThreadHandle(), g_MaximumShutdownWaitTime);
                }
            }
            break;
        }
        g_DataGatewayClosedEvent.Set();
    } catch (UtilError ue) {
        Util::Error(ue.iError, ue.iResult);
        g_ErrorCode = ue.iResult;
    }
    if (g_ErrorCode != NO_ERRORS)
    {
        // Resolve error code to clear text
        ERROR_LOOKUP(g_ErrorCode);
    }

    return g_ErrorCode;
}

void init(map<string, string>& props,
          map<string, string>& params)
{
    // Setting default values from properties file
    // to parameters
    params[PARAM_SWITCH_BUFSIZE]    = props["TCPIP_RECV_BUFFER_SIZE"];
    params[PARAM_SWITCH_PORT]       = props["TCPIP_PORT"];
    params[PARAM_SWITCH_COMMPLUGIN] = props["DEFAULT_COMM_CHANNEL_PLUGIN"];
    params[PARAM_SWITCH_SWT]        = props["MAXIMUM_SHUTDOWN_WAITTIME"];
    params[PARAM_SWITCH_VERBOSE]    = "info";
    params[PARAM_SWITCH_STAYALIVE]  = "true";
    params[PARAM_SWITCH_TIMESTAMP]  = "false";
    params[PARAM_SWITCH_CCLATEINIT] = "false";
}

void usage()
{
    
	cout << "USAGE: HtiGateway.exe [SWITCHES in form -switch=value]\n\n";
    cout << "\tSWITCHES:\n";
    cout << "\t-port         TCP/IP port\n";
    cout << "\t-bufsize      TCP/IP receive buffer size in bytes\n";
    cout << "\t-commchannel  Communication Channel Plugin (CCP) name e.g. SERIAL\n";
    cout << "\t-stayalive    Whether HtiGateway remains when client disconnects\n";
    cout << "\t              or not\n";
    cout << "\t-cclateinit   Whether CCP is initialized when client connects or not\n";
    cout << "\t-swt          Maximum shutdown wait time in milliseconds. 0 means\n";
    cout << "\t              no waiting.\n";
    cout << "\t-v            Verbose mode\n";
    cout << "\t-t            Timestamped output\n\n";
    cout << "\tBy default these values are defined in HtiGateway.ini file.\n";
    cout << "\tIf any switches are used those override values from ini file\n\n";
    cout << "\tSTAYALIVE:\n";
    cout << "\tfalse      HtiGateway shutdown after client closes connection\n";
    cout << "\ttrue       HtiGateway remains waiting new connections (*)\n\n";
    cout << "\tCCLATEINIT:\n";
    cout << "\tfalse      HtiGateway initializes CCP when started (*)\n";
    cout << "\ttrue       HtiGateway initializes CCP only when client connects to\n\n";
    cout << "\tVERBOSE:\n";
    cout << "\toff        Silent mode\n";
    cout << "\tresult     Prints only result (no info/errors)\n";
    cout << "\terror      Prints result and errors (*)\n";
    cout << "\tinfo       Prints normal output\n";
    cout << "\tdebug      Prints all\n\n";
    cout << "\tTIMESTAMPED:\n";
    cout << "\tfalse      No timestamp (*)\n";
    cout << "\ttrue       Adds timestamp to output\n\n";
    cout << "\t(*) means default value\n\n";
    cout << "\tAll communication channel plugin parameters can also be passed from\n";
    cout << "\tcommand line just by adding \"-\" to the parameter name. For example:\n";
	cout << "\t\"-COMPORT=COM3\" or -REMOTE_PORT=3000\n";
    cout << endl;
}

BOOL WINAPI ConsoleControlHandler(DWORD dwCtrlType)
{
    BOOL bReturnStatus = FALSE;
    switch (dwCtrlType)
    {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
        {
            Util::Info("[HtiGateway] Request to terminate");
            g_ConsoleCloseEvent.Set();

            // Wait DG shutdown before returning control to system
            WaitForSingleObject(g_DataGatewayClosedEvent.EventHandle(), g_MaximumShutdownWaitTime);
        }
        break;
    }
    bReturnStatus = TRUE;
    return bReturnStatus;
}

// End of the file