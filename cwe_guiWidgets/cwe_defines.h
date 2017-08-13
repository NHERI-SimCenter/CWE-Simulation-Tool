/*********************************************************************************
**
** Copyright (c) 2017 The University of Notre Dame
** Copyright (c) 2017 The Regents of the University of California
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, this
** list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice, this
** list of conditions and the following disclaimer in the documentation and/or other
** materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its contributors may
** be used to endorse or promote products derived from this software without specific
** prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
** SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
** IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
***********************************************************************************/

// Contributors:

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

/* define binary mode flags for all pages of the GUI  */
/* setting mode = CWE_LANDING_TAB|CWE_RESULTS_TAB|... */
/* testing if (mode & CWE_LANDING_TAB) { ... }        */

#define CWE_NONE_TAB         0x00000000
#define CWE_LANDING_TAB      0x00000001
#define CWE_CREATE_NEW_TAB   0x00000002
#define CWE_CREATE_COPY_TAB  0x00000004
#define CWE_CASE_SELECT_TAB  0x00000008
#define CWE_DETAILS_TAB      0x00000010
#define CWE_RESULTS_TAB      0x00000020
#define CWE_HELP_TAB         0x00000040
#define CWE_WIDGET_PAGE_TAB  0x00000080
#define CWE_FILE_MANAGER_TAB 0x00000100

/* define state switches */
#define CWE_STATE_NONE       0x00000000
#define CWE_STATE_NEW        0x00010000
#define CWE_STATE_RUNNING    0x00020000
#define CWE_STATE_RESULTS    0x00040000
#define CWE_STATE_CLEAR      0x00080000

#endif // CWE_DEFINES_H
