-----------------------------------------------------------------------------
--   Copyright (C) 2018 by Christoph Thelen                                --
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

local class = require 'pl.class'
local IoMatrix_netx_base = require 'io_matrix.netx_base'
local IoMatrix_netx90_mpw = class(IoMatrix_netx_base)


function IoMatrix_netx90_mpw:_init(tLog, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
  self:super(tLog, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
end



IoMatrix_netx90_mpw.atKnownPins = {
  ['MMIO0'] = { type=2, index=0 },
  ['MMIO1'] = { type=2, index=1 },
  ['MMIO2'] = { type=2, index=2 },
  ['MMIO3'] = { type=2, index=3 },
  ['MMIO4'] = { type=2, index=4 },
  ['MMIO5'] = { type=2, index=5 },
  ['MMIO6'] = { type=2, index=6 },
  ['MMIO7'] = { type=2, index=7 }
}



return IoMatrix_netx90_mpw
