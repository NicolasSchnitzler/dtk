/* dtkDistributedController.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed May 25 14:13:03 2011 (+0200)
 * Version: $Id$
 * Last-Updated: ven. nov. 25 09:54:00 2011 (+0100)
 *           By: Nicolas Niclausse
 *     Update #: 51
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKDISTRIBUTEDCONTROLLER_H
#define DTKDISTRIBUTEDCONTROLLER_H

#include "dtkDistributedExport.h"

#include <QtCore>
#include <QtNetwork>

class dtkDistributedControllerPrivate;
class dtkDistributedNode;

class DTKDISTRIBUTED_EXPORT dtkDistributedController : public QObject
{
    Q_OBJECT

public:
     dtkDistributedController(void);
    ~dtkDistributedController(void);

    bool    isConnected(const QUrl& server);
    bool isDisconnected(const QUrl& server);

signals:
    void    connected(const QUrl& server);
    void disconnected(const QUrl& server);

    void updated(void);
    void updated(const QUrl& server);

    void dataPosted(const QByteArray& data);

public slots:
    void    connect(const QUrl& server);
    void     deploy(const QUrl& server);
    void disconnect(const QUrl& server);
    void     submit(const QUrl& server, QByteArray& resources);

public:
    QList<dtkDistributedNode *> nodes(void);
    QList<dtkDistributedNode *> nodes(const QString& cluster);

protected slots:
    void read(void);
    void error(QAbstractSocket::SocketError error);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus );
    void cleanup(void);

private:
    dtkDistributedControllerPrivate *d;
};

#endif
