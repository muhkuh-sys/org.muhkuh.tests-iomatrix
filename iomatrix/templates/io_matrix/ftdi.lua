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
local IoMatrix_FTDI = class()



function IoMatrix_FTDI:_init(tLog)
  self.tLog = tLog

  self.bit = require 'bit'
  self.luaftdi = require 'luaftdi'
  self.cFTDI2232H = require 'io_matrix.ftdi_2232h'

  local tVersionInfo = self.luaftdi.get_library_version()
  tLog.debug("[FTDI version] major: %d, minor: %d, micro: %d, version_str: %s, snapshot_str: %s", tVersionInfo.major, tVersionInfo.minor, tVersionInfo.micro, tVersionInfo.version_str, tVersionInfo.snapshot_str)

  -- No FTDI devices detected yet.
  self.atDevices = {}
  self.atDeviceAttrs = {}

  -- Create a new context.
  self.tContext = self.luaftdi.Context()
end



function IoMatrix_FTDI:__print_list_entry(fnLog, tListEntry)
  fnLog('  Bus: %d', tListEntry:get_bus_number())
  fnLog('  Port number: %d', tListEntry:get_port_number())

  fnLog('  Port Numbers:')
  local tPN, strError = tListEntry:get_port_numbers()
  if tPN==nil then
    fnLog('    Failed to get the port numbers:', strError)
  else
    for uiCnt, uiPort in ipairs(tPN) do
      fnLog('    %d: 0x%02x', uiCnt, uiPort)
    end
  end

  fnLog('  Manufacturer: %s', tListEntry:get_manufacturer())
  fnLog('  Description: %s', tListEntry:get_description())
  fnLog('  Serial: %s', tListEntry:get_serial())
end



--- Convert a list of port numbers to a string for easier comparison.
function IoMatrix_FTDI.__usb_port_numbers_to_string(atPortNumbers)
  local strPN = ''
  for _, uiPort in ipairs(atPortNumbers) do
    strPN = strPN .. string.char(uiPort)
  end
  return strPN
end



function IoMatrix_FTDI:__build_ftdi_index(tList, ulModuleIndexMask, ulModuleIndexShift)
  -- Loop over all devices and...
  --  1) Get the FTDI serial.
  --  2) Get the location for all devices as strings.
  --  3) Read the index pins if available.
  local atDevices = {}
  for tListEntry in tList:iter() do
    local strFtdiSerial = tListEntry:get_serial()
    self.tLog.debug('Found serial %s.', strFtdiSerial)

    -- Get the port numbers as a table.
    local tPN, strError = tListEntry:get_port_numbers()
    if tPN==nil then
      self.tLog.debug('Failed to get the port numbers for the following entry: %s', tostring(strError))
      self.__print_list_entry(self.tLog.debug, tListEntry)
    else
      local strPrettyPN = table.concat(tPN, ', ')
      self.tLog.debug('Found port numbers [%s].', strPrettyPN)

      -- Combine all table entries to a string.
      local strPN = self.__usb_port_numbers_to_string(tPN)

      local ulModuleIndex
      -- Look for the module index only if the bit position is specified.
      if ulModuleIndexMask~=nil and ulModuleIndexShift~=nil then
        local tFtdi = self.cFTDI2232H(self.tLog)
        local tResult = tFtdi:open(tListEntry)
        if tResult==true then
          tFtdi:__write_pins(0, 0)
          tResult = tFtdi:__read_pins()
          if tResult==nil then
            self.tLog.debug('Failed to read the pins.')
          else
            local ulPins = tResult
            self.tLog.debug('Input data: 0x%s', self.bit.tohex(ulPins))
            ulModuleIndex = self.bit.rshift(self.bit.band(ulPins, ulModuleIndexMask), ulModuleIndexShift)
            self.tLog.debug('Found module index %d.', ulModuleIndex)
          end
        
          tFtdi:close()
        end
      end

      local tAttr = {}
      tAttr.tListEntry = tListEntry
      tAttr.strFtdiSerial = strFtdiSerial
      tAttr.strPortNumbers = strPN
      tAttr.strPrettyPortNumbers = strPrettyPN
      tAttr.ulModuleIndex = ulModuleIndex
      table.insert(atDevices, tAttr)
    end
  end

  return atDevices
end



