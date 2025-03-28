#include "ProcessFinder.h"
#include "qapplication.h"
#include "qmessagebox.h"
#include "ProcessMonitor.h"

static constexpr size_t defaultNumProcesses = 20;
static constexpr size_t defaultMaxNumProcesses = 999;

int main(int argc, char *argv[]) {

    QApplication App(argc, argv);
    QApplication::processEvents();

    if (FProcessFinder::IsRunningAsAdministrator() != true)
    {
        QMessageBox* MessageBox = new QMessageBox(
            QMessageBox::Warning,
            "Warning!",
            "Make sure app is running as administrator, otherwise it can't list procceses",
            QMessageBox::Close);

        MessageBox->show();
        MessageBox->raise();
        MessageBox->activateWindow();

        QObject::connect(MessageBox, &QMessageBox::close, [&MessageBox]() { 
            if(MessageBox) delete MessageBox; MessageBox = nullptr; 
        });
    }

    // Create main window
    QProcessMonitor MainWindow;
    MainWindow.resize(600, 400);


    // Show the window
    MainWindow.show();
        

    return App.exec();
}

/*
    // Create a main window
    QMainWindow MainWindow;
    MainWindow.resize(600, 400);
    
    // Create Central widget
    std::unique_ptr<QWidget> RootWidget = std::make_unique<QWidget>(nullptr);
    RootWidget->setObjectName("Central Widget");
    MainWindow.setCentralWidget(RootWidget.get());
    
    // Create vertical layout
    std::unique_ptr<QVBoxLayout> VerticalLayout = std::make_unique<QVBoxLayout>(RootWidget.get());
    VerticalLayout->setObjectName("Vertical layout");

    // Create Proccesses list view
    std::unique_ptr<QListView> ProccessList = std::make_unique<QListView>(RootWidget.get());
    ProccessList->setObjectName("List of proccesses");
    
    // Create a QStandardItemModel to hold the list items
    std::unique_ptr<QStandardItemModel> ItemModel = std::make_unique<QStandardItemModel>(ProccessList.get());
    ProccessList->setModel(ItemModel.get());

    // @TODO: Add number input for max proccess count
    std::unique_ptr<QSpinBox> NumberSpinBox = std::make_unique<QSpinBox>(RootWidget.get());
    NumberSpinBox->setObjectName("Number Spin Box");
    NumberSpinBox->setMinimum(0);
    NumberSpinBox->setMaximum(defaultMaxNumProcesses);
    NumberSpinBox->setValue(defaultNumProcesses);
    QObject::connect(NumberSpinBox.get(), &NumberSpinBox->valueChanged, )

    auto ProcessList = FProcessFinder::GetProcessList(NumberSpinBox->value());

    for (size_t i = 0; i < ProcessList.size(); ++i)
    {
        QStandardItem* Item = new QStandardItem(ProcessList[i].ProcessName);
        Item->setEditable(false);

        ItemModel->appendRow(Item);
    }

    MainWindow.setLayout(VerticalLayout.get());
    VerticalLayout->addWidget(ProccessList.get());
    VerticalLayout->addWidget(NumberSpinBox.get());

    // Show the window
    MainWindow.show();

    return App.exec();
*/

/*
{
if (MainWindow->objectName().isEmpty())
MainWindow->setObjectName("MainWindow");
MainWindow->resize(800, 600);

QWidget* MainWidget = new QWidget(MainWindow);
MainWidget->setObjectName("MainWidget");

QGridLayout* gridLayout = new QGridLayout(MainWidget);
gridLayout->setObjectName("gridLayout");

QVBoxLayout* verticalLayout = new QVBoxLayout();
verticalLayout->setObjectName("verticalLayout");

QListView* ProcessList = new QListView(MainWidget);
ProcessList->setObjectName("listView");
QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
sizePolicy.setHorizontalStretch(0);
sizePolicy.setVerticalStretch(0);
sizePolicy.setHeightForWidth(ProcessList->sizePolicy().hasHeightForWidth());
ProcessList->setSizePolicy(sizePolicy);

QVBoxLayout* verticalLayout_3 = new QVBoxLayout();
verticalLayout_3->setObjectName("verticalLayout_3");

verticalLayout->addWidget(ProcessList);
gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);
gridLayout->addLayout(verticalLayout_3, 0, 1, 1, 1);
MainWindow->setCentralWidget(MainWidget);

QMetaObject::connectSlotsByName(MainWindow);
}

// Set window title and icon
MainWindowInstance.setWindowTitle("Simple Qt Window");
MainWindowInstance.setWindowIcon(QIcon(":/path/to/icon.png"));  // Specify the path to your icon

// Create a label and set it as the central widget
QLabel *label = new QLabel("Hello, Qt!", &MainWindowInstance);
label->setAlignment(Qt::AlignCenter);
MainWindowInstance.setCentralWidget(label);

MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "App", nullptr));
*/