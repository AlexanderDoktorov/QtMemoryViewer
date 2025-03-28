#include "ProcessMonitor.h"
#include "ProcessItemModel.h"
#include <ProcessFinder.h>
// widgets
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>
#include <QStandardItemModel>
#include <QComboBox>
// charts
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
// std
#include <unordered_map>
#include <ranges>
#include <numeric>
#include <algorithm>


QProcessMonitor::QProcessMonitor(QWidget* parent, Qt::WindowFlags flags)
    : Super(parent, flags)
{
    CreateWidgets();

    // Set central widget
    setCentralWidget(RootWidget);

    // Setup layouts
    HorizontalLayout->addLayout(LVerticalLayout, 0);
    HorizontalLayout->addLayout(RVerticalLayout, 0);
    RootWidget->setLayout(HorizontalLayout);

    // Setup process list and connect selecting items to callback
    ProccessList->setModel(ItemModel);
    QObject::connect(ProccessList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ThisClass::OnProcessSelected);

    // Setup spin box
    NumberSpinBox->setMinimum(0);
    NumberSpinBox->setMaximum(defaultMaxNumProcesses);
    NumberSpinBox->setValue(defaultNumProcesses);
    QObject::connect(NumberSpinBox, &QSpinBox::valueChanged, [this](int) {
        UpdateProcessList();
        });

    // Create and setup pie chart for chart view
    QPieSeries* PieSeries = new QPieSeries(RootWidget);
    PieSeries->append("Cat 1", .33f);
    PieSeries->append("Cat 2", .33f);
    PieSeries->append("Cat 3", .33f);

    QPieSlice* FirtsSlice = PieSeries->slices().at(1);
    FirtsSlice->setBrush(Qt::green);

    QChart* PieChart = new QChart();
    PieChart->addSeries(PieSeries);
    PieChart->setTitle("Memory Regions");

    // Create a QChartView to visualize the chart
    PieChartView->setRenderHint(QPainter::Antialiasing);  // For better rendering quality
    PieChartView->setChart(PieChart);

    // add widgets to right Vertical layout
    RVerticalLayout->addWidget(SortTypeCombo);
    RVerticalLayout->addWidget(PieChartView);

    // add widgets to left Vertical layout
    LVerticalLayout->addWidget(ProccessList);
    LVerticalLayout->addWidget(NumberSpinBox);

    // Make initial update of process list
    UpdateProcessList();
}

void QProcessMonitor::UpdateProcessList()
{
    ItemModel->clear();

    std::vector<FProcessDesc> ProcessList = FProcessFinder::GetProcessList(NumberSpinBox->value());

    for (size_t i = 0; i < ProcessList.size(); ++i)
    {
        QStandardItem* Item = new QStandardItem(ProcessList[i].ProcessName);
        Item->setData(static_cast<unsigned int>(ProcessList[i].ProcessID), static_cast<int>(EProcessItemDataRole::ERole_ProcessID));
        Item->setEditable(false);

        ItemModel->appendRow(Item);
    }
}

QProcessMonitor::~QProcessMonitor()
{
    if (PieChartView)
        delete PieChartView;
}

void QProcessMonitor::CreateWidgets()
{
    // Create Central widget
    RootWidget = new QWidget(this);
    RootWidget->setObjectName("Central Widget");

    // Create horizontal layout
    HorizontalLayout = new QHBoxLayout(RootWidget);
    HorizontalLayout->setObjectName("Horizontal layout");

    // Create vertical layouts
    LVerticalLayout = new QVBoxLayout(nullptr);
    LVerticalLayout->setObjectName("Left Vertical layout");

    RVerticalLayout = new QVBoxLayout(nullptr);
    RVerticalLayout->setObjectName("Right Vertical layout");

    // Create Item Model
    ItemModel    = new QStandardItemModel(RootWidget);
    ItemModel->setObjectName("Standart item model");

    // Create List View
    ProccessList = new QListView(RootWidget);
    ProccessList->setObjectName("Process List View");

    // Create Spin Box for defining how many processess we will display
    NumberSpinBox = new QSpinBox(RootWidget);
    NumberSpinBox->setObjectName("Number of processes");

    // Create a QChartView to visualize the chart
    PieChartView = new QChartView(RootWidget);
    PieChartView->setObjectName("Process chart view");

    // Create combo for selecting sort type
    SortTypeCombo = new QComboBox(RootWidget);
    SortTypeCombo->addItem("None",              static_cast<quint8>(EProcessFilterType::None));
    SortTypeCombo->addItem("State",             static_cast<quint8>(EProcessFilterType::MemoryState));
    SortTypeCombo->addItem("Protection Type",   static_cast<quint8>(EProcessFilterType::MemoryProtectionType));
    SortTypeCombo->addItem("Memory Type",       static_cast<quint8>(EProcessFilterType::MemoryType));
    QObject::connect(SortTypeCombo, &QComboBox::activated, this, &ThisClass::OnSortTypeChanged);
    
}

