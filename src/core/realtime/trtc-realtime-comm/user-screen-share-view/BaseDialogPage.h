//
// Created by FU-QAQ on 2025/4/4.
//

#ifndef BASEDIALOGPAGE_H
#define BASEDIALOGPAGE_H

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include <QDialog>

#ifdef _WIN32
namespace wndproc_setting {
    void initAndSetWndProc(HWND hwnd);
}
#endif //_WIN32

class BaseDialogPage : public QDialog
{
    Q_OBJECT

public:
    explicit BaseDialogPage(QWidget* parent = nullptr);
    ~BaseDialogPage();

    virtual void resetUI();
protected:
    //UI-related
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void changeEvent(QEvent* event) override;
private:
    virtual void retranslateUi() = 0;
    virtual void updateDynamicTextUI();
#ifdef _WIN32
private:
    void registerWndProc(HWND hwnd);
    void recoverWndProc(HWND hwnd);
#endif //_WIN32
};

#endif //BASEDIALOGPAGE_H
