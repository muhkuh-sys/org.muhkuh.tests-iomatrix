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
local IoMatrix_netx_base = class()

function IoMatrix_netx_base:_init(tLog, fnInit, fnDeinit, ulVerbose, fnCallbackProgress, fnCallbackMessage)
  -- Disable verbose mode by default.
  self.ulVerbose = ulVerbose or 0

  self.bit = require 'bit'
  self.pl = require'pl.import_into'()
  local romloader = require 'romloader'
  self.romloader = romloader

  self.tLog = tLog
  self.fnInit = fnInit
  self.fnDeinit = fnDeinit

  ---------------------------------------------------------------------------
  --                          Definitions                                  --
  ---------------------------------------------------------------------------

  self.IOMATRIX_VERSION_MAJOR  = ${VERSION_MAJOR}
  self.IOMATRIX_VERSION_MINOR  = ${VERSION_MINOR}
  self.IOMATRIX_VERSION_MICRO  = ${VERSION_MICRO}
  self.IOMATRIX_VERSION_VCS    = ${VERSION_VCS}

  self.PINSTATUS_HIGHZ       = ${PINSTATUS_HIGHZ}
  self.PINSTATUS_OUTPUT0     = ${PINSTATUS_OUTPUT0}
  self.PINSTATUS_OUTPUT1     = ${PINSTATUS_OUTPUT1}

  self.HEADER_MAGIC          = 0x686f6f6d

  self.IOMATRIX_COMMAND_Parse_Pin_Description    = ${IOMATRIX_COMMAND_Parse_Pin_Description}
  self.IOMATRIX_COMMAND_Set_Pin                  = ${IOMATRIX_COMMAND_Set_Pin}
  self.IOMATRIX_COMMAND_Get_Pin                  = ${IOMATRIX_COMMAND_Get_Pin}
  self.IOMATRIX_COMMAND_Set_All_Pins             = ${IOMATRIX_COMMAND_Set_All_Pins}
  self.IOMATRIX_COMMAND_Get_All_Pins             = ${IOMATRIX_COMMAND_Get_All_Pins}

  self.strPinStatusZ = string.char(self.PINSTATUS_HIGHZ)
  self.strPinStatus0 = string.char(self.PINSTATUS_OUTPUT0)
  self.strPinStatus1 = string.char(self.PINSTATUS_OUTPUT1)

  self.tPlugin = nil
  self.hPinDescription = nil
  self.atPinsUnderTest = nil
  self.ulLoadAddress = nil
  self.ulExecutionAddress = nil
  self.ulParameterStartAddress = nil
  self.ulParameterEndAddress = nil
  self.strStandardHeader = nil

  self.aucCompiledPins = nil
  self.uiCurrentPinIndex = nil

  self.atInBuffer = {}
  self.atOutBuffer = {}

  self.__atAsicTypToName = {
    [romloader.ROMLOADER_CHIPTYP_NETX50]        = '50',
    [romloader.ROMLOADER_CHIPTYP_NETX100]       = '500',
    [romloader.ROMLOADER_CHIPTYP_NETX500]       = '500',
    [romloader.ROMLOADER_CHIPTYP_NETX10]        = '10',
    [romloader.ROMLOADER_CHIPTYP_NETX56]        = '56',
    [romloader.ROMLOADER_CHIPTYP_NETX56B]       = '56',
    [romloader.ROMLOADER_CHIPTYP_NETX90_MPW]    = '90_mpw',
    [romloader.ROMLOADER_CHIPTYP_NETX4000_FULL] = '4000',
    [romloader.ROMLOADER_CHIPTYP_NETX90]        = '90',
  }

  -----------------------------------------------------------------------------
  --
  -- Default callback functions.
  --
  self.ulProgressLastTime    = 0
  self.fProgressLastPercent  = 0
  self.ulProgressLastMax     = nil
  self.PROGRESS_STEP_PERCENT = 10

  local tCurrentSelf = self
  self.default_callback_progress = function(ulCnt, ulMax)
    local fPercent = ulCnt * 100 / ulMax
    local ulTime = os.time()
    if tCurrentSelf.ulProgressLastMax ~= ulMax
       or ulCnt == 0
       or ulCnt == ulMax
       or tCurrentSelf.fProgressLastPercent - fPercent > tCurrentSelf.PROGRESS_STEP_PERCENT
       or ulTime - tCurrentSelf.ulProgressLastTime > 1 then
      tCurrentSelf.fProgressLastPercent = fPercent
      tCurrentSelf.ulProgressLastMax = ulMax
      tCurrentSelf.ulProgressLastTime = ulTime
      tCurrentSelf.tLog.debug('%d%% (%d/%d)', fPercent, ulCnt, ulMax)
    end
    return true
  end
  self.default_callback_message = function(a,b)
    if type(a)=='string' then
      local ar = tCurrentSelf.pl.stringx.splitlines(a, false)
      for _, s in ipairs(ar) do
        tCurrentSelf.tLog.debug('[netx] %s', s)
      end
    end
    return true
  end

  -- Set the defaults for the callback parameter.
  self.fnCallbackProgress = fnCallbackProgress or self.default_callback_progress
  self.fnCallbackMessage = fnCallbackMessage or self.default_callback_message
