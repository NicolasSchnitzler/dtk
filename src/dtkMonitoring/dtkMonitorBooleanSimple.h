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

#include "dtkMonitor.h"
#include "dtkMonitorBoolean.h"

class dtkMonitorBooleanSimple : public dtkMonitorBoolean
{
public:
     dtkMonitorBooleanSimple(QGraphicsItem *parent = 0);
    ~dtkMonitorBooleanSimple(void);
private:
    class dtkMonitorBooleanSimplePrivate *d;
};


//
// dtkMonitorBoolean.h ends here
