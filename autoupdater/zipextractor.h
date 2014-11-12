#ifndef ZIPEXTRACTOR_H
#define ZIPEXTRACTOR_H

#include <QObject>

class ZipExtractor : public QObject
{
    Q_OBJECT
public:
    explicit ZipExtractor(QObject *parent = 0);

    void setArchiveFilePath(const QString &archiveFilePath);
    QString archiveFilePath() const;

    QString error() const;

public slots:
    void start();

signals:
    void finished();

private:
    static bool extractArchive(const QString &archiveFilePath,
                               const QString &extDirPath,
                               const QString &singleFileName = QString());

    QString m_archiveFilePath;
    QString m_error;
};

#endif // ZIPEXTRACTOR_H
