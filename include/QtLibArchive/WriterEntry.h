// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#ifndef QTLIBARCHIVE_WRITERENTRY_H
#define QTLIBARCHIVE_WRITERENTRY_H

#include <QtLibArchive/QtLibArchive.h>
#include <QtLibArchive/ReaderEntry.h>

namespace QtLibArchive {
class QTLIBARCHIVE_EXPORT WriterEntry : public ReaderEntry
{
    friend class Writer;

public:
    explicit WriterEntry();
    WriterEntry(const WriterEntry&) = delete;
    WriterEntry(WriterEntry&& other) noexcept;
    ~WriterEntry() override;

    WriterEntry& operator=(const WriterEntry&) = delete;
    WriterEntry& operator=(WriterEntry&& rhs) noexcept;

    void setFileType(QtLibArchive::FileType fileType);
    void setPathName(const QString& pathName);
    void setSize(qint64 size);
    void setPermissions(QFile::Permissions permissions);
};
} // namespace QtLibArchive

#endif
