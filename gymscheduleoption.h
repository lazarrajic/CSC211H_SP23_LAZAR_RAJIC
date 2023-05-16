#ifndef GYMSCHEDULEOPTION_H
#define GYMSCHEDULEOPTION_H

#include "ScheduleOption.h"
#include <QString>

class GymScheduleOption : public ScheduleOption {
public:
    GymScheduleOption(const QString& day, const QString& startTime, const QString& endTime, const QString& workoutDetails);

    QString workoutDetails() const { return m_workoutDetails; }

private:
    QString m_workoutDetails;
};

#endif // GYMSCHEDULEOPTION_H
