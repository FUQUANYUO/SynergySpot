//
// Created by FU-QAQ on 2024/12/11.
//

#include "FileManagerPage.h"
#include "common-data/CommonData.h"

#include "uuid/GenUUID.h"

#include "ela-widget-tools/ElaToolButton.h"
#include "ela-widget-tools/ElaSuggestBox.h"
#include "ela-widget-tools/ElaTableView.h"
#include "ela-widget-tools/ElaMenu.h"
#include "ela-widget-tools/ElaScrollArea.h"
#include "ela-widget-tools/ElaWidget.h"

#include <QDateTime>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QTimer>
#include <mutex>
#include <ela-widget-tools/ElaProgressBar.h>


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

void FileManagerPage::addFileInfo(FileStorageDTO infoDto) {
    _fileModel->addFileInfo(infoDto);
}

void FileManagerPage::removeFileInfo(QString fileId) {
    _fileModel->removeFileInfo(fileId);
}

void FileManagerPage::addFileDownloadInfo(FileStorageDTO file) {
    if (_fileIdToDownloadWid.contains(file.fileId))return;
    QWidget* wid = new QWidget(_downloadScrollWid);
    QGridLayout * layout = new QGridLayout(wid);

    wid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    wid->setFixedHeight(70);
    // file name
    QLabel * name = new QLabel(wid);
    name->setText(file.fileName);

    // progress bar
    ElaProgressBar * progressBar = new ElaProgressBar(wid);
    progressBar->setMaximum(100);

    // pause/resume and cancel button
    ElaToolButton * pauseOrResumeButton = new ElaToolButton(wid);
    ElaToolButton * cancelButton = new ElaToolButton(wid);
    pauseOrResumeButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    pauseOrResumeButton->setElaIcon(ElaIconType::CirclePause);
    pauseOrResumeButton->setFixedSize(32,32);
    cancelButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    cancelButton->setElaIcon(ElaIconType::Xmark);
    cancelButton->setFixedSize(32,32);

    layout->addWidget(name,0,0,1,8);
    layout->addWidget(progressBar,1,0,1,6);
    layout->addWidget(pauseOrResumeButton,1,6,1,1);
    layout->addWidget(cancelButton,1,7,1,1);

    connect(pauseOrResumeButton, &ElaToolButton::clicked,this, [=]() {
        auto curIcon = pauseOrResumeButton->property("ElaIconType").toInt();
        if (curIcon == ElaIconType::CirclePlay) {
            pauseOrResumeButton->setElaIcon(ElaIconType::CirclePause);
        }
        else {
            pauseOrResumeButton->setElaIcon(ElaIconType::CirclePlay);
        }
    });

    connect(cancelButton, &ElaToolButton::clicked,this, [=]() {
        removeFileDownloadInfo(file.fileId);
    });

    _fileIdToDownloadWid.insert(file.fileId,wid);
    _fileIdToProgress.insert(file.fileId,progressBar);

    _downloadLayout->insertWidget(0,wid);
}

void FileManagerPage::updateFileDownloadProgress(QString fileId, double progress) {
    // if (progress >= 100)
    _fileIdToProgress.value(fileId)->setValue(progress);
}

void FileManagerPage::removeFileDownloadInfo(QString fileId) {
    auto targetWid = _fileIdToDownloadWid.value(fileId);
    _downloadLayout->removeWidget(targetWid);
    _fileIdToDownloadWid.remove(fileId);
    _fileIdToProgress.remove(fileId);
    delete targetWid;
    targetWid = nullptr;
}

FileManagerPage::FileManagerPage(QWidget *parent) : ElaScrollPage(parent) {
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}

FileManagerPage::~FileManagerPage() {
    delete _downloadWid;
    _downloadWid = nullptr;
}

void FileManagerPage::initWindow() {
    setTitleVisible(false);
    _centralWidget      = new QWidget(this);
    _mainLayout         = new QVBoxLayout(_centralWidget);
    _search             = new ElaSuggestBox(this);
    _tableView          = new ElaTableView(this);
    _fileModel          = new FileModel(this);
    _uploadFile         = new ElaToolButton(this);
    _downloadWid        = new ElaWidget();
    _downloadScroll     = new ElaScrollPage(_downloadWid);
    _downloadScrollWid  = new QWidget(_downloadScroll);
    _downloadLayout     = new QVBoxLayout(_downloadScrollWid);
    _downloadPageButton = new ElaToolButton(this);
}

