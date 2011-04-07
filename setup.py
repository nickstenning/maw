from setuptools import setup, find_packages, Extension
from commands import getoutput

def pc_cflags(name):
    return getoutput('pkg-config --cflags ' + name).split(' ')

def pc_libs(name):
    return getoutput('pkg-config --libs ' + name).split(' ')

common_opts = {
    'swig_opts': ['-c++', '-outdir', 'maw/bindings']
}

nn_module = Extension(
    'maw.bindings._nn',
    sources=['src/nn.i', 'src/util.cpp', 'src/nn.cpp'],
    **common_opts
)

pendulum_module = Extension(
    'maw.bindings._pendulum',
    sources=['src/pendulum.i', 'src/pendulum.cpp'],
    **common_opts
)

unicycle_2d_module = Extension(
    'maw.bindings._unicycle_2d',
    sources=['src/unicycle_2d.i', 'src/util.cpp', 'src/vector.cpp', 'src/unicycle_2d.cpp'],
    **common_opts
)

unicycle_module = Extension(
    'maw.bindings._unicycle',
    extra_compile_args=pc_cflags('bullet'),
    extra_link_args=pc_libs('bullet'),
    sources=['src/unicycle.i', 'src/util.cpp', 'src/world_manager.cpp', 'src/unicycle.cpp'],
    **common_opts
)

world_manager_module = Extension(
    'maw.bindings._world_manager',
    extra_compile_args=pc_cflags('bullet'),
    extra_link_args=pc_libs('bullet'),
    sources=['src/world_manager.i', 'src/world_manager.cpp'],
    **common_opts
)

maw_simulator_module = Extension(
    'maw.bindings._maw_simulator',
    extra_compile_args=pc_cflags('bullet gl glu glut'),
    extra_link_args=pc_libs('bullet gl glu glut'),
    sources=[
        'src/maw_simulator.i',
        'src/util.cpp',
        'src/nn.cpp',
        'src/world_manager.cpp',
        'src/unicycle.cpp',
        'src/maw_simulator.cpp',
        'src/vendor/BulletGL/GlutRunner.cpp',
        'src/vendor/BulletGL/DemoApplication.cpp',
        'src/vendor/BulletGL/GlutDemoApplication.cpp',
        'src/vendor/BulletGL/GLShapeDrawer.cpp',
        'src/vendor/BulletGL/GLDebugDrawer.cpp',
    ],
    **common_opts
)

setup(
    name = 'maw',

    version = '0.1',

    packages = find_packages(),

    package_data = {
        'maw': ['simulate/media/*'],
    },

    install_requires = [
        'pyglet',
    ],

    ext_modules = [
        nn_module,
        unicycle_module,
        unicycle_2d_module,
        pendulum_module,
        world_manager_module,
        maw_simulator_module,
    ],

    entry_points = {
        'console_scripts': [
            'nngen = maw.command.nngen:main',
            'nnfeed = maw.command.nnfeed:main',
            'nn2dot = maw.command.nn2dot:main',
            'evolve = maw.command.evolve:main',
            'simulate_unicycle = maw.command.simulate_unicycle:main',
        ],
    }
)
