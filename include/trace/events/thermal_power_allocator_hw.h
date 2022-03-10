TRACE_EVENT(thermal_power_actor_cpu_limit,
	TP_PROTO(const struct cpumask *cpus, unsigned int freq,
		unsigned long cdev_state, u32 power),

	TP_ARGS(cpus, freq, cdev_state, power),

	TP_STRUCT__entry(
		__bitmask(cpumask, num_possible_cpus())
		__field(unsigned int,  freq      )
		__field(unsigned long, cdev_state)
		__field(u32,           power     )
	),

	TP_fast_assign(
		__assign_bitmask(cpumask, cpumask_bits(cpus),
				num_possible_cpus());
		__entry->freq = freq;
		__entry->cdev_state = cdev_state;
		__entry->power = power;
	),

	TP_printk("cpus=%s freq=%u cdev_state=%lu power=%u",
		__get_bitmask(cpumask), __entry->freq, __entry->cdev_state,
		__entry->power)
);

TRACE_EVENT(thermal_power_actor_gpu_get_power,
	TP_PROTO(unsigned long freq, unsigned long load,
		 unsigned long dynamic_power, unsigned long static_power, unsigned long req_power),

	TP_ARGS(freq, load, dynamic_power, static_power, req_power),

	TP_STRUCT__entry(
		__field(unsigned long, freq          )
		__field(unsigned long, load          )
		__field(unsigned long, dynamic_power )
		__field(unsigned long, static_power  )
		__field(unsigned long, req_power  )
	),

	TP_fast_assign(
		__entry->freq = freq;
		__entry->load = load;
		__entry->dynamic_power = dynamic_power;
		__entry->static_power = static_power;
		__entry->req_power = req_power;
	),

	TP_printk("gpu freq=%lu load=%lu dynamic_power=%lu static_power=%lu req_power=%lu",
		 __entry->freq, __entry->load,
		 __entry->dynamic_power, __entry->static_power, __entry->req_power)
);

TRACE_EVENT(IPA_allocator,
	TP_PROTO(struct thermal_zone_device *tz,
		 unsigned long current_temp, unsigned long control_temp, unsigned long switch_temp,
		 u32 num_actors, u32 power_range,
		 u32 *req_power, u32 total_req_power,
		 u32 *max_power, u32 max_allocatable_power,
		 u32 *granted_power, u32 total_granted_power
		),
	TP_ARGS(tz, current_temp, control_temp, switch_temp,
		num_actors, power_range,
		req_power, total_req_power,
		max_power, max_allocatable_power,
		granted_power, total_granted_power
		),
	TP_STRUCT__entry(
		__field(int,           tz_id                    )
		__field(unsigned long, current_temp             )
		__field(unsigned long, control_temp             )
		__field(unsigned long, switch_temp              )
		__field(u32,           num_actors               )
		__field(u32,           power_range              )
		__dynamic_array(u32,   req_power, num_actors    )
		__field(u32,           total_req_power          )
		__dynamic_array(u32,   granted_power, num_actors)
		__field(u32,           total_granted_power      )
		__dynamic_array(u32,   max_power, num_actors    )
		__field(u32,           max_allocatable_power    )

	),
	TP_fast_assign(
		__entry->tz_id = tz->id;
		__entry->current_temp = current_temp;
		__entry->control_temp = control_temp;
		__entry->switch_temp = switch_temp;
		__entry->num_actors = num_actors;
		__entry->power_range = power_range;
		memcpy(__get_dynamic_array(req_power), req_power, num_actors * sizeof(*req_power));
		__entry->total_req_power = total_req_power;
		memcpy(__get_dynamic_array(granted_power), granted_power, num_actors * sizeof(*granted_power));
		__entry->total_granted_power = total_granted_power;
		memcpy(__get_dynamic_array(max_power), max_power, num_actors * sizeof(*max_power));
		__entry->max_allocatable_power = max_allocatable_power;
	),

	TP_printk("%d,%d,%lu,%lu,%lu,%u,%s,%u,%s,%u,%s,%u",
		  __entry->tz_id,
		  __entry->num_actors,
		  __entry->current_temp, __entry->control_temp, __entry->switch_temp,
		  __entry->power_range,
		  __print_array(__get_dynamic_array(req_power), __entry->num_actors, sizeof(u32)),
		  __entry->total_req_power,
		  __print_array(__get_dynamic_array(max_power), __entry->num_actors, sizeof(u32)),
		  __entry->max_allocatable_power,
		  __print_array(__get_dynamic_array(granted_power), __entry->num_actors, sizeof(u32)),
		  __entry->total_granted_power
		)

);

