#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set default window size
    setFixedSize(600,400);
    ui->graphic->setFixedSize(0,375);

    // set the minimum and maximum date in the date entry fields
    QDateTime currentTime = QDateTime::currentDateTime(); // Get current date
    QDate minimumDate(currentTime.date().year() - 1,currentTime.date().month(),currentTime.date().day()); // We set the minimum value of the day, which was exactly one year ago, as this is the maximum you can look at in the free api free.currconv.com
    QDate currentDate(currentTime.date().year(),currentTime.date().month(),currentTime.date().day());
    ui->singleDateEdit->setMinimumDate(minimumDate);        // set the minimum date for single currency playback
    ui->singleDateEdit->setMaximumDate(currentDate);        // Set the max date for a single currency search

    ui->rangeBeginDateEdit->setMinimumDate(minimumDate);    // set the min.date for the start of the range
    ui->rangeBeginDateEdit->setMaximumDate(currentDate);    // max.value for the start of the range

    ui->rangeEndDateEdit->setMinimumDate(minimumDate);      // set the min.date for the start of the range

    // set icon
    QMainWindow::setWindowIcon(QIcon(":/resources/img/dollar.ico"));

    // set background
    QPixmap bg(":/resources/img/bg-dollars.png");
    bg = bg.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bg);
    this->setPalette(palette);

    // Set Today to Default
    ui->todayRadioButton->setChecked(true);

    // add items into combo box
    QStringList currencies;
    currencies.push_back("EUR");    // Euro
    currencies.push_back("USD");    // American dollar      (USA)
    currencies.push_back("CAD");    // Canadian dollar      (Canada)
    currencies.push_back("MYR");    // Ringgit              (Malaysia)
    currencies.push_back("GBP");    // Pound Sterling       (UK)
    currencies.push_back("RUB");    // Ruble                (Russia)
    currencies.push_back("UAH");    // Hryvnia              (Ukraine)
    currencies.push_back("CNY");    // Yuan                 (China)
    currencies.push_back("JPY");    // Yen                  (Japan)


    ui->currencyFrom->insertItems(0, currencies); // set these currencies in the comboBox
    ui->currencyTo->insertItems(0, currencies);

    /*
     * Set the English localization, so that you can enter floating-point integers instead of the comma, which is the default
     */
    QLocale locale(QLocale::English);
    QDoubleValidator  *inputRange = new QDoubleValidator(this);
    inputRange->setLocale(locale);
    ui->input->setValidator(inputRange);

    manager = new QNetworkAccessManager(this);  // Create a class object that is the center of the API
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));   // If the manager value changes, so does this( the finished function will be called)

    managerGraph = new QNetworkAccessManager(this);
    connect(managerGraph,
            &QNetworkAccessManager::finished,
            this,
            &MainWindow::finishedGraph);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete manager;
    delete managerGraph;
}

void MainWindow::on_convertButton_clicked()
{
    if (ui->input->text() != "")                            // If we have something in the input field
      {
        ui->convertButton->setEnabled(false);               // Turning off the button
        QString from = ui->currencyFrom->currentText();     // Turn the current currency( from) into a string and write it into a variable
        QString to = ui->currencyTo->currentText();         // Turn currency( to) into a string and write it into a variable
        targetCurrency = to;                                // Convert the target currency into a currency (to)

        QString url;

        if(ui->todayRadioButton->isChecked())
        {
            // If the button is set to today's date, then just write the api without specifying the date
            url = "https://free.currconv.com/api/v7/convert?q=" + from + "_" + to + "&compact=ultra&apiKey=45690ddeb55b52ed309c"; // Record the url where we are going to send the request
        }
        else if(ui->singleDateRadioButton->isChecked())
        {
            int day   = ui->singleDateEdit->date().day();
            int month = ui->singleDateEdit->date().month();
            int year  = ui->singleDateEdit->date().year();

            url = "https://free.currconv.com/api/v7/convert?q=" + from + "_" + to + "&compact=ultra&apiKey=45690ddeb55b52ed309c&date=" + QString::number(year) + "-" + QString::number(month) + "-" + QString::number(day);
        }
        QNetworkRequest request= QNetworkRequest(QUrl(url));// Create a QNetworkRequest object with the url address as the requested url address
        manager->get(request);                              // Sends a request to retrieve the contents of the target request and returns a new QNetworkRequest object,
                                                            // open for reading, which emits a signal whenever new data arrives
                                                            // so the finished function is called and the value of this is changed
      }
      else
      {
        QMessageBox::warning(this, "Error", "Please insert a value.");  // If no value is entered - error message
      }
}

