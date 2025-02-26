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

    static const QList<QPair<int, QFileDevice::Permission>> Mapping{
        {S_IRUSR, QFileDevice::ReadOwner},
        {S_IWUSR, QFileDevice::WriteOwner},
        {S_IXUSR, QFileDevice::ExeOwner},
        {S_IRGRP, QFileDevice::ReadGroup},
        {S_IWGRP, QFileDevice::WriteGroup},
        {S_IXGRP, QFileDevice::ExeGroup},
        {S_IROTH, QFileDevice::ReadOther},
        {S_IWOTH, QFileDevice::WriteOther},
        {S_IXOTH, QFileDevice::ExeOther}};

    for (auto [mask, permission] : Mapping) {
        if (mode & mask) {
            permissions |= permission;
        }
    }

    return permissions;
}
} // namespace QtLibArchive