TRACE_EVENT(IPA_allocator_pid,
	TP_PROTO(struct thermal_zone_device *tz,
		 s32 err, s32 err_integral, s64 p, s64 i, s64 d, s32 output),
	TP_ARGS(tz, err, err_integral, p, i, d, output),
	TP_STRUCT__entry(
		__field(int, tz_id       )
		__field(s32, err         )
		__field(s32, err_integral)
		__field(s64, p           )
		__field(s64, i           )
		__field(s64, d           )
		__field(s32, output      )
	),
	TP_fast_assign(
		__entry->tz_id = tz->id;
		__entry->err = err;
		__entry->err_integral = err_integral;
		__entry->p = p;
		__entry->i = i;
		__entry->d = d;
		__entry->output = output;
	),

	TP_printk("%d,%d,%d,%lld,%lld,%lld,%d",
		__entry->tz_id, __entry->err, __entry->err_integral,
		__entry->p, __entry->i, __entry->d, __entry->output)
);

TRACE_EVENT(IPA_actor_cpu_get_power,
	TP_PROTO(const struct cpumask *cpus, unsigned long freq, u32 *load,
		size_t load_len, u32 dynamic_power, u32 static_power, u32 req_power),

	TP_ARGS(cpus, freq, load, load_len, dynamic_power, static_power, req_power),

	TP_STRUCT__entry(
		__bitmask(cpumask, num_possible_cpus())
		__field(unsigned long, freq          )
		__dynamic_array(u32,   load, load_len)
		__field(size_t,        load_len      )
		__field(u32,           dynamic_power )
		__field(u32,           static_power  )
		__field(u32,           req_power     )
	),

	TP_fast_assign(
		__assign_bitmask(cpumask, cpumask_bits(cpus),
				num_possible_cpus());
		__entry->freq = freq;
		memcpy(__get_dynamic_array(load), load, load_len * sizeof(*load));
		__entry->load_len = load_len;
		__entry->dynamic_power = dynamic_power;
		__entry->static_power = static_power;
		__entry->req_power = req_power;
	),

	TP_printk("%s,%lu,%s,%u,%u,%u",
		__get_bitmask(cpumask),
		__entry->freq,
		__print_array(__get_dynamic_array(load), __entry->load_len, sizeof(u32)),
		__entry->dynamic_power,
		__entry->static_power,
		__entry->req_power)
);

TRACE_EVENT(IPA_actor_cpu_limit,
	TP_PROTO(const struct cpumask *cpus, unsigned int freq,
		unsigned long cdev_state, u32 power),

	TP_ARGS(cpus, freq, cdev_state, power),

	TP_STRUCT__entry(
		__bitmask(cpumask, num_possible_cpus())
		__field(unsigned int,  freq      )
		__field(unsigned long, cdev_state)
		__field(u32,           power     )
	),

	TP_fast_assign(
		__assign_bitmask(cpumask, cpumask_bits(cpus),
				num_possible_cpus());
		__entry->freq = freq;
		__entry->cdev_state = cdev_state;
		__entry->power = power;
	),

	TP_printk("%s,%u,%lu,%u",
		__get_bitmask(cpumask), __entry->freq, __entry->cdev_state,
		__entry->power)
);

TRACE_EVENT(IPA_actor_cpu_cooling,
	TP_PROTO(const struct cpumask *cpus, unsigned int freq,
		unsigned long cdev_state),

	TP_ARGS(cpus, freq, cdev_state),

	TP_STRUCT__entry(
		__bitmask(cpumask, num_possible_cpus())
		__field(unsigned int,  freq      )
		__field(unsigned long, cdev_state)
	),

	TP_fast_assign(
		__assign_bitmask(cpumask, cpumask_bits(cpus),
				num_possible_cpus());
		__entry->freq = freq;
		__entry->cdev_state = cdev_state;
	),

	TP_printk("%s,%u,%lu",
		__get_bitmask(cpumask), __entry->freq, __entry->cdev_state)
);

TRACE_EVENT(IPA_actor_gpu_limit,
	TP_PROTO(unsigned long freq, unsigned long cdev_state, u32 power, u32 est_power),

	TP_ARGS(freq, cdev_state, power, est_power),

	TP_STRUCT__entry(
		__field(unsigned long, freq)
		__field(unsigned long, cdev_state)
		__field(u32, power)
		__field(u32, est_power)
	),

	TP_fast_assign(
		__entry->freq = freq;
		__entry->cdev_state = cdev_state;
		__entry->power = power;
		__entry->est_power = est_power;
	),

	TP_printk("%lu,%lu,%u,%u",
		__entry->freq, __entry->cdev_state,
		__entry->power, __entry->est_power)
);

