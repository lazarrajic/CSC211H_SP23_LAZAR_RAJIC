#include "gymscheduleoption.h"

GymScheduleOption::GymScheduleOption(const QString& day, const QString& startTime, const QString& endTime, const QString& workoutDetails)
    : ScheduleOption(day, startTime, endTime), m_workoutDetails(workoutDetails) {}
