/*
 * lightos_utils.h: lightos utils
 *
 * Copyright (C) 2015 lzj7179@163.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __LIGHTOS_UTILS_H__
# define __LIGHTOS_UTILS_H__

# include "driver.h"
# include "domain_conf.h"
# include "configmake.h"
# include "virthread.h"

# define LIGHTOS_AUTOSTART_DIR    SYSCONFDIR "/libvirt/lightos/autostart"
# define LIGTHOS_CONFIG_DIR       SYSCONFDIR "/libvirt/lightos"
# define LIGHTOS_STATE_DIR        LOCALSTATEDIR "/run/libvirt/lightos"
# define LIGHTOS_LOG_DIR          LOCALSTATEDIR "/log/libvirt/ligthos"


struct _lightosConn{
	virMutex lock;
	virDomainObjListPtr domains;
	virCapsPtr caps;
	virDomainXMLOptionPtr xmlopt;
	char *pidfile;
};

typedef struct _lightosConn lightosConn;
typedef struct _lightosConn *lightosConnPtr;


void lightosDriverLock(lightosConnPtr driver);
void lightosDriverUnlock(lightosConnPtr driver);

#endif 
