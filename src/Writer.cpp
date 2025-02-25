// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#include <QtLibArchive/Writer.h>

#include <QBuffer>
#include <QFileInfo>

#include <archive.h>

namespace QtLibArchive {
class WriterPrivate
{
    friend class Writer;

public:
    WriterPrivate(const QString& filePath, SupportedFormat format, QList<SupportedFilter> filters)
        : _filePath{filePath}
        , _format{format}
        , _filters{std::move(filters)}
        , _archive{archive_write_new()}
    {
        if (_archive == nullptr) {
            _error = WriterError::CannotAllocateMemory;
            return;
        }

        if (archive_write_set_format(_archive, static_cast<int>(format)) != ARCHIVE_OK) {
            _error = WriterError::CannotSetFormat;
            return;
        }

        for (SupportedFilter filter : _filters) {
            // Might also be ARCHIVE_WARN.
            if (archive_write_add_filter(_archive, static_cast<int>(filter)) == ARCHIVE_FATAL) {
                _error = WriterError::CannotAddFilter;
                return;
            }
        }

        std::wstring fileName = filePath.toStdWString();
        if (archive_write_open_filename_w(_archive, fileName.c_str()) != ARCHIVE_OK) {
            _error = WriterError::CannotOpenFile;
            return;
        }
    }

    QString _filePath;
    SupportedFormat _format{SupportedFormat::Empty};
    QList<SupportedFilter> _filters;
    WriterError _error{WriterError::None};
    qint64 _fileCount{0};
    qint64 _blockSize{10240};

    archive* _archive{nullptr};
};

Writer::Writer(const QString& filePath, SupportedFormat format, SupportedFilter filter)
    : d_ptr{new WriterPrivate{filePath, format, {filter}}}
{}

Writer::~Writer()
{
    close();
}

bool Writer::writeHeader(const WriterEntry& entry)
{
    Q_D(Writer);

    if (d->_error != WriterError::None) {
        return false;
    }

    if (!entry.isValid()) {
        d->_error = WriterError::InvalidEntry;
        return false;
    }

    int r = archive_write_header(d->_archive, entry._entry);

    if (r != ARCHIVE_OK) {
        d->_error = WriterError::CannotWriteHeader;
        return false;
    }

    d->_fileCount++;
    return true;
}

bool Writer::writeData(const QByteArray& data)
{
    Q_D(Writer);

    if (d->_error != WriterError::None) {
        return false;
    }

    int written = archive_write_data(d->_archive, data.constData(), data.size());

    if (written != data.size()) {
        d->_error = WriterError::CannotWriteData;
        return false;
    }

    return true;
}

bool Writer::writeData(QIODevice* device)
{
    Q_D(Writer);

    if (d->_error != WriterError::None) {
        return false;
    }

    while (!device->atEnd()) {
        QByteArray data = device->read(d->_blockSize);

        if (!writeData(data)) {
            return false;
        }
    }

    return true;
}

bool Writer::addDirectory(const QString& path, QFileDevice::Permissions permissions)
{
    Q_D(Writer);

    if (d->_error != WriterError::None) {
        return false;
    }

    WriterEntry archiveEntry;
    archiveEntry.setFileType(FileType::Dir);
    archiveEntry.setPathName(path);
    archiveEntry.setPermissions(permissions);

    return writeHeader(archiveEntry);
}

bool Writer::addFile(
    const QString& pathInArchive, QIODevice* device, QFileDevice::Permissions permissions)
{
    Q_D(Writer);

    if (d->_error != WriterError::None) {
        return false;
    }

    WriterEntry archiveEntry;
    archiveEntry.setFileType(FileType::Regular);
    archiveEntry.setPathName(pathInArchive);
    archiveEntry.setPermissions(permissions);
    archiveEntry.setSize(device->size());

    if (!device->isOpen() && !device->open(QIODevice::ReadOnly)) {
        d->_error = WriterError::CannotOpenFile;
        return false;
    }

    if (!writeHeader(archiveEntry)) {
        return false;
    }

    if (!writeData(device)) {
        return false;
    }

    return true;
}

bool Writer::addFile(
    const QString& pathInArchive, const QByteArray& data, QFileDevice::Permissions permissions)
{
    QBuffer buffer;
    buffer.setData(data);

    return addFile(pathInArchive, &buffer, permissions);
}

void Writer::close()
{
    Q_D(Writer);

    if (d->_archive) {
        archive_write_close(d->_archive);
        archive_write_free(d->_archive);
        d->_archive = nullptr;
    }
}

WriterError Writer::error() const
{
    Q_D(const Writer);
    return d->_error;
}

qint64 Writer::blockSize() const
{
    Q_D(const Writer);
    return d->_blockSize;
}

void Writer::setBlockSize(qint64 blockSize)
{
    Q_D(Writer);
    d->_blockSize = blockSize;
}
} // namespace QtLibArchive
