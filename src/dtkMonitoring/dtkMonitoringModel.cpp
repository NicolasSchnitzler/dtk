#include "dtkMonitoringModel.h"

#include <QMetaType>
#include <dtkComposer/dtkComposerNode>

dtkMonitoringModel::dtkMonitoringModel()
{

}

dtkMonitoringModel::~dtkMonitoringModel()
{

}

int dtkMonitoringModel::rowCount(const QModelIndex &parent) const
{
    return dtkMonitoringController::instance()->nodeCount();
}

QVariant dtkMonitoringModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
            return QVariant();

    if (index.row() >= dtkMonitoringController::instance()->nodeCount())
        return QVariant();

    if (role == Qt::DisplayRole)
        return QVariant::fromValue(dtkMonitoringController::instance()->nodeList().at(index.row()));
    else
        return QVariant();
}

QMimeData * dtkMonitoringModel::mimeData(const QModelIndexList & indexes) const
{
    QMimeData* mimeData =new QMimeData();
    QString str;
    for(QModelIndex index:indexes)
    {
        dtkComposerNode* node=dtkMonitoringController::instance()->nodeList().at(index.row());
            str+=QString::number((qlonglong)(node))+" ";
    }
    mimeData->setText(str);
    return mimeData;
}

QStringList dtkMonitoringModel::mimeTypes() const
{
    return QStringList()<<"text/plain";
}

Qt::ItemFlags dtkMonitoringModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled;

    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return  defaultFlags;
}

void dtkMonitoringModel::update(int index)
{
    emit dataChanged(createIndex(index,0), createIndex(index,0));
}
