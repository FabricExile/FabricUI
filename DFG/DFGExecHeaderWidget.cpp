// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGExecHeaderWidget.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/Actions/ActionRegistry.h>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QProxyStyle>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGExecHeaderWidget::DFGExecHeaderWidget(
  QWidget * parent,
  DFGController *dfgController,
  const GraphView::GraphConfig &config
  )
  : QFrame( parent )
  , m_dfgController( dfgController )
{
  setObjectName( "DFGExecHeaderWidget" );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  setContentsMargins(0, 0, 0, 0);

  QHBoxLayout * layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  m_backgroundColor = config.headerBackgroundColor;
  m_pen = config.headerPen;

  m_execPathLabel = new QLabel;
  m_execPathLabel->setObjectName( "DFGExecPathLabel" );

  m_presetNameLabel = new QLabel;
  m_presetNameLabel->setObjectName( "DFGPresetNameLabel" );

  m_presetPathSep = new QLabel;
  m_presetPathSep->setObjectName( "DFGPresetPathSep" );

  m_reloadButton = new QPushButton( "Reload" );
  m_reloadButton->setObjectName( "DFGReloadButton" );
  m_reloadButton->setFocusPolicy( Qt::NoFocus );
  m_reloadButton->setAutoFillBackground(false);
  connect(
    m_reloadButton, SIGNAL(clicked()),
    this, SIGNAL(reloadPressed())
    );

  m_saveButton = new QPushButton( "Apply Code Changes" );
  m_saveButton->setObjectName( "DFGSaveButton" );
  m_saveButton->setFocusPolicy( Qt::NoFocus );
  m_saveButton->setAutoFillBackground(false);
  m_saveButton->setToolTip("Applies the changes made to the code\nand then re-compiles and executes the graph.");
  connect(
    m_saveButton, SIGNAL(clicked()),
    this, SIGNAL(savePressed())
    );

  m_reqExtLabel = new QLabel;
  m_reqExtLabel->setObjectName( "DFGRequiredExtensionsLabel" );
  m_reqExtLineEdit = new ReqExtLineEdit; // [FE-7883] [FE-4882]
  m_reqExtLineEdit->setObjectName( "DFGRequiredExtensionsLineEdit" );

  m_disableGraphButton = new QToolButton();
  m_disableGraphButton->setObjectName( "DFGDisableGraphButton" );
  m_disableGraphButton->setFocusPolicy( Qt::NoFocus );
  m_disableGraphButton->setAutoFillBackground(false);
  m_disableGraphButton->setCheckable( true );
  m_disableGraphButton->setText( QString::fromUtf8( "\xEF\x81\x8C" ) );
  m_disableGraphButton->setToolTip("Disables graph compilations.");

  QObject::connect(
    m_reqExtLineEdit, SIGNAL(editingFinished()),
    this, SLOT(reqExtEditingFinished())
    );
  QObject::connect(
    m_reqExtLineEdit, SIGNAL(textChanged(QString)),
    this, SLOT(reqExtResizeToContent())
    );
  reqExtResizeToContent();

  QIcon backIcon = LoadPixmap("DFGBack.png");
  m_backButton = new QPushButton(backIcon, "Back");
  m_backButton->setObjectName("DFGBackButton");
  m_backButton->setFocusPolicy(Qt::NoFocus);
  m_backButton->setAutoFillBackground(false);
  m_backButton->setToolTip("Leaves the function editor and goes back to the graph view.");
  QObject::connect(
    m_backButton, SIGNAL(clicked()),
    m_reqExtLineEdit, SLOT(onGoUpPressed())
  );
  QObject::connect(
    m_backButton, SIGNAL(clicked()),
    this, SIGNAL(goUpPressed())
  );

  layout->addWidget( m_presetNameLabel );
  layout->addWidget( m_presetPathSep );
  layout->addWidget( m_execPathLabel );
  layout->addStretch( 1 );  
  layout->addWidget( m_backButton );
  layout->addWidget( m_reloadButton );
  layout->addWidget( m_saveButton );
  layout->addWidget( m_reqExtLabel );
  layout->addWidget( m_reqExtLineEdit );
  layout->addWidget( m_disableGraphButton );

  QFrame *regWidget = new QFrame;
  regWidget->setObjectName( "DFGRegWidget" );
  regWidget->setLayout( layout );

  QPushButton *presetSplitButton = new QPushButton( "Split from Preset" );
  presetSplitButton->setObjectName( "DFGPresetSplitButton" );
  presetSplitButton->setSizePolicy(
    QSizePolicy::Minimum, QSizePolicy::Minimum
    );
  connect(
    presetSplitButton, SIGNAL(clicked()),
    this, SLOT(onSplitFromPresetClicked())
    );

  QLabel *presetSplitLabel = new QLabel;
  presetSplitLabel->setObjectName( "DFGPresetSplitLabel" );
  presetSplitLabel->setSizePolicy(
    QSizePolicy::Expanding, QSizePolicy::Minimum
    );

  QHBoxLayout *presetSplitLayout = new QHBoxLayout;
  presetSplitLayout->setContentsMargins( 0, 0, 0, 0 );
  presetSplitLayout->addWidget( presetSplitLabel );
  presetSplitLayout->addWidget( presetSplitButton );

  m_presetSplitWidget = new QFrame;
  m_presetSplitWidget->setObjectName( "DFGPresetSplitWidget" );
  m_presetSplitWidget->setLayout( presetSplitLayout );

  QVBoxLayout *vLayout = new QVBoxLayout;
  vLayout->setContentsMargins( 0, 0, 0, 0 );
  vLayout->setSpacing( 0 );
  vLayout->addWidget( m_presetSplitWidget );
  vLayout->addWidget( regWidget );
  setLayout( vLayout );

  QObject::connect(
    m_dfgController, SIGNAL(execChanged()),
    this, SLOT(onExecChanged())
    );
  QObject::connect(
    m_dfgController, SIGNAL(execSplitChanged()),
    this, SLOT(onExecChanged())
    );
  onExecChanged();
}

