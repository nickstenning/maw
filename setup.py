from setuptools import setup, find_packages

setup(
    name = "maw",

    version = "0.1",

    packages = find_packages(),

    package_data = {
        'maw': ['simulate/media/*'],
    },

    install_requires = [
        "pyglet",
    ],

    entry_points = {
        'console_scripts': [
            'nngen = maw.command.nngen:main',
            'nn2dot = maw.command.nn2dot:main',
            'simulate_unicycle = maw.command.simulate_unicycle:main',
        ],
    }
)
