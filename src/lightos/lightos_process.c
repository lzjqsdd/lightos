/*
 * lightos_process.c: Lightos process management
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

#include <config.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "lightos_process.h"
#include "lightos_command.h"
#include "datatypes.h"
#include "virerror.h"
#include "virlog.h"
#include "virfile.h"
#include "viralloc.h"
#include "vircommand.h"
#include "virstring.h"
#include "virpidfile.h"
#include "virprocess.h"
#include "virnetdev.h"
#include "virnetdevbridge.h"
#include "virnetdevtap.h"

#define VIR_FROM_THIS	VIR_FROM_LIGHTOS

int
virLightosProcessStart(virConnectPtr conn ATTRIBUTE_UNUSED,
                     lightosConnPtr driver,
                     virDomainObjPtr vm,
                     virDomainRunningReason reason ATTRIBUTE_UNUSED)
{
	virCommandPtr cmd = NULL;
	cmd = virLightosProcessBuildCreateCmd(driver,vm);
	printf("start process");
	//return virCommandRun(cmd,NULL);
	virCommandFree(cmd);
	return 1;
}

int
virLightosProcessStop(lightosConnPtr driver,
                    virDomainObjPtr vm,
                    virDomainShutoffReason reason ATTRIBUTE_UNUSED)
{
	virCommandPtr cmd = NULL;
	cmd = virLightosProcessBuildDestroyCmd(driver,vm);
	printf("stop process");
	//return virCommandRun(cmd,NULL);
	virCommandFree(cmd);
	return 1;
}
