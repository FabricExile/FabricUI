//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_CREATE_PV_TOOL_ACTION__
#define __UI_DFG_CREATE_PV_TOOL_ACTION__

#include <QMenu>
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
	    QString const& name,
	    QString const& text,
	  	QString const& itemPath
	    );

	  virtual ~DFGCreatePVToolAction();
 
	private slots:
	  virtual void onTriggered();
	  
  private:
  	QString m_itemPath;
};

class DFGDeletePVToolAction : public Actions::BaseAction
{
  Q_OBJECT

	public:
	  DFGDeletePVToolAction(
	    QObject *parent,
	    QString const& name,
	    QString const& text,
	  	QString const& itemPath
	    );

	  virtual ~DFGDeletePVToolAction();
 
	private slots:
	  virtual void onTriggered();
 
 	private:
  	QString m_itemPath;
};

class DFGDeleteAllPVToolsAction : public Actions::BaseAction
{
  Q_OBJECT

	public:
	  DFGDeleteAllPVToolsAction(
	    QObject *parent,
	    QString const& name,
	    QString const& text
	    );

	  virtual ~DFGDeleteAllPVToolsAction();
 
	private slots:
	  virtual void onTriggered();
};

class DFGDeleteAllAndCreatePVToolAction : public Actions::BaseAction
{
  Q_OBJECT

	public:
	  DFGDeleteAllAndCreatePVToolAction(
	    QObject *parent,
	    QString const& name,
	    QString const& text,
	  	QString const& itemPath
	    );

	  virtual ~DFGDeleteAllAndCreatePVToolAction();
 
	private slots:
	  virtual void onTriggered();
 
 	private:
  	QString m_itemPath;
};

class DFGPVToolMenu : public QMenu
{
	Q_OBJECT

	public:
		DFGPVToolMenu(
			QWidget *parent,
			QString const& itemPath
			);

		~DFGPVToolMenu();

		static QMenu* createMenu(
			QWidget *parent,
			QString const& itemPath
			);

		static QList<QAction*> createActions(
			QWidget *parent,
  		QString const& itemPath
			);

		static bool canCreate(
			QString const& itemPath
			);

		protected slots:
			virtual void onConstructMenu();

	private:
		QString m_itemPath;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_CREATE_PV_TOOL_ACTION__
