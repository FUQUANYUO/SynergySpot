//
// Created by FU-QAQ on 2024/12/11.
//

#include "FileManagerPage.h"

#include "ela-widget-tools/ElaToolButton.h"
#include "ela-widget-tools/ElaSuggestBox.h"
#include "ela-widget-tools/ElaTableView.h"

#include <QHeaderView>
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
    setTitleVisible(false);
    _centralWidget = new QWidget(this);
    _mainLayout    = new QVBoxLayout(_centralWidget);
    _search        = new ElaSuggestBox(this);
    _tableView     = new ElaTableView(this);
    _fileModel     = new FileModel(this);
    _uploadFile    = new ElaToolButton(this);
}

void FileManagerPage::initEdgeLayout() {
    _centralWidget->setContentsMargins(0,0,0,0);
    addCentralWidget(_centralWidget);

    QHBoxLayout * buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(_search);
    buttonLayout->addStretch();
    buttonLayout->addWidget(_uploadFile);

    _mainLayout->addLayout(buttonLayout);
    _mainLayout->addWidget(_tableView);
}

void FileManagerPage::initContent() {
    _centralWidget->setWindowTitle("FileManager");

    _search->setPlaceholderText("搜索文件...");
    _uploadFile->setFixedSize(60,40);
    _uploadFile->setText("上传文件");
    _uploadFile->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _uploadFile->setElaIcon(ElaIconType::FolderArrowUp);

    _tableView->setModel(_fileModel);
    _tableView->setAlternatingRowColors(true);
    _tableView->setIconSize(QSize(38, 38));
    _tableView->verticalHeader()->setHidden(true);
    _tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    _tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _tableView->horizontalHeader()->setMinimumSectionSize(60);
    _tableView->verticalHeader()->setMinimumSectionSize(46);
    _tableView->setFixedHeight(450);
}

void FileManagerPage::initConnectFunc() {
}