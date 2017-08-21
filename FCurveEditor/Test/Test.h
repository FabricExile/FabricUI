
#include <QString>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <assert.h>

QString LoadQSS()
{
  QFile qss( "FabricUI.qss" );
  if( qss.open( QIODevice::ReadOnly ) )
  {
    QString qssTxt = QTextStream( &qss ).readAll();
    char* fde = getenv( "FABRIC_DIR" );
    if( fde == NULL )
    {
      std::cerr << "undefined env-var FABRIC_DIR" << std::endl;
      return "";
    }
    qssTxt = qssTxt.replace( "${FABRIC_DIR}", fde );
    return qssTxt;
  }
  return "";
}