end



function IoMatrix_netx_base.__get_dword(strData, ulOffset)
  return (
    string.byte(strData, ulOffset) +
    string.byte(strData, ulOffset + 1) * 0x00000100 +
    string.byte(strData, ulOffset + 2) * 0x00010000 +
    string.byte(strData, ulOffset + 3) * 0x01000000
  )
end



function IoMatrix_netx_base:__chunk_add_dword(atTable, ulData)
  local bit = self.bit
  table.insert(atTable, string.char(
    bit.band(ulData, 0xff),
    bit.band(bit.rshift(ulData, 8), 0xff),
    bit.band(bit.rshift(ulData, 16), 0xff),
    bit.band(bit.rshift(ulData, 24), 0xff)
  ))
end



function IoMatrix_netx_base:initialize(tPlugin, strPattern)
  strPattern = strPattern or 'netx/iomatrix_netx%s.bin'

  -- Store the plugin.
  self.tPlugin = tPlugin

  -- Invalidate the handle for the pin description.
  self.hPinDescription = 0

  -- No pin list yet.
  self.atPinsUnderTest = nil

  -- Get the chip type.
  local tAsicTyp = tPlugin:GetChiptyp()

  -- Get the binary for the ASIC.
  local strAsic = self.__atAsicTypToName[tAsicTyp]
  if strAsic==nil then
    error('Unknown chiptyp!')
  end
  local strBinaryName = string.format(strPattern, strAsic)
  self.tLog.debug('Read the netX binary from "%s".', strBinaryName)

  -- Try to load the binary.
  local strData, strMsg = self.pl.utils.readfile(strBinaryName, true)
  if not strData then
    error(string.format('Failed to load binary "%s": %s', strBinaryName, strMsg))
  end

  -- Check the magic cookie.
  local ulMagicCookie = self.__get_dword(strData, 0 + 1)
  self.tLog.debug('Header magic of the file: 0x%08x', ulMagicCookie)
  if ulMagicCookie~=self.HEADER_MAGIC then
    error("The binary has no magic at the beginning. Is this an old binary?")
  end

  -- Check the version.
  local ulVersionMajor = self.__get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_ulVersionMajor} + 1)
  local ulVersionMinor = self.__get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_ulVersionMinor} + 1)
  local ulVersionMicro = self.__get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_ulVersionMicro} + 1)
  local strVersionVcs = string.sub(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_acVersionVcs} + 1, ${OFFSETOF_VERSION_HEADER_STRUCT_acVersionVcs} + 16)
  while string.byte(strVersionVcs,-1)==0 do
    strVersionVcs = string.sub(strVersionVcs,1,-2)
  end
  self.tLog.info('Binary version: %d.%d.%d %s', ulVersionMajor, ulVersionMinor, ulVersionMicro, strVersionVcs)
  self.tLog.info('Script version: %d.%d.%d %s', self.IOMATRIX_VERSION_MAJOR, self.IOMATRIX_VERSION_MINOR, self.IOMATRIX_VERSION_MICRO, self.IOMATRIX_VERSION_VCS)
  if ulVersionMajor~=self.IOMATRIX_VERSION_MAJOR then
    error('The major version number of the binary and this script differs. Cowardly refusing to continue.')
  end
  if ulVersionMinor~=self.IOMATRIX_VERSION_MINOR then
    error('The minor version number of the binary and this script differs. Cowardly refusing to continue.')
  end
  if ulVersionMicro~=self.IOMATRIX_VERSION_MICRO then
    error('The micro version number of the binary and this script differs. Cowardly refusing to continue.')
  end
  if strVersionVcs~=self.IOMATRIX_VERSION_VCS then
    error('The VCS version number of the binary and this script differs. Cowardly refusing to continue.')
  end

  -- Get the header from the binary.
  self.ulLoadAddress           = self.__get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_pulLoadAddress} + 1)
  self.ulExecutionAddress      = self.__get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_pfnExecutionAddress} + 1)
  self.ulParameterStartAddress = self.__get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_pulParameterStart} + 1)
  self.ulParameterEndAddress   = self.__get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_pulParameterEnd} + 1)

  -- Show the information:
  self.tLog.debug('load address:       0x%08x', self.ulLoadAddress)
  self.tLog.debug('start address:      0x%08x', self.ulExecutionAddress)
  self.tLog.debug('buffer start:       0x%08x', self.ulParameterStartAddress)
  self.tLog.debug('buffer end:         0x%08x', self.ulParameterEndAddress)

  self.tLog.debug('downloading to 0x%08x', self.ulLoadAddress)
  tPlugin:write_image(self.ulLoadAddress, strData, self.fnCallbackProgress, strData:len())

  -- Prepare the header for all commands.
  local atHeader = {}
  self:__chunk_add_dword(atHeader, 0xFFFFFFFF)
  self:__chunk_add_dword(atHeader, self.ulParameterStartAddress+0x0c)
  self:__chunk_add_dword(atHeader, 0x00000000)
  self.strStandardHeader = table.concat(atHeader)
