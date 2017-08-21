// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <QLayout>
#include <QTimer>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include "DFGEditPortDialog.h"
#include <FabricUI/Util/StringUtil.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGEditPortDialog::DFGEditPortDialog(
  QWidget * parent, 
  FabricCore::Client & client, 
  bool showPortType, 
  bool topGraphPort, 
  const DFGConfig & dfgConfig,
  bool setAlphaNum)
: DFGBaseDialog(parent, true, dfgConfig)
{
  setWindowTitle("Edit Port");
  setObjectName( "DFGEditPortDialog" );

  if(showPortType)
  {
    m_portTypeCombo = new QComboBox(this);
    m_portTypeCombo->addItem("In");
    m_portTypeCombo->addItem("Out");
    m_portTypeCombo->addItem("IO");
  }
  else
    m_portTypeCombo = NULL;
  m_titleEdit = new QLineEdit("", this);

  if( topGraphPort ) {
    m_dataTypeEdit = new DFGRegisteredTypeLineEdit( this, client, "" );
    m_extensionEdit = new DFGExtensionLineEdit( this, client );
  } else {
    m_dataTypeEdit = NULL;
    m_extensionEdit = NULL;
  }

  m_visibilityCombo = new QComboBox(this);
  m_visibilityCombo->addItem("normal");
  m_visibilityCombo->addItem("opaque");
  m_visibilityCombo->addItem("hidden");
  m_persistValue = new QCheckBox(this);
  m_hasSoftRange = new QCheckBox(this);
  m_softRangeMin = new QLineEdit("0.0", this);
  m_softRangeMax = new QLineEdit("1.0", this);
  m_hasHardRange = new QCheckBox(this);
  m_hardRangeMin = new QLineEdit("0.0", this);
  m_hardRangeMax = new QLineEdit("1.0", this);
  m_hasCombo = new QCheckBox(this);
  m_combo = new QLineEdit("OptionA, OptionB", this);
  m_hasFileTypeFilter = new QCheckBox(this);
  m_fileTypeFilter = new QLineEdit("*.*", this);
  m_isOpenFile = new QCheckBox(this);
  m_isOpenFile->setCheckState(Qt::Checked);

  m_softRangeMin->setEnabled(false);
  m_softRangeMax->setEnabled(false);
  m_hardRangeMin->setEnabled(false);
  m_hardRangeMax->setEnabled(false);
  m_combo->setEnabled(false);
  m_softRangeMin->setValidator(Util::newDoubleValidator(m_softRangeMin));
  m_softRangeMax->setValidator(Util::newDoubleValidator(m_softRangeMax));
  m_hardRangeMin->setValidator(Util::newDoubleValidator(m_hardRangeMin));
  m_hardRangeMax->setValidator(Util::newDoubleValidator(m_hardRangeMax));

  if(m_portTypeCombo)
    addInput(m_portTypeCombo, "Type", "Required");
  addInput(m_titleEdit, "Title", "Required");
  if( topGraphPort ) {
    addInput( m_dataTypeEdit, "Data Type", "Required" );
    addInput( m_extensionEdit, "Extension", "Advanced" );
  }
  addInput(m_visibilityCombo, "Visibility", "Metadata");
  addInput(m_persistValue, "Persist Value", "Metadata");
  addInput(m_hasSoftRange, "Soft Range", "Metadata");
  addInput(m_softRangeMin, "Soft Min", "Metadata");
  addInput(m_softRangeMax, "Soft Max", "Metadata");
  addInput(m_hasHardRange, "Hard Range", "Metadata");
  addInput(m_hardRangeMin, "Hard Min", "Metadata");
  addInput(m_hardRangeMax, "Hard Max", "Metadata");
  addInput(m_hasCombo, "Use Combo", "Metadata");
  addInput(m_combo, "Combo", "Metadata");
  addInput(m_hasFileTypeFilter, "Use File Filter", "Metadata");
  addInput(m_fileTypeFilter, "File Filter", "Metadata");
  addInput(m_isOpenFile, "Is Open File", "Metadata");

  // [Julien] FE-5188, FE-5276
  if(setAlphaNum) alphaNumicStringOnly();


  QObject::connect(m_hasSoftRange, SIGNAL(stateChanged(int)), this, SLOT(onSoftRangeToggled(int)));
  QObject::connect(m_hasHardRange, SIGNAL(stateChanged(int)), this, SLOT(onHardRangeToggled(int)));
  QObject::connect(m_hasCombo, SIGNAL(stateChanged(int)), this, SLOT(onComboToggled(int)));
  QObject::connect(m_hasFileTypeFilter, SIGNAL(stateChanged(int)), this, SLOT(onFileTypeFilterToggled(int)));
}

DFGEditPortDialog::~DFGEditPortDialog()
{
}

QString DFGEditPortDialog::portType() const
{
  if(!m_portTypeCombo)
    return "";
  return m_portTypeCombo->currentText();
}

