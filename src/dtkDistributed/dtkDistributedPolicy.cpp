/* @(#)dtkDistributedPolicy.cpp ---
 *
 * Author: Nicolas Niclausse
 * Copyright (C) 2013 - Nicolas Niclausse, Inria.
 * Created: 2013/02/18 15:18:41
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include <dtkCore/dtkCore.h>

#include "dtkDistributedCoreApplication.h"
#include "dtkDistributedPolicy.h"

#include <dtkDistributed>
#include <dtkLog>

#include <QCommandLineParser>

class dtkDistributedPolicyPrivate
{
public:
    QStringList hosts;

public:
    qlonglong nthreads;
    QString type;

public:
    dtkDistributedCommunicator *comm;
};

/*!
  \class dtkDistributedPolicy
  \inmodule dtkDistributed
  \brief dtkDistributedPolicy is used to setup the communicator  (plugin, hostnames, number of processes)


  The policy can be set directly (using dtkDistributedPolicy::addHosts) or can be discover from the environments.
  OAR and Torque resources can be discover automatically; you can also use the DTK_NUM_THREADS variable.

*/

dtkDistributedPolicy::dtkDistributedPolicy(void) : QObject(), d(new dtkDistributedPolicyPrivate)
{
    d->comm = NULL;
    d->nthreads = 1;
    d->type = "qthread";
}

dtkDistributedPolicy::~dtkDistributedPolicy(void)
{
    delete d;

    d = NULL;
}

dtkDistributedPolicy::dtkDistributedPolicy(const dtkDistributedPolicy& other)
{
}

dtkDistributedPolicy& dtkDistributedPolicy::operator = (const dtkDistributedPolicy& other)
{
    return (*this);
}

void dtkDistributedPolicy::addHost(QString host)
{
    dtkTrace() << "add host " << host;

    d->hosts.append(host);
}

dtkDistributedCommunicator *dtkDistributedPolicy::communicator(void)
{
    return d->comm;
}

void dtkDistributedPolicy::setType(const QString& type)
{
    dtkDebug() << "create" << type << "communicator";
    d->type = type;
    d->comm = dtkDistributed::communicator::pluginFactory().create(type);
    if (!d->comm)
        qWarning() << "NULL communicator !!" << type;
}

QStringList dtkDistributedPolicy::types(void)
{
    return dtkDistributed::communicator::pluginFactory().keys();
}

QStringList dtkDistributedPolicy::hosts(void)
{
    if (d->hosts.count() == 0) {
        //Try to get hostsfile from env
        QStringList schedulers;
        schedulers << "PBS_NODEFILE";
        schedulers << "OAR_NODEFILE";
        foreach (QString envname, schedulers) {
            QString nodefile =  QString::fromUtf8(qgetenv(qPrintable(envname)));
            if (!nodefile.isEmpty()) {
               dtkDebug() << "Extracting hosts from file" << nodefile;
                QFile file(nodefile);
                if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    dtkWarn() << "Error while opening"<< nodefile;
                    return d->hosts;
                }
                QTextStream in(&file);
                while (!in.atEnd()) {
                    d->hosts <<  in.readLine();
                }
                if (d->type == "hybrid") {
                    d->nthreads = d->hosts.count();
                    d->hosts.removeDuplicates();
                    d->nthreads /= d->hosts.count();
                }
                return d->hosts;
            }
        }
        dtkDebug() << "No hostfile found, try qapp args";
        d->nthreads = 1;
// skip this part on windows with qt < 5.5 because of QTBUG-30330
#if QT_VERSION < 0x050500 && defined Q_OS_WIN32
        dtkWarn() << "skip running parser on windows";
        d->hosts <<  "localhost";
#else
        QCommandLineParser *parser = dtkDistributed::app()->parser();
        QCommandLineOption npOption("np","number of processes","int");
        QCommandLineOption ntOption("nt","number of threads","int");
        parser->process(qApp->arguments());
        int np = 1;
        if (parser->isSet(npOption)) {
            dtkTrace() << "got np value from command line" ;
            np = parser->value(npOption).toInt();
        } else {
            QByteArray numprocs = qgetenv("DTK_NUM_THREADS");
            if (!numprocs.isEmpty()) {
                np = numprocs.toInt();
                dtkDebug() << "got num procs from env" << np;
            }
        }
        for (int i = 0; i <  np; i++) {
            d->hosts <<  "localhost";
        }
        //FIXME: rework hybrid case
        if (parser->isSet(ntOption)) {
                if (d->type == "mpi") {
                    int nt = parser->value(ntOption).toInt();
                    int total = d->hosts.count() * nt;
                    for (int i = d->hosts.count(); i <  total; i++) {
                        d->hosts <<  "localhost";
                    }
                } else {
                    d->nthreads = parser->value(ntOption).toInt();
                }
        }
#endif
        dtkDebug() << "policy updated, hosts:" << d->hosts.count() << "threads:" <<  d->nthreads;
    }
    return d->hosts;
}

void dtkDistributedPolicy::setNThreads(qlonglong nthreads)
{
    d->nthreads = nthreads;
}

qlonglong dtkDistributedPolicy::nthreads(void)
{
    return d->nthreads;
}