void MainWindow::finished(QNetworkReply* reply)
{
    QByteArray response = reply->readAll();                           // Read all the data we received in the query and write it in the byte array
    QJsonDocument jsonResponse = QJsonDocument::fromJson(response);   // Parsing response as utf-8 encoded json and creating a QJsonDocument object
    QJsonObject jsonObj = jsonResponse.object();                      // Get the QJsonObject stored in the document
    double rate = 1;

    if(ui->todayRadioButton->isChecked())
    {
        rate = jsonObj.value(ui->currencyFrom->currentText() + "_" + targetCurrency).toDouble();
    }
    else if(ui->singleDateRadioButton->isChecked())
    {
        QString dayStr;
        if(ui->singleDateEdit->date().day() < 10)
        {
            QTextStream(&dayStr) << "0" << ui->singleDateEdit->date().day();
        }
        else
        {
            QTextStream(&dayStr) << ui->singleDateEdit->date().day();
        }

        QString monthStr;
        if(ui->singleDateEdit->date().month() < 10)
        {
            QTextStream(&monthStr) << "0" << ui->singleDateEdit->date().month();
        }
        else
        {
            QTextStream(&monthStr) << ui->singleDateEdit->date().month();
        }
        QString tmpStr = QString::number(ui->singleDateEdit->date().year()) + "-" + monthStr + "-" + dayStr;
        QJsonObject jsonTimeStamp = jsonObj.value(ui->currencyFrom->currentText() + "_" + targetCurrency).toObject();
        rate = jsonTimeStamp.value(tmpStr).toDouble();
    }

    if (rate == 0)
    {
        rate = 1;
    }

    double amount = ui->input->text().toDouble();                     // We get the converted value we entered
    double result = amount * rate;                                    // Calculate the result by multiplying the number we entered by the exchange rate
    ui->output->setText(QString::number(result));                     // Print the result in the text box
    ui->convertButton->setEnabled(true);                              // Return the button position to true, so it can be used again
}

void MainWindow::on_rangeDateCheckBox_clicked(bool checked)
{
    // If we press the Range Date button, an additional part of the menu opens with a graph
    if(checked)
    {
        setMaximumSize(900, 400);
        setMinimumSize(900,400);
        ui->graphic->setMinimumSize(300,375);
        ui->graphic->setMaximumSize(300,375);
    }
    else
    {
        setFixedSize(600,400);
        ui->graphic->setFixedSize(0,375);
    }
}

double MainWindow::timeDifferenceBeginEndRangeDate()
{
    // First we need to find the number of days between the two dates, this will be our interval
    struct std::tm beginDate = {0, 0, 0, ui->rangeBeginDateEdit->date().day(), ui->rangeBeginDateEdit->date().month(), ui->rangeBeginDateEdit->date().year() - 1900};
    struct std::tm endDate = {0, 0, 0, ui->rangeEndDateEdit->date().day(), ui->rangeEndDateEdit->date().month(), ui->rangeEndDateEdit->date().year() - 1900};

    std::time_t x = std::mktime(&beginDate);
    std::time_t y = std::mktime(&endDate);

    /*
     * You don't have to check if the date exists, because it is 100% possible to enter an invalid date in QDateEdit
     */

    return std::difftime(y, x) / (60 * 60 * 24);
}

