local class = require 'pl.class'
local Ftdi2232H = class()

function Ftdi2232H:_init(tLog)
  self.tLog = tLog

  self.luaftdi = require 'luaftdi'
  self.bit = require 'bit'

  -- The FTDI2232H has 2 interfaces with 16 bit each.
  self.tContextA = nil
  self.tContextB = nil

  self.aucPinMask = nil

  self.atBuffer = {
    r = 0,
    w = 0,
    oe = 0
  }

  self.atPinId2BitIndex = nil
  self.atBitIndex2PinId = nil
  self.uiCurrentPinIndex = nil
end



function Ftdi2232H:open(tListEntry, aucPinMask)
  if aucPinMask==nil then
    aucPinMask = { 0xff, 0xff, 0xff, 0xff }
  end
  self.aucPinMask = aucPinMask

  -- Create a context for interface A and B.
  local tContextA = self.luaftdi.Context()
  local tContextB = self.luaftdi.Context()

  -- Set the interfaces.
  local tResult, strError = tContextA:set_interface(self.luaftdi.INTERFACE_A)
  if tResult==nil then
    self.tLog.debug('Failed to set the interface A: %s', tostring(strError))
  else
    tResult, strError = tContextB:set_interface(self.luaftdi.INTERFACE_B)
    if tResult==nil then
      self.tLog.debug('Failed to set the interface B: %s', tostring(strError))
    else
      -- Open the devices.
      tResult, strError = tContextA:usb_open_dev(tListEntry)
      if tResult==nil then
        self.tLog.debug('Failed to open the interface A: %s', tostring(strError))
      else
        tResult, strError = tContextB:usb_open_dev(tListEntry)
        if tResult==nil then
          tContextA:usb_close()
          self.tLog.debug('Failed to open the interface B: %s', tostring(strError))
        else
          tResult, strError = tContextA:set_bitmode(0x00, self.luaftdi.BITMODE_MPSSE)
          if tResult==nil then
            tContextA:usb_close()
            tContextB:usb_close()
            self.tLog.debug('Failed to activate the MPSSE mode on interface A: %s', tostring(strError))
          else
            tResult, strError = tContextB:set_bitmode(0x00, self.luaftdi.BITMODE_MPSSE)
            if tResult==nil then
              tContextA:usb_close()
              tContextB:usb_close()
              self.tLog.debug('Failed to activate the MPSSE mode on interface B: %s', tostring(strError))
            else
              self.tContextA = tContextA
              self.tContextB = tContextB

              tResult = true
            end
          end
        end
      end
    end
  end

  return tResult
end



function Ftdi2232H:close()
  local tContextA = self.tContextA
  if tContextA~=nil then
    tContextA:usb_close()
    self.tContextA = nil
  end

  local tContextB = self.tContextB
  if tContextB~=nil then
    tContextB:usb_close()
    self.tContextB = nil
  end
end


--[[
function Ftdi2232H:__write_and_read_pins(uiOe, uiOut)
  local luaftdi = self.luaftdi
  local bit = self.bit
  local tResult

  -- Construct the MPSSE sequence for interface A and B.
  local strCmdA = string.char(
    luaftdi.SET_BITS_LOW, bit.band(uiOut, 0xff), bit.band(uiOe, 0xff),
    luaftdi.SET_BITS_HIGH, bit.band(bit.rshift(uiOut, 8), 0xff), bit.band(bit.rshift(uiOe, 8), 0xff),
    luaftdi.GET_BITS_LOW,
    luaftdi.GET_BITS_HIGH,
    luaftdi.SEND_IMMEDIATE
  )
  local strCmdB = string.char(
    luaftdi.SET_BITS_LOW, bit.band(bit.rshift(uiOut, 16), 0xff), bit.band(bit.rshift(uiOe, 16), 0xff),
    luaftdi.SET_BITS_HIGH, bit.band(bit.rshift(uiOut, 24), 0xff), bit.band(bit.rshift(uiOe, 24), 0xff),
    luaftdi.GET_BITS_LOW,
    luaftdi.GET_BITS_HIGH,
    luaftdi.SEND_IMMEDIATE
  )
  -- Execute the commands and get the data.
  self.tContextA:write_data(strCmdA)
  local tResultA, strErrorA = self.tContextA:read_data(2)
  if tResultA==nil then
    self.tLog.error('Failed to read 2 bytes from interface A: %s', tostring(strErrorA))
  else
    self.tContextB:write_data(strCmdB)
    local tResultB, strErrorB = self.tContextB:read_data(2)
    if tResultB==nil then
      self.tLog.error('Failed to read 2 bytes from interface B: %s', tostring(strErrorB))
    else
      -- Convert both data parts to a number.
      tResult = bit.bor(
        string.byte(tResultA, 1),
        bit.lshift(string.byte(tResultA, 2), 8),
        bit.lshift(string.byte(tResultB, 1), 16),
        bit.lshift(string.byte(tResultB, 2), 24)
      )
    end
  end

  return tResult
end
--]]


