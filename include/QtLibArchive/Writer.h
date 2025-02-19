// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#ifndef QTLIBARCHIVE_ARCHIVEWRITER_H
#define QTLIBARCHIVE_ARCHIVEWRITER_H

#include <QString>
#include <QIODevice>

#include <QtLibArchive/Entry.h>

#include <memory>

namespace QtLibArchive {
class WriterPrivate;

class QTLIBARCHIVE_EXPORT Writer final
{
public:
    explicit Writer(const QString &filePath, SupportedFormat format, SupportedFilter filter);
    ~Writer();

    bool writeHeader(const Entry& entry);
    bool writeData(const QByteArray& data);
    bool writeData(QIODevice *device);

    bool addDirectory(const QString &path);
    bool addFile(const QString &sourceFilePath, const QString &targetFilePath);

    void close();

    [[nodiscard]] WriterError error() const;
    [[nodiscard]] qint64 fileCount() const;

    /*! Block size to read/write data in. This affects the behavior of addFile and writeData(QIODevice*). */
    [[nodiscard]] qint64 blockSize() const;
    void setBlockSize(qint64 blockSize);

private:
    Q_DECLARE_PRIVATE(Writer);
    std::unique_ptr<WriterPrivate> d_ptr;
};
} // namespace QtLibArchive

#endif //ARCHIVEWRITER_H
