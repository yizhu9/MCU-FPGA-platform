#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>
#include <QDateTime>

class QextSerialPort;

namespace Ui {
class frmMain;
}

class frmMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private:
    Ui::frmMain *ui;

    bool isOpen;
    bool isStart;
    QByteArray buffer;
    QDateTime lastTime;
    QextSerialPort *com;
    QTimer *timerRead;

    int currentCount;
    QStringList plot_data;
    QVector<double> plot_keys;
    QVector<double> plot_values;
    QVector<double> plot_keys_current;
    QVector<double> plot_values_current;
    qint16 strHexToShort(const QString &strHex);

    int pageCount;
    int pageCurrent;

private slots:
    void initForm();
    void initPlot();
    void initConfig();
    void saveConfig();
    void changeResult();
    void loadPlot();
    void readData();
    void writeData();
    void checkData();
    void showData();

private slots:
    void on_btnOpen_clicked();
    void on_btnSend_clicked();
    void on_btnSave_clicked();
    void on_btnLoad_clicked();
    void on_btnFirst_clicked();
    void on_btnPre_clicked();
    void on_btnNext_clicked();
    void on_btnLast_clicked();
};

#endif // FRMMAIN_H