void QProcessMonitor::OnProcessSelected(const QItemSelection& selected, const QItemSelection& deselected)
{
    if (selected.indexes().isEmpty())
        return;

    // Gather process memory regions
    QModelIndex index = selected.indexes().first();
    QStandardItem* Item = ItemModel->itemFromIndex(index);
    DWORD ProcessID = Item->data(static_cast<int>(EProcessItemDataRole::ERole_ProcessID)).toUInt();

   if(DisplayProcessChart(ProcessID))
       DisplayedProcessID = ProcessID;
}

void QProcessMonitor::UpdateSelectedProccessInfo()
{

}

void QProcessMonitor::OnSortTypeChanged(int ComboItemIndex)
{
    QVariant Data = SortTypeCombo->itemData(ComboItemIndex);
    
    if (Data.isValid())
        AppliedFilter = static_cast<EProcessFilterType>(Data.toUInt());

    // Update active chart
    DisplayProcessChart(DisplayedProcessID);
}

bool QProcessMonitor::DisplayProcessChart(DWORD ProcessID) const
{
    namespace rngs = std::ranges;
    // Clear current chart
    QPieSeries* PieSeries = qobject_cast<QPieSeries*>(PieChartView->chart()->series().at(0));
    if (!PieSeries)
        return false;

    PieSeries->clear();

    std::vector<MEMORY_BASIC_INFORMATION> memoryRegions = FProcessDesc::GatherProcessMemoryRegions(ProcessID);

    // Create a map to accumulate sums for each enum type
    std::unordered_map<DWORD, SIZE_T> FilteredRegionsSums;

    switch (AppliedFilter)
    {
    case EProcessFilterType::None:
        // sort memory regions by allocation base
        rngs::sort(memoryRegions, [](const MEMORY_BASIC_INFORMATION& lhs, const MEMORY_BASIC_INFORMATION& rhs) {
            return lhs.RegionSize > rhs.RegionSize;
            });
        break;
    case EProcessFilterType::MemoryType:
        // count sums by memory type
        for (const auto& reg : memoryRegions)
            FilteredRegionsSums[reg.Type] += reg.RegionSize;
        break;
    case EProcessFilterType::MemoryState:
        // count sums by memory state
        for (const auto& reg : memoryRegions)
            FilteredRegionsSums[reg.State] += reg.RegionSize;
        break;
    case EProcessFilterType::MemoryProtectionType:
        // count sums by memory protection type
        for (const auto& reg : memoryRegions)
            FilteredRegionsSums[reg.Protect] += reg.RegionSize;
        break;
    }

    SIZE_T TotalAllocationSize = std::accumulate(memoryRegions.begin(), memoryRegions.end(), (SIZE_T)0,
        [](SIZE_T sum, const MEMORY_BASIC_INFORMATION& curr) { return sum + curr.RegionSize; });

    if (AppliedFilter == EProcessFilterType::None)
    {
        // display 5 top most regions and keep others together
        const int DisplayCount = 5;

        for (size_t i = 0; (i < DisplayCount) && i < memoryRegions.size(); ++i)
        {
            auto& MemReg = memoryRegions[i];

            QPieSlice* NewSlice = PieSeries->append(QString("Memory region %1").arg(i), (double)MemReg.RegionSize / TotalAllocationSize);
            NewSlice->setLabelVisible(true);
            NewSlice->setLabelArmLengthFactor(0.15f);
            NewSlice->setLabelPosition(QPieSlice::LabelOutside);
        }
    }
    else
    {
        for (auto& RegionSum : FilteredRegionsSums)
        {
            const auto Percent = (double)RegionSum.second / TotalAllocationSize;
            QPieSlice* NewSlice = PieSeries->append(QString("Memory region"), Percent);
            NewSlice->setLabelVisible(true);
            NewSlice->setLabelArmLengthFactor(0.15f);
            NewSlice->setLabelPosition(QPieSlice::LabelOutside);
        }
    }

    return true;
}
