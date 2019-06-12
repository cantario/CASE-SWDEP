#ifndef CMODELVIEW_H
#define CMODELVIEW_H

#include <QGraphicsView>
#include <QMap>

class MainWindow;
class CDataModel;
class CTableItem;
class CRelationshipItem;
class CObjectItem;
class QMainWindow;

class CModelView : public QGraphicsView
{
    Q_OBJECT

public:
    CModelView(const QString &name, const QString &path, QWidget *parent = nullptr);
    ~CModelView();

    enum cursorToolType {
        POINTER = 0,
        CREATE,
        DELETE,
        ONE_ONE,
        ONE_MANY,
        MANY_MANY,
        AGGREGATE
    };

    void activateTool(const cursorToolType &type);
    void showResizeDialog();
    void showChangeTableDialog(int relationshipId, bool end) const;
    void showObjectEditor(CObjectItem *objectItem);
    void flipTables(int relationshipId);

    CTableItem *tableItem(int id) const;
    CDataModel *dataModel() const;

    QString name() const;
    void setName(const QString &name);
    QString path() const;
    void setPath(const QString &path);
    QMainWindow *pModelWindow() const;
    void setPModelWindow(QMainWindow *pModelWindow);

    QList<CTableItem *> tables() const;
    QList<CRelationshipItem *> relationships() const;

    QGraphicsScene *scene() const;

public slots:
    void changeSize(int w, int h);
    void removeRelationship(int id);
    void changeTable(int relationshipId, int tableId, bool start);

protected:
    // QWidget interface
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QString _name;
    QString _path;
    QMainWindow *_pModelWindow;

    MainWindow *_parent;
    QGraphicsScene *_scene;
    CDataModel *_dataModel;

    int   _width;
    int   _height;    

    QMap<cursorToolType, bool> _tools;
    QMap<int, CTableItem *> _tables;
    QMap<int, CRelationshipItem *> _relationships;
    QList<int> _tablesToRelate;

    void addTable(const QPoint &pos);
    void addRelationship(int startId, int endId);
    void removeItem(const QPoint &pos);
    void removeItems(QList<QGraphicsItem *> items);
    void deactivateTools();
    void showContextMenu(const QPoint &pos, bool isEnabled,  bool colorOption = false, bool relationOption = false);
    void showTableContextMenu(const QPoint &pos, QString tableName);
    void showRelationshipContextMenu(const QPoint &pos);
    void returnToPointer();

private slots:
    void addRelationship();
    void removeItems();
    void showObjectEditor();

};

#endif // CMODELVIEW_H