void MainWindow::finishedGraph(QNetworkReply* reply)
{
    double amountOfDays = timeDifferenceBeginEndRangeDate(); // Calculate the number of points we will draw
    if(amountOfDays <= 0)
    {
        QMessageBox::warning(this, "Error", "Incorrect date range input. Please enter the correct range!");
        return;
    }

    // If you purchased a different plan - remove this check in the code
    if(amountOfDays > 8)
    {
        QMessageBox::warning(this, "Error", "Unfortunately, in the free version of this API you cannot enter a time interval longer than 8 days");
        return;
    }
    QVector<QCPGraphData> currencyData(amountOfDays + 1);
    QDate xAxisMinimum(ui->rangeBeginDateEdit->date().year(),ui->rangeBeginDateEdit->date().month(), ui->rangeBeginDateEdit->date().day());

    // get reply from managerGraph
    QByteArray response = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
    QJsonObject jsonObj = jsonResponse.object();

    QString tmpStr;
    QJsonObject jsonTimeStamp;
    QString dayStr;
    QString monthStr;

    double now = ui->rangeBeginDateEdit->dateTime().toTime_t(); // Find the range start date in seconds

    for(int i=0;i <= amountOfDays;++i)
    {
        currencyData[i].key = now + 24 * 3600 * i;

        dayStr.clear();      // Reset the values of the variables to zero
        monthStr.clear();
        if(xAxisMinimum.day() < 10)
        {
            QTextStream(&dayStr) << "0" << xAxisMinimum.day();
        }
        else
        {
            QTextStream(&dayStr) << xAxisMinimum.day();
        }

        if(xAxisMinimum.month() < 10)
        {
            QTextStream(&monthStr) << "0" << xAxisMinimum.month();
        }
        else
        {
            QTextStream(&monthStr) << xAxisMinimum.month();
        }

        tmpStr = QString::number(xAxisMinimum.year()) + "-" + monthStr + "-" + dayStr;
        jsonTimeStamp = jsonObj.value(ui->currencyFrom->currentText() + "_" + targetCurrency).toObject();

        if(jsonTimeStamp.value(tmpStr).toDouble() != 0)
        {
            currencyData[i].value = jsonTimeStamp.value(tmpStr).toDouble();
        }
        else
        {
            currencyData[i].value = 1;
        }
        xAxisMinimum = xAxisMinimum.addDays(1); // Add one day at a time
    }

    ui->graphic->clearGraphs(); // Clear the previous charts, if there are any
    ui->graphic->addGraph(); // Add one graph to our widget

    QColor color(220, 42, 150, 150); // set color
    ui->graphic->graph(0)->setName("Graph of changes");
    ui->graphic->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->graphic->graph(0)->setPen(QPen(color));
    ui->graphic->graph(0)->setBrush(QBrush(color.lighter(150)));
    ui->graphic->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));

    ui->graphic->graph(0)->data()->set(currencyData);

    // Set the Oh axis to show the date instead of numbers
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("dd.MM.yyyy");
    ui->graphic->xAxis->setTicker(dateTicker);

    // Apply fonts and set text size
    ui->graphic->xAxis->setTickLabelFont(QFont(QFont().family(), 5));
    ui->graphic->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    // Signing our axes
    ui->graphic->xAxis->setLabel("Date");
    ui->graphic->yAxis->setLabel("Exchange rates");

    // Set the second axes, but without values (without the scale of graduations)
    ui->graphic->xAxis2->setVisible(true);
    ui->graphic->yAxis2->setVisible(true);
    ui->graphic->xAxis2->setTicks(false);
    ui->graphic->yAxis2->setTicks(false);
    ui->graphic->xAxis2->setTickLabels(false);
    ui->graphic->yAxis2->setTickLabels(false);

    // Specify that our graph should show all data (from the start date to the end date)
    ui->graphic->xAxis->setRange(now, now+24*3600*amountOfDays);
    ui->graphic->yAxis->setRange(0, currencyData[amountOfDays - 1].value * 1.5);

    // Set the legend with a slightly transparent brush
    ui->graphic->legend->setVisible(true);
    ui->graphic->legend->setBrush(QColor(255, 255, 255, 150));


    ui->graphic->setInteraction(QCP::iRangeZoom, true);     // Enabling zoom in/out interaction
    ui->graphic->setInteraction(QCP::iRangeDrag, true);     // Enabling drag-and-drop interaction

    ui->graphic->axisRect()->setRangeDrag(Qt::Horizontal);  // Activate the drag on the horizontal axis


    ui->graphic->replot();
}

void MainWindow::on_rangeEndDateEdit_editingFinished()
{
    QString from = ui->currencyFrom->currentText();
    QString to = ui->currencyTo->currentText();
    targetCurrency = to;

    QString url;

    int beginDay   = ui->rangeBeginDateEdit->date().day();
    int beginMonth = ui->rangeBeginDateEdit->date().month();
    int beginYear  = ui->rangeBeginDateEdit->date().year();

    int endDay   = ui->rangeEndDateEdit->date().day();
    int endMonth = ui->rangeEndDateEdit->date().month();
    int endYear  = ui->rangeEndDateEdit->date().year();

    url = "https://free.currconv.com/api/v7/convert?q=" + from + "_" + to + "&compact=ultra&apiKey=45690ddeb55b52ed309c&date="
            + QString::number(beginYear) + "-" + QString::number(beginMonth) + "-" + QString::number(beginDay) +
            "&endDate=" + QString::number(endYear) + "-" + QString::number(endMonth) + "-" + QString::number(endDay);

    QNetworkRequest request= QNetworkRequest(QUrl(url));
    managerGraph->get(request);
}
