#include "ScheduleOption.h"
#include <QString>



ScheduleOption::ScheduleOption(const QString& day, const QString& startTime, const QString& endTime)
    : m_day(day), m_startTime(startTime), m_endTime(endTime) {}
