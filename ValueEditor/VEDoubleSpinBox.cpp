//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "VEDoubleSpinBox.h"

#include <FabricUI/Util/StringUtil.h>

#include <float.h>
#include <math.h>
#include <algorithm>

using namespace FabricUI::ValueEditor;

double const VEDoubleSpinBox::MAX_QT_VAL = 999999999.0;
int const VEDoubleSpinBox::MAX_QT_EXP = 10;

VEDoubleSpinBox::VEDoubleSpinBox()
{
  m_validator = FabricUI::Util::newDoubleValidator(this);
  setRange( -MAX_QT_VAL, MAX_QT_VAL );
  resetPrecision();
}

VEDoubleSpinBox::~VEDoubleSpinBox()
{
}

QString VEDoubleSpinBox::textFromValue( double val ) const
{
  return QString::number( val );
}

double VEDoubleSpinBox::valueFromText(const QString &text) const
{
  QString t = text;
  t.replace(',', '.');
  if (t.startsWith('.'))
    t = "0" + t;
  return t.toDouble();
}

double VEDoubleSpinBox::implicitLogBaseChangePerStep()
{
  if ( minimum() == -MAX_QT_VAL
    || maximum() == +MAX_QT_VAL )
    return 0.0;

  return std::max(
    -6.0,
    log10( ( maximum() - minimum() ) / 20.0 )
    );
}

double VEDoubleSpinBox::updateStep(
  double deltaXInInches,
  double logBaseChangePerStep
  )
{
  double base10Exp = 0.5 * deltaXInInches + logBaseChangePerStep;
  double velocity = pow( 10, base10Exp );

  // Always step by a round-number 
  int roundedBase10Exp = int( round( base10Exp ) );
  double changePerStep = pow( 10.0, roundedBase10Exp );

  static const double minChangePerStep = 0.00001;
  changePerStep = std::max( minChangePerStep, changePerStep );
  static const double maxChangePerStep = 100000;
  changePerStep = std::min( maxChangePerStep, changePerStep );

  setSingleStep( changePerStep );

  return std::max(
    0.01,
    0.5 * velocity / changePerStep
    );
}

void VEDoubleSpinBox::resetPrecision()
{
  setDecimals( MAX_QT_EXP );
}

QValidator::State VEDoubleSpinBox::validate(QString &text, int &pos) const
{
  return m_validator->validate(text, pos);
}
