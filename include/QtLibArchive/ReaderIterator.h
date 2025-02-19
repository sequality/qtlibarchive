// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#ifndef QTLIBARCHIVE_READERITERATOR_H
#define QTLIBARCHIVE_READERITERATOR_H

#include <QtLibArchive/Entry.h>
#include <QtLibArchive/QtLibArchive.h>

#include <QFileDevice>

#include <memory>
#include <optional>

namespace QtLibArchive {
class Reader;
class ReaderIteratorPrivate;

class QTLIBARCHIVE_EXPORT ReaderIterator final
{
    friend class Reader;

public:
    ReaderIterator(const ReaderIterator &) = delete;
    ReaderIterator(ReaderIterator &&other) noexcept;
    ~ReaderIterator();

    ReaderIterator &operator=(const ReaderIterator &) = delete;
    ReaderIterator &operator=(ReaderIterator &&rhs) noexcept;

    [[nodiscard]] std::optional<const Entry> next();
    void close();

    [[nodiscard]] bool isValid() const;
    [[nodiscard]] QByteArray readData(std::optional<qint64> maxSize = std::nullopt) const;

    [[nodiscard]] ReaderError error() const;

    [[nodiscard]] const Entry entry() const;

private:
    ReaderIterator(const Reader *reader, qint64 blockSize);

    Q_DECLARE_PRIVATE(ReaderIterator);
    std::unique_ptr<ReaderIteratorPrivate> d_ptr;
};
} // namespace QtLibArchive

#endif
