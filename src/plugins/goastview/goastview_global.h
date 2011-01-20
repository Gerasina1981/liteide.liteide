#ifndef GOASTVIEW_GLOBAL_H
#define GOASTVIEW_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GOASTVIEW_LIBRARY)
#  define GOASTVIEWSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GOASTVIEWSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GOASTVIEW_GLOBAL_H
