#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import os, subprocess
Import(
  'allServicesLibFiles',
  'buildArch',
  'buildOS',
  'buildType',
  'capiSharedFiles',
  'capiSharedLibFlags',
  'corePythonModuleFiles',
  'dfgSamples',
  'extraDFGPresets',
  'fabricFlags',
  'feLogoPNG',
  'parentEnv',
  'pythonConfigs',
  'qtDir',
  'qtFlags',
  'qtInstalledLibs',
  'qtMOC',
  'splitSearchFiles',
  'stageDir',
  'uiLibPrefix',
  )

if buildOS == 'Windows' and buildType == 'Debug':
  Import('servicesFlags_mtd')
  servicesFlags = servicesFlags_mtd
else:
  Import('servicesFlags_mt')
  servicesFlags = servicesFlags_mt

qtIncludeDir = os.path.join(qtDir, 'include')
qtLibDir = os.path.join(qtDir, 'lib')
qtBinDir = os.path.join(qtDir, 'bin')

suffix = '4'
if buildType == 'Debug':
  suffix = 'd4'

qtMOCBuilder = Builder(
  action = [[qtMOC, '-o', '$TARGET', '$SOURCE']],
  prefix = 'moc_',
  suffix = '.cc',
  src_suffix = '.h',
)

# create the build environment
env = parentEnv.Clone()
env.Append(BUILDERS = {'QTMOC': qtMOCBuilder})
env.Append(CPPPATH = [env.Dir('#').Dir('Native').srcnode()])

if buildOS == 'Darwin':
  env.Append(CCFLAGS = ['-fvisibility=hidden'])
  env.Append(CXXFLAGS = ['-std=c++03'])
  env.Append(CXXFLAGS = ['-stdlib=libstdc++'])
  env.Append(CXXFLAGS = ['-fvisibility=hidden'])
  env.Append(LINKFLAGS = ['-stdlib=libstdc++'])

  frameworkPath = os.path.join(
    os.path.split(
      subprocess.Popen(
        'xcodebuild -version -sdk macosx10.9 Path',
        shell=True,
        stdout=subprocess.PIPE
        ).stdout.read()
      )[0],
    'MacOSX10.9.sdk',
    )
  env.Append(CCFLAGS = ["-isysroot", frameworkPath])
  env.Append(FRAMEWORKS = ['OpenGL', 'Cocoa', 'Foundation'])


if buildOS == 'Linux' and not env.get('BUILDING_MAYA_2017'):
  env.Replace( CC = '/opt/centos5/usr/bin/gcc' )
  env.Replace( CXX = '/opt/centos5/usr/bin/gcc' )

if buildOS == 'Windows':
  env.Append(CPPDEFINES = ['FABRIC_OS_WINDOWS'])
elif buildOS == 'Linux':
  env.Append(CPPDEFINES = ['FABRIC_OS_LINUX'])
elif buildOS == 'Darwin':
  env.Append(CPPDEFINES = ['FABRIC_OS_DARWIN'])

if buildOS != 'Windows':
  if '-static-libstdc++' in env['LINKFLAGS']:
    env['LINKFLAGS'].remove('-static-libstdc++')
  if '-static-libgcc' in env['LINKFLAGS']:
    env['LINKFLAGS'].remove('-static-libgcc')

# # enable timers
# env.Append(CPPDEFINES = ['FABRICUI_TIMERS'])

def GlobQObjectHeaders(env, filter):
  headers = Flatten(env.Glob(filter))
  qobjectHeaders = []
  for header in headers:
    content = open(header.srcnode().abspath, 'rb').read()
    if content.find('Q_OBJECT') > -1:
      qobjectHeaders.append(header)
  return qobjectHeaders
Export('GlobQObjectHeaders')
env.AddMethod(GlobQObjectHeaders)

def GlobQObjectSources(env, filter):
  headers = env.GlobQObjectHeaders(filter)
  sources = []
  for header in headers:
    sources += env.QTMOC(header)
  return sources
Export('GlobQObjectSources')
env.AddMethod(GlobQObjectSources)

if buildType == 'Debug':
  env.Append(CPPDEFINES = ['_DEBUG'])

env.MergeFlags(fabricFlags)
env.MergeFlags(qtFlags)