void DFGEditPortDialog::setPortType(QString value)
{
  if(!m_portTypeCombo)
    return;
  if(value.toLower() == "in")
    m_portTypeCombo->setCurrentIndex(0);
  else if(value.toLower() == "out")
    m_portTypeCombo->setCurrentIndex(1);
  else if(value.toLower() == "io")
    m_portTypeCombo->setCurrentIndex(2);
}

QString DFGEditPortDialog::title() const
{
  return m_titleEdit->text();
}

void DFGEditPortDialog::setTitle(QString value)
{
  m_titleEdit->setText(value);
}

QString DFGEditPortDialog::dataType() const
{
  return m_dataTypeEdit ? m_dataTypeEdit->text() : QString();
}

void DFGEditPortDialog::setDataType(QString value)
{
  if( m_dataTypeEdit )
    m_dataTypeEdit->setText( value );
  else
    assert( false );
}

bool DFGEditPortDialog::isDataTypeReadOnly() const 
{
  if(m_dataTypeEdit)
    return m_dataTypeEdit->isReadOnly();
  return false;
}

void DFGEditPortDialog::setDataTypeReadOnly( bool value ) 
{
  if(m_dataTypeEdit)
    m_dataTypeEdit->setReadOnly(value);
}

QString DFGEditPortDialog::extension() const
{
  return m_extensionEdit ? m_extensionEdit->text() : QString();
}

void DFGEditPortDialog::setExtension(QString value)
{
  if( m_extensionEdit )
    m_extensionEdit->setText( value );
  else
    assert( false );
}

bool DFGEditPortDialog::hidden() const
{
  return m_visibilityCombo->currentText() == "hidden";
}

void DFGEditPortDialog::setHidden()
{
  m_visibilityCombo->setCurrentIndex(2);
}

void DFGEditPortDialog::setVisibilityReadOnly( bool value ) 
{
  if(m_visibilityCombo)
  {
    QStandardItemModel *model = (QStandardItemModel *)(m_visibilityCombo->model());
    for(int i=0; i<m_visibilityCombo->count(); ++i)
    {
      QStandardItem *item = model->item(i,0);
      Qt::ItemFlags itemFlags = item->flags();
      if(value)
      {
        itemFlags &= ~Qt::ItemIsSelectable;
        itemFlags &= ~Qt::ItemIsEnabled;
      }
      else 
        itemFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
      item->setFlags(itemFlags);
    }
  }
}

bool DFGEditPortDialog::opaque() const
{
  return m_visibilityCombo->currentText() == "opaque";
}

void DFGEditPortDialog::setOpaque()
{
  m_visibilityCombo->setCurrentIndex(1);
}

bool DFGEditPortDialog::persistValue() const
{
  return m_persistValue->checkState() == Qt::Checked;
}

void DFGEditPortDialog::setPersistValue(bool value)
{
  m_persistValue->setCheckState(value ? Qt::Checked : Qt::Unchecked);
}

bool DFGEditPortDialog::hasSoftRange() const
{
  return m_hasSoftRange->checkState() == Qt::Checked;
}

void DFGEditPortDialog::setHasSoftRange(bool value)
{
  m_hasSoftRange->setCheckState(value ? Qt::Checked : Qt::Unchecked);
}

float DFGEditPortDialog::softRangeMin() const
{
  return (float)Util::tolerantStringToDouble(m_softRangeMin->text());
}

void DFGEditPortDialog::setSoftRangeMin(float value)
{
  m_softRangeMin->setText(QString::number(value));
}

float DFGEditPortDialog::softRangeMax() const
{
  return (float)Util::tolerantStringToDouble(m_softRangeMax->text());
}

void DFGEditPortDialog::setSoftRangeMax(float value)
{
  m_softRangeMax->setText(QString::number(value));
}

void DFGEditPortDialog::setSoftRangeReadOnly( bool value ) 
{
  if(m_softRangeMin)
    m_softRangeMin->setReadOnly(value);

  if(m_softRangeMax)
    m_softRangeMax->setReadOnly(value);

  if(m_hasSoftRange)
    m_hasSoftRange->setEnabled( !value );
}

bool DFGEditPortDialog::hasHardRange() const
{
  return m_hasHardRange->checkState() == Qt::Checked;
}

void DFGEditPortDialog::setHasHardRange(bool value)
{
  m_hasHardRange->setCheckState(value ? Qt::Checked : Qt::Unchecked);
}

float DFGEditPortDialog::hardRangeMin() const
{
  return (float)Util::tolerantStringToDouble(m_hardRangeMin->text());
}

void DFGEditPortDialog::setHardRangeMin(float value)
{
  m_hardRangeMin->setText(QString::number(value));
}

float DFGEditPortDialog::hardRangeMax() const
{
  return (float)Util::tolerantStringToDouble(m_hardRangeMax->text());
}

void DFGEditPortDialog::setHardRangeMax(float value)
{
  m_hardRangeMax->setText(QString::number(value));
}

