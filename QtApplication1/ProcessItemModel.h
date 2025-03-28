#pragma once
#include <qabstractitemmodel.h>
#include "ProcessFinder.h"

class QProcessItemModel : public QAbstractTableModel
{
    typedef QAbstractTableModel Super;
public:
    QProcessItemModel(QList<FProcessDesc> users, QObject* parent = Q_NULLPTR)
        : QAbstractTableModel(parent), ProcessList(ProcessList) {}

    /*
    QProcessItem() = default;
    explicit QProcessItem(const QString &text) : Super(text) {};
    QProcessItem(const QIcon &icon, const QString &text) : Super(icon, text) {};
    explicit QProcessItem(int rows, int columns = 1) : Super(rows, columns) {};
    */

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || role != Qt::DisplayRole) {
            return QVariant();
        }

        const FProcessDesc& process = ProcessList.at(index.row());
        return QString("%1 (PID: %2)").arg(process.ProcessName).arg(process.ProcessID);
    }
    void appendRow(const FProcessDesc& processDesc) {
        beginInsertRows(QModelIndex(), ProcessList.size(), ProcessList.size());
        ProcessList.append(processDesc);
        endInsertRows();
    }
    int rowCount(const QModelIndex &) const override
    {
        return ProcessList.size();
    }
    int columnCount(const QModelIndex &) const override
    {
        return 1;
    }
private:
    QList<FProcessDesc> ProcessList{};
};