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

require("bit")
require("muhkuh")
require("romloader")


-----------------------------------------------------------------------------
--                           Definitions                                   --
-----------------------------------------------------------------------------

IOMATRIX_VERSION_MAJ  = ${VERSION_MAJ}
IOMATRIX_VERSION_MIN  = ${VERSION_MIN}
IOMATRIX_VERSION_VCS  = ${VERSION_VCS}

PINTYPE_GPIO       = ${PINTYPE_GPIO}
PINTYPE_PIO        = ${PINTYPE_PIO}
PINTYPE_MMIO       = ${PINTYPE_MMIO}
PINTYPE_HIFPIO     = ${PINTYPE_HIFPIO}
PINTYPE_RSTOUT     = ${PINTYPE_RSTOUT}

PINSTATUS_HIGHZ    = ${PINSTATUS_HIGHZ}
PINSTATUS_OUTPUT0  = ${PINSTATUS_OUTPUT0}
PINSTATUS_OUTPUT1  = ${PINSTATUS_OUTPUT1}

PINFLAG_I          = ${PINFLAG_I}
PINFLAG_O          = ${PINFLAG_O}
PINFLAG_Z          = ${PINFLAG_Z}
PINFLAG_IOZ        = ${PINFLAG_IOZ}

HEADER_MAGIC         = 0x686f6f6d

-----------------------------------------------------------------------------

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


function compile_matrix(atPinsUnderTest, apcNetListNames)
	-- This table collects all chunks for the compiled pin list.
	local atPinsChunks = {}

	-- Loop over all pin descriptions.
	for iPinCnt,atPin in ipairs(atPinsUnderTest) do
		-- Add the name as a zero-terminated ASCII sequence.
		table.insert(atPinsChunks, atPin[1])
		table.insert(atPinsChunks, string.char(0))
		-- Add the pin type.
		chunk_add_dword(atPinsChunks, atPin[2])
		-- Add the index.
		chunk_add_dword(atPinsChunks, atPin[3])
		-- Add the default value.
		chunk_add_dword(atPinsChunks, atPin[4])
		-- Add the flags.
		chunk_add_dword(atPinsChunks, atPin[5])
	end

	-- Terminate the table with a zero byte.
	table.insert(atPinsChunks, string.char(0))
	
	-- Add all network descriptions.
	for iNetworkCnt,atNetwork in ipairs(apcNetListNames) do
		-- Loop over all pins in the network.
		for iPinCnt,strPinName in ipairs(atNetwork) do
			ulIdx = find_pin_name(atPinsUnderTest, strPinName)
			if ulIdx==nil then
				error(string.format("Error in network %d: pin '%s' not found in pin description!", iPinCnt-1, strPinName))
			end
			chunk_add_dword(atPinsChunks, ulIdx)
		end
		-- Terminate the network.
		chunk_add_dword(atPinsChunks, 0)
	end
	-- Terminate the network description.
	chunk_add_dword(atPinsChunks, 0)
	
	return table.concat(atPinsChunks)
end


function download(tPlugin, strPattern, fnCallback)
	-- Get the chip type.
	local tAsicTyp = tPlugin:GetChiptyp()
	local aAttr = {}
	local uiAsic
	local tDefaultBus


	-- Get the binary for the ASIC.
	if tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX50 then
		uiAsic = 50
	elseif tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX100 or tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX500 then
		uiAsic = 500
	elseif tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX10 then
		uiAsic = 10
	elseif tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX56 or tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX56B then
		uiAsic = 56
	else
		error("Unknown chiptyp!")
	end

	strBinaryName = string.format(strPattern, uiAsic)

	-- Try to load the binary.
	strData, strMsg = muhkuh.load(strBinaryName)
	if not strData then
		error(string.format("Failed to load binary '%s': ", strBinaryName, strMsg))
	end

	-- Check the magic cookie.
	ulMagicCookie = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_acMagic} + 1)
	if ulMagicCookie~=HEADER_MAGIC then
		error("The binary has no crypto magic at the beginning. Is this an old binary?")
	end

	-- Check the version.
	ulVersionMaj = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_ulVersionMaj} + 1)
	ulVersionMin = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_ulVersionMin} + 1)
	strVersionVcs = string.sub(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_acVersionVcs} + 1, ${OFFSETOF_VERSION_HEADER_STRUCT_acVersionVcs} + 16)
	while string.byte(strVersionVcs,-1)==0 do
		strVersionVcs = string.sub(strVersionVcs,1,-2)
	end
	print(string.format("Binary version: %d.%d.%s", ulVersionMaj, ulVersionMin, strVersionVcs))
	print(string.format("Script version: %d.%d.%s", IOMATRIX_VERSION_MAJ, IOMATRIX_VERSION_MIN, IOMATRIX_VERSION_VCS))
	if ulVersionMaj~=IOMATRIX_VERSION_MAJ then
		error("The major version number of the binary and this script differs. Cowardly refusing to continue.")
	end
	if ulVersionMin~=IOMATRIX_VERSION_MIN then
		error("The minor version number of the binary and this script differs. Cowardly refusing to continue.")
	end
	if strVersionVcs~=IOMATRIX_VERSION_VCS then
		error("The VCS version number of the binary and this script differs. Cowardly refusing to continue.")
	end

	-- Get the header from the binary.
	ulLoadAddress     = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_pulLoadAddress} + 1)
	ulStartAddress    = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_pfnExecutionAddress} + 1)
	ulParameterStartAddress = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_pulParameterStart} + 1)
	ulParameterEndAddress   = get_dword(strData, ${OFFSETOF_VERSION_HEADER_STRUCT_pulParameterEnd} + 1)

	-- Show the information:
	print(string.format("load address:       0x%08x", ulLoadAddress))
	print(string.format("start address:      0x%08x", ulStartAddress))
	print(string.format("buffer start:       0x%08x", ulParameterStartAddress))
	print(string.format("buffer end:         0x%08x", ulParameterEndAddress))
end

