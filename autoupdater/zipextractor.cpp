#include "zipextractor.h"

#include "defaults.h"

#include <quazip.h>
#include <quazipfile.h>

#include <QFile>
#include <QDir>
#include <QFileInfo>

#include <QDebug>

ZipExtractor::ZipExtractor(QObject *parent) :
    QObject(parent)
{
}

void ZipExtractor::setArchiveFilePath(const QString &archiveFilePath)
{
    m_archiveFilePath = archiveFilePath;
}

QString ZipExtractor::archiveFilePath() const
{
    return m_archiveFilePath;
}

QString ZipExtractor::error() const
{
    return m_error;
}

void ZipExtractor::start()
{
    QFileInfo fileInfo(m_archiveFilePath);
    QDir archiveDir(fileInfo.absoluteDir());

    if (archiveDir.exists(kUpdatesDirName))
    {
        archiveDir.cd(kUpdatesDirName);
        if (!archiveDir.removeRecursively())
        {
            m_error = "Cannot remove old updates dir";
            emit finished();
            return;
        }
        archiveDir.cdUp();
    }

    if (!archiveDir.mkdir(kUpdatesDirName))
    {
        m_error = "Cannot create updates folder";
        emit finished();
        return;
    }

    archiveDir.cd(kUpdatesDirName);
    if (!extractArchive(m_archiveFilePath, archiveDir.absolutePath()))
    {
        m_error = "Cannot extract updates from archive";
        emit finished();
        return;
    }

    QFile::remove(m_archiveFilePath);
    emit finished();
}

bool ZipExtractor::extractArchive(const QString &archiveFilePath,
                                  const QString &extDirPath,
                                  const QString &singleFileName)
{
    qDebug() << "file path: " << archiveFilePath
             << " extDirPath " << extDirPath
             << " singleFileName " << singleFileName;

    QuaZip zip(archiveFilePath);

    if (!zip.open(QuaZip::mdUnzip))
    {
        qWarning("Cannot open zip archive, error: %d", zip.getZipError());
        return false;
    }

    zip.setFileNameCodec("IBM866");

    QuaZipFileInfo info;

    QuaZipFile file(&zip);

    QFile out;
    QString name;
    char c;
    for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile())
    {
        if (!zip.getCurrentFileInfo(&info))
        {
            qWarning("Cannot get zip file info, error: %d", zip.getZipError());
            return false;
        }

        if (info.name.endsWith("/"))
        {
            QDir dir(extDirPath);
            dir.mkpath(info.name);
            continue;
        }

        if (!singleFileName.isEmpty())
            if (!info.name.contains(singleFileName))
                continue;

        if (!file.open(QIODevice::ReadOnly))
        {
            qWarning("Cannot open zip file, error: %d", file.getZipError());
            return false;
        }

        name = QString("%1/%2").arg(extDirPath).arg(file.getActualFileName());

        if (file.getZipError() != UNZ_OK)
        {
            qWarning("Cannot get actual file name, error: %d", file.getZipError());
            return false;
        }

        out.setFileName(name);

        // this will fail if "name" contains subdirectories, but we don't mind that
        out.open(QIODevice::WriteOnly);
        // Slow like hell (on GNU/Linux at least), but it is not my fault.
        // Not ZIP/UNZIP package's fault either.
        // The slowest thing here is out.putChar(c).
        while (file.getChar(&c))
            out.putChar(c);

        out.close();

        if (file.getZipError() != UNZ_OK)
        {
            qWarning("Read zip file error: %d", file.getZipError());
            return false;
        }

        if (!file.atEnd())
        {
            qWarning("Read all data from zip file but it's not EOF");
            return false;
        }

        file.close();

        if (file.getZipError() != UNZ_OK)
        {
            qWarning("Cannot close zip file, error: %d", file.getZipError());
            return false;
        }
    }

    zip.close();

    if (zip.getZipError() != UNZ_OK)
    {
        qWarning("Cannot close zip archive: %d", zip.getZipError());
        return false;
    }

    return true;
}
