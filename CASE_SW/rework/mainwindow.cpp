#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    _countCreatedModels = 0;

    _editActions.insert(CModelView::POINTER, QSharedPointer<QAction>(ui->actionPointer));
    _editActions.insert(CModelView::CREATE, QSharedPointer<QAction>(ui->actionCreate_Table));
    _editActions.insert(CModelView::DELETE, QSharedPointer<QAction>(ui->actionDelete));
    _editActions.insert(CModelView::ONE_ONE, QSharedPointer<QAction>(ui->actionOne_One));
    _editActions.insert(CModelView::ONE_MANY, QSharedPointer<QAction>(ui->actionOne_Many));
    _editActions.insert(CModelView::MANY_MANY, QSharedPointer<QAction>(ui->actionMany_Many));
    _editActions.insert(CModelView::AGGREGATE, QSharedPointer<QAction>(ui->actionAggregate));

    connect(ui->actionPointer, &QAction::triggered,
            this, [this]{activateEditAction(CModelView::POINTER);});
    connect(ui->actionCreate_Table, &QAction::triggered,
            this, [this]{activateEditAction(CModelView::CREATE);});
    connect(ui->actionDelete, &QAction::triggered,
            this, [this]{activateEditAction(CModelView::DELETE);});
    connect(ui->actionOne_One, &QAction::triggered,
            this, [this]{activateEditAction(CModelView::ONE_ONE);});
    connect(ui->actionOne_Many, &QAction::triggered,
            this, [this]{activateEditAction(CModelView::ONE_MANY);});
    connect(ui->actionMany_Many, &QAction::triggered,
            this, [this]{activateEditAction(CModelView::MANY_MANY);});
    connect(ui->actionAggregate, &QAction::triggered,
            this, [this]{activateEditAction(CModelView::AGGREGATE);});

    activateEditAction(CModelView::POINTER);
    this->setWindowTitle("CASE-SWDEP");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    QString modelName = "New " + QString::number(++_countCreatedModels);

    if(!addModelTab(modelName, ""))
        qDebug() << "ERROR: MainWIndow::addModelTab";
}

void MainWindow::on_actionOpen_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(
                this,
                QObject::tr("Open Model"),
                QString(),
                QObject::tr("Model Files (*.cdmod);;All Files (*.*)")
    );
    if(filePath.isEmpty())
        return;

    // no need for file extension in model name
    // because only one extention is possible
    QFileInfo fileInfo(filePath);
    addModelTab(fileInfo.fileName().split(".")[0], filePath);
}

void MainWindow::on_actionSave_triggered()
{
    QString modelName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    QString modelPath = _workspaceModels.value(modelName)->getPath();

    // can't save model if no path specified
    if(modelPath.isEmpty())
    {
        on_actionSave_as_triggered();
    }
    else
    {
//        CViewModel* modelWidget = (CViewModel*)_workspaceModels.value(modelName)->getView();
//        modelWidget->saveInFile(modelPath);
    }
}

void MainWindow::on_actionSave_as_triggered()
{
    QString newModelPath = QFileDialog::getSaveFileName(
        this,
        QObject::tr("Save as CDMOD"),
        QString(),
        QObject::tr("Model Files (*.cdmod)")
    );
    if (newModelPath.isEmpty()) {
        return;
    }

    QFileInfo newFileInfo(newModelPath);
    QString newModelName = newFileInfo.fileName().split(".")[0];

    QString oldModelName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());

//    CViewModel* modelWidget = (CViewModel*)_workspaceModels.value(oldModelName)->getView();
//    modelWidget->saveInFile(newModelPath);

    ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
    addModelTab(newModelName, newModelPath);
}

void MainWindow::on_actionClose_triggered()
{
    closeTab(ui->tabWidget->currentIndex());
}

void MainWindow::on_actionQuit_triggered()
{
    for(int i = 0; i < ui->tabWidget->count(); i++)
        closeTab(i);

    close();
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    closeTab(index);
}

void MainWindow::activateEditAction(CModelView::cursorToolType type)
{
    deactivateEditActions();
    _editActions.value(type)->setChecked(true);
}

