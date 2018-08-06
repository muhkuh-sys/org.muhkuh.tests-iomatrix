local class = require 'pl.class'
local TestClassIoMatrix = class()


function TestClassIoMatrix:_init(strTestName)
  self.bit = require 'bit'
  self.io_matrix = require 'io_matrix'
  self.lxp = require 'lxp'
  self.parameters = require 'parameters'
  self.pl = require'pl.import_into'()

  self.CFG_strTestName = strTestName

  self.CFG_aParameterDefinitions = {
    {
      name="definition",
      default=nil,
      help="The file name of the XML test definition.",
      mandatory=true,
      validate=nil,
      constrains=nil
    }
  }

  self.atDevicesNetx = nil
  self.atDevicesFTDI = nil
  self.atPins = nil
  self.atMatrix = nil
end



--- Expat callback function for starting an element.
-- This function is part of the callbacks for the expat XML parser.
-- It is called when a new element is opened.
-- @param tParser The parser object.
-- @param strName The name of the element.
-- @param atAttributes A table with all attributes of the element.
function TestClassIoMatrix.parseCfg_StartElement(tParser, strName, atAttributes)
  -- Get the user parameter from the expat parser. This is a table where we
  -- store the results and the current path in the XML.
  local aLxpAttr = tParser:getcallbacks().userdata
  -- Get the position in the XML text file for error messages.
  local iPosLine, iPosColumn = tParser:pos()

  -- Append the new element to the current path.
  table.insert(aLxpAttr.atCurrentPath, strName)
  local strCurrentPath = table.concat(aLxpAttr.atCurrentPath, "/")
  aLxpAttr.strCurrentPath = strCurrentPath

  -- Compare the current path with the expected locations.
  if strCurrentPath=="/IoMatrixTest/Devices/netX" then
    -- Get the "plugin" attribute. It must be there.
    local strPlugin = atAttributes['plugin']
    if strPlugin==nil then
      aLxpAttr.tResult = nil
      aLxpAttr.tLog.error('Error in line %d, col %d: missing attribute "plugin".', iPosLine, iPosColumn)
    else
      -- Does a netX device with the specified plugin already exist?
      local tDev
      for _, tDevice in ipairs(aLxpAttr.atDevicesNetx) do
        if tDevice.plugin==strPlugin then
          tDev = tDevice
          break
        end
      end
      if tDev~=nil then
        aLxpAttr.tResult = nil
        aLxpAttr.tLog.error('Error in line %d, col %d: plugin "%s" is already defined.', iPosLine, iPosColumn, strPlugin)
      else
        -- Create a new netX device.
        local atDevice = {
          plugin=strPlugin
        }
        table.insert(aLxpAttr.atDevicesNetx, atDevice)
        aLxpAttr.tCurrentNetxDevice = atDevice
      end
    end

  elseif strCurrentPath=="/IoMatrixTest/Devices/FTDI" then
    local fOk = true

    -- Get the "vendor" attribute. It must be there.
    local usVendor
    local strVendor = atAttributes['vendor']
    if strVendor==nil then
      fOk = nil
      aLxpAttr.tLog.error('Error in line %d, col %d: missing attribute "vendor".', iPosLine, iPosColumn)
    else
      usVendor = tonumber(strVendor)
      if usVendor==nil then
        fOk = nil
        aLxpAttr.tLog.error('Error in line %d, col %d: attribute "vendor" is no number: "%s".', iPosLine, iPosColumn, strVendor)
      end
    end

    -- Get the "product" attribute. It must be there.
    local usProduct
    local strProduct = atAttributes['product']
    if strProduct==nil then
      fOk = nil
      aLxpAttr.tLog.error('Error in line %d, col %d: missing attribute "product".', iPosLine, iPosColumn)
    else
      usProduct = tonumber(strProduct)
      if usProduct==nil then
        fOk = nil
        aLxpAttr.tLog.error('Error in line %d, col %d: attribute "product" is no number: "%s".', iPosLine, iPosColumn, strProduct)
      end
    end

    -- Get the optional "moduleIndexMask" attribute.
    local ulModuleIndexMask
    local ulModuleIndexShift
    local strModuleIndexMask = atAttributes['moduleIndexMask']
    if strModuleIndexMask~=nil then
      ulModuleIndexMask = tonumber(strModuleIndexMask)
      if ulModuleIndexMask==nil then
        fOk = nil
        aLxpAttr.tLog.error('Error in line %d, col %d: attribute "moduleIndexMask" is no number: "%s".', iPosLine, iPosColumn, strModuleIndexMask)
      elseif ulModuleIndexMask==0 then
        fOk = nil
        aLxpAttr.tLog.error('Error in line %d, col %d: the "moduleIndexMask" must not be 0.', iPosLine, iPosColumn, strModuleIndexMask)
      else
        -- Get the shift value from the mask.
        for uiBitCnt = 0, 31 do
          if aLxpAttr.bit.band(ulModuleIndexMask, bit.lshift(1, uiBitCnt))~=0 then
            ulModuleIndexShift = uiBitCnt
            break
          end
        end
        if ulModuleIndexShift==nil then
          fOk = nil
          aLxpAttr.tLog.error('Error in line %d, col %d: failed to get the shift value for a "moduleIndexMask" of 0x%08x.', iPosLine, iPosColumn, ulModuleIndexMask)
        end
      end
    end

    if fOk~=true then
      aLxpAttr.tResult = nil
    else
      -- Create a new FTDI device.
      local atDevice = {
        usUSBVendor = usVendor,
        usUSBProduct = usProduct,
        ulModuleIndexMask = ulModuleIndexMask,
        ulModuleIndexShift = ulModuleIndexShift
      }
      table.insert(aLxpAttr.atDevicesFTDI, atDevice)
      aLxpAttr.tCurrentFTDIDevice = atDevice
    end

  elseif strCurrentPath=="/IoMatrixTest/Devices/FTDI/Device" then
    local fOk = true

    local auiPort
    local strPort = atAttributes['port']
    if strPort~=nil then
      -- Split the string by comma.
      local atPort = aLxpAttr.pl.stringx.split(strPort, ',')
      -- Loop over all elements and convert them to numbers.
      auiPort = {}
      for uiPortIdx, strPort in ipairs(atPort) do
        local uiPort = tonumber(strPort)
        if uiPort==nil then
          fOk = nil
          aLxpAttr.tLog.error('Error in line %d, col %d: element %d of the "port" list is no number.', iPosLine, iPosColumn, uiPortIdx)
          break
        else
          table.insert(auiPort, uiPort)
        end
      end
    end

    local strSerial = atAttributes['serial']

    local uiModuleIndex
    local strModuleIndex = atAttributes['moduleIndex']
    if strModuleIndex~=nil then
      uiModuleIndex = tonumber(strModuleIndex)
      if uiModuleIndex==nil then
        fOk = nil
        aLxpAttr.tLog.error('Error in line %d, col %d: attribute "moduleIndex" is no number: "%s".', iPosLine, iPosColumn, strModuleIndex)
      end
    end

    if fOk~=true then
      aLxpAttr.tResult = nil
    elseif auiPort==nil and strSerial==nil and uiModuleIndex==nil then
      aLxpAttr.tResult = nil
      aLxpAttr.tLog.error('Error in line %d, col %d: attribute "moduleIndex" is no number: "%s".', iPosLine, iPosColumn, strModuleIndex)
    else
      local atDevice = {
        port = auiPort,
        serial = strSerial,
        moduleidx = uiModuleIndex
      }
      table.insert(aLxpAttr.tCurrentFTDIDevice, atDevice)
    end

  elseif strCurrentPath=="/IoMatrixTest/PinDescription/Pin" then
    local fOk = true

    -- Get the ID.
    local strID = atAttributes['id']
    if strID==nil then
      fOk = nil
      aLxpAttr.tLog.error('Error in line %d, col %d: missing attribute "id".', iPosLine, iPosColumn)
    end

    -- Get the family.
    local uiFamily
    local strFamily = atAttributes['family']
    if strFamily==nil then
      fOk = nil
      aLxpAttr.tLog.error('Error in line %d, col %d: missing attribute "family".', iPosLine, iPosColumn)
    else
      uiFamily = aLxpAttr.atFamilies[strFamily]
      if uiFamily==nil then
        fOk = nil
        aLxpAttr.tLog.error('Error in line %d, col %d: unknown family "%s".', iPosLine, iPosColumn, strFamily)
      end
    end

    -- Get the family index.
    local uiFamilyIndex
    local strFamilyIndex = atAttributes['familyIndex']
    if strFamilyIndex==nil then
      fOk = nil
      aLxpAttr.tLog.error('Error in line %d, col %d: missing attribute "familyIndex".', iPosLine, iPosColumn)
    else
      uiFamilyIndex = tonumber(strFamilyIndex)
      if uiFamilyIndex==nil then
        fOk = nil
        aLxpAttr.tLog.error('Error in line %d, col %d: attribute "familyIndex" is no number: "%s".', iPosLine, iPosColumn, strFamilyIndex)
      end
    end

    -- Get the pin.
    local strPin = atAttributes['pin']
    if strPin==nil then
      fOk = nil
      aLxpAttr.tLog.error('Error in line %d, col %d: missing attribute "pin".', iPosLine, iPosColumn)
    end

    -- Get the pin default.
    local ucPinDefault
    local strPinDefault = atAttributes['pinDefault']
    if strPinDefault==nil then
      fOk = nil
      aLxpAttr.tLog.error('Error in line %d, col %d: missing attribute "pinDefault".', iPosLine, iPosColumn)
    else
      ucPinDefault = tonumber(strPinDefault)
      if ucPinDefault==nil then
        fOk = nil
        aLxpAttr.tLog.error('Error in line %d, col %d: attribute "pinDefault" is no number: "%s".', iPosLine, iPosColumn, strPinDefault)
      elseif ucPinDefault~=0 and ucPinDefault~=1 then
        fOk = nil
        aLxpAttr.tLog.error('Error in line %d, col %d: attribute "pinDefault" is neither "0" nor "1": "%s".', iPosLine, iPosColumn, strPinDefault)
      end
    end

    -- Get the pin flags.
    local ulPinFlags
    local strPinFlags = atAttributes['pinFlags']
    if strPinFlags==nil then
      fOk = nil
      aLxpAttr.tLog.error('Error in line %d, col %d: missing attribute "pinFlags".', iPosLine, iPosColumn)
    else
      strPinFlags = string.upper(strPinFlags)
      if strPinFlags:match('^[IOZ]*$')==nil then
        fOk = nil
        aLxpAttr.tLog.error('Error in line %d, col %d: invalid attribute "pinFlags": "%s".', iPosLine, iPosColumn, strPinFlags)
      else
        ulPinFlags = 0
        if strPinFlags:find('I')~=nil then
          ulPinFlags = aLxpAttr.bit.bor(ulPinFlags, aLxpAttr.tIoMatrix.PINFLAG_I)
        end
        if strPinFlags:find('O')~=nil then
          ulPinFlags = aLxpAttr.bit.bor(ulPinFlags, aLxpAttr.tIoMatrix.PINFLAG_O)
        end
        if strPinFlags:find('Z')~=nil then
          ulPinFlags = aLxpAttr.bit.bor(ulPinFlags, aLxpAttr.tIoMatrix.PINFLAG_Z)
        end
      end
    end

    if fOk~=true then
      aLxpAttr.tResult = nil
    else
      local atPin = {
        strID,
        uiFamily,
        uiFamilyIndex,
        strPin,
        ucPinDefault,
        ulPinFlags
      }
      table.insert(aLxpAttr.atPins, atPin)
    end

  elseif strCurrentPath=="/IoMatrixTest/Matrix/Net" then
    -- Start a new net.
    local atNet = {}
    table.insert(aLxpAttr.atMatrix, atNet)
    aLxpAttr.tCurrentNet = atNet

  elseif strCurrentPath=="/IoMatrixTest/Matrix/Net/Pin" then
    -- Get the ID.
    local strID = atAttributes['id']
    if strID==nil then
      aLxpAttr.tResult = nil
      aLxpAttr.tLog.error('Error in line %d, col %d: missing attribute "id".', iPosLine, iPosColumn)
    else
      -- Add the pin to the current net.
      table.insert(aLxpAttr.tCurrentNet, strID)
    end
  end
