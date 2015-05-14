# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
VERSION = '0.1'
APPNAME = 'CP_API_Test'

from waflib import Build, Logs, Utils, Task, TaskGen, Configure
from waflib import Options

def options(opt):
    opt.load('compiler_c compiler_cxx gnu_dirs')
    opt.load('boost default-compiler-flags', tooldir=['.waf-tools'])

    opt.add_option('-l', '--log', dest="log", default='', help='log level', action='store')

    ropt = opt.add_option_group('con_pro_repo Options')

    ropt.add_option('--with-tests', action='store_true', default=False, dest='with_tests',
                    help='''build unit tests''')

    ropt.add_option('--without-tools', action='store_false', default=True, dest='with_tools',
                    help='''Do not build tools''')
    ropt.add_option('--with-examples', action='store_true', default=False, dest='with_examples',
                    help='''Build examples''')

def configure(conf):
    conf.load("compiler_c compiler_cxx gnu_dirs boost default-compiler-flags")

    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'],
                   uselib_store='NDN_CXX', mandatory=True)

    conf.env.FRAMEWORK = ['IOKit']

    if conf.options.with_tests:
        conf.env['WITH_TESTS'] = True

    conf.env['WITH_TOOLS'] = conf.options.with_tools
    conf.env['WITH_EXAMPLES'] = conf.options.with_examples

    USED_BOOST_LIBS = ['system', 'iostreams', 'filesystem', 'random']
    if conf.env['WITH_TESTS']:
        USED_BOOST_LIBS += ['unit_test_framework']
    conf.check_boost(lib=USED_BOOST_LIBS, mandatory=True)

    _enable_log(conf)       #log level

    try:
        conf.load("doxygen")
    except:
        pass

#    conf.write_config_header('src/config.hpp')

def build(bld):

    bld(target="time_space_pro",
    features=["cxx", "cxxprogram"],
    source= "src/time_space_pro.cpp",
    use='NDN_CXX',
    )

    bld(target="time_space_con",
    features=["cxx", "cxxprogram"],
    source= "src/time_space_con.cpp",
    use='NDN_CXX',
    )

    bld(target="ts_random_con",
    features=["cxx", "cxxprogram"],
    source= "src/ts_random_con.cpp",
    use='NDN_CXX',
    )
    
    bld(target="ts_real_ran_con",
    features=["cxx", "cxxprogram"],
    source= "src/ts_real_ran_con.cpp",
    use='NDN_CXX',
    )

    bld(target="ts_real_win_con",
    features=["cxx", "cxxprogram"],
    source= "src/ts_real_win_con.cpp",
    use='NDN_CXX',
    )

    # Tests
#    bld.recurse('tests')

    # Tools
#    bld.recurse('tools')

#    bld.recurse("examples")

#    bld.install_files('${SYSCONFDIR}/ndn', 'repo-ng.conf.sample')
def _enable_log(conf):
    if Options.options.log == 'trace':
        Logs.pprint("PINK", "Log level set to trace")
        conf.env.append_value("CFLAGS", "-DLOG_LEVEL=6")
        conf.env.append_value("CXXFLAGS", "-DLOG_LEVEL=6")
    elif Options.options.log == 'debug':
        Logs.pprint("PINK", "Log level set to debug")
        conf.env.append_value("CFLAGS", "-DLOG_LEVEL=5")
        conf.env.append_value("CXXFLAGS", "-DLOG_LEVEL=5")
    elif Options.options.log == 'info':
        Logs.pprint("PINK", "Log level set to info")
        conf.env.append_value("CFLAGS", "-DLOG_LEVEL=4")
        conf.env.append_value("CXXFLAGS", "-DLOG_LEVEL=4")
    elif Options.options.log == '':
        pass
    else:
        Logs.pprint("PINK", "unsupported log level")

