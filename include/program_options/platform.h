// ----------------------------------------------------------------------------
#ifndef __program_options__platform_h__
#define __program_options__platform_h__
// ----------------------------------------------------------------------------

#ifdef _WIN32

    // Visual Studio does not support noexcept.
    #define noexcept

#else
    // Assume GCC - TODO: Need to check version should probably be >= 4.7
#endif

// ----------------------------------------------------------------------------
#endif // __program_options__platform_h__