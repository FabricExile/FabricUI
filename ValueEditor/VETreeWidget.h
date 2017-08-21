//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_VETREEWIDGET_H
#define FABRICUI_VALUEEDITOR_VETREEWIDGET_H

#include <QTreeWidget>
#include <QKeyEvent>

namespace FabricUI {
namespace ValueEditor {

class BaseViewItem;
class BaseModelItem;
class VETreeWidgetItem;

class VETreeWidget : public QTreeWidget
{
  Q_OBJECT

public:

  VETreeWidget();
  ~VETreeWidget();

  void reloadStyles();

  void sortTree();
  bool focusNextPrevChild( bool next );

  VETreeWidgetItem* createTreeWidgetItem( BaseViewItem* viewItem, QTreeWidgetItem* parent, int index = -1 );

  VETreeWidgetItem* findTreeWidget( QWidget* widget ) const;
  VETreeWidgetItem* findTreeWidget( QWidget* widget, VETreeWidgetItem * item ) const;

  VETreeWidgetItem* findTreeWidget( BaseModelItem* pItem ) const;
  VETreeWidgetItem* findTreeWidget( BaseModelItem* pItem, VETreeWidgetItem* pWidget ) const;
  VETreeWidgetItem * findTreeWidget( BaseViewItem * pItem ) const;
  VETreeWidgetItem * findTreeWidget( BaseViewItem * pItem, VETreeWidgetItem * pWidget ) const;

public slots:

  void onSetModelItem( FabricUI::ValueEditor::BaseModelItem* pItem );

  void onModelItemChildInserted( FabricUI::ValueEditor::BaseModelItem* parent, int index, const char* name );
  void onModelItemRemoved( FabricUI::ValueEditor::BaseModelItem* item );
  void onModelItemRenamed( FabricUI::ValueEditor::BaseModelItem* item );
  void onModelItemTypeChanged( FabricUI::ValueEditor::BaseModelItem* item, const char* newType );
  void onModelItemChildrenReordered( FabricUI::ValueEditor::BaseModelItem* parent, const QList<int>& newOrder );

  void onViewItemChildrenRebuild( FabricUI::ValueEditor::BaseViewItem* item );

  // This slot is triggered when an item is edited in the view
  void onItemEdited( QTreeWidgetItem* item, int column );

  virtual void keyPressEvent(QKeyEvent *event) /*override*/;

  virtual void mouseMoveEvent(QMouseEvent *event) /*override*/;

  void emitToggleManipulation(bool toggle);

protected slots:

  void onTreeWidgetItemExpanded( QTreeWidgetItem *_treeWidgetItem );

  void onTreeWidgetItemCollapsed( QTreeWidgetItem *_treeWidgetItem );

  void prepareMenu( const QPoint& pt );

  void resetItem();
  
  void emitRefreshViewport();

signals:
  // Refreshes the viewport, if a klWidget
  // has been activated-deactivated.
  void refreshViewport();

  void toggleManipulation(bool);

  /// Emmitted when the item overed changed.
  /// Enabled is QWidget::hasMouseTracking if true (QWidget::setMouseTracking)
  void itemOveredChanged(
    QTreeWidgetItem *oldItem,
    QTreeWidgetItem *newItem
    );

protected:
  void setViewItemConnections(BaseViewItem* item);
  
  bool m_manipulationToggled;

  QTreeWidgetItem *m_currentOveredItem;
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_VETREEWIDGET_H
