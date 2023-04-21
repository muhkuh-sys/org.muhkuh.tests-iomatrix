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
local IoMatrix_netx56 = class(IoMatrix_netx_base)


function IoMatrix_netx56:_init(tLog, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
  self:super(tLog, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
end


-- Documentation of netx56 - https://hilscher.atlassian.net/wiki/spaces/DL/pages/77727855/netX+51+52
IoMatrix_netx56.atKnownPins = {

  ['HIF_D0']    = { type=${PINTYPE_HIFPIO}, index=0 },
  ['HIF_D1']    = { type=${PINTYPE_HIFPIO}, index=1 },
  ['HIF_D2']    = { type=${PINTYPE_HIFPIO}, index=2 },
  ['HIF_D3']    = { type=${PINTYPE_HIFPIO}, index=3 },
  ['HIF_D4']    = { type=${PINTYPE_HIFPIO}, index=4 },
  ['HIF_D5']    = { type=${PINTYPE_HIFPIO}, index=5 },
  ['HIF_D6']    = { type=${PINTYPE_HIFPIO}, index=6 },
  ['HIF_D7']    = { type=${PINTYPE_HIFPIO}, index=7 },
  ['HIF_D8']    = { type=${PINTYPE_HIFPIO}, index=8 },
  ['HIF_D9']    = { type=${PINTYPE_HIFPIO}, index=9 },
  ['HIF_D10']   = { type=${PINTYPE_HIFPIO}, index=10 },
  ['HIF_D11']   = { type=${PINTYPE_HIFPIO}, index=11 },
  ['HIF_D12']   = { type=${PINTYPE_HIFPIO}, index=12 },
  ['HIF_D13']   = { type=${PINTYPE_HIFPIO}, index=13 },
  ['HIF_D14']   = { type=${PINTYPE_HIFPIO}, index=14 },
  ['HIF_D15']   = { type=${PINTYPE_HIFPIO}, index=15 },
  ['HIF_D16']   = { type=${PINTYPE_HIFPIO}, index=16 },
  ['HIF_D17']   = { type=${PINTYPE_HIFPIO}, index=17 },
  ['HIF_D18']   = { type=${PINTYPE_HIFPIO}, index=18 },
  ['HIF_D19']   = { type=${PINTYPE_HIFPIO}, index=19 },
  ['HIF_D20']   = { type=${PINTYPE_HIFPIO}, index=20 },
  ['HIF_D21']   = { type=${PINTYPE_HIFPIO}, index=21 },
  ['HIF_D22']   = { type=${PINTYPE_HIFPIO}, index=22 },
  ['HIF_D23']   = { type=${PINTYPE_HIFPIO}, index=23 },
  ['HIF_D24']   = { type=${PINTYPE_HIFPIO}, index=24 },
  ['HIF_D25']   = { type=${PINTYPE_HIFPIO}, index=25 },
  ['HIF_D26']   = { type=${PINTYPE_HIFPIO}, index=26 },
  ['HIF_D27']   = { type=${PINTYPE_HIFPIO}, index=27 },
  ['HIF_D28']   = { type=${PINTYPE_HIFPIO}, index=28 },
  ['HIF_D29']   = { type=${PINTYPE_HIFPIO}, index=29 },
  ['HIF_D30']   = { type=${PINTYPE_HIFPIO}, index=30 },
  ['HIF_D31']   = { type=${PINTYPE_HIFPIO}, index=31 },

  ['HIF_A0']    = { type=${PINTYPE_HIFPIO}, index=32 },
  ['HIF_A1']    = { type=${PINTYPE_HIFPIO}, index=33 },
  ['HIF_A2']    = { type=${PINTYPE_HIFPIO}, index=34 },
  ['HIF_A3']    = { type=${PINTYPE_HIFPIO}, index=35 },
  ['HIF_A4']    = { type=${PINTYPE_HIFPIO}, index=36 },
  ['HIF_A5']    = { type=${PINTYPE_HIFPIO}, index=37 },
  ['HIF_A6']    = { type=${PINTYPE_HIFPIO}, index=38 },
  ['HIF_A7']    = { type=${PINTYPE_HIFPIO}, index=39 },
  ['HIF_A8']    = { type=${PINTYPE_HIFPIO}, index=40 },
  ['HIF_A9']    = { type=${PINTYPE_HIFPIO}, index=41 },
  ['HIF_A10']   = { type=${PINTYPE_HIFPIO}, index=42 },
  ['HIF_A11']   = { type=${PINTYPE_HIFPIO}, index=43 },
  ['HIF_A12']   = { type=${PINTYPE_HIFPIO}, index=44 },
  ['HIF_A13']   = { type=${PINTYPE_HIFPIO}, index=45 },
  ['HIF_A14']   = { type=${PINTYPE_HIFPIO}, index=46 },
  ['HIF_A15']   = { type=${PINTYPE_HIFPIO}, index=47 },

  ['HIF_AHI0']  = { type=${PINTYPE_HIFPIO}, index=48 },
  ['HIF_AHI1']  = { type=${PINTYPE_HIFPIO}, index=49 },

  ['HIF_BHE3']  = { type=${PINTYPE_HIFPIO}, index=50 },
  ['HIF_BHE1']  = { type=${PINTYPE_HIFPIO}, index=51 },
  ['HIF_RDN']   = { type=${PINTYPE_HIFPIO}, index=52 },
  ['HIF_WRN']   = { type=${PINTYPE_HIFPIO}, index=53 },
  ['HIF_CSN']   = { type=${PINTYPE_HIFPIO}, index=54 },
  ['HIF_RDY']   = { type=${PINTYPE_HIFPIO}, index=55 },
  ['HIF_DIRQ']  = { type=${PINTYPE_HIFPIO}, index=56 },
  ['HIF_SDCLK'] = { type=${PINTYPE_HIFPIO}, index=57 },


  ['MMIO0']     = { type=${PINTYPE_MMIO}, index=0 },
  ['MMIO1']     = { type=${PINTYPE_MMIO}, index=1 },
  ['MMIO2']     = { type=${PINTYPE_MMIO}, index=2 },
  ['MMIO3']     = { type=${PINTYPE_MMIO}, index=3 },
  ['MMIO4']     = { type=${PINTYPE_MMIO}, index=4 },
  ['MMIO5']     = { type=${PINTYPE_MMIO}, index=5 },
  ['MMIO6']     = { type=${PINTYPE_MMIO}, index=6 },
  ['MMIO7']     = { type=${PINTYPE_MMIO}, index=7 },
  ['MMIO8']     = { type=${PINTYPE_MMIO}, index=8 },
  ['MMIO9']     = { type=${PINTYPE_MMIO}, index=9 },
  ['MMIO10']    = { type=${PINTYPE_MMIO}, index=10 },
  ['MMIO11']    = { type=${PINTYPE_MMIO}, index=11 },
  ['MMIO12']    = { type=${PINTYPE_MMIO}, index=12 },
  ['MMIO13']    = { type=${PINTYPE_MMIO}, index=13 },
  ['MMIO14']    = { type=${PINTYPE_MMIO}, index=14 },
  ['MMIO15']    = { type=${PINTYPE_MMIO}, index=15 },
  ['MMIO16']    = { type=${PINTYPE_MMIO}, index=16 },
  ['MMIO17']    = { type=${PINTYPE_MMIO}, index=17 },
  ['MMIO18']    = { type=${PINTYPE_MMIO}, index=18 },
  ['MMIO19']    = { type=${PINTYPE_MMIO}, index=19 },
  ['MMIO20']    = { type=${PINTYPE_MMIO}, index=20 },
  ['MMIO21']    = { type=${PINTYPE_MMIO}, index=21 },
  ['MMIO22']    = { type=${PINTYPE_MMIO}, index=22 },
  ['MMIO23']    = { type=${PINTYPE_MMIO}, index=23 },
  ['MMIO24']    = { type=${PINTYPE_MMIO}, index=24 },
  ['MMIO25']    = { type=${PINTYPE_MMIO}, index=25 },
  ['MMIO26']    = { type=${PINTYPE_MMIO}, index=26 },
  ['MMIO27']    = { type=${PINTYPE_MMIO}, index=27 },
  ['MMIO28']    = { type=${PINTYPE_MMIO}, index=28 },
  ['MMIO29']    = { type=${PINTYPE_MMIO}, index=29 },
  ['MMIO30']    = { type=${PINTYPE_MMIO}, index=30 },
  ['MMIO31']    = { type=${PINTYPE_MMIO}, index=31 },

  ['MMIO32']    = { type=${PINTYPE_MMIO}, index=32 },
  ['MMIO33']    = { type=${PINTYPE_MMIO}, index=33 },
  ['MMIO34']    = { type=${PINTYPE_MMIO}, index=34 },
  ['MMIO35']    = { type=${PINTYPE_MMIO}, index=35 },
  ['MMIO36']    = { type=${PINTYPE_MMIO}, index=36 },
  ['MMIO37']    = { type=${PINTYPE_MMIO}, index=37 },
  ['MMIO38']    = { type=${PINTYPE_MMIO}, index=38 },
  ['MMIO39']    = { type=${PINTYPE_MMIO}, index=39 },

  ['MMIO48']    = { type=${PINTYPE_MMIO}, index=48 },

  ['RDY']       = { type=${PINTYPE_RDYRUN}, index=0 },
  ['RUN']       = { type=${PINTYPE_RDYRUN}, index=1 },
}

return IoMatrix_netx56