end



function IoMatrix_netx_base:__write_header(atParameter)
  -- Create the extended header from the parameter table.
  local atHeaderExt = {}
  for _, tValue in ipairs(atParameter) do
    if type(tValue)=='number' then
      self:__chunk_add_dword(atHeaderExt, tValue)
    else
      -- Append the entry as a string.
      table.insert(atHeaderExt, tValue)
    end
  end

  -- Combine the standard header with the extended header.
  local strHeaderAll = self.strStandardHeader .. table.concat(atHeaderExt)
  local sizHeaderAll = string.len(strHeaderAll)
  self.tPlugin:write_image(self.ulParameterStartAddress, strHeaderAll, self.fnCallbackProgress, sizHeaderAll)
end



function IoMatrix_netx_base:__set_pin(ulPinIndex, tValue)
  -- Collect the parameter.
  self:__write_header{
    self.ulVerbose,                      -- Verbose mode.
    self.IOMATRIX_COMMAND_Set_Pin,       -- The command code.
    self.hPinDescription,                -- Pin description handle.
    ulPinIndex,                          -- index of the pin to set.
    tValue                               -- status of the pin to set.
  }

  -- Call the netX program.
  self.tLog.debug('__/Output/____________________________________________________________________')
  self.tPlugin:call(self.ulExecutionAddress, self.ulParameterStartAddress, self.fnCallbackMessage, 0)
  self.tLog.debug('______________________________________________________________________________')

  -- Get the result.
  local ulResult = self.tPlugin:read_data32(self.ulParameterStartAddress)
  if ulResult~=0 then
    error('Failed to set the pin.')
  end
end



function IoMatrix_netx_base:__get_pin(ulPinIndex)
  -- Collect the parameter.
  self:__write_header{
    self.ulVerbose,                      -- Verbose mode.
    self.IOMATRIX_COMMAND_Get_Pin,       -- The command code.
    self.hPinDescription,                -- Pin description handle.
    ulPinIndex,                          -- index of the pin to set.
    0                                    -- A placeholder for the result.
  }

  -- Call the netX program.
  self.tLog.debug('__/Output/____________________________________________________________________')
  self.tPlugin:call(self.ulExecutionAddress, self.ulParameterStartAddress, self.fnCallbackMessage, 0)
  self.tLog.debug('______________________________________________________________________________')

  -- Get the result.
  local ulResult = self.tPlugin:read_data32(self.ulParameterStartAddress)
  if ulResult~=0 then
    error('Failed to set the pin.')
  end
  return self.tPlugin:read_data08(self.ulParameterStartAddress+0x1c)
end



-- This is the base class. It does not know any pins.
IoMatrix_netx_base.atKnownPins = {}



function IoMatrix_netx_base:clear_pins()
  self.aucCompiledPins = {}
  self.uiCurrentPinIndex = 0
end



