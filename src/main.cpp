#include <QApplication>
#include "page/base-arch/arch-page/ArchPage.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ArchPage ap;
    ap.show();
    return QApplication::exec();
}
