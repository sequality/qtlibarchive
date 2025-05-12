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
        {QFileDevice::ReadOwner, S_IRUSR},
        {QFileDevice::WriteOwner, S_IWUSR},
        {QFileDevice::ExeOwner, S_IXUSR},
        {QFileDevice::ReadGroup, S_IRGRP},
        {QFileDevice::WriteGroup, S_IWGRP},
        {QFileDevice::ExeGroup, S_IXGRP},
        {QFileDevice::ReadOther, S_IROTH},
        {QFileDevice::WriteOther, S_IWOTH},
        {QFileDevice::ExeOther, S_IXOTH}};

    for (auto [permission, mask] : Mapping) {
        if (permissions.testFlag(permission)) {
            mode |= mask;
        }
    }

    Q_ASSERT(_entry != nullptr);
    archive_entry_set_perm(_entry, mode);
}

void WriterEntry::setMtime(const QDateTime& mtime)
{
    Q_ASSERT(_entry != nullptr);

    // time_t contains seconds since epoch. QDateTime has milliseconds resolution.
    // We need to convert the rest of the milliseconds to nanoseconds.
    long nsec = (mtime.toMSecsSinceEpoch() - mtime.toSecsSinceEpoch() * 1000) * 1000000;

    archive_entry_set_mtime(_entry, mtime.toTime_t(), nsec);
}

} // namespace QtLibArchive
