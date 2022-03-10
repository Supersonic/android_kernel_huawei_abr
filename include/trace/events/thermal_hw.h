TRACE_EVENT(IPA_boost,
	TP_PROTO(pid_t pid, struct thermal_zone_device *tz, int boost,
		 unsigned int boost_timeout),

	TP_ARGS(pid, tz, boost, boost_timeout),

	TP_STRUCT__entry(
		__field(pid_t, pid)
		__field(int, tz_id)
		__field(int, boost)
		__field(unsigned int, boost_timeout)
	),

	TP_fast_assign(
		__entry->pid = pid;
		__entry->tz_id = tz->id;
		__entry->boost = boost;
		__entry->boost_timeout = boost_timeout;
	),

	TP_printk("pid=%d tz_id=%d boost=%d timeout=%u",
		__entry->pid,
		__entry->tz_id,
		__entry->boost,
		__entry->boost_timeout)
);

TRACE_EVENT(cpu_update_policy,
	TP_PROTO(unsigned int cid, unsigned long clipped_freq),

	TP_ARGS(cid, clipped_freq),

	TP_STRUCT__entry(
		__field(unsigned int, cid)
		__field(unsigned long, clipped_freq)
	),

	TP_fast_assign(
		__entry->cid = cid;
		__entry->clipped_freq = clipped_freq;
	),

	TP_printk("cpuid=%d clipped_freq=%d",
		__entry->cid, __entry->clipped_freq)
);

TRACE_EVENT(tz_target,
	TP_PROTO(int id, unsigned long target),

	TP_ARGS(id, target),

	TP_STRUCT__entry(
		__field(int, id)
		__field(unsigned long, target)
	),

	TP_fast_assign(
		__entry->id = id;
		__entry->target = target;
	),

	TP_printk("thermal_zone%d->target=%lu",
		__entry->id, __entry->target)
);

TRACE_EVENT(set_cur_state,
	TP_PROTO(unsigned int cur_cluster, unsigned int g_soc_state, unsigned int g_board_state,
		unsigned int g_soc_freq_limit, unsigned int g_board_freq_limit, unsigned long limit_state,
		unsigned long state),

	TP_ARGS(cur_cluster, g_soc_state, g_board_state, g_soc_freq_limit,
		g_board_freq_limit, limit_state, state),

	TP_STRUCT__entry(
		__field(unsigned int, cur_cluster)
		__field(unsigned int, g_soc_state)
		__field(unsigned int, g_board_state)
		__field(unsigned int, g_soc_freq_limit)
		__field(unsigned int, g_board_freq_limit)
		__field(unsigned long, limit_state)
		__field(unsigned long, state)
	),

	TP_fast_assign(
		__entry->cur_cluster = cur_cluster;
		__entry->g_soc_state = g_soc_state;
		__entry->g_board_state = g_board_state;
		__entry->g_soc_freq_limit = g_soc_freq_limit;
		__entry->g_board_freq_limit = g_board_freq_limit;
		__entry->limit_state = limit_state;
		__entry->state = state;
	),

	TP_printk("cur_cluster=%u,g_soc_state=%u,g_board_state=%u,g_soc_freq_limit=%u,g_board_freq_limit=%u,limit_state=%lu, state=%lu",
		__entry->cur_cluster, __entry->g_soc_state, __entry->g_board_state,
		__entry->g_soc_freq_limit, __entry->g_board_freq_limit,
		__entry->limit_state, __entry->state)
);

TRACE_EVENT(set_freq_limit,
	TP_PROTO(unsigned int cur_cluster, unsigned int g_freq_limit),

	TP_ARGS(cur_cluster, g_freq_limit),

	TP_STRUCT__entry(
		__field(unsigned int, cur_cluster)
		__field(unsigned int, g_freq_limit)
	),

	TP_fast_assign(
		__entry->cur_cluster = cur_cluster;
		__entry->g_freq_limit = g_freq_limit;
	),

	TP_printk("cur_cluster=%u, g_ipa_freq_limit=%u",
		__entry->cur_cluster, __entry->g_freq_limit)
);

TRACE_EVENT(gpu_freq_limit,
	TP_PROTO(int actor, unsigned int target_freq,
		unsigned int g_freq_limit, unsigned int final_freq),

	TP_ARGS(actor, target_freq, g_freq_limit, final_freq),

	TP_STRUCT__entry(
		__field(int, actor)
		__field(unsigned int, target_freq)
		__field(unsigned int, g_freq_limit)
		__field(unsigned int, final_freq)
	),

	TP_fast_assign(
		__entry->actor = actor;
		__entry->target_freq=  target_freq;
		__entry->g_freq_limit = g_freq_limit;
		__entry->final_freq = final_freq;
	),

	TP_printk("gpu_id=%u, target_freq=%u, g_ipa_freq_limit=%u, final_freq=%u",
		__entry->actor, __entry->target_freq, __entry->g_freq_limit, __entry->final_freq)
);

