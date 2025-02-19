// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#ifndef QTLIBARCHIVE_QTLIBARCHIVE_H
#define QTLIBARCHIVE_QTLIBARCHIVE_H

#include <QtCore/qglobal.h>

#include <limits>

#ifdef QTLIBARCHIVE_LIBRARY
#define QTLIBARCHIVE_EXPORT Q_DECL_EXPORT
#else
#define QTLIBARCHIVE_EXPORT Q_DECL_IMPORT
#endif

namespace QtLibArchive {
enum class SupportedFilter
{
    None,
    Gzip,
    Bzip2,
    Compress,
    Program,
    Lzma,
    Xz,
    Uu,
    Rpm,
    Lzip,
    Lrzip,
    Lzop,
    Grzip,
    Lz4,
    Zstd,

    All = -1,
};

enum class SupportedFormat
{
    Cpio = 0x10000,
    CpioPosix = (Cpio | 1),
    CpioBinLe = (Cpio | 2),
    CpioBinBe = (Cpio | 3),
    CpioSvr4Nocrc = (Cpio | 4),
    CpioSvr4Crc = (Cpio | 5),
    CpioAfioLarge = (Cpio | 6),
    CpioPwb = (Cpio | 7),

    Shar = 0x20000,
    SharBase = (Shar | 1),
    SharDump = (Shar | 2),

    Tar = 0x30000,
    TarUstar = (Tar | 1),
    TarPaxInterchange = (Tar | 2),
    TarPaxRestricted = (Tar | 3),
    TarGnu = (Tar | 4),

    Iso9660 = 0x40000,
    Iso9660Rockridge = (Iso9660 | 1),

    Zip = 0x50000,
    Empty = 0x60000,

    Ar = 0x70000,
    ArGnu = (Ar | 1),
    ArBsd = (Ar | 2),

    Mtree = 0x80000,
    Raw = 0x90000,
    Xar = 0xA0000,
    Lha = 0xB0000,
    Cab = 0xC0000,
    Rar = 0xD0000,
    SevenZip = 0xE0000,
    Warc = 0xF0000,
    RarV5 = 0x100000,

    All = -1,
};

enum class FileType
{
    Reg = 0100000,
    Lnk = 0120000,
    Sock = 0140000,
    Chr = 0020000,
    Blk = 0060000,
    Dir = 0040000,
    Fifo = 0010000,

    // Human-readable aliases
    Regular = Reg,
    Link = Lnk,
    Socket = Sock,
    Character = Chr,
    Block = Blk,
    Directory = Dir,

    Unknown = std::numeric_limits<std::underlying_type_t<FileType>>::max()
};

enum class ReaderError
{
    None,
    CannotAllocateMemory,
    FormatNotSupported,
    FilterNotSupported,
    CannotOpenFile,
    CannotReadData
};

QTLIBARCHIVE_EXPORT QDebug operator<<(QDebug dbg, ReaderError error);

enum class WriterError {
    None,
    CannotAllocateMemory,
    CannotOpenFile,
    CannotSetFormat,
    CannotAddFilter,
    CannotWriteHeader,
    CannotWriteData,
    InvalidEntry,
    UnexpectedFileType,
};

QTLIBARCHIVE_EXPORT QDebug operator<<(QDebug dbg, WriterError error);
} // namespace QtLibArchive

#endif