void DFGExecHeaderWidget::createMenu(QMenu *menu)
{
  QAction * blockCompilationsAction = new BlockCompilationsAction(this->parent(), menu);
  blockCompilationsAction->setCheckable(true);
  blockCompilationsAction->setChecked(false);
  blockCompilationsAction->setShortcutContext(Qt::WindowShortcut);

  this->m_disableGraphButton->setDefaultAction( blockCompilationsAction );
}


DFGExecHeaderWidget::~DFGExecHeaderWidget()
{
}

ReqExtLineEdit::ReqExtLineEdit(QWidget *parent)
: FELineEdit( parent )
, m_allowEdits( false)
{
  init();
}

void ReqExtLineEdit::setAllowEdits(bool allow)
{
  m_allowEdits = allow;
}

void ReqExtLineEdit::onEditingFinished()
{
  setEnabled(false);
}

bool ReqExtLineEdit::eventFilter(QObject * watched, QEvent * event)
{
  if (event->type() == QEvent::MouseButtonDblClick)
  {
    if (m_allowEdits)
    {
      setEnabled(true);
      setFocus();
    }
    selectAll();
    return true;
  }
  return QObject::eventFilter(watched, event);
}

void ReqExtLineEdit::init()
{
  setEnabled(false);
  installEventFilter(this);

  QObject::connect(
    this, SIGNAL(editingFinished()),
    this, SLOT(onEditingFinished())
    );
}

void ReqExtLineEdit::onGoUpPressed()
{
  clearFocus();
}

