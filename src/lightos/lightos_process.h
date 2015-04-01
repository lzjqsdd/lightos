/*
 * lightos_process.h: lightos process management
 *
 * Copyright (C) 2014 Roman Bogorodskiy
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

#ifndef __LIGHTOS_PROCESS_H__
# define __LIGHTOS_PROCESS_H__

# include "lightos_util.h"

int virLightosProcessStart(virConnectPtr conn,
                         lightosConnPtr driver,
                         virDomainObjPtr vm,
                         virDomainRunningReason reason);

int virLightosProcessStop(lightosConnPtr driver,
                        virDomainObjPtr vm,
                        virDomainShutoffReason reason);

#endif /* __BHYVE_PROCESS_H__ */
