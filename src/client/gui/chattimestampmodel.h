#ifndef CHATTIMESTAMPMODEL_H
#define CHATTIMESTAMPMODEL_H
#include <QtCore>

class ChatTimeStampModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ChatTimeStampModel(QObject *parent = 0);
    void setEntries(const QList<QPair<QDateTime, QString> > &entries);
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;

private:
    QList<QPair<QDateTime, QString> > modeldata;
};

#endif // CHATTIMESTAMPMODEL_H