void DFGExecHeaderWidget::refresh()
{
  FTL::CStrRef execPath = getExecPath();
  FabricCore::DFGExec &exec = getExec();
  FTL::StrRef execBlockName = m_dfgController->getExecBlockName();
  if ( exec )
  {
    bool isRoot = execPath.empty();
    bool isPreset = exec.isPreset();
    bool wouldSplitFromPreset = exec.editWouldSplitFromPreset();

    m_presetSplitWidget->setVisible( wouldSplitFromPreset );

    m_backButton->setVisible( !isRoot );

    m_execPathLabel->setVisible( !isRoot );
    if ( !isRoot )
    {
      QString pathLabelText( "Path: " );
      pathLabelText += QString::fromUtf8( execPath.data(), execPath.size() );
      if ( !execBlockName.empty() )
      {
        if ( !execPath.empty() )
          pathLabelText += '.';
        pathLabelText += QString::fromUtf8( execBlockName.data(), execBlockName.size() );
      }
      m_execPathLabel->setText( pathLabelText );
    }

    m_presetNameLabel->setVisible( isPreset );
    if ( isPreset )
    {
      FTL::CStrRef title = exec.getTitle();
      QString presetNameText( "Preset Name: ");
      presetNameText += QString::fromUtf8( title.data(), title.size() );
      m_presetNameLabel->setText( presetNameText );
    }
    m_presetPathSep->setVisible( isPreset );

    bool showFuncButtons = execBlockName.empty()
      && exec.getType() == FabricCore::DFGExecType_Func;
    m_reloadButton->setVisible( showFuncButtons );
    m_saveButton->setVisible( showFuncButtons );
    m_reloadButton->setEnabled( !isPreset );
    m_saveButton->setEnabled( !isPreset );

    FabricCore::String extDepsDesc = exec.getExtDeps();
    FTL::CStrRef extDepsDescCStr =
      extDepsDesc.getCStr()? extDepsDesc.getCStr() : "";
    m_reqExtLabel->setVisible( execBlockName.empty() );
    m_reqExtLabel->setText( "Required Extensions:" );
    m_reqExtLineEdit->setVisible( execBlockName.empty() );
    m_reqExtLineEdit->setAllowEdits( !wouldSplitFromPreset );
    m_reqExtLineEdit->setText( extDepsDescCStr.c_str() );

    update();
  }
}

void DFGExecHeaderWidget::refreshExtDeps( FTL::CStrRef extDeps )
{
  refresh();
}

void DFGExecHeaderWidget::reqExtEditingFinished()
{
  std::string extDepDesc = m_reqExtLineEdit->text().toUtf8().constData();  
  FabricCore::String currentExtDepDesc = getExec().getExtDeps();
  char const *currentExtDepDescCStr = currentExtDepDesc.getCStr();
  if ( !currentExtDepDescCStr )
    currentExtDepDescCStr = "";
  if ( extDepDesc == currentExtDepDescCStr )
    return;

  FTL::StrRef::Split split = FTL::StrRef( extDepDesc ).split(',');
  QStringList nameAndVers;
  while ( !split.first.empty() )
  {
    FTL::StrRef trimmed = split.first.trim();
    nameAndVers.append(
      QString::fromUtf8( trimmed.data(), trimmed.size() )
      );
    split = split.second.split(',');
  }

  m_dfgController->cmdSetExtDeps( nameAndVers );

  // FE-7961
  // Emit the signal to refresh DFGRegisteredTypeLineEdit
  emit extensionLoaded();
}

void DFGExecHeaderWidget::reqExtResizeToContent()
{
  QFontMetrics fontMetrics(m_reqExtLineEdit->font());

  QString textMin = " InlineDrawing:* ";
  QString textMax = " Math:*,Util:*,Singleton:*,Geometry:*,InlineDrawing:* ";

  int minPixels = fontMetrics.width(textMin);
  int maxPixels = fontMetrics.width(textMax);
  int txtPixels = fontMetrics.width(" " + m_reqExtLineEdit->text() + " ");

  m_reqExtLineEdit->setFixedWidth(std::max(minPixels, std::min(maxPixels, txtPixels)));

  reqExtSetToolTipFromContent();
}

void DFGExecHeaderWidget::reqExtSetToolTipFromContent()
{
  QString text = m_reqExtLineEdit->text();
  text.replace(',', '\n');
  m_reqExtLineEdit->setToolTip(text);
}

void DFGExecHeaderWidget::onExecChanged()
{
  refresh();
}

void DFGExecHeaderWidget::paintEvent(QPaintEvent * event)
{
  QRect rect = contentsRect();
  QPainter painter(this);

  painter.fillRect(rect, m_backgroundColor);

  painter.setPen(m_pen);
  painter.drawLine(rect.bottomLeft(), rect.bottomRight());

  QWidget::paintEvent(event);
}

FTL::CStrRef DFGExecHeaderWidget::getExecPath()
{
  return m_dfgController->getExecPath();
}

FabricCore::DFGExec &DFGExecHeaderWidget::getExec()
{
  return m_dfgController->getExec();
}

void DFGExecHeaderWidget::onSplitFromPresetClicked()
{
  m_dfgController->cmdSplitFromPreset();
}
