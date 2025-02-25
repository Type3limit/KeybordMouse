#ifndef T_LOGMODEL_H
#define T_LOGMODEL_H

#include <QAbstractListModel>

class LogModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit LogModel(QObject* parent = nullptr);
    ~LogModel();
    void setLogList(QStringList list);
    void appendLogList(QString log);
    QStringList getLogList() const;

protected:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

private:
    QStringList _logList;
};

#endif // T_LOGMODEL_H