dirs = [
  'Util',
  'Actions',
  'Dialog',
  'Style',
  'Commands',
  'Tools',
  'Viewports',
  'KLEditor',
  'Menus',
  'Application',
  'TreeView',
  'ValueEditor_Legacy',
  'ValueEditor',
  'OptionsEditor',
  'OptionsEditor/Commands',
  'GraphView',
  'GraphView/Commands',
  'DFG',
  'DFG/Commands',
  'DFG/DFGUICmd',
  'DFG/Dialogs',
  'DFG/PortEditor',
  'DFG/TabSearch',
  'DFG/Tools',
  'FCurveEditor',
  'FCurveEditor/Models/AnimXKL',
  'FCurveEditor/Models/DFG',
  'FCurveEditor/ValueEditor',
 
  'SceneHub',
  'SceneHub/TreeView',
  'SceneHub/Menus',
  'SceneHub/DFG',
  'SceneHub/Viewports',
  'SceneHub/Commands',
  'SceneHub/ValueEditor',

  'Licensing',
  'ModelItems',
  'Test',
  'SplashScreens',
]

installedHeaders = []
sources = []
strsources = []
strheaders = []
for d in dirs:
  headers = Flatten(env.Glob(os.path.join(env.Dir('.').abspath, d, '*.h')))
  dirsrc = Flatten(env.Glob(os.path.join(env.Dir('.').abspath, d, '*.cpp')))
  if buildOS == 'Darwin':
    dirsrcMM = Flatten(env.Glob(os.path.join(env.Dir('.').abspath, d, '*.mm')))
  
  for h in headers:
    strheaders.append(str(h))
  for c in dirsrc:
    strsources.append(str(c))

  sources += dirsrc
  if buildOS == 'Darwin':
    sources += dirsrcMM

  sources += env.GlobQObjectSources(os.path.join(env.Dir('.').abspath, d, '*.h'))
  if uiLibPrefix == 'ui':
    installedHeaders += env.Install(stageDir.Dir('include').Dir('FabricUI').Dir(d), headers)

uiLib = env.StaticLibrary('FabricUI', sources)

try:
  # HACK : this script is called several times, but only
  # once with this variable exported (hence the try/except here)
  Import( "registerFabricUIMSVS" )
except:
  registerFabricUIMSVS = False
  
if buildOS == 'Windows' and registerFabricUIMSVS :

  msvsEnv = env.Clone()

  # Making the include paths absolute
  msvsIncludes = msvsEnv["CPPPATH"]
  def makeAbs( p ):
    if( type(p) is str ) :
      return p
    else :
      return p.srcnode().abspath
  msvsIncludes = [ makeAbs( p ) for p in msvsIncludes ]

  msvsEnv["CPPPATH"] = msvsIncludes
  msvsProj = msvsEnv.MSVSProject(
    target = 'MSVS/FabricUI' + msvsEnv['MSVSPROJECTSUFFIX'],
    srcs = strsources + strheaders,
    buildtarget = uiLib,
    variant = 'Release'
  )
  msvsEnv.Alias( "FabricUIMSVS", msvsProj )

import copy
uiFiles = copy.copy(installedHeaders)
if uiLibPrefix == 'ui':
  uiLib = env.Install(stageDir.Dir('lib'), uiLib)
  uiFiles.append(uiLib)
  icons = env.Install(
    stageDir.srcnode().Dir('Resources').Dir('Icons'),
    [
      Glob(os.path.join(env.Dir('GraphView').Dir('images').srcnode().abspath, '*.png')),
      Glob(os.path.join(env.Dir('Icons').srcnode().abspath, '*.png')),
      Glob(os.path.join(env.Dir('DFG').Dir('Icons').srcnode().abspath, '*.png')),
      # Glob(os.path.join(env.Dir('ValueEditor').Dir('images').srcnode().abspath, '*.png')),
      ]
    )
  env.Depends(uiLib, icons)
  fonts = map(
    lambda name: 
      env.Install(
        stageDir.srcnode().Dir('Resources').Dir('Fonts').Dir(name),
        [
          Glob(os.path.join(env.Dir('DFG').Dir('Fonts').Dir(name).srcnode().abspath, '*')),
          ]
        ),
    ['Roboto', 'Roboto_Condensed', 'Roboto_Mono', 'Roboto_Slab', 'FontAwesome']
    )
  env.Depends(uiLib, fonts)
  qss = env.Install(
    stageDir.srcnode().Dir('Resources').Dir('QSS'),
    [
      Glob(os.path.join(env.Dir('ValueEditor').srcnode().abspath, '*.qss')),
      Glob(os.path.join(env.Dir('DFG').srcnode().abspath, '*.qss')),
      Glob(os.path.join(env.Dir('.').srcnode().abspath, '*.qss')),
      ]
    )
  env.Depends(uiLib, qss)
      
