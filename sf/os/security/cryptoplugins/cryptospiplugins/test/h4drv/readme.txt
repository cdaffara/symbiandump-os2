H4 crypto acceleration
======================

This directory tree contains a "proof of concept" H4 hardware
acceleration device driver (crypto_h4) and the code required to plug
it into the preq57 framework (crypto_h4_plugin).

Support
-------

NOT SUPPORTED!

It is important to note that this plugin/driver is prototype test
code, and is therefore NOT supported in any way.

In particular it is NOT a reference driver.


Limitations
-----------

Currently the driver has several major limitations:-

1) It is currently slower than the original software implementation...

This is believed to be due to the following issues:

  a) Even using 100% of the H4 CPU power in a tight loop to write/read
  data to the crypto hardware only runs the driver test case about 30%
  faster than the old software crypto.

  b) The driver currently fakes DMA transfers for every block (ie. 16
  bytes).

  Using real DMA, AND setting up longer DMAs may well cause a
  reduction in CPU loading and increase the throughput of the device
  driver.

2) Whilst the driver does handle multiple simultaneous user requests,
including scheduling between them to prevent starvation issues, some
areas of the device driver are inefficient:-

  a) Both CryptoH4JobRandom and CryptoH4JobAes maintain buffers
  per-channel, even if that channel has never been used or is stalled.

  Most buffering should be moved to be common to all RNG/AES jobs,
  with the job objects only saving data needed to later resume a job.

  b) Scheduling for AES jobs is limited to when a job has stalled
  (ie. either drained all input data or filled the output buffer)

     Theoretically this could allow starvation problems. It is not
     clear if this is a real issue or not.  It may be better to change
     the scheduling to only allow rescheduling between device driver
     requests, rather than within them.

End of notes
