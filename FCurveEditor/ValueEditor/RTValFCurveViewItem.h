//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_RTVALFCURVEVIEWITEM_H
#define FABRICUI_VALUEEDITOR_RTVALFCURVEVIEWITEM_H

#include <FabricUI/ValueEditor/BaseViewItem.h>
#include <FTL/Config.h>

namespace FabricUI {

namespace FCurveEditor
{
  class FCurveEditor;
  class RTValAnimXFCurveDFGController;
}

namespace ValueEditor {

class RTValFCurveViewItem : public BaseViewItem
{
  Q_OBJECT

  typedef BaseViewItem Parent;

  FCurveEditor::RTValAnimXFCurveDFGController* m_model;
  class Editor;
  Editor* m_editor;
  class ExpandedDialog;
  ExpandedDialog* m_expandedDialog;

public:
  static BaseViewItem *CreateItem(
    QString const& name,
    QVariant const& value,
    ItemMetadata* metaData
  );
  static const int Priority;

  RTValFCurveViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
  );
  ~RTValFCurveViewItem();

  virtual QWidget* getWidget() FTL_OVERRIDE;
  void onModelValueChanged( QVariant const & ) FTL_OVERRIDE;
  void deleteMe() FTL_OVERRIDE { delete this; }

protected:
  void setBaseModelItem( BaseModelItem* ) FTL_OVERRIDE;

private slots:
  void onViewValueChanged();
  void expand();
  void onMetadataChanged();
  inline void emitInteractionEnd() { emit this->interactionEnd( true ); }
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_RTVALFCURVEVIEWITEM_H
