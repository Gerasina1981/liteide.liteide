#ifndef GOLANGPRO_GLOBAL_H
#define GOLANGPRO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GOLANGPRO_LIBRARY)
#  define GOLANGPROSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GOLANGPROSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GOLANGPRO_GLOBAL_H
