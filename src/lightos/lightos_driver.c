/*
 * lightos.c: A "mock" hypervisor for use by application unit tests
 *
 * Copyright (C) 2006-2014 Red Hat, Inc.
 * Copyright (C) 2006 Daniel P. Berrange
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
 * Daniel Berrange <berrange@redhat.com>
 */

#include <config.h>

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <libxml/xmlsave.h>
#include <libxml/xpathInternals.h>


#include "virerror.h"
#include "datatypes.h"
#include "virbuffer.h"
#include "viruuid.h"
#include "capabilities.h"
#include "configmake.h"
#include "viralloc.h"
#include "network_conf.h"
#include "interface_conf.h"
#include "domain_conf.h"
#include "domain_event.h"
#include "network_event.h"
#include "snapshot_conf.h"
#include "fdstream.h"
#include "storage_conf.h"
#include "node_device_conf.h"
#include "virxml.h"
#include "virthread.h"
#include "virlog.h"
#include "virfile.h"
#include "virtypedparam.h"
#include "virrandom.h"
#include "virstring.h"
#include "cpu/cpu.h"
#include "virauth.h"
#include "access/viraccessapicheck.h"
#include "nodeinfo.h"

#include "lightos_util.h"
#include "lightos_driver.h"
#include "lightos_process.h"

#define VIR_FROM_THIS VIR_FROM_LIGHTOS



lightosConnPtr lightos_driver = NULL;
static virMutex defaultLock;
static int
lightosOnceInit(void)
{
	return virMutexInit(&defaultLock);
}

VIR_ONCE_GLOBAL_INIT(lightos)


void
lightosDriverLock(lightosConnPtr driver){
	virMutexLock(&driver->lock);
}	

void
lightosDriverUnlock(lightosConnPtr driver){
	virMutexUnlock(&driver->lock);
}



static virDomainObjPtr
lightosDomObjFromDomain(virDomainPtr domain)
{
	virDomainObjPtr vm;
	lightosConnPtr privconn = domain->conn->privateData;
	char uuidstr[VIR_UUID_STRING_BUFLEN];

	vm = virDomainObjListFindByUUID(privconn->domains,domain->uuid);
	if(!vm)
	{
		virUUIDFormat(domain->uuid,uuidstr);
		virReportError(VIR_ERR_NO_DOMAIN,
				_("no domain with mathcing uuid '%s' (%s'"),
				uuidstr,domain->name);
		return NULL;
	}
	return vm;
}

static virDrvOpenStatus
lightosConnectOpen(virConnectPtr conn,
		   virConnectAuthPtr auth ATTRIBUTE_UNUSED,
		   unsigned int flags)
{
    //int ret;
    virCheckFlags(VIR_CONNECT_RO, VIR_DRV_OPEN_ERROR);

    if (lightosInitialize() < 0)
        return VIR_DRV_OPEN_ERROR;

    if (!conn->uri)
        return VIR_DRV_OPEN_DECLINED;

    if (!conn->uri->scheme || STRNEQ(conn->uri->scheme, "lightos"))
        return VIR_DRV_OPEN_DECLINED;


    /* From this point on, the connection is for us. */
    if (!conn->uri->path
        || conn->uri->path[0] == '\0'
        || (conn->uri->path[0] == '/' && conn->uri->path[1] == '\0')) {
        virReportError(VIR_ERR_INVALID_ARG,
                       "%s", _("lightosOpen: supply a path or use lightos:///default"));
        return VIR_DRV_OPEN_ERROR;
    }

    if (STREQ(conn->uri->path, "/default"))
  //      ret = testOpenDefault(conn);
  	printf("need call lightosOpen\n");

//    if (ret != VIR_DRV_OPEN_SUCCESS)
//        return ret;

    return VIR_DRV_OPEN_SUCCESS;

}


static int lightosConnectClose(virConnectPtr conn)
{
	conn->privateData = NULL;
	return 0;
}



static int
lightosDomainCreate(virDomainPtr dom)
{
	lightosConnPtr privconn = dom->conn->privateData;
	virDomainObjPtr vm;
	int ret = -1;

	if(!(vm = lightosDomObjFromDomain(dom)))
		goto cleanup;

	if(virDomainCreateEnsureACL(dom->conn,vm->def)<0)
		goto cleanup;
	if (virDomainObjIsActive(vm)){
		virReportError(VIR_ERR_OPERATION_INVALID,
			"%s",_("Domain is already running"));
		goto cleanup;
	}
	
	ret = virLightosProcessStart(dom->conn,privconn,vm,VIR_DOMAIN_RUNNING_BOOTED);

cleanup:
	virObjectUnlock(vm);
	return ret;
}



static virDriver lightosDriver = {
	.no = VIR_DRV_LIGHTOS,
	.name = "lightos",
	.connectOpen = lightosConnectOpen,
	.connectClose = lightosConnectClose,
	.domainCreate = lightosDomainCreate,
};

int lightosRegister(void){
	
	printf("lightosRegister\n");
	virRegisterDriver(&lightosDriver);
	printf("Register!\n");
	return 0;
}
