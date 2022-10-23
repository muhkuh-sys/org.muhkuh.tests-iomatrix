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
local IoMatrix_netx500 = class(IoMatrix_netx_base)


function IoMatrix_netx500:_init(tLog, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
  self:super(tLog, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
end


-- Documentation of netx100/500 - https://kb.hilscher.com/pages/viewpage.action?pageId=25868147
IoMatrix_netx500.atKnownPins = {
  ['GPIO0']       = { type=${PINTYPE_GPIO}, index=0 },
  ['GPIO1']       = { type=${PINTYPE_GPIO}, index=1 },
  ['GPIO2']       = { type=${PINTYPE_GPIO}, index=2 },
  ['GPIO3']       = { type=${PINTYPE_GPIO}, index=3 },
  ['GPIO4']       = { type=${PINTYPE_GPIO}, index=4 },
  ['GPIO5']       = { type=${PINTYPE_GPIO}, index=5 },
  ['GPIO6']       = { type=${PINTYPE_GPIO}, index=6 },
  ['GPIO7']       = { type=${PINTYPE_GPIO}, index=7 },
  ['GPIO8']       = { type=${PINTYPE_GPIO}, index=8 },
  ['GPIO9']       = { type=${PINTYPE_GPIO}, index=9 },
  ['GPIO10']      = { type=${PINTYPE_GPIO}, index=10 },
  ['GPIO11']      = { type=${PINTYPE_GPIO}, index=11 },
  ['GPIO12']      = { type=${PINTYPE_GPIO}, index=12 },
  ['GPIO13']      = { type=${PINTYPE_GPIO}, index=13 },
  ['GPIO14']      = { type=${PINTYPE_GPIO}, index=14 },
  ['GPIO15']      = { type=${PINTYPE_GPIO}, index=15 },

  -- The description of HIFPIO pins depends on the PIO description -  see: http://netx01/pub/dreckdef/netx500/
  -- netX_HIF_Pinmultiplexing_and_config_bits.xls
  ['HIFPIO_32']   = { type=${PINTYPE_HIFPIO}, index=0 },
  ['HIFPIO_33']   = { type=${PINTYPE_HIFPIO}, index=1 },
  ['HIFPIO_34']   = { type=${PINTYPE_HIFPIO}, index=2 },
  ['HIFPIO_35']   = { type=${PINTYPE_HIFPIO}, index=3 },
  ['HIFPIO_36']   = { type=${PINTYPE_HIFPIO}, index=4 },
  ['HIFPIO_37']   = { type=${PINTYPE_HIFPIO}, index=5 },
  ['HIFPIO_38']   = { type=${PINTYPE_HIFPIO}, index=6 },
  ['HIFPIO_39']   = { type=${PINTYPE_HIFPIO}, index=7 },
  ['HIFPIO_40']   = { type=${PINTYPE_HIFPIO}, index=8 },
  ['HIFPIO_41']   = { type=${PINTYPE_HIFPIO}, index=9 },
  ['HIFPIO_42']   = { type=${PINTYPE_HIFPIO}, index=10 },
  ['HIFPIO_43']   = { type=${PINTYPE_HIFPIO}, index=11 },
  ['HIFPIO_44']   = { type=${PINTYPE_HIFPIO}, index=12 },
  ['HIFPIO_45']   = { type=${PINTYPE_HIFPIO}, index=13 },
  ['HIFPIO_46']   = { type=${PINTYPE_HIFPIO}, index=14 },
  ['HIFPIO_47']   = { type=${PINTYPE_HIFPIO}, index=15 },
  ['HIFPIO_48']   = { type=${PINTYPE_HIFPIO}, index=16 },
  ['HIFPIO_49']   = { type=${PINTYPE_HIFPIO}, index=17 },
  ['HIFPIO_50']   = { type=${PINTYPE_HIFPIO}, index=18 },
  ['HIFPIO_51']   = { type=${PINTYPE_HIFPIO}, index=19 },
  ['HIFPIO_52']   = { type=${PINTYPE_HIFPIO}, index=20 },
  ['HIFPIO_53']   = { type=${PINTYPE_HIFPIO}, index=21 },
  ['HIFPIO_54']   = { type=${PINTYPE_HIFPIO}, index=22 },
  ['HIFPIO_55']   = { type=${PINTYPE_HIFPIO}, index=23 },
  ['HIFPIO_56']   = { type=${PINTYPE_HIFPIO}, index=24 },
  ['HIFPIO_57']   = { type=${PINTYPE_HIFPIO}, index=25 },
  ['HIFPIO_58']   = { type=${PINTYPE_HIFPIO}, index=26 },
  ['HIFPIO_59']   = { type=${PINTYPE_HIFPIO}, index=27 },
  ['HIFPIO_60']   = { type=${PINTYPE_HIFPIO}, index=28 },
  ['HIFPIO_61']   = { type=${PINTYPE_HIFPIO}, index=29 },
  ['HIFPIO_62']   = { type=${PINTYPE_HIFPIO}, index=30 },
  ['HIFPIO_63']   = { type=${PINTYPE_HIFPIO}, index=31 },
  ['HIFPIO_64']   = { type=${PINTYPE_HIFPIO}, index=32 },
  ['HIFPIO_65']   = { type=${PINTYPE_HIFPIO}, index=33 },
  ['HIFPIO_66']   = { type=${PINTYPE_HIFPIO}, index=34 },
  ['HIFPIO_67']   = { type=${PINTYPE_HIFPIO}, index=35 },
  ['HIFPIO_68']   = { type=${PINTYPE_HIFPIO}, index=36 },
  ['HIFPIO_69']   = { type=${PINTYPE_HIFPIO}, index=37 },
  ['HIFPIO_70']   = { type=${PINTYPE_HIFPIO}, index=38 },
  ['HIFPIO_71']   = { type=${PINTYPE_HIFPIO}, index=39 },
  ['HIFPIO_72']   = { type=${PINTYPE_HIFPIO}, index=40 },
  ['HIFPIO_73']   = { type=${PINTYPE_HIFPIO}, index=41 },
  ['HIFPIO_74']   = { type=${PINTYPE_HIFPIO}, index=42 },
  ['HIFPIO_75']   = { type=${PINTYPE_HIFPIO}, index=43 },
  ['HIFPIO_76']   = { type=${PINTYPE_HIFPIO}, index=44 },
  ['HIFPIO_77']   = { type=${PINTYPE_HIFPIO}, index=45 },
  ['HIFPIO_78']   = { type=${PINTYPE_HIFPIO}, index=46 },
  ['HIFPIO_79']   = { type=${PINTYPE_HIFPIO}, index=47 },
  ['HIFPIO_80']   = { type=${PINTYPE_HIFPIO}, index=48 },
  ['HIFPIO_81']   = { type=${PINTYPE_HIFPIO}, index=49 },
  ['HIFPIO_82']   = { type=${PINTYPE_HIFPIO}, index=50 },
  ['HIFPIO_83']   = { type=${PINTYPE_HIFPIO}, index=51 },
  ['HIFPIO_84']   = { type=${PINTYPE_HIFPIO}, index=52 },

  ['PIO_0']       = { type=${PINTYPE_PIO}, index=0 },
  ['PIO_1']       = { type=${PINTYPE_PIO}, index=1 },
  ['PIO_2']       = { type=${PINTYPE_PIO}, index=2 },
  ['PIO_3']       = { type=${PINTYPE_PIO}, index=3 },
  ['PIO_4']       = { type=${PINTYPE_PIO}, index=4 },
  ['PIO_5']       = { type=${PINTYPE_PIO}, index=5 },
  ['PIO_6']       = { type=${PINTYPE_PIO}, index=6 },
  ['PIO_7']       = { type=${PINTYPE_PIO}, index=7 },
  ['PIO_8']       = { type=${PINTYPE_PIO}, index=8 },
  ['PIO_9']       = { type=${PINTYPE_PIO}, index=9 },
  ['PIO_10']      = { type=${PINTYPE_PIO}, index=10 },
  ['PIO_11']      = { type=${PINTYPE_PIO}, index=11 },
  ['PIO_12']      = { type=${PINTYPE_PIO}, index=12 },
  ['PIO_13']      = { type=${PINTYPE_PIO}, index=13 },
  ['PIO_14']      = { type=${PINTYPE_PIO}, index=14 },
  ['PIO_15']      = { type=${PINTYPE_PIO}, index=15 },
  ['PIO_16']      = { type=${PINTYPE_PIO}, index=16 },
  ['PIO_17']      = { type=${PINTYPE_PIO}, index=17 },
  ['PIO_18']      = { type=${PINTYPE_PIO}, index=18 },
  ['PIO_19']      = { type=${PINTYPE_PIO}, index=19 },
  ['PIO_20']      = { type=${PINTYPE_PIO}, index=20 },
  ['PIO_21']      = { type=${PINTYPE_PIO}, index=21 },
  ['PIO_22']      = { type=${PINTYPE_PIO}, index=22 },
  ['PIO_23']      = { type=${PINTYPE_PIO}, index=23 },
  ['PIO_24']      = { type=${PINTYPE_PIO}, index=24 },
  ['PIO_25']      = { type=${PINTYPE_PIO}, index=25 },
  ['PIO_26']      = { type=${PINTYPE_PIO}, index=26 },
  ['PIO_27']      = { type=${PINTYPE_PIO}, index=27 },
  ['PIO_28']      = { type=${PINTYPE_PIO}, index=28 },
  ['PIO_29']      = { type=${PINTYPE_PIO}, index=29 },
  ['PIO_30']      = { type=${PINTYPE_PIO}, index=30 },
  ['PIO_31']      = { type=${PINTYPE_PIO}, index=31 },

  ['RDY']         = { type=${PINTYPE_RDYRUN}, index=0 },
  ['RUN']         = { type=${PINTYPE_RDYRUN}, index=1 },

  ['RST_OUT']     = { type=${PINTYPE_RSTOUT}, index=0 },

  ['XM0_IO0'] = { type=${PINTYPE_XMIO}, index=0 },
  ['XM0_IO1'] = { type=${PINTYPE_XMIO}, index=1 },
  ['XM1_IO0'] = { type=${PINTYPE_XMIO}, index=2 },
  ['XM1_IO1'] = { type=${PINTYPE_XMIO}, index=3 },
}

return IoMatrix_netx500
