#include "mainwindow.h"
#include "gymscheduleoption.h"
#include "./ui_mainwindow.h"
#include <QInputDialog>
#include <QString>
#include <QListWidgetItem>
#include <QListWidget>
#include <QtCore/qpair.h>
#include <QTime>
#include <QFile>
#include <QMessageBox>
#include "userLogin.cpp"
#include <QTextStream>
#include "UserInfo.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QApplication>
#include <QPixmap>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    connect(ui->scheduleListWidget, SIGNAL(itemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(on_scheduleListWidget_currentItemChanged(QListWidgetItem*,QListWidgetItem*)));


    initializeScheduleTable();


    // Open the file in read mode
    QFile file("userInfo.txt");
    if (file.open(QIODevice::ReadOnly)) {
        // Create a QTextStream object to read from the file
        QTextStream in(&file);

        // Read the data from the file and populate the users list
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(",");
            if (fields.size() == 3) {
                UserInfo user = {fields[0], "", fields[1], fields[2]};
                users.append(user);
            }
        }

        // Close the file
        file.close();
    } else {
        qDebug() << "Failed to open file.";
    }


}



MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::initializeScheduleTable() {
    QVector<double> newTimes;
    for (double i = 8.0; i < 22.0; i += 0.5) {
        newTimes.push_back(i);
    }
    qDebug() << "times vector" << newTimes;

    // Define the time range
    const int startHour = 8; // 8am
    const int endHour = 22;  // 10pm

    // Define the weekdays
    QStringList weekdays = {"Time", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

    // Set the number of rows and columns in the table widget
    ui->scheduleTableWidget->setRowCount((endHour - startHour + 1) * 2); // each hour has two half-hour slots
    ui->scheduleTableWidget->setColumnCount(weekdays.size());

    // Set the table headers
    ui->scheduleTableWidget->setHorizontalHeaderLabels(weekdays);
    ui->scheduleTableWidget->verticalHeader()->setVisible(false);

    // Populate the table with time slots
    for (int hour = startHour; hour <= endHour; hour++) {
        for (int halfHour = 0; halfHour < 2; halfHour++) {
            QString time = QString("%1:%2 %3")
                               .arg(hour < 10 ? "0" + QString::number(hour) : QString::number(hour))
                               .arg(halfHour == 0 ? "00" : "30")
                               .arg(hour < 12 ? "AM" : "PM");
            QTableWidgetItem *timeItem = new QTableWidgetItem(time);
            timeItem->setFlags(Qt::NoItemFlags);
            ui->scheduleTableWidget->setItem((hour - startHour) * 2 + halfHour, 0, timeItem);
        }
    }

    // Set default values for the slots
    for (int i = 0; i < ui->scheduleTableWidget->rowCount(); i++) {
        for (int j = 1; j < ui->scheduleTableWidget->columnCount(); j++) {
            QTableWidgetItem *item = new QTableWidgetItem("Available");
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->scheduleTableWidget->setItem(i, j, item);
        }
    }
}




// Adding class times:
void MainWindow::on_dayComboBox_currentIndexChanged(int index)
{
    int dayChoice;
    QString selectedDay = ui->dayComboBox->currentText();
    qDebug() << "Selected day: " << selectedDay;


}

// start time option
void MainWindow::on_timeStartComboBox_currentIndexChanged(int index)
{

    QString selectedStartTime = ui->timeStartComboBox->currentText();
    double startTime = selectedStartTime.toDouble();
    qDebug() << "Start Time: " << startTime;
    newTimes.push_back(startTime);


}
// end time option
void MainWindow::on_timeEndComboBox_2_currentIndexChanged(int index)
{
    QString selectedEndTime = ui->timeEndComboBox_2->currentText();
    qDebug() << "End Time: " << selectedEndTime;
}



void MainWindow::on_addOptionButton_clicked()
{
    QString selectedDay = ui->dayComboBox->currentText();
    QString selectedStartTime = ui->timeStartComboBox->currentText();
    QString selectedEndTime = ui->timeEndComboBox_2->currentText();

    ScheduleOption newOption(selectedDay, selectedStartTime, selectedEndTime);
    scheduleOptions.append(newOption);
    updateScheduleListWidget();

    double startTime = selectedStartTime.toDouble();
    double endTime = selectedEndTime.toDouble();
    int startRow = (startTime - 8.0) * 2; // Calculate the start row of the time slot
    int endRow = (endTime - 8.0) * 2 + 1; // Calculate the end row of the time slot
    int dayColumn = ui->dayComboBox->currentIndex(); // Get the day column index

    for (int row = startRow; row <= endRow; row++) {
        QTableWidgetItem* item = new QTableWidgetItem("Busy");
        if (!item) {
            qDebug() << "Error: Null pointer returned by ui->scheduleTableWidget->item(row, dayColumn)";
            return;
        }
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->scheduleTableWidget->setItem(row, dayColumn, item);
        item->setBackground(Qt::red);

    }


    qDebug() << "Schedule for " << selectedDay << ":";
    for (int row = startRow; row <= endRow; row++) {
        QTableWidgetItem* timeItem = ui->scheduleTableWidget->item(row, 0);
        qDebug() << timeItem->text() << ": " << ui->scheduleTableWidget->item(row, dayColumn)->text();
    }

        bool addedToExistingSlot = false;
        for (auto& pair : newSchedule[dayChoice]) {
            if ((startTime >= pair.first && startTime <= pair.second) ||
                (endTime >= pair.first && endTime <= pair.second)) {
                // New time overlaps with existing time slot, update start or end time
                pair.first = std::min(pair.first, startTime);
                pair.second = std::max(pair.second, endTime);
                addedToExistingSlot = true;
                break;
            }
        }
        if (!addedToExistingSlot) {
            // New time does not overlap with any existing time slot, add as new time slot
            newSchedule[dayChoice].push_back(qMakePair(startTime, endTime));
        }

        qDebug() << "new schedule vector" << newSchedule;
        qDebug() << "Schedule for " << selectedDay << ":";
        for (auto& pair : newSchedule[dayChoice]) {
            // converts time entry from double to string
            QString startTime = QTime::fromMSecsSinceStartOfDay(pair.first * 3600 * 1000).toString("hh:mm");
            if (startTime.startsWith('0')) {
                startTime.remove(0, 1);
            }
            QString endTime = QTime::fromMSecsSinceStartOfDay(pair.second * 3600 * 1000).toString("hh:mm");
            if (endTime.startsWith('0')) {
                endTime.remove(0, 1);
            }
            qDebug() << QString("%1-%2").arg(startTime, endTime);
        }


QString username = ui->usernameLineEdit->text();
         saveSchedule(username);
        qDebug() << schedule;


}



// adding gym times:

void MainWindow::on_gymDayComboBox_currentIndexChanged(int index)
{
        int gymDayChoice;
        QString selectedDay = ui->gymDayComboBox->currentText();
        qDebug() << "Selected day: " << selectedDay;


}

void MainWindow::on_gymTimeStartComboBox_currentIndexChanged(int index)
{
        QString selectedStartTime = ui->gymTimeStartComboBox->currentText();
        double startTime = selectedStartTime.toDouble();
        qDebug() << "Start Time: " << startTime;
        newTimes.push_back(startTime);
}


void MainWindow::on_comboBox_currentIndexChanged(int index)
{
        QString selectedEndTime = ui->gymEndTimeComboBox->currentText();
        qDebug() << "End Time: " << selectedEndTime;
}


void MainWindow::on_addGymOptionButton_clicked()
{
        QString selectedDay = ui->gymDayComboBox->currentText();
        QString selectedStartTime = ui->gymTimeStartComboBox->currentText();
        QString selectedEndTime = ui->gymEndTimeComboBox->currentText();
        QString workoutDetails = ui->workoutDetailsLineEdit->text();

        GymScheduleOption newOption(selectedDay, selectedStartTime, selectedEndTime, workoutDetails);
        scheduleOptions.append(newOption);
        updateScheduleListWidget();

        double startTime = selectedStartTime.toDouble();
        double endTime = selectedEndTime.toDouble();
        int startRow = (startTime - 8.0) * 2; // Calculate the start row of the time slot
        int endRow = (endTime - 8.0) * 2 + 1; // Calculate the end row of the time slot
        int dayColumn = ui->gymDayComboBox->currentIndex(); // Get the day column index

        for (int row = startRow; row <= endRow; row++) {
            QTableWidgetItem* item = new QTableWidgetItem("Gym");
            if (!item) {
                qDebug() << "Error: Null pointer returned by ui->scheduleTableWidget->item(row, dayColumn)";
                return;
            }
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->scheduleTableWidget->setItem(row, dayColumn, item);
            item->setBackground(Qt::blue);
        }

        qDebug() << "Schedule for " << selectedDay << ":";
        for (int row = startRow; row <= endRow; row++) {
            QTableWidgetItem* timeItem = ui->scheduleTableWidget->item(row, 0);
            qDebug() << timeItem->text() << ": " << ui->scheduleTableWidget->item(row, dayColumn)->text();
        }

        bool addedToExistingSlot = false;
        for (auto& pair : gymSchedule[dayChoice]) {
            if ((startTime >= pair.first && startTime <= pair.second) ||
                (endTime >= pair.first && endTime <= pair.second)) {
                // New time overlaps with existing time slot, update start or end time
                pair.first = std::min(pair.first, startTime);
                pair.second = std::max(pair.second, endTime);
                addedToExistingSlot = true;
                break;
            }
        }
        if (!addedToExistingSlot) {
            // New time does not overlap with any existing time slot, add as new time slot
            gymSchedule[dayChoice].push_back(qMakePair(startTime, endTime));
        }

        qDebug() << "new schedule vector" << gymSchedule;
        qDebug() << "Schedule for " << selectedDay << ":";
        for (auto& pair : gymSchedule[dayChoice]) {
            // converts time entry from double to string
            QString startTime = QTime::fromMSecsSinceStartOfDay(pair.first * 3600 * 1000).toString("hh:mm");
            if (startTime.startsWith('0')) {
                startTime.remove(0, 1);
            }
            QString endTime = QTime::fromMSecsSinceStartOfDay(pair.second * 3600 * 1000).toString("hh:mm");
            if (endTime.startsWith('0')) {
                endTime.remove(0, 1);
            }
            qDebug() << QString("%1-%2").arg(startTime, endTime);
        }

        // Display workout details
        qDebug() << "Workout Details: " << workoutDetails;
        QTextEdit* gymScheduleDetailsTextEdit = ui->gymScheduleDetailsTextEdit;
        gymScheduleDetailsTextEdit->append("Day: " + selectedDay);
        gymScheduleDetailsTextEdit->append("Start Time: " + selectedStartTime);
        gymScheduleDetailsTextEdit->append("End Time: " + selectedEndTime);
        gymScheduleDetailsTextEdit->append("Workout Details: " + workoutDetails);
        gymScheduleDetailsTextEdit->append("-------------------");


        QString username = ui->usernameLineEdit->text();
        QString gymScheduleDetails = ui->gymScheduleDetailsTextEdit->toPlainText();
        saveGymSchedule(username);
        saveGymDetails(username, gymScheduleDetails);

        qDebug() << schedule;
}









void MainWindow::updateScheduleListWidget()
{
//    ui->scheduleListWidget->clear();
    for (const ScheduleOption& option : scheduleOptions) {
        QString startTime = QTime::fromMSecsSinceStartOfDay(option.startTime().toDouble() * 3600 * 1000).toString("hh:mm");
        QString endTime = QTime::fromMSecsSinceStartOfDay(option.endTime().toDouble() * 3600 * 1000).toString("hh:mm");
        QString optionText = option.day() + " " + startTime + " - " + endTime;
        QListWidgetItem* item = new QListWidgetItem(optionText);
//        ui->scheduleListWidget->addItem(item);
    }

}




void MainWindow::on_printSchedule_clicked()
{
//    ctimes.printSchedule();
}

void MainWindow::on_pushButton_clicked()
{
//    ctimes.stringReturn();
//     qDebug() << "int " <<ctimes.intReturn(); ;
}





void MainWindow::on_completedButton_clicked()
{
     const double gymHoursOpen = 8.0;
     const double gymHoursClosed = 18.0;



        for (const int day : newSchedule.keys()) {
//            QListWidget* newListWidget = ui->newListWidget;
            QVector<QPair<double, double>> nonOverlapTimes;
            double lastEndTime = gymHoursOpen;

            // Add the current day to the list
            QString day_str = "Day " + QString::number(day) + ":";
//            ui->newListWidget->addItem(day_str);

            for (auto const &time : newSchedule.value(day)) {
                if (time.first >= gymHoursClosed) {
                    // Class starts after gym closes, skip
                    continue;
                }
                if (time.second <= gymHoursOpen) {
                    // Class ends before gym opens, skip
                    continue;
                }
                if (time.first >= lastEndTime) {
                    nonOverlapTimes.push_back(qMakePair(lastEndTime, time.first));
                }
                lastEndTime = qMax(lastEndTime, time.second);

                if (lastEndTime < gymHoursClosed) {
                    nonOverlapTimes.push_back(qMakePair(lastEndTime + 0.5, gymHoursClosed));
                }
            }

            for (auto const &time : nonOverlapTimes) {
                QString start_time = "Available start time: " + QTime::fromMSecsSinceStartOfDay(qint64(time.first * 3600 * 1000)).toString("hh:mm ap");
                QString end_time = "Available end time: " + QTime::fromMSecsSinceStartOfDay(qint64(time.second * 3600 * 1000)).toString("hh:mm ap");

//                ui->newListWidget->addItem(start_time);
//                ui->newListWidget->addItem(end_time);
                newSchedule[dayChoice].clear();
            }
        }




     }


     void MainWindow::on_loginPushButton_clicked()
     {
        // Get the username and password entered by the user
        QString username = ui->usernameLineEdit->text();
        QString password = ui->passwordLineEdit->text();

        try {
            // Check if the user is in the list of users
            bool foundUser = false;
            QFile file("userInfo.txt");
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    QStringList fields = line.split(",");
                    if (fields.size() == 4 && fields[0] == username && fields[1] == password) {
                        foundUser = true;
                        qDebug() << "Login Success!";
                        QFile file("userInfo.txt");
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QTextStream in(&file);
                            while (!in.atEnd()) {
                                QString line = in.readLine();
                                qDebug() << line;
                            }
                        }
                        loadSchedule(username);
                        loadGymDetails(username);

                        loadTrackerData(username);

                        updateScheduleTable();
                        ui->loginListWidget->addItem("Success!");
                        break;
                    }
                }

                // Close the file
                file.close();
            } else {
                throw std::runtime_error("Failed to open file.");
            }

            // Display an error message if the user is not found
            if (!foundUser) {
                QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
            }
        } catch (const std::exception& e) {
            qDebug() << "Exception: " << e.what();
        } catch (...) {
            qDebug() << "An unknown exception occurred.";
        }
     }



     void MainWindow::on_createUserButton_clicked()
     {
        // Get the username, password, first name, and last name entered by the user
        bool ok;
        QString username = QInputDialog::getText(this, "Create new user", "Enter username:", QLineEdit::Normal, "", &ok);
        if (!ok) return;
        QString password = QInputDialog::getText(this, "Create new user", "Enter password:", QLineEdit::Password, "", &ok);
        if (!ok) return;
        QString firstName = QInputDialog::getText(this, "Create new user", "Enter first name:", QLineEdit::Normal, "", &ok);
        if (!ok) return;
        QString lastName = QInputDialog::getText(this, "Create new user", "Enter last name:", QLineEdit::Normal, "", &ok);
        if (!ok) return;

        // Check if the username already exists
        bool foundUser = false;
        QFile file("userInfo.txt");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList fields = line.split(",");
                if (fields.size() == 4 && fields[0] == username) {
                    foundUser = true;
                    break;
                }
            }

            // Close the file
            file.close();
        } else {
            qDebug() << "Failed to open file.";
        }

        // If the username does not exist, create the new user and add it to the file
        if (!foundUser) {
            // Open the file in append mode
            QFile outFile("userInfo.txt");
            if (outFile.open(QIODevice::Append | QIODevice::Text)) {
                // Create a QTextStream object to write to the file
                QTextStream out(&outFile);

                // Write the new user information to the file
                out << username << "," << password << "," << firstName << "," << lastName << "\n";

                // Close the file
                outFile.close();

                // Show a success message
                QMessageBox::information(this, "Create new user", "User created successfully.");
            } else {
                qDebug() << "Failed to open file.";
            }
        } else {
            // Show an error message if the username already exists
            QMessageBox::warning(this, "Create new user", "Username already exists. Please choose a different username.");
        }
     }

