#ifndef SetupH
#define SetupH

#pragma warning(disable: 4251) // class 'std::xxx<_Ty>' needs to have dll-interface to be used by clients of class
#pragma warning(disable: 4018) // signed/unsigned mismatch


// Generic helper definitions for shared library support
#if defined WIN32 || defined _WIN32 || defined __CYGWIN__
  #define SCIDI_HELPER_DLL_IMPORT __declspec(dllimport)
  #define SCIDI_HELPER_DLL_EXPORT __declspec(dllexport)
  #define SCIDI_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define SCIDI_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define SCIDI_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define SCIDI_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define SCIDI_HELPER_DLL_IMPORT
    #define SCIDI_HELPER_DLL_EXPORT
    #define SCIDI_HELPER_DLL_LOCAL
  #endif
#endif

// Now we use the generic helper definitions above to define SCIDI_API and SCIDI_LOCAL.
// SCIDI_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// SCIDI_LOCAL is used for non-api symbols.

#ifdef SCIDI_DLL // defined if SCIDI is compiled as a DLL
  #ifdef SCIDI_DLL_EXPORTS // defined if we are building the SCIDI DLL (instead of using it)
    #define SCIDI_API SCIDI_HELPER_DLL_EXPORT
  #else
    #define SCIDI_API SCIDI_HELPER_DLL_IMPORT
  #endif // SCIDI_DLL_EXPORTS
  #define SCIDI_LOCAL SCIDI_HELPER_DLL_LOCAL
#else // SCIDI_DLL is not defined: this means SCIDI is a static lib.
  #define SCIDI_API
  #define SCIDI_LOCAL
#endif // SCIDI_DLL



typedef int code_t;

#define MYMAXSTR 1024
#define VERSION_WITH_GAPS 1 // it's a const, not a parametr

#define USE_VILCO_LIBRARY 0

//#define TEST_LOG

#endif
