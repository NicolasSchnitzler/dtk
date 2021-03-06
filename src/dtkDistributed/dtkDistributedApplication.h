/* dtkDistributedApplication.h ---
 *
 * Author: Nicolas Niclausse
 * Copyright (C) 2015 - Nicolas Niclausse, Inria.
 * Created: 2015/02/04 12:28:51
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#pragma once

#include "dtkDistributedExport.h"

#include <dtkCore/dtkApplication.h>

class dtkDistributedCommunicator;
class dtkDistributedPolicy;
class dtkDistributedApplicationPrivate;

class DTKDISTRIBUTED_EXPORT dtkDistributedApplication: public dtkApplication
{
public:
             dtkDistributedApplication(int &argc, char **argv);
    virtual ~dtkDistributedApplication(void);

public:
    virtual void initialize(void);
    virtual void exec(QRunnable *task);
    virtual void spawn(QMap<QString, QString> options = QMap<QString, QString>() );
    virtual void unspawn(void);

public:
    bool isMaster(void);
    dtkDistributedCommunicator *communicator(void);
    dtkDistributedPolicy *policy(void);

private:
    dtkDistributedApplicationPrivate *d;

};


