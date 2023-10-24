/*H***********************************************************************
* FILENAME :        memfault_platform_config.h
*
* DESCRIPTION :
* Definition file for the Memfault Configuration (e.g. heartbeat interval)
*
* LICENSE AND COPYRIGHT:
* SPDX-FileCopyrightText: (c) 2023 Siemens AG
* SPDX-License-Identifier: BSD-3-Clause
* 
* NOTES :
* This application is part of the OpenSwarm Project.
*
* AUTHOR :
* Fabian Graf - fabian.graf@siemens.com
*
* CHANGES :
* VERSION DATE    WHO     DETAIL
* 0       26Sep23 FG      Initial Commit
*
*H************************************************************************/

#pragma once

#define MEMFAULT_METRICS_HEARTBEAT_INTERVAL_SECS 20 // Default: 3600s