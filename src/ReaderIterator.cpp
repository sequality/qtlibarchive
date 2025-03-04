// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#include <QtLibArchive/Reader.h>
#include <QtLibArchive/ReaderIterator.h>

#include <archive.h>
#include <archive_entry.h>

#include <QDir>

#include "archive_entry.h"

namespace QtLibArchive {
class ReaderIteratorPrivate
{
    friend class ReaderIterator;

public:
    ReaderIteratorPrivate(const Reader* reader, qint64 blockSize)
        : _reader{reader}
        , _blockSize{blockSize}
        , _archive{archive_read_new()}
    {
        Q_ASSERT(reader != nullptr);

        if (_archive == nullptr) {
            _error = ReaderError::CannotAllocateMemory;
            return;
        }

        if (reader->supportedFormats().contains(SupportedFormat::All)) {
            archive_read_support_format_all(_archive);
        } else {
            for (SupportedFormat format : reader->supportedFormats()) {
                if (archive_read_support_format_by_code(_archive, static_cast<int>(format))
                    != ARCHIVE_OK) {
                    _error = ReaderError::FormatNotSupported;
                    break;
                }
            }
        }

        if (reader->supportedFilters().contains(SupportedFilter::All)) {
            archive_read_support_filter_all(_archive);
        } else {
            for (SupportedFilter filter : reader->supportedFilters()) {
                if (archive_read_support_filter_by_code(_archive, static_cast<int>(filter))
                    != ARCHIVE_OK) {
                    _error = ReaderError::FilterNotSupported;
                    break;
                }
            }
        }

        if (archive_read_open_filename_w(
                _archive, reader->fileName().toStdWString().c_str(), _blockSize)
            != ARCHIVE_OK) {
            _error = ReaderError::CannotOpenFile;
        }
    }

private:
    const Reader* _reader{nullptr};
    qint64 _blockSize{10240};
    archive* _archive{nullptr};
    archive_entry* _archiveEntry{nullptr};
    bool _isValid{false};
    ReaderError _error{ReaderError::None};
};

ReaderIterator::ReaderIterator(ReaderIterator&& other) noexcept
{
    std::swap(d_ptr, other.d_ptr);
}

ReaderIterator::~ReaderIterator()
{
    close();
}

ReaderIterator& ReaderIterator::operator=(ReaderIterator&& rhs) noexcept
{
    if (this == &rhs) {
        return *this;
    }

    std::swap(d_ptr, rhs.d_ptr);
    return *this;
}

std::optional<ReaderEntry> ReaderIterator::next()
{
    Q_D(ReaderIterator);

    d->_isValid = (archive_read_next_header(d->_archive, &d->_archiveEntry) == ARCHIVE_OK);
    return d->_isValid ? std::make_optional(ReaderEntry{d->_archiveEntry}) : std::nullopt;
}

void ReaderIterator::close()
{
    Q_D(ReaderIterator);

    if (d->_archive != nullptr) {
        archive_read_close(d->_archive);
        archive_read_free(d->_archive);
        d->_archive = nullptr;
        d->_isValid = false;
    }
}

bool ReaderIterator::isValid() const
{
    Q_D(const ReaderIterator);
    return d->_isValid;
}

QByteArray ReaderIterator::readData(std::optional<qint64> maxSize) const
{
    Q_D(const ReaderIterator);
    Q_ASSERT(d->_isValid);

    QByteArray data;

    if (maxSize) {
        data.resize(*maxSize);
    } else {
        auto expectedSize = entry().size();

        if (expectedSize) {
            data.resize(*expectedSize);
        }
    }

    qint64 read = archive_read_data(d->_archive, data.data(), data.size());
    data.resize(read);

    return data;
}

ReaderError ReaderIterator::error() const
{
    Q_D(const ReaderIterator);
    return d->_error;
}

ReaderEntry ReaderIterator::entry() const
{
    Q_D(const ReaderIterator);
    return ReaderEntry{d->_archiveEntry};
}

ReaderIterator::ReaderIterator(const Reader* reader, qint64 blockSize)
    : d_ptr{new ReaderIteratorPrivate{reader, blockSize}}
{}
} // namespace QtLibArchive
