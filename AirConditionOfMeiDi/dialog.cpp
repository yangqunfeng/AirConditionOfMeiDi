#include "dialog.h"
#include "ui_dialog.h"
#include "QMessageBox"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    bool res = serialportscan();
    if (res == false)
    {
        exit(0);
    }
    serialOpen();

}

bool Dialog::serialportscan()
{
    m_serial = new QSerialPort();
    int icount = 0;
    if(m_serial != NULL)
    {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            if(info.description() == "USB-SERIAL CH340")
            {
                icount += 1;
                QSerialPort ser;
                ser.setPort(info);
                if(ser.open(QIODevice::ReadWrite))
                {
                    m_sPortName =  ser.portName();
                    ser.close();
                }
            }
        }
        if (icount == 0)
        {
            QMessageBox::warning(this,"失败","打开串口失败，请先插入模块后再打开本软件或先安装USB TO TTL驱动！");
            return false;
        }
        else
        {
            return true;
        }
    }
}

bool Dialog::serialOpen()
{
    if(m_serial->isOpen())
    {
        m_serial->close();
    }
    QString PortName = m_sPortName;
    m_serial->setPortName(PortName);
    m_serial->setBaudRate(QSerialPort::Baud115200);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);



//    m_serial->setReadBufferSize(1024);

    if(m_serial->open(QIODevice::ReadWrite) == true)
    {
//        QObject::connect(m_serial,&QSerialPort::readyRead,this,SerialR);
        return true;
    }
    else
    {

        return false;
    }

}


Dialog::~Dialog()
{
    delete ui;
}


//将输入格式化，补满四位：0XFFFF
QString Dialog::formatInput(QString hexStr)
{
    int strlen = hexStr.length();
    switch(strlen)
    {
        case 0:
            hexStr = "0000";
            break;
        case 1:
            hexStr = "000" + hexStr;
            break;
        case 2:
            hexStr = "00" + hexStr;
            break;
        case 3:
            hexStr = "0" + hexStr;
            break;
        case 4:
            hexStr = hexStr;
            break;
        default:
            break;
    }
    return hexStr;
}

