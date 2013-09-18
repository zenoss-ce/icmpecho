#! /usr/bin/env python

##############################################################################
# 
# Copyright (C) Zenoss, Inc. 2011, all rights reserved.
# 
# This content is made available according to terms specified in
# License.zenoss under the directory where your Zenoss product is installed.
# 
##############################################################################


from distutils.core import setup, Extension

setup (name='icmpecho',
       version='1.0',
       description='Packet encode/decode of ICMPv4/6 echo and reply.',
       package_dir={'icmpecho': 'src'},
       packages=['icmpecho'],
       ext_modules=[Extension('icmpecho._network', sources=['src/_network.c'])],
       )
