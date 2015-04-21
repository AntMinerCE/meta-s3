
DESCRIPTION = "Kernel Module Bitmain lcd Driver"
HOMEPAGE = "http://www.example.com"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "CLOSED"
KERNEL_VERSION="3.8.13"

RRECOMMENDS_${PN} = "kernel (= ${KERNEL_VERSION})"
DEPENDS = "virtual/kernel"
PR = "r0"

SRC_URI = " \
file://lcd12864.c \
file://lcd12864.h \
file://lcdcharlib.h \
file://Makefile \
	"

S = "${WORKDIR}"

inherit module

do_compile () {
	echo "compile bitmain-lcd"
	unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS CC LD CPP
	oe_runmake 'MODPATH="${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/drivers"' \
		'KERNEL_SOURCE="${STAGING_KERNEL_DIR}"' \
		'KDIR="${STAGING_KERNEL_DIR}"' \
		'KERNEL_VERSION="${KERNEL_VERSION}"' \
		'CC="${KERNEL_CC}"' \
		'LD="${KERNEL_LD}"'

}


do_install () {
	install -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/drivers/bitmain
	install -m 0644 ${S}/bitmain_lcd*${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/drivers/bitmain
}
