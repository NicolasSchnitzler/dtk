/* dtkVrManager.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Feb 18 10:09:51 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Feb 25 11:27:30 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 59
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkVrManager.h"
#include "dtkVrMaster.h"
#include "dtkVrSlave.h"

#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkLog.h>

#include <dtkDistributed/dtkDistributedCommunicator.h>
#include <dtkDistributed/dtkDistributedCommunicatorMpi.h>

class dtkVrManagerPrivate
{
public:
    dtkDistributedCommunicatorMpi *communicator;
};

dtkVrManager *dtkVrManager::instance(void)
{
    if(!s_instance)
        s_instance = new dtkVrManager;

    return s_instance;
}

void dtkVrManager::initialize(void)
{
    d->communicator->initialize();
}

void dtkVrManager::uninitialize(void)
{
    d->communicator->uninitialize();
}

dtkVrProcess *dtkVrManager::create(QString type)
{
    dtkAbstractView *view = dtkAbstractViewFactory::instance()->create(type);

    if(!view)
        dtkWarning() << "no" << type << "specialization is available";

    dtkVrProcess *process = NULL;

    if(d->communicator->rank()) {
        process = new dtkVrSlave(d->communicator);
        dynamic_cast <dtkVrSlave *>(process)->setView(view);
    } else {
        process = new dtkVrMaster(d->communicator);
    }

    return process;
}

dtkVrManager::dtkVrManager(void) : QObject(), d(new dtkVrManagerPrivate)
{
    d->communicator = new dtkDistributedCommunicatorMpi;
}

dtkVrManager::~dtkVrManager(void)
{
    delete d;

    d = NULL;
}

dtkVrManager *dtkVrManager::s_instance = NULL;
