// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#ifndef QTLIBARCHIVE_ARCHIVEREADER_H
#define QTLIBARCHIVE_ARCHIVEREADER_H

#include <QtLibArchive/Entry.h>
#include <QtLibArchive/ReaderIterator.h>
#include <QtLibArchive/QtLibArchive.h>

#include <QList>

namespace QtLibArchive {
class ReaderIterator;

class QTLIBARCHIVE_EXPORT Reader
{
public:
    explicit Reader(
        QString fileName,
        QList<SupportedFormat> supportedFormats = {SupportedFormat::All},
        QList<SupportedFilter> supportedFilters = {SupportedFilter::All});

    explicit Reader(
        SupportedFormat supportedFormat,
        SupportedFilter supportedFilter);

    ~Reader();

    [[nodiscard]] QString fileName() const;
    [[nodiscard]] QList<SupportedFormat> supportedFormats() const;
    [[nodiscard]] QList<SupportedFilter> supportedFilters() const;
    [[nodiscard]] ReaderError error() const;
    [[nodiscard]] qint64 fileCount();

    bool open(const QString &fileName, qint64 blockSize = 10240);

    [[nodiscard]] ReaderIterator iterator() const;

    [[nodiscard]] std::optional<QByteArray> fileData(const QString &pathName) const;

private:
    QString _fileName;
    QList<SupportedFormat> _supportedFormats{SupportedFormat::All};
    QList<SupportedFilter> _supportedFilters{SupportedFilter::All};
    qint64 _blockSize{10240};
    ReaderError _error{ReaderError::None};
    std::optional<qint64> _fileCount{std::nullopt};
};
} // namespace QtLibArchive

#endif
