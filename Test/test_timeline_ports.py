import os, pytest
import fabric_test_ui
from PySide import QtCore
from FabricEngine import Core
from FabricEngine.FabricUI import Application
from FabricEngine.Canvas.CanvasWindow import CanvasWindow

# [andrew 20160330] FE-6364
pytestmark = pytest.mark.skipif(
    fabric_test_ui.is_missing_display(),
    reason="missing display",
    )

@pytest.yield_fixture(scope="module")
def canvas_app():
    app = fabric_test_ui.create_canvas_app()
    yield app
    fabric_test_ui.cleanup_canvas_app(app)

@pytest.yield_fixture(scope="module")
def canvas_win(canvas_app):
    win = fabric_test_ui.create_canvas_win(
      canvas_app,
      report_line_mapper=lambda line: line + "\n",
      )
    yield win
    fabric_test_ui.cleanup_canvas_win(win)

# Returns the output of the test
def main(canvas) :
  from PySide import QtCore
  from FabricEngine import Core
  binding = canvas.dfgWidget.getDFGController().getBinding()
  ex = binding.getExec()

  names = [
    "timeline",
    "timelineStart",
    "timelineEnd",
    "timelineFramerate"
  ]
  for name in names :
    ex.addExecPort( name, canvas.client.DFG.PortTypes.In, "Float32" )

  funcName = "reportPorts"
  ex.addInstWithNewFunc( funcName )
  func = ex.getSubExec( funcName )

  for name in names :
    func.addExecPort( name, canvas.client.DFG.PortTypes.In, "Float32" )
    ex.connectTo( name, funcName + '.' + name )

  mainCode = "\n".join( [ " report( '" + name + " = ' + " + name + " );" for name in names ] )
  func.setCode("""
  dfgEntry
  {
  """ + mainCode + """
  report("");
  }
  """)

  ex.connectTo( funcName + '.exec', 'exec' )

  #print("TEST_START");
  canvas.test_output = ""


  # Default values
  QtCore.QCoreApplication.processEvents()

  # Time Range
  canvas.timeLine.setTimeRange( 11, 314 )
  QtCore.QCoreApplication.processEvents()

  # FrameRate
  canvas.timeLine.setFrameRate( 25 )
  QtCore.QCoreApplication.processEvents()

  # Back to an empty graph
  canvas.show()
  canvas.onNewGraph(True)

  #print("TEST_END");
  return canvas.test_output

def test_timeline_ports(canvas_win):

  import os

  refOutputPath = os.environ['FABRIC_SCENE_GRAPH_DIR'] + '/Native/FabricUI/Test/test_timeline_ports.out'
  refOutput = open( refOutputPath, 'r' ).read()

  refBaseName, refOutputExt = os.path.splitext( refOutputPath )
  if( os.path.exists( refBaseName + os.extsep + os.environ['FABRIC_BUILD_OS'] + os.extsep + "skip" ) ) :
    import pytest
    pytest.skip("Skip")

  # TODO: some of the output from the DFGController is not caught
  testOutput = main(canvas_win)

  if testOutput != refOutput :
    import difflib
    diff = difflib.unified_diff( refOutput.splitlines(1), testOutput.splitlines(1) )
    raise Exception( ''.join( diff ) )
