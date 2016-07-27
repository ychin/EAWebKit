

#include <stddef.h>


#if defined(_XBOX)      // XBox platform
    #include <stdlib.h>
    #include <tchar.h>

    const _TSCHAR * __cdecl _tgetenv_helper_nolock(const _TSCHAR* option)
    {
        return NULL;
    }

    char* getenv(const char* name)
    {
        return NULL;
    }

#elif defined(__SNC__) || (defined(__ELF__) && defined(__x86_64__) && defined(__clang__)) // Sony platforms
    char* getenv(const char* name)
    {
        return NULL;
    }
#endif






