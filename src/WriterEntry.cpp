// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#include <QtLibArchive/WriterEntry.h>

#include <QDir>

#include <archive.h>
#include <archive_entry.h>

namespace QtLibArchive {
WriterEntry::WriterEntry()
    : ReaderEntry{archive_entry_new()}
{}

WriterEntry::WriterEntry(WriterEntry&& other) noexcept
    : ReaderEntry{std::move(other)}
{}

WriterEntry::~WriterEntry()
{
    if (_entry != nullptr) {
        archive_entry_free(_entry);
        _entry = nullptr;
    }
}

WriterEntry& WriterEntry::operator=(WriterEntry&& rhs) noexcept
{
    if (this == &rhs) {
        return *this;
    }

    std::swap(_entry, rhs._entry);

    return *this;
}

void WriterEntry::setFileType(FileType fileType)
{
    Q_ASSERT(_entry != nullptr);
    archive_entry_set_filetype(_entry, static_cast<int>(fileType));
}

void WriterEntry::setPathName(const QString& pathName)
{
    QByteArray utf8Data = pathName.toUtf8();
    Q_ASSERT(_entry != nullptr);
    archive_entry_set_pathname_utf8(_entry, utf8Data.constData());
}

void WriterEntry::setSize(qint64 size)
{
    Q_ASSERT(_entry != nullptr);
    archive_entry_set_size(_entry, size);
}

void WriterEntry::setPermissions(QFileDevice::Permissions permissions)
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

} // namespace QtLibArchive
