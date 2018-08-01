# -*- coding: utf-8 -*-
#-------------------------------------------------------------------------#
#   Copyright (C) 2014 by Christoph Thelen                                #
#   doc_bacardi@users.sourceforge.net                                     #
#                                                                         #
#   This program is free software; you can redistribute it and/or modify  #
#   it under the terms of the GNU General Public License as published by  #
#   the Free Software Foundation; either version 2 of the License, or     #
#   (at your option) any later version.                                   #
#                                                                         #
#   This program is distributed in the hope that it will be useful,       #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#   GNU General Public License for more details.                          #
#                                                                         #
#   You should have received a copy of the GNU General Public License     #
#   along with this program; if not, write to the                         #
#   Free Software Foundation, Inc.,                                       #
#   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
#-------------------------------------------------------------------------#

import os.path


#----------------------------------------------------------------------------
#
# Set up the Muhkuh Build System.
#
SConscript('mbs/SConscript')
Import('atEnv')


#----------------------------------------------------------------------------
#
# Create the compiler environments.
#
# Create a build environment for the ARM9 based netX chips.
env_arm9 = atEnv.DEFAULT.CreateEnvironment(['gcc-arm-none-eabi-4.7', 'asciidoc'])
env_arm9.CreateCompilerEnv('NETX500', ['arch=armv5te'])
env_arm9.CreateCompilerEnv('NETX56', ['arch=armv5te'])
env_arm9.CreateCompilerEnv('NETX50', ['arch=armv5te'])
env_arm9.CreateCompilerEnv('NETX10', ['arch=armv5te'])

# Create a build environment for the Cortex-R7 and Cortex-A9 based netX chips.
env_cortexR7 = atEnv.DEFAULT.CreateEnvironment(['gcc-arm-none-eabi-4.9', 'asciidoc'])
env_cortexR7.CreateCompilerEnv('NETX4000_RELAXED', ['arch=armv7', 'thumb'], ['arch=armv7-r', 'thumb'])

# Create a build environment for the Cortex-M4 based netX chips.
env_cortexM4 = atEnv.DEFAULT.CreateEnvironment(['gcc-arm-none-eabi-4.9', 'asciidoc'])
env_cortexM4.CreateCompilerEnv('NETX90_MPW', ['arch=armv7', 'thumb'], ['arch=armv7e-m', 'thumb'])
env_cortexM4.CreateCompilerEnv('NETX90_MPW_APP', ['arch=armv7', 'thumb'], ['arch=armv7e-m', 'thumb'])


#----------------------------------------------------------------------------
#
# Build the platform library.
#
SConscript('platform/SConscript')


# ----------------------------------------------------------------------------
#
# Get the source code version from the VCS.
#
atEnv.DEFAULT.Version('targets/version/version.h', 'templates/version.h')


# ----------------------------------------------------------------------------
#
# Build all sub-projects.
#
SConscript('iomatrix/SConscript')
Import(
    'IOMATRIX_NETX500',
    'IOMATRIX_NETX90_MPW',
    'IOMATRIX_NETX56',
    'IOMATRIX_NETX10',
    'LUA_NETX_BASE',
    'LUA_NETX90_MPW'
)


# ----------------------------------------------------------------------------
#
# Build the documentation.
#

# Get the default attributes.
aAttribs = atEnv.DEFAULT['ASCIIDOC_ATTRIBUTES']
# Add some custom attributes.
aAttribs.update(dict({
    # Use ASCIIMath formulas.
    'asciimath': True,

    # Embed images into the HTML file as data URIs.
    'data-uri': True,

    # Use icons instead of text for markers and callouts.
    'icons': True,

    # Use numbers in the table of contents.
    'numbered': True,

    # Generate a scrollable table of contents on the left of the text.
    'toc2': True,

    # Use 4 levels in the table of contents.
    'toclevels': 4
}))
tDoc = atEnv.DEFAULT.Asciidoc(
    'targets/doc/org.muhkuh.tests-iomatrix.html',
    'doc/org.muhkuh.tests-iomatrix.asciidoc',
    ASCIIDOC_BACKEND='html5',
    ASCIIDOC_ATTRIBUTES=aAttribs
)

#----------------------------------------------------------------------------
#
# Build the artifacts.
#

aArtifactServer = ('nexus@netx01', 'muhkuh', 'muhkuh_snapshots')
strArtifactGroup = 'tests.muhkuh.org'

