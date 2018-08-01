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
local IoMatrix = class()


function IoMatrix:_init(tLog)
  self.bit = require 'bit'
  self.pl = require'pl.import_into'()

  self.tLog = tLog

  ---------------------------------------------------------------------------
  --                          Definitions                                  --
  ---------------------------------------------------------------------------
  self.PINFLAG_I    = 1
  self.PINFLAG_O    = 2
  self.PINFLAG_Z    = 4
  self.PINFLAG_IOZ  = 7

  self.FAMILY_NETX  = 1
  self.FAMILY_FTDI  = 2

  self.atPins = nil

  -- Load the distributors for the pin families.
  self.ftdi = require 'io_matrix.ftdi'(tLog)
  self.netx = require 'io_matrix.netx'(tLog)
  -- Create a lookup table for the distributors.
  self.atFamily2Distributor = {
    [self.FAMILY_NETX] = self.netx,
    [self.FAMILY_FTDI] = self.ftdi
  }
end



function IoMatrix:add_netx(tPlugin, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
  self.netx:add_netx_device(tPlugin, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
end



function IoMatrix:add_ftdi(atAttributes)
  self.ftdi:add_ftdi_devices(atAttributes)
end



function IoMatrix:parse_pin_description(atPinDescription)
  self.atPins = {}
  self.netx:clear_pins()
  self.ftdi:clear_pins()

  for _, atAttr in pairs(atPinDescription) do
    -- Elements:
    --   1: the ID (a free form identifier)
    --   2: the family (e.g. netx or ftdi)
    --   3: the index for the family
    --   4: the defined name of the pin
    --   5: the default value of the pin in the net
    --   6: the flags for the pin
    local strID = atAttr[1]
    local tFamily = atAttr[2]
    local uiIndex = atAttr[3]
    local strKnownName = atAttr[4]
    local uiDefaultValue = atAttr[5]
    local uiFlags = atAttr[6]

    -- Does the ID already exist?
    if self.atPins[strID]~=nil then
      error('The pin "%s" already exists.', strID)
    end

    -- Get the distributor fot the family.
    local tDistrib = self.atFamily2Distributor[tFamily]
    if tFamily==nil then
      error('Invalid family: %s.', tostring(tFamily))
    end

    -- Pass the pin to the distributor.
    local tPin = tDistrib:add_pin(strID, uiIndex, strKnownName, uiDefaultValue, uiFlags)

    -- Add common attributes to the pin.
    tPin.id = strID
    tPin.default = uiDefaultValue
    tPin.flags = uiFlags

    self.atPins[strID] = tPin
  end

  -- Start parsing in the handlers.
  self.netx:parse_pins()
  self.ftdi:parse_pins()
end



function IoMatrix:setZ()
  self.netx:setZ()
  self.ftdi:setZ()
end



function IoMatrix:bset()
  self.netx:bset()
  self.ftdi:bset()
end


function IoMatrix:bget()
  self.netx:bget()
  self.ftdi:bget()
end



function IoMatrix:close()
  self.netx:close()
  self.ftdi:close()
end



function IoMatrix:__delay_to_stabilize_pins()
  -- TODO: Add some delay routine here.
end



function IoMatrix:__parse_networks(atNetworksAscii)
  -- Get a shortcut to the pins.
  local p = self.atPins

  local atNetworks = {}

  -- Loop over all networks.
  for uiNetworkCnt, tNetworkAscii in ipairs(atNetworksAscii) do
    local tNetwork = {}

    -- Loop over all pins in the network.
    for uiPinCnt, strPin in ipairs(tNetworkAscii) do
      -- Look up the pin.
      local tPin = p[strPin]
      if tPin==nil then
        self.tLog.error('The ID of pin %d in network %d is unknown: "%s"', uiPinCnt, uiNetworkCnt, strPin)
        error('Unknown pin.')
      end

      table.insert(tNetwork, tPin)
    end

    table.insert(atNetworks, tNetwork)
  end

  return atNetworks
end



function IoMatrix:__check_all_networks_for_default(atNetworks, tNetworkExcept)
  -- Be optimistic.
  local uiErrorCounter = 0

  -- Loop over all networks.
  for uiNetworkCnt, tNetwork in ipairs(atNetworks) do
    if tNetwork~=tNetworkExcept then
      -- Loop over all pins.
      for uiPinCnt, tPin in ipairs(tNetwork) do
        -- Get the buffered pin state.
        local ucVal = tPin.bget()
        if ucVal~=tPin.default then
          self.tLog.error('Pin "%s" should be in the default state of %d, but it is %d.', tPin.id, tPin.default, ucVal)
          uiErrorCounter = uiErrorCounter + 1
        end
      end
    end
  end

  if uiErrorCounter==0 then
    self.tLog.debug('All pins are in default state.')
  end
  return uiErrorCounter
end



function IoMatrix:__test_pin(atNetworks, tNetworkUnderTest, tPinUnderTest, ucValue)
  local bit = self.bit
  local uiErrorCounter = 0

  -- Can the network be tested by driving the selected pin?
  local fOk = true

  -- Does the pin have output capabilities?
  if bit.band(tPinUnderTest.flags, self.PINFLAG_O)==0 then
    self.tLog.debug('Not testing pin "%s": it has no output capabilities.', tPinUnderTest.id)
    fOk = false
  else
    -- The rest of the network must not have an output-only pin.
    local uiInputCnt = 0
    for _, tOtherPin in ipairs(tNetworkUnderTest) do
      if tOtherPin~=tPinUnderTest then
        if bit.band(tOtherPin.flags, self.PINFLAG_IOZ)==self.PINFLAG_O then
          self.tLog.debug('Not testing pin "%s": the connected pin "%s" is output-only.', tPinUnderTest.id, tOtherPin.id)
          fOk = false
          break
        end
        if bit.band(tPinUnderTest.flags, self.PINFLAG_I)~=0 then
          uiInputCnt = uiInputCnt + 1
        end
      end
    end
    if uiInputCnt==0 then
      self.tLog.debug('Not testing pin "%s": no pins with input capabilities connected.', tPinUnderTest.id)
      fOk = false
    end
  end

  if fOk==true then
    self.tLog.debug('Set pin "%s" to %d.', tPinUnderTest.id, ucValue)
    tPinUnderTest.bset(ucValue)
    self:bset()
    self:__delay_to_stabilize_pins()
    self:bget()

    -- All pins of the same network must follow.
    for uiOtherPinCnt, tOtherPin in ipairs(tNetworkUnderTest) do
      if tOtherPin~=tPinUnderTest then
        local ucOtherVal = tOtherPin.bget()
        if ucValue~=ucOtherVal then
          self.tLog.error('Pin "%s" did not follow setting "%s" to %d.', tOtherPin.id, tPinUnderTest.id, ucValue)
          uiErrorCounter = uiErrorCounter + 1
        else
          self.tLog.debug('Pin "%s" follows.', tOtherPin.id)
        end
      end
    end

    -- All pins of all other networks must stay at their default values.
    uiErrorCounter = uiErrorCounter + self:__check_all_networks_for_default(atNetworks, tNetworkUnderTest)
  end

  return uiErrorCounter
end



function IoMatrix:matrix_test(atNetworksAscii)
  local uiErrorCounter = 0

  -- Parse the networks.
  local atNetworks = self:__parse_networks(atNetworksAscii)

  self.netx:start_testing()
  self.ftdi:start_testing()

  -- Set all pins to input.
  self:setZ()
  self:bset()
  self:__delay_to_stabilize_pins()
  self:bget()

  -- Check all networks for the default state.
  uiErrorCounter = uiErrorCounter + self:__check_all_networks_for_default(atNetworks)

  -- Loop over all networks.
  for uiNetworkCnt, tNetworkUnderTest in ipairs(atNetworks) do
    -- Loop over all pins.
    for uiPinCnt, tPinUnderTest in ipairs(tNetworkUnderTest) do
      -- Set the pin to 0.
      uiErrorCounter = uiErrorCounter + self:__test_pin(atNetworks, tNetworkUnderTest, tPinUnderTest, 0)
      -- Set the pin to 1.
      uiErrorCounter = uiErrorCounter + self:__test_pin(atNetworks, tNetworkUnderTest, tPinUnderTest, 1)
      -- Set the pin to z.
      self.tLog.debug('Set pin "%s" to z.', tPinUnderTest.id)
      tPinUnderTest.bset('z')
      self:bset()
      self:__delay_to_stabilize_pins()
      self:bget()
      -- Check all networks for the default state.
      uiErrorCounter = uiErrorCounter + self:__check_all_networks_for_default(atNetworks)
    end
  end

  self.netx:stop_testing()
  self.ftdi:stop_testing()

  return uiErrorCounter
end



return IoMatrix
