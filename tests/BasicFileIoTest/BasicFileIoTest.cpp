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
    void testFilePermissions();
};

void BasicFileIoTest::testCreateTarArchiveAndRead()
{
    QTemporaryFile file;
    QVERIFY(file.open());

    QByteArray testData{};
    std::generate_n(
        std::back_inserter(testData), QRandomGenerator::system()->bounded(10000), []() -> char {
            return QRandomGenerator::system()->bounded('a', 'z');
        });

    {
        QtLibArchive::Writer writer{
            file.fileName(),
            QtLibArchive::SupportedFormat::Tar,
            QtLibArchive::SupportedFilter::None};

        QCOMPARE(writer.error(), QtLibArchive::WriterError::None);

        QtLibArchive::WriterEntry entry;
        entry.setFileType(QtLibArchive::FileType::Regular);
        entry.setPathName("test.txt");
        entry.setSize(testData.size());
        entry.setPermissions(
            QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadGroup
            | QFileDevice::ReadOther);

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

        QtLibArchive::ReaderIterator iterator = reader.iterator();
        std::optional<QtLibArchive::ReaderEntry> entry = iterator.next();
        QVERIFY(entry.has_value());
        QCOMPARE(entry->cleanPathName(), "test.txt");
        QCOMPARE(entry->size(), testData.size());
        QCOMPARE(
            entry->permissions(),
            QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadGroup
                | QFileDevice::ReadOther);

        QCOMPARE(iterator.readData(), testData);
    }
}

void BasicFileIoTest::testFilePermissions()
{
    QTemporaryFile file;
    QVERIFY(file.open());

    QList<QFileDevice::Permission> allPermissions{
        QFileDevice::ReadOwner,
        QFileDevice::WriteOwner,
        QFileDevice::ExeOwner,
        QFileDevice::ReadGroup,
        QFileDevice::WriteGroup,
        QFileDevice::ExeGroup,
        QFileDevice::ReadOther,
        QFileDevice::WriteOther,
        QFileDevice::ExeOther};

    QList<QFileDevice::Permissions> combinations{{}};

    for (QFileDevice::Permission permission : allPermissions) {
        QList<QFileDevice::Permissions> nextCombinations{};

        for (QFileDevice::Permissions& combination : combinations) {
            QFileDevice::Permissions next = combination | permission;
            nextCombinations.push_back(next);
        }

        combinations << nextCombinations;
    }
    QCOMPARE(combinations.size(), 2 << (allPermissions.length() - 1));

    {
        QtLibArchive::Writer writer{
            file.fileName(),
            QtLibArchive::SupportedFormat::Tar,
            QtLibArchive::SupportedFilter::None};

        QCOMPARE(writer.error(), QtLibArchive::WriterError::None);

        writer.addDirectory("./");

        for (QFileDevice::Permissions permissions : combinations) {
            QString permissionsStr
                = QString::number(permissions, 16).rightJustified(4, QLatin1Char('0'));
            QString pathName = QString{"./file.%1.bin"}.arg(permissionsStr);

            QtLibArchive::WriterEntry entry;
            entry.setFileType(QtLibArchive::FileType::Regular);
            entry.setPathName(pathName);
            entry.setSize(permissionsStr.length());
            entry.setPermissions(permissions);

            QVERIFY(writer.writeHeader(entry));
            QVERIFY(writer.writeData(permissionsStr.toUtf8()));
        }
    }

    {
        QtLibArchive::Reader reader{file.fileName()};
        QCOMPARE(reader.error(), QtLibArchive::ReaderError::None);

        auto it = reader.iterator();

        while (auto entry = it.next()) {
            if (entry->fileType() != QtLibArchive::FileType::Regular) {
                continue;
            }

            auto actualPermissions = *entry->permissions();
            QCOMPARE(entry->size(), 4);
            bool ok = false;
            QByteArray data = it.readData();
            auto expectedPermissions = static_cast<QFileDevice::Permissions>(data.toInt(&ok, 16));
            QVERIFY(ok);
            QCOMPARE(actualPermissions, expectedPermissions);
        }
    }
}

QTEST_APPLESS_MAIN(BasicFileIoTest)

#include "BasicFileIoTest.moc"
