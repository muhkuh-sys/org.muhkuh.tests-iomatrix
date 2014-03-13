module("io_matrix", package.seeall)

-----------------------------------------------------------------------------
--   Copyright (C) 2014 by Christoph Thelen                                --
--   doc_bacardi@users.sourceforge.net                                     --
--                                                                         --
--   This program is free software; you can redistribute it and/or modify  --
--   it under the terms of the GNU General Public License as published by  --
--   the Free Software Foundation; either version 2 of the License, or     --
--   (at your option) any later version.                                   --
--                                                                         --
--   This program is distributed in the hope that it will be useful,       --
--   but WITHOUT ANY WARRANTY; without even the implied warranty of        --
--   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         --
--   GNU General Public License for more details.                          --
--                                                                         --
--   You should have received a copy of the GNU General Public License     --
--   along with this program; if not, write to the                         --
--   Free Software Foundation, Inc.,                                       --
--   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             --
-----------------------------------------------------------------------------

require("bit")
require("muhkuh")
require("romloader")


-----------------------------------------------------------------------------
--                           Definitions                                   --
-----------------------------------------------------------------------------

IOMATRIX_VERSION_MAJ  = ${VERSION_MAJ}
IOMATRIX_VERSION_MIN  = ${VERSION_MIN}
IOMATRIX_VERSION_VCS  = ${VERSION_VCS}
IOMATRIX_VERSION_ALL  = ${VERSION_ALL}


PINTYPE_GPIO       = ${PINTYPE_GPIO}
PINTYPE_PIO        = ${PINTYPE_PIO}
PINTYPE_MMIO       = ${PINTYPE_MMIO}
PINTYPE_HIFPIO     = ${PINTYPE_HIFPIO}
PINTYPE_RSTOUT     = ${PINTYPE_RSTOUT}

PINSTATUS_HIGHZ    = ${PINSTATUS_HIGHZ}
PINSTATUS_OUTPUT0  = ${PINSTATUS_OUTPUT0}
PINSTATUS_OUTPUT1  = ${PINSTATUS_OUTPUT1}

PINFLAG_I          = ${PINFLAG_I}
PINFLAG_O          = ${PINFLAG_O}
PINFLAG_Z          = ${PINFLAG_Z}
PINFLAG_IOZ        = ${PINFLAG_IOZ}