function IoMatrix_netx_base:add_pin(strID, strKnownName, uiDefaultValue, uiFlags)
  -- Look up the known pin name.
  local tPin = self.atKnownPins[strKnownName]
  if tPin==nil then
    self.tLog.error('Pin "%s" has an unknown name: "%s".', strID, strKnownName)
    error('Unknown pin name.')
  end

  local aucCompiledPins = self.aucCompiledPins

  -- Add the ID as a zero-terminated ASCII sequence.
  table.insert(aucCompiledPins, strID)
  table.insert(aucCompiledPins, string.char(0))
  -- Add the pin type.
  self:__chunk_add_dword(aucCompiledPins, tPin.type)
  -- Add the index.
  self:__chunk_add_dword(aucCompiledPins, tPin.index)

  -- Count the defined pins.
  self.uiCurrentPinIndex = self.uiCurrentPinIndex + 1

  -- Construct a shortcut to this pin.
  local tSelf = self
  local atInBuffer = self.atInBuffer
  local atOutBuffer = self.atOutBuffer
  local ulPinIndex = self.uiCurrentPinIndex
  local atPin = {}
  atPin.bget = function()
    return atInBuffer[ulPinIndex]
  end
  atPin.__atRB = {
    ['z'] = string.char(${PINSTATUS_HIGHZ}),
    ['Z'] = string.char(${PINSTATUS_HIGHZ}),
    ['0'] = string.char(${PINSTATUS_OUTPUT0}),
    ['1'] = string.char(${PINSTATUS_OUTPUT1})
  }
  atPin.__atRS = {
    ['z'] = ${PINSTATUS_HIGHZ},
    ['Z'] = ${PINSTATUS_HIGHZ},
    ['0'] = ${PINSTATUS_OUTPUT0},
    ['1'] = ${PINSTATUS_OUTPUT1}
  }
  atPin.bset = function(tVal)
    local strVal = tostring(tVal)
    local tRaw = string.gsub(strVal, '[01zZ]', atPin.__atRB)
    if tVal==nil then
      tSelf.tLog.error('Invalid value: "%s"', strVal)
      error('Invalid value.')
    end
    atOutBuffer[ulPinIndex] = tRaw
  end
  atPin.get = function()
    return tSelf:__get_pin(ulPinIndex-1)
  end
  atPin.set = function(tVal)
    local strVal = tostring(tVal)
    local tRaw = atPin.__atRS[strVal]
    if tVal==nil then
      error('Invalid value: "%s"', strVal)
    end
    tSelf:__set_pin(ulPinIndex-1, tRaw)
  end

  return atPin
end



function IoMatrix_netx_base:parse_pins()
  -- Get the compiled pins.
  if self.aucCompiledPins==nil then
    error('No compiled pin definition found.')
  end
  local strPinDefinition = table.concat(self.aucCompiledPins)
  local sizPinDefinition = strPinDefinition:len()

  -- Place the pin description right after the header.
  local ulPinDefinitionAddress = self.ulParameterStartAddress + 0x20

  -- Collect the parameter.
  self:__write_header{
    self.ulVerbose,                               -- Verbose mode.
    self.IOMATRIX_COMMAND_Parse_Pin_Description,  -- The command code.
    ulPinDefinitionAddress,                       -- Start address of the pin definition.
    sizPinDefinition,                             -- Size of the pin definition in bytes.
    0                                             -- Placeholder for the handle.
  }

  -- Download the compiled pin description.
  self.tLog.debug('downloading to 0x%08x', ulPinDefinitionAddress)
  self.tPlugin:write_image(ulPinDefinitionAddress, strPinDefinition, self.fnCallbackProgress, sizPinDefinition)

  -- Call the netX program.
  self.tLog.debug('__/Output/____________________________________________________________________')
  self.tPlugin:call(self.ulExecutionAddress, self.ulParameterStartAddress, self.fnCallbackMessage, 0)
  self.tLog.debug('______________________________________________________________________________')

  -- Get the result.
  local ulResult = self.tPlugin:read_data32(self.ulParameterStartAddress)
  if ulResult~=0 then
    error('The netX program returned an error!')
  end

  -- Get the pin description handle.
  self.hPinDescription = self.tPlugin:read_data32(self.ulParameterStartAddress+0x1c)
  self.tLog.debug('pin desc handle: %08x', self.hPinDescription)
end



