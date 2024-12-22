//
// Created by FU-QAQ on 2024/12/11.
//

#include "FileManagerPage.h"

#include <mutex>


FileManagerPage * FileManagerPage::_fileManagerPage = nullptr;
static std::mutex m;

FileManagerPage *FileManagerPage::getInstance() {
    if(_fileManagerPage == nullptr){
        m.lock();
        if(_fileManagerPage == nullptr){
            _fileManagerPage = new FileManagerPage();
        }
        m.unlock();
    }
    return _fileManagerPage;
}
void FileManagerPage::destroyFileManagerPage() {
    if(_fileManagerPage == nullptr)
        return ;
    else{
        m.lock();
        delete _fileManagerPage;
        _fileManagerPage = nullptr;
        m.unlock();
    }
}
FileManagerPage::FileManagerPage(QWidget *parent) : ElaScrollPage(parent) {
    initWindow();

    initEdgeLayout();

    initContent();

    initContent();
}
FileManagerPage::~FileManagerPage() {
}

void FileManagerPage::initWindow() {
    _centralWidget = new QWidget(this);
}

void FileManagerPage::initEdgeLayout() {
    _centralWidget->setContentsMargins(0,0,0,0);
    addCentralWidget(_centralWidget);
}

void FileManagerPage::initContent() {
    _centralWidget->setWindowTitle("FileManager");
}

void FileManagerPage::initConnectFunc() {
}