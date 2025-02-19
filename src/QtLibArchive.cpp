// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#include <QtLibArchive/QtLibArchive.h>

#include <QDebug>

namespace QtLibArchive {
constexpr const char *readerErrorStr(ReaderError readerError)
{
    switch (readerError) {
    case ReaderError::None:
        return "None";
    case ReaderError::CannotAllocateMemory:
        return "CannotAllocateMemory";
    case ReaderError::FormatNotSupported:
        return "FormatNotSupported";
    case ReaderError::FilterNotSupported:
        return "FilterNotSupported";
    case ReaderError::CannotOpenFile:
        return "CannotOpenFile";
    case ReaderError::CannotReadData:
        return "CannotReadData";
    }

    return "";
}

constexpr const char *writerErrorStr(WriterError writerError)
{
    switch (writerError) {
    case WriterError::None:
        return "None";
    }

    return "";
}

QDebug operator<<(QDebug dbg, ReaderError readerError)
{
    QDebugStateSaver stateSaver{dbg};
    dbg.nospace().noquote() << "ReaderError::" << readerErrorStr(readerError);
    return dbg;
}

QDebug operator<<(QDebug dbg, WriterError writerError)
{
    QDebugStateSaver stateSaver{dbg};
    dbg.nospace().noquote() << "WriterError::" << writerErrorStr(writerError);
    return dbg;
}
} // namespace QtLibArchive
