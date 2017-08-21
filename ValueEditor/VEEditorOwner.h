//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_VEEDITOROWNER_H
#define FABRICUI_VALUEEDITOR_VEEDITOROWNER_H

#include <FabricCore.h>
#include <FTL/CStrRef.h>
#include <FTL/ArrayRef.h>
#include <QWidget>
#include <QMetaType>
#include <QSharedPointer>


namespace FTL {
  class JSONArray;
}

namespace FabricUI {

  namespace DFG {
    class DFGController;
    class DFGWidget;
  }
  namespace GraphView {
    class Node;
    class Graph;
  }
  namespace ModelItems {
    class BindingModelItem;
    class ItemModelItem;
    class RootModelItem;
  }

  namespace ValueEditor 
  {     
    class BaseModelItem;
    class VETreeWidget;

    class VEEditorOwner : public QObject 
    {
      Q_OBJECT

    public:

      VEEditorOwner();
      ~VEEditorOwner();

      virtual QWidget* getWidget() const;
      virtual void initConnections();

    public slots :
    
      virtual void onOutputsChanged() {} // Call after each evaluation
      void reloadStyles();
      void emitReplaceModelRoot(FabricUI::ValueEditor::BaseModelItem* model);
      void emitToggleManipulation(bool toggle);

    protected slots:
      virtual void onModelItemInserted( FabricUI::ValueEditor::BaseModelItem* parent, int index, const char* childName );
      virtual void onModelValueChanged( QVariant const &newValue );
      void emitRefreshViewport();

    signals:
      void log(const char * message);
      void replaceModelRoot( FabricUI::ValueEditor::BaseModelItem* model );
      void modelItemValueChanged( FabricUI::ValueEditor::BaseModelItem * item, QVariant const &newValue );
      void modelItemInserted( FabricUI::ValueEditor::BaseModelItem* parent, int index, const char* childName );
      void modelItemTypeChange( FabricUI::ValueEditor::BaseModelItem* changingItem, const char* newType );
      void modelItemRemoved( FabricUI::ValueEditor::BaseModelItem* removedItem);
      void modelItemChildrenReordered( FabricUI::ValueEditor::BaseModelItem* parent, const QList<int>& newOrder );
      void modelItemRenamed( FabricUI::ValueEditor::BaseModelItem* renamedItem );
      // Refreshes the viewport, if a klWidget
      // has been activated-deactivated.
      void refreshViewport();
      void toggleManipulation(bool);

    protected:

      VETreeWidget* m_valueEditor;
      FabricUI::ModelItems::RootModelItem* m_modelRoot;
    };
  }
}

#endif // FABRICUI_VALUEEDITOR_VEEDITOROWNER_H
