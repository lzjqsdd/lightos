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
    virCheckFlags(VIR_CONNECT_RO, VIR_DRV_OPEN_ERROR);

    if (lightosInitialize() < 0)
        return VIR_DRV_OPEN_ERROR;

 //   if(conn->uri == NULL)
//	if (lightos_driver == NULL)
//		return VIR_DRV_OPEN_DECLINED;
    

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

//    if (STREQ(conn->uri->path, "/default"))
//        ret = testOpenDefault(conn);
//  	printf("need call lightosOpen\n");

    if(lightos_driver == NULL)
        virReportError(VIR_ERR_INTERNAL_ERROR,
		"%s",_("lightos state driver is ont active!!!!"));

 //   if (STREQ(conn->uri->path, "/default"))
  //  {
//	if(virConnectOpenEnsureACL(conn)< 0)
 //           return VIR_DRV_OPEN_ERROR;
   // }

    conn->privateData = lightos_driver;
    return VIR_DRV_OPEN_SUCCESS;

}


static int lightosConnectClose(virConnectPtr conn)
{
	conn->privateData = NULL;
	return 0;
}


static virDomainPtr
lightosDomainDefineXML(virConnectPtr conn,const char * xml)
{
    lightosConnPtr privconn = conn->privateData;
    virDomainPtr dom = NULL;
    virDomainDefPtr def = NULL;
    virDomainDefPtr oldDef = NULL;
    virDomainObjPtr vm = NULL;

    if((def = virDomainDefParseString(xml,privconn->caps,privconn->xmlopt,
                                    1<<VIR_DOMAIN_VIRT_LIGHTOS,
                                     VIR_DOMAIN_XML_INACTIVE)) == NULL)
    {
        goto cleanup;
    }

    if(!(vm = virDomainObjListAdd(privconn->domains,def,privconn->xmlopt,
                                0,&olddef)))
        goto cleanup;

    def = NULL;
    vm->persistent = 1;

    dom = virGetDomain(conn,vm->def->name,vm->def->uuid);
    if(dom)
        dom->id = vm->def->id;
    if(virDomainSaveConfig(LIGHTOS_CONFIG_DIR,vm->def)<0)
        goto cleanup;

cleanup:
    virDomainDefFree(def);
    virObjectUnlock(vm);

    return dom;
}

/*
static int 
lightosDomainUndefine(virDomainPtr domain)
{
    lightosConnPtr privconn = domain->conn->privateData;
    virDomainObjPtr vm;
    int ret = -1;

    if(!(vm = lightosDomObjFromDomain(domain)))
        goto cleanup;
    if(!vm->persistent){
        virReportError(VIR_ERR_OPERATION_INVALID,
                      "%s",_("cannot undefine transient domain"));
        goto cleanup;
    }

    if(virDomainDeleteConfig(LIGHTOS_CONFIG_DIR,
                            LIGHTOS_AUTOSTART_DIR,
                            vm)<0)
        goto cleanup;

    if(virDomainObjIsActive(vm)){
        vm->persistent = 0;
    }else{
        virDomainObjListRemove(privconn->domains,vm);
        vm = NULL;
    }
    ret = 0;

cleanup:
    virObjectUnlock(vm);
    return ret;
}
*/


static int
lightosConnectListDomains(virConnectPtr conn,int *ids,int maxids)
{
    lightosConnPtr privconn = conn->privateData;
    int n;

    n = virDomainObjListGetActiveIDs(privconn->domains,ids,maxids,
                                     NULL,NULL);

    return n;
}


static int
lightosConnectNumOfDomains(virConnectPtr conn)
{
    lightosConnPtr privconn = conn->privateData;
    int count;
    
    count = virDomainObjListNumOfDomains(privconn->domains,true,NULL,NULL);

    return count;
}


static int 
lightosConnectListDefinedDomains(virConnectPtr conn,char **const names,int maxnames){
    
    lightosConnPtr privconn = conn->privateData;
    int n;

    memset(names,0,sizeof(*names)*maxnames);
    n = virDomainObjListGetInactiveNames(privconn->domains,names,maxnames,NULL,NULL);

    return n;
}

static int
lightosConnectNumOfDefinedDomains(virConnectPtr conn){

    lightosConnPtr privconn = conn->privateData;
    int count;

    count = virDomainObjListNumOfDomains(privconn->domains,false,NULL,NULL);
    return count;

}

static int
lightosConnectListAllDomains(virConnectPtr conn,virDomainPtr **domains,unsigned int flags){
    
    lightosConnPtr privconn = conn->privateData;
    int ret;

    virCheckFlags(VIR_CONNECT_LIST_DOMAINS_FILTERS_ALL,-1);
    ret = virDomainObjListExport(privconn->domains,conn,domains,NULL,flags);
    return ret;

}

