#include "frmmain.h"
#include "quiwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setFont(QFont("Microsoft Yahei", 9));

    QUIHelper::setCode();
    QUIHelper::setTranslator();
    QUIHelper::setStyle(QUIWidget::Style_PSBlack);

    QUIConfig::ConfigFile = QUIHelper::appPath() + "/PTool_config.ini";
    QUIConfig::readConfig();

    QUIWidget qui;
    qui.setMainWidget(new frmMain);
    qui.setTitle("数据采集波形显示工具");
    qui.setIcon(QUIWidget::Lab_Ico, QChar(0xf201), 11);
    qui.show();

    return a.exec();
}