//////////


     // Save the class schedule to a file for the specified user
     void MainWindow::saveSchedule(const QString& username)
     {
        QFile file("schedule_" + username + ".txt");
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            for (const ScheduleOption& option : scheduleOptions) {
                out << option.day() << "," << option.startTime() << "," << option.endTime() << "\n";
            }

            file.close();
        } else {
            qDebug() << "Failed to open file for writing.";
        }
     }

     // Load the schedule from a file for the specified user
     void MainWindow::loadSchedule(const QString& username)
     {
        QFile file("schedule_" + username + ".txt");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList fields = line.split(",");
                if (fields.size() == 3) {
                    ScheduleOption option(fields[0], fields[1], fields[2]);
                    scheduleOptions.append(option);
                    qDebug() << "Users saved schedule" << username << ":" << option.day() << option.startTime() << option.endTime();
                }


            }

            file.close();

        } else {
            qDebug() << "Failed to open file for reading.";
        }


        // Load gym schedule
        QFile gymScheduleFile("gymSchedule_" + username + ".txt");
        if (gymScheduleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&gymScheduleFile);
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList fields = line.split(",");
                if (fields.size() == 3) {
                    QString day = fields[0];
                    double startTime = fields[1].toDouble();
                    double endTime = fields[2].toDouble();
                    int dayIndex = ui->gymDayComboBox->findText(day);
                    if (dayIndex != -1) {
                        int startRow = (startTime - 8.0) * 2; // Calculate the start row of the time slot
                        int endRow = (endTime - 8.0) * 2 + 1; // Calculate the end row of the time slot
                        for (int row = startRow; row <= endRow; row++) {
                            QTableWidgetItem* item = new QTableWidgetItem("Gym");
                            if (!item) {
                                qDebug() << "Error: Null pointer returned by ui->scheduleTableWidget->item(row, dayIndex)";
                                return;
                            }
                            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                            ui->scheduleTableWidget->setItem(row, dayIndex, item);
                            item->setBackground(Qt::blue);
                        }
                        gymSchedule[dayIndex].push_back(qMakePair(startTime, endTime));
                    }
                }
            }
            gymScheduleFile.close();
        } else {
            qDebug() << "Failed to open gym schedule file for reading.";
        }
     }







     // Save the gym schedule to a file for the specified user
     void MainWindow::saveGymSchedule(const QString& username)
     {
        QFile file("gymSchedule_" + username + ".txt");
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            for (const ScheduleOption& option : scheduleOptions) {
                out << option.day() << "," << option.startTime() << "," << option.endTime() << "\n";
            }

            file.close();
        } else {
            qDebug() << "Failed to open file for writing.";
        }
     }


     void MainWindow::updateScheduleTable()
     {
        if (!ui->scheduleTableWidget) {
            qDebug() << "Schedule table widget is null";
            return;
        }

        // Set default values for the slots that don't already have a value
        for (int i = 0; i < ui->scheduleTableWidget->rowCount(); i++) {
            for (int j = 1; j < ui->scheduleTableWidget->columnCount(); j++) {
                QTableWidgetItem *item = ui->scheduleTableWidget->item(i, j);
                if (!item) {
                    item = new QTableWidgetItem("Available");
                    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                    ui->scheduleTableWidget->setItem(i, j, item);
                }
            }
        }

        // Update the table with busy time slots and populate the schedule map
        schedule.clear();
        for (const ScheduleOption& option : scheduleOptions) {
            double startTime = option.startTime().toDouble();
            double endTime = option.endTime().toDouble();
            int startRow = (startTime - 8.0) * 2; // Calculate the start row of the time slot
            int endRow = (endTime - 8.0) * 2 + 1; // Calculate the end row of the time slot
            int dayColumn = ui->dayComboBox->findText(option.day()); // Get the day column index

            // Set the cell values to "Busy" for the selected time slot
            for (int row = startRow; row <= endRow; row++) {
                QTableWidgetItem* item = ui->scheduleTableWidget->item(row, dayColumn);
                if (!item) {
                    item = new QTableWidgetItem("Busy");
                    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                    ui->scheduleTableWidget->setItem(row, dayColumn, item);
                }
                item->setBackground(Qt::red);

                // Update the schedule map
                QString day = ui->dayComboBox->itemText(dayColumn);
                if (!schedule.contains(day)) {
                    schedule[day] = QVector<QPair<double, double>>();
                }
                schedule[day].append(QPair<double, double>(startTime, endTime));
            }


        }
     }


     void MainWindow::on_hoursPushButton_clicked()
     {
        QDialog* popup = new QDialog(this);
        popup->setWindowTitle("Popup Window");

        QLabel* titleLabel = new QLabel("Fitness Center Hours", popup);
        QFont font = titleLabel->font();
        font.setPointSize(35);
        font.setWeight(QFont::Bold);
        titleLabel->setFont(font);

        QLabel* cardioLabel = new QLabel("\nMonday: 9am - 6pm\n\nTuesday: 9am - 6pm\n\nWednesday: 9am - 6pm\n\nThursday: 9am - 6pm\n\nFriday: 9am - 6pm\n\nSaturday: 9am - 12pm\n\n", popup);
        QPushButton* button = new QPushButton("Close", popup);

        QVBoxLayout* layout = new QVBoxLayout(popup);
        layout->addWidget(titleLabel);
        layout->addWidget(cardioLabel);
        layout->addWidget(button);

        connect(button, &QPushButton::clicked, popup, &QDialog::close);

        popup->exec();
     }




