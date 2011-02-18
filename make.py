#!/usr/bin/env python

import os, commands, re
from fabricate import *

compiler = commands.getoutput('which llvm-g++ || echo -n "g++"')

targets  = {
    'evolve': {
        'sources': ['evolve', 'ga', 'evolvable', 'nn', 'brain', 'unicycle_2d', 'fitness_function', 'unicycle_2d_fitness_function', 'util']
    },
    'simulate': {
        'libs': ['libzmq'],
        'sources': ['simulate', 'zhelpers', 'nn', 'unicycle_2d', 'vector', 'util']
    },
    'nngen': {
        'sources': ['nngen', 'nn', 'util']
    },
    'nn2dot': {
        'sources': ['nn2dot', 'nn', 'nn_dot_printer', 'util']
    },
    'feedforward': {
        'sources': ['feedforward', 'nn', 'util']
    },
    'maw': {
        'libs': ['bullet', 'gl', 'glu', 'glut'],
        'sources': [
            'maw',
            'world_manager',
            'unicycle',
            'vendor/BulletGL/GlutRunner',
            'vendor/BulletGL/DemoApplication',
            'vendor/BulletGL/GlutDemoApplication',
            'vendor/BulletGL/GLShapeDrawer',
            'vendor/BulletGL/GLDebugDrawer',
        ]
    }
}

cflags  = '''-g -O2 -ansi -pedantic-errors -Werror
             -Wall -Wextra -Wconversion -Wshadow -Weffc++
             -Wpointer-arith -Wcast-qual -Wwrite-strings
             -D__USE_FIXED_PROTOTYPES__'''.split()

def cflags_for_target(target):
    flags = []

    if 'libs' in targets[target]:
        flags += commands.getoutput('pkg-config --cflags ' + (' '.join(targets[target]['libs']) + '| sed -e "s/^-I/-isystem/g" -e "s/ -I/ -isystem/g"')).split()

    return flags

def libs_for_target(target):
    if 'libs' in targets[target]:
        return commands.getoutput('pkg-config --libs ' + (' '.join(targets[target]['libs']))).split()
    else:
        return []

def build():
    for target in targets:
        if 'sources' in targets[target]:
            compile(target)
            link(target)

def oname(filename):
    return os.path.join('build', os.path.basename(filename))

def compile(target):
    for source in targets[target]['sources']:
        run(compiler, '-c', 'src/' + source + '.cpp', '-o', oname(source+'.o'), cflags, cflags_for_target(target))

def link(target):
    objects = [oname(s+'.o') for s in targets[target]['sources']]
    run(compiler, objects, '-o', oname(target), libs_for_target(target))

def clean():
    autoclean()

def rebuild():
    clean()
    build()

class QuieterBuilder(Builder):
    def echo_command(self, command):
        compiling = re.compile(r'-c (\S+\.cpp)\b', re.IGNORECASE).search(command)
        linking   = re.compile(r'((?:\S+\.o\s+)+)-o (\S+)\b', re.IGNORECASE).search(command)
        if compiling:
            self.echo('CC ' + compiling.group(1))
        elif linking:
            # self.echo('LD ' + linking.group(1) + '-> ' + linking.group(2))
            self.echo('LD ' + linking.group(2))
        else:
            self.echo(command)

setup(builder=QuieterBuilder)
main()
