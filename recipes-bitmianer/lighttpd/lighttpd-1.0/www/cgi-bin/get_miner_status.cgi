#!/bin/sh

echo {

# Read miner status
ant_elapsed=
ant_ghs5s=
ant_ghsav=
ant_foundblocks=
ant_getworks=
ant_accepted=
ant_rejected=
ant_hw=
ant_utility=
ant_discarded=
ant_stale=
ant_localwork=
ant_wu=
ant_diffa=
ant_diffr=
ant_bestshare=

echo \"summary\": {

ant_tmp=`cgminer-api -o`

if [ "${ant_tmp}" == "Socket connect failed: Connection refused" ]; then
	ant_elapsed=0
	ant_ghs5s=0
	ant_ghsav=0
	ant_foundblocks=0
	ant_getworks=0
	ant_accepted=0
	ant_rejected=0
	ant_hw=0
	ant_utility=0
	ant_discarded=0
	ant_stale=0
	ant_localwork=0
	ant_wu=0
	ant_diffa=0
	ant_diffr=0
	ant_bestshare=0
else
	ant_elapsed=${ant_tmp#*Elapsed=}
	ant_elapsed=${ant_elapsed%%,*}
	
	has_mhs="`echo "$ant_tmp" | grep 'MHS 5s'`"
	if [ "$has_mhs" ] ; then
		ant_mhs5s=${ant_tmp#*MHS 5s=}
		ant_mhs5s=${ant_mhs5s%%,*}
		ant_mhs5s=${ant_mhs5s%%.*}
		ant_ghs5s=$((${ant_mhs5s}/1000))

		if [ "$ant_elapsed" -lt "300" ] ; then
			ant_ghs5m=
		else
			ant_mhs5m=${ant_tmp#*MHS 5m=}
			ant_mhs5m=${ant_mhs5m%%,*}
			ant_mhs5m=${ant_mhs5m%%.*}
			ant_ghs5m=$((${ant_mhs5m}/1000))
		fi

		ant_mhsav=${ant_tmp#*MHS av=}
		ant_mhsav=${ant_mhsav%%,*}
		ant_mhsav=${ant_mhsav%%.*}
		ant_ghsav=$((${ant_mhsav}/1000))
	else
		ant_ghs5s=${ant_tmp#*GHS 5s=}
		ant_ghs5s=${ant_ghs5s%%,*}

		ant_ghsav=${ant_tmp#*GHS av=}
		ant_ghsav=${ant_ghsav%%,*}
	fi
	
	ant_foundblocks=${ant_tmp#*Found Blocks=}
	ant_foundblocks=${ant_foundblocks%%,*}
	
	ant_getworks=${ant_tmp#*Getworks=}
	ant_getworks=${ant_getworks%%,*}
	
	ant_accepted=${ant_tmp#*Accepted=}
	ant_accepted=${ant_accepted%%,*}
	
	ant_rejected=${ant_tmp#*Rejected=}
	ant_rejected=${ant_rejected%%,*}
	
	ant_hw=${ant_tmp#*Hardware Errors=}
	ant_hw=${ant_hw%%,*}
	
	ant_utility=${ant_tmp#*Utility=}
	ant_utility=${ant_utility%%,*}
	
	ant_discarded=${ant_tmp#*Discarded=}
	ant_discarded=${ant_discarded%%,*}
	
	ant_stale=${ant_tmp#*Stale=}
	ant_stale=${ant_stale%%,*}
	
	ant_localwork=${ant_tmp#*Local Work=}
	ant_localwork=${ant_localwork%%,*}
	
	ant_wu=${ant_tmp#*Work Utility=}
	ant_wu=${ant_wu%%,*}
	
	ant_diffa=${ant_tmp#*Difficulty Accepted=}
	ant_diffa=${ant_diffa%%,*}
	
	ant_diffr=${ant_tmp#*Difficulty Rejected=}
	ant_diffr=${ant_diffr%%,*}
	
	ant_diffs=${ant_tmp#*Difficulty Stale=}
	ant_diffs=${ant_diffs%%,*}
	
	ant_bestshare=${ant_tmp#*Best Share=}
	ant_bestshare=${ant_bestshare%%,*}
fi

echo \"elapsed\":\"${ant_elapsed}\",
echo \"ghs5s\":\"${ant_ghs5s}\",
echo \"ghsav\":\"${ant_ghsav}\",
echo \"foundblocks\":\"${ant_foundblocks}\",
echo \"getworks\":\"${ant_getworks}\",
echo \"accepted\":\"${ant_accepted}\",
echo \"rejected\":\"${ant_rejected}\",
echo \"hw\":\"${ant_hw}\",
echo \"utility\":\"${ant_utility}\",
echo \"discarded\":\"${ant_discarded}\",
echo \"stale\":\"${ant_stale}\",
echo \"localwork\":\"${ant_localwork}\",
echo \"wu\":\"${ant_wu}\",
echo \"diffa\":\"${ant_diffa}\",
echo \"diffr\":\"${ant_diffr}\",
echo \"diffs\":\"${ant_diffs}\",
echo \"bestshare\":\"${ant_bestshare}\"

echo },

ant_tmp=`cgminer-api -o pools`

echo \"pools\": [

if [ "${ant_tmp}" != "Socket connect failed: Connection refused" ]; then
	ant_last_len=0
	ant_len=0
	ant_first=1
	while :;
	do
		ant_tmp=${ant_tmp#*POOL=}
		ant_len=${#ant_tmp}
	
		if [ ${ant_len} -eq ${ant_last_len} ]; then
			break
		fi
		ant_last_len=${ant_len}
		
		if [ ${ant_first} -eq 1 ]; then
			ant_first=0
		else
			echo ,
		fi
		
		echo {
		ant_pool_index=
		ant_pool_url=
		ant_pool_user=
		ant_pool_status=
		ant_pool_priority=
		ant_pool_getworks=
		ant_pool_accepted=
		ant_pool_rejected=
		ant_pool_discarded=
		ant_pool_stale=
		ant_pool_diff=
		ant_pool_diff1=
		ant_pool_diffa=
		ant_pool_diffr=
		ant_pool_diffs=
		ant_pool_lsdiff=
		ant_pool_lstime=
		
		ant_pool_index=${ant_tmp%%,*}
		echo \"index\":\"${ant_pool_index}\",
		
		ant_pool_url=${ant_tmp#*URL=}
		ant_pool_url=${ant_pool_url%%,*}
		echo \"url\":\"${ant_pool_url}\",
		
		ant_pool_user=${ant_tmp#*User=}
		ant_pool_user=${ant_pool_user%%,*}
		echo \"user\":\"${ant_pool_user}\",
		
		ant_pool_status=${ant_tmp#*Status=}
		ant_pool_status=${ant_pool_status%%,*}
		echo \"status\":\"${ant_pool_status}\",
		
		ant_pool_priority=${ant_tmp#*Priority=}
		ant_pool_priority=${ant_pool_priority%%,*}
		echo \"priority\":\"${ant_pool_priority}\",
		
		ant_pool_getworks=${ant_tmp#*Getworks=}
		ant_pool_getworks=${ant_pool_getworks%%,*}
		echo \"getworks\":\"${ant_pool_getworks}\",
		
		ant_pool_accepted=${ant_tmp#*Accepted=}
		ant_pool_accepted=${ant_pool_accepted%%,*}
		echo \"accepted\":\"${ant_pool_accepted}\",
		
		ant_pool_rejected=${ant_tmp#*Rejected=}
		ant_pool_rejected=${ant_pool_rejected%%,*}
		echo \"rejected\":\"${ant_pool_rejected}\",
		
		ant_pool_discarded=${ant_tmp#*Discarded=}
		ant_pool_discarded=${ant_pool_discarded%%,*}
		echo \"discarded\":\"${ant_pool_discarded}\",
		
		ant_pool_stale=${ant_tmp#*Stale=}
		ant_pool_stale=${ant_pool_stale%%,*}
		echo \"stale\":\"${ant_pool_stale}\",
		
		has_diff="`echo "$ant_tmp" | grep ',Diff='`"
		if [ "$has_diff" ] ; then
			ant_pool_diff=${ant_tmp#*Diff=}
			ant_pool_diff=${ant_pool_diff%%,*}
		else
			ant_pool_diff=${ant_tmp#*Last Share Difficulty=}
			ant_pool_diff=${ant_pool_lsdiff%%,*}
		fi
		echo \"diff\":\"${ant_pool_diff}\",
		
		ant_pool_diff1=${ant_tmp#*Diff1 Shares=}
		ant_pool_diff1=${ant_pool_diff1%%,*}
		echo \"diff1\":\"${ant_pool_diff1}\",
		
		ant_pool_diffa=${ant_tmp#*Difficulty Accepted=}
		ant_pool_diffa=${ant_pool_diffa%%,*}
		echo \"diffa\":\"${ant_pool_diffa}\",
		
		ant_pool_diffr=${ant_tmp#*Difficulty Rejected=}
		ant_pool_diffr=${ant_pool_diffr%%,*}
		echo \"diffr\":\"${ant_pool_diffr}\",
		
		ant_pool_diffs=${ant_tmp#*Difficulty Stale=}
		ant_pool_diffs=${ant_pool_diffs%%,*}
		echo \"diffs\":\"${ant_pool_diffs}\",
		
		ant_pool_lsdiff=${ant_tmp#*Last Share Difficulty=}
		ant_pool_lsdiff=${ant_pool_lsdiff%%,*}
		echo \"lsdiff\":\"${ant_pool_lsdiff}\",
		
		ant_pool_lstime=${ant_tmp#*Last Share Time=}
		ant_pool_lstime=${ant_pool_lstime%%,*}
		echo \"lstime\":\"${ant_pool_lstime}\"
		echo }
	done
fi

echo ],

ant_tmp=`cgminer-api -o stats`

echo \"devs\": [

if [ "${ant_tmp}" != "Socket connect failed: Connection refused" ]; then
	i=1
	first=1
	ant_chain_acn=
	ant_freq=
	ant_fan=
	ant_temp=
	ant_chain_acs=
	
	ant_freq=${ant_tmp#*frequency=}
	ant_freq=${ant_freq%%,*}
		
	for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 ; do
		ant_chain_acn=
		ant_fan=
		ant_temp=
		ant_chain_acs=
		
		ant_chain_acn=${ant_tmp#*chain_acn$i=}
		ant_chain_acn=${ant_chain_acn%%,*}
		if [ -n ${ant_chain_acn} -a ${ant_chain_acn} != "0" ]; then
			if [ "${first}" == "1" ]; then
				first=0
			else
				echo ,
			fi
			echo {
			echo \"index\":\"${i}\",
			echo \"chain_acn\":\"${ant_chain_acn}\",
			echo \"freq\":\"${ant_freq}\",
		
			ant_fan=${ant_tmp#*fan$i=}
			ant_fan=${ant_fan%%,*}
			echo \"fan\":\"${ant_fan}\",
		
			ant_temp=${ant_tmp#*temp$i=}
			ant_temp=${ant_temp%%,*}
			echo \"temp\":\"${ant_temp}\",
		
			ant_chain_acs=${ant_tmp#*chain_acs$i=}
			ant_chain_acs=${ant_chain_acs%%,*}
			echo \"chain_acs\":\"${ant_chain_acs}\"
			echo }
		fi
	done
fi

echo ]

echo }