FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-1.0:"

PRINC := "${@int(PRINC) + 1}"

do_install_append() {
	install -m 0755 ${WORKDIR}/mountdevtmpfs.sh ${D}${sysconfdir}/init.d
	update-rc.d -r ${D} mountdevtmpfs.sh start 02 S .

	install -m 0755 ${WORKDIR}/network.sh ${D}${sysconfdir}/init.d
	update-rc.d -r ${D} network.sh start 38 S .

	install -m 0755 ${WORKDIR}/bitmainer_setup.sh ${D}${sysconfdir}/init.d
	update-rc.d -r ${D} bitmainer_setup.sh start 37 S .

	install -m 0755 ${WORKDIR}/ntpdate.sh ${D}${sysconfdir}/init.d
	update-rc.d -r ${D} ntpdate.sh start 39 S .

	install -m 0755 ${WORKDIR}/cgminer.sh ${D}${sysconfdir}/init.d
	update-rc.d -r ${D} cgminer.sh start 70 S .

	install -m 0755 ${WORKDIR}/pgnand.sh ${D}${sysconfdir}/init.d
	
	cd ${D}${sysconfdir}/rcS.d
	ln -s ../init.d/ntpd S40ntpd
	
	if [ x"YES" == x"${Miner_Monitor}" ]; then
	install -m 0755 ${WORKDIR}/minermonitor ${D}${sysconfdir}/init.d
        update-rc.d -r ${D} minermonitor start 100 S .

        install -m 0400 ${WORKDIR}/minermonitor.conf  ${D}${sysconfdir}/minermonitor.conf.factory
        install -d ${D}${sysconfdir}/dataformatconfig
	install -m 0400 ${WORKDIR}/dataformatconfig/*  ${D}${sysconfdir}/dataformatconfig/
        #cp -rf ${WORKDIR}/dataformatconfig  ${D}${sysconfdir}/

	install -d ${D}${base_sbindir}
        install -m 0755 ${WORKDIR}/miner-m.sh ${D}${base_sbindir}/

	#install -d ${D}${base_sbindir}
        #install -m 0755 ${WORKDIR}/d-ddos-m.sh ${D}${base_sbindir}/
        

	fi

        install -d ${D}${base_sbindir}
        install -m 0755 ${WORKDIR}/auto_freq.sh ${D}${base_sbindir}/
	install -m 0400 ${WORKDIR}/auto_freq.conf ${D}${sysconfdir}/auto_freq.conf.factory

	install -m 0400 ${WORKDIR}/shadow.factory ${D}${sysconfdir}/shadow.factory
	if [ x"C1" == x"${Miner_TYPE}" ]; then
		install -m 0400 ${WORKDIR}/network_c1.conf.factory ${D}${sysconfdir}/network.conf.factory
		install -m 0400 ${WORKDIR}/cgminer_c1.conf.factory ${D}${sysconfdir}/cgminer.conf.factory
	elif [ x"C2" == x"${Miner_TYPE}" ]; then
		install -m 0400 ${WORKDIR}/network_c1.conf.factory ${D}${sysconfdir}/network.conf.factory
		install -m 0400 ${WORKDIR}/cgminer_c2.conf.factory ${D}${sysconfdir}/cgminer.conf.factory
	elif [ x"S5" == x"${Miner_TYPE}" ]; then
		install -m 0400 ${WORKDIR}/network.conf.factory ${D}${sysconfdir}/network.conf.factory
		install -m 0400 ${WORKDIR}/cgminer_s5.conf.factory ${D}${sysconfdir}/cgminer.conf.factory
	elif [ x"S2" == x"${Miner_TYPE}" ]; then
		echo "AntMiner S2" > ${WORKDIR}/user_defined_lcd.factory
		install -m 0400 ${WORKDIR}/network.conf.factory ${D}${sysconfdir}/network.conf.factory
		install -m 0400 ${WORKDIR}/cgminer_s2.conf.factory ${D}${sysconfdir}/cgminer.conf.factory
	elif [ x"S4+" == x"${Miner_TYPE}" ]; then
		echo "AntMiner S4+" > ${WORKDIR}/user_defined_lcd.factory
		install -m 0400 ${WORKDIR}/network.conf.factory ${D}${sysconfdir}/network.conf.factory
		install -m 0400 ${WORKDIR}/cgminer_s4p.conf.factory ${D}${sysconfdir}/cgminer.conf.factory
	else	
		echo "AntMiner S4" > ${WORKDIR}/user_defined_lcd.factory
		install -m 0400 ${WORKDIR}/network.conf.factory ${D}${sysconfdir}/network.conf.factory
		install -m 0400 ${WORKDIR}/cgminer.conf.factory ${D}${sysconfdir}/cgminer.conf.factory
	fi
#install -m 0400 ${WORKDIR}/cgminer.conf.factory ${D}${sysconfdir}/cgminer.conf.factory
#install -m 0400 ${WORKDIR}/s3-asic-freq.config ${D}${sysconfdir}/asic-freq.config
#install -m 0400 ${WORKDIR}/asic-freq.config ${D}${sysconfdir}/asic-freq.config
	install -m 0400 ${WORKDIR}/user_defined_lcd.factory ${D}${sysconfdir}/user_defined_lcd.factory
	install -m 0400 ${WORKDIR}/user_setting.factory ${D}${sysconfdir}/user_setting.factory

	install -d ${D}${base_sbindir}
	install -m 0755 ${WORKDIR}/monitorcg ${D}${base_sbindir}/
	install -m 0755 ${WORKDIR}/miner_lcd.sh ${D}${base_sbindir}/minerlcd
	install -d ${D}${bindir}
	rm -rf ${D}${bindir}/compile_time
	date >> ${D}${bindir}/compile_time
	if [ x"C1" == x"${Miner_TYPE}" ]; then
		echo "AntMiner C1" >> ${D}${bindir}/compile_time
	elif [ x"C2" == x"${Miner_TYPE}" ]; then
		echo "Antminer C2" >> ${D}${bindir}/compile_time
	elif [ x"S5" == x"${Miner_TYPE}" ]; then
		echo "Antminer S5" >> ${D}${bindir}/compile_time
	elif [ x"S2" == x"${Miner_TYPE}" ]; then
		echo "Antminer S2" >> ${D}${bindir}/compile_time
	else
		if [ x"S4+" == x"${Miner_TYPE}" ]; then
			echo "Antminer S4+" >> ${D}${bindir}/compile_time
		else
			echo "Antminer S4" >> ${D}${bindir}/compile_time
		fi
	fi		
}

SRC_URI_append = " file://mountdevtmpfs.sh"
SRC_URI_append = " file://network.sh"
SRC_URI_append = " file://network.conf.factory"
SRC_URI_append = " file://network_c1.conf.factory"
SRC_URI_append = " file://cgminer.sh"
SRC_URI_append = " file://cgminer.conf.factory"
SRC_URI_append = " file://cgminer_c1.conf.factory"
SRC_URI_append = " file://cgminer_c2.conf.factory"
SRC_URI_append = " file://cgminer_s4p.conf.factory"
SRC_URI_append = " file://cgminer_s5.conf.factory"
SRC_URI_append = " file://cgminer_s2.conf.factory"
#SRC_URI_append = " file://asic-freq.config"
#SRC_URI_append = " file://s3-asic-freq.config"
SRC_URI_append = " file://user_setting.factory"
SRC_URI_append = " file://user_defined_lcd.factory"
SRC_URI_append = " file://ntpdate.sh"
SRC_URI_append = " file://shadow.factory"
SRC_URI_append = " file://bitmainer_setup.sh"
SRC_URI_append = " file://monitorcg"
SRC_URI_append = " file://miner_lcd.sh"
SRC_URI_append = " file://pgnand.sh"
#SRC_URI_append = " file://d-ddos-m.sh"

SRC_URI_append = " file://auto_freq.sh"
SRC_URI_append = " file://auto_freq.conf"

SRC_URI_append = " file://miner-m.sh"
SRC_URI_append = " file://dataformatconfig"
SRC_URI_append = " file://minermonitor.conf"
SRC_URI_append = " file://minermonitor"

