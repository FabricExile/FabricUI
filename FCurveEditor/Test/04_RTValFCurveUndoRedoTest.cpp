
#include <FabricUI/FCurveEditor/Test/Test.h>
#include <FabricUI/FCurveEditor/FCurveEditor.h>
#include <FabricUI/FCurveEditor/Models/AnimXKL/UndoRedo/RTValAnimXFCurveDFGController.h>

#include <FabricUI/Application/FabricApplicationStates.h>
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/Commands/KLCommandRegistry.h>
#include <FabricUI/DFG/Commands/DFGPathValueResolver.h>
#include <FabricUI/Commands/PathValueResolverRegistry.h>
#include <FabricUI/Util/QtSignalsSlots.h>

#include <FabricCore.h>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QTextEdit>

#include <iostream>
#include <assert.h>

using namespace FabricUI::FCurveEditor;

void ReportCallBack(
  void *userdata,
  FEC_ReportSource source,
  FEC_ReportLevel level,
  char const *data,
  uint32_t size
)
{
  std::cout << std::string( data, size ).c_str() << std::endl;
}

inline float RandFloat() { return float( rand() ) / RAND_MAX; }

void FormatEditor( FCurveEditor* editor )
{
  editor->resize( 800, 600 );
  editor->show();
  editor->setStyleSheet( LoadQSS() );
}

FabricCore::DFGBinding binding;
FabricCore::DFGExec exec;
RTValAnimXFCurveDFGController* s_model = NULL;
QTextEdit* stackView = NULL;

void NotifCallback(
  void *userdata,
  char const *jsonCString,
  uint32_t jsonLength
)
{
  if( s_model != NULL )
  {
    FabricCore::RTVal val = binding.getArgValue( "curve" );
    s_model->setValue( val );
    {
      FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
      stackView->setText( manager->getContent( false ) );
    }
  }
}

class UndoAction : public AbstractAction
{
public:
  UndoAction( QObject* parent ) : AbstractAction( parent ) {}

protected:
  void onTriggered()
  {
    std::cout << "Undo" << std::endl;
    FabricUI::Commands::CommandManager::getCommandManager()->undoCommand();
    FabricCore::RTVal val = binding.getArgValue( "curve" );
    s_model->setValue( val );
    s_model->update();
  }
};

class RedoAction : public AbstractAction
{
public:
  RedoAction( QObject* parent ) : AbstractAction( parent ) {}

protected:
  void onTriggered()
  {
    std::cout << "Undo" << std::endl;
    FabricUI::Commands::CommandManager::getCommandManager()->redoCommand();
    FabricCore::RTVal val = binding.getArgValue( "curve" );
    s_model->setValue( val );
    s_model->update();
  }
};


int main()
{
  int argc = 0;
  QApplication app( argc, NULL );

  stackView = new QTextEdit();
  stackView->show();

  FabricCore::Client::CreateOptions createOptions = {};
  createOptions.guarded = true;
  FabricCore::Client client( &ReportCallBack, 0, &createOptions );
  new FabricUI::Application::FabricApplicationStates( client );

  client.loadExtension( "AnimX", "", false );
  binding = client.getDFGHost().createBindingToNewGraph();
  binding.registerNotificationCallback( NotifCallback, NULL );
  exec = binding.getExec();
  exec.addExecPort( "curve", FEC_DFGPortType_In, "AnimX::AnimCurve" );

  new FabricUI::Commands::KLCommandManager();
  ( new FabricUI::Commands::KLCommandRegistry() )->synchronizeKL();
  FabricUI::DFG::DFGPathValueResolver* resolver = new FabricUI::DFG::DFGPathValueResolver();
  FabricUI::Commands::PathValueResolverRegistry::getRegistry()->registerResolver( resolver, "DFGPathValueResolver" );
  resolver->onBindingChanged( binding );

  RTValAnimXFCurveDFGController model;
  s_model = &model;
  model.setPath( QString::number( binding.getBindingID() ).toUtf8().data(), "curve" );

  FCurveEditor* editor = new FCurveEditor();

  QOBJECT_CONNECT(
    editor, SIGNAL, FabricUI::FCurveEditor::FCurveEditor, interactionBegin, ( ),
    &model, SLOT, RTValAnimXFCurveDFGController, onInteractionBegin, ( )
  );
  QOBJECT_CONNECT(
    editor, SIGNAL, FabricUI::FCurveEditor::FCurveEditor, interactionEnd, ( ),
    &model, SLOT, RTValAnimXFCurveDFGController, onInteractionEnd, ( )
  );

  editor->setModel( &model );
  FormatEditor( editor );
  UndoAction* undoAction = new UndoAction( editor );
  undoAction->setShortcut( QKeySequence::Undo );
  editor->addAction( undoAction );
  RedoAction* redoAction = new RedoAction( editor );
  redoAction->setShortcut( QKeySequence::Redo );
  editor->addAction( redoAction );

  app.exec();
  return 0;
}
