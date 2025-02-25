// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#ifndef QTLIBARCHIVE_READERENTRY_H
#define QTLIBARCHIVE_READERENTRY_H

#include <QFile>
#include <QtLibArchive/QtLibArchive.h>

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

    [[nodiscard]] bool isValid() const;

protected:
    explicit ReaderEntry(archive_entry* entry);

    archive_entry* _entry{nullptr};
};
} // namespace QtLibArchive

#endif
