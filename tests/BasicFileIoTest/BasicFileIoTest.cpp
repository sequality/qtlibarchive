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

QTEST_APPLESS_MAIN(BasicFileIoTest)

#include "BasicFileIoTest.moc"
