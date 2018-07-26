module("io_matrix", package.seeall)

-----------------------------------------------------------------------------
--   Copyright (C) 2014 by Christoph Thelen                                --
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


-- TODO: document the functions with doxygen. See here for details: ftp://ftp.tex.ac.uk/ctan%3A/web/lua2dox/lua2dox-refm.pdf

require("bit")
require("muhkuh")
require("romloader")


-----------------------------------------------------------------------------
--                           Definitions                                   --
-----------------------------------------------------------------------------

IOMATRIX_VERSION_MAJOR  = ${VERSION_MAJOR}
IOMATRIX_VERSION_MINOR  = ${VERSION_MINOR}
IOMATRIX_VERSION_MICRO  = ${VERSION_MICRO}
IOMATRIX_VERSION_VCS  = ${VERSION_VCS}

PINTYPE_GPIO          = ${PINTYPE_GPIO}
PINTYPE_PIO           = ${PINTYPE_PIO}
PINTYPE_MMIO          = ${PINTYPE_MMIO}
PINTYPE_HIFPIO        = ${PINTYPE_HIFPIO}
PINTYPE_RDYRUN        = ${PINTYPE_RDYRUN}
PINTYPE_RSTOUT        = ${PINTYPE_RSTOUT}

PINSTATUS_HIGHZ       = ${PINSTATUS_HIGHZ}
PINSTATUS_OUTPUT0     = ${PINSTATUS_OUTPUT0}
PINSTATUS_OUTPUT1     = ${PINSTATUS_OUTPUT1}

PINFLAG_I             = ${PINFLAG_I}
PINFLAG_O             = ${PINFLAG_O}
PINFLAG_Z             = ${PINFLAG_Z}
PINFLAG_IOZ           = ${PINFLAG_IOZ}

HEADER_MAGIC          = 0x686f6f6d

IOMATRIX_COMMAND_Parse_Pin_Description    = ${IOMATRIX_COMMAND_Parse_Pin_Description}
IOMATRIX_COMMAND_Run_Matrix_Test          = ${IOMATRIX_COMMAND_Run_Matrix_Test}
IOMATRIX_COMMAND_Set_Pin                  = ${IOMATRIX_COMMAND_Set_Pin}
IOMATRIX_COMMAND_Get_Pin                  = ${IOMATRIX_COMMAND_Get_Pin}


-----------------------------------------------------------------------------
--
-- Default callback functions.
--
local ulProgressLastTime    = 0
local fProgressLastPercent  = 0
local ulProgressLastMax     = nil
PROGRESS_STEP_PERCENT       = 10

function default_callback_progress(ulCnt, ulMax)
	local fPercent = ulCnt * 100 / ulMax
	local ulTime = os.time()
	if ulProgressLastMax ~= ulMax
	   or ulCnt == 0
	   or ulCnt == ulMax
	   or fProgressLastPercent - fPercent > PROGRESS_STEP_PERCENT
	   or ulTime - ulProgressLastTime > 1 then
		fProgressLastPercent = fPercent
		ulProgressLastMax = ulMax
		ulProgressLastTime = ulTime
		print(string.format("%d%% (%d/%d)", fPercent, ulCnt, ulMax))
	end
	return true
end


function default_callback_message(a,b)
	if type(a)=="string" then
		io.write("[netx] ")
		local strLastChar = string.sub(a, -1)
		if strLastChar == "\r" or strLastChar == "\n" then
			io.write(a)
		else
			print(a)
		end
	end
	return true
end

-----------------------------------------------------------------------------
--
-- Compiler for the pin description and the net list.
--
local function chunk_add_dword(atTable, ulData)
	table.insert(atTable, string.char(bit.band(ulData, 0xff), bit.band(bit.rshift(ulData, 8), 0xff), bit.band(bit.rshift(ulData, 16), 0xff), bit.band(bit.rshift(ulData, 24), 0xff)))
end



local function find_pin_name(atPinsUnderTest, strName)
	local ulIdx = nil
	
	
	-- Loop over all pin descriptions.
	for ulCnt,atPin in ipairs(atPinsUnderTest) do
		if atPin[1]==strName then
			ulIdx = ulCnt
			break
		end
	end
	
	return ulIdx	
end



local function get_dword(strData, ulOffset)
	return string.byte(strData,ulOffset) + string.byte(strData,ulOffset+1)*0x00000100 + string.byte(strData,ulOffset+2)*0x00010000 + string.byte(strData,ulOffset+3)*0x01000000
end



