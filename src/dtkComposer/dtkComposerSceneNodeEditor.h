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

#include "dtkComposerExport.h"

#include <QtWidgets>

class dtkComposerScene;
class dtkComposerSceneNode;
class dtkComposerSceneNodeEditorPrivate;
class dtkComposerStack;
class dtkComposerGraph;

class DTKCOMPOSER_EXPORT dtkComposerSceneNodeEditor : public QWidget
{
    Q_OBJECT

public:
     dtkComposerSceneNodeEditor(QWidget *parent = 0);
    ~dtkComposerSceneNodeEditor(void);

public slots:
    virtual void setNode(dtkComposerSceneNode *node);
    virtual void setScene(dtkComposerScene *scene);
    virtual void setStack(dtkComposerStack *stack);
    virtual void setGraph(dtkComposerGraph *graph);

public slots:
    virtual void clear(void);

protected slots:
    void addLoopPort(void);
    void removeLoopPort(void);

protected slots:
    void addBlock(void);
    void removeBlock(void);

protected slots:
    void addInputPort(void);
    void removeInputPort(void);

protected slots:
    void addOutputPort(void);
    void removeOutputPort(void);

protected slots:
    void onBlockChanged(int index);
    void onTitleChanged(const QString& text);

protected slots:
    void onBrowse(void);
    void onBrowseDirectory(void);
    void onValueChanged(bool);
    void onValueChanged(int);
    void onValueChanged(double);
    void onValueChanged(const QString&);
    void onImplementationChanged(const QString&);
    void onMonitoringChanged(bool checked);

protected:
    dtkComposerSceneNodeEditorPrivate *d;
};

//
// dtkComposerSceneNodeEditor.h ends here
