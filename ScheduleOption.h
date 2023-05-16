#ifndef SCHEDULEOPTION_H
#define SCHEDULEOPTION_H
#include <QString>





class ScheduleOption {
public:

 virtual ~ScheduleOption() {}
    ScheduleOption(const QString& day, const QString& startTime, const QString& endTime);

    QString day() const { return m_day; }
    QString startTime() const { return m_startTime; }
    QString endTime() const { return m_endTime; }




private:
    QString m_day;
    QString m_startTime;
    QString m_endTime;

};




#endif // SCHEDULEOPTION_H
