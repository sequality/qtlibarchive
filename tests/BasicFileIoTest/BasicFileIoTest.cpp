// SPDX-License-Identifier: MIT
// Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

#include <QtTest>

#include <QTemporaryDir>
#include <QTemporaryFile>

#include <QtLibArchive/Reader.h>
#include <QtLibArchive/Writer.h>

class BasicFileIoTest : public QObject
{
    Q_OBJECT

private slots:
    void testCreateTarArchiveAndRead();
    void testUtf8FileNames();
    void testTimeStamps();
};

void BasicFileIoTest::testCreateTarArchiveAndRead()
{
    QTemporaryFile file;
    QVERIFY(file.open());

    {
        QtLibArchive::Writer writer{
            file.fileName(),
            QtLibArchive::SupportedFormat::Tar,
            QtLibArchive::SupportedFilter::None};

        QCOMPARE(writer.error(), QtLibArchive::WriterError::None);

        QByteArray testData("test data");

        QtLibArchive::WriterEntry entry;
        entry.setFileType(QtLibArchive::FileType::Regular);
        entry.setPathName("test.txt");
        entry.setSize(testData.size());

        QVERIFY(writer.writeHeader(entry));
        QVERIFY(writer.writeData(testData));
    }

    {
        QtLibArchive::Reader reader{
            file.fileName(),
            {QtLibArchive::SupportedFormat::All},
            {QtLibArchive::SupportedFilter::All}};
        QCOMPARE(reader.error(), QtLibArchive::ReaderError::None);

        QCOMPARE(reader.fileCount(), 1);
        QCOMPARE(reader.fileData("test.txt"), "test data");
    }
}

void BasicFileIoTest::testUtf8FileNames()
{
    QTemporaryFile archive;
    QVERIFY(archive.open());

    QtLibArchive::Writer writer{
        archive.fileName(), QtLibArchive::SupportedFormat::Zip, QtLibArchive::SupportedFilter::None};
    QCOMPARE(writer.error(), QtLibArchive::WriterError::None);

    QByteArray testData{"Testdaten"};

    QVERIFY(writer.addFile("Wunderschöner Name mit ß und ü.txt", testData));
    writer.close();

    QCOMPARE(writer.error(), QtLibArchive::WriterError::None);

    QtLibArchive::Reader reader{archive.fileName()};
    QCOMPARE(reader.error(), QtLibArchive::ReaderError::None);

    QCOMPARE(reader.fileCount(), 1);

    auto iterator = reader.iterator();
    auto entry = iterator.next();
    QVERIFY(entry.has_value());
    QVERIFY(entry->isValid());
    QVERIFY(entry->cleanPathName().has_value());
    QCOMPARE(entry->cleanPathName(), "Wunderschöner Name mit ß und ü.txt");
    QCOMPARE(entry->size(), testData.size());
}

void BasicFileIoTest::testTimeStamps()
{
    QTemporaryFile archive;
    QVERIFY(archive.open());

    auto now = QDateTime::currentDateTime();

    {
        QtLibArchive::Writer writer{
            archive.fileName(),
            QtLibArchive::SupportedFormat::Zip,
            QtLibArchive::SupportedFilter::None};

        QCOMPARE(writer.error(), QtLibArchive::WriterError::None);

        QByteArray testData{"test data"};

        QtLibArchive::WriterEntry entry;
        entry.setFileType(QtLibArchive::FileType::Regular);
        entry.setPathName("test.txt");
        entry.setSize(testData.size());
        entry.setMtime(now);

        QVERIFY(writer.writeHeader(entry));
        QVERIFY(writer.writeData(testData));
    }

    {
        QtLibArchive::Reader reader{
            archive.fileName(),
            {QtLibArchive::SupportedFormat::All},
            {QtLibArchive::SupportedFilter::All}};
        QCOMPARE(reader.error(), QtLibArchive::ReaderError::None);

        auto iterator = reader.iterator();
        auto entry = iterator.next();

        QVERIFY(entry.has_value());
        QVERIFY(entry->isValid());

        QVERIFY(entry->cleanPathName().has_value());
        QCOMPARE(entry->cleanPathName(), "test.txt");

        QVERIFY(entry->lastModified().has_value());

        // Check that it is precise to the second. Some archive formats do not store milliseconds.
        QCOMPARE(entry->lastModified(), now.addMSecs(-now.time().msec()));
    }
}

QTEST_APPLESS_MAIN(BasicFileIoTest)

#include "BasicFileIoTest.moc"