function IoMatrix_netx_base:set_all_pins_raw(strRawPinStates)
  local tResult

  -- Collect the parameter.
  self:__write_header{
    self.ulVerbose,                      -- Verbose mode.
    self.IOMATRIX_COMMAND_Set_All_Pins,  -- The command code.
    self.hPinDescription,                -- Pin description handle.
    strRawPinStates
  }

  -- Call the netX program.
  self.tLog.debug('__/Output/____________________________________________________________________')
  self.tPlugin:call(self.ulExecutionAddress, self.ulParameterStartAddress, self.fnCallbackMessage, 0)
  self.tLog.debug('______________________________________________________________________________')

  -- Get the result.
  local ulResult = self.tPlugin:read_data32(self.ulParameterStartAddress)
  if ulResult==0 then
    tResult = true
  end

  return tResult
end



IoMatrix_netx_base.__atAsciiToPinstate = {
  ['z'] = string.char(${PINSTATUS_HIGHZ}),
  ['Z'] = string.char(${PINSTATUS_HIGHZ}),
  ['0'] = string.char(${PINSTATUS_OUTPUT0}),
  ['1'] = string.char(${PINSTATUS_OUTPUT1})
}


function IoMatrix_netx_base:set_all_pins(strPinStates)
  -- Convert all pin states.
  local strRawPinStates = string.gsub(tostring(strPinStates), '[01zZ]', self.__atAsciiToPinstate)

  return self:set_all_pins_raw(strRawPinStates)
end



function IoMatrix_netx_base:get_all_pins_raw()
  -- Collect the parameter.
  self:__write_header{
    self.ulVerbose,                      -- Verbose mode.
    self.IOMATRIX_COMMAND_Get_All_Pins,  -- The command code.
    self.hPinDescription                 -- Pin description handle.
  }

  -- Call the netX program.
  self.tLog.debug('__/Output/____________________________________________________________________')
  self.tPlugin:call(self.ulExecutionAddress, self.ulParameterStartAddress, self.fnCallbackMessage, 0)
  self.tLog.debug('______________________________________________________________________________')

  -- Get the result.
  local strRawStates
  local sizDataBlock = 0x18 + self.uiCurrentPinIndex
  local strResult = self.tPlugin:read_image(self.ulParameterStartAddress, sizDataBlock, self.fnCallbackProgress, sizDataBlock)
  local ulResult = self.__get_dword(strResult, 1)
  if ulResult==0 then
    -- Extract all pin states.
    strRawStates = string.sub(strResult, 0x18+1)
  end

  return strRawStates
end



IoMatrix_netx_base.__atPinstateToAscii = {
  [string.char(0)] = '0',
  [string.char(1)] = '1'
}



function IoMatrix_netx_base:get_all_pins()
  local strAsciiStates
  local strRawPinStates = self:get_all_pins_raw()
  if strRawPinStates~=nil then
    -- Convert all pin states.
    strAsciiStates = string.gsub(strRawPinStates, '.', self.__atPinstateToAscii)
  end

  return strAsciiStates
end



function IoMatrix_netx_base:start_testing()
  -- Call the init function if it exists.
  if self.fnInit~=nil then
    self.fnInit(self.tPlugin)
  end
end



function IoMatrix_netx_base:stop_testing()
  -- Call the init function if it exists.
  if self.fnDenit~=nil then
    self.fnDenit(self.tPlugin)
  end
end



function IoMatrix_netx_base:setZ()
  -- Set the complete buffer to 'z'.
  for iCnt = 1, self.uiCurrentPinIndex do
    self.atOutBuffer[iCnt] = string.char(${PINSTATUS_HIGHZ})
  end
end



function IoMatrix_netx_base:bset()
  -- Concat all elements of the buffer.
  local strRawPinStates = table.concat(self.atOutBuffer)

  local tResult = self:set_all_pins_raw(strRawPinStates)
  if tResult==nil then
    error('Failed to set all pins in raw mode.')
  end
end



function IoMatrix_netx_base:bget()
  local strRawPinStates = self:get_all_pins_raw()
  if strRawPinStates==nil then
    error('Failed to get all pins in raw mode.')
  end
  local sizRawPinStates = string.len(strRawPinStates)
  for iPos = 1, sizRawPinStates do
    self.atInBuffer[iPos] = string.byte(strRawPinStates, iPos)
  end
end



return IoMatrix_netx_base
