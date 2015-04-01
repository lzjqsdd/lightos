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

#include <config.h>

#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/ioctl.h>

#include "lightos_command.h"
#include "viralloc.h"
#include "virfile.h"
#include "virstring.h"
#include "virlog.h"
#include "virnetdev.h"
#include "virnetdevbridge.h"
#include "virnetdevtap.h"

#define VIR_FROM_THIS VIR_FROM_LIGHTOS


virCommandPtr
virLightosProcessBuildCreateCmd(lightosConnPtr driver ATTRIBUTE_UNUSED,
                             virDomainObjPtr vm ATTRIBUTE_UNUSED)
{

/*
 * %path%/osloader3 lightos.xml
 *
 * LIGHTOS was define in ../../m4/virt_driver_lightos.m4
 *
 */
	

/*
 * init cmd
 */

virCommandPtr cmd = virCommandNew(LIGHTOS);

/*
 * xml path
 */

const char * xmlpath = "/home/lightos1.xml";

virCommandAddArg(cmd,xmlpath);
return cmd;

goto error;

error:
    virCommandFree(cmd);
    return NULL;
}

virCommandPtr
virLightosProcessBuildDestroyCmd(lightosConnPtr driver ATTRIBUTE_UNUSED,
                               virDomainObjPtr vm ATTRIBUTE_UNUSED)
{
/*
 * %path% shutdown id
 * id is 0
 */
    virCommandPtr cmd = virCommandNew(LIGHTOS);

    return cmd;
}

/*virCommandPtr virLightosResizeCmd(lightosConnPtr driver,
				virDomainObjPtr vm,int type,int size){


*
 * LIGHTOSLOAD defined in m4,should be lightoresize,
 * reisze.sh default path 
 *
 * type = 1,resize the mem
 * type = 2,resize the vcpu 
 *
	

	int vmid; //vm id,in virDomainObjPtr->privateData
	virCommandPtr cmd = virCommandNew(LIGHTOS);
	vmid = vm->privateData->vmid;
	if(type==1)
		virCommandAddArgPair(cmd,"-mem",size);
	else if(type==2)
		virCommandAddArgPair(cmd,"-vcpu",size);
	else
		goto error;

	return cmd;

error:
	virCommandFree(cmd);
	return NULL;
}
*/