-- Write all pins with an output enable and an output value.
function Ftdi2232H:__write_pins(uiOe, uiOut)
  local luaftdi = self.luaftdi
  local bit = self.bit
  local aucPinMask = self.aucPinMask

--  print(string.format('%08x %08x %02x %02x %02x %02x', uiOe, uiOut, aucPinMask[1], aucPinMask[2], aucPinMask[3], aucPinMask[4]))

  -- Construct the MPSSE sequence for interface A and B.
  if bit.bor(aucPinMask[1], aucPinMask[2])~=0x00 then
    local aucCmd = {}
    if aucPinMask[1]~=0x00 then
      table.insert(aucCmd, string.char(luaftdi.SET_BITS_LOW, bit.band(uiOut, 0xff), bit.band(uiOe, 0xff)))
    end
    if aucPinMask[2]~=0x00 then
      table.insert(aucCmd, string.char(luaftdi.SET_BITS_HIGH, bit.band(bit.rshift(uiOut, 8), 0xff), bit.band(bit.rshift(uiOe, 8), 0xff)))
    end
    table.insert(aucCmd, string.char(luaftdi.SEND_IMMEDIATE))

    self.tContextA:write_data(table.concat(aucCmd))
  end

  if bit.bor(aucPinMask[3], aucPinMask[4])~=0x00 then
    local aucCmd = {}
    if aucPinMask[3]~=0x00 then
      table.insert(aucCmd, string.char(luaftdi.SET_BITS_LOW, bit.band(bit.rshift(uiOut, 16), 0xff), bit.band(bit.rshift(uiOe, 16), 0xff)))
    end
    if aucPinMask[4]~=0x00 then
      table.insert(aucCmd, string.char(luaftdi.SET_BITS_HIGH, bit.band(bit.rshift(uiOut, 24), 0xff), bit.band(bit.rshift(uiOe, 24), 0xff)))
    end
    table.insert(aucCmd, string.char(luaftdi.SEND_IMMEDIATE))

    self.tContextB:write_data(table.concat(aucCmd))
  end
end



-- Read all pins from the FTDI device.
function Ftdi2232H:__read_pins()
  local luaftdi = self.luaftdi
  local bit = self.bit
  local tResult

  -- Construct the MPSSE sequence for interface A and B.
  local strCmdA = string.char(
    luaftdi.GET_BITS_LOW,
    luaftdi.GET_BITS_HIGH,
    luaftdi.SEND_IMMEDIATE
  )
  local strCmdB = string.char(
    luaftdi.GET_BITS_LOW,
    luaftdi.GET_BITS_HIGH,
    luaftdi.SEND_IMMEDIATE
  )
  -- Execute the commands and get the data.
  self.tContextA:write_data(strCmdA)
  local tResultA, strErrorA = self.tContextA:read_data(2)
  if tResultA==nil then
    self.tLog.error('Failed to read 2 bytes from interface A: %s', tostring(strErrorA))
  else
    self.tContextB:write_data(strCmdB)
    local tResultB, strErrorB = self.tContextB:read_data(2)
    if tResultB==nil then
      self.tLog.error('Failed to read 2 bytes from interface B: %s', tostring(strErrorB))
    else
      -- Convert both data parts to a number.
      tResult = bit.bor(
        string.byte(tResultA, 1),
        bit.lshift(string.byte(tResultA, 2), 8),
        bit.lshift(string.byte(tResultB, 1), 16),
        bit.lshift(string.byte(tResultB, 2), 24)
      )
    end
  end

  return tResult