void MainWindow::on_popupPushButton_clicked()
{
        QDialog* popup = new QDialog(this);
        popup->setWindowTitle("Popup Window");

        QLabel* titleLabel = new QLabel("Cardiovascular Equipment", popup);
        QFont font = titleLabel->font();
        font.setWeight(QFont::Bold);
        titleLabel->setFont(font);

        QLabel* cardioLabel = new QLabel("The fitness center provides many options of cardiovascular exercise to get your heart rate up and to improve oxygen intake.\nMany cardiovascular machines help you lose or maintain weight, and help you build endurance so you can remain active for\na longer period of time. The BMCC Fitness Center offers a variety of options, including:\n\n9 Precor treadmills\n3 Precor Open Stride Adaptive Motion Trainer\n4 Precor Elliptical Trainers\n2 Precor Upright Bikes\n2 Gauntlet Step Machines\n1 Concept 2 Rower", popup);
        QPushButton* button = new QPushButton("Close", popup);

        QVBoxLayout* layout = new QVBoxLayout(popup);
        layout->addWidget(titleLabel);
        layout->addWidget(cardioLabel);
        layout->addWidget(button);

        connect(button, &QPushButton::clicked, popup, &QDialog::close);

        popup->exec();
}


void MainWindow::on_strengthPushButton_clicked()
{
        QDialog* popup = new QDialog(this);
        popup->setWindowTitle("Popup Window");




        QLabel* titleLabel = new QLabel("Strength/Resistance Equipment", popup);
        QFont font = titleLabel->font();
        font.setWeight(QFont::Bold);
        titleLabel->setFont(font);
        QLabel* cardioLabel = new QLabel("The BMCC Fitness Center offers students total conditioning that features resistance training equipment to help individuals\n strengthen and tone, build mass and expand endurance. The fitness center offers many options, including:\n\nLat pull down\nLow Row\nChest Delt/Pec Fly\nShoulder Press\nLateral Raise\nLeg Press\nDual Ab back\nDual Bicep/Tricep\nDual Leg Extension/Curl\nDual Ab /Adductor\nDip/Chin Assist\nFTS Glide\nVerticle Knee Up/Dip", popup);
        QPushButton* button = new QPushButton("Close", popup);

        QVBoxLayout* layout = new QVBoxLayout(popup);

        layout->addWidget(titleLabel);
        layout->addWidget(cardioLabel);
        layout->addWidget(button);


        connect(button, &QPushButton::clicked, popup, &QDialog::close);
        popup->update();

        popup->exec();
}