TRACE_EVENT(IPA_actor_gpu_cooling,
	TP_PROTO(unsigned long freq, unsigned long cdev_state),

	TP_ARGS(freq, cdev_state),

	TP_STRUCT__entry(
		__field(unsigned long,  freq      )
		__field(unsigned long, cdev_state)
	),

	TP_fast_assign(
		__entry->freq = freq;
		__entry->cdev_state = cdev_state;
	),

	TP_printk("%lu,%lu",
		__entry->freq, __entry->cdev_state)
);

TRACE_EVENT(IPA_actor_gpu_get_power,
	TP_PROTO(unsigned long freq, unsigned long load,
		 unsigned long dynamic_power, unsigned long static_power, unsigned long req_power),

	TP_ARGS(freq, load, dynamic_power, static_power, req_power),

	TP_STRUCT__entry(
		__field(unsigned long, freq          )
		__field(unsigned long, load          )
		__field(unsigned long, dynamic_power )
		__field(unsigned long, static_power  )
		__field(unsigned long, req_power  )
	),

	TP_fast_assign(
		__entry->freq = freq;
		__entry->load = load;
		__entry->dynamic_power = dynamic_power;
		__entry->static_power = static_power;
		__entry->req_power = req_power;
	),

	TP_printk("%lu,%lu,%lu,%lu,%lu",
		 __entry->freq, __entry->load,
		 __entry->dynamic_power, __entry->static_power, __entry->req_power)
);


TRACE_EVENT(IPA_get_tsens_value,
	TP_PROTO(u32 tsens_num, int *tsens_value, int tsens_value_max),

	TP_ARGS(tsens_num, tsens_value, tsens_value_max),

	TP_STRUCT__entry(
		__field(u32, tsens_num)
		__dynamic_array(int, tsens_value, tsens_num)
		__field(int, tsens_value_max)
	),

	TP_fast_assign(
		__entry->tsens_num = tsens_num;
		memcpy(__get_dynamic_array(tsens_value), tsens_value, tsens_num * sizeof(*tsens_value));
		__entry->tsens_value_max = tsens_value_max;
	),

	TP_printk("%s,%d",
		 __print_array(__get_dynamic_array(tsens_value), __entry->tsens_num,
		 sizeof(int)), __entry->tsens_value_max)
);

TRACE_EVENT(IPA_cpu_hot_plug,
	TP_PROTO(bool cpu_downed, long sensor_val, int up_threshold,
		int down_threshold),

	TP_ARGS(cpu_downed, sensor_val, up_threshold, down_threshold),

	TP_STRUCT__entry(
		__field(bool, cpu_downed)
		__field(long, sensor_val)
		__field(int, up_threshold)
		__field(int, down_threshold)
	),

	TP_fast_assign(
		__entry->cpu_downed = cpu_downed;
		__entry->sensor_val = sensor_val;
		__entry->up_threshold = up_threshold;
		__entry->down_threshold = down_threshold;
	),

	TP_printk("thermal hotplug: %u,%ld,%d,%d",
		 __entry->cpu_downed, __entry->sensor_val, __entry->up_threshold,
		 __entry->down_threshold)
);

TRACE_EVENT(IPA_gpu_hot_plug,
	TP_PROTO(bool gpu_downed, long sensor_val, int gpu_up_threshold,
		int gpu_down_threshold),

	TP_ARGS(gpu_downed, sensor_val, gpu_up_threshold, gpu_down_threshold),

	TP_STRUCT__entry(
		__field(bool, gpu_downed)
		__field(long, sensor_val)
		__field(int, gpu_up_threshold)
		__field(int, gpu_down_threshold)
	),

	TP_fast_assign(
		__entry->gpu_downed = gpu_downed;
		__entry->sensor_val = sensor_val;
		__entry->gpu_up_threshold = gpu_up_threshold;
		__entry->gpu_down_threshold = gpu_down_threshold;
	),

	TP_printk("thermal gpu hotplug: %u,%ld,%d,%d",
		 __entry->gpu_downed, __entry->sensor_val, __entry->gpu_up_threshold,
		 __entry->gpu_down_threshold)
);

TRACE_EVENT(allocate_power_actors,
	TP_PROTO(int num_actors, int total_weight),

	TP_ARGS(num_actors, total_weight),

	TP_STRUCT__entry(
		__field(int, num_actors)
		__field(int, total_weight)
	),

	TP_fast_assign(
		__entry->num_actors = num_actors;
		__entry->total_weight = total_weight;
	),

	TP_printk("num_actors=%d, total_weight=%d",
		__entry->num_actors, __entry->total_weight)
);

TRACE_EVENT(allocate_power_instances,
	TP_PROTO(struct thermal_cooling_device *cooling_device,
		int trip, int trip_max_desired_temperature),

	TP_ARGS(cooling_device, trip, trip_max_desired_temperature),

	TP_STRUCT__entry(
		__string(type, cooling_device->type)
		__field(int, trip)
		__field(int, trip_max_desired_temperature)
	),

	TP_fast_assign(
		__assign_str(type, cooling_device->type);
		__entry->trip = trip;
		__entry->trip_max_desired_temperature = trip_max_desired_temperature;
	),

	TP_printk("cooling device=%s trip=%d trip_max_desired_temperature=%d",
		__get_str(type), __entry->trip, __entry->trip_max_desired_temperature)
);

