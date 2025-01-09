#include "logmodel.h"

LogModel::LogModel(QObject* parent)
    : QAbstractListModel{parent}
{
}

LogModel::~LogModel()
{
}

int LogModel::rowCount(const QModelIndex& parent) const
{
    return this->_logList.count();
}

QVariant LogModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        return _logList[index.row()];
    }
    return QVariant();
}

void LogModel::setLogList(QStringList list)
{
    beginResetModel();
    this->_logList = list;
    endResetModel();
}

void LogModel::appendLogList(QString log)
{
    int maxLogs = 10000;
    if (_logList.size() >= maxLogs) {
        _logList.removeFirst();
    }
    beginResetModel();
    this->_logList.append(log);
    endResetModel();
}

QStringList LogModel::getLogList() const
{
    return this->_logList;
}