local function compile_pin_definition(atPinsUnderTest)
	-- This table collects all chunks for the compiled pin description.
	local atChunks = {}

	-- Loop over all pin descriptions.
	for iPinCnt,atPin in ipairs(atPinsUnderTest) do
		-- Add the name as a zero-terminated ASCII sequence.
		table.insert(atChunks, atPin[1])
		table.insert(atChunks, string.char(0))
		-- Add the pin type.
		chunk_add_dword(atChunks, atPin[2])
		-- Add the index.
		chunk_add_dword(atChunks, atPin[3])
		-- Add the default value.
		chunk_add_dword(atChunks, atPin[4])
		-- Add the flags.
		chunk_add_dword(atChunks, atPin[5])
	end

	return table.concat(atChunks)
end



local function compile_net_list(atPinsUnderTest, apcNetListNames)
	-- This table collects all chunks for the compiled net list.
	local atChunks = {}
	
	-- Add all network descriptions.
	for iNetworkCnt,atNetwork in ipairs(apcNetListNames) do
		-- Loop over all pins in the network.
		for iPinCnt,strPinName in ipairs(atNetwork) do
			ulIdx = find_pin_name(atPinsUnderTest, strPinName)
			if ulIdx==nil then
				error(string.format("Error in network %d: pin '%s' not found in pin description!", iPinCnt-1, strPinName))
			end
			chunk_add_dword(atChunks, ulIdx)
		end
		-- Terminate the network.
		chunk_add_dword(atChunks, 0)
	end
	
	return table.concat(atChunks)
end



function initialize(tPlugin, strPattern, fnCallbackProgress)
	-- Set the defaults for the optional parameter.
	fnCallbackProgress = fnCallbackProgress or default_callback_progress  -- Use the built-in callback.

	-- Get the chip type.
	local tAsicTyp = tPlugin:GetChiptyp()
	local aAttr = {}
	local uiAsic
	local tDefaultBus


	-- Set the plugin in the attributes.
	aAttr.tPlugin = tPlugin
	
	-- Invalidate the handle for the pin description.
	aAttr.hPinDescription = 0

	-- No pin list yet.
	aAttr.atPinsUnderTest = nil
	
	-- Get the binary for the ASIC.
	if tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX50 then
		strAsic = '50'
	elseif tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX100 or tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX500 then
		strAsic = '500'
	elseif tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX10 then
		strAsic = '10'
	elseif tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX56 or tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX56B then
		strAsic = '56'
	elseif tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX90_MPW then
		strAsic = '90_mpw'
	else
		error("Unknown chiptyp!")
	end

	strBinaryName = string.format(strPattern, strAsic)

	-- Try to load the binary.
	strData, strMsg = muhkuh.load(strBinaryName)
	if not strData then
		error(string.format("Failed to load binary '%s': ", strBinaryName, strMsg))
	end

	-- Check the magic cookie.
	ulMagicCookie = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_acMagic} + 1)
	if ulMagicCookie~=HEADER_MAGIC then
		error("The binary has no magic at the beginning. Is this an old binary?")
	end

	-- Check the version.
	ulVersionMajor = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_ulVersionMajor} + 1)
	ulVersionMinor = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_ulVersionMinor} + 1)
	ulVersionMicro = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_ulVersionMicro} + 1)
	strVersionVcs = string.sub(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_acVersionVcs} + 1, ${OFFSETOF_VERSION_HEADER_STRUCT_acVersionVcs} + 16)
	while string.byte(strVersionVcs,-1)==0 do
		strVersionVcs = string.sub(strVersionVcs,1,-2)
	end
	print(string.format("Binary version: %d.%d.%d %s", ulVersionMajor, ulVersionMinor, ulVersionMicro, strVersionVcs))
	print(string.format("Script version: %d.%d.%d %s", IOMATRIX_VERSION_MAJOR, IOMATRIX_VERSION_MINOR, IOMATRIX_VERSION_MICRO, IOMATRIX_VERSION_VCS))
	if ulVersionMajor~=IOMATRIX_VERSION_MAJOR then
		error("The major version number of the binary and this script differs. Cowardly refusing to continue.")
	end
	if ulVersionMinor~=IOMATRIX_VERSION_MINOR then
		error("The minor version number of the binary and this script differs. Cowardly refusing to continue.")
	end
	if ulVersionMicro~=IOMATRIX_VERSION_MICRO then
		error("The micro version number of the binary and this script differs. Cowardly refusing to continue.")
	end
	if strVersionVcs~=IOMATRIX_VERSION_VCS then
		error("The VCS version number of the binary and this script differs. Cowardly refusing to continue.")
	end

	-- Get the header from the binary.
	aAttr.ulLoadAddress           = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_pulLoadAddress} + 1)
	aAttr.ulExecutionAddress      = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_pfnExecutionAddress} + 1)
	aAttr.ulParameterStartAddress = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_pulParameterStart} + 1)
	aAttr.ulParameterEndAddress   = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_pulParameterEnd} + 1)

	-- Show the information:
	print(string.format("load address:       0x%08x", aAttr.ulLoadAddress))
	print(string.format("start address:      0x%08x", aAttr.ulExecutionAddress))
	print(string.format("buffer start:       0x%08x", aAttr.ulParameterStartAddress))
	print(string.format("buffer end:         0x%08x", aAttr.ulParameterEndAddress))

	print(string.format("downloading to 0x%08x", aAttr.ulLoadAddress))
	tPlugin:write_image(aAttr.ulLoadAddress, strData, fnCallbackProgress, strData:len())

	return aAttr
