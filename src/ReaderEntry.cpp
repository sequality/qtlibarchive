// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#include <QtLibArchive/ReaderEntry.h>

#include <QDir>

#include <archive.h>
#include <archive_entry.h>

namespace QtLibArchive {
ReaderEntry::ReaderEntry(ReaderEntry&& other) noexcept
{
    std::swap(_entry, other._entry);
}

ReaderEntry::~ReaderEntry() {}

ReaderEntry& ReaderEntry::operator=(ReaderEntry&& rhs) noexcept
{
    if (this == &rhs) {
        return *this;
    }

    std::swap(_entry, rhs._entry);

    return *this;
}

QtLibArchive::FileType ReaderEntry::fileType() const
{
    Q_ASSERT(_entry != nullptr);
    return static_cast<QtLibArchive::FileType>(archive_entry_filetype(_entry));
}

std::optional<QString> ReaderEntry::pathName() const
{
    Q_ASSERT(_entry != nullptr);
    const char* pathName = archive_entry_pathname_utf8(_entry);
    return pathName == nullptr ? std::nullopt : std::optional<QString>{QString::fromUtf8(pathName)};
}

std::optional<qint64> ReaderEntry::size() const
{
    Q_ASSERT(_entry != nullptr);
    if (!archive_entry_size_is_set(_entry)) {
        return std::nullopt;
    }

    return archive_entry_size(_entry);
}

std::optional<QDateTime> ReaderEntry::mtime() const
{
    Q_ASSERT(_entry != nullptr);

    if (!archive_entry_mtime_is_set(_entry)) {
        return std::nullopt;
    }

    time_t time = archive_entry_mtime(_entry);
    long nsec = archive_entry_mtime_nsec(_entry);

    return QDateTime::fromTime_t(time).addMSecs(nsec / 1000000);
}

bool ReaderEntry::isValid() const
{
    return _entry != nullptr;
}

ReaderEntry::ReaderEntry(archive_entry* entry)
    : _entry{entry}
{}

std::optional<QString> ReaderEntry::cleanPathName() const
{
    std::optional<QString> value = pathName();
    return value ? std::make_optional(QDir::cleanPath(*value)) : std::nullopt;
}

std::optional<QFileDevice::Permissions> ReaderEntry::permissions() const
{
    Q_ASSERT(_entry != nullptr);

    if (!archive_entry_perm_is_set(_entry)) {
        return std::nullopt;
    }

    QFile::Permissions permissions{};

    auto mode = archive_entry_perm(_entry);

    static const QList<QPair<int, QFile::Permission>> Mapping{
        {S_IRUSR, QFile::ReadUser},
        {S_IWUSR, QFile::WriteUser},
        {S_IXUSR, QFile::ExeUser},
        {S_IRGRP, QFile::ReadGroup},
        {S_IWGRP, QFile::WriteGroup},
        {S_IXGRP, QFile::ExeGroup},
        {S_IROTH, QFile::ReadOther},
        {S_IWOTH, QFile::WriteOther},
        {S_IXOTH, QFile::ExeOther}};

    for (auto [mask, permission] : Mapping) {
        if (mode & mask) {
            permissions |= permission;
        }
    }

    return permissions;
}
} // namespace QtLibArchive
