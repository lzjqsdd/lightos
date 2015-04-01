/*
 * lightos_process.c: lightos command generation
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

#ifndef __LIGHTOS_COMMAND_H_
# define __LIGHTOS_COMMAND_H_

# include "vircommand.h"
# include "lightos_util.h"

virCommandPtr virLightosProcessBuildCreateCmd(lightosConnPtr,
					virDomainObjPtr vm);

virCommandPtr virLightosProcessBuildDestroyCmd(lightosConnPtr driver,
					virDomainObjPtr vm);

virCommandPtr virLightosResizeCmd(lightosConnPtr driver,
		virDomainObjPtr vm,int type,int size);

#endif 