void FileManagerPage::initEdgeLayout() {
    _centralWidget->setContentsMargins(0,0,0,0);
    addCentralWidget(_centralWidget);

    QHBoxLayout * buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(_search);
    buttonLayout->addStretch();
    buttonLayout->addWidget(_uploadFile);
    buttonLayout->addWidget(_downloadPageButton);

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
    _tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    _tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    _downloadScrollWid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _downloadWid->setWindowTitle("文件下载列表");
    _downloadWid->setWindowButtonFlag(ElaAppBarType::StayTopButtonHint,false);
    _downloadWid->setWindowButtonFlag(ElaAppBarType::ThemeChangeButtonHint,false);
    _downloadWid->setWindowButtonFlag(ElaAppBarType::MinimizeButtonHint,false);
    _downloadWid->setWindowButtonFlag(ElaAppBarType::MaximizeButtonHint,false);
    _downloadWid->setWindowButtonFlag(ElaAppBarType::CloseButtonHint,false);
    _downloadWid->setWindowFlags(Qt::Popup | Qt::NoDropShadowWindowHint);
    _downloadWid->setFixedSize(600,300);
    QHBoxLayout * downloadWidLayout = new QHBoxLayout(_downloadWid);
    _downloadScroll->setCustomWidget(_downloadScrollWid);

    downloadWidLayout->setContentsMargins(0,0,0,0);
    downloadWidLayout->addWidget(_downloadScroll);

    _downloadPageButton->setFixedSize(60,40);
    _downloadPageButton->setText("下载列表");
    _downloadPageButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _downloadPageButton->setElaIcon(ElaIconType::LaptopArrowDown);
}

void FileManagerPage::initConnectFunc() {
    connect(_tableView, &QTableView::customContextMenuRequested, this, [=](const QPoint &pos) {
        QModelIndex index = _tableView->indexAt(pos);
        if (!index.isValid())
          return;

        ElaMenu menu;
        QAction *deleteAction = menu.addAction("删除文件");

        connect(deleteAction, &QAction::triggered, this, [this, index]() {
          QString fileId = _fileModel->getFileId(index.row());
          _fileModel->removeFileInfo(fileId);
        });

        menu.exec(_tableView->viewport()->mapToGlobal(pos));
    });

    connect(_uploadFile, &ElaToolButton::clicked, this, [=]() {
        QString filePath = QFileDialog::getOpenFileName(this, "弹窗标题",
                                                   "D://","*.*");

        if (filePath.isEmpty()) {
            return;
        }
        QFileInfo fileInfo(filePath);
        qint64 fileSizeBytes = fileInfo.size();

        FileStorageDTO fileDto {
            QString::fromStdString(GenUUID::getInstance()->generateUUID("upload")),
            g_pCommonData->getCurUserInfo().ssid,
            fileInfo.fileName(),
            fileSizeBytes,
            fileInfo.suffix(),
            filePath,
            QDateTime::currentSecsSinceEpoch()
        };

        emit sigUploadUserSelectFile(fileDto);
    });

    connect(_tableView,&QTableView::doubleClicked, this, [=](const QModelIndex &index) {
        auto fileId = _fileModel->getFileId(index.row());
        if (fileId.isEmpty())return;
        addFileDownloadInfo(_fileModel->getFileData(fileId));

        // emit to grpc download it
        emit sigDownloadFileRequest(_fileModel->getFileData(fileId));
    });

    connect(_downloadPageButton, &ElaToolButton::clicked, this, [=]() {
        // 获取按钮的左下角全局坐标
        QPoint buttonBottomLeft = _downloadPageButton->mapToGlobal(QPoint(0, _downloadPageButton->height()));
        QPoint popupPos = buttonBottomLeft - QPoint(_downloadWid->width(), 0);

        _downloadWid->move(popupPos);
        _downloadWid->show();
    });
}