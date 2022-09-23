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
local IoMatrix_netx = class()

function IoMatrix_netx:_init(tLog)
  self.pl = require'pl.import_into'()

  self.tLog = tLog
  self.atDevices = {}

  local romloader = require 'romloader'
  self.__atAsicTypToClass = {
    [romloader.ROMLOADER_CHIPTYP_NETX50]        = 'io_matrix.netx_base',
    [romloader.ROMLOADER_CHIPTYP_NETX100]       = 'io_matrix.netx_base',
    [romloader.ROMLOADER_CHIPTYP_NETX500]       = 'io_matrix.netx_base',
    [romloader.ROMLOADER_CHIPTYP_NETX10]        = 'io_matrix.netx_base',
    [romloader.ROMLOADER_CHIPTYP_NETX56]        = 'io_matrix.netx_base',
    [romloader.ROMLOADER_CHIPTYP_NETX56B]       = 'io_matrix.netx_base',
    [romloader.ROMLOADER_CHIPTYP_NETX90_MPW]    = 'io_matrix.netx90_mpw',
    [romloader.ROMLOADER_CHIPTYP_NETX4000_FULL] = 'io_matrix.netx4000',
    [romloader.ROMLOADER_CHIPTYP_NETX90]        = 'io_matrix.netx90',
    [romloader.ROMLOADER_CHIPTYP_NETX90B]       = 'io_matrix.netx90'
  }
end



function IoMatrix_netx:add_netx_device(tPlugin, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
  -- Get the chip type.
  local tAsicTyp = tPlugin:GetChiptyp()

  -- Get the class for the ASIC.
  local strClass = self.__atAsicTypToClass[tAsicTyp]
  if strClass==nil then
    error('Unknown chiptyp!')
  end

  -- Create a new instance of the class.
  self.tLog.debug('Creating a new instance of "%s".', strClass)
  local cClass = require(strClass)
  local tNetx = cClass(self.tLog, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)

  tNetx:initialize(tPlugin)

  -- Add it to the list of netX devices.
  table.insert(self.atDevices, tNetx)
end



function IoMatrix_netx:clear_pins()
  for _, tDev in pairs(self.atDevices) do
    tDev:clear_pins()
  end
end



function IoMatrix_netx:add_pin(strID, uiIndex, strKnownName, uiDefaultValue, uiFlags)
  local tNetx = self.atDevices[uiIndex]
  if tNetx==nil then
    self.tLog.error('No netX device with index %d exists.', uiIndex)
    error('Invalid index.')
  end
  return tNetx:add_pin(strID, strKnownName, uiDefaultValue, uiFlags)
end

function IoMatrix_netx:get_continuous_status_match(tStateList, ulNumberOfPatternsToTest)
  for _, tDev in pairs(self.atDevices) do
    tDev:get_continuous_status_match(tStateList, ulNumberOfPatternsToTest)
  end
end

function IoMatrix_netx:parse_pins()
  for _, tDev in pairs(self.atDevices) do
    tDev:parse_pins()
  end
end



function IoMatrix_netx:start_testing()
  for _, tDev in pairs(self.atDevices) do
    tDev:start_testing()
  end
end



function IoMatrix_netx:stop_testing()
  for _, tDev in pairs(self.atDevices) do
    tDev:stop_testing()
  end
end



function IoMatrix_netx:setZ()
  for _, tDev in pairs(self.atDevices) do
    tDev:setZ()
  end
end



function IoMatrix_netx:bset()
  for _, tDev in pairs(self.atDevices) do
    tDev:bset()
  end
end



function IoMatrix_netx:bget()
  for _, tDev in pairs(self.atDevices) do
    tDev:bget()
  end
end



function IoMatrix_netx:getContinuousChanges(astrStates, fnCallback, pvUser)
  -- Convert the list of states to 32bit values.
  local aulStates = {}
  for _, strState in ipairs(astrStates) do
    local ulState = 0
    for uiPos=string.len(strState),1,-1 do
      ulState = ulState * 2
      if string.sub(strState, uiPos, uiPos)=='1' then
        ulState = ulState + 1
      end
    end
    table.insert(aulStates, ulState)
  end

  for _, tDev in pairs(self.atDevices) do
    tDev:getContinuousChanges(aulStates, fnCallback, pvUser)
  end
end



function IoMatrix_netx:close()
  -- No need to close something.
end


return IoMatrix_netx
