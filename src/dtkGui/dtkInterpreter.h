/* dtkInterpreter.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Apr 10 15:29:01 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Sat May 16 00:17:28 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 34
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKINTERPRETER_H
#define DTKINTERPRETER_H

#include "dtkGuiExport.h"
#include <dtkGui/dtkTextEditor.h>

class dtkScriptInterpreter;

class dtkInterpreterPrivate;
class dtkInterpreterPreferencesWidget;

class DTKGUI_EXPORT dtkInterpreter : public dtkTextEditor
{
    Q_OBJECT

public:
     dtkInterpreter(QWidget *parent = 0);
    ~dtkInterpreter(void);

    void registerInterpreter(dtkScriptInterpreter *interpreter);

    QString filter(QString text);

    dtkInterpreterPreferencesWidget *preferencesWidget(QWidget *parent = 0);

public slots:
    void  readSettings(void);
    void writeSettings(void);

    void onKeyUpPressed(void);
    void onKeyDownPressed(void);
    void onKeyLeftPressed(void);
    void onKeyRightPressed(void);
    void onKeyEnterPressed(void);
    void onKeyBackspacePressed(void);

    void output(const QString& result,  int *stat);

signals:
    void   input(const QString& command, int *stat);
    void    load(const QString& path);
    void    save(const QString& path);
    void    help(void);
    void stopped(void);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    bool eventFilter(QObject *object, QEvent *event);

private:
    dtkInterpreterPrivate *d;
};

#endif