end



function parse_pin_description(aAttr, atPinsUnderTest, ulVerbose, fnCallbackProgress, fnCallbackMessage)
	aAttr.atPinsUnderTest = atPinsUnderTest
	
	-- Set the defaults for the optional parameter.
	ulVerbose = ulVerbose or 0                                            -- Disable verbose mode by default.
	fnCallbackProgress = fnCallbackProgress or default_callback_progress  -- Use the built-in callback.
	fnCallbackMessage = fnCallbackMessage or default_callback_message     -- Use the built-in callback.

	-- Compile the pin definition.
	local strPinDefinition = compile_pin_definition(atPinsUnderTest)
	local sizPinDefinition = strPinDefinition:len()

	-- Place the pin description right after the header. 
	local ulPinDefinitionAddress = aAttr.ulParameterStartAddress+0x20
	
	-- Collect the parameter.
	local atParameters = {
		ulVerbose,                               -- Verbose mode.
		IOMATRIX_COMMAND_Parse_Pin_Description,  -- The command code.
		ulPinDefinitionAddress,                  -- Start address of the pin definition.
		sizPinDefinition,                        -- Size of the pin definition in bytes.
		0                                        -- Placeholder for the handle.
	}
	
	-- Write the standard header.
	aAttr.tPlugin:write_data32(aAttr.ulParameterStartAddress+0x00, 0xFFFFFFFF)                          -- Init the test result.
	aAttr.tPlugin:write_data32(aAttr.ulParameterStartAddress+0x04, aAttr.ulParameterStartAddress+0x0c)  -- Address of test parameters.
	aAttr.tPlugin:write_data32(aAttr.ulParameterStartAddress+0x08, 0x00000000)                          -- Reserved
	
	-- Write the extended header.
	for iIdx,ulValue in ipairs(atParameters) do
		aAttr.tPlugin:write_data32(aAttr.ulParameterStartAddress+0x0c+((iIdx-1)*4), ulValue)
	end

	-- Download the compiled pin description.
	print(string.format("downloading to 0x%08x", ulPinDefinitionAddress))
	aAttr.tPlugin:write_image(ulPinDefinitionAddress, strPinDefinition, fnCallbackProgress, sizPinDefinition)
	
	-- Call the netX program.
	 print("__/Output/____________________________________________________________________")
	aAttr.tPlugin:call(aAttr.ulExecutionAddress, aAttr.ulParameterStartAddress, fnCallbackMessage, 0)
	print("")
	print("______________________________________________________________________________")
	
	-- Get the result.
	local ulResult = aAttr.tPlugin:read_data32(aAttr.ulParameterStartAddress)
	if ulResult~=0 then
		error("The netX program returned an error!")
	end
	
	-- Get the pin description handle.
	aAttr.hPinDescription = aAttr.tPlugin:read_data32(aAttr.ulParameterStartAddress+0x1c)
	print(string.format("pin desc handle: %08x", aAttr.hPinDescription))
end



