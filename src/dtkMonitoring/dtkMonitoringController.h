// Version: $Id$
//
//

// Commentary:
//
//

// Change Log:
//
//

// Code:

#pragma once

#include <QtCore>

#include "dtkMonitoringExport.h"

class dtkComposerNode;
class dtkMonitor;

class DTKMONITORING_EXPORT dtkMonitoringController : public QObject
{
    Q_OBJECT

public:
    static dtkMonitoringController *instance(void);

public:
    void registerNode(dtkComposerNode *);

public:
    void replaceMonitor(dtkComposerNode *, dtkMonitor *);
    void removeMonitor(dtkMonitor *);

public:
    int nodeCount() const;
    QList<dtkComposerNode*> nodeList() const;

public:
    dtkMonitor* monitor(dtkComposerNode *);

private:
     dtkMonitoringController(void);
    ~dtkMonitoringController(void);

private:
    static dtkMonitoringController *s_instance;

private:
    class dtkMonitoringControllerPrivate *d;
};

//
// dtkMonitoringController.h ends here
