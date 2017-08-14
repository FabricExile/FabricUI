//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_CREATE_PV_TOOL_ACTION__
#define __UI_DFG_CREATE_PV_TOOL_ACTION__

#include <QString>
#include <FabricUI/Actions/BaseAction.h>

namespace FabricUI {
namespace DFG {
 
class DFGCreatePVToolAction : public Actions::BaseAction
{
  Q_OBJECT

	public:
	  DFGCreatePVToolAction(
	    QObject *parent,
	  	QString const& itemPath
	    );

	  virtual ~DFGCreatePVToolAction();
 
	private slots:
	  virtual void onTriggered(
	  	bool checked 
	  	);
	  
  private:
  	QString m_itemPath;
};

class DFGDeletePVToolAction : public Actions::BaseAction
{
  Q_OBJECT

	public:
	  DFGDeletePVToolAction(
	    QObject *parent,
	  	QString const& itemPath
	    );

	  virtual ~DFGDeletePVToolAction();
 
	private slots:
	  virtual void onTriggered(
	  	bool checked 
	  	);
 
 	private:
  	QString m_itemPath;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_CREATE_PV_TOOL_ACTION__
