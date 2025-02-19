// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#include <QtLibArchive/Entry.h>

#include <QDir>

#include <archive.h>
#include <archive_entry.h>

namespace QtLibArchive {
Entry::Entry()
    : _entry{archive_entry_new()}
      , _ownEntry{true}
{
}

Entry::Entry(Entry &&other) noexcept
{
    std::swap(_entry, other._entry);
    std::swap(_ownEntry, other._ownEntry);
}

Entry::~Entry()
{
    if (_ownEntry && _entry != nullptr) {
        archive_entry_free(_entry);
    }
}

Entry &Entry::operator=(Entry &&rhs) noexcept
{
    if (this == &rhs) {
        return *this;
    }

    std::swap(_entry, rhs._entry);
    std::swap(_ownEntry, rhs._ownEntry);

    return *this;
}

QtLibArchive::FileType Entry::fileType() const
{
    Q_ASSERT(_entry != nullptr);
    return static_cast<QtLibArchive::FileType>(archive_entry_filetype(_entry));
}

void Entry::setFileType(FileType fileType)
{
    Q_ASSERT(_entry != nullptr);
    archive_entry_set_filetype(_entry, static_cast<int>(fileType));
}

std::optional<QString> Entry::pathName() const
{
    Q_ASSERT(_entry != nullptr);
    const char *pathName = archive_entry_pathname_utf8(_entry);
    return pathName == nullptr
               ? std::nullopt
               : std::optional<QString>{QString::fromUtf8(pathName)};
}

void Entry::setPathName(const QString &pathName)
{    
    QByteArray utf8Data = pathName.toUtf8();
    Q_ASSERT(_entry != nullptr);
    archive_entry_set_pathname_utf8(_entry, utf8Data.constData());
}

std::optional<qint64> Entry::size() const
{
    Q_ASSERT(_entry != nullptr);
    if (!archive_entry_size_is_set(_entry)) {
        return std::nullopt;
    }

    return archive_entry_size(_entry);
}

void Entry::setSize(qint64 size)
{
    Q_ASSERT(_entry != nullptr);
    archive_entry_set_size(_entry, size);
}

void Entry::setPermissions(QFileDevice::Permissions permissions)
{    
    mode_t mode{};

    static const QList<QPair<QFile::Permission, int>> Mapping{
        {QFile::ReadUser, S_IRUSR},
        {QFile::WriteUser, S_IWUSR},
        {QFile::ExeUser, S_IXUSR},
        {QFile::ReadGroup, S_IRGRP},
        {QFile::WriteGroup, S_IWGRP},
        {QFile::ExeGroup, S_IXGRP},
        {QFile::ReadOther, S_IROTH},
        {QFile::WriteOther, S_IWOTH},
        {QFile::ExeOther, S_IXOTH}};

    for (auto [permission, mask] : Mapping) {
        if (permissions & permission) {
            mode |= mask;
        }
    }

    Q_ASSERT(_entry != nullptr);
    archive_entry_set_perm(_entry, mode);
}

bool Entry::isValid() const
{
    return _entry != nullptr;
}

std::optional<QString> Entry::cleanPathName() const
{    
    std::optional<QString> value = pathName();
    return value ? std::make_optional(QDir::cleanPath(*value)) : std::nullopt;
}

std::optional<QFileDevice::Permissions> Entry::permissions() const
{
    Q_ASSERT(_entry != nullptr);

    if (!archive_entry_perm_is_set(_entry)) {
        return std::nullopt;
    }

    QFile::Permissions permissions{};

    auto mode = archive_entry_perm(_entry);

    static const QList<QPair<int, QFile::Permission> > Mapping{{S_IRUSR, QFile::ReadUser},
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

Entry::Entry(archive_entry *entry)
    : _entry{entry}
{
}
} // namespace QtLibArchive