TRACE_EVENT(freq_level,
	TP_PROTO(unsigned long rec_freq, unsigned long cur_freq, int level,
		uint32_t thermal_cycle, unsigned int max_pwrlevel, unsigned int min_pwrlevel),

	TP_ARGS(rec_freq, cur_freq, level, thermal_cycle,
		max_pwrlevel, min_pwrlevel),

	TP_STRUCT__entry(
		__field(unsigned long, rec_freq)
		__field(unsigned long, cur_freq)
		__field(int, level)
		__field(uint32_t, thermal_cycle)
		__field(unsigned int, max_pwrlevel)
		__field(unsigned int, min_pwrlevel)
	),

	TP_fast_assign(
		__entry->rec_freq = rec_freq;
		__entry->cur_freq = cur_freq;
		__entry->level = level;
		__entry->thermal_cycle = thermal_cycle;
		__entry->max_pwrlevel = max_pwrlevel;
		__entry->min_pwrlevel = min_pwrlevel;
	),

	TP_printk("rec_freq=%lu, cur_freq=%lu,cur_level=%d,thermal_cycle=%lu,max_pwrlevel=%d,min_pwrlevel=%d",
		__entry->rec_freq, __entry->cur_freq, __entry->level, __entry->thermal_cycle,
		__entry->max_pwrlevel, __entry->min_pwrlevel)
);

TRACE_EVENT(gpu_min_state,
	TP_PROTO(unsigned int ipa_state, unsigned long state,
		unsigned long g_gpu_state_min),

	TP_ARGS(ipa_state, state, g_gpu_state_min),

	TP_STRUCT__entry(
		__field(unsigned int, ipa_state)
		__field(unsigned long, state)
		__field(unsigned long, g_gpu_state_min)
	),

	TP_fast_assign(
		__entry->ipa_state = ipa_state;
		__entry->state = state;
		__entry->g_gpu_state_min = g_gpu_state_min;
	),

	TP_printk("ipa_state_board&soc=%u,min_state=%lu,g_gpu_state_min=%lu",
		__entry->ipa_state, __entry->state, __entry->g_gpu_state_min)
);

TRACE_EVENT(set_thermal_pwrlevel,
	TP_PROTO(unsigned long min_freq, unsigned long max_freq, int min_level, int max_level,
		unsigned int thermal_pwrlevel, unsigned int thermal_pwrlevel_floor),

	TP_ARGS(min_freq, max_freq, min_level, max_level,
		thermal_pwrlevel, thermal_pwrlevel_floor),

	TP_STRUCT__entry(
		__field(unsigned long, min_freq)
		__field(unsigned long, max_freq)
		__field(int, min_level)
		__field(int, max_level)
		__field(unsigned int, thermal_pwrlevel)
		__field(unsigned int, thermal_pwrlevel_floor)
	),

	TP_fast_assign(
		__entry->min_freq = min_freq;
		__entry->max_freq = max_freq;
		__entry->min_level = min_level;
		__entry->max_level = max_level;
		__entry->thermal_pwrlevel = thermal_pwrlevel;
		__entry->thermal_pwrlevel_floor = thermal_pwrlevel_floor;
	),

	TP_printk("min_freq=%u,max_freq=%u,min_level=%d,max_level=%d,thermal_pwrlevel=%u,thermal_pwrlevel_floor=%u",
		__entry->min_freq, __entry->max_freq, __entry->min_level, __entry->max_level,
		__entry->thermal_pwrlevel, __entry->thermal_pwrlevel_floor)
);

TRACE_EVENT(cpu_freq_voltage,
	TP_PROTO(unsigned int cpu_id, unsigned long freq, unsigned long voltage),

	TP_ARGS(cpu_id, freq, voltage),

	TP_STRUCT__entry(
		__field(unsigned int, cpu_id)
		__field(unsigned long, freq)
		__field(unsigned long, voltage)
	),

	TP_fast_assign(
		__entry->cpu_id = cpu_id;
		__entry->freq = freq;
		__entry->voltage = voltage;
	),

	TP_printk("cpu_id = %u, freq = %lu, voltage = %lu",
		__entry->cpu_id, __entry->freq, __entry->voltage)
);

TRACE_EVENT(cpu_static_power,
	TP_PROTO(u32 nr_cpus, int temperature, unsigned long t_scale, unsigned long v_scale, u32 cpu_coeff),

	TP_ARGS(nr_cpus, temperature, t_scale, v_scale, cpu_coeff),

	TP_STRUCT__entry(
		__field(u32, nr_cpus)
		__field(int, temperature)
		__field(unsigned long, t_scale)
		__field(unsigned long, v_scale)
		__field(u32, cpu_coeff)
	),

	TP_fast_assign(
		__entry->nr_cpus = nr_cpus;
		__entry->temperature = temperature;
		__entry->t_scale = t_scale;
		__entry->v_scale = v_scale;
		__entry->cpu_coeff = cpu_coeff;
	),

	TP_printk("nr_cpus = %u, temperature = %d, t_scale = %lu, v_scale = %lu, cpu_coeff = %u",
		__entry->nr_cpus, __entry->temperature, __entry->t_scale,
		__entry->v_scale, __entry->cpu_coeff)
	);