aArtifactGroupReverse = strArtifactGroup.split('.')
aArtifactGroupReverse.reverse()


strArtifactId = 'iomatrix'
tArcList = atEnv.DEFAULT.ArchiveList('zip')
tArcList.AddFiles('netx/',
    IOMATRIX_NETX500,
    IOMATRIX_NETX90_MPW,
    IOMATRIX_NETX56,
    IOMATRIX_NETX10)
tArcList.AddFiles('lua/',
    'iomatrix/templates/io_matrix.lua')
tArcList.AddFiles('lua/io_matrix',
    'iomatrix/templates/io_matrix/ftdi_2232h.lua',
    'iomatrix/templates/io_matrix/ftdi.lua',
    LUA_NETX_BASE,
    LUA_NETX90_MPW,
    'iomatrix/templates/io_matrix/netx.lua'
)
#tArcList.AddFiles('templates/',
#    'lua/attributes_template.lua',
#    'lua/ramtest_template.lua',
#    'lua/test.lua',
#    'lua/timing_phase_test_template.lua')
tArcList.AddFiles('doc/',
    tDoc)
tArcList.AddFiles('',
    'ivy/org.muhkuh.tests.iomatrix/install.xml')

strArtifactPath = 'targets/ivy/repository/%s/%s/%s' % ('/'.join(aArtifactGroupReverse),strArtifactId,PROJECT_VERSION)
tArc = atEnv.DEFAULT.Archive(os.path.join(strArtifactPath, '%s-%s.zip' % (strArtifactId,PROJECT_VERSION)), None, ARCHIVE_CONTENTS=tArcList)
tIvy = atEnv.DEFAULT.Version(os.path.join(strArtifactPath, 'ivy-%s.xml' % PROJECT_VERSION), 'ivy/%s.%s/ivy.xml' % ('.'.join(aArtifactGroupReverse),strArtifactId))
tPom = atEnv.DEFAULT.ArtifactVersion(os.path.join(strArtifactPath, '%s-%s.pom' % (strArtifactId,PROJECT_VERSION)), 'ivy/%s.%s/pom.xml' % ('.'.join(aArtifactGroupReverse),strArtifactId))


# Build the artifact list for the deploy operation to bintray.
atEnv.DEFAULT.AddArtifact(tArc, aArtifactServer, strArtifactGroup, strArtifactId, PROJECT_VERSION, 'zip')
atEnv.DEFAULT.AddArtifact(tIvy, aArtifactServer, strArtifactGroup, strArtifactId, PROJECT_VERSION, 'ivy')
tArtifacts = atEnv.DEFAULT.Artifact('targets/artifacts.xml', None)

# Copy the artifacts to a fixed filename to allow a deploy to github.
Command('targets/ivy/%s.zip' % strArtifactId,  tArc,  Copy("$TARGET", "$SOURCE"))
Command('targets/ivy/ivy.xml', tIvy,  Copy("$TARGET", "$SOURCE"))


#----------------------------------------------------------------------------
#
# Make a local demo installation.
#
# Copy all binary binaries.
atFiles = {
    'targets/testbench/netx/iomatrix_netx10.bin': IOMATRIX_NETX10,
#    'targets/testbench/netx/iomatrix_netx50.bin': iomatrix_netx50,
    'targets/testbench/netx/iomatrix_netx56.bin': IOMATRIX_NETX56,
    'targets/testbench/netx/iomatrix_netx90_mpw.bin': IOMATRIX_NETX90_MPW,
    'targets/testbench/netx/iomatrix_netx500.bin': IOMATRIX_NETX500,

    # Copy all LUA scripts.
    'targets/testbench/lua/io_matrix.lua': 'iomatrix/templates/io_matrix.lua',
    'targets/testbench/lua/io_matrix/ftdi_2232h.lua': 'iomatrix/templates/io_matrix/ftdi_2232h.lua',
    'targets/testbench/lua/io_matrix/ftdi.lua': 'iomatrix/templates/io_matrix/ftdi.lua',
    'targets/testbench/lua/io_matrix/netx90_mpw.lua': LUA_NETX90_MPW,
    'targets/testbench/lua/io_matrix/netx_base.lua': LUA_NETX_BASE,
    'targets/testbench/lua/io_matrix/netx.lua': 'iomatrix/templates/io_matrix/netx.lua'
}
for tDst, tSrc in atFiles.iteritems():
    Command(tDst, tSrc, Copy("$TARGET", "$SOURCE"))