bool MainWindow::addModelTab(QString modelName, QString modelPath)
{
    CModelView *modelView = new CModelView();
//    if(!modelPath.isEmpty())
//        modelView->loadFromFile(modelPath); // add return statement to load
    modelView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QScrollArea* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setWidget(modelView);
    modelView->changeSize(1280, 720);

    QSharedPointer<ModelInfo> model =
            QSharedPointer<ModelInfo>(new ModelInfo(modelName, modelPath, modelView));

    _workspaceModels.insert(modelName, model);

    ui->tabWidget->addTab(scroll, modelName);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);

    // trigger currently selected action
    /* For user it seems that this action was allegedly triggered before any tab was open.
     * Otherwice, user will have to trigger this action again.
     */
    for(int i = 0; i < _editActions.size(); i++)
    {
        if(_editActions.value((CModelView::cursorToolType)i)->isChecked())
        {
            _editActions.value((CModelView::cursorToolType)i)->trigger();
            break;
        }
    }

    return true;
}

void MainWindow::closeTab(int index)
{
    QString modelName = ui->tabWidget->tabText(index);

    ui->tabWidget->removeTab(index);

    _workspaceModels.remove(modelName);
}

void MainWindow::deactivateEditActions()
{
    for(int i = 0; i < _editActions.size(); i++)
        _editActions.value((CModelView::cursorToolType)i)->setChecked(false);
}

MainWindow::ModelInfo::ModelInfo(QString name, QString path, CModelView *widget)
{
    setName(name);
    setPath(path);
    setView(widget);
}

MainWindow::ModelInfo::~ModelInfo()
{
    if(view != NULL)
        delete view;
}

void MainWindow::ModelInfo::setName(QString name)
{
    this->name = name;
}

void MainWindow::ModelInfo::setPath(QString path)
{
    this->path = path;
}

void MainWindow::ModelInfo::setView(CModelView *widget)
{
    this->view = widget;
}

QString MainWindow::ModelInfo::getName()
{
    return name;
}

QString MainWindow::ModelInfo::getPath()
{
    return path;
}

CModelView *MainWindow::ModelInfo::getView()
{
    return view;
}

void MainWindow::on_actionPointer_triggered()
{
    QString modelName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    if(_workspaceModels.find(modelName) == _workspaceModels.end())
        return;
    _workspaceModels.value(modelName)->getView()->activateTool(CModelView::POINTER);
}

void MainWindow::on_actionCreate_Table_triggered()
{
    QString modelName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    if(_workspaceModels.find(modelName) == _workspaceModels.end())
        return;
    _workspaceModels.value(modelName)->getView()->activateTool(CModelView::CREATE);
}

void MainWindow::on_actionDelete_triggered()
{
    QString modelName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    if(_workspaceModels.find(modelName) == _workspaceModels.end())
        return;
    _workspaceModels.value(modelName)->getView()->activateTool(CModelView::DELETE);
}

void MainWindow::on_actionOne_One_triggered()
{
    QString modelName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    if(_workspaceModels.find(modelName) == _workspaceModels.end())
        return;
    _workspaceModels.value(modelName)->getView()->activateTool(CModelView::ONE_ONE);
}

void MainWindow::on_actionOne_Many_triggered()
{
    QString modelName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    if(_workspaceModels.find(modelName) == _workspaceModels.end())
        return;
    _workspaceModels.value(modelName)->getView()->activateTool(CModelView::ONE_MANY);
}

void MainWindow::on_actionMany_Many_triggered()
{
    QString modelName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    if(_workspaceModels.find(modelName) == _workspaceModels.end())
        return;
    _workspaceModels.value(modelName)->getView()->activateTool(CModelView::MANY_MANY);
}

void MainWindow::on_actionAggregate_triggered()
{
    QString modelName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    if(_workspaceModels.find(modelName) == _workspaceModels.end())
        return;
    _workspaceModels.value(modelName)->getView()->activateTool(CModelView::AGGREGATE);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    QString modelName = ui->tabWidget->tabText(index);
    this->setWindowTitle((modelName.isNull()) ? "CASE-SWDEP" :  modelName + " - CASE-SWDEP");
}

void MainWindow::on_actionTo_PDM_triggered()
{
    QString modelName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    if(_workspaceModels.find(modelName) == _workspaceModels.end())
        return;

}

void MainWindow::on_actionScript_triggered()
{
    QString modelName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    if(_workspaceModels.find(modelName) == _workspaceModels.end())
        return;
}

void MainWindow::on_actionChange_Size_triggered()
{
    QString modelName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    if(_workspaceModels.find(modelName) == _workspaceModels.end())
        return;
    _workspaceModels.value(modelName)->getView()->showResizeDialog();
}