void MainWindow::on_freePushButton_clicked()
{
        QDialog* popup = new QDialog(this);
        popup->setWindowTitle("Popup Window");

        QLabel* titleLabel = new QLabel("Free Weight Equipment", popup);
        QFont font = titleLabel->font();
        font.setWeight(QFont::Bold);
        titleLabel->setFont(font);

        QLabel* cardioLabel = new QLabel("The BMCC Fitness Center also includes a wide selection of free weights that are useful for building strength, speed, flexibility,\nbalance and muscle mass. Weight lifters of all skill levels can benefit from the use of free weights,\nprovided that they train with proper form at all times and understand the function and limitations of this type of equipment.\nThe BMCC Fitness Center offers many free weight options, including:\n\nHalf Rack\nPlate loaded Smith\nAngled Leg Press\nArm Curl\nAssorted Dumbbells and Olympic\nWeight Plates\nOlympic Flat and Incline Bench", popup);
        QPushButton* button = new QPushButton("Close", popup);

        QVBoxLayout* layout = new QVBoxLayout(popup);
        layout->addWidget(titleLabel);
        layout->addWidget(cardioLabel);
        layout->addWidget(button);

        connect(button, &QPushButton::clicked, popup, &QDialog::close);

        popup->exec();
}







void MainWindow::on_trackerPushButton_clicked()
{
        QString exerciseName = ui->exerciseNameLineEdit->text();
        int weight = ui->weightDoubleSpinBox->value();

        int row = ui->trackerTableWidget->rowCount();
        int column = ui->trackerTableWidget->columnCount();


        if (row == 0 || ui->trackerTableWidget->item(row - 1, 0) != nullptr) {
            ui->trackerTableWidget->insertRow(row);
            row++;
        }

        if (column < 2) {
            ui->trackerTableWidget->insertColumn(column);
            QTableWidgetItem *headerItem = new QTableWidgetItem("Exercise Name");
            ui->trackerTableWidget->setHorizontalHeaderItem(column, headerItem);
            column++;
            ui->trackerTableWidget->insertColumn(column);
            headerItem = new QTableWidgetItem("Weight");
            ui->trackerTableWidget->setHorizontalHeaderItem(column, headerItem);
        }

        bool found = false;
        int exerciseRow = -1;
        for (int i = 0; i < row; i++) {
            QTableWidgetItem *item = ui->trackerTableWidget->item(i, 0);
            if (item != nullptr && item->text() == exerciseName) {
                exerciseRow = i;
                found = true;
                break;
            }
        }

        if (!found) {
            ui->trackerTableWidget->insertRow(row);
            exerciseRow = row - 1;
            QTableWidgetItem *exerciseItem = new QTableWidgetItem(exerciseName);
            ui->trackerTableWidget->setItem(exerciseRow, 0, exerciseItem);
        } else {
            int emptyColumn = -1;
            for (int i = 1; i < column; i++) {
                if (ui->trackerTableWidget->item(exerciseRow, i) == nullptr) {
                    emptyColumn = i;
                    break;
                }
            }

            if (emptyColumn != -1) {
                ui->trackerTableWidget->setItem(exerciseRow, emptyColumn, new QTableWidgetItem(QString::number(weight)));
            } else {
                ui->trackerTableWidget->insertColumn(column);
                QTableWidgetItem *headerItem = new QTableWidgetItem("Weight");
                ui->trackerTableWidget->setHorizontalHeaderItem(column, headerItem);
                ui->trackerTableWidget->setItem(exerciseRow, column, new QTableWidgetItem(QString::number(weight)));
            }
        }



        int emptyColumn = -1;
        for (int i = 1; i < column; i++) {
            if (ui->trackerTableWidget->item(exerciseRow, i) == nullptr) {
                emptyColumn = i;
                break;
            }
        }


        if(!found){
        if (emptyColumn != -1) {
            ui->trackerTableWidget->setItem(exerciseRow, emptyColumn, new QTableWidgetItem(QString::number(weight)));
        }
        else {
            ui->trackerTableWidget->insertColumn(column);
            QTableWidgetItem *headerItem = new QTableWidgetItem("Weight");
            ui->trackerTableWidget->setHorizontalHeaderItem(column, headerItem);
            ui->trackerTableWidget->setItem(exerciseRow, column, new QTableWidgetItem(QString::number(weight)));
        }
        }
 QString username = ui->usernameLineEdit->text();
saveTrackerData(username);
}



