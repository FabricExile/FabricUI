
#include <FabricUI/FCurveEditor/RuledGraphicsView.h>

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>

using namespace FabricUI::FCurveEditor;

inline float RandFloat() { return float( rand() ) / RAND_MAX; }

int main()
{
  int argc = 0;
  QApplication app( argc, NULL );
  {
    QGraphicsScene* scene = new QGraphicsScene();
    scene->setSceneRect( QRectF( -1E3, -1E3, 2 * 1E3, 2 * 1E3 ) );
    for( size_t i = 0; i < 100; i++ )
    {
      QGraphicsItemGroup* item = new QGraphicsItemGroup();
      item->moveBy( RandFloat(), RandFloat() );
      QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem( item );
      ellipse->setFlag( QGraphicsItem::ItemIgnoresTransformations, true );
      float scale = 8;
      ellipse->setRect( QRect( -0.5*scale, -0.5*scale, scale, scale ) );
      ellipse->setBrush( QColor( 255, 128, 0 ) );
      QPen pen; pen.setWidthF( 0.1 * scale );
      ellipse->setPen( pen );
      scene->addItem( item );
    }

    RuledGraphicsView* view = new RuledGraphicsView();
    view->view()->setScene( scene );
    view->resize( 800, 600 );
    view->view()->fitInView( 0, 0, 1, 1, Qt::KeepAspectRatio );
    view->show();
  }
  app.exec();
  return 0;
}