end



--- Expat callback function for closing an element.
-- This function is part of the callbacks for the expat XML parser.
-- It is called when an element is closed.
-- @param tParser The parser object.
-- @param strName The name of the closed element.
function TestClassIoMatrix.parseCfg_EndElement(tParser, strName)
  local aLxpAttr = tParser:getcallbacks().userdata

  local tCurrentNetxDevice = aLxpAttr.tCurrentNetxDevice
  local strCurrentPath = aLxpAttr.strCurrentPath
  if strCurrentPath=='/IoMatrixTest/Devices/netX/Init' then
    if tCurrentNetxDevice.init~=nil then
      -- Compile the init function.
      local fn = loadstring(tCurrentNetxDevice.init)
      tCurrentNetxDevice.init = fn
    end
  elseif strCurrentPath=='/IoMatrixTest/Devices/netX/Deinit' then
    if tCurrentNetxDevice.deinit~=nil then
      -- Compile the deinit function.
      local fn = loadstring(tCurrentNetxDevice.deinit)
      tCurrentNetxDevice.deinit = fn
    end
  end

  -- Remove the last element from the current path.
  table.remove(aLxpAttr.atCurrentPath)
  aLxpAttr.strCurrentPath = table.concat(aLxpAttr.atCurrentPath, "/")
