let path = require('path');

let device = "am243x";

const files = {
    common: [
        "usb_descriptors.c",
        "main.c",
		"ncm_main.c"
    ],
};

/* Relative to where the makefile will be generated
 * Typically at <example_folder>/<BOARD>/<core_os_combo>/<compiler>
 */
const filedirs = {
    common: [
        "..",       /* core_os_combo base */
        "../../..", /* Example base */
    ],
};

const defines = {
    common: [
        "TINYUSB_INTEGRATION"
    ],
};

const includes_nortos_r5f = {
    common: [
        "${MCU_PLUS_SDK_PATH}/source/usb/tinyusb/config/nortos/am64x_am243x/ncm_config",
        "${MCU_PLUS_SDK_PATH}/source/usb/tinyusb/tinyusb-stack/src",
        "${MCU_PLUS_SDK_PATH}/source/usb/tinyusb/tinyusb-stack/lib/networking",
        "${MCU_PLUS_SDK_PATH}/source/networking/lwip/lwip-stack/src/include",
		"${MCU_PLUS_SDK_PATH}/source/networking/lwip/lwip-port/include/",
		"${MCU_PLUS_SDK_PATH}/source/networking/lwip/lwip-config/am243x/usb/",
		"${MCU_PLUS_SDK_PATH}/source/networking/lwip/lwip-stack/src/include/lwip/apps",
		"${MCU_PLUS_SDK_PATH}/source/kernel/dpl",
    ],
};

const libs_nortos_r5f = {
    common: [
        "nortos.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "usbd_cdn_nortos.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
        "usbd_tusb_ncm_nortos.am243x.r5f.ti-arm-clang.${ConfigName}.lib",
    ],
};

const libs_nortos_r5f_gcc = {
    common: [
        "nortos.am243x.r5f.gcc-armv7.${ConfigName}.lib",
        "board.am243x.r5f.gcc-armv7.${ConfigName}.lib",
        "drivers.am243x.r5f.gcc-armv7.${ConfigName}.lib",
        "usbd_cdn_nortos.am243x.r5f.gcc-armv7.${ConfigName}.lib",
        "usbd_tusb_ncm_nortos.am243x.r5f.gcc-armv7.${ConfigName}.lib",
    ],
};

const libdirs_nortos = {
    common: [
        "${MCU_PLUS_SDK_PATH}/source/kernel/nortos/lib",
        "${MCU_PLUS_SDK_PATH}/source/board/lib",
        "${MCU_PLUS_SDK_PATH}/source/drivers/lib",
        "${MCU_PLUS_SDK_PATH}/source/usb/cdn/lib",
        "${MCU_PLUS_SDK_PATH}/source/usb/tinyusb/lib",
    ],
};

const lnkfiles = {
    common: [
        "../linker.cmd",
    ]
};

const lnkfiles_gcc = {
    common: [
        "linker.cmd",
    ]
};

const syscfgfile = "../example.syscfg";

const templates_nortos_r5f =
[
];

const templates_nortos_r5f_gcc =
[
];

const buildOptionCombos = [
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am243x-evm", os: "nortos",isPartOfSystemProject: true},
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am243x-lp", os: "nortos",isPartOfSystemProject: true},
    { device: device, cpu: "r5fss0-0", cgt: "gcc-armv7", board: "am243x-evm", os: "nortos",isPartOfSystemProject: true},
    { device: device, cpu: "r5fss0-0", cgt: "gcc-armv7", board: "am243x-lp", os: "nortos",isPartOfSystemProject: true},
];

const systemProject = [
    {
        name: "ncm_nortos",
        tag: "nortos",
        skipProjectSpec: false,
        board: "am243x-evm",
        projects: [
			{ device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am243x-evm", os: "nortos"},
        ],
    },
    {
        name: "ncm_nortos",
        tag: "nortos",
        skipProjectSpec: false,
        board: "am243x-lp",
        projects: [
			{ device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am243x-lp", os: "nortos"},
        ],
    },
    {
        name: "ncm_nortos",
        tag: "nortos_gcc",
        skipProjectSpec: false,
        board: "am243x-evm",
        projects: [
			{ device: device, cpu: "r5fss0-0", cgt: "gcc-armv7", board: "am243x-evm", os: "nortos"},
        ],
    },
    {
        name: "ncm_nortos",
        tag: "nortos_gcc",
        skipProjectSpec: false,
        board: "am243x-lp",
        projects: [
			{ device: device, cpu: "r5fss0-0", cgt: "gcc-armv7", board: "am243x-lp", os: "nortos"},
        ],
    },
]

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "executable";
    property.name = "ncm_nortos";
    property.isInternal = false;
    property.description = "A USB Device NCM example"
    property.buildOptionCombos = buildOptionCombos;

    return property;
}

function getComponentBuildProperty(buildOption) {
    let build_property = {};

    build_property.files = files;
    build_property.filedirs = filedirs;
    build_property.libdirs = libdirs_nortos;
    build_property.syscfgfile = syscfgfile;
    if(buildOption.cpu.match(/r5f*/)) {
        build_property.defines = defines;
        build_property.includes = includes_nortos_r5f;
        if(buildOption.cgt.match(/gcc*/) )
        {
            build_property.lnkfiles = lnkfiles_gcc;
            build_property.libs = libs_nortos_r5f_gcc;
            build_property.templates = templates_nortos_r5f_gcc;
        }
        else
        {
            build_property.lnkfiles = lnkfiles;
            build_property.libs = libs_nortos_r5f;
            build_property.templates = templates_nortos_r5f;
        }
    }

    return build_property;
}

function getSystemProjects(device)
{
    return systemProject;
}

module.exports = {
    getComponentProperty,
    getComponentBuildProperty,
	getSystemProjects,
};