function run_matrix_test(aAttr, atNetList, ulVerbose, fnCallbackProgress, fnCallbackMessage)
	-- Set the defaults for the optional parameter.
	ulVerbose = ulVerbose or 0                                            -- Disable verbose mode by default.
	fnCallbackProgress = fnCallbackProgress or default_callback_progress  -- Use the built-in callback.
	fnCallbackMessage = fnCallbackMessage or default_callback_message     -- Use the built-in callback.

	-- Compile the net list.
	local strNetList = compile_net_list(aAttr.atPinsUnderTest, atNetList)
	local sizNetList = strNetList:len()

	-- Place the pin description right after the header. 
	local ulNetListAddress = aAttr.ulParameterStartAddress+0x20
	
	-- Collect the parameter.
	local atParameters = {
		ulVerbose,                           -- Verbose mode.
		IOMATRIX_COMMAND_Run_Matrix_Test,    -- The command code.
		aAttr.hPinDescription,               -- Pin description handle.
		ulNetListAddress,                    -- Start address of the net list.
		sizNetList                           -- Size of the net list in bytes.
	}
	
	-- Write the standard header.
	aAttr.tPlugin:write_data32(aAttr.ulParameterStartAddress+0x00, 0xFFFFFFFF)                          -- Init the test result.
	aAttr.tPlugin:write_data32(aAttr.ulParameterStartAddress+0x04, aAttr.ulParameterStartAddress+0x0c)  -- Address of test parameters.
	aAttr.tPlugin:write_data32(aAttr.ulParameterStartAddress+0x08, 0x00000000)                          -- Reserved
	
	-- Write the extended header.
	for iIdx,ulValue in ipairs(atParameters) do
		aAttr.tPlugin:write_data32(aAttr.ulParameterStartAddress+0x0c+((iIdx-1)*4), ulValue)
	end

	-- Download the compiled net list.
	print(string.format("downloading to 0x%08x", ulNetListAddress))
	aAttr.tPlugin:write_image(ulNetListAddress, strNetList, fnCallbackProgress, sizNetList)
	
	-- Call the netX program.
	print("__/Output/____________________________________________________________________")
	aAttr.tPlugin:call(aAttr.ulExecutionAddress, aAttr.ulParameterStartAddress, fnCallbackMessage, 0)
	print("")
	print("______________________________________________________________________________")
	
	-- Get the result.
	local ulResult = aAttr.tPlugin:read_data32(aAttr.ulParameterStartAddress)
	return ulResult
end

function getIndexByName(aAttr, strPinName)
	local uiCounter = 1 
	local uiIndex 
	
	while(aAttr.atPinsUnderTest[uiCounter] ~= nil) do 
		if aAttr.atPinsUnderTest[uiCounter][1] == strPinName then 
			uiIndex = uiCounter - 1 
		end 
		uiCounter = uiCounter + 1 
	end 
	
	if uiIndex == nil then 
		print("Pin name not found in net table.")
	end 
	return uiIndex 
end 

function set_pin(aAttr, ulPinIndex_strName, tStatus, ulVerbose, fnCallbackProgress, fnCallbackMessage)
	-- Set the defaults for the optional parameter.
	ulVerbose = ulVerbose or 0                                            -- Disable verbose mode by default.
	fnCallbackProgress = fnCallbackProgress or default_callback_progress  -- Use the built-in callback.
	fnCallbackMessage = fnCallbackMessage or default_callback_message     -- Use the built-in callback.
	
	-- local ulPinIndex
	local ulPinIndex 
	
	-- Check if ulPinIndex is a string, if yes extract your index.
	if(type(ulPinIndex_strName) == "string") then 
		ulPinIndex = getIndexByName(aAttr, ulPinIndex_strName)
		if ulPinIndex==nil then
			error(string.format("Pin name not found: '%s'", ulPinIndex_strName))
		end
	else
	-- ulPinIndex was actually given as a number. 
		ulPinIndex = ulPinIndex_strName
	end 
	
	-- Collect the parameter.
	local atParameters = {
		ulVerbose,                           -- Verbose mode.
		IOMATRIX_COMMAND_Set_Pin,    		 -- The command code.
		aAttr.hPinDescription,               -- Pin description handle.
		ulPinIndex,                   		 -- index of the pin to set. 
		tStatus                              -- status of the pin to set. 
	}
	
	-- Write the standard header.
	aAttr.tPlugin:write_data32(aAttr.ulParameterStartAddress+0x00, 0xFFFFFFFF)                          -- Init the test result.
	aAttr.tPlugin:write_data32(aAttr.ulParameterStartAddress+0x04, aAttr.ulParameterStartAddress+0x0c)  -- Address of test parameters.
	aAttr.tPlugin:write_data32(aAttr.ulParameterStartAddress+0x08, 0x00000000)                          -- Reserved
	
	-- Write the extended header.
	for iIdx,ulValue in ipairs(atParameters) do
		aAttr.tPlugin:write_data32(aAttr.ulParameterStartAddress+0x0c+((iIdx-1)*4), ulValue)
	end

	-- Call the netX program.
	print("__/Output/____________________________________________________________________")
	aAttr.tPlugin:call(aAttr.ulExecutionAddress, aAttr.ulParameterStartAddress, fnCallbackMessage, 0)
	print("")
	print("______________________________________________________________________________")
	
	-- Get the result.
	local ulResult = aAttr.tPlugin:read_data32(aAttr.ulParameterStartAddress)
	return ulResult
end