TRACE_EVENT(cdev_set_state,
	TP_PROTO(struct thermal_cooling_device *cooling_device, u32 granted_power, int state),

	TP_ARGS(cooling_device, granted_power, state),

	TP_STRUCT__entry(
		__string(type, cooling_device->type)
		__field(u32, granted_power)
		__field(int, state)
	),

	TP_fast_assign(
		__assign_str(type, cooling_device->type);
		__entry->granted_power = granted_power;
		__entry->state = state;
	),

	TP_printk("cooling device=%s granted_power=%u state=%d",
		__get_str(type), __entry->granted_power, __entry->state)
);

TRACE_EVENT(pid_reset,
	TP_PROTO(s64 err_integral, s32 prev_err, struct thermal_cooling_device *cdev,
		struct thermal_zone_device *tz, unsigned long state),

	TP_ARGS(err_integral, prev_err, cdev, tz, state),

	TP_STRUCT__entry(
		__field(s64, err_integral)
		__field(s32, prev_err)
		__string(cdev_type, cdev->type)
		__string(tz_type, tz->type)
		__field(unsigned long, state)
	),

	TP_fast_assign(
		__entry->err_integral = err_integral;
		__entry->prev_err = prev_err;
		__assign_str(cdev_type, cdev->type);
		__assign_str(tz_type, tz->type);
		__entry->state = state;
	),

	TP_printk("pid is reset, err_integral=%lld prev_err=%d cdev_type=%s tz_type=%s cdev's target=%lu",
		__entry->err_integral, __entry->prev_err, __get_str(cdev_type), __get_str(tz_type), __entry->state)
);

TRACE_EVENT(xpu_freq_table,
	TP_PROTO(u32 *f_table, u32 *p_table, int num),

	TP_ARGS(f_table, p_table, num),

	TP_STRUCT__entry(
		__field(int, num)
		__dynamic_array(u32, f_table, num)
		__dynamic_array(u32, p_table, num)
	),

	TP_fast_assign(
		__entry->num = num;
		memcpy(__get_dynamic_array(f_table), f_table,
			num * sizeof(*f_table));
		memcpy(__get_dynamic_array(p_table), p_table,
			num * sizeof(*p_table));
	),

	TP_printk("freq_table={%s} power_table={%s} num=%d",
		__print_array(__get_dynamic_array(f_table), __entry->num, 4),
		__print_array(__get_dynamic_array(p_table), __entry->num, 4),
		__entry->num)
);

TRACE_EVENT(devfreq_power2state,
	TP_PROTO(unsigned long freq, u32 static_power, s32 dyn_power, unsigned long status_busy_time,
		unsigned long busy_time, unsigned long total_time, s32 est_power),

	TP_ARGS(freq, static_power, dyn_power, status_busy_time, busy_time, total_time, est_power),

	TP_STRUCT__entry(
		__field(unsigned long, freq)
		__field(u32, static_power)
		__field(s32, dyn_power)
		__field(unsigned long, status_busy_time)
		__field(unsigned long, busy_time)
		__field(unsigned long, total_time)
		__field(s32, est_power)
	),

	TP_fast_assign(
		__entry->freq = freq;
		__entry->static_power = static_power;
		__entry->dyn_power = dyn_power;
		__entry->status_busy_time = status_busy_time;
		__entry->busy_time = busy_time;
		__entry->total_time = total_time;
		__entry->est_power = est_power;
	),

	TP_printk("cur_freq=%lu,static_power=%u,dyn_power=%d,status_busy_time=%lu,busy_time=%lu,total_time=%lu,est_power=%d",
		__entry->freq, __entry->static_power, __entry->dyn_power, __entry->status_busy_time,
		__entry->busy_time, __entry->total_time, __entry->est_power)
);

TRACE_EVENT(cluster_state_limit,
	TP_PROTO(int actor_id, unsigned long state, unsigned long ipa_limit),

	TP_ARGS(actor_id, state, ipa_limit),

	TP_STRUCT__entry(
		__field(int, actor_id)
		__field(unsigned long, state)
		__field(unsigned long, ipa_limit)
	),

	TP_fast_assign(
		__entry->actor_id = actor_id;
		__entry->state = state;
		__entry->ipa_limit = ipa_limit;
	),

	TP_printk("cluster=%d, state=%lu, ipa_state_limit=%lu", __entry->actor_id,
		__entry->state, __entry->ipa_limit)
);
