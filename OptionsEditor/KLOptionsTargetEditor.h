//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_OPTIONS_EDITOR__
#define __UI_RTVAL_OPTIONS_EDITOR__

#include <QDockWidget>
#include <QMainWindow>
#include "BaseRTValOptionsEditor.h"
 
namespace FabricUI {
namespace OptionsEditor {

class KLOptionsTargetEditor 
  : public BaseRTValOptionsEditor
{
  /** 
    KLOptionsTargetEditor specializes BaseRTValOptionsEditor 
    to edit KL OptionsTargets, see Exts/FabricInterfaces/OptionsEditor.
  */
  Q_OBJECT
 
  public:
    /// Constructs a RTValDictModelItem .
    /// \param title Title of the editor.
    KLOptionsTargetEditor(
      QString title = QString()
      );
      
    virtual ~KLOptionsTargetEditor();

    static QDockWidget* create( 
      QString editorID,
      QString title,
      QString groupeName,
      QMainWindow* mainWindow 
      );

    static QDockWidget* create( 
      QString editorID,
      QString title,
      QString groupeName 
      );

    /// \implmentation of BaseRTValOptionsEditor
    virtual BaseRTValModelItem* constructModel(
      const std::string &name,
      const std::string &path,
      BaseRTValOptionsEditor *editor,
      FabricCore::RTVal options
      );

    virtual void refreshKLOptions();

  public slots:  
    /// \implmentation of BaseRTValOptionsEditor
    virtual void resetModel(
      FabricCore::RTVal options=FabricCore::RTVal()
      );
    
    /// \implmentation of BaseRTValOptionsEditor
    virtual void updateModel(
      FabricCore::RTVal options=FabricCore::RTVal()
      );
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_RTVAL_OPTIONS_EDITOR__