end



--- Expat callback function for character data.
-- This function is part of the callbacks for the expat XML parser.
-- It is called when character data is parsed.
-- @param tParser The parser object.
-- @param strData The character data.
function TestClassIoMatrix.parseCfg_CharacterData(tParser, strData)
  local aLxpAttr = tParser:getcallbacks().userdata

  local tCurrentNetxDevice = aLxpAttr.tCurrentNetxDevice
  local strCurrentPath = table.concat(aLxpAttr.atCurrentPath, "/")
  if strCurrentPath=='/IoMatrixTest/Devices/netX/Init' then
    if tCurrentNetxDevice.init==nil then
      tCurrentNetxDevice.init = strData
    else
      tCurrentNetxDevice.init = tCurrentNetxDevice.init .. '\n' .. strData
    end

  elseif strCurrentPath=='/IoMatrixTest/Devices/netX/Deinit' then
    if tCurrentNetxDevice.deinit==nil then
      tCurrentNetxDevice.deinit = strData
    else
      tCurrentNetxDevice.deinit = tCurrentNetxDevice.deinit .. '\n' .. strData
    end
  end
end



--- Parse a FTDI configuration file.
-- @param strFilename The path to the configuration file.
-- @param tLog A lua-log object which can be used for log messages.
-- @param atValidKeys A list of valid keys. It is used to validate the keys pairs in the configuration file and to translate them to numbers.
function TestClassIoMatrix:parse_test_definition(strFilename, tLog, tIoMatrix)
  -- Be optimistic!
  local tResult = true

  local aLxpAttr = {
    -- Start at root ("/").
    atCurrentPath = {""},
    strCurrentPath = nil,

    tCurrentNetxDevice = nil,
    tCurrentFTDIDevice = nil,
    tCurrentNet = nil,

    bit = self.bit,
    pl = self.pl,
    atFamilies = {
      NETX = tIoMatrix.FAMILY_NETX,
      FTDI = tIoMatrix.FAMILY_FTDI
    },
    tIoMatrix = tIoMatrix,

    atDevicesNetx = {},
    atDevicesFTDI = {},
    atPins = {},
    atMatrix = {},

    tResult = true,
    tLog = tLog
  }

  local aLxpCallbacks = {}
  aLxpCallbacks._nonstrict    = false
  aLxpCallbacks.StartElement  = self.parseCfg_StartElement
  aLxpCallbacks.EndElement    = self.parseCfg_EndElement
  aLxpCallbacks.CharacterData = self.parseCfg_CharacterData
  aLxpCallbacks.userdata      = aLxpAttr

  local tParser = self.lxp.new(aLxpCallbacks)

  -- Read the complete file.
  local strXmlText, strError = self.pl.utils.readfile(strFilename, false)
  if strXmlText==nil then
    tResult = nil
    tLog.error('Error reading the file: %s', strError)
  else
    local tParseResult, strMsg, uiLine, uiCol, uiPos = tParser:parse(strXmlText)
    if tParseResult~=nil then
      tParseResult, strMsg, uiLine, uiCol, uiPos = tParser:parse()
    end
    tParser:close()

    if tParseResult==nil then
      tResult = nil
      tLog.error("%s: %d,%d,%d", strMsg, uiLine, uiCol, uiPos)
    elseif aLxpAttr.tResult==nil then
      tResult = nil
    else
      self.atDevicesNetx = aLxpAttr.atDevicesNetx
      self.atDevicesFTDI = aLxpAttr.atDevicesFTDI
      self.atPins = aLxpAttr.atPins
      self.atMatrix = aLxpAttr.atMatrix
    end
  end

  return tResult
