#ifndef EASYMSG_EXPORT_H
#define EASYMSG_EXPORT_H

#ifdef EASYMSG_SHARED_BUILD
#ifdef _WIN32

#ifdef EASYMSG_EXPORTS
#define EASYMSG_API __declspec(dllexport)
#else
#define EASYMSG_API __declspec(dllimport)
#endif // EASYMSG_EXPORTS

#else // NOT _WIN32

#if defined(__GNUC__)
#define EASYMSG_API __attribute__((visibility("default")))
#endif // __GNUC__

#endif // NOT _WIN32 __GNUC__

#else
#define EASYMSG_API
#endif

#endif // EASTMSG_EXPORT_H
