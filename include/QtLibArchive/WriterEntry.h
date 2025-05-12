// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#ifndef QTLIBARCHIVE_WRITERENTRY_H
#define QTLIBARCHIVE_WRITERENTRY_H

#include <QtLibArchive/QtLibArchive.h>
#include <QtLibArchive/ReaderEntry.h>

#include <QDateTime>

namespace QtLibArchive {
class QTLIBARCHIVE_EXPORT WriterEntry : public ReaderEntry
{
    friend class Writer;

public:
    explicit WriterEntry();
    WriterEntry(const WriterEntry&) = delete;
    WriterEntry(WriterEntry&& other) noexcept;
    ~WriterEntry() override;

    WriterEntry& operator=(const WriterEntry&) = delete;
    WriterEntry& operator=(WriterEntry&& rhs) noexcept;

    void setFileType(QtLibArchive::FileType fileType);
    void setPathName(const QString& pathName);
    void setSize(qint64 size);
    void setPermissions(QFile::Permissions permissions);

    /*! Set the last modified time of the entry.
     *  
     *  This method corresponds to libarchive's archive_entry_set_mtime.
     *  
     * \param mtime The last modified time to set.
     */
    void setMtime(const QDateTime& mtime);

    /*! Set the last modified time of the entry.
     *  
     *  This method is a convenience alias for setMtime with Qt style naming.
     *  
     * \param mtime The last modified time to set.
     */
    void setLastModified(const QDateTime& mtime) { setMtime(mtime); }
};
} // namespace QtLibArchive

#endif
