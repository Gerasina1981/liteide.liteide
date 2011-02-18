#ifndef GODOC_GLOBAL_H
#define GODOC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GODOC_LIBRARY)
#  define GODOCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GODOCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GODOC_GLOBAL_H
