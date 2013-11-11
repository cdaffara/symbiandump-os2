//
// Copyright (c) Symbian Software Ltd 2007. All rights reserved.
//


*** Overview ***

This directory contains example code that demonstrates how user permission
prompt support(UPS) may be added to a system server. The example code consists of
three components.

*** Components ***

1) tmsgserver 
A simple server that provides an asynchronous API to send messages. Since this
is example code the messages are displayed to the screen.

2) tmsclient 
The client library for tmsgserver.  
No modifications are necessary in order to support user permission prompts.

3) tmsgapp
An example application that sends a number of messages and also tests cancellation. 

No modification was required as a consequence of the UPS integration. However,
in order to allow the test to be run multiple times the UPS management API is
used to delete old decision records for this application.

*** Integration Overview *** 
Whilst the APIs for the UPS are relatively small the integration is slightly
more complicated than might be expected because existing synchronous security
checks must become asynchronous since interaction with the device user is
inherently an asynchronous operation. For example, it would be undesirable to
block the entire network server whilst waiting for the user to respond to a
dialog.

The second factor that must be considered is that in order to make an informed
security decision the user may need to know data about the request they are
authorizing. Consequently, it may be necessary to extract the parameters from
the client application at the security check stage.

Finally, depending on the service and the security requirements of the
manufacturer an operator user prompts may be in addition to the platform
security (e.g. capabilities) checks. Alternatively, user prompts could be used
to allow applications without capabilities limited access to a protected
service. Since most implementations of the CPolicyServer fail the client
immediately if the platsec check fails these checks may have to be modified.

*** Integration Points *** 
If the system server uses the CPolicyServer framework then there are three
candidates for the main integration point.

1) CPolicyServer::CustomSecurityCheckL
This allows an arbitrary and even asynchronous check to be made instead of using
static security policies.

2) CPolicyServer::CustomFailureActionL
This virtual method is invoked if the client fails the static security checks. The
failure may be overridden or deferred. Asynchronous operations are possible.

3) CSession2::ServiceL
The static policy checks could be deferred until the session is created provided that
the server connection API is not guarded by user prompts.

*** Example integration ***
In the example code a combination of (2) and (3) are used because it involves
minimal changes to the existing security policy checks.

**** Steps **** 

1) An RUpsSession object has been added to the CMsgServer class. This is
initialized at startup.

2) CMsgServer::iPolicyElements was changed to cause CustomFailureActionL to be
invoked if the static security policy check fails instead of failing the
client.

3) CMsgServer::CustomFailureActionL notifies the session object that the
platsec check failed. This is important because RUpsSubsession::Authorise
requires this information in order to allow unconfigured systems to be
compatible with platsec. It also increases the flexibility of the UPS policy
definitions.

4) An RUpsSubsession instance is created for each CSession2 object. i.e.
requests from different clients are authorized independently. This could be
modified so that there is one UPS sub-session for each client process instead
of each connection from a client process.

5) An extra state (iAuthorising) was added at the start of the CMsgProcessor
   state machine.

6) The RMessage2 parameters from the client API are now extracted in the
authorization stage to enable the information to be passed to the UPS.

7) CMsgProcessor::DoCancel was updated to cancel the call to
RUpsSubsession::Authorise if the client cancels the sending of the message or
disconnects.

8) CMsgProcessor::RunL now checks whether the request was authorized before
changing from the authorization to the sending state.




