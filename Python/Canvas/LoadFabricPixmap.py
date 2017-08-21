#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import os
from PySide import QtGui

def LoadFabricPixmap(basename):
    """Loads the contents of the pixmap below
    
    Args:
        basename (str): Stylesheet base name.
    
    Returns:
        pixmap
    
    """

    fabricDir = os.environ.get('FABRIC_DIR')
    if fabricDir is None:
        raise IOError("LoadFabricPixmap: missing FABRIC_DIR environment variable\n")
    path = os.path.join(fabricDir, "Resources", basename)

    return QtGui.QPixmap(path)