void MainWindow::saveTrackerData(QString username) {
        QString filename = "gymTracker_" + username + ".txt";
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        int rows = ui->trackerTableWidget->rowCount();
        int columns = ui->trackerTableWidget->columnCount();
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < columns; ++j) {
                    QTableWidgetItem *item = ui->trackerTableWidget->item(i, j);
                    if (item != nullptr) {
                        out << item->text() << ",";
                    } else {
                        out << ",";
                    }
            }
            out << "\n";
        }
        file.close();
        }
}


void MainWindow::loadTrackerData(QString username)
{
        QString filename = "gymTracker_" + username + ".txt";
        QFile file(filename);
        qDebug() << filename;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
        qDebug() << file.errorString();
        QTextStream in(&file);

        // Determine the number of rows and columns from the file
        int numRows = 0;
        int numColumns = 0;
        QString line = in.readLine();
        if (!line.isNull())
        {
            QStringList cells = line.split(",");
            numColumns = cells.size();
            ++numRows;
        }

        while (!line.isNull())
        {
            line = in.readLine();
            if (!line.isNull())
                    ++numRows;
        }

        // Initialize the table widget with the correct dimensions
        initializeTrackerTable(numRows, numColumns);

        file.seek(0);
        line = in.readLine(); // Skip the header line

        int row = 0;
        while (!line.isNull())
        {
            QStringList cells = line.split(",");
            for (int i = 0; i < cells.size(); ++i)
            {
                    QTableWidgetItem *item = new QTableWidgetItem(cells[i]);
                    ui->trackerTableWidget->setItem(row, i, item);
            }
            ++row;
            line = in.readLine();
            qDebug() << line;
        }
        file.close();
        }
}


