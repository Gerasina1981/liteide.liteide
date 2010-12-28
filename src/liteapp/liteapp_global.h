#ifndef LITEAPP_GLOBAL_H
#define LITEAPP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LITEAPP_LIBRARY)
#  define LITEAPP_EXPORT Q_DECL_EXPORT
#else
#  define LITEAPP_EXPORT Q_DECL_IMPORT
#endif

#endif // LITEAPP_GLOBAL_H