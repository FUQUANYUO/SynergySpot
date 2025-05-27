//
// Created by FU-QAQ on 2025/5/5.
//

#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QAbstractTableModel>

struct FileStorageDTO;

class FileModel : public QAbstractTableModel{
    Q_OBJECT
public:
    explicit FileModel(QObject* parent = nullptr);
    ~FileModel();

    QString getFileId(int row);
    FileStorageDTO getFileData(QString fileId);

    void addFileInfo(FileStorageDTO fileInfo);
    void removeFileInfo(const QString& fileId);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
private:
    QString formatSize(qint64 bytes) const;
private:
    QStringList                     _header;
    QList<FileStorageDTO>           _fileList; // 存储文件信息列表
};



#endif //FILEMODEL_H
