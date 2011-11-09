
import SCons.Node.FS
import SCons.Builder

# Stolen from http://oss.linn.co.uk/repos/Releases/Davaar/Kinsky4.2.4/BuildSystem/Tool/PackageMaker.py

# $TARGET is the name of the output pkg file
# $SOURCE is the .dst folder containing the stuff to package
#
# Requires the following params:
#
# TITLE           - the title to be displayed in the installer
# INFO_PLIST      - the .plist file for the package
# SCRIPT_DIR      - the folder containing the installer scripts
# VERSION         - the version of the application


PackageMakerCom = '$PACKAGEMAKER -d dist/CaSPER.pmdoc --out $TARGET --version $VERSION'

PackageMakerBuilder = SCons.Builder.Builder(action = '$PACKAGEMAKERCOM',
                                            source_factory = SCons.Node.FS.default_fs.Dir,
                                            target_factory = SCons.Node.FS.default_fs.Dir,
                                            prefix = '$PACKAGEMAKERPREFIX',
                                            suffix = '$PACKAGEMAKERSUFFIX')

def generate(env):
    env['BUILDERS']['PackageMaker'] = PackageMakerBuilder

    env['PACKAGEMAKER']             = env.Detect('/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker')
    env['PACKAGEMAKERCOM']          = SCons.Action.Action(PackageMakerCom)
    env['PACKAGEMAKERPREFIX']       = ''
    env['PACKAGEMAKERSUFFIX']       = '.pkg'


def exists(env):
    return env.Detect('/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker')