locals()[uiLibPrefix + 'Lib'] = uiLib
locals()[uiLibPrefix + 'IncludeDir'] = env.Dir('#').Dir('Native').srcnode()
locals()[uiLibPrefix + 'Flags'] = {
  'CPPPATH': [locals()[uiLibPrefix + 'IncludeDir']],
  'LIBS': [locals()[uiLibPrefix + 'Lib']]
}
Export(uiLibPrefix + 'Lib', uiLibPrefix + 'IncludeDir', uiLibPrefix + 'Flags')

env.Alias(uiLibPrefix + 'Lib', uiFiles)

if uiLibPrefix == 'uiModo901' and buildOS == 'Darwin':
  env.Append(CCFLAGS = ['-Wno-#warnings'])
  env.Append(CCFLAGS = ['-Wno-unused-private-field'])

if uiLibPrefix == 'ui':

  fabricDir = env.Dir(os.environ['FABRIC_DIR'])

  pysideGens = []
  installedPySideLibs = []

  for pythonVersion, pythonConfig in pythonConfigs.iteritems():

    if not pythonConfig['havePySide']:
      continue

    pysideEnv = env.Clone()
    pysideEnv.MergeFlags(capiSharedLibFlags)
    pysideEnv.MergeFlags(servicesFlags)
    pysideEnv.Append(LIBS = ['FabricSplitSearch'])
    pysideEnv.MergeFlags(pythonConfig['pythonFlags'])
    pysideEnv.MergeFlags(pythonConfig['shibokenFlags'])
    pysideEnv.MergeFlags(pythonConfig['pysideFlags'])
    pysideEnv.MergeFlags(qtFlags)
    # These are necessary because of the way that shiboken
    # generates its includes
    pysideEnv.Append(CPPPATH = [
      stageDir.Dir('include').Dir('FabricServices').Dir('ASTWrapper'),
      os.path.join(qtIncludeDir, 'QtCore'),
      os.path.join(qtIncludeDir, 'QtGui'),
      os.path.join(qtIncludeDir, 'QtOpenGL'),
      ])

    if buildOS != 'Windows':
      pysideEnv.Append(CCFLAGS = ['-Wno-sign-compare', '-Wno-error'])

    pysideDir = pysideEnv.Dir('pyside').Dir('python'+pythonVersion)
    shibokenDir = pysideEnv.Dir('shiboken')

    if buildOS == 'Windows':
      pysideEnv['CCFLAGS'].remove('/W2')
      if buildType == 'Debug':
        pysideEnv.Append(LINKFLAGS = ['/NODEFAULTLIB:LIBCMTD'])
      else:
        pysideEnv.Append(LINKFLAGS = ['/NODEFAULTLIB:LIBCMT'])
    else:
      pysideEnv.Append(CCFLAGS = ['-Wno-sign-compare', '-Wno-error'])

    shibokenIncludePaths = [
      str(pythonConfig['pysideIncludeDir']),
      ]
    shibokenIncludePaths.append(qtIncludeDir)
    shibokenIncludePaths.append(os.path.join(os.environ['FABRIC_DIR'], "include"))

    if buildOS == 'Linux':
      pysideEnv['ENV']['LD_LIBRARY_PATH'] = qtLibDir
    elif buildOS == 'Darwin':
      # [andrew 20160329] need to point here so that rpath'd paths show up as expected for shiboken
      pysideEnv['ENV']['DYLD_LIBRARY_PATH'] = os.path.join(os.environ['FABRIC_DIR'], 'lib')
    elif buildOS == 'Windows':
      pysideEnv['ENV']['PATH'] = qtBinDir+';'+pysideEnv['ENV']['PATH']

    diffFile = shibokenDir.File('fabricui.diff')

    pysideGen = pysideEnv.Command(
      pysideDir.Dir('FabricUI').File('fabricui_python.h'),
      [
        shibokenDir.File('global.h'),
        diffFile,
        shibokenDir.File('fabricui.xml'),
        shibokenDir.File('fabricui_core.xml'),
        shibokenDir.File('fabricui_style.xml'),
        shibokenDir.File('fabricui_application.xml'),
        shibokenDir.File('fabricui_tools.xml'),
        shibokenDir.File('fabricui_actions.xml'),
        shibokenDir.File('fabricui_commands.xml'),
        shibokenDir.File('fabricui_optionseditor.xml'),
        shibokenDir.File('fabricui_dialog.xml'),
        shibokenDir.File('fabricui_dfg.xml'),
        shibokenDir.File('fabricui_viewports.xml'),
        shibokenDir.File('fabricui_util.xml'),
        shibokenDir.File('fabricui_scenehub.xml'),
        ],
      [
          [
              pythonConfig['shibokenBin'],
              '${SOURCES[0]}',
              #'--no-suppress-warnings',
              '--typesystem-paths='+os.pathsep.join([
                str(shibokenDir.srcnode()),
                str(pythonConfig['pysideTypesystemsDir']),
                ]),
              '--include-paths='+os.pathsep.join(shibokenIncludePaths),
              '--output-directory='+pysideDir.abspath,
              '--avoid-protected-hack',
              #################################################################
              # These are copied from PySide's CMakeLists.txt:
              '--generator-set=shiboken',
              '--enable-parent-ctor-heuristic',
              '--enable-pyside-extensions',
              '--enable-return-value-heuristic',
              '--use-isnull-as-nb_nonzero',
              #################################################################
              '${SOURCES[2]}',
          ],
          [ 'cd', pysideDir.Dir("FabricUI").abspath, '&&', 'patch', '-p1',
              '<${SOURCES[1].abspath}' ],
      ]
      )
    pysideEnv.Depends(pysideGen, installedHeaders)
    pysideEnv.Requires(pysideGen, uiLib) # HACK [FE-8436] : this line shouldn't be needed
      # ... instead, we should just add QtCore to the RPath on OSX for the shiboken executable
    pysideEnv.Depends(pysideGen, corePythonModuleFiles)
    pysideGens.append(pysideGen)

    pysideEnv.Append(CPPPATH = [
        pysideEnv.Dir('Util').srcnode(),
        pysideEnv.Dir('Commands').srcnode(),
        pysideEnv.Dir('Menus').srcnode(),
        pysideEnv.Dir('Dialog').srcnode(),
        pysideEnv.Dir('Style').srcnode(),
        pysideEnv.Dir('Tools').srcnode(),
        pysideEnv.Dir('Actions').srcnode(),
        pysideEnv.Dir('OptionsEditor').srcnode(),
        pysideEnv.Dir('OptionsEditor/Commands').srcnode(),
        pysideEnv.Dir('DFG').srcnode(),
        pysideEnv.Dir('DFG/Commands').srcnode(),
        pysideEnv.Dir('DFG/Tools').srcnode(),
        pysideEnv.Dir('DFG/DFGUICmd').srcnode(),
        pysideEnv.Dir('GraphView').srcnode(),
        pysideEnv.Dir('Licensing').srcnode(),
        pysideEnv.Dir('Application').srcnode(),
        pysideEnv.Dir('ModelItems').srcnode(),
        pysideEnv.Dir('ValueEditor').srcnode(),
        pysideEnv.Dir('ValueEditor_Legacy').srcnode(),
        pysideEnv.Dir('Viewports').srcnode(),
        pysideEnv.Dir('Util').srcnode(),
        pysideEnv.Dir('Test').srcnode(),
        pysideEnv.Dir('SceneHub').srcnode(),
        pysideEnv.Dir('SceneHub/DFG').srcnode(),
        pysideEnv.Dir('SceneHub/Menus').srcnode(),
        pysideEnv.Dir('SceneHub/Commands').srcnode(),
        pysideEnv.Dir('SceneHub/TreeView').srcnode(),
        pysideEnv.Dir('SceneHub/Viewports').srcnode(),
        pysideEnv.Dir('SceneHub/ValueEditor').srcnode(),
        fabricDir.Dir('include'),
        ])
    pysideEnv.Append(CPPPATH = [pythonConfig['includeDir']])
    if buildOS == 'Darwin':
      pysideEnv.Append(LIBPATH = [pythonConfig['libDir']])
      pysideEnv.Append(LIBS = [pythonConfig['lib']])
      pysideEnv.Append(FRAMEWORKS = ['CoreFoundation'])
    
    pysideEnv.Append(LIBS = [
      'FabricUI',
      ])
    pysideEnv.Append(CPPDEFINES = [
      'FEC_PROVIDE_STL_BINDINGS',
      ])

    fabricPythonLibName = "FabricPython"

    if buildOS == 'Linux':
      pysideEnv.Append(LINKFLAGS = Literal(','.join([
        '-Wl',
        '-rpath',
        '$ORIGIN/../../../lib/',
        '-rpath',
        '$ORIGIN/../../../Python/' + pythonVersion + '/FabricEngine/',
        ])))
    if buildOS == 'Darwin':
      pysideEnv.Append(LINKFLAGS = ['-Wl,-rpath,@loader_path/../../..'])
      pysideEnv.Append(CCFLAGS = ['-Wno-mismatched-tags'])
    pysideEnv.Append(CPPPATH = pysideDir.abspath)
    pysideEnv.Append(CPPPATH = stageDir.Dir('Python/' + pythonVersion + '/FabricEngine'))
    pysideEnv.Append(LIBPATH = [stageDir.Dir('Python/' + pythonVersion + '/FabricEngine')])
    pysideEnv.Append(LIBS = [fabricPythonLibName])
    if buildOS == 'Windows':
      if buildType == 'Debug':
        pysideEnv.Append(LIBS = "MSVCRTD")
      else:
        pysideEnv.Append(LIBS = "MSVCRT")
    installedPySideLibName = 'FabricUI'+pythonConfig['moduleSuffix']
    pythonDstDir = pysideEnv['STAGE_DIR'].Dir('Python').Dir(pythonVersion).Dir('FabricEngine')

    # [andrew 20160411] building FabricUI from open repo needs to resolve circular deps
    # but if building in the Fabric tree this is already done elsewhere
    if buildOS == 'Linux' and not '-Wl,--start-group' in pysideEnv['_LIBFLAGS']:
      pysideEnv['_LIBFLAGS'] = '-Wl,--start-group ' + pysideEnv['_LIBFLAGS'] + ' -Wl,--end-group'

    installedPySideLib = pysideEnv.LoadableModule(
      pythonDstDir.File(installedPySideLibName),
      [
        pysideEnv.Glob('pyside/python'+pythonVersion+'/*.cpp'),
        pysideEnv.Glob('pyside/python'+pythonVersion+'/FabricUI/*.cpp'),
      ],
      LDMODULEPREFIX='',
      LDMODULESUFFIX=pythonConfig['moduleSuffix'],
      )
    pysideEnv.Depends(installedPySideLib, pysideGen)
    installedPySideLibs.append(installedPySideLib)
    if buildOS == 'Windows':
      pysideDLLDst = pysideEnv['STAGE_DIR'].Dir('bin')
    else:
      pysideDLLDst = pysideEnv['STAGE_DIR'].Dir('lib')

    pysideDLLs = ['shiboken-python'+pythonVersion, 'pyside-python'+pythonVersion]
    pysideVersion = pythonConfig['pysideVersion']
    pysideMaj, pysideMin, pysideBug = pysideVersion.split('.')

    if buildOS == 'Windows':
      for pysideDLL in pysideDLLs:
        installedPySideLibs.append(
          pysideEnv.Install(
            pysideDLLDst,
            pythonConfig['pysideDir'].Dir('bin').File(pysideDLL+'.dll').abspath
            )
          )
    elif buildOS == 'Linux':
      libSuffix = 'so'
      for pysideDLL in pysideDLLs:
        installedPySideLibs.append(
          pysideEnv.AddPostAction(
            pysideEnv.Install(
              pysideDLLDst,
              pythonConfig['pysideDir'].Dir('lib').File('lib'+pysideDLL+'.'+libSuffix+'.'+pysideVersion)
              ),
            [
              [
                'ln', '-snf',
                '.'.join(['lib' + pysideDLL, libSuffix, pysideMaj, pysideMin, pysideBug]),
                pysideDLLDst.File('.'.join(['lib' + pysideDLL, libSuffix, pysideMaj, pysideMin])),
                ],
              [
                'ln', '-snf',
                '.'.join(['lib' + pysideDLL, libSuffix, pysideMaj, pysideMin]),
                pysideDLLDst.File('.'.join(['lib' + pysideDLL, libSuffix, pysideMaj])),
                ],
              [
                'ln', '-snf',
                '.'.join(['lib' + pysideDLL, libSuffix, pysideMaj]),
                pysideDLLDst.File('.'.join(['lib' + pysideDLL, libSuffix])),
                ]
              ]
            )
          )
    elif buildOS == 'Darwin':
      libSuffix = 'dylib'
      for pysideDLL in pysideDLLs:
        installedPySideLibs.append(
          pysideEnv.AddPostAction(
            pysideEnv.Install(
              pysideDLLDst,
              pythonConfig['pysideDir'].Dir('lib').File('lib'+pysideDLL+'.'+pysideVersion+'.'+libSuffix)
              ),
            [
              [
                'ln', '-snf',
                '.'.join(['lib' + pysideDLL, pysideMaj, pysideMin, pysideBug, libSuffix]),
                pysideDLLDst.File('.'.join(['lib' + pysideDLL, pysideMaj, pysideMin, libSuffix])),
                ],
              [
                'ln', '-snf',
                '.'.join(['lib' + pysideDLL, pysideMaj, pysideMin, libSuffix]),
                pysideDLLDst.File('.'.join(['lib' + pysideDLL, pysideMaj, libSuffix])),
                ],
              [
                'ln', '-snf',
                '.'.join(['lib' + pysideDLL, pysideMaj, libSuffix]),
                pysideDLLDst.File('.'.join(['lib' + pysideDLL, libSuffix])),
                ]
              ]
            )
          )

    pysideModuleDstDir = pysideEnv['STAGE_DIR'].Dir('Python').Dir(pythonVersion).Dir('PySide')
    if buildOS == 'Windows':
      pysideModuleSrcDir = pythonConfig['pysideDir'].Dir('lib').Dir('site-packages').Dir('PySide')
    else:
      pysideModuleSrcDir = pythonConfig['pysideDir'].Dir('lib').Dir('python'+pythonVersion).Dir('site-packages').Dir('PySide')
    installedPySideLibs.append(
      pysideEnv.Install(
        pysideModuleDstDir,
        Glob(os.path.join(pysideModuleSrcDir.abspath, '*'))
        )
      )

    pythonDir = [
      'Canvas',
      'Canvas/Application',
      'Canvas/HotkeyEditor',
      'Canvas/Commands'
    ]

    for dir_ in pythonDir:
      installedPySideLibs.append(
        pysideEnv.Install(
          pysideEnv['STAGE_DIR'].Dir('Python').Dir(pythonVersion).Dir('FabricEngine').Dir(dir_),
          Glob(os.path.join(pysideEnv.Dir('Python').Dir(dir_).abspath, '*.py'))
          )
        )

    installedPySideLibs.append(
      pysideEnv.Install(
        pysideEnv['STAGE_DIR'].Dir('bin'),
        pysideEnv.Dir('Python').File('canvas.py')
        )
      )
    installedPySideLibs.append(
      pysideEnv.Install(
        pysideEnv['STAGE_DIR'].Dir('Samples').Dir('Python').Dir('AlembicViewer'),
        Glob(os.path.join(pysideEnv.Dir('Samples').Dir('AlembicViewer').abspath, '*'))
        )
      )

    if os.environ.get('FABRIC_SCENEHUB', 0):
      installedPySideLibs.append(
        pysideEnv.Install(
          pysideEnv['STAGE_DIR'].Dir('Python').Dir(pythonVersion).Dir('FabricEngine').Dir('SceneHub'),
          Glob(os.path.join(pysideEnv.Dir('Python').Dir('SceneHub').abspath, '*'))
          )
        )
      installedPySideLibs.append(
        pysideEnv.Install(
          pysideEnv['STAGE_DIR'].Dir('bin'),
          pysideEnv.Dir('Python').File('sceneHub.py')
          )
        )
      
  pysideEnv.Alias('pysideGen', pysideGens)
  pysideEnv.Alias('pyside', installedPySideLibs)
  pysideEnv.Alias('canvas.py', [
    installedPySideLibs,
    capiSharedFiles,
    extraDFGPresets,
    splitSearchFiles,
    dfgSamples,
    qtInstalledLibs,
    allServicesLibFiles,
    feLogoPNG,
    ])

Return('uiFiles')
