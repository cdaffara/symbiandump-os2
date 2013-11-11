The cryptography libraries should not build thumb.  

On EKA1 they cannot build on thumb because the long long support files (supplied by base) contain instructions that are not supported on that platform.  This has the knock-on effect that those components that include bigint code in their build (and thus require the long long support files too) also cannot build thumb.  The entire list of components affected is as follows:

the cryptography library cryptography.dll
tasn1 (use CopyL, operator ==, operator < and operator *=) 
tx509, twtlscert (both use operator < and > which are not exported from TInteger) 


On EKA2 it should not build thumb either, because it has been deemed to be too slow.  

Thus the cryptography library must be built on the ARMi platform to substitute for the thumb binary where a thumb set of binaries is required (eg for Lubbock roms).  The ARMi build should occur first in the build order.  To speed up the build process, I've stopped our testcode building ARMi where it's not required to do so.

Some information (from Chris Mokes) about how to prevent the cryptography library building on thumb, and how to manage ROM building follows:


(a)	To stop thumb being built do this in the PRJ_MMPFILES section of   bld.inf

#if !defined(MARM_THUMB)
	mmpfilename
#endif

or I think you can specify -THUMB in bld.inf but that'll stop everything being build for thumb.


(b)	You need to build the dll for ARMI, but still build a thumb lib file for thumb binaries which use the dll to be linked with.  Thumb and armi dlls are interchangable, armi code linked with the armi lib can call exports in a thumb dll and the other way around.

As you've seen the rombuild downgrades to armi if the thumb binary isn't present., so if you have built the armi version and not the thumb one it will pick up the correct one.

Euser has its own define EUSER_ABI in the rombuilding scripts.  EUSER_ABI is ARM4 in an ARM4 build and ARMI for thumb and armi builds. 

The nicest solution is to add to epoc32\rom\include\header.iby to define a CRYPTO_ABI which will be basically the same as the euser one.

Thus,

header.iby 
<snip>
#ifdef _ARM4
define DESIRED_ABI     ARM4
define EUSER_ABI       ARM4
define ELOCL_ABI       ARM4
define CRYPTO_ABI      ARM4
#endif

#ifdef _ARMI
define DESIRED_ABI     ARMI
define EUSER_ABI       ARMI
define ELOCL_ABI       ARM4
define CRYPTO_ABI      ARMI
#endif

#ifdef _THUMB
define DESIRED_ABI     THUMB
define EUSER_ABI       ARMI
define ELOCL_ABI       ARM4
define CRYPTO_ABI      ARMI
#endif
<snip>


Then use  CRYPTO_ABI rather than ABI_DIR  in cryptalg.iby.

So, the simplest way is the just build armi and arm4 and the armi one will be downgraded to in a thumb rom.  This is the situation at present.  The most complex but best is to use the define CRYPTO_ABI as explained above.  This cannot be done until the cryptography libraries are submitted to the build and the Master Codeline accepts divergence from 7.0s release.
