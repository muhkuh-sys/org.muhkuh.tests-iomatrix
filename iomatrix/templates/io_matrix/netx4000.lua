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
local IoMatrix_netx4000 = class(IoMatrix_netx_base)


function IoMatrix_netx4000:_init(tLog, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
  self:super(tLog, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
end



IoMatrix_netx4000.atKnownPins = {
  ['MMIO0'] = { type=${PINTYPE_MMIO}, index=0 },
  ['MMIO1'] = { type=${PINTYPE_MMIO}, index=1 },
  ['MMIO2'] = { type=${PINTYPE_MMIO}, index=2 },
  ['MMIO3'] = { type=${PINTYPE_MMIO}, index=3 },
  ['MMIO4'] = { type=${PINTYPE_MMIO}, index=4 },
  ['MMIO5'] = { type=${PINTYPE_MMIO}, index=5 },
  ['MMIO6'] = { type=${PINTYPE_MMIO}, index=6 },
  ['MMIO7'] = { type=${PINTYPE_MMIO}, index=7 },
  ['MMIO8'] = { type=${PINTYPE_MMIO}, index=6 },
  ['MMIO9'] = { type=${PINTYPE_MMIO}, index=7 },
  ['MMIO10'] = { type=${PINTYPE_MMIO}, index=10 },
  ['MMIO11'] = { type=${PINTYPE_MMIO}, index=11 },
  ['MMIO12'] = { type=${PINTYPE_MMIO}, index=12 },
  ['MMIO13'] = { type=${PINTYPE_MMIO}, index=13 },
  ['MMIO14'] = { type=${PINTYPE_MMIO}, index=14 },
  ['MMIO15'] = { type=${PINTYPE_MMIO}, index=15 },
  ['MMIO16'] = { type=${PINTYPE_MMIO}, index=16 },
  ['MMIO17'] = { type=${PINTYPE_MMIO}, index=17 },
  ['MMIO18'] = { type=${PINTYPE_MMIO}, index=18 },
  ['MMIO19'] = { type=${PINTYPE_MMIO}, index=19 },
  ['MMIO20'] = { type=${PINTYPE_MMIO}, index=20 },
  ['MMIO21'] = { type=${PINTYPE_MMIO}, index=21 },
  ['MMIO22'] = { type=${PINTYPE_MMIO}, index=22 },
  ['MMIO23'] = { type=${PINTYPE_MMIO}, index=23 },
  ['MMIO24'] = { type=${PINTYPE_MMIO}, index=24 },
  ['MMIO25'] = { type=${PINTYPE_MMIO}, index=25 },
  ['MMIO26'] = { type=${PINTYPE_MMIO}, index=26 },
  ['MMIO27'] = { type=${PINTYPE_MMIO}, index=27 },
  ['MMIO28'] = { type=${PINTYPE_MMIO}, index=28 },
  ['MMIO29'] = { type=${PINTYPE_MMIO}, index=29 },
  ['MMIO30'] = { type=${PINTYPE_MMIO}, index=30 },
  ['MMIO31'] = { type=${PINTYPE_MMIO}, index=31 },
  ['MMIO32'] = { type=${PINTYPE_MMIO}, index=32 },
  ['MMIO33'] = { type=${PINTYPE_MMIO}, index=33 },
  ['MMIO34'] = { type=${PINTYPE_MMIO}, index=34 },
  ['MMIO35'] = { type=${PINTYPE_MMIO}, index=35 },
  ['MMIO36'] = { type=${PINTYPE_MMIO}, index=36 },
  ['MMIO37'] = { type=${PINTYPE_MMIO}, index=37 },
  ['MMIO38'] = { type=${PINTYPE_MMIO}, index=38 },
  ['MMIO39'] = { type=${PINTYPE_MMIO}, index=39 },
  ['MMIO40'] = { type=${PINTYPE_MMIO}, index=40 },
  ['MMIO41'] = { type=${PINTYPE_MMIO}, index=41 },
  ['MMIO42'] = { type=${PINTYPE_MMIO}, index=42 },
  ['MMIO43'] = { type=${PINTYPE_MMIO}, index=43 },
  ['MMIO44'] = { type=${PINTYPE_MMIO}, index=44 },
  ['MMIO45'] = { type=${PINTYPE_MMIO}, index=45 },
  ['MMIO46'] = { type=${PINTYPE_MMIO}, index=46 },
  ['MMIO47'] = { type=${PINTYPE_MMIO}, index=47 },
  ['MMIO48'] = { type=${PINTYPE_MMIO}, index=48 },
  ['MMIO49'] = { type=${PINTYPE_MMIO}, index=49 },
  ['MMIO50'] = { type=${PINTYPE_MMIO}, index=50 },
  ['MMIO51'] = { type=${PINTYPE_MMIO}, index=51 },
  ['MMIO52'] = { type=${PINTYPE_MMIO}, index=52 },
  ['MMIO53'] = { type=${PINTYPE_MMIO}, index=53 },
  ['MMIO54'] = { type=${PINTYPE_MMIO}, index=54 },
  ['MMIO55'] = { type=${PINTYPE_MMIO}, index=55 },
  ['MMIO56'] = { type=${PINTYPE_MMIO}, index=56 },
  ['MMIO57'] = { type=${PINTYPE_MMIO}, index=57 },
  ['MMIO58'] = { type=${PINTYPE_MMIO}, index=58 },
  ['MMIO59'] = { type=${PINTYPE_MMIO}, index=59 },
  ['MMIO60'] = { type=${PINTYPE_MMIO}, index=60 },
  ['MMIO61'] = { type=${PINTYPE_MMIO}, index=61 },
  ['MMIO62'] = { type=${PINTYPE_MMIO}, index=62 },
  ['MMIO63'] = { type=${PINTYPE_MMIO}, index=63 },
  ['MMIO64'] = { type=${PINTYPE_MMIO}, index=64 },
  ['MMIO65'] = { type=${PINTYPE_MMIO}, index=65 },
  ['MMIO66'] = { type=${PINTYPE_MMIO}, index=66 },
  ['MMIO67'] = { type=${PINTYPE_MMIO}, index=67 },
  ['MMIO68'] = { type=${PINTYPE_MMIO}, index=68 },
  ['MMIO69'] = { type=${PINTYPE_MMIO}, index=69 },
  ['MMIO70'] = { type=${PINTYPE_MMIO}, index=70 },
  ['MMIO71'] = { type=${PINTYPE_MMIO}, index=71 },
  ['MMIO72'] = { type=${PINTYPE_MMIO}, index=72 },
  ['MMIO73'] = { type=${PINTYPE_MMIO}, index=73 },
  ['MMIO74'] = { type=${PINTYPE_MMIO}, index=74 },
  ['MMIO75'] = { type=${PINTYPE_MMIO}, index=75 },
  ['MMIO76'] = { type=${PINTYPE_MMIO}, index=76 },
  ['MMIO77'] = { type=${PINTYPE_MMIO}, index=77 },
  ['MMIO78'] = { type=${PINTYPE_MMIO}, index=78 },
  ['MMIO79'] = { type=${PINTYPE_MMIO}, index=79 },
  ['MMIO80'] = { type=${PINTYPE_MMIO}, index=80 },
  ['MMIO81'] = { type=${PINTYPE_MMIO}, index=81 },
  ['MMIO82'] = { type=${PINTYPE_MMIO}, index=82 },
  ['MMIO83'] = { type=${PINTYPE_MMIO}, index=83 },
  ['MMIO84'] = { type=${PINTYPE_MMIO}, index=84 },
  ['MMIO85'] = { type=${PINTYPE_MMIO}, index=85 },
  ['MMIO86'] = { type=${PINTYPE_MMIO}, index=86 },
  ['MMIO87'] = { type=${PINTYPE_MMIO}, index=87 },
  ['MMIO88'] = { type=${PINTYPE_MMIO}, index=88 },
  ['MMIO89'] = { type=${PINTYPE_MMIO}, index=89 },
  ['MMIO90'] = { type=${PINTYPE_MMIO}, index=90 },
  ['MMIO91'] = { type=${PINTYPE_MMIO}, index=91 },
  ['MMIO92'] = { type=${PINTYPE_MMIO}, index=92 },
  ['MMIO93'] = { type=${PINTYPE_MMIO}, index=93 },
  ['MMIO94'] = { type=${PINTYPE_MMIO}, index=94 },
  ['MMIO95'] = { type=${PINTYPE_MMIO}, index=95 },
  ['MMIO96'] = { type=${PINTYPE_MMIO}, index=96 },
  ['MMIO97'] = { type=${PINTYPE_MMIO}, index=97 },
  ['MMIO98'] = { type=${PINTYPE_MMIO}, index=98 },
  ['MMIO99'] = { type=${PINTYPE_MMIO}, index=99 },
  ['MMIO100'] = { type=${PINTYPE_MMIO}, index=100 },
  ['MMIO101'] = { type=${PINTYPE_MMIO}, index=101 },
  ['MMIO102'] = { type=${PINTYPE_MMIO}, index=102 },
  ['MMIO103'] = { type=${PINTYPE_MMIO}, index=103 },
  ['MMIO104'] = { type=${PINTYPE_MMIO}, index=104 },
  ['MMIO105'] = { type=${PINTYPE_MMIO}, index=105 },
  ['MMIO106'] = { type=${PINTYPE_MMIO}, index=106 },

  ['RDY'] = { type=${PINTYPE_RDYRUN}, index=0 },
  ['RUN'] = { type=${PINTYPE_RDYRUN}, index=1 }
}



return IoMatrix_netx4000
