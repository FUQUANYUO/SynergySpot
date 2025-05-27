//
// Created by FU-QAQ on 2025/5/5.
//

#include "FileModel.h"

#include "common-data/common-dto/CommonDatabaseDTO.h"

FileModel::FileModel(QObject *parent) : QAbstractTableModel(parent){
    _header << "文件名";
    _header << "文件大小";
    _header << "来源";
    _header << "上传日期";
}

FileModel::~FileModel() {}

QString FileModel::getFileId(int row) {
    if (row >= 0 && row < _fileList.size())
        return _fileList.at(row).fileId;
    return QString();
}

FileStorageDTO FileModel::getFileData(QString fileId) {
    for (auto file : _fileList) {
        if (file.fileId == fileId) {
            return file;
        }
    }
    return {};
}

void FileModel::addFileInfo(FileStorageDTO fileInfo) {
    beginInsertRows(QModelIndex(), _fileList.size(), _fileList.size());
    _fileList.append(fileInfo);
    endInsertRows();
}

void FileModel::removeFileInfo(const QString &fileId) {
    auto it = _fileList.begin();
    while (it != _fileList.end()) {
        if (it->fileId == fileId) {
            int row = std::distance(_fileList.begin(), it);

            // 通知视图即将删除数据
            beginRemoveRows(QModelIndex(), row, row);

            // 删除元素并更新迭代器
            it = _fileList.erase(it);

            // 通知视图删除完成
            endRemoveRows();
            break;
        }
        ++it;
    }
}

int FileModel::rowCount(const QModelIndex &parent) const {
    return _fileList.count();
}

int FileModel::columnCount(const QModelIndex &parent) const {
    return _header.count();
}

QVariant FileModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    // 先处理 DisplayRole，确保数据能返回
    if (role == Qt::DisplayRole) {
        int row = index.row();
        int col = index.column();

        if (row < 0 || row >= _fileList.count() || col < 0 || col >= _header.count())
            return QVariant();

        const FileStorageDTO &info = _fileList.at(row);
        switch (col) {
            case 0: return info.fileName;
            case 1: return formatSize(info.fileSize);
            case 2: return info.uploaderSsid;
            case 3: return QDateTime::fromSecsSinceEpoch(info.uploadTime).toString();
            default: return QVariant();
        }
    }

    // 再处理对齐方式
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }
    if (role == Qt::ToolTipRole) {  // 设置 ToolTip
        int row = index.row();
        int col = index.column();
        const FileStorageDTO &info = _fileList.at(row);

        switch (col) {
            case 0: return QString("文件名: %1").arg(info.fileName);
            case 1: return QString("文件大小: %1").arg(formatSize(info.fileSize));
            case 2: return QString("上传者: %1").arg(info.uploaderSsid);
            case 3: return QString("上传时间: %1").arg(QDateTime::fromSecsSinceEpoch(info.uploadTime).toString());
            default: return QVariant();
        }
    }


    return QVariant();
}

QVariant FileModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return _header[section];
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QString FileModel::formatSize(qint64 bytes) const {
    const qint64 kb = 1024;
    const qint64 mb = kb * 1024;
    const qint64 gb = mb * 1024;

    if (bytes >= gb)
        return QString("%1 GB").arg(bytes / (double)gb, 0, 'f', 2);
    else if (bytes >= mb)
        return QString("%1 MB").arg(bytes / (double)mb, 0, 'f', 2);
    else if (bytes >= kb)
        return QString("%1 KB").arg(bytes / (double)kb, 0, 'f', 2);
    else
        return QString("%1 B").arg(bytes);
}