QByteArray Dialog::QString2Hex(QString hexStr)
{
    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = hexStr.length();
    senddata.resize(len/2);
    char lstr, hstr;
    for(int i = 0; i < len; )
    {
        //将第一个不为' '的字符赋给hstr;
        hstr = hexStr[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        //当i >= len时，跳出循环
        if(i >= len)
            break;
        //当i < len时，将下一个字符赋值给lstr;
        lstr = hexStr[i].toLatin1();
        //将hstr和lstr转换为0-15的对应数值
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        //
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata * 16 + lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}

//将单个字符串转换为hex
//0-F -> 0-15
char Dialog::ConvertHexChar(char c)
{
    if((c >= '0') && (c <= '9'))
        return c - 0x30;
    else if((c >= 'A') && (c <= 'F'))
        return c - 'A' + 10;//'A' = 65;
    else if((c >= 'a') && (c <= 'f'))
        return c - 'a' + 10;
    else
        return -1;
}

void Dialog::on_Open_Serial_clicked()
{
    serialOpen();
}

void Dialog::on_Open_hot_clicked()
{
    if(m_serial->isOpen())
    {
        QString str = "FE FD 01 BC 01 BB 33 A4 36 35 37 A4 36 A3 37 34 37 35 37 A3 36 35 36 35 "
                      "37 A4 36 35 34 37 36 A7 33 A4 31 3A 37 A3 37 35 36 35 36 A4 36 A4 36 A6 "
                      "34 A3 32 A8 37 A6 34 A4 36 A3 37 35 36 35 36 35 36 36 36 35 36 35 37 A3 "
                      "36 35 36 A5 36 A3 32 A8 36 A4 36 35 37 35 36 35 36 A4 36 35 36 35 37 35 "
                      "36 35 36 A7 2E AC 33 02 0A 01 B8 01 C0 31 A6 31 3A 36 A4 36 A4 36 35 36 "
                      "35 37 A3 36 35 37 35 36 A6 34 35 36 35 37 A3 36 A4 32 3A 31 A9 36 35 36 "
                      "35 31 AC 33 A4 31 AC 33 A4 36 A4 36 A6 2F AB 34 A6 2F 3A 36 35 37 34 37 "
                      "34 37 35 36 35 37 A6 33 35 36 A4 32 A8 31 AB 2F A9 36 35 36 35 36 36 31 "
                      "AB 2F 39 32 3A 36 35 36 35 37 A6 2F AB 33 FF ";

        m_serial->write(QString2Hex(formatInput(str)));
    }
}

void Dialog::on_hot_30_clicked()
{
    if(m_serial->isOpen())
    {
        QString str = "FE FD 01 C1 01 B6 38 9E 3C 31 34 A6 3B A2 35 33 3B 30 3B A2 38 31 3B 30 "
                      "3B 9F 3B 30 3B 31 3B A1 39 9F 3B 30 3B A0 3A 30 3B 31 3B A2 38 A2 38 9F "
                      "3B 9F 3B 9F 3B A2 38 A2 38 9F 3B 30 36 36 3A 31 3B 30 3B 31 3A 31 3B 9F "
                      "3B 30 3B A2 38 9F 3B 9F 3B A2 38 30 3B 31 3B 30 3B 9F 3B 30 3B 30 3B 31 "
                      "3B 30 3B A2 38 9F 3B 02 06 01 C0 01 B7 38 A2 38 30 3B 9F 3B A2 38 31 3B "
                      "30 3B 9F 3B 30 3B 31 3A A2 38 31 3B 30 35 A5 3B A0 3A 30 3B 9F 3B 31 3B "
                      "30 3B A2 38 A2 38 9F 3B A2 38 9F 3B A2 38 9F 3B A2 38 30 3B 31 3A 31 3B "
                      "30 3B 30 3B 31 3B A2 38 30 35 A8 38 A2 38 A2 38 9F 3B 30 3B 30 3B 31 3B "
                      "A1 39 30 3B 30 3B 31 3B 30 3B A1 39 A2 38 FF ";

        m_serial->write(QString2Hex(formatInput(str)));
    }
}

void Dialog::on_Close_clicked()
{
    if(m_serial->isOpen())
    {
        QString str = "FE FD 01 BC 01 BA 34 A3 37 34 37 A2 37 A3 37 34 37 35 36 A6 34 34 37 34 "
                      "37 A4 36 34 37 34 37 A6 34 9D 3D 34 37 A3 36 35 39 A1 36 A6 34 A3 37 A3 "
                      "37 34 37 A3 37 A2 37 A4 36 34 37 35 36 35 36 35 37 A3 36 35 37 34 37 A6 "
                      "33 A6 34 A6 34 34 37 34 37 35 36 35 37 34 37 34 37 34 37 34 37 A3 37 A3 "
                      "37 A3 37 A2 37 A3 37 02 09 01 BD 01 BA 33 A3 37 34 37 A6 34 A6 34 34 37 "
                      "34 37 A3 37 34 37 34 37 A6 34 34 37 34 37 A4 36 A3 37 34 37 A3 36 35 37 "
                      "A3 36 A4 36 A3 37 A3 36 35 37 A3 36 A4 36 A3 37 34 37 34 37 34 37 35 36 "
                      "A6 34 35 36 35 36 A3 37 A3 36 A6 34 35 37 34 36 35 37 34 37 34 37 34 37 "
                      "35 36 35 36 A4 37 A5 34 A6 2F A8 36 A4 36 FF";

        m_serial->write(QString2Hex(formatInput(str)));
    }
}



void Dialog::on_Open_cold_clicked()
{
    if(m_serial->isOpen())
    {
        QString str = "FE FD 01 C1 01 B6 38 A2 38 30 35 A8 38 A2 38 31 3B 30 3B 9F 3B 30 3B "
                      "30 3B A0 3A 31 3B 30 3B 9F 3B A2 38 30 3B 9F 3B 30 3B 31 3B 9F 3B 9F "
                      "3A A0 3A A2 38 9F 3B A2 38 9F 34 A6 3B 30 3B 30 3C 30 3B 30 3B 30 3B "
                      "30 35 A8 38 A0 3A 31 3A 31 3B 30 3B 30 3B 31 3B 30 3B 30 3B 31 3A A2 "
                      "38 A2 38 A2 38 A2 38 9F 3B A2 38 02 05 01 C1 01 B4 3B A2 38 30 3B 9F "
                      "3B A2 38 30 3B 31 3A A2 38 31 3B 30 3B A2 38 30 3B 30 35 A8 38 A2 38 "
                      "31 3A 9F 3B 31 3B 30 3B A2 38 A2 38 9F 3B A2 38 A2 38 9F 3B A2 38 A2 "
                      "38 30 3B 30 3B 31 3B 30 3B 30 3B 31 3B 9F 3B A1 38 31 37 34 3B 30 35 "
                      "36 3B 31 3B 30 3B 30 3B 31 3B 9F 3B 9F 3B A2 38 9F 3B 9F 3B 9F 3B FF ";

        m_serial->write(QString2Hex(formatInput(str)));
    }
}

void Dialog::on_cold_24_clicked()
{
    if(m_serial->isOpen())
    {
        QString str = "FE FD 01 BD 01 BA 34 A3 34 37 37 A6 34 A3 37 34 37 35 37 A3 36 35 37 "
                      "34 37 A3 37 34 37 35 36 A6 35 A3 37 34 37 A6 34 34 37 34 37 A4 36 A3 "
                      "37 A4 36 A6 35 A3 36 A4 36 A4 36 A4 36 35 36 35 37 34 37 34 37 35 37 "
                      "34 37 34 37 A4 36 35 36 35 37 34 37 35 36 35 37 34 37 A6 34 35 37 A3 "
                      "36 A4 36 A4 36 A6 34 A4 36 A4 37 02 09 01 BD 01 BA 35 A6 30 38 37 A3 "
                      "37 A3 36 35 37 34 37 A4 36 35 36 35 37 A3 37 34 37 34 37 A6 34 A3 37 "
                      "35 36 A4 36 35 36 35 37 A6 34 A5 34 A4 37 A5 35 A5 34 A6 34 A6 34 A4 "
                      "36 35 37 34 37 34 37 34 37 35 37 34 37 34 37 A6 34 34 37 34 37 35 36 "
                      "35 37 34 37 34 37 A6 34 35 36 A6 34 A4 36 A6 34 A6 34 A3 37 A6 34 FF ";

        m_serial->write(QString2Hex(formatInput(str)));
    }
}

void Dialog::on_cold_26_clicked()
{
    if(m_serial->isOpen())
    {
        QString str = "FE FD 01 BD 01 B9 34 A4 36 35 37 A3 36 A6 34 35 37 34 37 A6 34 34 37 "
                      "34 37 A3 37 35 37 34 37 A3 34 A6 36 35 36 A6 34 35 37 34 37 A3 36 A4 "
                      "37 A3 37 A3 36 A6 34 A6 34 A6 34 A3 34 37 35 37 36 35 37 34 37 34 37 "
                      "34 37 A6 34 A3 34 37 37 A3 37 35 36 35 34 37 37 34 37 35 36 35 37 A3 "
                      "37 34 37 A6 31 A6 37 A6 33 A4 36 02 0A 01 BC 01 B8 37 A5 35 34 37 A3 "
                      "37 A3 36 35 36 35 36 A4 37 34 37 34 37 A6 34 34 37 34 37 A4 36 A4 36 "
                      "35 36 A6 34 35 37 34 31 A9 37 A3 37 A2 37 A6 34 A6 34 B4 26 A3 36 A4 "
                      "36 35 37 34 37 34 37 35 36 35 37 34 37 A6 33 A4 37 34 37 A3 37 34 37 "
                      "34 37 35 37 34 36 35 37 34 37 A3 37 34 37 A4 36 A4 36 A3 37 A6 34 FF ";

        m_serial->write(QString2Hex(formatInput(str)));
    }
}
