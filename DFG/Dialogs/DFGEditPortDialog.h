// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGEditPortDialog__
#define __UI_DFG_DFGEditPortDialog__

#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QShowEvent>
#include <QKeyEvent>
#include "DFGBaseDialog.h"
#include "DFGRegisteredTypeLineEdit.h"
#include "DFGExtensionLineEdit.h"

namespace FabricUI
{
  namespace DFG
  {

    class DFGEditPortDialog : public DFGBaseDialog
    {
      Q_OBJECT

    public:

      DFGEditPortDialog(
        QWidget * parent, 
        FabricCore::Client & client, 
        bool showPortType, 
        bool topGraphPort, 
        const DFGConfig & dfgConfig = DFGConfig(),
        bool setAlphaNum = false
      );
      virtual ~DFGEditPortDialog();

      QString portType() const;
      void setPortType(QString value);
      QString title() const;
      void setTitle(QString value);
      QString dataType() const;
      void setDataType(QString value);
      QString extension() const;
      void setExtension(QString value);
      bool hidden() const;
      void setHidden();
      void setVisibilityReadOnly( bool value );

      bool opaque() const;
      void setOpaque();
      bool persistValue() const;
      void setPersistValue( bool value );
      bool isDataTypeReadOnly() const;
      void setDataTypeReadOnly( bool value );
     
      bool hasSoftRange() const;
      void setHasSoftRange(bool value);
      float softRangeMin() const;
      void setSoftRangeMin(float value);
      float softRangeMax() const;
      void setSoftRangeMax(float value);
      void setSoftRangeReadOnly( bool value );

      bool hasHardRange() const;
      void setHasHardRange(bool value);
      float hardRangeMin() const;
      void setHardRangeMin(float value);
      float hardRangeMax() const;
      void setHardRangeMax(float value);
      void setHardRangeReadOnly( bool value );

      bool hasCombo() const;
      void setHasCombo(bool value);
      QStringList comboValues() const;
      void setComboValues(QStringList value);
      void setComboReadOnly( bool value );

      bool hasFileTypeFilter() const;
      void setHasFileTypeFilter(bool value);
      QString fileTypeFilter() const;
      void setFileTypeFilter(QString value);
      bool isOpenFile() const;
      void setIsOpenFile(bool value);

      virtual void showEvent(QShowEvent * event);

      /// Filter the QLineEdit text with the regexFilter
      void alphaNumicStringOnly();
      /// Filter the QLineEdit text with the setRegexFilter
      void setRegexFilter(QString regex);

    protected:
      virtual void keyPressEvent(QKeyEvent * event);

    public slots:
      virtual void done(int r);

    private slots:

      void onSoftRangeToggled(int state);
      void onHardRangeToggled(int state);
      void onComboToggled(int state);
      void onFileTypeFilterToggled(int state);

    private:

      QComboBox * m_portTypeCombo;
      QLineEdit * m_titleEdit;
      DFGRegisteredTypeLineEdit * m_dataTypeEdit;
      DFGExtensionLineEdit * m_extensionEdit;
      QComboBox * m_visibilityCombo;
      QCheckBox * m_persistValue;
      QCheckBox * m_hasSoftRange;
      QLineEdit * m_softRangeMin;
      QLineEdit * m_softRangeMax;
      QCheckBox * m_hasHardRange;
      QLineEdit * m_hardRangeMin;
      QLineEdit * m_hardRangeMax;
      QCheckBox * m_hasCombo;
      QLineEdit * m_combo;
      QCheckBox * m_hasFileTypeFilter;
      QLineEdit * m_fileTypeFilter;
      QCheckBox * m_isOpenFile;

    };

  };

};

#endif // __UI_DFG_DFGEditPortDialog__
