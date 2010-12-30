#ifndef BUILDGO_GLOBAL_H
#define BUILDGO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BUILDGO_LIBRARY)
#  define BUILDGOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BUILDGOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // BUILDGO_GLOBAL_H