static virDomainPtr lightosDomainLookupByUUID(virConnectPtr conn,
                                             const unsigned char *uuid)
{
    lightosConnPtr privconn = conn->privateData;
    virDomainPtr ret = NULL;
    virDomainObjPtr dom;

    dom = virDomainObjListFindByUUID(privconn->domains,uuid);

    if(dom == NULL){
        virReportError(VIR_ERR_NO_DOMAIN,NULL);
        goto cleanup;
    }
    ret = virGetDomain(conn,dom->def->name,dom->def->uuid);

    if(ret){
        ret->id = dom->def->id;
    }
cleanup:
    if(dom)
        virObjectUnlock(dom);
    return ret;
}

static virDomainPtr lightosDomainLookupByName(virConnectPtr conn,
                                              const char *name)
{
    lightosConnPtr privconn = conn->privateData;
    virDomainPtr ret = NULL;
    virDomainObjPtr dom;

    dom = virDomainObjListFindByName(privconn->domains,name);
    if(dom == NULL){
        virReportError(VIR_ERR_NO_DOMAIN,NULL);
        goto cleanup;
    }
    ret = virGetDomain(conn,dom->def->name,dom->def->uuid);
    if(ret)
        ret->id = dom->def->id;

cleanup:
    if(dom)
        virObjectUnlock(dom);
    return ret;
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



static int
lightosStateCleanup(void)
{
    VIR_DEBUG("lightos state cleanup");

    if (lightos_driver == NULL)
        return -1;

    virObjectUnref(lightos_driver->domains);
    virObjectUnref(lightos_driver->caps);
    virObjectUnref(lightos_driver->xmlopt);

    virMutexDestroy(&lightos_driver->lock);
    VIR_FREE(lightos_driver);

    return 0;
}




static int
lightosStateInitialize(bool priveleged ATTRIBUTE_UNUSED,
                     virStateInhibitCallback callback ATTRIBUTE_UNUSED,
                     void *opaque ATTRIBUTE_UNUSED)
{
    if (!priveleged) {
        VIR_INFO("Not running priveleged, disabling driver");
        return 0;
    }

    if (VIR_ALLOC(lightos_driver) < 0) {
        printf("Alloc lightos_drvier error!\n");
        return -1;
    }

    if (virMutexInit(&lightos_driver->lock) < 0) {
        VIR_FREE(lightos_driver);
        return -1;
    }

  //  if (!(lightos_driver->caps = lightosBuildCapabilities()))
  //      goto cleanup;

    if (!(lightos_driver->xmlopt = virDomainXMLOptionNew(NULL, NULL, NULL)))
        goto cleanup;

    if (!(lightos_driver->domains = virDomainObjListNew()))
        goto cleanup;

    if (virFileMakePath(LIGHTOS_LOG_DIR) < 0) {
        virReportSystemError(errno,
                             _("Failed to mkdir %s"),
                             LIGHTOS_LOG_DIR);
        goto cleanup;
    }

    if (virFileMakePath(LIGHTOS_STATE_DIR) < 0) {
        virReportSystemError(errno,
                             _("Failed to mkdir %s"),
                             LIGHTOS_LOG_DIR);
        goto cleanup;
    }

    if (virDomainObjListLoadAllConfigs(lightos_driver->domains,
                                       LIGHTOS_CONFIG_DIR,
                                       NULL, 0,
                                       lightos_driver->caps,
                                       lightos_driver->xmlopt,
                                       1 << VIR_DOMAIN_VIRT_LIGHTOS,
                                       NULL, NULL) < 0)
	goto cleanup;
    return 0;

cleanup:
    lightosStateCleanup();
    return -1;
}


static int lightosConnectGetVersion(virConnectPtr conn ATTRIBUTE_UNUSED,
				unsigned long *hvVer)
{
	*hvVer = 2;
	return 0;
}



static int lightosDomainCreate(virDomainPtr ATTRIBUTE_UNUSED)
{
	lightosConnPtr privconn = domain->conn->privateData;
	virDomainObjPtr privdom;

	virObjectEventPtr event = NULL;
	
	virCheckFlags(0,-1);
	lightosDriverLock(privconn);
	
	/*judge wheather the domain was exists*/
	
}


static virDriver lightosDriver = {
	.no = VIR_DRV_LIGHTOS,
	.name = "lightos",
	.connectOpen = lightosConnectOpen,
	.connectClose = lightosConnectClose,
	.domainCreate = lightosDomainCreate,
	.connectGetVersion = lightosConnectGetVersion,
    .connectListDomains = lightosConnectListDomains,
    .connectListAllDomains = lightosConnectListAllDomains,
    .connectNumOfDomains = lightosConnectNumOfDomains,
    .connectNumofDefinedDomains = lightosConnectNumOfDefinedDomains,
    .domainDefineXML = lightosDomainDefineXML,
    .domainLookupByUUID = lightosDomainLookupByUUID,
    .domainLookupByName = lightosDomainLookupByName,
};

static virStateDriver lightosStateDriver = {
	.name="lightos",
	.stateInitialize = lightosStateInitialize,
	.stateCleanup = lightosStateCleanup,
};

int lightosRegister(void){
	
	//printf("lightosRegister\n");
	virRegisterDriver(&lightosDriver);
	virRegisterStateDriver(&lightosStateDriver);
	printf("Lightos Register!\n");
	return 0;
}
