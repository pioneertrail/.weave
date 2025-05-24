#ifndef CHRONOVYAN_VARIANT_FIX_H
#define CHRONOVYAN_VARIANT_FIX_H

// Fix for MinGW implementation of std::variant
#if defined(__MINGW32__) || defined(__MINGW64__)
#define _GLIBCXX_USE_CXX11_ABI 0
#include <variant>
#else
#include <variant>
#endif

#endif // CHRONOVYAN_VARIANT_FIX_H 