#ifndef CWE_DEFINES_H
#define CWE_DEFINES_H

/* define an enum for all tasks/widgets stacked on the right side of cwe_mainwindow */
/* note: TASK_MAX_TASKS must always be the last entry.                              *
 *       It will be used for dimensioning a storage array                           */
typedef enum {
    TASK_LANDING,
    TASK_CREATE_NEW_SIMULATION,
    TASK_MANAGE_SIMULATION,
    TASK_MANAGE_FILES,
    TASK_MANAGE_JOBS,
    TASK_LIST_TASKS,
    TASK_HELP,
    TASK_SIMULATION_DETAILS,
    TASK_MAX_TASKS
} TASK;

typedef enum {
    SIM_MODE_2D,
    SIM_MODE_3D
} SIM_MODE;

#endif // CWE_DEFINES_H
