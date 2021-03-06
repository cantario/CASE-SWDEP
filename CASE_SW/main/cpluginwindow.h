#ifndef CPLUGINWINDOW_H
#define CPLUGINWINDOW_H

#include <QMainWindow>
#include <QMap>

namespace Ui {
class CPluginWindow;
}

class CModelView;
class PModelView;
class CDataModel;
class CPluginInterface;

class CPluginWindow : public QMainWindow
{
    Q_OBJECT

public:
    CPluginWindow(CModelView *model, QWidget *parent = 0);
    CPluginWindow(PModelView *model, QWidget *parent = 0);
    ~CPluginWindow();

private slots:
    void on_pushRefresh_clicked();
    void on_pushGenerate_clicked();

    void on_actionExport_as_triggered();

private:
    Ui::CPluginWindow *ui;

    CDataModel *_dataModel;
    QMap<QString, QString> _plugins;

    void loadPlugins();
};

#endif // CPLUGINWINDOW_H
