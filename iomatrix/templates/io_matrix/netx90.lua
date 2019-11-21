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
local IoMatrix_netx90 = class(IoMatrix_netx_base)


function IoMatrix_netx90:_init(tLog, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
  self:super(tLog, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
end



IoMatrix_netx90.atKnownPins = {
  ['APPPIO0']  = { type=${PINTYPE_APPPIO}, index=0 },
  ['APPPIO1']  = { type=${PINTYPE_APPPIO}, index=1 },
  ['APPPIO2']  = { type=${PINTYPE_APPPIO}, index=2 },
  ['APPPIO3']  = { type=${PINTYPE_APPPIO}, index=3 },
  ['APPPIO4']  = { type=${PINTYPE_APPPIO}, index=4 },
  ['APPPIO5']  = { type=${PINTYPE_APPPIO}, index=5 },
  ['APPPIO6']  = { type=${PINTYPE_APPPIO}, index=6 },
  ['APPPIO7']  = { type=${PINTYPE_APPPIO}, index=7 },
  ['APPPIO8']  = { type=${PINTYPE_APPPIO}, index=8 },
  ['APPPIO9']  = { type=${PINTYPE_APPPIO}, index=9 },
  ['APPPIO10']  = { type=${PINTYPE_APPPIO}, index=10 },
  ['APPPIO11']  = { type=${PINTYPE_APPPIO}, index=11 },
  ['APPPIO12']  = { type=${PINTYPE_APPPIO}, index=12 },
  ['APPPIO13']  = { type=${PINTYPE_APPPIO}, index=13 },
  ['APPPIO14']  = { type=${PINTYPE_APPPIO}, index=14 },
  ['APPPIO15']  = { type=${PINTYPE_APPPIO}, index=15 },
  ['APPPIO16']  = { type=${PINTYPE_APPPIO}, index=16 },
  ['APPPIO17']  = { type=${PINTYPE_APPPIO}, index=17 },
  ['APPPIO18']  = { type=${PINTYPE_APPPIO}, index=18 },
  ['APPPIO19']  = { type=${PINTYPE_APPPIO}, index=19 },
  ['APPPIO20']  = { type=${PINTYPE_APPPIO}, index=20 },
  ['APPPIO21']  = { type=${PINTYPE_APPPIO}, index=21 },
  ['APPPIO22']  = { type=${PINTYPE_APPPIO}, index=22 },
  ['APPPIO23']  = { type=${PINTYPE_APPPIO}, index=23 },
  ['APPPIO24']  = { type=${PINTYPE_APPPIO}, index=24 },
  ['APPPIO25']  = { type=${PINTYPE_APPPIO}, index=25 },
  ['APPPIO26']  = { type=${PINTYPE_APPPIO}, index=26 },
  ['APPPIO27']  = { type=${PINTYPE_APPPIO}, index=27 },
  ['APPPIO28']  = { type=${PINTYPE_APPPIO}, index=28 },
  ['APPPIO29']  = { type=${PINTYPE_APPPIO}, index=29 },
  ['APPPIO30']  = { type=${PINTYPE_APPPIO}, index=30 },
  ['APPPIO31']  = { type=${PINTYPE_APPPIO}, index=31 },

  ['GPIO8']  = { type=${PINTYPE_GPIO}, index=8 },
  ['GPIO9']  = { type=${PINTYPE_GPIO}, index=9 },
  ['GPIO10'] = { type=${PINTYPE_GPIO}, index=10 },
  ['GPIO11'] = { type=${PINTYPE_GPIO}, index=11 },

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
  ['HIF_A0']    = { type=${PINTYPE_HIFPIO}, index=16 },
  ['HIF_A1']    = { type=${PINTYPE_HIFPIO}, index=17 },
  ['HIF_A2']    = { type=${PINTYPE_HIFPIO}, index=18 },
  ['HIF_A3']    = { type=${PINTYPE_HIFPIO}, index=19 },
  ['HIF_A4']    = { type=${PINTYPE_HIFPIO}, index=20 },
  ['HIF_A5']    = { type=${PINTYPE_HIFPIO}, index=21 },
  ['HIF_A6']    = { type=${PINTYPE_HIFPIO}, index=22 },
  ['HIF_A7']    = { type=${PINTYPE_HIFPIO}, index=23 },
  ['HIF_A8']    = { type=${PINTYPE_HIFPIO}, index=24 },
  ['HIF_A9']    = { type=${PINTYPE_HIFPIO}, index=25 },
  ['HIF_A10']   = { type=${PINTYPE_HIFPIO}, index=26 },
  ['HIF_A11']   = { type=${PINTYPE_HIFPIO}, index=27 },
  ['HIF_A12']   = { type=${PINTYPE_HIFPIO}, index=28 },
  ['HIF_A13']   = { type=${PINTYPE_HIFPIO}, index=29 },
  ['HIF_A14']   = { type=${PINTYPE_HIFPIO}, index=30 },
  ['HIF_A15']   = { type=${PINTYPE_HIFPIO}, index=31 },
  ['HIF_A16']   = { type=${PINTYPE_HIFPIO}, index=32 },
  ['HIF_A17']   = { type=${PINTYPE_HIFPIO}, index=33 },
  ['HIF_BHEN']  = { type=${PINTYPE_HIFPIO}, index=34 },
  ['HIF_RDN']   = { type=${PINTYPE_HIFPIO}, index=35 },
  ['HIF_WRN']   = { type=${PINTYPE_HIFPIO}, index=36 },
  ['HIF_CSN']   = { type=${PINTYPE_HIFPIO}, index=37 },
  ['HIF_RDY']   = { type=${PINTYPE_HIFPIO}, index=38 },
  ['HIF_DIRQ']  = { type=${PINTYPE_HIFPIO}, index=39 },
  ['HIF_SDCLK'] = { type=${PINTYPE_HIFPIO}, index=40 },

  ['MLED0'] = { type=${PINTYPE_MLED}, index=0 },
  ['MLED1'] = { type=${PINTYPE_MLED}, index=1 },
  ['MLED2'] = { type=${PINTYPE_MLED}, index=2 },
  ['MLED3'] = { type=${PINTYPE_MLED}, index=3 },
  ['MLED4'] = { type=${PINTYPE_MLED}, index=4 },
  ['MLED5'] = { type=${PINTYPE_MLED}, index=5 },
  ['MLED6'] = { type=${PINTYPE_MLED}, index=6 },
  ['MLED7'] = { type=${PINTYPE_MLED}, index=7 },

  ['MMIO0'] = { type=${PINTYPE_MMIO}, index=0 },
  ['MMIO1'] = { type=${PINTYPE_MMIO}, index=1 },
  ['MMIO2'] = { type=${PINTYPE_MMIO}, index=2 },
  ['MMIO3'] = { type=${PINTYPE_MMIO}, index=3 },
  ['MMIO4'] = { type=${PINTYPE_MMIO}, index=4 },
  ['MMIO5'] = { type=${PINTYPE_MMIO}, index=5 },
  ['MMIO6'] = { type=${PINTYPE_MMIO}, index=6 },
  ['MMIO7'] = { type=${PINTYPE_MMIO}, index=7 },

  ['XM0_IO0'] = { type=${PINTYPE_XMIO}, index=0 },
  ['XM0_IO1'] = { type=${PINTYPE_XMIO}, index=1 },
  ['XM0_IO2'] = { type=${PINTYPE_XMIO}, index=2 },
  ['XM0_IO3'] = { type=${PINTYPE_XMIO}, index=3 },
  ['XM0_IO4'] = { type=${PINTYPE_XMIO}, index=4 },
  ['XM0_IO5'] = { type=${PINTYPE_XMIO}, index=5 },
  ['XM1_IO0'] = { type=${PINTYPE_XMIO}, index=6 },
  ['XM1_IO1'] = { type=${PINTYPE_XMIO}, index=7 },
  ['XM1_IO2'] = { type=${PINTYPE_XMIO}, index=8 },
  ['XM1_IO3'] = { type=${PINTYPE_XMIO}, index=9 },
  ['XM1_IO4'] = { type=${PINTYPE_XMIO}, index=10 },
  ['XM1_IO5'] = { type=${PINTYPE_XMIO}, index=11 },

  ['RDY'] = { type=${PINTYPE_RDYRUN}, index=0 },
  ['RUN'] = { type=${PINTYPE_RDYRUN}, index=1 }
}



return IoMatrix_netx90
