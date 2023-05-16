#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "ScheduleOption.h"
#include <QListWidgetItem>

#include <QVector>
#include <QMap>
//#include "UserInfo.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
//      void updateScheduleListWidget();

    ~MainWindow();



private slots:




    void on_dayComboBox_currentIndexChanged(int index);

    void on_timeStartComboBox_currentIndexChanged(int index);

    void on_timeEndComboBox_2_currentIndexChanged(int index);

    void on_addOptionButton_clicked();

    void updateScheduleListWidget();

    void on_printSchedule_clicked();

    void on_pushButton_clicked();

    void on_completedButton_clicked();

    void on_loginPushButton_clicked();
    void on_createUserButton_clicked();
    void saveSchedule(const QString& username);
    void loadSchedule(const QString& username);
    void saveGymSchedule(const QString& username);
    void updateScheduleTable();
    void on_gymDayComboBox_currentIndexChanged(int index);
    void on_gymTimeStartComboBox_currentIndexChanged(int index);
    void on_comboBox_currentIndexChanged(int index);
    void on_addGymOptionButton_clicked();
    void on_popupPushButton_clicked();
    void on_strengthPushButton_clicked();
    void on_freePushButton_clicked();
    void on_logoutPushButton_clicked();
    void initializeScheduleTable();
    void on_trackerPushButton_clicked();
    void loadTrackerData(QString username);
    void saveTrackerData(QString username);
    void initializeTrackerTable(int numRows, int numColumns);
    void on_hoursPushButton_clicked();
    void saveGymDetails(const QString& username, const QString& gymScheduleDetails);
    void loadGymDetails(const QString& username);


private:
    Ui::MainWindow *ui;
    QList<ScheduleOption> scheduleOptions;
//    ClassTimes ctimes;
//    QVector <double> schedule;


    QMap<QString, QVector<QPair<double, double>>> schedule;

    QMap<int, QList<QPair<double, double>>> newSchedule;

    QVector<double>newTimes;
    QMap<QString, int> daysMap = {{"Monday", 0}, {"Tuesday", 1}, {"Wednesday", 2}, {"Thursday", 3}, {"Friday", 4}, {"Saturday", 5}, {"Sunday", 6}};
    int dayChoice;

    QMap<int, QList<QPair<double, double>>> gymSchedule;





};


#endif // MAINWINDOW_H
