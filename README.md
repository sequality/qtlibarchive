# QtLibArchive

QtLibArchive is a [Qt](https://github.com/qt) wrapper for [libarchive](https://github.com/libarchive/libarchive). QtLibArchive requires C++17.

# Programming Examples

Some of the examples correspond to the original [libarchive examples](https://github.com/libarchive/libarchive/wiki/Examples).

## List contents of Archive stored in File

Here's a very basic example that simply opens a file and lists the contents of the archive:

```c++
QtLibArchive::Reader a{SupportedFormat::All, SupportedFilter::All}; 
if (!a.open("archive.tar", 10240)) {
    QCoreApplication::exit(1);
}

QtLibArchive::ReaderIterator it = a.iterator(); 
while (std::optional<const QtLibArchive::Entry> entry = it.next()) {
    qDebug() << entry->pathName();

    QByteArray data = entry->readData(); // Note 2
}

// Note 1
```

Note 1: Closing or deallocating is not needed. Resource management is automatic. 

Note 2: It is possible to read data in chunks if the expected file size is too big. You may read until the read data is zero: 

```c++
QtLibArchive::ReaderIterator it = a.iterator(); 

while (std::optional<const QtLibArchive::Entry> entry = it.next()) {
    QByteArray chunk = entry->readData(10240);
    while (chunk.size() > 0)
        chunk = entry->readData(10240);
    }
}
```

## A Basic Write Example

```c++
void writeArchive(QString outname, QStringList filenames) {
    QtLibArchive::Writer a{outname, SupportedFormat::PaxRestricted SupportedFilter::GZip};

    for (QString filename: filenames) {
        // Note 1
        a.addFile(filename, QFileInfo{filename}.completeFileName()); 
    }
}

int main(int argc, const char* argv[]) {
    QString outname{argv[1]};
    QStringList filenames;
    for (int i = 2; i < argc; ++i) {
        filenames.push_back(argv[i]);
    }
    writeArchive(outname, filenames);
}
```

Note 1: `addFile` is a helper function reading an existing file on the file system and putting it into the archive under the specifed name preserving permissions. For finer-grained control you can use `writeHeader` and `writeData`. 