// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#ifndef QTLIBARCHIVE_ARCHIVEENTRY_H
#define QTLIBARCHIVE_ARCHIVEENTRY_H

#include <QDateTime>
#include <QFile>
#include <QtLibArchive/QtLibArchive.h>

#include <optional>

class archive;
class archive_entry;

namespace QtLibArchive {
class QTLIBARCHIVE_EXPORT Entry final
{
    friend class ReaderIterator;
    friend class Writer;

public:
    explicit Entry();
    Entry(const Entry &) = delete;
    Entry(Entry &&other) noexcept;
    ~Entry();

    Entry &operator=(const Entry &) = delete;
    Entry &operator=(Entry &&rhs) noexcept;

    [[nodiscard]] QtLibArchive::FileType fileType() const;
    void setFileType(QtLibArchive::FileType fileType);

    [[nodiscard]] std::optional<QString> pathName() const;
    [[nodiscard]] std::optional<QString> cleanPathName() const;
    void setPathName(const QString &pathName);

    [[nodiscard]] std::optional<qint64> size() const;
    void setSize(qint64 size);

    [[nodiscard]] std::optional<QFile::Permissions> permissions() const;
    void setPermissions(QFile::Permissions permissions);

    [[nodiscard]] bool isValid() const;

private:
    explicit Entry(archive_entry *entry);

    archive_entry *_entry{nullptr};
    bool _ownEntry{false};
};
} // namespace QtLibArchive

#endif