void DFGEditPortDialog::setHardRangeReadOnly( bool value ) 
{
  if(m_hardRangeMin)
    m_hardRangeMin->setReadOnly(value);

  if(m_hardRangeMax)
    m_hardRangeMax->setReadOnly(value);

   if(m_hasHardRange)
    m_hasHardRange->setEnabled( !value );
}

bool DFGEditPortDialog::hasCombo() const
{
  return m_hasCombo->checkState() == Qt::Checked;
}

void DFGEditPortDialog::setHasCombo(bool value)
{
  m_hasCombo->setCheckState(value ? Qt::Checked : Qt::Unchecked);
}

QStringList DFGEditPortDialog::comboValues() const
{
  QStringList results = m_combo->text().split(',');
  for(int i=0;i<results.length();i++)
  {
    results[i] = results[i].trimmed();
    if(results[i][0] == '\'' || results[i][0] == '"')
      results[i] = results[i].right(results[i].length()-1);
    if(results[i][results[i].length()-1] == '\'' || results[i][results[i].length()-1] == '"')
      results[i] = results[i].left(results[i].length()-1);
  }
  return results;
}

void DFGEditPortDialog::setComboValues(QStringList value)
{
  QString flat;
  for(int i=0;i<value.length();i++)
  {
    if(i > 0)
      flat += ", ";
    flat += value[i];
  }
  m_combo->setText(flat);
}

void DFGEditPortDialog::setComboReadOnly( bool value ) 
{
  if(m_hasCombo)
    m_hasCombo->setEnabled( !value );
}

bool DFGEditPortDialog::hasFileTypeFilter() const
{
  return m_hasFileTypeFilter->checkState() == Qt::Checked;
}

void DFGEditPortDialog::setHasFileTypeFilter(bool value)
{
  m_hasFileTypeFilter->setCheckState(value ? Qt::Checked : Qt::Unchecked);
}

QString DFGEditPortDialog::fileTypeFilter() const
{
  return m_fileTypeFilter->text();
}

void DFGEditPortDialog::setFileTypeFilter(QString value)
{
  m_fileTypeFilter->setText(value);
}

bool DFGEditPortDialog::isOpenFile() const
{
  return m_isOpenFile->checkState() == Qt::Checked;
}

void DFGEditPortDialog::setIsOpenFile(bool value)
{
  m_isOpenFile->setCheckState(value ? Qt::Checked : Qt::Unchecked);
}


void DFGEditPortDialog::showEvent(QShowEvent * event)
{
  QTimer::singleShot(0, m_titleEdit, SLOT(setFocus()));
  DFGBaseDialog::showEvent(event);  
}

// Allows only alpha-numeric text only 
void DFGEditPortDialog::alphaNumicStringOnly() {
  setRegexFilter(QString("[a-zA-Z][_a-zA-Z0-9]*"));
}

// Filters the QLineEdit text with the regexFilter
void DFGEditPortDialog::setRegexFilter(QString regexFilter) {
  if(m_titleEdit)
  {
    QRegExp regex(regexFilter);
    QValidator *validator = new QRegExpValidator(regex, 0);
    m_titleEdit->setValidator(validator);
  }
}

void DFGEditPortDialog::onSoftRangeToggled(int state)
{
  m_softRangeMin->setEnabled(state == Qt::Checked);
  m_softRangeMax->setEnabled(state == Qt::Checked);
}

void DFGEditPortDialog::onHardRangeToggled(int state)
{
  m_hardRangeMin->setEnabled(state == Qt::Checked);
  m_hardRangeMax->setEnabled(state == Qt::Checked);
}

void DFGEditPortDialog::onComboToggled(int state)
{
  m_combo->setEnabled(state == Qt::Checked);
}

void DFGEditPortDialog::onFileTypeFilterToggled(int state)
{
  m_fileTypeFilter->setEnabled(state == Qt::Checked);
  m_isOpenFile->setEnabled(state == Qt::Checked);
}

void DFGEditPortDialog::done(int r)
{
  // Ok (Enter) pressed
  if(QDialog::Accepted == r)  
  { 
    // FE-7691 : Check if the current text is a KL valid type
    // If not, don't close and validate the dialog
    if(m_dataTypeEdit->checkIfTypeExist())    
      QDialog::done(r);
    else
      m_dataTypeEdit->displayInvalidTypeWarning();
  }
  // Cancel, Close, Exc pressed
  else  
    QDialog::done(r);
}

void DFGEditPortDialog::keyPressEvent(QKeyEvent * event)
{
  // FE-7691 : Don't call directly the parent QDialog::keyPressEvent(event);
  // Otherwise the warning message is displayed twice when the user press enter.
  // Really weird behaviour, and there is no Qt doc about this.
  if( event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return )
    QWidget::keyPressEvent(event);
  // FE-7878
  else
    QDialog::keyPressEvent(event);
}
