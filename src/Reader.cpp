// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#include <QtLibArchive/Reader.h>
#include <QtLibArchive/ReaderIterator.h>

#include <archive.h>

#include <QDir>

namespace QtLibArchive {
Reader::Reader(
    QString fileName,
    QList<SupportedFormat> supportedFormats,
    QList<SupportedFilter> supportedFilters)
    : _fileName{std::move(fileName)}
    , _supportedFormats{std::move(supportedFormats)}
    , _supportedFilters{std::move(supportedFilters)}
{
    _error = iterator().error();
}

Reader::Reader(SupportedFormat supportedFormat, SupportedFilter supportedFilter)
    : _supportedFormats{supportedFormat}
    , _supportedFilters{supportedFilter}
{
}


Reader::~Reader()
{
}

QString Reader::fileName() const
{
    return _fileName;
}

QList<SupportedFormat> Reader::supportedFormats() const
{
    return _supportedFormats;
}

QList<SupportedFilter> Reader::supportedFilters() const
{
    return _supportedFilters;
}

qint64 Reader::fileCount()
{
    if (!_fileCount.has_value()) {
        qint64 count = 0;
        auto it = iterator();
        while (it.next()) {
            count++;
        }
        _fileCount = count;
    }

    return _fileCount.value();
}

bool Reader::open(const QString &fileName, qint64 blockSize)
{
    _fileName = fileName;
    _blockSize = blockSize;
    _error = iterator().error();

    return _error == ReaderError::None;
}

ReaderIterator Reader::iterator() const
{
    return ReaderIterator{this, _blockSize};
}

std::optional<QByteArray> Reader::fileData(const QString &pathName) const
{
    QString cleanPathName = QDir::cleanPath(pathName);

    ReaderIterator it{iterator()};

    while (it.next()) {
        if (it.entry().cleanPathName() == cleanPathName) {
            return it.readData();
        }
    }

    return std::nullopt;
}
} // namespace QtLibArchive