end



function TestClassIoMatrix:run(aParameters, tLog)
  ----------------------------------------------------------------------
  --
  -- Parse the parameters and collect all options.
  --
  local tIoMatrix = self.io_matrix(tLog)

  -- Read the test definition file.
  local strFileName = self.pl.path.exists(aParameters['definition'])
  if strFileName==nil then
    tLog.error('The test definition file "%s" does not exist.', aParameters['definition'])
    error('Failed to load the test definition.')
  end

  local tResult = self:parse_test_definition(strFileName, tLog, tIoMatrix)
  if tResult==nil then
    error('Failed to parse the test definition!')
  end

  for _, tNetxDevice in ipairs(self.atDevicesNetx) do
    local tPlugin
    if tNetxDevice.plugin=='COMMON' then
      tPlugin = tester.getCommonPlugin()
      if not tPlugin then
        error("No plugin selected, nothing to do!")
      end
    end

    if tPlugin==nil then
      error('Invalid plugin: "%s".', tostring(tNetxDevice.plugin))
    else
      tIoMatrix:add_netx(tPlugin, tNetxDevice.init, tNetxDevice.deinit)
    end
  end

  for _, tFtdiDevice in ipairs(self.atDevicesFTDI) do
    tIoMatrix:add_ftdi(tFtdiDevice)
  end

  tIoMatrix:parse_pin_description(self.atPins)

  local uiErrorCounter = tIoMatrix:matrix_test(self.atMatrix)
  if uiErrorCounter~=0 then
    tLog.error('%d errors occured.', uiErrorCounter)
    error('The test failed.')
  else
    tLog.info('')
    tLog.info(' #######  ##    ## ')
    tLog.info('##     ## ##   ##  ')
    tLog.info('##     ## ##  ##   ')
    tLog.info('##     ## #####    ')
    tLog.info('##     ## ##  ##   ')
    tLog.info('##     ## ##   ##  ')
    tLog.info(' #######  ##    ## ')
    tLog.info('')
  end
end

return TestClassIoMatrix