void MainWindow::on_logoutPushButton_clicked()
{

        initializeScheduleTable();
//        ui->scheduleListWidget->clear();

        ui->usernameLineEdit->clear();
        ui->passwordLineEdit->clear();
        ui->trackerTableWidget->clear();
        ui->exerciseNameLineEdit->clear();
        ui->weightDoubleSpinBox->clear();
        ui->trackerTableWidget->setRowCount(0);
        ui->trackerTableWidget->setColumnCount(0);
        ui->gymScheduleDetailsTextEdit->clear();

}


void MainWindow::initializeTrackerTable(int numRows, int numColumns)
{
        // Initialize the table widget with the desired number of rows and columns
        ui->trackerTableWidget->setRowCount(numRows);
        ui->trackerTableWidget->setColumnCount(numColumns);
}




void MainWindow::saveGymDetails(const QString& username, const QString& gymScheduleDetails)
{
        QString fileName = "gymDetails_" + username + ".txt";
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
        QTextStream stream(&file);
        stream << gymScheduleDetails;
        file.close();
        }
}

void MainWindow::loadGymDetails(const QString& username)
{
        QString fileName = "gymDetails_" + username + ".txt";
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
        QTextStream stream(&file);
        QString gymScheduleDetails = stream.readAll();
        file.close();

        ui->gymScheduleDetailsTextEdit->setText(gymScheduleDetails);
        }
}



