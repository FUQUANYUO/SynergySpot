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

void FileModel::addFileInfo(FileStorageDTO fileInfo) {}

void FileModel::removeFileInfo(const QString &fileId) {}

int FileModel::rowCount(const QModelIndex &parent) const {
    return 0;
}

int FileModel::columnCount(const QModelIndex &parent) const {
    return _header.count();
}

QVariant FileModel::data(const QModelIndex &index, int role) const {
    return QVariant();
}

QVariant FileModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return _header[section];
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}