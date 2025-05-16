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

    void addFileInfo(FileStorageDTO fileInfo);
    void removeFileInfo(const QString& fileId);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
private:
    QStringList _header;
};



#endif //FILEMODEL_H
