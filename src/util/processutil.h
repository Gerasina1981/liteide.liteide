#ifndef PROCESSUTIL_H
#define PROCESSUTIL_H

#include <QProcess>

inline QByteArray processErrorName(QProcess::ProcessError code)
{
    static QByteArray err;
    switch (code) {
    case QProcess::FailedToStart:
        err = "File not found, resouce error";
        break;
    case QProcess::Crashed:
        err = "Crashed";
        break;
    case QProcess::Timedout:
        err = "Time out";
        break;
    case QProcess::ReadError:
        err = "Read error";
        break;
    case QProcess::WriteError:
        err = "Write error";
        break;
    case QProcess::UnknownError:
    default:
        err = "Unknown error";
    }
    return err;
}

#endif // PROCESSUTIL_H
