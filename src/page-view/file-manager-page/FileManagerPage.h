//
// Created by FU-QAQ on 2024/12/11.
//

#ifndef SYNERGYSPOT_FILEMANAGERPAGE_H
#define SYNERGYSPOT_FILEMANAGERPAGE_H

#include "define.h"
#include "ela-widget-tools/ElaScrollPage.h"
#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_FILE_MANAGER_PAGE_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_FILE_MANAGER_PAGE_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif

#define g_pFileManagerPage FileManagerPage::getInstance()

#include <QPushButton>
#include <QVBoxLayout>
#include <ela-widget-tools/ElaProgressBar.h>

#include "file-model/FileModel.h"

#include "common-data/common-dto/CommonDatabaseDTO.h"

class ElaWidget;
class ElaSuggestBox;
class ElaTableView;
class ElaToolButton;

class SS_API FileManagerPage : public ElaScrollPage{
    Q_OBJECT
public:
    static FileManagerPage * getInstance();
    static void destroyFileManagerPage();

    void addFileInfo(FileStorageDTO infoDto);
    void removeFileInfo(QString fileId);

    void addFileDownloadInfo(FileStorageDTO file);
    void updateFileDownloadProgress(QString fileId, double progress);
    void removeFileDownloadInfo(QString fileId);

signals:
    void sigUploadUserSelectFile(const FileStorageDTO& dto);
    void sigDownloadFileRequest(FileStorageDTO dto);
private:
    explicit FileManagerPage(QWidget *parent = nullptr);
    ~FileManagerPage() override;

protected:
    void initConnectFunc();
    void initWindow();
    void initEdgeLayout();
    void initContent();

protected slots:
private:
    // ----------------- UI -----------------
    QWidget                 * _centralWidget       =   nullptr;
    QVBoxLayout             * _mainLayout          =   nullptr;
    ElaSuggestBox           * _search              =   nullptr;
    ElaTableView            * _tableView           =   nullptr;
    FileModel               * _fileModel           =   nullptr;
    ElaToolButton           * _uploadFile          =   nullptr;
    ElaWidget               * _downloadWid         =   nullptr;
    ElaScrollPage           * _downloadScroll      =   nullptr;
    QWidget                 * _downloadScrollWid   =   nullptr;
    QVBoxLayout             * _downloadLayout      =   nullptr;
    ElaToolButton           * _downloadPageButton  =   nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    QHash<QString, QWidget*> _fileIdToDownloadWid;
    QHash<QString, ElaProgressBar*> _fileIdToProgress;
    // --------------- BackEnd --------------

    static FileManagerPage* _fileManagerPage;
};

#endif//SYNERGYSPOT_FILEMANAGERPAGE_H
