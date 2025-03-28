#pragma once
#include "qmainwindow.h"
#include "Windows.h"
#include <qlistview.h>

QT_FORWARD_DECLARE_CLASS(QStandardItemModel)
QT_FORWARD_DECLARE_CLASS(QSpinBox)
QT_FORWARD_DECLARE_CLASS(QVBoxLayout)
QT_FORWARD_DECLARE_CLASS(QHBoxLayout)
QT_FORWARD_DECLARE_CLASS(QChartView)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QComboBox)

enum class EProcessItemDataRole
{
    ERole_ProcessID = Qt::UserRole,
};

enum class EProcessFilterType : quint8
{
    None,
    MemoryType,
    MemoryState,
    MemoryProtectionType,
};

inline EProcessFilterType operator|(EProcessFilterType a, EProcessFilterType b)
{
    return static_cast<EProcessFilterType>(static_cast<std::underlying_type_t<EProcessFilterType>>(a) | static_cast< std::underlying_type_t<EProcessFilterType>>(b));
}

class QProcessMonitor : public QMainWindow
{
    typedef QMainWindow Super;
    typedef QProcessMonitor ThisClass;
    inline static constexpr size_t defaultNumProcesses = 20;
    inline static constexpr size_t defaultMaxNumProcesses = 999;
public:
    explicit QProcessMonitor(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    void UpdateProcessList();
    ~QProcessMonitor();
    
protected:
    void CreateWidgets();
    void OnProcessSelected(const QItemSelection& selected, const QItemSelection& deselected);
    void UpdateSelectedProccessInfo();
    bool DisplayProcessChart(DWORD ProcessID) const;
    void OnSortTypeChanged(int ComboItemIndex);
private:
    // Widgets
    QWidget*            RootWidget          = nullptr;
    QVBoxLayout*        LVerticalLayout     = nullptr;
    QVBoxLayout*        RVerticalLayout     = nullptr;
    QHBoxLayout*        HorizontalLayout    = nullptr;
    QListView*          ProccessList        = nullptr;
    QSpinBox*           NumberSpinBox       = nullptr;
    QChartView*         PieChartView        = nullptr;
    QComboBox*          SortTypeCombo       = nullptr;
    QStandardItemModel* ItemModel           = nullptr;

    // Sorting of processes
    EProcessFilterType AppliedFilter = EProcessFilterType::None;
    DWORD DisplayedProcessID = NULL;
};