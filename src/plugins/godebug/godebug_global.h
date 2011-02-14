#ifndef GODEBUG_GLOBAL_H
#define GODEBUG_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GODEBUG_LIBRARY)
#  define GODEBUGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GODEBUGSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GODEBUG_GLOBAL_H
