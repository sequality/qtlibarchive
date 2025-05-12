// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#ifndef QTLIBARCHIVE_READERENTRY_H
#define QTLIBARCHIVE_READERENTRY_H

#include <QtLibArchive/QtLibArchive.h>

#include <QDateTime>
#include <QFile>

#include <optional>

class archive;
class archive_entry;

namespace QtLibArchive {
class QTLIBARCHIVE_EXPORT ReaderEntry
{
    friend class ReaderIterator;

public:
    ReaderEntry(const ReaderEntry&) = delete;
    ReaderEntry(ReaderEntry&& other) noexcept;
    virtual ~ReaderEntry();

    ReaderEntry& operator=(const ReaderEntry&) = delete;
    ReaderEntry& operator=(ReaderEntry&& rhs) noexcept;

    [[nodiscard]] QtLibArchive::FileType fileType() const;

    [[nodiscard]] std::optional<QString> pathName() const;
    [[nodiscard]] std::optional<QString> cleanPathName() const;

    [[nodiscard]] std::optional<qint64> size() const;

    [[nodiscard]] std::optional<QFile::Permissions> permissions() const;

    /*!
     * Returns the last modification time of the file.    
     */
    [[nodiscard]] std::optional<QDateTime> mtime() const;

    /*!
     * Returns the last modification time of the file.
     * 
     * This is an alias for mtime() using the Qt-style naming.
     */
    [[nodiscard]] std::optional<QDateTime> lastModified() const { return mtime(); }

    [[nodiscard]] bool isValid() const;

protected:
    explicit ReaderEntry(archive_entry* entry);

    archive_entry* _entry{nullptr};
};
} // namespace QtLibArchive

#endif