end



function Ftdi2232H:clear_pins()
  self.atPinId2BitIndex = {}
  self.atBitIndex2PinId = {}
  self.uiCurrentPinIndex = 0
end



Ftdi2232H.__atInterfaceToOffset = {
  ['AD'] =  0,
  ['AC'] =  8,
  ['BD'] = 16,
  ['BC'] = 24
}



function Ftdi2232H:add_pin(strID, strKnownName, uiDefaultValue, uiFlags)
  -- Parse the known name.
  local strIf, strOffset = string.match(string.upper(strKnownName), '([AB][CD])(%d+)')
  if strIf==nil then
    self.tLog.error('Invalid pin name: "%s"', strKnownName)
    error('Invalid pin name.')
  end

  -- Check the offset.
  local uiOffset = tonumber(strOffset)
  if uiOffset<0 or uiOffset>7 then
    self.tLog.error('Invalid pin index: %s', strOffset)
    error('Invalid pin index.')
  end
  
  
  local uiBitIndex = uiOffset + self.__atInterfaceToOffset[strIf]

  if self.atPinId2BitIndex[strID]~=nil then
    self.tLog.error('The pin "%s" is already defined.', strID)
    error('Pin already defined.')
  end
  local strConflictingPin = self.atBitIndex2PinId[uiBitIndex]
  if strConflictingPin~=nil then
    self.tLog.error('The signal "%s" is already used by pin "%s".', strKnownName, strConflictingPin)
    error('Signal already used.')
  end
  self.atPinId2BitIndex[strID] = uiBitIndex
  self.atBitIndex2PinId[uiBitIndex] = strID
  self.uiCurrentPinIndex = self.uiCurrentPinIndex + 1

  -- Construct a shortcut to this pin.
  local tSelf = self
  local bit = self.bit
  local atBuffer = self.atBuffer
  local ulPinMask = bit.lshift(1, uiBitIndex)
  local ulPinMaskN = bit.bnot(ulPinMask)
  local atPin = {}
  atPin.bget = function()
    return bit.band(bit.rshift(atBuffer.r, uiBitIndex), 1)
  end
  atPin.bset = function(tVal)
    local strVal = string.upper(tostring(tVal))
    if strVal=='Z' then
      atBuffer.w = bit.band(atBuffer.w, ulPinMaskN)
      atBuffer.oe = bit.band(atBuffer.oe, ulPinMaskN)
    elseif strVal=='0' then
      atBuffer.w = bit.band(atBuffer.w, ulPinMaskN)
      atBuffer.oe = bit.bor(atBuffer.oe, ulPinMask)
    elseif strVal=='1' then
      atBuffer.w = bit.bor(atBuffer.w, ulPinMask)
      atBuffer.oe = bit.bor(atBuffer.oe, ulPinMask)
    else
      error('Invalid value: ' .. strVal)
    end
  end
  atPin.get = function()
    atBuffer.r = tSelf:__read_pins()
    return atPin.bget()
  end
  atPin.set = function(tVal)
    atPin.bset(tVal)
    tSelf:__write_pins(atBuffer.oe, atBuffer.w)
  end

  return atPin
end



function Ftdi2232H:parse_pins()
  -- Set all pins to input.
  self.atBuffer.w = 0
  self.atBuffer.oe = 0
  self:__write_pins(0, 0)
  -- Read the current pin status.
  self.atBuffer.r = self:__read_pins()
end



function Ftdi2232H:start_testing()
  -- Nothing do do here.
end



function Ftdi2232H:stop_testing()
  -- Nothing do do here.
end



function Ftdi2232H:setZ()
  self.atBuffer.oe = 0
  self.atBuffer.w = 0
end



function Ftdi2232H:bset()
  self:__write_pins(self.atBuffer.oe, self.atBuffer.w)
end



function Ftdi2232H:bget()
  self.atBuffer.r = self:__read_pins()
end


return Ftdi2232H
