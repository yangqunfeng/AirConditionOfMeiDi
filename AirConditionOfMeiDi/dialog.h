#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "QSerialPort"
#include "qserialportinfo.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    QSerialPort*    m_serial;
    QString         m_sPortName;


    bool serialportscan();
    bool serialOpen();
private slots:
    void on_Open_hot_clicked();
    QString formatInput(QString hexStr);
    QByteArray QString2Hex(QString hexStr);
    char ConvertHexChar(char c);
    void on_hot_30_clicked();

    void on_Close_clicked();

    void on_Open_Serial_clicked();

    void on_Open_cold_clicked();

    void on_cold_24_clicked();

    void on_cold_26_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
