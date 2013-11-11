How To Run TNETDB test case manually (Windows Environment)

(1) One has to replace epoc.ini (this epoc.ini was provided by Networking team) file which has WIN_TUNNEL field. 
This field has networking test network IP address and port number.

(2) One has to build //PR/share/DABSRelease/buildtools/wintunnel/eka2/wins/... component.
So that wintunnel application will redirect the HostResolver request to Networking Test Network.

How To Run TNETDB test case manually (Hardware)

(1) Include StdLibTests.iby at the time of building ROM.
(2) Start TCP2SER application in the PC to which H2/H4 hardware is connected.
Please refer to (http://loncoredev01.intra/twiki/bin/view/ToolsAndUtilities/TCP2SerTool) link for further detail.
(3) In the hardware, go to z:\test directory and run TNETDB. Before running TNETDB, TCP2SER must be started as that
application will be responsible for re-directing the request to Test Network.

NOTE: The above epoc.ini (which has WINTUNNEL info) and wintunnel application are needed at the time of testing in Emulator but not in hardware.
At the time of testing in hardware TCP2SER will be used in place of wintunnel. 

Why use Networking Test Environment ?

GetHostByName and GetHostByAddr uses RHostResolver class which internally sends the request to internet to get the info.
But networking team is using their own test environment to simulate above things. As we are calling their functions to
do our stuff we need their environment too.

GetHostByName and GetHostByAddr might return multiple host names for a single address or
multiple addresses for a single host name. All these Host reolve info was stored in their 
test environment. So it is easy to test and compare the test result as the addresses were not changed.

Few Reasons TNETDB might fail.

(1) In the test code, addresses and host names are hard-coded. So check wheter the test is failing
at the the time of comparing the results. It might be due to change of addresses and host names in test database.

If you are testing in Emulator:

(2) As epoc.ini was copied once from networking team. Cross check whether epoc.ini file is still relevant or not.
Or Networking team has changed their testing strategy. (As this test case is closely associated with networking test environment).

If you are testing in Hardware:

(2) Check whether you have correct config.xml file in TCP2SER directory. For detail please refer to
http://loncoredev01.intra/twiki/bin/view/ToolsAndUtilities/TCP2SerTool
