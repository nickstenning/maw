#!/usr/bin/env python

import os, commands, re
from fabricate import *

compiler = commands.getoutput('which llvm-g++ || echo -n "g++"')

targets  = {
    'maw/bindings/_nn.so': {
        'swig': 'nn',
        'sources': [
            'util',
            'nn'
        ]
    },

    'maw/bindings/_unicycle.so': {
        'swig': 'unicycle',
        'libs': ['bullet'],
        'sources': [
            'unicycle',
            'world_manager'
        ]
    },

    'maw/bindings/_world_manager.so': {
        'swig': 'world_manager',
        'libs': ['bullet'],
        'sources': ['world_manager']
    },

    # 'bin/simulate': {
    #     'libs': ['libzmq'],
    #     'sources': ['simulate', 'zhelpers', 'nn', 'unicycle_2d', 'vector', 'util']
    # },

    # 'bin/maw': {
    #     'libs': ['bullet', 'gl', 'glu', 'glut'],
    #     'sources': [
    #         'maw',
    #         'nn',
    #         'world_manager',
    #         'unicycle',
    #         'util',
    #         'vendor/BulletGL/GlutRunner',
    #         'vendor/BulletGL/DemoApplication',
    #         'vendor/BulletGL/GlutDemoApplication',
    #         'vendor/BulletGL/GLShapeDrawer',
    #         'vendor/BulletGL/GLDebugDrawer',
    #     ]
    # }
}

cflags  = '''-g -O2 -fPIC -ansi -pedantic
             -Wall -Wextra -Wconversion -Wshadow -Weffc++
             -Wpointer-arith -Wcast-qual -Wwrite-strings
             -D__USE_FIXED_PROTOTYPES__'''.split()

def cflags_for_target(tgt):
    libs = tgt.get('libs', [])
    stat, out = commands.getstatusoutput('pkg-config --cflags ' + ' '.join(libs))

    # Suppress compiler warnings from system headers
    out = re.sub('\B-I', '-isystem', out)

    return out.split() if stat == 0 else []

def libs_for_target(tgt):
    libs = tgt.get('libs', [])
    stat, out = commands.getstatusoutput('pkg-config --libs ' + ' '.join(libs))

    return out.split() if stat == 0 else []

def build():
    for target_name in targets:
        tgt = targets[target_name]

        if 'swig' in tgt:
            gen_swig(target_name)
            compile_swig(tgt)
            compile(tgt)
            link_swig(target_name)

        elif 'sources' in tgt:
            compile(tgt)
            link(target_name)

def oname(filename):
    return os.path.join('build', os.path.basename(filename))

def compile(tgt):
    for source in tgt['sources']:
        run(compiler, '-c', 'src/' + source + '.cpp', '-o', oname(source+'.o'), cflags, cflags_for_target(tgt))

def link(target_name):
    tgt = targets[target_name]
    objects = [oname(s+'.o') for s in tgt['sources']]
    run(compiler, objects, '-o', target_name, libs_for_target(tgt))

def gen_swig(target_name):
    tgt = targets[target_name]
    run('swig', '-c++', '-python', '-o', 'src/' + tgt['swig'] + '_wrap.cpp', '-outdir', os.path.dirname(target_name), 'src/' + tgt['swig'] + '.i')

def compile_swig(tgt):
    wrap_file = 'src/' + tgt['swig'] + '_wrap.cpp'
    run(compiler, '-c', wrap_file, '-o', oname(tgt['swig'] + '_wrap.o'), '-O2', '-fPIC', '-ansi', '-w', cflags_for_target(tgt), '-I/usr/include/python2.6')

def link_swig(target_name):
    tgt = targets[target_name]
    objects = [oname(s + '.o') for s in tgt['sources']]
    objects.append(oname(tgt['swig'] + '_wrap.o'))
    run(compiler, '-shared', objects, '-o', target_name, libs_for_target(tgt), '-L/usr/lib/python2.6', '-lpython2.6')

def clean():
    autoclean()

def rebuild():
    clean()
    build()

class QuieterBuilder(Builder):
    def echo_command(self, command):
        swigging  = re.compile(r'(\S+\.i)\b', re.IGNORECASE).search(command)
        compiling = re.compile(r'-c (\S+\.cpp)\b', re.IGNORECASE).search(command)
        linking   = re.compile(r'((?:\S+\.o\s+)+)-o (\S+)\b', re.IGNORECASE).search(command)
        if swigging:
            self.echo('SW ' + swigging.group(1))
        elif compiling:
            self.echo('CC ' + compiling.group(1))
        elif linking:
            # self.echo('LD ' + linking.group(1) + '-> ' + linking.group(2))
            self.echo('LD ' + linking.group(2))
        else:
            self.echo(command)

setup(builder=QuieterBuilder)
setup()
main()
