module("test01", package.seeall)

require("io_matrix")
require("parameters")

CFG_strTestName = "IOMATRIX_NXHX51ETM_R4"

CFG_aParameterDefinitions = {
	{
		name="verbose",
		default=0,
		help="Set the verbose mode. A value of 0 prints only errors and the analysis. Higher values print more details.",
		mandatory=true,
		validate=parameters.test_uint32,
		constrains=nil
	}
}



----------------------------------------------------------------------
--
-- Define all pins.
--
local atPinsUnderTest = {
	{ "HIF_D00",    io_matrix.PINTYPE_HIFPIO,  0, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D01",    io_matrix.PINTYPE_HIFPIO,  1, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D02",    io_matrix.PINTYPE_HIFPIO,  2, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D03",    io_matrix.PINTYPE_HIFPIO,  3, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D04",    io_matrix.PINTYPE_HIFPIO,  4, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D05",    io_matrix.PINTYPE_HIFPIO,  5, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D06",    io_matrix.PINTYPE_HIFPIO,  6, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D07",    io_matrix.PINTYPE_HIFPIO,  7, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D08",    io_matrix.PINTYPE_HIFPIO,  8, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D09",    io_matrix.PINTYPE_HIFPIO,  9, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D10",    io_matrix.PINTYPE_HIFPIO, 10, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D11",    io_matrix.PINTYPE_HIFPIO, 11, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D12",    io_matrix.PINTYPE_HIFPIO, 12, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D13",    io_matrix.PINTYPE_HIFPIO, 13, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D14",    io_matrix.PINTYPE_HIFPIO, 14, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D15",    io_matrix.PINTYPE_HIFPIO, 15, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D16",    io_matrix.PINTYPE_HIFPIO, 16, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D17",    io_matrix.PINTYPE_HIFPIO, 17, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D18",    io_matrix.PINTYPE_HIFPIO, 18, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D19",    io_matrix.PINTYPE_HIFPIO, 19, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D20",    io_matrix.PINTYPE_HIFPIO, 20, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D21",    io_matrix.PINTYPE_HIFPIO, 21, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D22",    io_matrix.PINTYPE_HIFPIO, 22, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D23",    io_matrix.PINTYPE_HIFPIO, 23, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D24",    io_matrix.PINTYPE_HIFPIO, 24, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D25",    io_matrix.PINTYPE_HIFPIO, 25, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D26",    io_matrix.PINTYPE_HIFPIO, 26, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D27",    io_matrix.PINTYPE_HIFPIO, 27, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D28",    io_matrix.PINTYPE_HIFPIO, 28, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D29",    io_matrix.PINTYPE_HIFPIO, 29, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D30",    io_matrix.PINTYPE_HIFPIO, 30, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_D31",    io_matrix.PINTYPE_HIFPIO, 31, 1, io_matrix.PINFLAG_IOZ },

	{ "HIF_A00",    io_matrix.PINTYPE_HIFPIO, 32, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A01",    io_matrix.PINTYPE_HIFPIO, 33, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A02",    io_matrix.PINTYPE_HIFPIO, 34, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A03",    io_matrix.PINTYPE_HIFPIO, 35, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A04",    io_matrix.PINTYPE_HIFPIO, 36, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A05",    io_matrix.PINTYPE_HIFPIO, 37, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A06",    io_matrix.PINTYPE_HIFPIO, 38, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A07",    io_matrix.PINTYPE_HIFPIO, 39, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A08",    io_matrix.PINTYPE_HIFPIO, 40, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A09",    io_matrix.PINTYPE_HIFPIO, 41, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A10",    io_matrix.PINTYPE_HIFPIO, 42, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A11",    io_matrix.PINTYPE_HIFPIO, 43, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A12",    io_matrix.PINTYPE_HIFPIO, 44, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A13",    io_matrix.PINTYPE_HIFPIO, 45, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A14",    io_matrix.PINTYPE_HIFPIO, 46, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_A15",    io_matrix.PINTYPE_HIFPIO, 47, 1, io_matrix.PINFLAG_IOZ },

	{ "HIF_AHI0",   io_matrix.PINTYPE_HIFPIO, 48, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_AHI1",   io_matrix.PINTYPE_HIFPIO, 49, 1, io_matrix.PINFLAG_IOZ },

	{ "HIF_BHE3",   io_matrix.PINTYPE_HIFPIO, 50, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_BHE1",   io_matrix.PINTYPE_HIFPIO, 51, 1, io_matrix.PINFLAG_IOZ },

	{ "HIF_RDn",    io_matrix.PINTYPE_HIFPIO, 52, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_WRn",    io_matrix.PINTYPE_HIFPIO, 53, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_CSn",    io_matrix.PINTYPE_HIFPIO, 54, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_RDY",    io_matrix.PINTYPE_HIFPIO, 55, 1, io_matrix.PINFLAG_I + io_matrix.PINFLAG_Z },
	{ "HIF_DIRQ",   io_matrix.PINTYPE_HIFPIO, 56, 1, io_matrix.PINFLAG_IOZ },
	{ "HIF_SDCLK",  io_matrix.PINTYPE_HIFPIO, 57, 1, io_matrix.PINFLAG_IOZ },

	{ "RSTOUT",     io_matrix.PINTYPE_RSTOUT,  0, 0, io_matrix.PINFLAG_O + io_matrix.PINFLAG_Z }
}



----------------------------------------------------------------------
--
-- Define all networks.
--
local apcNetListNames = {
	{ "HIF_A00",    "HIF_A08" },
	{ "HIF_A01",    "HIF_A09" },
	{ "HIF_A02",    "HIF_A10" },
	{ "HIF_A03",    "HIF_A11" },
	{ "HIF_A04",    "HIF_A12" },
	{ "HIF_A05",    "HIF_A13" },
	{ "HIF_A06",    "HIF_A14" },
	{ "HIF_A07",    "HIF_A15" },
	{ "HIF_D00",    "HIF_D08" },
	{ "HIF_D01",    "HIF_D09" },
	{ "HIF_D02",    "HIF_D10" },
	{ "HIF_D03",    "HIF_D11" },
	{ "HIF_D04",    "HIF_D12" },
	{ "HIF_D05",    "HIF_D13" },
	{ "HIF_D06",    "HIF_D14" },
	{ "HIF_D07",    "HIF_D15" },

	{ "HIF_D16",    "HIF_CSn" },
	{ "HIF_D17",    "HIF_D25" },
	{ "HIF_D18",    "HIF_BHE3" },
	{ "HIF_D19",    "HIF_D21" },
	{ "HIF_D20",    "HIF_D27",      "HIF_AHI0" },
	{ "HIF_D22",    "HIF_D29" },
	{ "HIF_D23",    "HIF_D28" },
	{ "HIF_D24",    "HIF_D30" },
	{ "HIF_D26",    "HIF_BHE1" },
	{ "HIF_SDCLK",  "HIF_WRn" },
	{ "HIF_AHI1",   "HIF_DIRQ" },

	{ "RSTOUT",     "HIF_RDY" }
}



function run(aParameters)
	----------------------------------------------------------------------
	--
	-- Parse the parameters and collect all options.
	--
	
	-- Parse the verbose option.
	local ulVerbose = aParameters["verbose"]

	
	----------------------------------------------------------------------
	--
	-- Open the connection to the netX.
	-- (or re-use an existing connection.)
	--
	local tPlugin = tester.getCommonPlugin()
	if tPlugin==nil then
		error("No plug-in selected, nothing to do!")
	end

	-- Download the test software.
	local aAttr = io_matrix.initialize(tPlugin, "netx/iomatrix_netx%d.bin")
	
	-- Parse and download the pin description.
	io_matrix.parse_pin_description(aAttr, atPinsUnderTest, ulVerbose)

	-- Parse the net list and run the matrix test.
	local ulResult = io_matrix.run_matrix_test(aAttr, apcNetListNames, ulVerbose)
	if ulResult==0 then
		print("")
		print(" #######  ##    ## ")
		print("##     ## ##   ##  ")
		print("##     ## ##  ##   ")
		print("##     ## #####    ")
		print("##     ## ##  ##   ")
		print("##     ## ##   ##  ")
		print(" #######  ##    ## ")
		print("")
	else
		error("The I/O matrix test failed!")
	end
end
