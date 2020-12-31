#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDoubleValidator>
#include <QLocale>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QMessageBox>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_convertButton_clicked();
    void finished(QNetworkReply* reply);

    void finishedGraph(QNetworkReply* reply);

    double timeDifferenceBeginEndRangeDate();
    void on_rangeDateCheckBox_clicked(bool checked);

    //void on_rangeEndDateEdit_userDateChanged(const QDate &date);

    void on_rangeEndDateEdit_editingFinished();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager* manager; // The QNetworkAccessManager class allows the application to send network requests and receive responses
    QNetworkAccessManager* managerGraph;
    QString targetCurrency;         // The target currency is the one in which we will convert
};
#endif // MAINWINDOW_H