function IoMatrix_FTDI:__find_ftdi_devices(atAllDevices, atDeviceAttr)
  local tAllOk = true

  -- Loop over all required FTDI devices.
  for _, tDeviceAttr in ipairs(atDeviceAttr) do
    -- Get the required port numbers as a string or nil.
    local strPN
    if tDeviceAttr.port~=nil then
      strPN = self.__usb_port_numbers_to_string(tDeviceAttr.port)
    end
    -- Get the required serial or nil.
    local strSerial = tDeviceAttr.serial
    local ulModuleIndex = tDeviceAttr.moduleidx

    -- Search the complete list of devices.
    local tFoundDevice
    for _, tDevice in pairs(atAllDevices) do
      -- Is the device already part of the identified devices?
      for _, tIdentDev in pairs(self.atDeviceAttrs) do
        if tIdentDev.strPortNumbers==tDevice.strPortNumbers then
          self.tLog.debug('Not considering device at port [%s] again.', tIdentDev.strPrettyPortNumbers)
          tDevice = nil
          break
        end 
      end
      if tDevice~=nil then
        -- Should a port number be checked and does it differ?
        if strPN~=nil and tDevice.strPortNumbers~=strPN then
          -- Do not use this device.
          self.tLog.debug('The port of device [%s] does not match.', tDevice.strPrettyPortNumbers)
        elseif strSerial~=nil and tDevice.strFtdiSerial~=strSerial then
          self.tLog.debug('The serial of device [%s] does not match.', tDevice.strPrettyPortNumbers)
        elseif ulModuleIndex~=nil and tDevice.ulModuleIndex~=ulModuleIndex then
          self.tLog.debug('The module index of device [%s] does not match.', tDevice.strPrettyPortNumbers)
        else
          self.tLog.debug('Device [%s] matches.', tDevice.strPrettyPortNumbers)
          tFoundDevice = tDevice
          break
        end
      end
    end

    if tFoundDevice==nil then
      self.tLog.error('No suitable device found.')
      tAllOk = false
      break
    else
      local tFtdi = self.cFTDI2232H(self.tLog)
      local tResult = tFtdi:open(tFoundDevice.tListEntry)
      if tResult==nil then
        self.tLog.error('Failed to open the device [%s].', tFoundDevice.strPrettyPortNumbers)
        tAllOk = false
      else
        table.insert(self.atDevices, tFtdi)
        table.insert(self.atDeviceAttrs, tFoundDevice)
        self.tLog.debug('Using device [%s] as module %d.', tFoundDevice.strPrettyPortNumbers, #self.atDevices)
      end
    end
  end

  return tAllOk
end



function IoMatrix_FTDI:add_ftdi_devices(atAttributes)
  local usUSBVendor = atAttributes.usUSBVendor
  local usUSBProduct = atAttributes.usUSBProduct
  local ulModuleIndexMask = atAttributes.ulModuleIndexMask
  local ulModuleIndexShift = atAttributes.ulModuleIndexShift

  -- Scan for all devices.
  local tList = self.tContext:usb_find_all(usUSBVendor, usUSBProduct)

  -- This is only a debug dump of all available devices.
  self.tLog.debug('-----------------------------------------------------------------------')
  self.tLog.debug("List all devices with VID 0x%04x and PID 0x%04x.", usUSBVendor, usUSBProduct)
  for tListEntry in tList:iter() do
    self:__print_list_entry(self.tLog.debug, tListEntry)
    self.tLog.debug('')
  end
  self.tLog.debug('-----------------------------------------------------------------------')

  -- Collect information about all FTDI devices.
  local atAllDevices = self:__build_ftdi_index(tList, ulModuleIndexMask, ulModuleIndexShift)
  if #atAllDevices==0 then
    self.tLog.debug('No devices found.')
  else
    local tResult = self:__find_ftdi_devices(atAllDevices, atAttributes)
    if tResult~=true then
      error('Failed to find all FTDI devices.')
    end
  end
end



function IoMatrix_FTDI:clear_pins()
  for _, tDev in pairs(self.atDevices) do
    tDev:clear_pins()
  end
end



function IoMatrix_FTDI:add_pin(strID, uiIndex, strKnownName, uiDefaultValue, uiFlags)
  local tDev = self.atDevices[uiIndex]
  if tDev==nil then
    self.tLog.error('No FTDI device with index %d exists.', uiIndex)
    error('FTDI index not found.')
  end
  return tDev:add_pin(strID, strKnownName, uiDefaultValue, uiFlags)
end



function IoMatrix_FTDI:parse_pins()
  for _, tDev in pairs(self.atDevices) do
    tDev:parse_pins()
  end
end



function IoMatrix_FTDI:start_testing()
  for _, tDev in pairs(self.atDevices) do
    tDev:start_testing()
  end
end



function IoMatrix_FTDI:stop_testing()
  for _, tDev in pairs(self.atDevices) do
    tDev:stop_testing()
  end
end



function IoMatrix_FTDI:setZ()
  for _, tDev in pairs(self.atDevices) do
    tDev:setZ()
  end
end



function IoMatrix_FTDI:bset()
  for _, tDev in pairs(self.atDevices) do
    tDev:bset()
  end
end



function IoMatrix_FTDI:bget()
  for _, tDev in pairs(self.atDevices) do
    tDev:bget()
  end
end



function IoMatrix_FTDI:close()
  -- Close all FTDI devices.
  for _, tDev in pairs(self.atDevices) do
    tDev:close()
  end
end


return IoMatrix_FTDI
