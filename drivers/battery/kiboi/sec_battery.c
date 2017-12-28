/*
 *  sec_battery.c
 *  Samsung Mobile Battery Driver
 *
 *  Copyright (C) 2012 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/battery/sec_battery.h>
#include <linux/sec_debug.h>

const char *charger_chip_name;
bool sec_calibrate_battery = false;
static unsigned int STORE_MODE_CHARGING_MAX = 90;
static unsigned int STORE_MODE_CHARGING_MIN = 20;

module_param_named(store_mode_max, STORE_MODE_CHARGING_MAX, uint, S_IWUSR | S_IRUGO);
module_param_named(store_mode_min, STORE_MODE_CHARGING_MIN, uint, S_IWUSR | S_IRUGO);

const char *charger_chip_name;
bool sleep_mode = false;
#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
bool wpc_temp_mode = false;
#endif
bool slate_mode_state;

/**************************************************************/
#ifdef CONFIG_BATTERY_SWELLING_SELF_DISCHARGING
#ifdef CONFIG_BATTERY_SWELLING_SELF_DISCHARGING_ZERO_ONLY
static int zero_sdchg_ic_exist = 1;	// default : ic exist
static int __init zero_sdchg_ic_exist_setup(char *str)
{
	zero_sdchg_ic_exist = simple_strtol(str, NULL, 0);
	return 1;
}

__setup("zero_sdchg_ic=", zero_sdchg_ic_exist_setup);
#endif
#endif
/**************************************************************/
static struct device_attribute sec_battery_attrs[] = {
	SEC_BATTERY_ATTR(batt_reset_soc),
	SEC_BATTERY_ATTR(batt_read_raw_soc),
	SEC_BATTERY_ATTR(batt_read_adj_soc),
	SEC_BATTERY_ATTR(batt_type),
	SEC_BATTERY_ATTR(batt_vfocv),
	SEC_BATTERY_ATTR(batt_vol_adc),
	SEC_BATTERY_ATTR(batt_vol_adc_cal),
	SEC_BATTERY_ATTR(batt_vol_aver),
	SEC_BATTERY_ATTR(batt_vol_adc_aver),
	SEC_BATTERY_ATTR(batt_current_ua_now),
	SEC_BATTERY_ATTR(batt_current_ua_avg),
	SEC_BATTERY_ATTR(batt_filter_cfg),
	SEC_BATTERY_ATTR(batt_temp),
	SEC_BATTERY_ATTR(batt_temp_adc),
	SEC_BATTERY_ATTR(batt_temp_aver),
	SEC_BATTERY_ATTR(batt_temp_adc_aver),
	SEC_BATTERY_ATTR(chg_temp),
	SEC_BATTERY_ATTR(chg_temp_adc),
	SEC_BATTERY_ATTR(batt_vf_adc),
	SEC_BATTERY_ATTR(batt_slate_mode),

	SEC_BATTERY_ATTR(batt_lp_charging),
	SEC_BATTERY_ATTR(siop_activated),
	SEC_BATTERY_ATTR(siop_level),
	SEC_BATTERY_ATTR(siop_event),
	SEC_BATTERY_ATTR(batt_charging_source),
	SEC_BATTERY_ATTR(fg_reg_dump),
	SEC_BATTERY_ATTR(fg_reset_cap),
	SEC_BATTERY_ATTR(fg_capacity),
	SEC_BATTERY_ATTR(fg_asoc),
	SEC_BATTERY_ATTR(auth),
	SEC_BATTERY_ATTR(chg_current_adc),
	SEC_BATTERY_ATTR(wc_adc),
	SEC_BATTERY_ATTR(wc_status),
	SEC_BATTERY_ATTR(wc_enable),
	SEC_BATTERY_ATTR(hv_charger_status),
	SEC_BATTERY_ATTR(hv_charger_set),
	SEC_BATTERY_ATTR(hv_charger_support),
	SEC_BATTERY_ATTR(hv_wc_charger_support),
	SEC_BATTERY_ATTR(factory_mode),
	SEC_BATTERY_ATTR(store_mode),
	SEC_BATTERY_ATTR(update),
	SEC_BATTERY_ATTR(test_mode),

	SEC_BATTERY_ATTR(call),
	SEC_BATTERY_ATTR(2g_call),
	SEC_BATTERY_ATTR(talk_gsm),
	SEC_BATTERY_ATTR(3g_call),
	SEC_BATTERY_ATTR(talk_wcdma),
	SEC_BATTERY_ATTR(music),
	SEC_BATTERY_ATTR(video),
	SEC_BATTERY_ATTR(browser),
	SEC_BATTERY_ATTR(hotspot),
	SEC_BATTERY_ATTR(camera),
	SEC_BATTERY_ATTR(camcorder),
	SEC_BATTERY_ATTR(data_call),
	SEC_BATTERY_ATTR(wifi),
	SEC_BATTERY_ATTR(wibro),
	SEC_BATTERY_ATTR(lte),
	SEC_BATTERY_ATTR(lcd),
	SEC_BATTERY_ATTR(gps),
	SEC_BATTERY_ATTR(event),
	SEC_BATTERY_ATTR(batt_temp_table),
	SEC_BATTERY_ATTR(batt_high_current_usb),
#if defined(CONFIG_SAMSUNG_BATTERY_ENG_TEST)
	SEC_BATTERY_ATTR(test_charge_current),
#endif
	SEC_BATTERY_ATTR(set_stability_test),
	SEC_BATTERY_ATTR(batt_capacity_max),
	SEC_BATTERY_ATTR(batt_inbat_voltage),
#if defined(CONFIG_BATTERY_SWELLING_SELF_DISCHARGING)
	SEC_BATTERY_ATTR(batt_discharging_check),
	SEC_BATTERY_ATTR(batt_discharging_check_adc),
	SEC_BATTERY_ATTR(batt_discharging_ntc),
	SEC_BATTERY_ATTR(batt_discharging_ntc_adc),
	SEC_BATTERY_ATTR(batt_self_discharging_control),
#endif
#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY) || defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
	SEC_BATTERY_ATTR(batt_inbat_wireless_cs100),
#endif
	SEC_BATTERY_ATTR(hmt_ta_connected),
	SEC_BATTERY_ATTR(hmt_ta_charge),
	SEC_BATTERY_ATTR(fg_cycle),
	SEC_BATTERY_ATTR(fg_full_voltage),
	SEC_BATTERY_ATTR(fg_fullcapnom),
	SEC_BATTERY_ATTR(battery_cycle),
#if defined(CONFIG_WIRELESS_CHARGER_THM)
	SEC_BATTERY_ATTR(batt_wpc_temp),
	SEC_BATTERY_ATTR(batt_wpc_temp_adc),
#endif
#if defined(CONFIG_WIRELESS_FIRMWARE_UPDATE)
	SEC_BATTERY_ATTR(batt_wireless_firmware_update),
	SEC_BATTERY_ATTR(otp_firmware_result),
	SEC_BATTERY_ATTR(wc_ic_grade),
	SEC_BATTERY_ATTR(otp_firmware_ver_bin),
	SEC_BATTERY_ATTR(otp_firmware_ver),
	SEC_BATTERY_ATTR(tx_firmware_result),
	SEC_BATTERY_ATTR(tx_firmware_ver),
	SEC_BATTERY_ATTR(batt_tx_status),
#endif
#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
	SEC_BATTERY_ATTR(wc_vout),
	SEC_BATTERY_ATTR(wc_vrect),
	SEC_BATTERY_ATTR(batt_hv_wireless_status),
	SEC_BATTERY_ATTR(batt_hv_wireless_pad_ctrl),
#endif
	SEC_BATTERY_ATTR(batt_tune_float_voltage),
	SEC_BATTERY_ATTR(batt_tune_intput_charge_current),
	SEC_BATTERY_ATTR(batt_tune_fast_charge_current),
	SEC_BATTERY_ATTR(batt_tune_ui_term_cur_1st),
	SEC_BATTERY_ATTR(batt_tune_ui_term_cur_2nd),
	SEC_BATTERY_ATTR(batt_tune_temp_high_event),
	SEC_BATTERY_ATTR(batt_tune_temp_high_rec_event),
	SEC_BATTERY_ATTR(batt_tune_temp_low_event),
	SEC_BATTERY_ATTR(batt_tune_temp_low_rec_event),
	SEC_BATTERY_ATTR(batt_tune_temp_high_normal),
	SEC_BATTERY_ATTR(batt_tune_temp_high_rec_normal),
	SEC_BATTERY_ATTR(batt_tune_temp_low_normal),
	SEC_BATTERY_ATTR(batt_tune_temp_low_rec_normal),
	SEC_BATTERY_ATTR(batt_tune_chg_temp_high),
	SEC_BATTERY_ATTR(batt_tune_chg_temp_rec),
	SEC_BATTERY_ATTR(batt_tune_chg_limit_cur),
	SEC_BATTERY_ATTR(batt_tune_coil_temp_high),
	SEC_BATTERY_ATTR(batt_tune_coil_temp_rec),
	SEC_BATTERY_ATTR(batt_tune_coil_limit_cur),
	SEC_BATTERY_ATTR(camera_temp_adc),
	SEC_BATTERY_ATTR(camera_temp),
	SEC_BATTERY_ATTR(camera_limit),
	SEC_BATTERY_ATTR(batt_misc_event),
	SEC_BATTERY_ATTR(cisd_fullcaprep_max),
#if defined(CONFIG_BATTERY_CISD)
	SEC_BATTERY_ATTR(cisd_data),
#endif	
};

static enum power_supply_property sec_battery_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_CHARGE_TYPE,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE, /* this cable type should be called by fw not internal */
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_VOLTAGE_AVG,
	POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, /* this value is for internal cable type */
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_CURRENT_AVG,
	POWER_SUPPLY_PROP_CHARGE_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_TEMP,
	POWER_SUPPLY_PROP_TEMP_AMBIENT,
	POWER_SUPPLY_PROP_POWER_NOW,
	POWER_SUPPLY_PROP_POWER_AVG,
#if defined(CONFIG_CALC_TIME_TO_FULL)
	POWER_SUPPLY_PROP_TIME_TO_FULL_NOW,
#endif
#if defined(CONFIG_BATTERY_SWELLING)
	POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT,
#endif
	POWER_SUPPLY_PROP_CHARGE_COUNTER_SHADOW,
	POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL,
	POWER_SUPPLY_PROP_INPUT_CURRENT_MAX,
};

static enum power_supply_property sec_power_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static enum power_supply_property sec_ac_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_TEMP,
};

static enum power_supply_property sec_ps_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_ONLINE,
};

static char *supply_list[] = {
	"battery",
};

char *sec_bat_charging_mode_str[] = {
	"None",
	"Normal",
	"Additional",
	"Re-Charging",
	"ABS"
};

char *sec_bat_status_str[] = {
	"Unknown",
	"Charging",
	"Discharging",
	"Not-charging",
	"Full"
};

char *sec_bat_health_str[] = {
	"Unknown",
	"Good",
	"Overheat",
	"Warm",
	"Dead",
	"OverVoltage",
	"UnspecFailure",
	"Cold",
	"Cool",
	"WatchdogTimerExpire",
	"SafetyTimerExpire",
	"UnderVoltage",
	"OverheatLimit"
};

static int sec_bat_set_charge(
				struct sec_battery_info *battery,
				bool enable)
{
	union power_supply_propval val;
	ktime_t current_time;
	struct timespec ts;

	if (battery->cable_type == POWER_SUPPLY_TYPE_OTG)
		return 0;
	val.intval = battery->status;
	psy_do_property(battery->pdata->charger_name, set,
		POWER_SUPPLY_PROP_STATUS, val);
#if defined(ANDROID_ALARM_ACTIVATED)
	current_time = alarm_get_elapsed_realtime();
	ts = ktime_to_timespec(current_time);
#else
	current_time = ktime_get_boottime();
	ts = ktime_to_timespec(current_time);
#endif

	if (enable) {
		val.intval = battery->cable_type;
		/*Reset charging start time only in initial charging start */
		if (battery->charging_start_time == 0) {
			if (ts.tv_sec < 1)
				ts.tv_sec = 1;
			battery->charging_start_time = ts.tv_sec;
			battery->charging_next_time =
				battery->pdata->charging_reset_time;
		}
		if (battery->siop_level < 100)
			battery->lcd_on_time = ts.tv_sec;
	} else {
		val.intval = POWER_SUPPLY_TYPE_BATTERY;
		battery->charging_start_time = 0;
		battery->charging_passed_time = 0;
		battery->charging_next_time = 0;
		battery->charging_fullcharged_time = 0;
		battery->full_check_cnt = 0;
#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY) || defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
		battery->cc_cv_mode = 0;
		battery->full_mode = false; // need to check
		battery->cs100_status = false;
#endif
#if defined(CONFIG_BATTERY_CISD)
		battery->cisd.ab_vbat_check_count = 0;
#endif
		battery->lcd_on_total_time = 0;
		battery->lcd_on_time = 0;
	}

	battery->charging_block = !enable;

	battery->temp_highlimit_cnt = 0;
	battery->temp_high_cnt = 0;
	battery->temp_low_cnt = 0;
	battery->temp_recover_cnt = 0;

	psy_do_property(battery->pdata->charger_name, set,
		POWER_SUPPLY_PROP_ONLINE, val);

	psy_do_property(battery->pdata->fuelgauge_name, set,
		POWER_SUPPLY_PROP_ONLINE, val);

#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY)
	if (battery->wc_status) {
		psy_do_property(battery->pdata->charger_name, get,
				POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, val);
		psy_do_property(battery->pdata->wireless_charger_name, set,
				POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, val);
	}

	if (battery->wc_status) {
		val.intval = POWER_SUPPLY_TYPE_WIRELESS;
	} else {
		val.intval = POWER_SUPPLY_TYPE_BATTERY;
	}
	psy_do_property(battery->pdata->wireless_charger_name, set,
		POWER_SUPPLY_PROP_ONLINE, val);

#endif

	return 0;
}

static void sec_bat_set_misc_event(struct sec_battery_info *battery,
	const int misc_event_type, bool do_clear) {

	mutex_lock(&battery->misclock);
	pr_info("%s: %s misc event(now=0x%x, value=0x%x)\n",
		__func__, ((do_clear) ? "clear" : "set"), battery->misc_event, misc_event_type);
	if (do_clear) {
		battery->misc_event &= ~misc_event_type;
	} else {
		battery->misc_event |= misc_event_type;
	}
	mutex_unlock(&battery->misclock);

	if (battery->prev_misc_event != battery->misc_event) {
		cancel_delayed_work(&battery->misc_event_work);
		wake_lock(&battery->misc_event_wake_lock);
		queue_delayed_work_on(0, battery->monitor_wqueue,
			&battery->misc_event_work, 0);
	}
}

static int sec_bat_get_adc_data(struct sec_battery_info *battery,
			int adc_ch, int count)
{
	int adc_data;
	int adc_max;
	int adc_min;
	int adc_total;
	int i;

	adc_data = 0;
	adc_max = 0;
	adc_min = 0;
	adc_total = 0;

	for (i = 0; i < count; i++) {
		mutex_lock(&battery->adclock);
#ifdef CONFIG_OF
		adc_data = adc_read(battery, adc_ch);
#else
		adc_data = adc_read(battery->pdata, adc_ch);
#endif
		mutex_unlock(&battery->adclock);

		if (adc_data < 0)
			goto err;

		if (i != 0) {
			if (adc_data > adc_max)
				adc_max = adc_data;
			else if (adc_data < adc_min)
				adc_min = adc_data;
		} else {
			adc_max = adc_data;
			adc_min = adc_data;
		}
		adc_total += adc_data;
	}

	return (adc_total - adc_max - adc_min) / (count - 2);
err:
	return adc_data;
}

/*
static unsigned long calculate_average_adc(
			struct sec_battery_info *battery,
			int channel, int adc)
{
	unsigned int cnt = 0;
	int total_adc = 0;
	int average_adc = 0;
	int index = 0;

	cnt = battery->adc_sample[channel].cnt;
	total_adc = battery->adc_sample[channel].total_adc;

	if (adc < 0) {
		dev_err(battery->dev,
			"%s : Invalid ADC : %d\n", __func__, adc);
		adc = battery->adc_sample[channel].average_adc;
	}

	if (cnt < ADC_SAMPLE_COUNT) {
		battery->adc_sample[channel].adc_arr[cnt] = adc;
		battery->adc_sample[channel].index = cnt;
		battery->adc_sample[channel].cnt = ++cnt;

		total_adc += adc;
		average_adc = total_adc / cnt;
	} else {
		index = battery->adc_sample[channel].index;
		if (++index >= ADC_SAMPLE_COUNT)
			index = 0;

		total_adc = total_adc -
			battery->adc_sample[channel].adc_arr[index] + adc;
		average_adc = total_adc / ADC_SAMPLE_COUNT;

		battery->adc_sample[channel].adc_arr[index] = adc;
		battery->adc_sample[channel].index = index;
	}

	battery->adc_sample[channel].total_adc = total_adc;
	battery->adc_sample[channel].average_adc = average_adc;

	return average_adc;
}
*/
static int sec_bat_get_adc_value(
		struct sec_battery_info *battery, int channel)
{
	int adc;

	adc = sec_bat_get_adc_data(battery, channel,
		battery->pdata->adc_check_count);

	if (adc < 0) {
		dev_err(battery->dev,
			"%s: Error in ADC\n", __func__);
		return adc;
	}

	return adc;
}
#ifdef CONFIG_BATTERY_SWELLING_SELF_DISCHARGING
int sec_bat_get_adc_value_using_sdchg(
	struct sec_battery_info *battery, int channel)
{
	return sec_bat_get_adc_value(battery, channel);
}
EXPORT_SYMBOL(sec_bat_get_adc_value_using_sdchg);
#endif

static int sec_bat_get_charger_type_adc
				(struct sec_battery_info *battery)
{
	/* It is true something valid is
	connected to the device for charging.
	By default this something is considered to be USB.*/
	int result = POWER_SUPPLY_TYPE_USB;

	int adc = 0;
	int i;

	/* Do NOT check cable type when cable_switch_check() returns false
	 * and keep current cable type
	 */
	if (battery->pdata->cable_switch_check &&
	    !battery->pdata->cable_switch_check())
		return battery->cable_type;

	adc = sec_bat_get_adc_value(battery,
		SEC_BAT_ADC_CHANNEL_CABLE_CHECK);

	/* Do NOT check cable type when cable_switch_normal() returns false
	 * and keep current cable type
	 */
	if (battery->pdata->cable_switch_normal &&
	    !battery->pdata->cable_switch_normal())
		return battery->cable_type;

	for (i = 0; i < SEC_SIZEOF_POWER_SUPPLY_TYPE; i++)
		if ((adc > battery->pdata->cable_adc_value[i].min) &&
			(adc < battery->pdata->cable_adc_value[i].max))
			break;
	if (i >= SEC_SIZEOF_POWER_SUPPLY_TYPE)
		dev_err(battery->dev,
			"%s : default USB\n", __func__);
	else
		result = i;

	dev_dbg(battery->dev, "%s : result(%d), adc(%d)\n",
		__func__, result, adc);

	return result;
}

static bool sec_bat_check_vf_adc(struct sec_battery_info *battery)
{
	int adc;

	adc = sec_bat_get_adc_data(battery,
		SEC_BAT_ADC_CHANNEL_BAT_CHECK,
		battery->pdata->adc_check_count);

	if (adc < 0) {
		dev_err(battery->dev, "%s: VF ADC error\n", __func__);
		adc = battery->check_adc_value;
	} else
		battery->check_adc_value = adc;

	if ((battery->check_adc_value <= battery->pdata->check_adc_max) &&
		(battery->check_adc_value >= battery->pdata->check_adc_min)) {
		return true;
	} else {
		dev_info(battery->dev, "%s: adc (%d)\n", __func__, battery->check_adc_value);
		return false;
	}
}

static bool sec_bat_check_by_psy(struct sec_battery_info *battery)
{
	char *psy_name;
	union power_supply_propval value;
	bool ret;
	ret = true;

	switch (battery->pdata->battery_check_type) {
	case SEC_BATTERY_CHECK_PMIC:
		psy_name = battery->pdata->pmic_name;
		break;
	case SEC_BATTERY_CHECK_FUELGAUGE:
		psy_name = battery->pdata->fuelgauge_name;
		break;
	case SEC_BATTERY_CHECK_CHARGER:
		psy_name = battery->pdata->charger_name;
		break;
	default:
		dev_err(battery->dev,
			"%s: Invalid Battery Check Type\n", __func__);
		ret = false;
		goto battery_check_error;
		break;
	}

	psy_do_property(psy_name, get,
		POWER_SUPPLY_PROP_PRESENT, value);
	ret = (bool)value.intval;

battery_check_error:
	return ret;
}

static bool sec_bat_check(struct sec_battery_info *battery)
{
	bool ret;
	ret = true;

	if (battery->factory_mode || battery->is_jig_on) {
		dev_dbg(battery->dev, "%s: No need to check in factory mode\n",
			__func__);
		return ret;
	}

	if (battery->health != POWER_SUPPLY_HEALTH_GOOD &&
		battery->health != POWER_SUPPLY_HEALTH_UNSPEC_FAILURE) {
		dev_dbg(battery->dev, "%s: No need to check\n", __func__);
		return ret;
	}

	switch (battery->pdata->battery_check_type) {
	case SEC_BATTERY_CHECK_ADC:
		if(battery->cable_type == POWER_SUPPLY_TYPE_BATTERY)
			ret = battery->present;
		else
			ret = sec_bat_check_vf_adc(battery);
		break;
	case SEC_BATTERY_CHECK_INT:
	case SEC_BATTERY_CHECK_CALLBACK:
		if(battery->cable_type == POWER_SUPPLY_TYPE_BATTERY) {
			ret = battery->present;
		} else {
			if (battery->pdata->check_battery_callback)
				ret = battery->pdata->check_battery_callback();
		}
		break;
	case SEC_BATTERY_CHECK_PMIC:
	case SEC_BATTERY_CHECK_FUELGAUGE:
	case SEC_BATTERY_CHECK_CHARGER:
		ret = sec_bat_check_by_psy(battery);
		break;
	case SEC_BATTERY_CHECK_NONE:
		dev_dbg(battery->dev, "%s: No Check\n", __func__);
	default:
		break;
	}

	return ret;
}

static bool sec_bat_get_cable_type(
			struct sec_battery_info *battery,
			int cable_source_type)
{
	bool ret;
	int cable_type;

	ret = false;
	cable_type = battery->cable_type;

	if (cable_source_type & SEC_BATTERY_CABLE_SOURCE_CALLBACK) {
		if (battery->pdata->check_cable_callback)
			cable_type =
				battery->pdata->check_cable_callback();
	}

	if (cable_source_type & SEC_BATTERY_CABLE_SOURCE_ADC) {
		if (gpio_get_value_cansleep(
			battery->pdata->bat_gpio_ta_nconnected) ^
			battery->pdata->bat_polarity_ta_nconnected)
			cable_type = POWER_SUPPLY_TYPE_BATTERY;
		else
			cable_type =
				sec_bat_get_charger_type_adc(battery);
	}

	if (battery->cable_type == cable_type) {
		dev_dbg(battery->dev,
			"%s: No need to change cable status\n", __func__);
	} else {
		if (cable_type < POWER_SUPPLY_TYPE_BATTERY ||
			cable_type >= SEC_SIZEOF_POWER_SUPPLY_TYPE) {
			dev_err(battery->dev,
				"%s: Invalid cable type\n", __func__);
		} else {
			battery->cable_type = cable_type;
			if (battery->pdata->check_cable_result_callback)
				battery->pdata->check_cable_result_callback(
					battery->cable_type);

			ret = true;

			dev_dbg(battery->dev, "%s: Cable Changed (%d)\n",
				__func__, battery->cable_type);
		}
	}

	return ret;
}

static void sec_bat_set_charging_status(struct sec_battery_info *battery,
		int status) {
	union power_supply_propval value;
	switch (status) {
	case POWER_SUPPLY_STATUS_NOT_CHARGING:
	case POWER_SUPPLY_STATUS_DISCHARGING:
		if((battery->status == POWER_SUPPLY_STATUS_FULL) ||
		   (battery->capacity == 100)){
#if defined(CONFIG_AFC_CHARGER_MODE)
			value.intval = battery->capacity;
			psy_do_property(battery->pdata->fuelgauge_name, set,
					POWER_SUPPLY_PROP_CHARGE_FULL, value);
#else
			value.intval = POWER_SUPPLY_TYPE_BATTERY;
			psy_do_property(battery->pdata->fuelgauge_name, set,
					POWER_SUPPLY_PROP_CHARGE_FULL, value);
#endif
			/* To get SOC value (NOT raw SOC), need to reset value */
			value.intval = 0;
			psy_do_property(battery->pdata->fuelgauge_name, get,
					POWER_SUPPLY_PROP_CAPACITY, value);
			battery->capacity = value.intval;
		}
		break;
	default:
		break;
	}
	battery->status = status;
}

static bool sec_bat_battery_cable_check(struct sec_battery_info *battery)
{
	if (!sec_bat_check(battery)) {
		if (battery->check_count < battery->pdata->check_count)
			battery->check_count++;
		else {
			dev_err(battery->dev,
				"%s: Battery Disconnected\n", __func__);
			battery->present = false;
			battery->health = POWER_SUPPLY_HEALTH_UNSPEC_FAILURE;

			if (battery->status !=
				POWER_SUPPLY_STATUS_DISCHARGING) {
				sec_bat_set_charging_status(battery,
						POWER_SUPPLY_STATUS_NOT_CHARGING);
				sec_bat_set_charge(battery, false);
			}

			if (battery->pdata->check_battery_result_callback)
				battery->pdata->
					check_battery_result_callback();
			return false;
		}
	} else
		battery->check_count = 0;

	battery->present = true;

	if (battery->health == POWER_SUPPLY_HEALTH_UNSPEC_FAILURE) {
		battery->health = POWER_SUPPLY_HEALTH_GOOD;

		if (battery->status == POWER_SUPPLY_STATUS_NOT_CHARGING) {
			sec_bat_set_charging_status(battery,
					POWER_SUPPLY_STATUS_CHARGING);
#if defined(CONFIG_BATTERY_SWELLING)
			if (!battery->swelling_mode)
				sec_bat_set_charge(battery, true);
#else
			sec_bat_set_charge(battery, true);
#endif
		}
	}

	dev_dbg(battery->dev, "%s: Battery Connected\n", __func__);

	if (battery->pdata->cable_check_type &
		SEC_BATTERY_CABLE_CHECK_POLLING) {
		if (sec_bat_get_cable_type(battery,
			battery->pdata->cable_source_type)) {
			wake_lock(&battery->cable_wake_lock);
			queue_delayed_work_on(0, battery->monitor_wqueue,
					   &battery->cable_work, 0);
		}
	}
	return true;
}

static int sec_bat_ovp_uvlo_by_psy(struct sec_battery_info *battery)
{
	char *psy_name;
	union power_supply_propval value;

	value.intval = POWER_SUPPLY_HEALTH_GOOD;

	switch (battery->pdata->ovp_uvlo_check_type) {
	case SEC_BATTERY_OVP_UVLO_PMICPOLLING:
		psy_name = battery->pdata->pmic_name;
		break;
	case SEC_BATTERY_OVP_UVLO_CHGPOLLING:
		psy_name = battery->pdata->charger_name;
		break;
	default:
		dev_err(battery->dev,
			"%s: Invalid OVP/UVLO Check Type\n", __func__);
		goto ovp_uvlo_check_error;
		break;
	}

	psy_do_property(psy_name, get,
		POWER_SUPPLY_PROP_HEALTH, value);

ovp_uvlo_check_error:
	return value.intval;
}

static bool sec_bat_ovp_uvlo_result(
		struct sec_battery_info *battery, int health)
{
	if (battery->health != health) {
		battery->health = health;
		switch (health) {
		case POWER_SUPPLY_HEALTH_GOOD:
			dev_info(battery->dev, "%s: Safe voltage\n", __func__);
			dev_info(battery->dev, "%s: is_recharging : %d\n", __func__, battery->is_recharging);
			sec_bat_set_charging_status(battery,
					POWER_SUPPLY_STATUS_CHARGING);
			battery->charging_mode = SEC_BATTERY_CHARGING_1ST;
#if defined(CONFIG_BATTERY_SWELLING)
		if (!battery->swelling_mode)
			sec_bat_set_charge(battery, true);
#else
			sec_bat_set_charge(battery, true);
#endif
			break;
		case POWER_SUPPLY_HEALTH_OVERVOLTAGE:
		case POWER_SUPPLY_HEALTH_UNDERVOLTAGE:
			dev_info(battery->dev,
				"%s: Unsafe voltage (%d)\n",
				__func__, health);
			sec_bat_set_charging_status(battery,
					POWER_SUPPLY_STATUS_NOT_CHARGING);
			sec_bat_set_charge(battery, false);
			battery->charging_mode = SEC_BATTERY_CHARGING_NONE;
			battery->is_recharging = false;
			/* Take the wakelock during 10 seconds
			   when over-voltage status is detected	 */
			wake_lock_timeout(&battery->vbus_wake_lock, HZ * 10);
			break;
		}
		power_supply_changed(&battery->psy_bat);
		return true;
	}

	return false;
}

static bool sec_bat_ovp_uvlo(struct sec_battery_info *battery)
{
	int health;

	if ((battery->status == POWER_SUPPLY_STATUS_FULL) &&
		   (battery->charging_mode == SEC_BATTERY_CHARGING_NONE)) {
		dev_dbg(battery->dev, "%s: No need to check in Full status", __func__);
		return false;
	}

	if (battery->health != POWER_SUPPLY_HEALTH_GOOD &&
		battery->health != POWER_SUPPLY_HEALTH_OVERVOLTAGE &&
		battery->health != POWER_SUPPLY_HEALTH_UNDERVOLTAGE) {
		dev_dbg(battery->dev, "%s: No need to check\n", __func__);
		return false;
	}

	health = battery->health;

	switch (battery->pdata->ovp_uvlo_check_type) {
	case SEC_BATTERY_OVP_UVLO_CALLBACK:
		if (battery->pdata->ovp_uvlo_callback)
			health = battery->pdata->ovp_uvlo_callback();
		break;
	case SEC_BATTERY_OVP_UVLO_PMICPOLLING:
	case SEC_BATTERY_OVP_UVLO_CHGPOLLING:
		health = sec_bat_ovp_uvlo_by_psy(battery);
		break;
	case SEC_BATTERY_OVP_UVLO_PMICINT:
	case SEC_BATTERY_OVP_UVLO_CHGINT:
		/* nothing for interrupt check */
	default:
		break;
	}

	/* Move the location for calling the get_health
	   in case of attaching the jig */ 
	if (battery->factory_mode || battery->is_jig_on) {
		dev_dbg(battery->dev,
			"%s: No need to check in factory mode\n",
			__func__);
		return false;
	}

	return sec_bat_ovp_uvlo_result(battery, health);
}

static bool sec_bat_check_recharge(struct sec_battery_info *battery)
{
#if defined(CONFIG_BATTERY_SWELLING)
	if (battery->swelling_mode) {
		pr_info("%s: Skip normal recharge check routine for swelling mode\n",
			__func__);
		return false;
	}
#endif
	if ((battery->status == POWER_SUPPLY_STATUS_CHARGING) &&
			(battery->pdata->full_condition_type &
			 SEC_BATTERY_FULL_CONDITION_NOTIMEFULL) &&
			(battery->charging_mode == SEC_BATTERY_CHARGING_NONE)) {
		dev_info(battery->dev,
				"%s: Re-charging by NOTIMEFULL (%d)\n",
				__func__, battery->capacity);
		goto check_recharge_check_count;
	}

	if (battery->status == POWER_SUPPLY_STATUS_FULL &&
			battery->charging_mode == SEC_BATTERY_CHARGING_NONE) {
		if ((battery->pdata->recharge_condition_type &
					SEC_BATTERY_RECHARGE_CONDITION_SOC) &&
				(battery->capacity <=
				 battery->pdata->recharge_condition_soc)) {
			dev_info(battery->dev,
					"%s: Re-charging by SOC (%d)\n",
					__func__, battery->capacity);
			goto check_recharge_check_count;
		}

		if ((battery->pdata->recharge_condition_type &
					SEC_BATTERY_RECHARGE_CONDITION_AVGVCELL) &&
				(battery->voltage_avg <=
				 battery->pdata->recharge_condition_avgvcell)) {
			dev_info(battery->dev,
					"%s: Re-charging by average VCELL (%d)\n",
					__func__, battery->voltage_avg);
			goto check_recharge_check_count;
		}

		if ((battery->pdata->recharge_condition_type &
					SEC_BATTERY_RECHARGE_CONDITION_VCELL) &&
				(battery->voltage_now <=
				 battery->pdata->recharge_condition_vcell)) {
			dev_info(battery->dev,
					"%s: Re-charging by VCELL (%d)\n",
					__func__, battery->voltage_now);
			goto check_recharge_check_count;
		}
	}

	battery->recharge_check_cnt = 0;
	return false;

check_recharge_check_count:
	if (battery->recharge_check_cnt <
		battery->pdata->recharge_check_count)
		battery->recharge_check_cnt++;
	dev_dbg(battery->dev,
		"%s: recharge count = %d\n",
		__func__, battery->recharge_check_cnt);

	if (battery->recharge_check_cnt >=
		battery->pdata->recharge_check_count)
		return true;
	else
		return false;
}

static bool sec_bat_voltage_check(struct sec_battery_info *battery)
{
	union power_supply_propval value;

	if (battery->status == POWER_SUPPLY_STATUS_DISCHARGING) {
		dev_dbg(battery->dev,
			"%s: Charging Disabled\n", __func__);
		return true;
	}

	/* OVP/UVLO check */
	if (sec_bat_ovp_uvlo(battery)) {
		if (battery->pdata->ovp_uvlo_result_callback)
			battery->pdata->
				ovp_uvlo_result_callback(battery->health);
		return false;
	}

	if ((battery->status == POWER_SUPPLY_STATUS_FULL) &&
		(battery->charging_mode == SEC_BATTERY_CHARGING_2ND ||
		 battery->is_recharging)) {
		value.intval = 0;
		psy_do_property(battery->pdata->fuelgauge_name, get,
			POWER_SUPPLY_PROP_CAPACITY, value);
		if (value.intval <
			battery->pdata->full_condition_soc &&
				battery->voltage_now <
				(battery->pdata->recharge_condition_vcell - 50)) {
			sec_bat_set_charging_status(battery,
					POWER_SUPPLY_STATUS_CHARGING);
			battery->voltage_now = 1080;
			battery->voltage_avg = 1080;
			power_supply_changed(&battery->psy_bat);
			dev_info(battery->dev,
				"%s: battery status full -> charging, RepSOC(%d)\n", __func__, value.intval);
		}
	}

	/* Re-Charging check */
	if (sec_bat_check_recharge(battery)) {
		if (battery->pdata->full_check_type !=
			SEC_BATTERY_FULLCHARGED_NONE)
			battery->charging_mode = SEC_BATTERY_CHARGING_1ST;
		else
			battery->charging_mode = SEC_BATTERY_CHARGING_2ND;
		battery->is_recharging = true;
#if defined(CONFIG_BATTERY_CISD)
		battery->cisd.recharge_count++;
		battery->cisd.recharge_count_2++;
		if (battery->cisd.charging_end_time_2 > 0) {
			struct timespec now_ts;
			struct cisd *pcisd = &battery->cisd;
			now_ts = ktime_to_timespec(ktime_get_boottime());
			if ((now_ts.tv_sec - pcisd->charging_end_time_2) < pcisd->data[CISD_DATA_RECHARGING_TIME]) {
				pcisd->data[CISD_DATA_RECHARGING_TIME] = (int)(now_ts.tv_sec - pcisd->charging_end_time_2);
				dev_info(battery->dev,
						"%s: cisd Recharging TIME(%d)\n", __func__, pcisd->data[CISD_DATA_RECHARGING_TIME]);
			}
		}
#endif
#if defined(CONFIG_BATTERY_SWELLING)
		if (!battery->swelling_mode)
			sec_bat_set_charge(battery, true);
#else
		sec_bat_set_charge(battery, true);
#endif
		return false;
	}

	return true;
}

static bool sec_bat_get_temperature_by_adc(
				struct sec_battery_info *battery,
				enum sec_battery_adc_channel channel,
				union power_supply_propval *value)
{
	int temp = 0;
	int temp_adc;
	int low = 0;
	int high = 0;
	int mid = 0;
	const sec_bat_adc_table_data_t *temp_adc_table;
	unsigned int temp_adc_table_size;

	temp_adc = sec_bat_get_adc_value(battery, channel);
	if (temp_adc < 0)
		return true;

	switch (channel) {
	case SEC_BAT_ADC_CHANNEL_TEMP:
		temp_adc_table = battery->pdata->temp_adc_table;
		temp_adc_table_size =
			battery->pdata->temp_adc_table_size;
		battery->temp_adc = temp_adc;
		break;
	case SEC_BAT_ADC_CHANNEL_TEMP_AMBIENT:
		temp_adc_table = battery->pdata->temp_amb_adc_table;
		temp_adc_table_size =
			battery->pdata->temp_amb_adc_table_size;
		battery->temp_ambient_adc = temp_adc;
		break;
	case SEC_BAT_ADC_CHANNEL_CHG_TEMP:
		temp_adc_table = battery->pdata->chg_temp_adc_table;
		temp_adc_table_size =
			battery->pdata->chg_temp_adc_table_size;
		battery->chg_temp_adc = temp_adc;
		break;
	case SEC_BAT_ADC_CHANNEL_WPC_TEMP:
		temp_adc_table = battery->pdata->wpc_temp_adc_table;
		temp_adc_table_size =
			battery->pdata->wpc_temp_adc_table_size;
		battery->wpc_temp_adc = temp_adc;
		pr_info("%s wpc_temp_adc = %d \n",__func__, battery->wpc_temp_adc);
		break;
	case SEC_BAT_ADC_CHANNEL_CAMERA_TEMP:
		temp_adc_table = battery->pdata->camera_temp_adc_table;
		temp_adc_table_size =
			battery->pdata->camera_temp_adc_table_size;
		battery->camera_temp_adc = temp_adc;
		break;
	default:
		dev_err(battery->dev,
			"%s: Invalid Property\n", __func__);
		return false;
	}

	if (temp_adc_table[0].adc >= temp_adc) {
		temp = temp_adc_table[0].data;
		goto temp_by_adc_goto;
	} else if (temp_adc_table[temp_adc_table_size-1].adc <= temp_adc) {
		temp = temp_adc_table[temp_adc_table_size-1].data;
		goto temp_by_adc_goto;
	}

	high = temp_adc_table_size - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		if (temp_adc_table[mid].adc > temp_adc)
			high = mid - 1;
		else if (temp_adc_table[mid].adc < temp_adc)
			low = mid + 1;
		else {
			temp = temp_adc_table[mid].data;
			goto temp_by_adc_goto;
		}
	}

	temp = temp_adc_table[high].data;
	temp += ((temp_adc_table[low].data - temp_adc_table[high].data) *
		 (temp_adc - temp_adc_table[high].adc)) /
		(temp_adc_table[low].adc - temp_adc_table[high].adc);

temp_by_adc_goto:

#ifdef CONFIG_SEC_FACTORY
	/* if Sub PBA Unavailable when SMD test, fix to temp*/
	if(!battery->pdata->sub_pba_available){
		if(channel== SEC_BAT_ADC_CHANNEL_TEMP || channel==SEC_BAT_ADC_CHANNEL_TEMP_AMBIENT)      // temp test factory
			temp = 300;
	}
#endif

	value->intval = temp;

	dev_dbg(battery->dev,
		"%s: Temp(%d), Temp-ADC(%d)\n",
		__func__, temp, temp_adc);

	return true;
}

static bool sec_bat_temperature(
				struct sec_battery_info *battery)
{
	bool ret;
	ret = true;

	if (battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS ||
		battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS ||
		battery->cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS) {
		battery->temp_highlimit_threshold =
			battery->pdata->temp_highlimit_threshold_event;
		battery->temp_highlimit_recovery =
			battery->pdata->temp_highlimit_recovery_event;
		battery->temp_high_threshold =
			battery->pdata->wpc_high_threshold_normal;
		battery->temp_high_recovery =
			battery->pdata->wpc_high_recovery_normal;
		battery->temp_low_recovery =
			battery->pdata->wpc_low_recovery_normal;
		battery->temp_low_threshold =
			battery->pdata->wpc_low_threshold_normal;
	} else {
		if (battery->pdata->event_check && battery->event) {
			battery->temp_highlimit_threshold =
				battery->pdata->temp_highlimit_threshold_event;
			battery->temp_highlimit_recovery =
				battery->pdata->temp_highlimit_recovery_event;
			battery->temp_high_threshold =
				battery->pdata->temp_high_threshold_event;
			battery->temp_high_recovery =
				battery->pdata->temp_high_recovery_event;
			battery->temp_low_recovery =
				battery->pdata->temp_low_recovery_event;
			battery->temp_low_threshold =
				battery->pdata->temp_low_threshold_event;
		} else if (lpcharge) {
			battery->temp_highlimit_threshold =
				battery->pdata->temp_highlimit_threshold_lpm;
			battery->temp_highlimit_recovery =
				battery->pdata->temp_highlimit_recovery_lpm;
			battery->temp_high_threshold =
				battery->pdata->temp_high_threshold_lpm;
			battery->temp_high_recovery =
				battery->pdata->temp_high_recovery_lpm;
			battery->temp_low_recovery =
				battery->pdata->temp_low_recovery_lpm;
			battery->temp_low_threshold =
				battery->pdata->temp_low_threshold_lpm;
		} else {
			battery->temp_highlimit_threshold =
				battery->pdata->temp_highlimit_threshold_normal;
			battery->temp_highlimit_recovery =
				battery->pdata->temp_highlimit_recovery_normal;
			battery->temp_high_threshold =
				battery->pdata->temp_high_threshold_normal;
			battery->temp_high_recovery =
				battery->pdata->temp_high_recovery_normal;
			battery->temp_low_recovery =
				battery->pdata->temp_low_recovery_normal;
			battery->temp_low_threshold =
				battery->pdata->temp_low_threshold_normal;
		}
	}

	dev_info(battery->dev,
		"%s: HLT(%d) HLR(%d) HT(%d), HR(%d), LT(%d), LR(%d)\n",
		__func__, battery->temp_highlimit_threshold,
		battery->temp_highlimit_recovery,
		battery->temp_high_threshold,
		battery->temp_high_recovery,
		battery->temp_low_threshold,
		battery->temp_low_recovery);
	return ret;
}

#if defined(CONFIG_BATTERY_SWELLING)
static void sec_bat_swelling_check(struct sec_battery_info *battery, int temperature)
{
	union power_supply_propval val;
	int swelling_rechg_voltage;

	psy_do_property(battery->pdata->charger_name, get,
			POWER_SUPPLY_PROP_VOLTAGE_MAX, val);

	pr_info("%s: status(%d), swell_mode(%d:%d), cv(0x%02x), temp(%d)\n",
		__func__, battery->status, battery->swelling_mode,
		battery->charging_block, val.intval, temperature);

	/* swelling_mode
	   	under voltage over voltage, battery missing */
	if ((battery->status == POWER_SUPPLY_STATUS_DISCHARGING) ||\
		(battery->status == POWER_SUPPLY_STATUS_NOT_CHARGING)) {
		pr_info("%s: DISCHARGING or NOT-CHARGING. stop swelling mode\n", __func__);
		battery->swelling_mode = false;
		goto skip_swelling_chek;
	}

	if (!battery->swelling_mode) {
		if (((temperature >= battery->pdata->swelling_high_temp_block) ||
			(temperature <= battery->pdata->swelling_low_temp_block)) &&
			battery->pdata->temp_check_type) {

			if (temperature >= battery->pdata->swelling_high_temp_block &&
				battery->pdata->event_check &&
				!battery->event) {
				pr_info("%s: skip check swelling in high temperature event mode(%d)\n",
						__func__, battery->event);
				return;
			}

			pr_info("%s: swelling mode start. stop charging\n", __func__);
			battery->swelling_mode = true;
			battery->swelling_full_check_cnt = 0;
			sec_bat_set_charge(battery, false);
		}
	}

	if (!battery->voltage_now)
		return;

	if (battery->swelling_mode) {
		if (temperature <= battery->pdata->swelling_low_temp_recov)
			swelling_rechg_voltage = battery->pdata->swelling_low_rechg_voltage;
		else
			swelling_rechg_voltage = battery->pdata->swelling_high_rechg_voltage;

		if ((temperature <= battery->pdata->swelling_high_temp_recov) &&
		    (temperature >= battery->pdata->swelling_low_temp_recov)) {
			pr_info("%s: swelling mode end. restart charging\n", __func__);
			battery->swelling_mode = false;
			battery->charging_mode = SEC_BATTERY_CHARGING_1ST;
			sec_bat_set_charge(battery, true);
			/* restore 4.4V float voltage */
			val.intval = battery->pdata->swelling_normal_float_voltage;
			psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_VOLTAGE_MAX, val);
		} else if (battery->voltage_now < swelling_rechg_voltage &&
			   battery->charging_block) {
			pr_info("%s: swelling mode recharging start. Vbatt(%d)\n",
				__func__, battery->voltage_now);
			battery->charging_mode = SEC_BATTERY_CHARGING_1ST;
			sec_bat_set_charge(battery, true);
			/* change 4.20V float voltage */
			val.intval = battery->pdata->swelling_drop_float_voltage;
			psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_VOLTAGE_MAX, val);
			if ((temperature <= battery->pdata->swelling_low_temp_block) ||
			    (temperature >= battery->pdata->swelling_high_temp_block)) {
				pr_info("%s: swelling mode reduce charging current(temp:%d)\n",
					__func__, temperature);
				if (battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS ||
					battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS ||
					battery->cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS) {
					val.intval = battery->pdata->swelling_wc_chg_current;
				} else {
					val.intval = battery->pdata->swelling_chg_current;
				}
				psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_CURRENT_AVG, val);
			}
		}
	}
skip_swelling_chek:
	dev_dbg(battery->dev, "%s end\n", __func__);
}
#endif

#if defined(CONFIG_BATTERY_AGE_FORECAST)
static bool sec_bat_set_aging_step(struct sec_battery_info *battery, int step)
{
	union power_supply_propval value;

	if (battery->pdata->num_age_step <= 0 || step < 0 || step >= battery->pdata->num_age_step) {
		pr_info("%s: [AGE] abnormal age step : %d/%d\n",
			__func__, step, battery->pdata->num_age_step-1);
		return false;
	}

	if (battery->temperature < 50) {
		pr_info("%s: [AGE] skip (temperature:%d)\n", __func__, battery->temperature < 50);
		return false;
	}

	battery->pdata->age_step = step;

	/* float voltage */
	battery->pdata->chg_float_voltage =
		battery->pdata->age_data[battery->pdata->age_step].float_voltage;
	battery->pdata->swelling_normal_float_voltage =
		battery->pdata->chg_float_voltage;
	if (!battery->swelling_mode) {
		value.intval = battery->pdata->chg_float_voltage;
		psy_do_property(battery->pdata->charger_name, set,
			POWER_SUPPLY_PROP_VOLTAGE_MAX, value);
	}

	/* full/recharge condition */
	battery->pdata->recharge_condition_vcell =
		battery->pdata->age_data[battery->pdata->age_step].recharge_condition_vcell;
	battery->pdata->full_condition_soc =
		battery->pdata->age_data[battery->pdata->age_step].full_condition_soc;
	battery->pdata->full_condition_vcell =
		battery->pdata->age_data[battery->pdata->age_step].full_condition_vcell;

	value.intval = battery->pdata->full_condition_soc;
	psy_do_property(battery->pdata->fuelgauge_name, set,
		POWER_SUPPLY_PROP_CAPACITY_LEVEL, value);

	dev_info(battery->dev,
		 "%s: Step(%d/%d), Cycle(%d), float_v(%d), r_v(%d), f_s(%d), f_vl(%d)\n",
		 __func__,
		 battery->pdata->age_step, battery->pdata->num_age_step-1, battery->batt_cycle,
		 battery->pdata->chg_float_voltage,
		 battery->pdata->recharge_condition_vcell,
		 battery->pdata->full_condition_soc,
		 battery->pdata->full_condition_vcell);

	return true;
}

static void sec_bat_aging_check(struct sec_battery_info *battery)
{
	int prev_step = battery->pdata->age_step;
	int calc_step = -1;
	bool ret;

	if (battery->pdata->num_age_step <= 0)
		return;

	if (battery->temperature < 50) {
		pr_info("%s: [AGE] skip (temperature:%d)\n", __func__, battery->temperature);
		return;
	}

	for (calc_step = battery->pdata->num_age_step - 1; calc_step >= 0; calc_step--) {
		if (battery->pdata->age_data[calc_step].cycle <= battery->batt_cycle)
			break;
	}

	if (calc_step == prev_step)
		return;

	ret = sec_bat_set_aging_step(battery, calc_step);
	dev_info(battery->dev,
		 "%s: %s change step (%d->%d), Cycle(%d)\n",
		 __func__, ret ? "Succeed in" : "Fail to",
		 prev_step, battery->pdata->age_step, battery->batt_cycle);
}
#endif

static void sec_bat_camera_temp_check(struct sec_battery_info *battery)
{
	union power_supply_propval value;
	sec_bat_get_temperature_by_adc(battery,
				       SEC_BAT_ADC_CHANNEL_CAMERA_TEMP,
				       &value);
	battery->camera_temp = value.intval;

	if (battery->camera_temp >= battery->pdata->camera_temp_limit)
		battery->camera_limit = true;

	pr_info("%s : CAMERA_TEMP(%d) CAMERA_TEMP_ADC(%d) || CAMERA_LIMIT(%d)\n",
		__func__, battery->camera_temp, battery->camera_temp_adc,
		battery->camera_limit);
}

static bool sec_bat_temperature_check(
				struct sec_battery_info *battery)
{
	int temp_value;
	int pre_health;

	if (battery->status == POWER_SUPPLY_STATUS_DISCHARGING) {
		dev_dbg(battery->dev,
			"%s: Charging Disabled\n", __func__);
		return true;
	}

	if (battery->health != POWER_SUPPLY_HEALTH_GOOD &&
		battery->health != POWER_SUPPLY_HEALTH_OVERHEAT &&
		battery->health != POWER_SUPPLY_HEALTH_COLD &&
		battery->health != POWER_SUPPLY_HEALTH_OVERHEATLIMIT) {
		dev_dbg(battery->dev, "%s: No need to check\n", __func__);
		return false;
	}

	sec_bat_temperature(battery);

	switch (battery->pdata->temp_check_type) {
	case SEC_BATTERY_TEMP_CHECK_ADC:
		temp_value = battery->temp_adc;
		break;
	case SEC_BATTERY_TEMP_CHECK_TEMP:
		temp_value = battery->temperature;
		break;
	default:
		dev_err(battery->dev,
			"%s: Invalid Temp Check Type\n", __func__);
		return true;
	}
	pre_health = battery->health;

	if (temp_value >= battery->temp_highlimit_threshold) {
		if (battery->health != POWER_SUPPLY_HEALTH_OVERHEATLIMIT) {
			if (battery->temp_highlimit_cnt <
			    battery->pdata->temp_check_count) {
				battery->temp_highlimit_cnt++;
				battery->temp_high_cnt = 0;
				battery->temp_low_cnt = 0;
				battery->temp_recover_cnt = 0;
			}
			dev_dbg(battery->dev,
				"%s: highlimit count = %d\n",
				__func__, battery->temp_highlimit_cnt);
		}
	} else if ((temp_value >= battery->temp_high_threshold) ||
		   (battery->pdata->camera_temp_check && battery->camera_limit)) {
		if (battery->health == POWER_SUPPLY_HEALTH_OVERHEATLIMIT) {
			if (temp_value <= battery->temp_highlimit_recovery) {
				if (battery->temp_recover_cnt <
				    battery->pdata->temp_check_count) {
					battery->temp_recover_cnt++;
					battery->temp_highlimit_cnt = 0;
					battery->temp_high_cnt = 0;
					battery->temp_low_cnt = 0;
				}
				dev_dbg(battery->dev,
					"%s: recovery count = %d\n",
					__func__, battery->temp_recover_cnt);
			}
		} else if (battery->health != POWER_SUPPLY_HEALTH_OVERHEAT) {
			if (battery->temp_high_cnt <
			    battery->pdata->temp_check_count) {
				battery->temp_high_cnt++;
				battery->temp_highlimit_cnt = 0;
				battery->temp_low_cnt = 0;
				battery->temp_recover_cnt = 0;
			}
			dev_dbg(battery->dev,
				"%s: high count = %d\n",
				__func__, battery->temp_high_cnt);
		}
	} else if (((temp_value <= battery->temp_high_recovery) &&
		    (temp_value >= battery->temp_low_recovery)) &&
		   (!battery->pdata->camera_temp_check ||
		    (battery->pdata->camera_temp_check && !battery->camera_limit))) {
		if (battery->health == POWER_SUPPLY_HEALTH_OVERHEAT ||
		    battery->health == POWER_SUPPLY_HEALTH_OVERHEATLIMIT ||
		    battery->health == POWER_SUPPLY_HEALTH_COLD) {
			if (battery->temp_recover_cnt <
			    battery->pdata->temp_check_count) {
				battery->temp_recover_cnt++;
				battery->temp_highlimit_cnt = 0;
				battery->temp_high_cnt = 0;
				battery->temp_low_cnt = 0;
			}
			dev_dbg(battery->dev,
				"%s: recovery count = %d\n",
				__func__, battery->temp_recover_cnt);
		}
	} else if (temp_value <= battery->temp_low_threshold) {
		if (battery->health != POWER_SUPPLY_HEALTH_COLD) {
			if (battery->temp_low_cnt <
			    battery->pdata->temp_check_count) {
				battery->temp_low_cnt++;
				battery->temp_highlimit_cnt = 0;
				battery->temp_high_cnt = 0;
				battery->temp_recover_cnt = 0;
			}
			dev_dbg(battery->dev,
				"%s: low count = %d\n",
				__func__, battery->temp_low_cnt);
		}
	} else {
		battery->temp_highlimit_cnt = 0;
		battery->temp_high_cnt = 0;
		battery->temp_low_cnt = 0;
		battery->temp_recover_cnt = 0;
	}

	if (battery->temp_highlimit_cnt >=
	    battery->pdata->temp_check_count) {
		battery->health = POWER_SUPPLY_HEALTH_OVERHEATLIMIT;
		battery->temp_highlimit_cnt = 0;
	} else if (battery->temp_high_cnt >=
		battery->pdata->temp_check_count) {
		battery->health = POWER_SUPPLY_HEALTH_OVERHEAT;
		battery->temp_high_cnt = 0;
	} else if (battery->temp_low_cnt >=
		battery->pdata->temp_check_count) {
		battery->health = POWER_SUPPLY_HEALTH_COLD;
		battery->temp_low_cnt = 0;
	} else if (battery->temp_recover_cnt >=
		 battery->pdata->temp_check_count) {
		if (battery->health == POWER_SUPPLY_HEALTH_OVERHEATLIMIT) {
			battery->health = POWER_SUPPLY_HEALTH_OVERHEAT;
		} else {
#if defined(CONFIG_BATTERY_SWELLING_SELF_DISCHARGING)
			union power_supply_propval value;

			psy_do_property(battery->pdata->charger_name, get,
					POWER_SUPPLY_PROP_VOLTAGE_MAX, value);
			if (value.intval <= battery->pdata->swelling_normal_float_voltage) {
				value.intval = battery->pdata->swelling_normal_float_voltage;
				psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_VOLTAGE_MAX, value);
			}
#endif
			battery->health = POWER_SUPPLY_HEALTH_GOOD;
		}
		battery->temp_recover_cnt = 0;
	}
	if (pre_health != battery->health) {
		battery->health_change = true;
		dev_info(battery->dev, "%s, health_change true\n", __func__);
	} else {
		battery->health_change = false;
	}

	if ((battery->health == POWER_SUPPLY_HEALTH_OVERHEAT) ||
		(battery->health == POWER_SUPPLY_HEALTH_COLD) ||
		(battery->health == POWER_SUPPLY_HEALTH_OVERHEATLIMIT)) {
		if (battery->status != POWER_SUPPLY_STATUS_NOT_CHARGING) {
#if defined(CONFIG_BATTERY_SWELLING_SELF_DISCHARGING)
			if ((battery->health == POWER_SUPPLY_HEALTH_OVERHEAT) ||
				(battery->health == POWER_SUPPLY_HEALTH_OVERHEATLIMIT)) {
				union power_supply_propval val;
				/* change 4.20V float voltage */
				val.intval = battery->pdata->swelling_drop_float_voltage;
				psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_VOLTAGE_MAX, val);
			}
#endif
#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY) || defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
			if (battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS ||
				battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS ||
				battery->cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS) {
				union power_supply_propval val;
				val.intval = battery->health;
				psy_do_property(battery->pdata->wireless_charger_name, set,
						POWER_SUPPLY_PROP_HEALTH, val);
			}
#endif
			dev_info(battery->dev,
				"%s: Unsafe Temperature\n", __func__);
			sec_bat_set_charging_status(battery,
					POWER_SUPPLY_STATUS_NOT_CHARGING);
			/* change charging current to battery (default 0mA) */
			sec_bat_set_charge(battery, false);
			return false;
		}
	} else {
		/* if recovered from not charging */
		if ((battery->health == POWER_SUPPLY_HEALTH_GOOD) &&
			(battery->status ==
			 POWER_SUPPLY_STATUS_NOT_CHARGING)) {
			dev_info(battery->dev,
					"%s: Safe Temperature\n", __func__);
			if (battery->capacity >= 100)
				sec_bat_set_charging_status(battery,
						POWER_SUPPLY_STATUS_FULL);
			else	/* Normal Charging */
				sec_bat_set_charging_status(battery,
						POWER_SUPPLY_STATUS_CHARGING);
#if defined(CONFIG_BATTERY_SWELLING)
			if ((temp_value >= battery->pdata->swelling_high_temp_block) ||
				(temp_value <= battery->pdata->swelling_low_temp_block)) {
				pr_info("%s: swelling mode start. stop charging\n", __func__);
				battery->swelling_mode = true;
				battery->swelling_full_check_cnt = 0;
				sec_bat_set_charge(battery, false);
			} else {
				/* turn on charger by cable type */
				sec_bat_set_charge(battery, true);
			}
#else
			/* turn on charger by cable type */
			sec_bat_set_charge(battery, true);
#endif
			return false;
		}
	}
	return true;
}

#if !defined(CONFIG_SEC_FACTORY)
static void sec_bat_chg_temperature_check(
	struct sec_battery_info *battery)
{
#if defined(CONFIG_MUIC_HV) || defined(CONFIG_AFC_CHARGER_MODE)
	static bool is_vbus_changed = false;
	union power_supply_propval value;
#endif

#if defined(CONFIG_MUIC_HV) || defined(CONFIG_AFC_CHARGER_MODE)
	pr_info("%s: called (is_vbus:%d, cable:%d))\n", __func__, is_vbus_changed, battery->cable_type);
	if (battery->siop_level >= 100 && is_vbus_changed &&
		battery->cable_type == POWER_SUPPLY_TYPE_HV_MAINS_CHG_LIMIT) {
		is_vbus_changed = false;
		value.intval = SEC_INPUT_VOLTAGE_9V;
		psy_do_property(battery->pdata->charger_name, set,
			POWER_SUPPLY_PROP_SET_CHARGE_VOLTAGE, value);
		pr_info("%s: changed vbus level 5V -> 9V\n", __func__);
		return;
	} else if (battery->siop_level < 100 && !is_vbus_changed &&
		battery->status == POWER_SUPPLY_STATUS_CHARGING &&
		is_hv_wire_type(battery->cable_type)) {
		is_vbus_changed = true;
		pr_info("%s: changed vbus level 9V -> 5V\n", __func__);
		value.intval = SEC_INPUT_VOLTAGE_5V;
		psy_do_property(battery->pdata->charger_name, set,
			POWER_SUPPLY_PROP_SET_CHARGE_VOLTAGE, value);

		return;
	} else {
		if ((is_vbus_changed && is_hv_wire_type(battery->cable_type)) ||
			(!is_vbus_changed && (battery->cable_type == POWER_SUPPLY_TYPE_HV_MAINS_CHG_LIMIT))) {
			pr_info("%s: prevent abnormal case (cable_type:%d, is_vbus_changed:%d)\n",
				__func__, battery->cable_type, is_vbus_changed);
			return;
		} else if (battery->cable_type == POWER_SUPPLY_TYPE_BATTERY) {
			is_vbus_changed = false;
		}
	}
#endif

	if (battery->siop_level >= 100 && (battery->mix_limit == SEC_BATTERY_MIX_TEMP_NONE) &&
			((battery->cable_type == POWER_SUPPLY_TYPE_HV_MAINS) ||
			 (battery->cable_type == POWER_SUPPLY_TYPE_HV_ERR))) {
		union power_supply_propval value;
		if ((battery->chg_limit == SEC_BATTERY_CHG_TEMP_NONE) &&
				(battery->chg_temp > battery->pdata->chg_high_temp_1st)) {
			battery->chg_limit = SEC_BATTERY_CHG_TEMP_HIGH_1ST;
			value.intval = battery->pdata->chg_charging_limit_current;
			psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_CURRENT_MAX, value);
			dev_info(battery->dev,"%s: Chg current is reduced by Temp: %d\n",
					__func__, battery->chg_temp);
		} else if ((battery->chg_limit == SEC_BATTERY_CHG_TEMP_HIGH_1ST) &&
				(battery->pre_chg_temp < battery->pdata->chg_high_temp_2nd) &&
				(battery->chg_temp > battery->pdata->chg_high_temp_2nd)) {
			battery->chg_limit = SEC_BATTERY_CHG_TEMP_HIGH_2ND;
			value.intval = battery->pdata->chg_charging_limit_current_2nd;
			psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_CURRENT_MAX, value);
			dev_info(battery->dev,"%s: Chg current 2nd is reduced by Temp: %d\n",
					__func__, battery->chg_temp);
		} else if ((battery->chg_limit != SEC_BATTERY_CHG_TEMP_NONE) &&
				(battery->chg_temp < battery->pdata->chg_high_temp_recovery)) {
			battery->chg_limit = SEC_BATTERY_CHG_TEMP_NONE;
			value.intval = battery->pdata->charging_current
				[battery->cable_type].input_current_limit;
			psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_CURRENT_MAX, value);
			dev_info(battery->dev,"%s: Chg current is recovered by Temp: %d\n",
					__func__, battery->chg_temp);
		}
#if defined(CONFIG_WIRELESS_CHARGER_THM)
	} else if (battery->siop_level >= 100 &&
			(battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS ||
			 battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS ||
			 battery->cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS) && battery->pdata->wpc_temp_check) {
		union power_supply_propval value;
		if ((battery->chg_limit == SEC_BATTERY_CHG_TEMP_NONE) &&
			(battery->wpc_temp > battery->pdata->wpc_high_temp)) {

#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
			wpc_temp_mode = true;
#endif
			battery->chg_limit = SEC_BATTERY_CHG_TEMP_HIGH_1ST;
			value.intval = battery->pdata->wpc_charging_limit_current;
			psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_CURRENT_MAX, value);
			dev_info(battery->dev,"%s: WPC Chg current is reduced by Temp: %d\n",
					__func__, battery->wpc_temp);
		} else if ((battery->chg_limit != SEC_BATTERY_CHG_TEMP_NONE) &&
			(battery->wpc_temp < battery->pdata->wpc_high_temp_recovery)) {

#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
			wpc_temp_mode = false;
#endif
			battery->chg_limit = SEC_BATTERY_CHG_TEMP_NONE;
			value.intval = battery->pdata->charging_current
				[battery->cable_type].input_current_limit;
			psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_CURRENT_MAX, value);
			dev_info(battery->dev,"%s: WPC Chg current is recovered by Temp: %d\n",
					__func__, battery->wpc_temp);
		}

		if (battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS &&
			battery->pad_limit != SEC_BATTERY_WPC_TEMP_NONE &&
			battery->wpc_temp < battery->pdata->wpc_lcd_on_high_temp_rec ) {

			battery->pad_limit = SEC_BATTERY_WPC_TEMP_NONE;

			value.intval = WIRELESS_VOUT_9V;
			psy_do_property(battery->pdata->wireless_charger_name, set,
				POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);

			dev_info(battery->dev,"%s: RX voltage is recovered by Temp: %d, siop(%d)\n",
					__func__, battery->wpc_temp, battery->siop_level);
		}
	} else if (battery->siop_level < 100 &&
			battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS ) {
		union power_supply_propval value;
		if ((battery->pad_limit == SEC_BATTERY_WPC_TEMP_NONE) &&
			(battery->wpc_temp > battery->pdata->wpc_lcd_on_high_temp)) {
			battery->pad_limit = SEC_BATTERY_WPC_TEMP_HIGH;

			value.intval = WIRELESS_VOUT_5V;
			psy_do_property(battery->pdata->wireless_charger_name, set,
					POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);

			dev_info(battery->dev,"%s: RX voltage is reduced by Temp: %d\n",
					__func__, battery->wpc_temp);
		} else if ((battery->pad_limit != SEC_BATTERY_WPC_TEMP_NONE) &&
			(battery->wpc_temp < battery->pdata->wpc_lcd_on_high_temp_rec)) {
			battery->pad_limit = SEC_BATTERY_WPC_TEMP_NONE;

			value.intval = WIRELESS_VOUT_9V;
			psy_do_property(battery->pdata->wireless_charger_name, set,
					POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);

			dev_info(battery->dev,"%s: RX voltage is recovered by Temp: %d\n",
					__func__, battery->wpc_temp);
		}
#else
	} else if (battery->siop_level >= 100 &&
			(battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS) && battery->pdata->wpc_temp_check) {
		union power_supply_propval value;
		if ((battery->chg_limit == SEC_BATTERY_CHG_TEMP_NONE) &&
				(battery->chg_temp > battery->pdata->wpc_high_temp)) {
			battery->chg_limit = SEC_BATTERY_CHG_TEMP_HIGH_1ST;
			value.intval = battery->pdata->wpc_charging_limit_current;
			psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_CURRENT_MAX, value);
			dev_info(battery->dev,"%s: WPC Chg current is reduced by Temp: %d\n",
					__func__, battery->chg_temp);
		} else if ((battery->chg_limit != SEC_BATTERY_CHG_TEMP_NONE) &&
				(battery->chg_temp < battery->pdata->wpc_high_temp_recovery)) {
			battery->chg_limit = SEC_BATTERY_CHG_TEMP_NONE;
			value.intval = battery->pdata->charging_current
				[battery->cable_type].input_current_limit;
			psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_CURRENT_MAX, value);
			dev_info(battery->dev,"%s: WPC Chg current is recovered by Temp: %d\n",
					__func__, battery->chg_temp);
		}
#endif
	} else if (battery->chg_limit != SEC_BATTERY_CHG_TEMP_NONE) {
		battery->chg_limit = SEC_BATTERY_CHG_TEMP_NONE;
#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
		wpc_temp_mode = false;
#endif
#if defined(CONFIG_WIRELESS_CHARGER_THM)
	} else if (battery->pad_limit != SEC_BATTERY_WPC_TEMP_NONE) {
		battery->pad_limit = SEC_BATTERY_WPC_TEMP_NONE;
#endif
	}
}

static void sec_bat_mix_temperature_check(
	struct sec_battery_info *battery) {
	union power_supply_propval value;
	int mix_high_tbat, mix_high_tchg, mix_high_tbat_recov;
	unsigned int mix_input_limit_current;

	if ((battery->cable_type == POWER_SUPPLY_TYPE_HV_MAINS) ||
		(battery->cable_type == POWER_SUPPLY_TYPE_HV_ERR) ||
		(battery->cable_type == POWER_SUPPLY_TYPE_MAINS)) {

		if ((battery->cable_type == POWER_SUPPLY_TYPE_HV_MAINS) ||
			(battery->cable_type == POWER_SUPPLY_TYPE_HV_ERR)) {
			mix_high_tbat = battery->pdata->mix_high_tbat_hv;
			mix_high_tchg = battery->pdata->mix_high_tchg_hv;
			mix_high_tbat_recov = battery->pdata->mix_high_tbat_recov_hv;
			mix_input_limit_current = battery->pdata->mix_input_limit_current_hv;
		} else {
			mix_high_tbat = battery->pdata->mix_high_tbat;
			mix_high_tchg = battery->pdata->mix_high_tchg;
			mix_high_tbat_recov = battery->pdata->mix_high_tbat_recov;
			mix_input_limit_current = battery->pdata->mix_input_limit_current;
		}

		if (battery->siop_level >= 100) {
			if ((battery->mix_limit == SEC_BATTERY_MIX_TEMP_NONE) &&
				(battery->temperature >= mix_high_tbat) &&
				(battery->chg_temp >= mix_high_tchg)) {
				battery->mix_limit = SEC_BATTERY_MIX_TEMP_HIGH;
				value.intval = mix_input_limit_current;
				psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_CURRENT_MAX, value);
				dev_info(battery->dev,"%s: Input current is reduced by Temp(tbat:%d, tchg:%d)\n",
						__func__, battery->temperature, battery->chg_temp);
			} else if ((battery->mix_limit != SEC_BATTERY_MIX_TEMP_NONE) &&
					(battery->temperature < mix_high_tbat_recov)) {
				battery->mix_limit = SEC_BATTERY_MIX_TEMP_NONE;
				value.intval = battery->pdata->charging_current
						[battery->cable_type].input_current_limit;
				psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_CURRENT_MAX, value);
				dev_info(battery->dev,"%s: Input current is recovered by Temp(tbat:%d)\n",
						__func__, battery->temperature);
			}
		} else {
			psy_do_property(battery->pdata->charger_name, get,
					POWER_SUPPLY_PROP_CURRENT_MAX, value);
			if (value.intval == mix_input_limit_current) {
				battery->mix_limit = SEC_BATTERY_MIX_TEMP_NONE;
				battery->chg_limit = SEC_BATTERY_CHG_TEMP_NONE;

				value.intval = battery->pdata->charging_current
					[battery->cable_type].input_current_limit;
				psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_CURRENT_MAX, value);

				value.intval = battery->siop_level;
				psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN, value);

				dev_info(battery->dev,"%s: Input current is recovered\n", __func__);
			}
		}
	} else if (battery->mix_limit != SEC_BATTERY_MIX_TEMP_NONE) {
		battery->mix_limit = SEC_BATTERY_MIX_TEMP_NONE;
	}
}
#endif

static int sec_bat_get_inbat_vol_by_adc(struct sec_battery_info *battery)
{
	int inbat = 0;
	int inbat_adc;
	int low = 0;
	int high = 0;
	int mid = 0;
	const sec_bat_adc_table_data_t *inbat_adc_table;
	unsigned int inbat_adc_table_size;

	if (!battery->pdata->inbat_adc_table) {
		dev_err(battery->dev, "%s: not designed to read in-bat voltage\n", __func__);
		return -1;
	}

	inbat_adc_table = battery->pdata->inbat_adc_table;
	inbat_adc_table_size =
		battery->pdata->inbat_adc_table_size;

	inbat_adc = sec_bat_get_adc_value(battery, SEC_BAT_ADC_CHANNEL_INBAT_VOLTAGE);
	if (inbat_adc <= 0)
		return inbat_adc;
	battery->inbat_adc = inbat_adc;

	if (inbat_adc_table[0].adc <= inbat_adc) {
		inbat = inbat_adc_table[0].data;
		goto inbat_by_adc_goto;
	} else if (inbat_adc_table[inbat_adc_table_size-1].adc >= inbat_adc) {
		inbat = inbat_adc_table[inbat_adc_table_size-1].data;
		goto inbat_by_adc_goto;
	}

	high = inbat_adc_table_size - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		if (inbat_adc_table[mid].adc < inbat_adc)
			high = mid - 1;
		else if (inbat_adc_table[mid].adc > inbat_adc)
			low = mid + 1;
		else {
			inbat = inbat_adc_table[mid].data;
			goto inbat_by_adc_goto;
		}
	}

	inbat = inbat_adc_table[high].data;
	inbat +=
		((inbat_adc_table[low].data - inbat_adc_table[high].data) *
		 (inbat_adc - inbat_adc_table[high].adc)) /
		(inbat_adc_table[low].adc - inbat_adc_table[high].adc);

	if (inbat < 0)
		inbat = 0;

inbat_by_adc_goto:
	dev_info(battery->dev,
			"%s: inbat(%d), inbat-ADC(%d)\n",
			__func__, inbat, inbat_adc);

	return inbat;
}

static void  sec_bat_event_program_alarm(
	struct sec_battery_info *battery, int seconds)
{
#if defined(ANDROID_ALARM_ACTIVATED)
	ktime_t low_interval = ktime_set(seconds - 10, 0);
	ktime_t slack = ktime_set(20, 0);
	ktime_t next;

	next = ktime_add(battery->last_event_time, low_interval);
	alarm_start_range(&battery->event_termination_alarm,
		next, ktime_add(next, slack));
#else
	alarm_start(&battery->event_termination_alarm,
		ktime_add(battery->last_event_time, ktime_set(seconds - 10, 0)));
#endif
}

#if defined(ANDROID_ALARM_ACTIVATED)
static void sec_bat_event_expired_timer_func(struct alarm *alarm)
#else
static enum alarmtimer_restart sec_bat_event_expired_timer_func(
	struct alarm *alarm, ktime_t now)
#endif
{
	struct sec_battery_info *battery =
		container_of(alarm, struct sec_battery_info,
			event_termination_alarm);

	battery->event &= (~battery->event_wait);
	dev_info(battery->dev,
		"%s: event expired (0x%x)\n", __func__, battery->event);

#if !defined(ANDROID_ALARM_ACTIVATED)
	return ALARMTIMER_NORESTART;
#endif
}

static void sec_bat_event_set(
	struct sec_battery_info *battery, int event, int enable)
{
	if (!battery->pdata->event_check)
		return;

	/* ignore duplicated deactivation of same event
	 * only if the event is one last event
	 */
	if (!enable && (battery->event == battery->event_wait)) {
		dev_info(battery->dev,
			"%s: ignore duplicated deactivation of same event\n",
			__func__);
		return;
	}

	alarm_cancel(&battery->event_termination_alarm);
	battery->event &= (~battery->event_wait);

	if (enable) {
		battery->event_wait = 0;
		battery->event |= event;

		dev_info(battery->dev,
			"%s: event set (0x%x)\n", __func__, battery->event);
	} else {
		if (battery->event == 0) {
			dev_dbg(battery->dev,
				"%s: nothing to clear\n", __func__);
			return;	/* nothing to clear */
		}
		battery->event_wait = event;
#if defined(ANDROID_ALARM_ACTIVATED)
		battery->last_event_time = alarm_get_elapsed_realtime();
#else
		battery->last_event_time = ktime_get_boottime();
#endif
		sec_bat_event_program_alarm(battery,
			battery->pdata->event_waiting_time);
		dev_info(battery->dev,
			"%s: start timer (curr 0x%x, wait 0x%x)\n",
			__func__, battery->event, battery->event_wait);
	}
}

static bool sec_bat_check_fullcharged_condition(
					struct sec_battery_info *battery)
{
	int full_check_type;

	if (battery->charging_mode == SEC_BATTERY_CHARGING_1ST)
		full_check_type = battery->pdata->full_check_type;
	else
		full_check_type = battery->pdata->full_check_type_2nd;

	switch (full_check_type) {
	case SEC_BATTERY_FULLCHARGED_ADC:
	case SEC_BATTERY_FULLCHARGED_FG_CURRENT:
	case SEC_BATTERY_FULLCHARGED_SOC:
	case SEC_BATTERY_FULLCHARGED_CHGGPIO:
	case SEC_BATTERY_FULLCHARGED_CHGPSY:
		break;

	/* If these is NOT full check type or NONE full check type,
	 * it is full-charged
	 */
	case SEC_BATTERY_FULLCHARGED_CHGINT:
	case SEC_BATTERY_FULLCHARGED_TIME:
	case SEC_BATTERY_FULLCHARGED_NONE:
	default:
		return true;
		break;
	}

	if (battery->pdata->full_condition_type &
		SEC_BATTERY_FULL_CONDITION_SOC) {
		if (battery->capacity <
			battery->pdata->full_condition_soc) {
			dev_dbg(battery->dev,
				"%s: Not enough SOC (%d%%)\n",
				__func__, battery->capacity);
			return false;
		}
	}

	if (battery->pdata->full_condition_type &
		SEC_BATTERY_FULL_CONDITION_VCELL) {
		if (battery->voltage_now <
			battery->pdata->full_condition_vcell) {
			dev_dbg(battery->dev,
				"%s: Not enough VCELL (%dmV)\n",
				__func__, battery->voltage_now);
			return false;
		}
	}

	if (battery->pdata->full_condition_type &
		SEC_BATTERY_FULL_CONDITION_AVGVCELL) {
		if (battery->voltage_avg <
			battery->pdata->full_condition_avgvcell) {
			dev_dbg(battery->dev,
				"%s: Not enough AVGVCELL (%dmV)\n",
				__func__, battery->voltage_avg);
			return false;
		}
	}

	if (battery->pdata->full_condition_type &
		SEC_BATTERY_FULL_CONDITION_OCV) {
		if (battery->voltage_ocv <
			battery->pdata->full_condition_ocv) {
			dev_dbg(battery->dev,
				"%s: Not enough OCV (%dmV)\n",
				__func__, battery->voltage_ocv);
			return false;
		}
	}

	return true;
}

static void sec_bat_do_test_function(
		struct sec_battery_info *battery)
{
	union power_supply_propval value;

	switch (battery->test_mode) {
		case 1:
			if (battery->status == POWER_SUPPLY_STATUS_CHARGING) {
				sec_bat_set_charge(battery, false);
				sec_bat_set_charging_status(battery,
						POWER_SUPPLY_STATUS_DISCHARGING);
			}
			break;
		case 2:
			if(battery->status == POWER_SUPPLY_STATUS_DISCHARGING) {
				sec_bat_set_charge(battery, true);
				psy_do_property(battery->pdata->charger_name, get,
						POWER_SUPPLY_PROP_STATUS, value);
				sec_bat_set_charging_status(battery, value.intval);
			}
			battery->test_mode = 0;
			break;
		case 3: // clear temp block
			battery->health = POWER_SUPPLY_HEALTH_GOOD;
			sec_bat_set_charging_status(battery,
					POWER_SUPPLY_STATUS_DISCHARGING);
			break;
		case 4:
			if(battery->status == POWER_SUPPLY_STATUS_DISCHARGING) {
				sec_bat_set_charge(battery, true);
				psy_do_property(battery->pdata->charger_name, get,
						POWER_SUPPLY_PROP_STATUS, value);
				sec_bat_set_charging_status(battery, value.intval);
			}
			break;
		default:
			pr_info("%s: error test: unknown state\n", __func__);
			break;
	}
}

static bool sec_bat_time_management(
				struct sec_battery_info *battery)
{
	unsigned long charging_time;
	unsigned long expired_time = 0;
	struct timespec ts = {0, };
	unsigned int charging_total_time = 10 * 60 * 60;

#if defined(ANDROID_ALARM_ACTIVATED)
	ktime_t current_time;

	current_time = alarm_get_elapsed_realtime();
	ts = ktime_to_timespec(current_time);
#else
	get_monotonic_boottime(&ts);
#endif

	if (battery->charging_start_time == 0) {
		dev_dbg(battery->dev,
			"%s: Charging Disabled\n", __func__);
		return true;
	}

	if (ts.tv_sec >= battery->charging_start_time)
		charging_time = ts.tv_sec - battery->charging_start_time;
	else
		charging_time = 0xFFFFFFFF - battery->charging_start_time
			+ ts.tv_sec;

	battery->charging_passed_time = charging_time;

	if (!battery->store_mode){
		if (battery->cable_type == POWER_SUPPLY_TYPE_HV_MAINS ||
		    battery->cable_type == POWER_SUPPLY_TYPE_HV_UNKNOWN ||
		    battery->cable_type == POWER_SUPPLY_TYPE_HV_ERR) {
			charging_total_time = battery->pdata->hv_charging_total_time;
		} else if (battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS ||
			   battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS ||
			   battery->cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS) {
			charging_total_time = battery->pdata->normal_charging_total_time;
		} else if (battery->cable_type == POWER_SUPPLY_TYPE_USB) {
			charging_total_time = battery->pdata->usb_charging_total_time;
		} else {
			charging_total_time = battery->pdata->normal_charging_total_time;
		}
	}

	dev_info(battery->dev,
		"%s: Safety Timer %d secs, Charging Time : %ld secs\n", __func__,
		 charging_total_time, battery->charging_passed_time);

	if (battery->siop_level >= 100) {
		if (charging_time < battery->lcd_on_total_time)
                        battery->lcd_on_total_time = charging_time;
		expired_time = charging_time - battery->lcd_on_total_time;
		pr_info("%s : Total Charging time : %ld, Expired Time : %ld, LCD On total Time : %ld\n",
			__func__, charging_time, expired_time, battery->lcd_on_total_time);
	} else {
		return true;
	}

#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY) && !defined(CONFIG_WIRELESS_CHARGER_THM)
	if (battery->pdata->wpc_delayed_current_en &&
		(battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS) &&
		(battery->status == POWER_SUPPLY_STATUS_CHARGING) &&
		(battery->wc_current == battery->pdata->wpc_delayed_current) &&
		(battery->charging_passed_time > battery->pdata->wpc_delayed_current_time)) {
		union power_supply_propval value;
		pr_info("%s changed wireless current to default value \n",__func__);
		battery->wc_current = battery->pdata->charging_current
			[POWER_SUPPLY_TYPE_WIRELESS].input_current_limit;
		value.intval = battery->wc_current;
		psy_do_property(battery->pdata->charger_name, set,
			POWER_SUPPLY_PROP_CURRENT_MAX, value);
	}
#endif

	switch (battery->status) {
	case POWER_SUPPLY_STATUS_FULL:
		if (battery->is_recharging && (expired_time >
			battery->pdata->recharging_total_time)) {
			dev_info(battery->dev,
				"%s: Recharging Timer Expired\n", __func__);
			battery->charging_mode = SEC_BATTERY_CHARGING_NONE;
			battery->health = POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE;
			sec_bat_set_charging_status(battery, POWER_SUPPLY_STATUS_NOT_CHARGING);
			battery->is_recharging = false;
			if (sec_bat_set_charge(battery, false)) {
				dev_err(battery->dev,
					"%s: Fail to Set Charger\n", __func__);
				return true;
			}

			return false;
		}
		break;
	case POWER_SUPPLY_STATUS_CHARGING:
		if ((battery->pdata->full_condition_type &
			SEC_BATTERY_FULL_CONDITION_NOTIMEFULL) &&
			(battery->is_recharging && (expired_time >
			battery->pdata->recharging_total_time))) {
			dev_info(battery->dev,
			"%s: Recharging Timer Expired\n", __func__);
			battery->charging_mode = SEC_BATTERY_CHARGING_NONE;
			battery->health = POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE;
			sec_bat_set_charging_status(battery, POWER_SUPPLY_STATUS_NOT_CHARGING);
			battery->is_recharging = false;
			if (sec_bat_set_charge(battery, false)) {
				dev_err(battery->dev,
					"%s: Fail to Set Charger\n", __func__);
				return true;
			}
			return false;
		} else if (!battery->is_recharging &&
			(expired_time > charging_total_time)) {
			dev_info(battery->dev,
				"%s: Charging Timer Expired\n", __func__);
			battery->charging_mode = SEC_BATTERY_CHARGING_NONE;
			battery->health = POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE;
			sec_bat_set_charging_status(battery, POWER_SUPPLY_STATUS_NOT_CHARGING);
			if (sec_bat_set_charge(battery, false)) {
				dev_err(battery->dev,
					"%s: Fail to Set Charger\n", __func__);
				return true;
			}

			return false;
		}
		if (battery->pdata->charging_reset_time) {
			if (charging_time > battery->charging_next_time) {
				/*reset current in charging status */
				battery->charging_next_time =
					battery->charging_passed_time +
					(battery->pdata->charging_reset_time);

				dev_dbg(battery->dev,
					"%s: Reset charging current\n",
					__func__);
#if defined(CONFIG_BATTERY_SWELLING)
				if (!battery->swelling_mode) {
					if (sec_bat_set_charge(battery, true)) {
						dev_err(battery->dev,
							"%s: Fail to Set Charger\n",
							__func__);
						return true;
					}
				}
#else
				if (sec_bat_set_charge(battery, true)) {
					dev_err(battery->dev,
						"%s: Fail to Set Charger\n",
						__func__);
					return true;
				}
#endif
			}
		}
		break;
	default:
		dev_err(battery->dev,
			"%s: Undefine Battery Status\n", __func__);
		return true;
	}

	return true;
}

static bool sec_bat_check_fullcharged(
				struct sec_battery_info *battery)
{
	union power_supply_propval value;
	int current_adc;
	int full_check_type;
	bool ret;
	int err;

	ret = false;

	if (!sec_bat_check_fullcharged_condition(battery))
		goto not_full_charged;

	if (battery->charging_mode == SEC_BATTERY_CHARGING_1ST)
		full_check_type = battery->pdata->full_check_type;
	else
		full_check_type = battery->pdata->full_check_type_2nd;

	switch (full_check_type) {
	case SEC_BATTERY_FULLCHARGED_ADC:
		current_adc =
			sec_bat_get_adc_value(battery,
			SEC_BAT_ADC_CHANNEL_FULL_CHECK);

		dev_dbg(battery->dev,
			"%s: Current ADC (%d)\n",
			__func__, current_adc);

		if (current_adc < 0)
			break;
		battery->current_adc = current_adc;

		if (battery->current_adc <
			(battery->charging_mode ==
			SEC_BATTERY_CHARGING_1ST ?
			battery->pdata->charging_current[
			battery->cable_type].full_check_current_1st :
			battery->pdata->charging_current[
			battery->cable_type].full_check_current_2nd)) {
			battery->full_check_cnt++;
			dev_dbg(battery->dev,
				"%s: Full Check ADC (%d)\n",
				__func__,
				battery->full_check_cnt);
		} else
			battery->full_check_cnt = 0;
		break;

	case SEC_BATTERY_FULLCHARGED_FG_CURRENT:
		if ((battery->current_now > 0 && battery->current_now <
			battery->pdata->charging_current[
			battery->cable_type].full_check_current_1st) &&
			(battery->current_avg > 0 && battery->current_avg <
			(battery->charging_mode ==
			SEC_BATTERY_CHARGING_1ST ?
			battery->pdata->charging_current[
			battery->cable_type].full_check_current_1st :
			battery->pdata->charging_current[
			battery->cable_type].full_check_current_2nd))) {
				battery->full_check_cnt++;
				dev_dbg(battery->dev,
				"%s: Full Check Current (%d)\n",
				__func__,
				battery->full_check_cnt);
		} else
			battery->full_check_cnt = 0;
		break;

	case SEC_BATTERY_FULLCHARGED_TIME:
		if ((battery->charging_mode ==
			SEC_BATTERY_CHARGING_2ND ?
			(battery->charging_passed_time -
			battery->charging_fullcharged_time) :
			battery->charging_passed_time) >
			(battery->charging_mode ==
			SEC_BATTERY_CHARGING_1ST ?
			battery->pdata->charging_current[
			battery->cable_type].full_check_current_1st :
			battery->pdata->charging_current[
			battery->cable_type].full_check_current_2nd)) {
			battery->full_check_cnt++;
			dev_dbg(battery->dev,
				"%s: Full Check Time (%d)\n",
				__func__,
				battery->full_check_cnt);
		} else
			battery->full_check_cnt = 0;
		break;

	case SEC_BATTERY_FULLCHARGED_SOC:
		if (battery->capacity <=
			(battery->charging_mode ==
			SEC_BATTERY_CHARGING_1ST ?
			battery->pdata->charging_current[
			battery->cable_type].full_check_current_1st :
			battery->pdata->charging_current[
			battery->cable_type].full_check_current_2nd)) {
			battery->full_check_cnt++;
			dev_dbg(battery->dev,
				"%s: Full Check SOC (%d)\n",
				__func__,
				battery->full_check_cnt);
		} else
			battery->full_check_cnt = 0;
		break;

	case SEC_BATTERY_FULLCHARGED_CHGGPIO:
		err = gpio_request(
			battery->pdata->chg_gpio_full_check,
			"GPIO_CHG_FULL");
		if (err) {
			dev_err(battery->dev,
				"%s: Error in Request of GPIO\n", __func__);
			break;
		}
		if (!(gpio_get_value_cansleep(
			battery->pdata->chg_gpio_full_check) ^
			!battery->pdata->chg_polarity_full_check)) {
			battery->full_check_cnt++;
			dev_dbg(battery->dev,
				"%s: Full Check GPIO (%d)\n",
				__func__, battery->full_check_cnt);
		} else
			battery->full_check_cnt = 0;
		gpio_free(battery->pdata->chg_gpio_full_check);
		break;

	case SEC_BATTERY_FULLCHARGED_CHGINT:
	case SEC_BATTERY_FULLCHARGED_CHGPSY:
		psy_do_property(battery->pdata->charger_name, get,
			POWER_SUPPLY_PROP_STATUS, value);

		if (value.intval == POWER_SUPPLY_STATUS_FULL) {
			battery->full_check_cnt++;
			dev_info(battery->dev,
				"%s: Full Check Charger (%d)\n",
				__func__, battery->full_check_cnt);
		} else
			battery->full_check_cnt = 0;
		break;

	/* If these is NOT full check type or NONE full check type,
	 * it is full-charged
	 */
	case SEC_BATTERY_FULLCHARGED_NONE:
		battery->full_check_cnt = 0;
		ret = true;
		break;
	default:
		dev_err(battery->dev,
			"%s: Invalid Full Check\n", __func__);
		break;
	}

	if (battery->full_check_cnt >=
		battery->pdata->full_check_count) {
		battery->full_check_cnt = 0;
		ret = true;
	}

not_full_charged:
	return ret;
}

static void sec_bat_do_fullcharged(
				struct sec_battery_info *battery)
{
	union power_supply_propval value;

	/* To let charger/fuel gauge know the full status,
	 * set status before calling sec_bat_set_charge()
	 */
#if defined(CONFIG_BATTERY_CISD)
	struct timespec now_ts;

	if (battery->status != POWER_SUPPLY_STATUS_FULL)
		battery->cisd.data[CISD_DATA_FULL_COUNT]++;
#endif
	sec_bat_set_charging_status(battery,
			POWER_SUPPLY_STATUS_FULL);


	if (battery->charging_mode == SEC_BATTERY_CHARGING_1ST) {
		battery->charging_mode = SEC_BATTERY_CHARGING_2ND;
		battery->charging_fullcharged_time =
			battery->charging_passed_time;
		sec_bat_set_charge(battery, true);
#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY) || defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE) && defined(CONFIG_WIRELESS_CHARGER_INBATTERY_CS100)
		if (battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS ||
			battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS ||
			battery->cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS) {
			if(battery->capacity >= 100) {
				battery->cs100_status = false;
				value.intval = POWER_SUPPLY_STATUS_FULL;
				psy_do_property(battery->pdata->wireless_charger_name, set,
					POWER_SUPPLY_PROP_STATUS, value);
			} else
				battery->cs100_status = true;
		}
#endif
	} else {
		battery->charging_mode = SEC_BATTERY_CHARGING_NONE;
#if defined(CONFIG_BATTERY_CISD)
		now_ts = ktime_to_timespec(ktime_get_boottime());
		if (!battery->is_recharging) {
			battery->cisd.charging_end_time = now_ts.tv_sec;
		}
		if (battery->siop_level == 100) {
			dev_info(battery->dev, "%s: cisd - leakage EFGH start(%ld)\n", __func__, ((unsigned long)now_ts.tv_sec));
			battery->cisd.state &= ~(CISD_STATE_LEAK_E|CISD_STATE_LEAK_F|CISD_STATE_LEAK_G);
			battery->cisd.charging_end_time_2 = now_ts.tv_sec;
			battery->cisd.recharge_count_2 = 0;
		} else {
			battery->cisd.state &= ~(CISD_STATE_LEAK_E|CISD_STATE_LEAK_F|CISD_STATE_LEAK_G);
			battery->cisd.recharge_count_2 = 0;
			battery->cisd.charging_end_time_2 = 0;
		}
#endif
		battery->is_recharging = false;
		sec_bat_set_charge(battery, false);

#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY) || defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE) && !defined(CONFIG_WIRELESS_CHARGER_INBATTERY_CS100)
		if (battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS ||
			battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS ||
			battery->cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS) {
			if(battery->capacity >= 100) {
				battery->cs100_status = false;
				value.intval = POWER_SUPPLY_STATUS_FULL;
				psy_do_property(battery->pdata->wireless_charger_name, set,
					POWER_SUPPLY_PROP_STATUS, value);
			} else
				battery->cs100_status = true;
		}
#endif
#if defined(CONFIG_BATTERY_AGE_FORECAST)
		sec_bat_aging_check(battery);
#endif
		value.intval = POWER_SUPPLY_STATUS_FULL;
		psy_do_property(battery->pdata->fuelgauge_name, set,
			POWER_SUPPLY_PROP_STATUS, value);
	}

	/* platform can NOT get information of battery
	 * because wakeup time is too short to check uevent
	 * To make sure that target is wakeup if full-charged,
	 * activated wake lock in a few seconds
	 */
	if (battery->pdata->polling_type == SEC_BATTERY_MONITOR_ALARM)
		wake_lock_timeout(&battery->vbus_wake_lock, HZ * 10);
}

static bool sec_bat_fullcharged_check(
				struct sec_battery_info *battery)
{
	if ((battery->charging_mode == SEC_BATTERY_CHARGING_NONE) ||
		(battery->status == POWER_SUPPLY_STATUS_NOT_CHARGING)) {
		dev_dbg(battery->dev,
			"%s: No Need to Check Full-Charged\n", __func__);
		return true;
	}

	if (sec_bat_check_fullcharged(battery))
		sec_bat_do_fullcharged(battery);

	dev_info(battery->dev,
		"%s: Charging Mode : %s\n", __func__,
		battery->is_recharging ?
		sec_bat_charging_mode_str[SEC_BATTERY_CHARGING_RECHARGING] :
		sec_bat_charging_mode_str[battery->charging_mode]);

	return true;
}

static void sec_bat_get_temperature_info(
				struct sec_battery_info *battery)
{
	union power_supply_propval value;

	switch (battery->pdata->thermal_source) {
	case SEC_BATTERY_THERMAL_SOURCE_FG:
		psy_do_property(battery->pdata->fuelgauge_name, get,
			POWER_SUPPLY_PROP_TEMP, value);
		battery->temperature = value.intval;

		psy_do_property(battery->pdata->fuelgauge_name, get,
			POWER_SUPPLY_PROP_TEMP_AMBIENT, value);
		battery->temper_amb = value.intval;
		break;
	case SEC_BATTERY_THERMAL_SOURCE_CALLBACK:
		if (battery->pdata->get_temperature_callback) {
			battery->pdata->get_temperature_callback(
				POWER_SUPPLY_PROP_TEMP, &value);
			battery->temperature = value.intval;
			psy_do_property(battery->pdata->fuelgauge_name, set,
				POWER_SUPPLY_PROP_TEMP, value);

			battery->pdata->get_temperature_callback(
				POWER_SUPPLY_PROP_TEMP_AMBIENT, &value);
			battery->temper_amb = value.intval;
			psy_do_property(battery->pdata->fuelgauge_name, set,
				POWER_SUPPLY_PROP_TEMP_AMBIENT, value);
		}
		break;
	case SEC_BATTERY_THERMAL_SOURCE_ADC:
		sec_bat_get_temperature_by_adc(battery,
			SEC_BAT_ADC_CHANNEL_TEMP, &value);
		battery->temperature = value.intval;
		psy_do_property(battery->pdata->fuelgauge_name, set,
			POWER_SUPPLY_PROP_TEMP, value);

		sec_bat_get_temperature_by_adc(battery,
			SEC_BAT_ADC_CHANNEL_TEMP_AMBIENT, &value);
		battery->temper_amb = value.intval;
		psy_do_property(battery->pdata->fuelgauge_name, set,
			POWER_SUPPLY_PROP_TEMP_AMBIENT, value);

#if defined(CONFIG_AFC_CHARGER_MODE)
		sec_bat_get_temperature_by_adc(battery,
			   SEC_BAT_ADC_CHANNEL_CHG_TEMP, &value);
		if (battery->pre_chg_temp == 0) {
			battery->pre_chg_temp = value.intval;
			battery->chg_temp = value.intval;
		} else {
			battery->pre_chg_temp = battery->chg_temp;
			battery->chg_temp = value.intval;
		}
#endif
#if defined(CONFIG_WIRELESS_CHARGER_THM)
		sec_bat_get_temperature_by_adc(battery,
			SEC_BAT_ADC_CHANNEL_WPC_TEMP, &value);
			battery->wpc_temp = value.intval;
			pr_info("%s wpc temp = %d \n",__func__, battery->wpc_temp );
#endif
		break;
	default:
		break;
	}
}

static void sec_bat_get_battery_info(
				struct sec_battery_info *battery)
{
	union power_supply_propval value;
#if defined(CONFIG_AFC_CHARGER_MODE)
	static struct timespec old_ts;
	struct timespec c_ts;
#if defined(ANDROID_ALARM_ACTIVATED)
	c_ts = ktime_to_timespec(alarm_get_elapsed_realtime());
#else
	c_ts = ktime_to_timespec(ktime_get_boottime());
#endif
#endif

	psy_do_property(battery->pdata->fuelgauge_name, get,
		POWER_SUPPLY_PROP_VOLTAGE_NOW, value);
	battery->voltage_now = value.intval;

	value.intval = SEC_BATTERY_VOLTAGE_AVERAGE;
	psy_do_property(battery->pdata->fuelgauge_name, get,
		POWER_SUPPLY_PROP_VOLTAGE_AVG, value);
	battery->voltage_avg = value.intval;

	value.intval = SEC_BATTERY_VOLTAGE_OCV;
	psy_do_property(battery->pdata->fuelgauge_name, get,
		POWER_SUPPLY_PROP_VOLTAGE_AVG, value);
	battery->voltage_ocv = value.intval;

	value.intval = SEC_BATTERY_CURRENT_MA;
	psy_do_property(battery->pdata->fuelgauge_name, get,
		POWER_SUPPLY_PROP_CURRENT_NOW, value);
	battery->current_now = value.intval;

	value.intval = SEC_BATTERY_CURRENT_MA;
	psy_do_property(battery->pdata->fuelgauge_name, get,
		POWER_SUPPLY_PROP_CURRENT_AVG, value);
	battery->current_avg = value.intval;

#if defined(CONFIG_FUELGAUGE_MAX77833) && !defined(CONFIG_FUELGAUGE_MAX77843)
	value.intval = SEC_BATTERY_CURRENT_MA;
	psy_do_property(battery->pdata->fuelgauge_name, get,
		POWER_SUPPLY_PROP_POWER_NOW, value);
	battery->current_isys_now = value.intval;

	value.intval = SEC_BATTERY_CURRENT_MA;
	psy_do_property(battery->pdata->fuelgauge_name, get,
		POWER_SUPPLY_PROP_POWER_AVG, value);
	battery->current_isys_avg = value.intval;
#endif

	/* input current limit in charger */
	psy_do_property(battery->pdata->charger_name, get,
		POWER_SUPPLY_PROP_CURRENT_MAX, value);
	battery->current_max = value.intval;

	sec_bat_get_temperature_info(battery);



	/* To get SOC value (NOT raw SOC), need to reset value */
	value.intval = 0;
	psy_do_property(battery->pdata->fuelgauge_name, get,
			POWER_SUPPLY_PROP_CAPACITY, value);
#if defined(CONFIG_AFC_CHARGER_MODE)
	/* if the battery status was full, and SOC wasn't 100% yet,
		then ignore FG SOC, and report (previous SOC +1)% */
	if (battery->status != POWER_SUPPLY_STATUS_FULL) {
		battery->capacity = value.intval;
	} else if ((c_ts.tv_sec - old_ts.tv_sec) >= 30) {
		if (battery->capacity != 100) {
			battery->capacity++;
			pr_info("%s : forced full-charged sequence for the capacity(%d)\n",
				__func__, battery->capacity);
		}

		if (value.intval >= battery->pdata->full_condition_soc &&
			battery->voltage_now >= (battery->pdata->recharge_condition_vcell - 50)) {
			/* update capacity max */
			value.intval = battery->capacity;
			psy_do_property(battery->pdata->fuelgauge_name, set,
				POWER_SUPPLY_PROP_CHARGE_FULL, value);
		}
		old_ts = c_ts;
	}
#else
	battery->capacity = value.intval;
#endif

	if (battery->capacity > 5 && battery->ignore_siop &&
	    (battery->r_siop_level != battery->siop_level)) {
		battery->siop_level = battery->r_siop_level;
		battery->ignore_siop = false;
		if (battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS ||
			battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS ||
			battery->cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS)
			queue_delayed_work_on(0, battery->monitor_wqueue, &battery->siop_work,
					      msecs_to_jiffies(1200));
		else
			queue_delayed_work_on(0, battery->monitor_wqueue, &battery->siop_work, 0);
	}

#if !defined(CONFIG_SEC_FACTORY)
	if (battery->capacity > 5 && battery->ignore_store_mode) {
		battery->ignore_store_mode = false;
		value.intval = battery->store_mode;
		psy_do_property(battery->pdata->charger_name, set,
				POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX, value);
	}
#endif

#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY) || defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
	if ( battery->wc_status != SEC_WIRELESS_PAD_NONE &&
			battery->status == POWER_SUPPLY_STATUS_FULL &&
			battery->capacity >= 100 &&
			battery->cs100_status == true ) {
		pr_info("%s cs100 command was missed bacause of SoC, try here! \n", __func__);
		battery->cs100_status = false;
		value.intval = POWER_SUPPLY_STATUS_FULL;
		psy_do_property(battery->pdata->wireless_charger_name, set,
			POWER_SUPPLY_PROP_STATUS, value);
	}
#endif

	dev_info(battery->dev,
		"%s:Vnow(%dmV),Inow(%dmA),Isysnow(%dmA),Imax(%dmA),SOC(%d%%),Tbat(%d),Tchg(%d),Twpc(%d),is_hc_usb(%d)\n",
		__func__,
		battery->voltage_now, battery->current_now, battery->current_isys_now,
		battery->current_max, battery->capacity,
		battery->temperature, battery->chg_temp,
		battery->wpc_temp, battery->is_hc_usb);
	dev_info(battery->dev,
		"%s,Vavg(%dmV),Vocv(%dmV),Tamb(%d),"
		"Iavg(%dmA),Isysavg(%dmA),Iadc(%d)\n",
		battery->present ? "Connected" : "Disconnected",
		battery->voltage_avg, battery->voltage_ocv,
		battery->temper_amb,
		battery->current_avg, battery->current_isys_avg, battery->current_adc);
}

static void sec_bat_polling_work(struct work_struct *work)
{
	struct sec_battery_info *battery = container_of(
		work, struct sec_battery_info, polling_work.work);

	wake_lock(&battery->monitor_wake_lock);
	queue_delayed_work_on(0, battery->monitor_wqueue, &battery->monitor_work, 0);
	dev_dbg(battery->dev, "%s: Activated\n", __func__);
}

static void sec_bat_program_alarm(
				struct sec_battery_info *battery, int seconds)
{
#if defined(ANDROID_ALARM_ACTIVATED)
	ktime_t low_interval = ktime_set(seconds, 0);
	ktime_t slack = ktime_set(10, 0);
	ktime_t next;

	next = ktime_add(battery->last_poll_time, low_interval);
	alarm_start_range(&battery->polling_alarm,
		next, ktime_add(next, slack));
#else
	alarm_start(&battery->polling_alarm,
		    ktime_add(battery->last_poll_time, ktime_set(seconds, 0)));
#endif
}

static unsigned int sec_bat_get_polling_time(
	struct sec_battery_info *battery)
{
	if (battery->status ==
		POWER_SUPPLY_STATUS_FULL)
		battery->polling_time =
			battery->pdata->polling_time[
			POWER_SUPPLY_STATUS_CHARGING];
	else
		battery->polling_time =
			battery->pdata->polling_time[
			battery->status];

	battery->polling_short = true;

	switch (battery->status) {
	case POWER_SUPPLY_STATUS_CHARGING:
		if (battery->polling_in_sleep)
			battery->polling_short = false;
		break;
	case POWER_SUPPLY_STATUS_DISCHARGING:
		if (battery->polling_in_sleep && (battery->ps_enable != true))
			battery->polling_time =
				battery->pdata->polling_time[
				SEC_BATTERY_POLLING_TIME_SLEEP];
		else
			battery->polling_time =
				battery->pdata->polling_time[
				battery->status];
		battery->polling_short = false;
		break;
	case POWER_SUPPLY_STATUS_FULL:
		if (battery->polling_in_sleep) {
			if (!(battery->pdata->full_condition_type &
				SEC_BATTERY_FULL_CONDITION_NOSLEEPINFULL) &&
				battery->charging_mode ==
				SEC_BATTERY_CHARGING_NONE)
				battery->polling_time =
					battery->pdata->polling_time[
					SEC_BATTERY_POLLING_TIME_SLEEP];
			battery->polling_short = false;
		} else {
			if (battery->charging_mode ==
				SEC_BATTERY_CHARGING_NONE)
				battery->polling_short = false;
		}
		break;
	}

	if (battery->polling_short)
		return battery->pdata->polling_time[
			SEC_BATTERY_POLLING_TIME_BASIC];
	/* set polling time to 46s to reduce current noise on wc */
	else if (battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS &&
			battery->status == POWER_SUPPLY_STATUS_CHARGING)
		battery->polling_time = 46;

#ifdef CONFIG_BATTERY_SWELLING_SELF_DISCHARGING
	if (battery->pdata->sdchg_info && battery->pdata->sdchg_info->nochip)
		battery->polling_time = sdchg_get_polling_time(battery->polling_time);
#endif

	return battery->polling_time;
}

static bool sec_bat_is_short_polling(
	struct sec_battery_info *battery)
{
	/* Change the full and short monitoring sequence
	 * Originally, full monitoring was the last time of polling_count
	 * But change full monitoring to first time
	 * because temperature check is too late
	 */
	if (!battery->polling_short || battery->polling_count == 1)
		return false;
	else
		return true;
}

static void sec_bat_update_polling_count(
	struct sec_battery_info *battery)
{
	/* do NOT change polling count in sleep
	 * even though it is short polling
	 * to keep polling count along sleep/wakeup
	 */
	if (battery->polling_short && battery->polling_in_sleep)
		return;

	if (battery->polling_short &&
		((battery->polling_time /
		battery->pdata->polling_time[
		SEC_BATTERY_POLLING_TIME_BASIC])
		> battery->polling_count))
		battery->polling_count++;
	else
		battery->polling_count = 1;	/* initial value = 1 */
}

static void sec_bat_set_polling(
	struct sec_battery_info *battery)
{
	unsigned int polling_time_temp;

	dev_dbg(battery->dev, "%s: Start\n", __func__);

	polling_time_temp = sec_bat_get_polling_time(battery);

	dev_dbg(battery->dev,
		"%s: Status:%s, Sleep:%s, Charging:%s, Short Poll:%s\n",
		__func__, sec_bat_status_str[battery->status],
		battery->polling_in_sleep ? "Yes" : "No",
		(battery->charging_mode ==
		SEC_BATTERY_CHARGING_NONE) ? "No" : "Yes",
		battery->polling_short ? "Yes" : "No");
	dev_dbg(battery->dev,
		"%s: Polling time %d/%d sec.\n", __func__,
		battery->polling_short ?
		(polling_time_temp * battery->polling_count) :
		polling_time_temp, battery->polling_time);

	/* To sync with log above,
	 * change polling count after log is displayed
	 * Do NOT update polling count in initial monitor
	 */
	if (!battery->pdata->monitor_initial_count)
		sec_bat_update_polling_count(battery);
	else
		dev_dbg(battery->dev,
			"%s: Initial monitor %d times left.\n", __func__,
			battery->pdata->monitor_initial_count);

	switch (battery->pdata->polling_type) {
	case SEC_BATTERY_MONITOR_WORKQUEUE:
		if (battery->pdata->monitor_initial_count) {
			battery->pdata->monitor_initial_count--;
			schedule_delayed_work(&battery->polling_work, HZ);
		} else
			schedule_delayed_work(&battery->polling_work,
				polling_time_temp * HZ);
		break;
	case SEC_BATTERY_MONITOR_ALARM:
#if defined(ANDROID_ALARM_ACTIVATED)
		battery->last_poll_time = alarm_get_elapsed_realtime();
#else
		battery->last_poll_time = ktime_get_boottime();
#endif

		if (battery->pdata->monitor_initial_count) {
			battery->pdata->monitor_initial_count--;
			sec_bat_program_alarm(battery, 1);
		} else
			sec_bat_program_alarm(battery, polling_time_temp);
		break;
	case SEC_BATTERY_MONITOR_TIMER:
		break;
	default:
		break;
	}
	dev_dbg(battery->dev, "%s: End\n", __func__);
}
#if defined(CONFIG_BATTERY_SWELLING)
static void sec_bat_swelling_fullcharged_check(struct sec_battery_info *battery)
{
	union power_supply_propval value;

	psy_do_property(battery->pdata->charger_name, get,
		POWER_SUPPLY_PROP_STATUS, value);

	if (value.intval == POWER_SUPPLY_STATUS_FULL) {
		battery->swelling_full_check_cnt++;
		pr_info("%s: Swelling mode full-charged check (%d)\n",
			__func__, battery->swelling_full_check_cnt);
	} else
		battery->swelling_full_check_cnt = 0;

	if (battery->swelling_full_check_cnt >=
		battery->pdata->full_check_count) {
		battery->swelling_full_check_cnt = 0;
		battery->charging_mode = SEC_BATTERY_CHARGING_NONE;
		battery->is_recharging = false;
		sec_bat_set_charge(battery, false);
	}
}
#endif

#if defined(CONFIG_CALC_TIME_TO_FULL)
static void sec_bat_calc_time_to_full(struct sec_battery_info * battery)
{
	if (battery->status == POWER_SUPPLY_STATUS_CHARGING ||
		(battery->status == POWER_SUPPLY_STATUS_FULL && battery->capacity != 100)) {
		union power_supply_propval value;
		int cable_type, input, charge;
		if (battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS && sleep_mode)
			cable_type = POWER_SUPPLY_TYPE_WIRELESS;
		else
			cable_type = battery->cable_type;

		input = battery->pdata->charging_current[cable_type].input_current_limit;
		charge = battery->pdata->charging_current[cable_type].fast_charging_current;
		if ((cable_type == POWER_SUPPLY_TYPE_HV_MAINS) ||
			(cable_type == POWER_SUPPLY_TYPE_HV_ERR) ||
			(cable_type == POWER_SUPPLY_TYPE_HV_MAINS_CHG_LIMIT)) {
			value.intval = charge;
		} else if (cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS) {
			value.intval = charge - 300;
		} else if (cable_type == POWER_SUPPLY_TYPE_USB) {
			value.intval = battery->current_max - 50;
		} else if ((cable_type == POWER_SUPPLY_TYPE_WIRELESS) ||
			(cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS)) {
			value.intval = battery->current_max + 100;
		} else if (input == battery->current_max) {
			if (input == 1800) // TA cannot charge 2100
				value.intval = 1950;
			else
				value.intval = charge - 50;
		} else {
			value.intval = battery->current_max + 100;
		}
		psy_do_property(battery->pdata->fuelgauge_name, get,
				POWER_SUPPLY_PROP_TIME_TO_FULL_NOW, value);
		dev_info(battery->dev, "%s: T: %5d sec, passed time: %5ld\n",
				__func__, value.intval, battery->charging_passed_time);
		battery->timetofull = value.intval;
	} else {
		battery->timetofull = -1;
	}
}

static void sec_bat_time_to_full_work(struct work_struct *work)
{
	struct sec_battery_info *battery = container_of(work,
				struct sec_battery_info, timetofull_work.work);
	union power_supply_propval value;

	psy_do_property(battery->pdata->charger_name, get,
		POWER_SUPPLY_PROP_CURRENT_MAX, value);
	battery->current_max = value.intval;

	value.intval = SEC_BATTERY_CURRENT_MA;
	psy_do_property(battery->pdata->fuelgauge_name, get,
		POWER_SUPPLY_PROP_CURRENT_NOW, value);
	battery->current_now = value.intval;

	value.intval = SEC_BATTERY_CURRENT_MA;
	psy_do_property(battery->pdata->fuelgauge_name, get,
		POWER_SUPPLY_PROP_CURRENT_AVG, value);
	battery->current_avg = value.intval;

	sec_bat_calc_time_to_full(battery);
	battery->complete_timetofull = true;
	dev_info(battery->dev, "%s: \n",__func__);
	if (battery->voltage_now > 0)
		battery->voltage_now--;
	power_supply_changed(&battery->psy_bat);
}
#endif

#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY)
static void sec_bat_cc_cv_mode_check(struct sec_battery_info *battery)
{
	union power_supply_propval value;

	if ((battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS) &&
		(battery->status == POWER_SUPPLY_STATUS_CHARGING) &&
		(battery->capacity >= battery->pdata->wireless_cc_cv) &&
		!battery->cc_cv_mode &&
		(battery->charging_passed_time > 5)) {
		pr_info("%s changed wireless vout voltage to default value \n",__func__);
		battery->cc_cv_mode = 1;
		value.intval = 1;
		psy_do_property(battery->pdata->wireless_charger_name, set,
			POWER_SUPPLY_PROP_CHARGE_TYPE, value);
	}
}
#elif defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
static void sec_bat_cc_cv_mode_check(struct sec_battery_info *battery)
{
	union power_supply_propval value;

	pr_info("%s battery->wc_status = %d, battery->cc_cv_mode = %d \n", __func__,battery->wc_status, battery->cc_cv_mode);
	if ( (battery->wc_status != SEC_WIRELESS_PAD_NONE) &&
		!battery->cc_cv_mode &&
		(battery->charging_passed_time > 10)) {

		battery->cc_cv_mode = 1;

		if( (battery->capacity < 99 &&
			battery->status != POWER_SUPPLY_STATUS_FULL) ||
			battery->wc_status == SEC_WIRELESS_PAD_WPC_HV ) {
			pr_info("%s changed wireless vout voltage to default value, charging time = %ld \n",__func__, battery->charging_passed_time);

			value.intval = WIRELESS_VRECT_ADJ_OFF;
			psy_do_property(battery->pdata->wireless_charger_name, set,
				POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);
		}
	}

	if ( (battery->wc_status != SEC_WIRELESS_PAD_NONE) &&
		!battery->full_mode &&
		battery->capacity >= 99 &&
		battery->siop_level == 100) {
		battery->full_mode = true;

		if ( battery->wc_status != SEC_WIRELESS_PAD_WPC_HV) {
			pr_info("%s vrect headroom set ROOM 2 \n",__func__);
			value.intval = WIRELESS_VRECT_ADJ_ROOM_2;
			psy_do_property(battery->pdata->wireless_charger_name, set,
				POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);
		}
	}

	if ( (battery->wc_status != SEC_WIRELESS_PAD_NONE) &&
		battery->complete_timetofull) {
		pr_info("%s check out miss align, charging time = %ld \n",__func__, battery->charging_passed_time);

		value.intval = 1;
		psy_do_property(battery->pdata->charger_name, set,
			POWER_SUPPLY_PROP_CHARGE_NOW, value);
	}
}
#endif

static void sec_bat_siop_work(struct work_struct *work)
{
	struct sec_battery_info *battery = container_of(work,
				struct sec_battery_info, siop_work.work);
	union power_supply_propval value;

#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
	if (battery->siop_event == SIOP_EVENT_WPC_CALL_START) {
		value.intval = battery->siop_event;
		pr_info("%s : set current by siop event(%d)\n",__func__, battery->siop_event);
		psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN, value);
		if (battery->capacity >= battery->pdata->wireless_cc_cv) {
			pr_info("%s SIOP EVENT CALL START.\n", __func__);
			value.intval = WIRELESS_VOUT_CV_CALL;
			psy_do_property(battery->pdata->wireless_charger_name, set,
							POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);
		} else {
			pr_info("%s SIOP EVENT CALL START.\n", __func__);
			value.intval = WIRELESS_VOUT_CC_CALL;
			psy_do_property(battery->pdata->wireless_charger_name, set,
							POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);
		}
		wake_unlock(&battery->siop_wake_lock);
		return;
	} else if (battery->siop_event == SIOP_EVENT_WPC_CALL_END) {
		battery->siop_event = 0;
		value.intval = WIRELESS_VOUT_5V;
		psy_do_property(battery->pdata->wireless_charger_name, set,
						POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);
	}
#endif
	value.intval = battery->siop_level;
	pr_info("%s : set current by siop level(%d)\n",__func__, battery->siop_level);
	psy_do_property(battery->pdata->charger_name, set,
		POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN, value);

#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
	if( (battery->wc_status == SEC_WIRELESS_PAD_WPC || battery->wc_status == SEC_WIRELESS_PAD_PMA) &&
		(battery->capacity >= 99 ||
		battery->status == POWER_SUPPLY_STATUS_FULL)){
		value.intval = battery->siop_level;
		psy_do_property(battery->pdata->wireless_charger_name, set,
			POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN, value);
	}
#endif

#if !defined(CONFIG_SEC_FACTORY)
	if (battery->pdata->mix_temp_check)
		sec_bat_mix_temperature_check(battery);

	if ((battery->pdata->chg_temp_check || battery->pdata->wpc_temp_check))
		sec_bat_chg_temperature_check(battery);
#endif

	wake_unlock(&battery->siop_wake_lock);
}

#if defined(CONFIG_WIRELESS_FIRMWARE_UPDATE)
static void sec_bat_fw_update_work(struct sec_battery_info *battery, int mode)
{
	union power_supply_propval value;

	dev_info(battery->dev, "%s \n", __func__);

	wake_lock_timeout(&battery->vbus_wake_lock, HZ * 10);

	switch (mode) {
		case SEC_WIRELESS_RX_SDCARD_MODE:
		case SEC_WIRELESS_RX_BUILT_IN_MODE:
			value.intval = true;
			psy_do_property(battery->pdata->charger_name, set,
				POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, value);

			value.intval = mode;
			psy_do_property("p9220-charger", set,
				POWER_SUPPLY_PROP_CHARGE_POWERED_OTG_CONTROL, value);

			value.intval = false;
			psy_do_property(battery->pdata->charger_name, set,
				POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, value);

			break;
		case SEC_WIRELESS_TX_ON_MODE:
			value.intval = true;
			psy_do_property(battery->pdata->charger_name, set,
				POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, value);

			value.intval = mode;
			psy_do_property("p9220-charger", set,
				POWER_SUPPLY_PROP_CHARGE_POWERED_OTG_CONTROL, value);

			break;
		case SEC_WIRELESS_TX_OFF_MODE:
			value.intval = false;
			psy_do_property(battery->pdata->charger_name, set,
				POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, value);
			break;
		default:
			break;
	}
}

static void sec_bat_fw_init_work(struct work_struct *work)
{
	struct sec_battery_info *battery = container_of(work,
				struct sec_battery_info, fw_init_work.work);

	union power_supply_propval value;
	int otg_status;
	int wpc_det;

	dev_info(battery->dev, "%s \n", __func__);

	wpc_det = gpio_get_value(battery->pdata->wpc_det);

	pr_info("%s wpc_det = %d \n", __func__,wpc_det);

	psy_do_property(battery->pdata->charger_name, get,
		POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, value);
	otg_status = value.intval;
	pr_info("%s otg = %d \n", __func__, otg_status);

	if(!otg_status && !wpc_det) {
		pr_info("%s otg on \n", __func__);
		value.intval = true;
		psy_do_property(battery->pdata->charger_name, set,
			POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, value);
	}

	value.intval = SEC_WIRELESS_RX_INIT;
	psy_do_property(battery->pdata->wireless_charger_name, set,
		POWER_SUPPLY_PROP_CHARGE_POWERED_OTG_CONTROL, value);

	if(!otg_status && !wpc_det) {
		pr_info("%s otg off \n", __func__);
		value.intval = false;
		psy_do_property(battery->pdata->charger_name, set,
			POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, value);
	}
}
#endif

static void sec_bat_misc_event_work(struct work_struct *work)
{
	struct sec_battery_info *battery = container_of(work,
				struct sec_battery_info, misc_event_work.work);
	int xor_misc_event = battery->prev_misc_event ^ battery->misc_event;

	if ((xor_misc_event & BATT_MISC_EVENT_UNDEFINED_RANGE_TYPE) &&
		(battery->cable_type == POWER_SUPPLY_TYPE_BATTERY)) {
		if (battery->misc_event & BATT_MISC_EVENT_UNDEFINED_RANGE_TYPE) {
			sec_bat_set_charge(battery, false);
			if (battery->pdata->waterproof) {
				/* waterproof model have to disable buck */
				union power_supply_propval value;
				value.intval = 0;
				psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_CURRENT_MAX, value);
			}
		} else if (battery->prev_misc_event & BATT_MISC_EVENT_UNDEFINED_RANGE_TYPE) {
			sec_bat_set_charge(battery, false);
		}
	}

	pr_info("%s: change misc event(0x%x --> 0x%x)\n",
		__func__, battery->prev_misc_event, battery->misc_event);
	battery->prev_misc_event = battery->misc_event;
	wake_unlock(&battery->misc_event_wake_lock);

	wake_lock(&battery->monitor_wake_lock);
	queue_delayed_work_on(0, battery->monitor_wqueue, &battery->monitor_work, 0);
}

static void sec_bat_monitor_work(
				struct work_struct *work)
{
	struct sec_battery_info *battery =
		container_of(work, struct sec_battery_info,
		monitor_work.work);
	static struct timespec old_ts;
	struct timespec c_ts;
#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY)	
	union power_supply_propval value;
#endif
	dev_dbg(battery->dev, "%s: Start\n", __func__);
#if defined(ANDROID_ALARM_ACTIVATED)
	c_ts = ktime_to_timespec(alarm_get_elapsed_realtime());
#else
	c_ts = ktime_to_timespec(ktime_get_boottime());
#endif

	/* monitor once after wakeup */
	if (battery->polling_in_sleep) {
		battery->polling_in_sleep = false;
		if ((battery->status == POWER_SUPPLY_STATUS_DISCHARGING) &&
			(battery->ps_enable != true)) {
			if ((unsigned long)(c_ts.tv_sec - old_ts.tv_sec) < 10 * 60) {
					union power_supply_propval value;

					psy_do_property(battery->pdata->fuelgauge_name, get,
						POWER_SUPPLY_PROP_VOLTAGE_NOW, value);
					battery->voltage_now = value.intval;
					sec_bat_get_temperature_info(battery);
#if defined(CONFIG_BATTERY_CISD)
					sec_bat_cisd_check(battery);
#endif
					power_supply_changed(&battery->psy_bat);
					pr_info("Skip monitor work(%ld, Vnow:%dmV, Tbat:%d)\n",
						c_ts.tv_sec - old_ts.tv_sec, battery->voltage_now, battery->temperature);

#ifdef CONFIG_BATTERY_SWELLING_SELF_DISCHARGING
					if (battery->pdata->sdchg_info &&
							battery->pdata->sdchg_info->nochip)
						battery->pdata->sdchg_info->nochip->sdchg_monitor(battery, c_ts.tv_sec, true);
#endif
				goto skip_monitor;
			}
		}
	}
	/* update last monitor time */
	old_ts = c_ts;

	sec_bat_get_battery_info(battery);
#if defined(CONFIG_BATTERY_CISD)
	sec_bat_cisd_check(battery);
#endif
#ifdef CONFIG_BATTERY_SWELLING_SELF_DISCHARGING
	if (battery->pdata->sdchg_info && battery->pdata->sdchg_info->nochip)
		battery->pdata->sdchg_info->nochip->sdchg_monitor(battery, c_ts.tv_sec, false);
#endif

#if defined(CONFIG_CALC_TIME_TO_FULL)
	/* time to full check */
	sec_bat_calc_time_to_full(battery);
#endif

#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY) || defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
	if(battery->wc_status != SEC_WIRELESS_PAD_NONE)
		sec_bat_cc_cv_mode_check(battery);
#endif

#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY)
	if(battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS) {
		psy_do_property(battery->pdata->charger_name, get,
			POWER_SUPPLY_PROP_CURRENT_NOW, value);
		pr_info("%s: set_input_current: current_avg(%d), imax(%d)\n",
			__func__, value.intval, battery->current_max);
		value.intval = battery->current_max;
		psy_do_property(battery->pdata->charger_name, set,
			POWER_SUPPLY_PROP_CURRENT_MAX, value);
	}
#endif

	/* 0. test mode */
	if (battery->test_mode) {
		dev_err(battery->dev, "%s: Test Mode\n", __func__);
		sec_bat_do_test_function(battery);
		if (battery->test_mode != 0)
			goto continue_monitor;
	}

	/* 1. battery check */
	if (!sec_bat_battery_cable_check(battery))
		goto continue_monitor;

	/* 2. voltage check */
	if (!sec_bat_voltage_check(battery))
		goto continue_monitor;

	/* monitor short routine in initial monitor */
	if (battery->pdata->monitor_initial_count ||
		sec_bat_is_short_polling(battery))
		goto continue_monitor;

	/* 3. time management */
	if (!sec_bat_time_management(battery))
		goto continue_monitor;

	if (battery->pdata->camera_temp_check)
		sec_bat_camera_temp_check(battery);

	/* 4. temperature check */
	if (!sec_bat_temperature_check(battery))
		goto continue_monitor;

#if defined(CONFIG_BATTERY_SWELLING_SELF_DISCHARGING)
	if (battery->pdata->sdchg_info)
		battery->pdata->sdchg_info->sdchg_discharging_check(battery);
#endif

#if defined(CONFIG_BATTERY_SWELLING)
	sec_bat_swelling_check(battery, battery->temperature);

	if (battery->swelling_mode && !battery->charging_block)
		sec_bat_swelling_fullcharged_check(battery);
	else
		sec_bat_fullcharged_check(battery);
#else
	/* 5. full charging check */
	sec_bat_fullcharged_check(battery);
#endif /* CONFIG_BATTERY_SWELLING */

	/* 6. additional check */
	if (battery->pdata->monitor_additional_check)
		battery->pdata->monitor_additional_check();

#if !defined(CONFIG_SEC_FACTORY)
	/* 7. battery temperature check */
	if (battery->pdata->mix_temp_check)
		sec_bat_mix_temperature_check(battery);

	/* 8. charger temperature check */
	if (battery->pdata->chg_temp_check || battery->pdata->wpc_temp_check)
		sec_bat_chg_temperature_check(battery);
#endif

continue_monitor:
#if defined(CONFIG_AFC_CHARGER_MODE)
	dev_info(battery->dev,
		 "%s: Status(%s), mode(%s), Health(%s), Cable(%d,%d), level(%d%%), HV(%s), Cycle(%d)\n",
		 __func__,
		 sec_bat_status_str[battery->status],
		 sec_bat_charging_mode_str[battery->charging_mode],
		 sec_bat_health_str[battery->health],
		 battery->cable_type, battery->muic_cable_type, battery->siop_level,
		 battery->hv_chg_name, battery->batt_cycle);
		if((battery->status == POWER_SUPPLY_STATUS_DISCHARGING) && (battery->capacity <=50)) {
			if(sec_calibrate_battery==false){
				sec_calibrate_battery=true;
				dev_info(battery->dev,"MARTITOS: SETTED SEC_CALIBRATE_BATTERY TO TRUE\n");
			}
		}
		if((battery->status == POWER_SUPPLY_STATUS_FULL) && (battery->cable_type==3) && (sec_calibrate_battery==true)){
		 sec_calibrate_battery=false;
		 sec_bat_set_charging_status(battery, POWER_SUPPLY_STATUS_DISCHARGING);
		 sec_bat_set_charge(battery, false);
		 battery->cable_type = POWER_SUPPLY_TYPE_BATTERY;
		 dev_info(battery->dev,"MARTITOS: SETTED BATTERY AND SLEEP\n");
		 msleep(5000);
		 battery->cable_type = 3;
		 sec_bat_set_charging_status(battery, POWER_SUPPLY_STATUS_CHARGING);
		 sec_bat_set_charge(battery, true);
		 dev_info(battery->dev,"MARTITOS: SETTED CABLE 3 TYPE\n");
		 }

#else
	dev_info(battery->dev,
		 "%s: Status(%s), mode(%s), Health(%s), Cable(%d,%d), level(%d%%), Cycle(%d)\n",
		 __func__,
		 sec_bat_status_str[battery->status],
		 sec_bat_charging_mode_str[battery->charging_mode],
		 sec_bat_health_str[battery->health],
		 battery->cable_type, battery->muic_cable_type, battery->siop_level, battery->batt_cycle);
#endif
#if defined(CONFIG_SAMSUNG_BATTERY_ENG_TEST)
	dev_info(battery->dev,
			"%s: battery->stability_test(%d), battery->eng_not_full_status(%d)\n",
			__func__, battery->stability_test, battery->eng_not_full_status);
#endif
	if (battery->store_mode && !lpcharge && (battery->cable_type != POWER_SUPPLY_TYPE_BATTERY)) {

		dev_info(battery->dev,
			"%s: @battery->capacity = (%d), battery->status= (%d), battery->store_mode=(%d)\n",
			__func__, battery->capacity, battery->status, battery->store_mode);

		if ((battery->capacity >= STORE_MODE_CHARGING_MAX) && (battery->status == POWER_SUPPLY_STATUS_CHARGING)) {
			sec_bat_set_charging_status(battery,
					POWER_SUPPLY_STATUS_DISCHARGING);
			sec_bat_set_charge(battery, false);
		}
		if ((battery->capacity <= STORE_MODE_CHARGING_MIN) && (battery->status == POWER_SUPPLY_STATUS_DISCHARGING)) {
			sec_bat_set_charging_status(battery,
					POWER_SUPPLY_STATUS_CHARGING);
			sec_bat_set_charge(battery, true);
		}
	}
	power_supply_changed(&battery->psy_bat);

skip_monitor:
	sec_bat_set_polling(battery);

	if (battery->capacity <= 0 || battery->health_change)
		wake_lock_timeout(&battery->monitor_wake_lock, HZ * 5);
	else
		wake_unlock(&battery->monitor_wake_lock);

	dev_dbg(battery->dev, "%s: End\n", __func__);

	return;
}

#if defined(ANDROID_ALARM_ACTIVATED)
static void sec_bat_alarm(struct alarm *alarm)
#else
static enum alarmtimer_restart sec_bat_alarm(
	struct alarm *alarm, ktime_t now)

#endif
{
	struct sec_battery_info *battery = container_of(alarm,
				struct sec_battery_info, polling_alarm);

	dev_dbg(battery->dev,
			"%s\n", __func__);

	/* In wake up, monitor work will be queued in complete function
	 * To avoid duplicated queuing of monitor work,
	 * do NOT queue monitor work in wake up by polling alarm
	 */
	if (!battery->polling_in_sleep) {
		wake_lock(&battery->monitor_wake_lock);
		queue_delayed_work_on(0, battery->monitor_wqueue, &battery->monitor_work, 0);
		dev_dbg(battery->dev, "%s: Activated\n", __func__);
	}
#if !defined(ANDROID_ALARM_ACTIVATED)
	return ALARMTIMER_NORESTART;
#endif
}


static void sec_bat_cable_work(struct work_struct *work)
{
	struct sec_battery_info *battery = container_of(work,
				struct sec_battery_info, cable_work.work);
	union power_supply_propval val;
	int wl_cur, wr_cur, current_cable_type;
	bool keep_charging_state = false;

	dev_info(battery->dev, "%s: Start\n", __func__);

	wl_cur = battery->pdata->charging_current[
		POWER_SUPPLY_TYPE_WIRELESS].input_current_limit;
	wr_cur = battery->pdata->charging_current[
		battery->wire_status].input_current_limit;
#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
	if (battery->pdata->wireless_charger_name != NULL
		&& !strcmp(battery->pdata->wireless_charger_name,"max77900-charger")) {
		if (battery->wc_status && battery->wc_enable &&
				(wl_cur > wr_cur))
			current_cable_type = POWER_SUPPLY_TYPE_WIRELESS;
		else
			current_cable_type = battery->wire_status;
	} else if (battery->wc_status && battery->wc_enable) {
		if(battery->wc_status == SEC_WIRELESS_PAD_WPC)
			current_cable_type = POWER_SUPPLY_TYPE_WIRELESS;
		else if(battery->wc_status == SEC_WIRELESS_PAD_WPC_HV)
			current_cable_type = POWER_SUPPLY_TYPE_HV_WIRELESS;
		else
			current_cable_type = POWER_SUPPLY_TYPE_PMA_WIRELESS;
	}
	else
		current_cable_type = battery->wire_status;
#else
	if (battery->wc_status && battery->wc_enable &&
			(wl_cur > wr_cur))
		current_cable_type = POWER_SUPPLY_TYPE_WIRELESS;
	else
		current_cable_type = battery->wire_status;
#endif	

	if ((current_cable_type == battery->cable_type) && !battery->slate_mode) {
		dev_dbg(battery->dev,
				"%s: Cable is NOT Changed(%d)\n",
				__func__, battery->cable_type);
		/* Do NOT activate cable work for NOT changed */
		goto end_of_cable_work;
	}

#if defined(CONFIG_BATTERY_SWELLING)
	if (current_cable_type == POWER_SUPPLY_TYPE_BATTERY ||
		battery->cable_type == POWER_SUPPLY_TYPE_BATTERY) {
		battery->swelling_mode = false;
		/* restore 4.4V float voltage */
		val.intval = battery->pdata->swelling_normal_float_voltage;
		psy_do_property(battery->pdata->charger_name, set,
			POWER_SUPPLY_PROP_VOLTAGE_MAX, val);
		pr_info("%s: float voltage = %d\n", __func__, val.intval);
	} else {
		pr_info("%s: skip  float_voltage setting, swelling_mode(%d)\n",
			__func__, battery->swelling_mode);
	}
#endif

#if defined(CONFIG_CALC_TIME_TO_FULL)
	if ((battery->cable_type != POWER_SUPPLY_TYPE_HV_MAINS_CHG_LIMIT) &&
		(current_cable_type != POWER_SUPPLY_TYPE_HV_MAINS_CHG_LIMIT))
		battery->complete_timetofull = false;
#endif
	if (battery->charging_block &&
		((battery->cable_type != POWER_SUPPLY_TYPE_BATTERY && current_cable_type == POWER_SUPPLY_TYPE_HV_MAINS_CHG_LIMIT) ||
		(current_cable_type != POWER_SUPPLY_TYPE_BATTERY && battery->cable_type == POWER_SUPPLY_TYPE_HV_MAINS_CHG_LIMIT) ||
		(current_cable_type != POWER_SUPPLY_TYPE_BATTERY && battery->cable_type == POWER_SUPPLY_TYPE_HV_PREPARE_MAINS) ||
		(current_cable_type == POWER_SUPPLY_TYPE_HV_PREPARE_MAINS) || battery->swelling_mode)) {
		keep_charging_state = true;
		pr_info("%s: keep charging state (prev cable type:%d, now cable type:%d, swelling_mode:%d)\n",
			__func__, battery->cable_type, current_cable_type, battery->swelling_mode);
	}

	battery->cable_type = current_cable_type;

	if (battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS ||
		battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS ||
		battery->cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS)
		power_supply_changed(&battery->psy_bat);

	if (battery->pdata->check_cable_result_callback)
		battery->pdata->check_cable_result_callback(
			battery->cable_type);
	/* platform can NOT get information of cable connection
	 * because wakeup time is too short to check uevent
	 * To make sure that target is wakeup
	 * if cable is connected and disconnected,
	 * activated wake lock in a few seconds
	 */
	wake_lock_timeout(&battery->vbus_wake_lock, HZ * 10);

	if (battery->cable_type == POWER_SUPPLY_TYPE_BATTERY ||
		((battery->pdata->cable_check_type &
		SEC_BATTERY_CABLE_CHECK_NOINCOMPATIBLECHARGE) &&
		battery->cable_type == POWER_SUPPLY_TYPE_UNKNOWN)) {
		battery->charging_mode = SEC_BATTERY_CHARGING_NONE;
		battery->is_recharging = false;
#if defined(CONFIG_BATTERY_CISD)
		battery->cisd.charging_end_time = 0;
		battery->cisd.recharge_count = 0;
		battery->cisd.charging_end_time_2 = 0;
		battery->cisd.recharge_count_2 = 0;
		battery->cisd.ab_vbat_check_count = 0;
		battery->cisd.state &= ~CISD_STATE_OVER_VOLTAGE;
#endif
		sec_bat_set_charging_status(battery,
				POWER_SUPPLY_STATUS_DISCHARGING);
		battery->health = POWER_SUPPLY_HEALTH_GOOD;
#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
		wpc_temp_mode = false;
#endif
#if defined(CONFIG_CALC_TIME_TO_FULL)
		cancel_delayed_work(&battery->timetofull_work);
#endif
		if (sec_bat_set_charge(battery, false))
			goto end_of_cable_work;
	} else if (battery->slate_mode == true) {
		sec_bat_set_charging_status(battery,
				POWER_SUPPLY_STATUS_DISCHARGING);
		battery->cable_type = POWER_SUPPLY_TYPE_BATTERY;

		val.intval = 0;
		psy_do_property(battery->pdata->charger_name, set,
			POWER_SUPPLY_PROP_CURRENT_NOW, val);

		dev_info(battery->dev,
			"%s:slate mode on\n",__func__);
	} else {
#if defined(CONFIG_EN_OOPS)
		val.intval = battery->cable_type;
		psy_do_property(battery->pdata->fuelgauge_name, set,
				POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN, val);
#endif
#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
		if (battery->cable_type != POWER_SUPPLY_TYPE_WIRELESS)
			battery->siop_event = SIOP_EVENT_IDLE;
#endif
		/* Do NOT display the charging icon when OTG or HMT_CONNECTED is enabled */
		if (battery->cable_type == POWER_SUPPLY_TYPE_OTG ||
			battery->cable_type == POWER_SUPPLY_TYPE_POWER_SHARING) {
			battery->charging_mode = SEC_BATTERY_CHARGING_NONE;
			sec_bat_set_charging_status(battery,
					POWER_SUPPLY_STATUS_DISCHARGING);
		} else {
			if (!keep_charging_state) {
				if (battery->pdata->full_check_type !=
					SEC_BATTERY_FULLCHARGED_NONE)
					battery->charging_mode =
						SEC_BATTERY_CHARGING_1ST;
				else
					battery->charging_mode =
						SEC_BATTERY_CHARGING_2ND;
			}

			if (((battery->cable_type == POWER_SUPPLY_TYPE_HV_MAINS) ||
				(battery->cable_type == POWER_SUPPLY_TYPE_HV_PREPARE_MAINS) ||
				(battery->cable_type == POWER_SUPPLY_TYPE_HV_ERR)) &&
				(battery->status == POWER_SUPPLY_STATUS_FULL))
				sec_bat_set_charging_status(battery,
						POWER_SUPPLY_STATUS_FULL);
			else if (!keep_charging_state)
				sec_bat_set_charging_status(battery,
						POWER_SUPPLY_STATUS_CHARGING);

			if (!keep_charging_state) {
				if (sec_bat_set_charge(battery, true))
					goto end_of_cable_work;
			}
		}

#if defined(CONFIG_CALC_TIME_TO_FULL)
		queue_delayed_work_on(0, battery->monitor_wqueue, &battery->timetofull_work,
					msecs_to_jiffies(7000));
#endif
#if defined(ANDROID_ALARM_ACTIVATED)
		/* No need for wakelock in Alarm */
		if (battery->pdata->polling_type != SEC_BATTERY_MONITOR_ALARM)
			wake_lock(&battery->vbus_wake_lock);
#endif

#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY)
		if (battery->pdata->wpc_delayed_current_en &&
			(battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS) &&
			(battery->status == POWER_SUPPLY_STATUS_CHARGING)) {
			battery->wc_current = battery->pdata->wpc_delayed_current;
			pr_info("%s changed wireless current to %dmA \n",__func__, battery->wc_current);
			val.intval = battery->wc_current;
			psy_do_property(battery->pdata->charger_name, set,
				POWER_SUPPLY_PROP_CURRENT_MAX, val);
		}
#endif
	}
	/* polling time should be reset when cable is changed
	 * polling_in_sleep should be reset also
	 * before polling time is re-calculated
	 * to prevent from counting 1 for events
	 * right after cable is connected
	 */
	battery->polling_in_sleep = false;
	sec_bat_get_polling_time(battery);

	dev_info(battery->dev,
		"%s: Status:%s, Sleep:%s, Charging:%s, Short Poll:%s\n",
		__func__, sec_bat_status_str[battery->status],
		battery->polling_in_sleep ? "Yes" : "No",
		(battery->charging_mode ==
		SEC_BATTERY_CHARGING_NONE) ? "No" : "Yes",
		battery->polling_short ? "Yes" : "No");
	dev_info(battery->dev,
		"%s: Polling time is reset to %d sec.\n", __func__,
		battery->polling_time);

	battery->polling_count = 1;	/* initial value = 1 */

	wake_lock(&battery->monitor_wake_lock);
	queue_delayed_work_on(0, battery->monitor_wqueue, &battery->monitor_work, 0);
end_of_cable_work:
	wake_unlock(&battery->cable_wake_lock);
	dev_dbg(battery->dev, "%s: End\n", __func__);
}

ssize_t sec_bat_show_attrs(struct device *dev,
				  struct device_attribute *attr, char *buf)
{
	struct power_supply *psy = dev_get_drvdata(dev);
	struct sec_battery_info *battery =
		container_of(psy, struct sec_battery_info, psy_bat);
	const ptrdiff_t offset = attr - sec_battery_attrs;
	union power_supply_propval value;
	int i = 0;
	int ret = 0;

	switch (offset) {
	case BATT_RESET_SOC:
		break;
	case BATT_READ_RAW_SOC:
		{
			union power_supply_propval value;

			value.intval =
				SEC_FUELGAUGE_CAPACITY_TYPE_RAW;
			psy_do_property(battery->pdata->fuelgauge_name, get,
				POWER_SUPPLY_PROP_CAPACITY, value);

			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				value.intval);
		}
		break;
	case BATT_READ_ADJ_SOC:
		break;
	case BATT_TYPE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%s\n",
			battery->pdata->vendor);
		break;
	case BATT_VFOCV:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->voltage_ocv);
		break;
	case BATT_VOL_ADC:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->inbat_adc);
		break;
	case BATT_VOL_ADC_CAL:
		break;
	case BATT_VOL_AVER:
		break;
	case BATT_VOL_ADC_AVER:
		break;

	case BATT_CURRENT_UA_NOW:
		{
			union power_supply_propval value;

			value.intval = SEC_BATTERY_CURRENT_UA;
			psy_do_property(battery->pdata->fuelgauge_name, get,
				POWER_SUPPLY_PROP_CURRENT_NOW, value);

			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				value.intval);
		}
		break;
	case BATT_CURRENT_UA_AVG:
		{
			union power_supply_propval value;

			value.intval = SEC_BATTERY_CURRENT_UA;
			psy_do_property(battery->pdata->fuelgauge_name, get,
				POWER_SUPPLY_PROP_CURRENT_AVG, value);

			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				value.intval);
		}
		break;

	case BATT_FILTER_CFG:
		{
			union power_supply_propval value;

			psy_do_property(battery->pdata->fuelgauge_name, get,
				POWER_SUPPLY_PROP_FILTER_CFG, value);

			i += scnprintf(buf + i, PAGE_SIZE - i, "%x\n",
				value.intval);
		}
		break;
	case BATT_TEMP:
		switch (battery->pdata->thermal_source) {
		case SEC_BATTERY_THERMAL_SOURCE_FG:
			psy_do_property(battery->pdata->fuelgauge_name, get,
				POWER_SUPPLY_PROP_TEMP, value);
			break;
		case SEC_BATTERY_THERMAL_SOURCE_CALLBACK:
			if (battery->pdata->get_temperature_callback) {
			battery->pdata->get_temperature_callback(
				POWER_SUPPLY_PROP_TEMP, &value);
			}
			break;
		case SEC_BATTERY_THERMAL_SOURCE_ADC:
			sec_bat_get_temperature_by_adc(battery,
				SEC_BAT_ADC_CHANNEL_TEMP, &value);
			break;
		default:
			break;
		}
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			value.intval);
		break;
	case BATT_TEMP_ADC:
		/*
			If F/G is used for reading the temperature and
			compensation table is used,
			the raw value that isn't compensated can be read by
			POWER_SUPPLY_PROP_TEMP_AMBIENT
		 */
		switch (battery->pdata->thermal_source) {
		case SEC_BATTERY_THERMAL_SOURCE_FG:
			psy_do_property(battery->pdata->fuelgauge_name, get,
				POWER_SUPPLY_PROP_TEMP_AMBIENT, value);
			battery->temp_adc = value.intval;
			break;
		default:
			break;
		}
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->temp_adc);
		break;
	case BATT_TEMP_AVER:
		break;
	case BATT_TEMP_ADC_AVER:
		break;
	case CHG_TEMP:
#if defined(CONFIG_AFC_CHARGER_MODE)
		sec_bat_get_temperature_by_adc(battery,
			       SEC_BAT_ADC_CHANNEL_CHG_TEMP, &value);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				value.intval);
#else
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				0);
#endif
		break;
	case CHG_TEMP_ADC:
#if defined(CONFIG_AFC_CHARGER_MODE)
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				battery->chg_temp_adc);
#else
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				0);
#endif
		break;
	case BATT_VF_ADC:
		break;
	case BATT_SLATE_MODE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->slate_mode);
		break;

	case BATT_LP_CHARGING:
		if (lpcharge) {
			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				       lpcharge ? 1 : 0);
		}
		break;
	case SIOP_ACTIVATED:
		break;
	case SIOP_LEVEL:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->siop_level);
		break;
	case SIOP_EVENT:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->siop_event);
		break;
	case BATT_CHARGING_SOURCE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->cable_type);
		break;
	case FG_REG_DUMP:
		break;
	case FG_RESET_CAP:
		break;
	case FG_CAPACITY:
	{
		union power_supply_propval value;

		value.intval =
			SEC_BATTERY_CAPACITY_DESIGNED;
		psy_do_property(battery->pdata->fuelgauge_name, get,
			POWER_SUPPLY_PROP_ENERGY_NOW, value);

		i += scnprintf(buf + i, PAGE_SIZE - i, "0x%04x ",
			value.intval);

		value.intval =
			SEC_BATTERY_CAPACITY_ABSOLUTE;
		psy_do_property(battery->pdata->fuelgauge_name, get,
			POWER_SUPPLY_PROP_ENERGY_NOW, value);

		i += scnprintf(buf + i, PAGE_SIZE - i, "0x%04x ",
			value.intval);

		value.intval =
			SEC_BATTERY_CAPACITY_TEMPERARY;
		psy_do_property(battery->pdata->fuelgauge_name, get,
			POWER_SUPPLY_PROP_ENERGY_NOW, value);

		i += scnprintf(buf + i, PAGE_SIZE - i, "0x%04x ",
			value.intval);

		value.intval =
			SEC_BATTERY_CAPACITY_CURRENT;
		psy_do_property(battery->pdata->fuelgauge_name, get,
			POWER_SUPPLY_PROP_ENERGY_NOW, value);

		i += scnprintf(buf + i, PAGE_SIZE - i, "0x%04x\n",
			value.intval);
	}
		break;
	case FG_ASOC:
#if defined(CONFIG_INBATTERY)
		psy_do_property(battery->pdata->fuelgauge_name, get,
				POWER_SUPPLY_PROP_ENERGY_FULL, value);
#else
		value.intval = -1;
#endif
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			       value.intval);
		break;
	case AUTH:
		break;
	case CHG_CURRENT_ADC:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->current_adc);
		break;
	case WC_ADC:
		break;
	case WC_STATUS:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			((battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS) ||
			(battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS) ||
			(battery->cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS)) ? 1: 0);
		break;
	case WC_ENABLE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->wc_enable);
		break;
	case HV_CHARGER_STATUS:
		{
			int check_val = 0;
			if (battery->cable_type == POWER_SUPPLY_TYPE_HV_MAINS ||
				battery->cable_type == POWER_SUPPLY_TYPE_HV_ERR ||
				battery->cable_type == POWER_SUPPLY_TYPE_HV_MAINS_CHG_LIMIT)
				check_val = 1;
			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", check_val);
		}
		break;
	case HV_CHARGER_SET:
		break;
	case HV_CHARGER_SUPPORT:
#if defined(CONFIG_AFC_CHARGER_MODE)
		i += scnprintf(buf + i, PAGE_SIZE - i, "1\n");
#else
		i += scnprintf(buf + i, PAGE_SIZE - i, "0\n");
#endif
		break;
	case HV_WC_CHARGER_SUPPORT:
#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
		i += scnprintf(buf + i, PAGE_SIZE - i, "1\n");
#else
		i += scnprintf(buf + i, PAGE_SIZE - i, "0\n");
#endif
		break;
	case FACTORY_MODE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->factory_mode);
		break;
	case STORE_MODE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->store_mode);
		break;
	case UPDATE:
		break;
	case TEST_MODE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->test_mode);
		break;

	case BATT_EVENT_CALL:
	case BATT_EVENT_2G_CALL:
	case BATT_EVENT_TALK_GSM:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_2G_CALL) ? 1 : 0);
		break;
	case BATT_EVENT_3G_CALL:
	case BATT_EVENT_TALK_WCDMA:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_3G_CALL) ? 1 : 0);
		break;
	case BATT_EVENT_MUSIC:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_MUSIC) ? 1 : 0);
		break;
	case BATT_EVENT_VIDEO:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_VIDEO) ? 1 : 0);
		break;
	case BATT_EVENT_BROWSER:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_BROWSER) ? 1 : 0);
		break;
	case BATT_EVENT_HOTSPOT:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_HOTSPOT) ? 1 : 0);
		break;
	case BATT_EVENT_CAMERA:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_CAMERA) ? 1 : 0);
		break;
	case BATT_EVENT_CAMCORDER:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_CAMCORDER) ? 1 : 0);
		break;
	case BATT_EVENT_DATA_CALL:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_DATA_CALL) ? 1 : 0);
		break;
	case BATT_EVENT_WIFI:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_WIFI) ? 1 : 0);
		break;
	case BATT_EVENT_WIBRO:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_WIBRO) ? 1 : 0);
		break;
	case BATT_EVENT_LTE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_LTE) ? 1 : 0);
		break;
	case BATT_EVENT_LCD:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_LCD) ? 1 : 0);
		break;
	case BATT_EVENT_GPS:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->event & EVENT_GPS) ? 1 : 0);
		break;
	case BATT_EVENT:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->event);
		break;
	case BATT_TEMP_TABLE:
		i += scnprintf(buf + i, PAGE_SIZE - i,
			"%d %d %d %d %d %d %d %d %d %d %d %d\n",
			battery->pdata->temp_high_threshold_event,
			battery->pdata->temp_high_recovery_event,
			battery->pdata->temp_low_threshold_event,
			battery->pdata->temp_low_recovery_event,
			battery->pdata->temp_high_threshold_normal,
			battery->pdata->temp_high_recovery_normal,
			battery->pdata->temp_low_threshold_normal,
			battery->pdata->temp_low_recovery_normal,
			battery->pdata->temp_high_threshold_lpm,
			battery->pdata->temp_high_recovery_lpm,
			battery->pdata->temp_low_threshold_lpm,
			battery->pdata->temp_low_recovery_lpm);
		break;
	case BATT_HIGH_CURRENT_USB:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->is_hc_usb);
		break;
#if defined(CONFIG_SAMSUNG_BATTERY_ENG_TEST)
	case BATT_TEST_CHARGE_CURRENT:
		{
			union power_supply_propval value;

			psy_do_property(battery->pdata->charger_name, get,
				POWER_SUPPLY_PROP_CURRENT_NOW, value);
			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
					value.intval);
		}
		break;
#endif
	case BATT_STABILITY_TEST:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			battery->stability_test);
		break;
#if !defined(CONFIG_DISABLE_SAVE_CAPACITY_MAX)
	case BATT_CAPACITY_MAX:
		psy_do_property(battery->pdata->fuelgauge_name, get,
				POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN, value);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", value.intval);
		break;
#endif
	case BATT_INBAT_VOLTAGE:
	{
		int ret;
		ret = sec_bat_get_inbat_vol_by_adc(battery);
		dev_info(battery->dev, "in-battery voltage(%d)\n", ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
	}
		break;
#if defined(CONFIG_BATTERY_SWELLING_SELF_DISCHARGING)
	case BATT_DISCHARGING_CHECK:
	{
		int ret;
		if (battery->pdata->sdchg_info) {
			ret = battery->pdata->sdchg_info->sdchg_force_check(battery);
			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				       ret);
		} else {
			pr_info("[SDCHG][%s] discharging info structure is null!!\n", __func__);
		}
	}
		break;
	case BATT_DISCHARGING_CHECK_ADC:
		if (battery->pdata->sdchg_info) {
			battery->pdata->sdchg_info->sdchg_adc_check(battery);
			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				       battery->self_discharging_adc);
		} else {
			pr_info("[SDCHG][%s] discharging info structure is null!!\n", __func__);
		}
		break;
	case BATT_DISCHARGING_NTC:
		if (battery->pdata->sdchg_info) {
			battery->pdata->sdchg_info->sdchg_ntc_check(battery);
			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				       battery->discharging_ntc);
		} else {
			pr_info("[SDCHG][%s] discharging info structure is null!!\n", __func__);
		}	
		break;
	case BATT_DISCHARGING_NTC_ADC:
		if (battery->pdata->sdchg_info) {
			battery->pdata->sdchg_info->sdchg_ntc_check(battery);
			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				       battery->discharging_ntc_adc);
		} else {
			pr_info("[SDCHG][%s] discharging info structure is null!!\n", __func__);
		}
		break;
	case BATT_SELF_DISCHARGING_CONTROL:
		break;
#endif
#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY) || defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
	case BATT_INBAT_WIRELESS_CS100:
		psy_do_property(battery->pdata->wireless_charger_name, get,
			POWER_SUPPLY_PROP_STATUS, value);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", value.intval);
		break;
#endif
	case HMT_TA_CONNECTED:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->cable_type == POWER_SUPPLY_TYPE_HMT_CONNECTED) ? 1 : 0);
		break;
	case HMT_TA_CHARGE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(battery->cable_type == POWER_SUPPLY_TYPE_HMT_CHARGE) ? 1 : 0);
		break;
	case FG_CYCLE:
		value.intval = SEC_BATTERY_CAPACITY_CYCLE;
		psy_do_property(battery->pdata->fuelgauge_name, get,
			POWER_SUPPLY_PROP_ENERGY_NOW, value);
		value.intval = value.intval / 100;
		dev_info(battery->dev, "fg cycle(%d)\n", value.intval);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", value.intval);
		break;
	case FG_FULL_VOLTAGE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", battery->pdata->chg_float_voltage);
		break;
	case FG_FULLCAPNOM:
		value.intval =
			SEC_BATTERY_CAPACITY_AGEDCELL;
		psy_do_property(battery->pdata->fuelgauge_name, get,
			POWER_SUPPLY_PROP_ENERGY_NOW, value);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", value.intval);
		break;
	case BATTERY_CYCLE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", battery->batt_cycle);
		break;
#if defined(CONFIG_WIRELESS_CHARGER_THM)
	case BATT_WPC_TEMP:
		sec_bat_get_temperature_by_adc(battery,
			SEC_BAT_ADC_CHANNEL_WPC_TEMP, &value);
		pr_info("%s wpc temp = %d \n",__func__, value.intval);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
					   value.intval);

		break;
	case BATT_WPC_TEMP_ADC:
		pr_info("%s wpc_temp_adc = %d \n",__func__, battery->wpc_temp_adc);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
					   battery->wpc_temp_adc);
		break;
#endif
#if defined(CONFIG_WIRELESS_FIRMWARE_UPDATE)
	case BATT_WIRELESS_FIRMWARE_UPDATE:
		value.intval = SEC_WIRELESS_OTP_FIRM_VERIFY;
		psy_do_property(battery->pdata->wireless_charger_name, get,
			POWER_SUPPLY_PROP_MANUFACTURER, value);
		pr_info("%s RX firmware verify. result: %d\n", __func__, value.intval);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", value.intval);
		break;
	case BATT_WIRELESS_OTP_FIRMWARE_RESULT:
		value.intval = SEC_WIRELESS_OTP_FIRM_RESULT;
		psy_do_property(battery->pdata->wireless_charger_name, get,
			POWER_SUPPLY_PROP_MANUFACTURER, value);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", value.intval);
		break;
	case BATT_WIRELESS_IC_GRADE:
		value.intval = SEC_WIRELESS_IC_REVISION;
		psy_do_property(battery->pdata->wireless_charger_name, get,
			POWER_SUPPLY_PROP_MANUFACTURER, value);
		i += scnprintf(buf + i, PAGE_SIZE - i, "0x%x ", value.intval);

		value.intval = SEC_WIRELESS_IC_GRADE;
		psy_do_property(battery->pdata->wireless_charger_name, get,
			POWER_SUPPLY_PROP_MANUFACTURER, value);
		i += scnprintf(buf + i, PAGE_SIZE - i, "0x%x\n", value.intval);
		break;
	case BATT_WIRELESS_FIRMWARE_VER_BIN:
		value.intval = SEC_WIRELESS_OTP_FIRM_VER_BIN;
		psy_do_property(battery->pdata->wireless_charger_name, get,
			POWER_SUPPLY_PROP_MANUFACTURER, value);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%x\n", value.intval);
		break;
	case BATT_WIRELESS_FIRMWARE_VER:
		value.intval = SEC_WIRELESS_OTP_FIRM_VER;
		psy_do_property(battery->pdata->wireless_charger_name, get,
			POWER_SUPPLY_PROP_MANUFACTURER, value);

		i += scnprintf(buf + i, PAGE_SIZE - i, "%x\n", value.intval);
		break;
	case BATT_WIRELESS_TX_FIRMWARE_RESULT:
		value.intval = SEC_WIRELESS_TX_FIRM_RESULT;
		psy_do_property(battery->pdata->wireless_charger_name, get,
			POWER_SUPPLY_PROP_MANUFACTURER, value);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", value.intval);
		break;
	case BATT_WIRELESS_TX_FIRMWARE_VER:
		value.intval = SEC_WIRELESS_TX_FIRM_VER;
		psy_do_property(battery->pdata->wireless_charger_name, get,
			POWER_SUPPLY_PROP_MANUFACTURER, value);

		i += scnprintf(buf + i, PAGE_SIZE - i, "%x\n", value.intval);
		break;
	case BATT_TX_STATUS:
		value.intval = SEC_TX_FIRMWARE;
		psy_do_property(battery->pdata->wireless_charger_name, get,
			POWER_SUPPLY_PROP_MANUFACTURER, value);

		i += scnprintf(buf + i, PAGE_SIZE - i, "%x\n", value.intval);
		break;
#endif
#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
	case BATT_WIRELESS_VOUT:
		psy_do_property(battery->pdata->wireless_charger_name, get,
			POWER_SUPPLY_PROP_ENERGY_NOW, value);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", value.intval);	
		break;
	case BATT_WIRELESS_VRCT:
		psy_do_property(battery->pdata->wireless_charger_name, get,
			POWER_SUPPLY_PROP_ENERGY_AVG, value);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", value.intval);
		break;
	case BATT_HV_WIRELESS_STATUS:
		psy_do_property(battery->pdata->wireless_charger_name, get,
			POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", value.intval);
		break;
	case BATT_HV_WIRELESS_PAD_CTRL:
		break;
#endif
	case BATT_TUNE_FLOAT_VOLTAGE:
		ret = battery->pdata->chg_float_voltage;
		pr_info("%s float voltage = %d mA",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_INPUT_CHARGE_CURRENT:
		ret = battery->pdata->charging_current[i].input_current_limit;
		pr_info("%s input charge current = %d mA",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_FAST_CHARGE_CURRENT:
		ret = battery->pdata->charging_current[i].fast_charging_current;
		pr_info("%s fast charge current = %d mA",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_UI_TERM_CURRENT_1ST:
		ret = battery->pdata->charging_current[i].full_check_current_1st;
		pr_info("%s ui term current = %d mA",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_UI_TERM_CURRENT_2ND:
		ret = battery->pdata->charging_current[i].full_check_current_1st;
		pr_info("%s ui term current = %d mA",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_TEMP_HIGH_EVENT:
		ret = battery->pdata->temp_high_threshold_event;
		pr_info("%s temp high event block  = %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_TEMP_HIGH_REC_EVENT:
		ret = battery->pdata->temp_high_recovery_event;
		pr_info("%s temp high event recover  = %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_TEMP_LOW_EVENT:
		ret = battery->pdata->temp_low_threshold_event;
		pr_info("%s temp low event block  = %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_TEMP_LOW_REC_EVENT:
		ret = battery->pdata->temp_low_recovery_event;
		pr_info("%s temp low event recover	= %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_TEMP_HIGH_NORMAL:
		ret = battery->pdata->temp_high_threshold_normal;
		pr_info("%s temp high normal block	= %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_TEMP_HIGH_REC_NORMAL:
		ret = battery->pdata->temp_high_recovery_normal;
		pr_info("%s temp high normal recover  = %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_TEMP_LOW_NORMAL:
		ret = battery->pdata->temp_low_threshold_normal;
		pr_info("%s temp low normal block  = %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_TEMP_LOW_REC_NORMAL:
		ret = battery->pdata->temp_low_recovery_normal;
		pr_info("%s temp low normal recover  = %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_CHG_TEMP_HIGH:
		ret = battery->pdata->chg_high_temp_1st;
		pr_info("%s chg_high_temp_1st = %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_CHG_TEMP_REC:
		ret = battery->pdata->chg_high_temp_recovery;
		pr_info("%s chg_high_temp_recovery	= %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_CHG_LIMMIT_CURRENT:
		ret = battery->pdata->chg_charging_limit_current;
		pr_info("%s chg_charging_limit_current = %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_COIL_TEMP_HIGH:
		ret = battery->pdata->wpc_high_temp;
		pr_info("%s wpc_high_temp = %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_COIL_TEMP_REC:
		ret = battery->pdata->wpc_high_temp_recovery;
		pr_info("%s wpc_high_temp_recovery	= %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case BATT_TUNE_COIL_LIMMIT_CURRENT:
		ret = battery->pdata->wpc_charging_limit_current;
		pr_info("%s wpc_charging_limit_current = %d ",__func__, ret);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				ret);
		break;
	case CAMERA_TEMP_ADC:
		if (battery->pdata->camera_temp_check) {
			sec_bat_camera_temp_check(battery);
			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				       battery->camera_temp_adc);
		}
		break;
	case CAMERA_TEMP:
		if (battery->pdata->camera_temp_check) {
			sec_bat_camera_temp_check(battery);
			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
				       battery->camera_temp);
		}
		break;
	case CAMERA_LIMIT:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			       battery->camera_limit);
		break;
	case BATT_MISC_EVENT:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			       battery->misc_event);
		break;
	case CISD_FULLCAPREP_MAX:
		{
			union power_supply_propval fullcaprep_val;
			
			fullcaprep_val.intval = SEC_BATTERY_CAPACITY_FULL;
			psy_do_property(battery->pdata->fuelgauge_name, get,
				POWER_SUPPLY_PROP_ENERGY_NOW, fullcaprep_val);

			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
					fullcaprep_val.intval);
		}
		break;
#if defined(CONFIG_BATTERY_CISD)
	case CISD_DATA:
		{
			struct cisd *pcisd = &battery->cisd;

			i+= scnprintf(buf + i, PAGE_SIZE - i, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
					pcisd->data[CISD_DATA_FULL_COUNT], pcisd->data[CISD_DATA_CAP_MAX],
					pcisd->data[CISD_DATA_CAP_MIN], pcisd->data[CISD_DATA_CAP_ONCE],
					pcisd->data[CISD_DATA_LEAKAGE_A], pcisd->data[CISD_DATA_LEAKAGE_B],
					pcisd->data[CISD_DATA_LEAKAGE_C], pcisd->data[CISD_DATA_LEAKAGE_D],
					pcisd->data[CISD_DATA_CAP_PER_TIME], pcisd->data[CISD_DATA_ERRCAP_LOW],
					pcisd->data[CISD_DATA_ERRCAP_HIGH], pcisd->data[CISD_DATA_OVER_VOLTAGE],
					pcisd->data[CISD_DATA_LEAKAGE_E], pcisd->data[CISD_DATA_LEAKAGE_F],
					pcisd->data[CISD_DATA_LEAKAGE_G], pcisd->data[CISD_DATA_RECHARGING_TIME],
					-1, -1, -1, battery->cisd.cisd_alg_index);
		}
		break;
#endif
	default:
		i = -EINVAL;
	}

	return i;
}
void update_external_temp_table(struct sec_battery_info *battery, int temp[])
{
	battery->pdata->temp_high_threshold_event = temp[0];
	battery->pdata->temp_high_recovery_event = temp[1];
	battery->pdata->temp_low_threshold_event = temp[2];
	battery->pdata->temp_low_recovery_event = temp[3];
	battery->pdata->temp_high_threshold_normal = temp[4];
	battery->pdata->temp_high_recovery_normal = temp[5];
	battery->pdata->temp_low_threshold_normal = temp[6];
	battery->pdata->temp_low_recovery_normal = temp[7];
	battery->pdata->temp_high_threshold_lpm = temp[8];
	battery->pdata->temp_high_recovery_lpm = temp[9];
	battery->pdata->temp_low_threshold_lpm = temp[10];
	battery->pdata->temp_low_recovery_lpm = temp[11];

	if (battery->pdata->temp_high_threshold_event !=
		battery->pdata->temp_high_threshold_normal)
		battery->pdata->event_check = 1;
}

ssize_t sec_bat_store_attrs(
					struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	struct power_supply *psy = dev_get_drvdata(dev);
	struct sec_battery_info *battery =
		container_of(psy, struct sec_battery_info, psy_bat);
	const ptrdiff_t offset = attr - sec_battery_attrs;
	int ret = -EINVAL;
	int x = 0;
	int t[12];
	int i = 0;

	switch (offset) {
	case BATT_RESET_SOC:
		/* Do NOT reset fuel gauge in charging mode */
		if (battery->cable_type == POWER_SUPPLY_TYPE_BATTERY ||
			battery->cable_type == POWER_SUPPLY_TYPE_UARTOFF) {
			union power_supply_propval value;
			battery->voltage_now = 1234;
			battery->voltage_avg = 1234;
			power_supply_changed(&battery->psy_bat);

			value.intval =
				SEC_FUELGAUGE_CAPACITY_TYPE_RESET;
			psy_do_property(battery->pdata->fuelgauge_name, set,
					POWER_SUPPLY_PROP_CAPACITY, value);
			dev_info(battery->dev,"do reset SOC\n");
			/* update battery info */
			sec_bat_get_battery_info(battery);
		}
		ret = count;
		break;
	case BATT_READ_RAW_SOC:
		break;
	case BATT_READ_ADJ_SOC:
		break;
	case BATT_TYPE:
		break;
	case BATT_VFOCV:
		break;
	case BATT_VOL_ADC:
		break;
	case BATT_VOL_ADC_CAL:
		break;
	case BATT_VOL_AVER:
		break;
	case BATT_VOL_ADC_AVER:
		break;
	case BATT_CURRENT_UA_NOW:
		break;
	case BATT_CURRENT_UA_AVG:
		break;
	case BATT_FILTER_CFG:
		if (sscanf(buf, "%d\n", &x) == 1) {
			union power_supply_propval value;
			value.intval = x;
			psy_do_property(battery->pdata->fuelgauge_name, set,
					POWER_SUPPLY_PROP_FILTER_CFG, value);
			ret = count;
		}

		break;
	case BATT_TEMP:
		break;
	case BATT_TEMP_ADC:
		break;
	case BATT_TEMP_AVER:
		break;
	case BATT_TEMP_ADC_AVER:
		break;
	case CHG_TEMP:
		break;
	case CHG_TEMP_ADC:
		break;
	case BATT_VF_ADC:
		break;
	case BATT_SLATE_MODE:
		if (sscanf(buf, "%d\n", &x) == 1) {
			if (x == 1) {
				battery->slate_mode = true;
			} else if (x == 0) {
				battery->slate_mode = false;
			} else {
				dev_info(battery->dev,
					"%s: SLATE MODE unknown command\n",
					__func__);
				return -EINVAL;
			}
			slate_mode_state = battery->slate_mode;
			wake_lock(&battery->cable_wake_lock);
			queue_delayed_work_on(0, battery->monitor_wqueue,
					   &battery->cable_work, 0);
			ret = count;
		}
		break;

	case BATT_LP_CHARGING:
		break;
	case SIOP_ACTIVATED:
		break;
	case SIOP_LEVEL:
		if (sscanf(buf, "%d\n", &x) == 1) {
#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY)
			union power_supply_propval value;
#endif
			dev_info(battery->dev,
					"%s: siop level: %d\n", __func__, x);
			battery->chg_limit = SEC_BATTERY_CHG_TEMP_NONE;
			battery->mix_limit = SEC_BATTERY_MIX_TEMP_NONE;

#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
			wpc_temp_mode = false;
#endif
			if (x == battery->siop_level && battery->capacity > 5) {
				dev_info(battery->dev,
						"%s: skip same siop level: %d\n", __func__, x);
				return count;
			} else if (x >= 0 && x <= 100) {
				battery->siop_level = x;
			} else {
				battery->siop_level = 100;
			}
			battery->r_siop_level = battery->siop_level;

#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
			if (battery->siop_event == SIOP_EVENT_WPC_CALL_START ||
				battery->siop_event == SIOP_EVENT_WPC_CALL_END)
				return count;
#endif

#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY)
			value.intval = battery->siop_level;
			psy_do_property(battery->pdata->wireless_charger_name, set,
				POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN, value);

			if (battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS &&
					battery->status == POWER_SUPPLY_STATUS_CHARGING &&
					!battery->cc_cv_mode &&
					battery->siop_level == 100 ) {
				value.intval = 0; /* 5.5 */
				psy_do_property(battery->pdata->wireless_charger_name, set,
					POWER_SUPPLY_PROP_CHARGE_TYPE, value);
			} else if (battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS &&
					battery->status == POWER_SUPPLY_STATUS_CHARGING &&
					!battery->cc_cv_mode &&
					battery->siop_level != 100) {
				value.intval = 1; /* 5 */
				psy_do_property(battery->pdata->wireless_charger_name, set,
					POWER_SUPPLY_PROP_CHARGE_TYPE, value);
			}
#endif
			if (battery->capacity <= 5) {
				battery->siop_level = 100;
				battery->ignore_siop = true;
			} else if (battery->ignore_siop) {
				battery->ignore_siop = false;
			}

			wake_lock(&battery->siop_wake_lock);

			if (battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS ||
				battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS ||
				battery->cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS) {
				queue_delayed_work_on(0, battery->monitor_wqueue, &battery->siop_work,
					msecs_to_jiffies(1200));
			}
			else
				queue_delayed_work_on(0, battery->monitor_wqueue, &battery->siop_work, 0);

			ret = count;
		}
		break;
	case SIOP_EVENT:
#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
		if (sscanf(buf, "%d\n", &x) == 1) {
			if (battery->cable_type == POWER_SUPPLY_TYPE_WIRELESS) {
				if (x == SIOP_EVENT_WPC_CALL_START) {
					battery->siop_event = x;
					wake_lock(&battery->siop_wake_lock);
					queue_delayed_work_on(0, battery->monitor_wqueue, &battery->siop_work, 0);
				} else if (x == SIOP_EVENT_WPC_CALL_END) {
					battery->siop_event = x;
					pr_info("%s SIOP EVENT CALL END.\n", __func__);
					wake_lock(&battery->siop_wake_lock);
					queue_delayed_work_on(0, battery->monitor_wqueue, &battery->siop_work,
									msecs_to_jiffies(5000));
				} else {
					battery->siop_event = 0;
					pr_info("%s SIOP EVENT IDLE. \n", __func__);
				}
			} else {
				battery->siop_event = SIOP_EVENT_IDLE;
			}
		}
#endif
		break;
	case BATT_CHARGING_SOURCE:
		break;
	case FG_REG_DUMP:
		break;
	case FG_RESET_CAP:
		break;
	case FG_CAPACITY:
		break;
	case AUTH:
		break;
	case CHG_CURRENT_ADC:
		break;
	case WC_ADC:
		break;
	case WC_STATUS:
		break;
	case WC_ENABLE:
		if (sscanf(buf, "%d\n", &x) == 1) {
			if (x == 0) {
				battery->wc_enable = false;
			} else if (x == 1) {
				battery->wc_enable = true;
			} else {
				dev_info(battery->dev,
					"%s: WPC ENABLE unknown command\n",
					__func__);
				return -EINVAL;
			}
			wake_lock(&battery->cable_wake_lock);
			queue_delayed_work_on(0, battery->monitor_wqueue,
					&battery->cable_work, 0);
			ret = count;
		}
		break;
	case HV_CHARGER_STATUS:
		break;
	case HV_CHARGER_SET:
		if (sscanf(buf, "%d\n", &x) == 1) {
			dev_info(battery->dev,
				"%s: HV_CHARGER_SET(%d)\n", __func__, x);
			if (x == 1) {
				battery->wire_status = POWER_SUPPLY_TYPE_HV_MAINS;
				wake_lock(&battery->cable_wake_lock);
				queue_delayed_work_on(0, battery->monitor_wqueue, &battery->cable_work, 0);
			} else {
				battery->wire_status = POWER_SUPPLY_TYPE_BATTERY;
				wake_lock(&battery->cable_wake_lock);
				queue_delayed_work_on(0, battery->monitor_wqueue, &battery->cable_work, 0);
			}
			ret = count;
		}
		break;
	case HV_CHARGER_SUPPORT:
		break;
	case HV_WC_CHARGER_SUPPORT:
		break;	
	case FACTORY_MODE:
		if (sscanf(buf, "%d\n", &x) == 1) {
			battery->factory_mode = x ? true : false;
			ret = count;
		}
		break;
	case STORE_MODE:
		if (sscanf(buf, "%d\n", &x) == 1) {
			battery->store_mode = x ? true : false;
			ret = count;
			if (battery->store_mode) {
 				union power_supply_propval value;
 				value.intval = battery->store_mode;
 				psy_do_property(battery->pdata->charger_name, set,
 						POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX, value);
 			}
		}
		break;
	case UPDATE:
		if (sscanf(buf, "%d\n", &x) == 1) {
			/* update battery info */
			sec_bat_get_battery_info(battery);
			ret = count;
		}
		break;
	case TEST_MODE:
		if (sscanf(buf, "%d\n", &x) == 1) {
			battery->test_mode = x;
			wake_lock(&battery->monitor_wake_lock);
			queue_delayed_work_on(0, battery->monitor_wqueue,
				&battery->monitor_work, 0);
			ret = count;
		}
		break;

	case BATT_EVENT_CALL:
	case BATT_EVENT_2G_CALL:
	case BATT_EVENT_TALK_GSM:
		if (sscanf(buf, "%d\n", &x) == 1) {
			union power_supply_propval value;
			sec_bat_event_set(battery, EVENT_2G_CALL, x);
			value.intval = x;
			psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_SCOPE, value);
			ret = count;
		}
		break;
	case BATT_EVENT_3G_CALL:
	case BATT_EVENT_TALK_WCDMA:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(battery, EVENT_3G_CALL, x);
			ret = count;
		}
		break;
	case BATT_EVENT_MUSIC:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(battery, EVENT_MUSIC, x);
			ret = count;
		}
		break;
	case BATT_EVENT_VIDEO:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(battery, EVENT_VIDEO, x);
			ret = count;
		}
		break;
	case BATT_EVENT_BROWSER:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(battery, EVENT_BROWSER, x);
			ret = count;
		}
		break;
	case BATT_EVENT_HOTSPOT:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(battery, EVENT_HOTSPOT, x);
			ret = count;
		}
		break;
	case BATT_EVENT_CAMERA:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(battery, EVENT_CAMERA, x);
			ret = count;
		}
		break;
	case BATT_EVENT_CAMCORDER:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(battery, EVENT_CAMCORDER, x);
			ret = count;
		}
		break;
	case BATT_EVENT_DATA_CALL:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(battery, EVENT_DATA_CALL, x);
			ret = count;
		}
		break;
	case BATT_EVENT_WIFI:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(battery, EVENT_WIFI, x);
			ret = count;
		}
		break;
	case BATT_EVENT_WIBRO:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(battery, EVENT_WIBRO, x);
			ret = count;
		}
		break;
	case BATT_EVENT_LTE:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(battery, EVENT_LTE, x);
			ret = count;
		}
		break;
	case BATT_EVENT_LCD:
		if (sscanf(buf, "%d\n", &x) == 1) {
			struct timespec ts;
			ktime_t current_time;
			union power_supply_propval value;

#if defined(ANDROID_ALARM_ACTIVATED)
			current_time = alarm_get_elapsed_realtime();
			ts = ktime_to_timespec(current_time);
#else
			current_time = ktime_get_boottime();
			ts = ktime_to_timespec(current_time);
#endif

			if (x == 1) {
				if ((battery->status == POWER_SUPPLY_STATUS_CHARGING ||
				     battery->status == POWER_SUPPLY_STATUS_FULL) &&
				    !(battery->charging_block) &&
				    !battery->lcd_on_time) {
					battery->lcd_on_time = ts.tv_sec;
					pr_info("%s : LCD EVENT(%d) LCD_ON_TOTAL_TIME (%ld) LCD_ON_TIME (%ld)\n",
						__func__, x, battery->lcd_on_total_time, battery->lcd_on_time);
				}

				if (!(battery->event & EVENT_LCD)) {
					battery->event |= EVENT_LCD;
					pr_info("LCD on\n");
					value.intval = 1;
					psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_SCOPE, value);
				}
			} else {
				if ((battery->status == POWER_SUPPLY_STATUS_CHARGING ||
				     battery->status == POWER_SUPPLY_STATUS_FULL) &&
				    !(battery->charging_block) &&
				    battery->lcd_on_time) {
					battery->lcd_on_total_time += ts.tv_sec - battery->lcd_on_time;
					pr_info("%s : LCD EVENT(%d) LCD_ON_TOTAL_TIME (%ld) LCD_ON_TIME (%ld)\n",
						__func__, x, battery->lcd_on_total_time, battery->lcd_on_time);
					battery->lcd_on_time = 0;
				}

				if (battery->event & EVENT_LCD) {
					battery->event &= ~EVENT_LCD;
					pr_info("LCD off\n");
					value.intval = 0;
					psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_SCOPE, value);
				}
			}
			ret = count;
		}
		break;
	case BATT_EVENT_GPS:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(battery, EVENT_GPS, x);
			ret = count;
		}
		break;
	case BATT_TEMP_TABLE:
		if (sscanf(buf, "%d %d %d %d %d %d %d %d %d %d %d %d\n",
			&t[0], &t[1], &t[2], &t[3], &t[4], &t[5], &t[6], &t[7], &t[8], &t[9], &t[10], &t[11]) == 12) {
			pr_info("%s: (new) %d %d %d %d %d %d %d %d %d %d %d %d\n",
				__func__, t[0], t[1], t[2], t[3], t[4], t[5], t[6], t[7], t[8], t[9], t[10], t[11]);
			pr_info("%s: (default) %d %d %d %d %d %d %d %d %d %d %d %d\n",
				__func__,
				battery->pdata->temp_high_threshold_event,
				battery->pdata->temp_high_recovery_event,
				battery->pdata->temp_low_threshold_event,
				battery->pdata->temp_low_recovery_event,
				battery->pdata->temp_high_threshold_normal,
				battery->pdata->temp_high_recovery_normal,
				battery->pdata->temp_low_threshold_normal,
				battery->pdata->temp_low_recovery_normal,
				battery->pdata->temp_high_threshold_lpm,
				battery->pdata->temp_high_recovery_lpm,
				battery->pdata->temp_low_threshold_lpm,
				battery->pdata->temp_low_recovery_lpm);
			update_external_temp_table(battery, t);
			ret = count;
		}
		break;
	case BATT_HIGH_CURRENT_USB:
		if (sscanf(buf, "%d\n", &x) == 1) {
			union power_supply_propval value;
			battery->is_hc_usb = x ? true : false;
			value.intval = battery->is_hc_usb;

			psy_do_property(battery->pdata->charger_name, set,
				POWER_SUPPLY_PROP_USB_HC, value);

			pr_info("%s: is_hc_usb (%d)\n", __func__, battery->is_hc_usb);
			ret = count;
		}
		break;
#if defined(CONFIG_SAMSUNG_BATTERY_ENG_TEST)
	case BATT_TEST_CHARGE_CURRENT:
		if (sscanf(buf, "%d\n", &x) == 1) {
			if (x >= 0 && x <= 2000) {
				union power_supply_propval value;
				dev_err(battery->dev,
					"%s: BATT_TEST_CHARGE_CURRENT(%d)\n", __func__, x);
				battery->pdata->charging_current[
					POWER_SUPPLY_TYPE_USB].input_current_limit = x;
				battery->pdata->charging_current[
					POWER_SUPPLY_TYPE_USB].fast_charging_current = x;
				if (x > 500) {
					battery->eng_not_full_status = true;
					battery->pdata->temp_check_type =
						SEC_BATTERY_TEMP_CHECK_NONE;
					battery->pdata->charging_total_time =
						10000 * 60 * 60;
				}
				if (battery->cable_type == POWER_SUPPLY_TYPE_USB) {
					value.intval = x;
					psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_CURRENT_NOW,
						value);
				}
			}
			ret = count;
		}
		break;
#endif
	case BATT_STABILITY_TEST:
		if (sscanf(buf, "%d\n", &x) == 1) {
			dev_err(battery->dev,
				"%s: BATT_STABILITY_TEST(%d)\n", __func__, x);
			if (x) {
				battery->stability_test = true;
				battery->eng_not_full_status = true;
			}
			else {
				battery->stability_test = false;
				battery->eng_not_full_status = false;
			}
			ret = count;
		}
		break;
#if !defined(CONFIG_DISABLE_SAVE_CAPACITY_MAX)
	case BATT_CAPACITY_MAX:
		if (sscanf(buf, "%d\n", &x) == 1) {
			union power_supply_propval value;
			dev_err(battery->dev,
					"%s: BATT_CAPACITY_MAX(%d), fg_reset(%d)\n", __func__, x, fg_reset);
			if (x  > 800 && x < 1200 && !fg_reset) {
				value.intval = x;
				psy_do_property(battery->pdata->fuelgauge_name, set,
						POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN, value);
				/* To get SOC value (NOT raw SOC), need to reset value */
				value.intval = 0;
				psy_do_property(battery->pdata->fuelgauge_name, get,
						POWER_SUPPLY_PROP_CAPACITY, value);
				dev_err(battery->dev,
					"%s: soc(%d)->soc(%d), BATT_CAPACITY_MAX(%d)\n",
					__func__, battery->capacity, value.intval, x);
				battery->capacity = value.intval;
				wake_lock(&battery->monitor_wake_lock);
				queue_delayed_work_on(0, battery->monitor_wqueue,
						   &battery->monitor_work, 0);
			}
			ret = count;
		}
		break;
#endif
	case BATT_INBAT_VOLTAGE:
		break;
#if defined(CONFIG_BATTERY_SWELLING_SELF_DISCHARGING)
	case BATT_DISCHARGING_CHECK:
		break;
	case BATT_DISCHARGING_CHECK_ADC:
		break;
	case BATT_DISCHARGING_NTC:
		break;
	case BATT_DISCHARGING_NTC_ADC:
		break;
	case BATT_SELF_DISCHARGING_CONTROL:
		if (battery->pdata->sdchg_info) {
			if (sscanf(buf, "%d\n", &x) == 1) {
				dev_err(battery->dev,
					"%s: BATT_SELF_DISCHARGING_CONTROL(%d)\n", __func__, x);
				if (x) {
					battery->factory_self_discharging_mode_on = true;
					pr_info("SELF DISCHARGING IC ENABLE\n");
					battery->pdata->sdchg_info->sdchg_force_control(battery, true);
				} else {
					battery->factory_self_discharging_mode_on = false;
					pr_info("SELF DISCHARGING IC DISENABLE\n");
					battery->pdata->sdchg_info->sdchg_force_control(battery, false);
				}
				ret = count;
			}
		} else {
			pr_info("[SDCHG][%s] discharging info structure is null!!\n", __func__);
		}
		break;
#endif
#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY) || defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
	case BATT_INBAT_WIRELESS_CS100:
		if (sscanf(buf, "%d\n", &x) == 1) {
			union power_supply_propval value;

			pr_info("%s send cs100 command \n",__func__);
			value.intval = POWER_SUPPLY_STATUS_FULL;
			psy_do_property(battery->pdata->wireless_charger_name, set,
					POWER_SUPPLY_PROP_STATUS, value);
			ret = count;
		}
		break;
#endif
	case HMT_TA_CONNECTED:
		if (sscanf(buf, "%d\n", &x) == 1) {
			union power_supply_propval value;
			dev_info(battery->dev,
					"%s: HMT_TA_CONNECTED(%d)\n", __func__, x);
			if (x) {
				value.intval = false;
				psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL,
						value);
				dev_info(battery->dev,
						"%s: changed to OTG cable detached\n", __func__);

				battery->wire_status = POWER_SUPPLY_TYPE_HMT_CONNECTED;
				wake_lock(&battery->cable_wake_lock);
				queue_delayed_work_on(0, battery->monitor_wqueue, &battery->cable_work, 0);
			} else {
				value.intval = true;
				psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL,
						value);
				dev_info(battery->dev,
						"%s: changed to OTG cable attached\n", __func__);

				battery->wire_status = POWER_SUPPLY_TYPE_OTG;
				wake_lock(&battery->cable_wake_lock);
				queue_delayed_work_on(0, battery->monitor_wqueue, &battery->cable_work, 0);
			}
			ret = count;
		}
		break;
	case HMT_TA_CHARGE:
		if (sscanf(buf, "%d\n", &x) == 1) {
			union power_supply_propval value;
			dev_info(battery->dev,
					"%s: HMT_TA_CHARGE(%d)\n", __func__, x);
			psy_do_property(battery->pdata->charger_name, get,
					POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, value);
			if (value.intval) {
				dev_info(battery->dev,
					"%s: ignore HMT_TA_CHARGE(%d)\n", __func__, x);
			} else {
				if (x) {
					value.intval = false;
					psy_do_property(battery->pdata->charger_name, set,
							POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL,
							value);
					dev_info(battery->dev,
						"%s: changed to OTG cable detached\n", __func__);
					battery->wire_status = POWER_SUPPLY_TYPE_HMT_CHARGE;
					wake_lock(&battery->cable_wake_lock);
					queue_delayed_work_on(0, battery->monitor_wqueue, &battery->cable_work, 0);
				} else {
					value.intval = false;
					psy_do_property(battery->pdata->charger_name, set,
							POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL,
							value);
					dev_info(battery->dev,
							"%s: changed to OTG cable detached\n", __func__);
					battery->wire_status = POWER_SUPPLY_TYPE_HMT_CONNECTED;
					wake_lock(&battery->cable_wake_lock);
					queue_delayed_work_on(0, battery->monitor_wqueue, &battery->cable_work, 0);
				}
			}
			ret = count;
		}
		break;
	case FG_CYCLE:
		break;
	case FG_FULL_VOLTAGE:
		break;
	case FG_FULLCAPNOM:
		break;
	case BATTERY_CYCLE:
		if (sscanf(buf, "%d\n", &x) == 1) {
			dev_info(battery->dev, "%s: BATTERY_CYCLE(%d)\n", __func__, x);
			if (x >= 0) {
				int prev_battery_cycle;
				prev_battery_cycle = battery->batt_cycle;
				battery->batt_cycle = x;
#if defined(CONFIG_BATTERY_AGE_FORECAST)
				if (prev_battery_cycle < 0) {
					sec_bat_aging_check(battery);
				}
#endif
			}
			ret = count;
		}
		break;
#if defined(CONFIG_WIRELESS_CHARGER_THM)
	case BATT_WPC_TEMP:
	case BATT_WPC_TEMP_ADC:
		break;
#endif
#if defined(CONFIG_WIRELESS_FIRMWARE_UPDATE)
	case BATT_WIRELESS_FIRMWARE_UPDATE:
		if (sscanf(buf, "%d\n", &x) == 1) {
			if (x == SEC_WIRELESS_RX_SDCARD_MODE) {
				pr_info("%s fw mode is SDCARD \n", __func__);
				sec_bat_fw_update_work(battery, SEC_WIRELESS_RX_SDCARD_MODE);
			} else if (x == SEC_WIRELESS_RX_BUILT_IN_MODE) {
				pr_info("%s fw mode is BUILD IN \n", __func__);
				sec_bat_fw_update_work(battery, SEC_WIRELESS_RX_BUILT_IN_MODE);
			} else if (x == SEC_WIRELESS_TX_ON_MODE) {
				pr_info("%s tx mode is on \n", __func__);
				sec_bat_fw_update_work(battery, SEC_WIRELESS_TX_ON_MODE);
			} else if (x == SEC_WIRELESS_TX_OFF_MODE) {
				pr_info("%s tx mode is off \n", __func__);
				sec_bat_fw_update_work(battery, SEC_WIRELESS_TX_OFF_MODE);
			} else {
				dev_info(battery->dev, "%s: wireless firmware unknown command\n", __func__);
				return -EINVAL;
			}
			ret = count;
		}
		break;
	case BATT_WIRELESS_OTP_FIRMWARE_RESULT:
		if (sscanf(buf, "%d\n", &x) == 1) {
			union power_supply_propval value;
			if (x == 2) {
				value.intval = x;
				pr_info("%s RX firmware update ready!\n", __func__);
				psy_do_property(battery->pdata->wireless_charger_name, set,
								POWER_SUPPLY_PROP_MANUFACTURER, value);
			} else {
				dev_info(battery->dev, "%s: firmware unknown command\n", __func__);
				return -EINVAL;
			}
			ret = count;
		}
		break;
	case BATT_WIRELESS_IC_GRADE:
	case BATT_WIRELESS_FIRMWARE_VER_BIN:
	case BATT_WIRELESS_FIRMWARE_VER:
	case BATT_WIRELESS_TX_FIRMWARE_RESULT:
	case BATT_WIRELESS_TX_FIRMWARE_VER:
		break;
	case BATT_TX_STATUS:
		if (sscanf(buf, "%d\n", &x) == 1) {
			if (x == SEC_TX_OFF) {
				pr_info("%s TX mode is off \n", __func__);
				sec_bat_fw_update_work(battery, SEC_WIRELESS_TX_OFF_MODE);
			} else if (x == SEC_TX_STANDBY) {
				pr_info("%s TX mode is on \n", __func__);
				sec_bat_fw_update_work(battery, SEC_WIRELESS_TX_ON_MODE);
			} else {
				dev_info(battery->dev, "%s: TX firmware unknown command\n", __func__);
				return -EINVAL;
			}
			ret = count;
		}
		break;
#endif
#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
	case BATT_WIRELESS_VOUT:
	case BATT_WIRELESS_VRCT:
		break;
	case BATT_HV_WIRELESS_STATUS:
		if (sscanf(buf, "%d\n", &x) == 1) {
			union power_supply_propval value;
			if (x == 1 && battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS) {
				wake_lock(&battery->cable_wake_lock);
#ifdef CONFIG_SEC_FACTORY
				pr_info("%s change cable type HV WIRELESS -> WIRELESS \n", __func__);
				value.intval = SEC_WIRELESS_PAD_WPC;
				psy_do_property("wireless", set, POWER_SUPPLY_PROP_ONLINE, value);

				msleep(1500);
#endif
				pr_info("%s HV_WIRELESS_STATUS set to 1. Vout set to 5V. \n", __func__);
				value.intval = WIRELESS_VOUT_5V;
				psy_do_property(battery->pdata->wireless_charger_name, set,
								POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);
				wake_unlock(&battery->cable_wake_lock);
			} else if (x == 3 && battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS) {
				pr_info("%s HV_WIRELESS_STATUS set to 3. Vout set to 9V. \n", __func__);
				value.intval = WIRELESS_VOUT_9V;
				psy_do_property(battery->pdata->wireless_charger_name, set,
								POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);
			} else {
				dev_info(battery->dev, "%s: HV_WIRELESS_STATUS unknown command\n", __func__);
				return -EINVAL;
			}
			ret = count;
		}
		break;
	case BATT_HV_WIRELESS_PAD_CTRL:
		if (sscanf(buf, "%d\n", &x) == 1) {
			union power_supply_propval value;

			pr_err("%s: x : %d\n", __func__, x);

			if (x == 1) {
				ret = sec_set_param(CM_OFFSET, '1');
				if (ret < 0) {
					pr_err("%s:sec_set_param failed\n", __func__);
					return ret;
				}
				else {
					pr_info("%s fan off \n", __func__);
					sleep_mode = true;
					if (battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS) {
#if defined(CONFIG_CALC_TIME_TO_FULL)
						battery->complete_timetofull = false;
#endif
						value.intval = WIRELESS_PAD_FAN_ON;
						psy_do_property(battery->pdata->wireless_charger_name, set,
									POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);
						msleep(250);
						value.intval = WIRELESS_PAD_FAN_OFF;
						psy_do_property(battery->pdata->wireless_charger_name, set,
							POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);

						msleep(250);
						value.intval = battery->pdata->sleep_mode_limit_current;
						psy_do_property(battery->pdata->charger_name, set,
								POWER_SUPPLY_PROP_CURRENT_MAX, value);
#if defined(CONFIG_CALC_TIME_TO_FULL)
						queue_delayed_work_on(0, battery->monitor_wqueue, &battery->timetofull_work,
								msecs_to_jiffies(5000));
#endif
						wake_lock(&battery->monitor_wake_lock);
						queue_delayed_work_on(0, battery->monitor_wqueue, &battery->monitor_work, 0);
					}
				}
			} else if (x == 2) {
				ret = sec_set_param(CM_OFFSET, '0');
				if (ret < 0) {
					pr_err("%s: sec_set_param failed\n", __func__);
					return ret;
				}
				else {
					sleep_mode = false;
					pr_info("%s fan on \n", __func__);
					if (battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS) {
#if defined(CONFIG_CALC_TIME_TO_FULL)
						battery->complete_timetofull = false;
#endif
						value.intval = WIRELESS_PAD_FAN_ON;
						psy_do_property(battery->pdata->wireless_charger_name, set,
									POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);

						msleep(250);

#if defined(CONFIG_CALC_TIME_TO_FULL)
						queue_delayed_work_on(0, battery->monitor_wqueue, &battery->timetofull_work,
								msecs_to_jiffies(5000));
#endif
						wake_lock(&battery->monitor_wake_lock);
						queue_delayed_work_on(0, battery->monitor_wqueue, &battery->monitor_work, 0);
					}
				}
			} else if (x == 3) {
				pr_info("%s led off \n", __func__);
				value.intval = WIRELESS_PAD_LED_OFF;
				psy_do_property(battery->pdata->wireless_charger_name, set,
								POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);
			} else if (x == 4) {
				pr_info("%s led on \n", __func__);
				value.intval = WIRELESS_PAD_LED_ON;
				psy_do_property(battery->pdata->wireless_charger_name, set,
								POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);
			} else {
				dev_info(battery->dev, "%s: BATT_HV_WIRELESS_PAD_CTRL unknown command\n", __func__);
				return -EINVAL;
			}
			ret = count;
		}
		break;
#endif
	case BATT_TUNE_FLOAT_VOLTAGE:
		sscanf(buf, "%d\n", &x);
		pr_info("%s float voltage = %d mV",__func__, x);

		if(x > 4000 && x <= 4400 ){
			union power_supply_propval value;
			value.intval = x;
			psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_VOLTAGE_MAX, value);
		}
		break;
	case BATT_TUNE_INPUT_CHARGE_CURRENT:
		sscanf(buf, "%d\n", &x);
		pr_info("%s input charge current = %d mA",__func__, x);

		if(x > 0 && x <= 4000 ){
			union power_supply_propval value;
			for(i=0; i<POWER_SUPPLY_TYPE_MAX; i++)
				battery->pdata->charging_current[i].input_current_limit = x;

			value.intval = x;
			psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_CURRENT_MAX, value);
		}
		break;
	case BATT_TUNE_FAST_CHARGE_CURRENT:
		sscanf(buf, "%d\n", &x);
		pr_info("%s fast charge current = %d mA",__func__, x);
		if(x > 0 && x <= 4000 ){
			union power_supply_propval value;
			for(i=0; i<POWER_SUPPLY_TYPE_MAX; i++)
				battery->pdata->charging_current[i].fast_charging_current = x;

			value.intval = x;
				psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_CURRENT_AVG, value);
		}
		break;
	case BATT_TUNE_UI_TERM_CURRENT_1ST:
		sscanf(buf, "%d\n", &x);
		pr_info("%s ui term current = %d mA",__func__, x);

		if(x > 0 && x < 1000 ){
			for(i=0; i<POWER_SUPPLY_TYPE_MAX; i++)
				battery->pdata->charging_current[i].full_check_current_1st = x;
		}
		break;
	case BATT_TUNE_UI_TERM_CURRENT_2ND:
		sscanf(buf, "%d\n", &x);
		pr_info("%s ui term current = %d mA",__func__, x);

		if(x > 0 && x < 1000 ){
			for(i=0; i<POWER_SUPPLY_TYPE_MAX; i++)
				battery->pdata->charging_current[i].full_check_current_1st = x;
		}
		break;	
	case BATT_TUNE_TEMP_HIGH_EVENT:
		sscanf(buf, "%d\n", &x);
		pr_info("%s temp high event block  = %d ",__func__, x);
		if(x < 900 && x > -200)
			battery->pdata->temp_high_threshold_event = x;
		break;
	case BATT_TUNE_TEMP_HIGH_REC_EVENT:
		sscanf(buf, "%d\n", &x);
		pr_info("%s temp high event recover  = %d ",__func__, x);
		if(x < 900 && x > -200)
			battery->pdata->temp_high_recovery_event = x;
		break;
	case BATT_TUNE_TEMP_LOW_EVENT:
		sscanf(buf, "%d\n", &x);
		pr_info("%s temp low event block  = %d ",__func__, x);
		if(x <900 && x > -200)
			battery->pdata->temp_low_threshold_event = x;
		break;
	case BATT_TUNE_TEMP_LOW_REC_EVENT:
		sscanf(buf, "%d\n", &x);
		pr_info("%s temp low event recover	= %d ",__func__, x);
		if(x < 900 && x > -200)
			battery->pdata->temp_low_recovery_event = x;
		break;
	case BATT_TUNE_TEMP_HIGH_NORMAL:
		sscanf(buf, "%d\n", &x);
		pr_info("%s temp high normal block	= %d ",__func__, x);
		if(x < 900 && x > -200)
			battery->pdata->temp_high_threshold_normal = x;
		break;
	case BATT_TUNE_TEMP_HIGH_REC_NORMAL:
		sscanf(buf, "%d\n", &x);
		pr_info("%s temp high normal recover  = %d ",__func__, x);
		if(x <900 && x > -200)
			battery->pdata->temp_high_recovery_normal = x;
		break;
	case BATT_TUNE_TEMP_LOW_NORMAL:
		sscanf(buf, "%d\n", &x);
		pr_info("%s temp low normal block  = %d ",__func__, x);
		if(x <900 && x > -200)
			battery->pdata->temp_low_threshold_normal = x;
		break;
	case BATT_TUNE_TEMP_LOW_REC_NORMAL:
		sscanf(buf, "%d\n", &x);
		pr_info("%s temp low normal recover  = %d ",__func__, x);
		if(x <900 && x > -200)
			battery->pdata->temp_low_recovery_normal = x;
		break;
	case BATT_TUNE_CHG_TEMP_HIGH:
		sscanf(buf, "%d\n", &x);
		pr_info("%s chg_high_temp  = %d ",__func__, x);
		if(x <900 && x > -200)
			battery->pdata->chg_high_temp_1st = x;
		break;
	case BATT_TUNE_CHG_TEMP_REC:
		sscanf(buf, "%d\n", &x);
		pr_info("%s chg_high_temp_recovery	= %d ",__func__, x);
		if(x <900 && x > -200)
			battery->pdata->chg_high_temp_recovery = x;
		break;
	case BATT_TUNE_CHG_LIMMIT_CURRENT:
		sscanf(buf, "%d\n", &x);
		pr_info("%s chg_charging_limit_current	= %d ",__func__, x);
		if(x <3000 && x > 0)
		{
			battery->pdata->chg_charging_limit_current = x;
			battery->pdata->charging_current[POWER_SUPPLY_TYPE_HV_ERR].input_current_limit= x;
			battery->pdata->charging_current[POWER_SUPPLY_TYPE_HV_UNKNOWN].input_current_limit= x;
			battery->pdata->charging_current[POWER_SUPPLY_TYPE_HV_MAINS].input_current_limit= x;
		}
		break;
	case BATT_TUNE_COIL_TEMP_HIGH:
		sscanf(buf, "%d\n", &x);
		pr_info("%s wpc_high_temp  = %d ",__func__, x);
		if(x <900 && x > -200)
			battery->pdata->wpc_high_temp = x;
		break;
	case BATT_TUNE_COIL_TEMP_REC:
		sscanf(buf, "%d\n", &x);
		pr_info("%s wpc_high_temp_recovery	= %d ",__func__, x);
		if(x <900 && x > -200)
			battery->pdata->wpc_high_temp_recovery = x;
		break;
	case BATT_TUNE_COIL_LIMMIT_CURRENT:
		sscanf(buf, "%d\n", &x);
		pr_info("%s wpc_charging_limit_current	= %d ",__func__, x);
		if(x <3000 && x > 0)
		{
			battery->pdata->wpc_charging_limit_current = x;
			battery->pdata->charging_current[POWER_SUPPLY_TYPE_HV_ERR].input_current_limit= x;
			battery->pdata->charging_current[POWER_SUPPLY_TYPE_HV_UNKNOWN].input_current_limit= x;
			battery->pdata->charging_current[POWER_SUPPLY_TYPE_HV_MAINS].input_current_limit= x;
		}
		break;
	case CAMERA_TEMP_ADC:
		break;
	case CAMERA_TEMP:
		break;
	case CAMERA_LIMIT:
		break;
	case BATT_MISC_EVENT:
		break;
	case CISD_FULLCAPREP_MAX:
		break;
#if defined(CONFIG_BATTERY_CISD)
	case CISD_DATA:
		{
			int temp_data[CISD_DATA_MAX];

			if (sscanf(buf, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
					&temp_data[0], &temp_data[1], &temp_data[2],
					&temp_data[3], &temp_data[4], &temp_data[5],
					&temp_data[6], &temp_data[7], &temp_data[8],
					&temp_data[9], &temp_data[10], &temp_data[11],
					&temp_data[12], &temp_data[13], &temp_data[14],
					&temp_data[15]) == CISD_DATA_MAX) {
				struct cisd *pcisd = &battery->cisd;

				pr_info("%s: %s cisd data\n", __func__, ((temp_data[0] < 0) ? "init" : "update"));
				if (temp_data[0] < 0) {
					/* initialize data */
					pcisd->data[CISD_DATA_CAP_ONCE] = 0;
					pcisd->data[CISD_DATA_CAP_PER_TIME] = 0;
					pcisd->data[CISD_DATA_RECHARGING_TIME] = 0x7FFFFFFF;
				} else {
					/* update data */			
					for (i = 0; i < CISD_DATA_MAX; i++)
						pcisd->data[i] = temp_data[i];
				}
				ret = count;

				wake_lock(&battery->monitor_wake_lock);
				queue_delayed_work(battery->monitor_wqueue, &battery->monitor_work, 0);
			}
		}
		break;
#endif
	default:
		ret = -EINVAL;
	}

	return ret;
}

static int sec_bat_create_attrs(struct device *dev)
{
	unsigned long i;
	int rc;

	for (i = 0; i < ARRAY_SIZE(sec_battery_attrs); i++) {
		rc = device_create_file(dev, &sec_battery_attrs[i]);
		if (rc)
			goto create_attrs_failed;
	}
	goto create_attrs_succeed;

create_attrs_failed:
	while (i--)
		device_remove_file(dev, &sec_battery_attrs[i]);
create_attrs_succeed:
	return rc;
}

static int sec_bat_set_property(struct power_supply *psy,
				enum power_supply_property psp,
				const union power_supply_propval *val)
{
	struct sec_battery_info *battery =
		container_of(psy, struct sec_battery_info, psy_bat);
	int current_cable_type;
	int full_check_type;
	union power_supply_propval value;

	dev_dbg(battery->dev,
		"%s: (%d,%d)\n", __func__, psp, val->intval);

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		if (battery->charging_mode == SEC_BATTERY_CHARGING_1ST)
			full_check_type = battery->pdata->full_check_type;
		else
			full_check_type = battery->pdata->full_check_type_2nd;
		if ((full_check_type == SEC_BATTERY_FULLCHARGED_CHGINT) &&
			(val->intval == POWER_SUPPLY_STATUS_FULL))
			sec_bat_do_fullcharged(battery);
		sec_bat_set_charging_status(battery, val->intval);
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		sec_bat_ovp_uvlo_result(battery, val->intval);
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		current_cable_type = val->intval;

		if ((battery->muic_cable_type != ATTACHED_DEV_SMARTDOCK_TA_MUIC)
		    && ((current_cable_type == POWER_SUPPLY_TYPE_SMART_OTG) ||
			(current_cable_type == POWER_SUPPLY_TYPE_SMART_NOTG)))
			break;

		if (current_cable_type < 0) {
			dev_info(battery->dev,
					"%s: ignore event(%d)\n",
					__func__, current_cable_type);
		} else if (current_cable_type == POWER_SUPPLY_TYPE_OTG) {
			battery->charging_mode = SEC_BATTERY_CHARGING_NONE;
			battery->is_recharging = false;
			sec_bat_set_charging_status(battery,
					POWER_SUPPLY_STATUS_DISCHARGING);
			battery->cable_type = current_cable_type;
			wake_lock(&battery->monitor_wake_lock);
			queue_delayed_work_on(0, battery->monitor_wqueue,
					   &battery->monitor_work, 0);
			break;
		} else {
			battery->wire_status = current_cable_type;
			if ((battery->wire_status == POWER_SUPPLY_TYPE_BATTERY) && 
				(battery->wc_status != SEC_WIRELESS_PAD_NONE) )
				current_cable_type = POWER_SUPPLY_TYPE_WIRELESS;
		}
		dev_info(battery->dev,
				"%s: current_cable(%d), wc_status(%d), wire_status(%d)\n",
				__func__, current_cable_type, battery->wc_status,
				battery->wire_status);

		/* cable is attached or detached
		 * if current_cable_type is minus value,
		 * check cable by sec_bat_get_cable_type()
		 * although SEC_BATTERY_CABLE_SOURCE_EXTERNAL is set
		 * (0 is POWER_SUPPLY_TYPE_UNKNOWN)
		 */
		if ((current_cable_type >= 0) &&
			(current_cable_type < SEC_SIZEOF_POWER_SUPPLY_TYPE) &&
			(battery->pdata->cable_source_type &
			SEC_BATTERY_CABLE_SOURCE_EXTERNAL)) {

			wake_lock(&battery->cable_wake_lock);
				queue_delayed_work_on(0, battery->monitor_wqueue,
					&battery->cable_work,0);
		} else {
			if (sec_bat_get_cable_type(battery,
						battery->pdata->cable_source_type)) {
				wake_lock(&battery->cable_wake_lock);
					queue_delayed_work_on(0, battery->monitor_wqueue,
						&battery->cable_work,0);
			}
		}
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		battery->capacity = val->intval;
		power_supply_changed(&battery->psy_bat);
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		/* If JIG is attached, the voltage is set as 1079 */
		pr_info("%s : set to the battery history : (%d)\n",__func__, val->intval);
		if(val->intval == 1079)	{
			battery->voltage_now = 1079;
			battery->voltage_avg = 1079;
			power_supply_changed(&battery->psy_bat);
		}
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		queue_delayed_work_on(0, battery->monitor_wqueue, &battery->monitor_work, 0);
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		battery->present = val->intval;

		wake_lock(&battery->monitor_wake_lock);
		queue_delayed_work_on(0, battery->monitor_wqueue,
				   &battery->monitor_work, 0);
		break;
#if defined(CONFIG_BATTERY_SWELLING)
	case POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT:
		break;
#endif
	case POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION:
	case POWER_SUPPLY_PROP_CHARGE_COUNTER_SHADOW:
		break;
	case POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL:
		value.intval = val->intval;
		pr_info("%s: OTG %s\n", __func__, value.intval > 0 ? "on" : "off");
		psy_do_property(battery->pdata->charger_name, set,
				POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, value);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int sec_bat_get_property(struct power_supply *psy,
				enum power_supply_property psp,
				union power_supply_propval *val)
{
	struct sec_battery_info *battery =
		container_of(psy, struct sec_battery_info, psy_bat);
	union power_supply_propval value;
#if defined(CONFIG_STORE_MODE)
	union power_supply_propval value_ac;
	union power_supply_propval value_usb;
#endif

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		if ((battery->health == POWER_SUPPLY_HEALTH_OVERVOLTAGE) ||
			(battery->health == POWER_SUPPLY_HEALTH_UNDERVOLTAGE)) {
				val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		} else {
			if ((battery->pdata->cable_check_type &
				SEC_BATTERY_CABLE_CHECK_NOUSBCHARGE) &&
				!lpcharge) {
					switch (battery->cable_type) {
					case POWER_SUPPLY_TYPE_USB:
					case POWER_SUPPLY_TYPE_USB_DCP:
					case POWER_SUPPLY_TYPE_USB_CDP:
					case POWER_SUPPLY_TYPE_USB_ACA:
						val->intval =
							POWER_SUPPLY_STATUS_DISCHARGING;
						return 0;
					}
				}
#if defined(CONFIG_AFC_CHARGER_MODE)
			if (battery->status == POWER_SUPPLY_STATUS_FULL &&
				battery->capacity != 100) {
				val->intval = POWER_SUPPLY_STATUS_CHARGING;
				pr_info("%s: forced full-charged sequence progressing\n", __func__);
			} else
#endif
				val->intval = battery->status;
		}
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		if (battery->cable_type == POWER_SUPPLY_TYPE_BATTERY ||
			battery->cable_type == POWER_SUPPLY_TYPE_MHL_USB_100) {
			val->intval = POWER_SUPPLY_CHARGE_TYPE_NONE;
		} else {
			psy_do_property(battery->pdata->charger_name, get,
				POWER_SUPPLY_PROP_CHARGE_TYPE, value);
			if (value.intval == POWER_SUPPLY_CHARGE_TYPE_UNKNOWN)
				/* if error in CHARGE_TYPE of charger
				 * set CHARGE_TYPE as NONE
				 */
				val->intval = POWER_SUPPLY_CHARGE_TYPE_NONE;
			else
				val->intval = value.intval;
		}
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = battery->health;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = battery->present;
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		if(battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS && sleep_mode)
			val->intval = POWER_SUPPLY_TYPE_WIRELESS;		
		else if(battery->cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS)
			val->intval = POWER_SUPPLY_TYPE_HV_WIRELESS_ETX;

		else if(battery->cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS)
			val->intval = POWER_SUPPLY_TYPE_WIRELESS;
		else
			val->intval = battery->cable_type;

		pr_info("%s cable type = %d sleep_mode = %d\n", __func__, val->intval, sleep_mode);
		break;
	case POWER_SUPPLY_PROP_TECHNOLOGY:
		val->intval = battery->pdata->technology;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
#ifdef CONFIG_SEC_FACTORY
		psy_do_property(battery->pdata->fuelgauge_name, get,
				POWER_SUPPLY_PROP_VOLTAGE_NOW, value);
		battery->voltage_now = value.intval;
		dev_err(battery->dev,
			"%s: voltage now(%d)\n", __func__, battery->voltage_now);
#endif
		/* voltage value should be in uV */
		val->intval = battery->voltage_now * 1000;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_AVG:
#ifdef CONFIG_SEC_FACTORY
		value.intval = SEC_BATTERY_VOLTAGE_AVERAGE;
		psy_do_property(battery->pdata->fuelgauge_name, get,
				POWER_SUPPLY_PROP_VOLTAGE_AVG, value);
		battery->voltage_avg = value.intval;
		dev_err(battery->dev,
			"%s: voltage avg(%d)\n", __func__, battery->voltage_avg);
#endif
		/* voltage value should be in uV */
		val->intval = battery->voltage_avg * 1000;
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		val->intval = battery->current_now;
		break;	
	case POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION:
		val->intval = battery->cable_type;
		pr_info("%s cable type = %d \n", __func__, val->intval);
		break;
	case POWER_SUPPLY_PROP_CURRENT_AVG:
		val->intval = battery->current_avg;
		break;
	/* charging mode (differ from power supply) */
	case POWER_SUPPLY_PROP_CHARGE_NOW:
		val->intval = battery->charging_mode;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		if (battery->pdata->fake_capacity) {
			val->intval = 90;
			pr_info("%s : capacity(%d)\n", __func__, val->intval);
		} else {
#if defined(CONFIG_SAMSUNG_BATTERY_ENG_TEST)
			if (battery->status == POWER_SUPPLY_STATUS_FULL) {
				if(battery->eng_not_full_status)
					val->intval = battery->capacity;
				else
					val->intval = 100;
			} else {
				val->intval = battery->capacity;
			}
#else
#if defined(CONFIG_AFC_CHARGER_MODE)
			val->intval = battery->capacity;
#else
			/* In full-charged status, SOC is always 100% */
			if (battery->status == POWER_SUPPLY_STATUS_FULL)
				val->intval = 100;
			else
				val->intval = battery->capacity;
#endif
#endif
		}
		break;
	case POWER_SUPPLY_PROP_TEMP:
		val->intval = battery->temperature;
		break;
	case POWER_SUPPLY_PROP_TEMP_AMBIENT:
		val->intval = battery->temper_amb;
		break;
	case POWER_SUPPLY_PROP_POWER_NOW:
		val->intval = battery->current_isys_now;
		break;
	case POWER_SUPPLY_PROP_POWER_AVG:
#if defined(CONFIG_FUELGAUGE_MAX77833) && !defined(CONFIG_FUELGAUGE_MAX77843)
		value.intval = SEC_BATTERY_CURRENT_MA;
		psy_do_property(battery->pdata->fuelgauge_name, get,
			POWER_SUPPLY_PROP_POWER_AVG, value);
		battery->current_isys_avg= value.intval;
#endif
		val->intval = battery->current_isys_avg;
		break;
#if defined(CONFIG_CALC_TIME_TO_FULL)
	case POWER_SUPPLY_PROP_TIME_TO_FULL_NOW:
		if (battery->capacity == 100 && !lpcharge) {
			val->intval = -1;
			break;
		}

		if (((battery->status == POWER_SUPPLY_STATUS_CHARGING) ||
			(battery->status == POWER_SUPPLY_STATUS_FULL && battery->capacity != 100)) &&
			battery->complete_timetofull &&
			!battery->swelling_mode)
			val->intval = battery->timetofull;
		else
			val->intval = -1;
		break;
#endif
#if defined(CONFIG_BATTERY_SWELLING)
	case POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT:
		if (battery->swelling_mode)
			val->intval = 1;
		else
			val->intval = 0;
		break;
#endif
	case POWER_SUPPLY_PROP_CHARGE_COUNTER_SHADOW:
		val->intval = battery->wire_status;
		break;
	case POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL:
		/*psy_do_property(battery->pdata->charger_name, get,
						POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, value);*/
		break;
	case POWER_SUPPLY_PROP_INPUT_CURRENT_MAX:
		val->intval = battery->charging_mode;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int sec_usb_get_property(struct power_supply *psy,
				enum power_supply_property psp,
				union power_supply_propval *val)
{
	struct sec_battery_info *battery =
		container_of(psy, struct sec_battery_info, psy_usb);

	if (psp != POWER_SUPPLY_PROP_ONLINE)
		return -EINVAL;

	if ((battery->health == POWER_SUPPLY_HEALTH_OVERVOLTAGE) ||
	    (battery->health == POWER_SUPPLY_HEALTH_UNDERVOLTAGE) ||
	    (battery->pdata->camera_temp_check && battery->camera_limit)){
			val->intval = 0;
			return 0;
	}
	/* Set enable=1 only if the USB charger is connected */
	switch (battery->wire_status) {
	case POWER_SUPPLY_TYPE_USB:
	case POWER_SUPPLY_TYPE_USB_DCP:
	case POWER_SUPPLY_TYPE_USB_CDP:
	case POWER_SUPPLY_TYPE_USB_ACA:
	case POWER_SUPPLY_TYPE_MHL_USB:
	case POWER_SUPPLY_TYPE_MHL_USB_100:
		val->intval = 1;
		break;
	default:
		val->intval = 0;
		break;
	}

	if (battery->slate_mode)
		val->intval = 0;
	return 0;
}

static int sec_ac_get_property(struct power_supply *psy,
			       enum power_supply_property psp,
			       union power_supply_propval *val)
{
	struct sec_battery_info *battery =
		container_of(psy, struct sec_battery_info, psy_ac);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
	if ((battery->health == POWER_SUPPLY_HEALTH_OVERVOLTAGE) ||
	    (battery->health == POWER_SUPPLY_HEALTH_UNDERVOLTAGE) ||
	    (battery->pdata->camera_temp_check && battery->camera_limit)){
			val->intval = 0;
			return 0;
		}

		/* Set enable=1 only if the AC charger is connected */
		switch (battery->cable_type) {
			case POWER_SUPPLY_TYPE_MAINS:
			case POWER_SUPPLY_TYPE_MISC:
			case POWER_SUPPLY_TYPE_CARDOCK:
			case POWER_SUPPLY_TYPE_UARTOFF:
			case POWER_SUPPLY_TYPE_LAN_HUB:
			case POWER_SUPPLY_TYPE_UNKNOWN:
			case POWER_SUPPLY_TYPE_MHL_500:
			case POWER_SUPPLY_TYPE_MHL_900:
			case POWER_SUPPLY_TYPE_MHL_1500:
			case POWER_SUPPLY_TYPE_MHL_2000:
			case POWER_SUPPLY_TYPE_SMART_OTG:
			case POWER_SUPPLY_TYPE_SMART_NOTG:
			case POWER_SUPPLY_TYPE_HV_PREPARE_MAINS:
			case POWER_SUPPLY_TYPE_HV_ERR:
			case POWER_SUPPLY_TYPE_HV_UNKNOWN:
			case POWER_SUPPLY_TYPE_HV_MAINS:
			case POWER_SUPPLY_TYPE_MDOCK_TA:
			case POWER_SUPPLY_TYPE_HMT_CONNECTED:
			case POWER_SUPPLY_TYPE_HMT_CHARGE:
			case POWER_SUPPLY_TYPE_HV_MAINS_CHG_LIMIT:
				val->intval = 1;
				break;
			default:
				val->intval = 0;
				break;
		}
		break;
	case POWER_SUPPLY_PROP_TEMP:
		val->intval = battery->chg_temp;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int sec_wireless_get_property(struct power_supply *psy,
			       enum power_supply_property psp,
			       union power_supply_propval *val)
{
	struct sec_battery_info *battery =
		container_of(psy, struct sec_battery_info, psy_wireless);

	if (psp != POWER_SUPPLY_PROP_ONLINE)
		return -EINVAL;

	if (battery->pdata->camera_temp_check && battery->camera_limit) {
		val->intval = 0;
		return 0;
	}
	val->intval = battery->wc_status;

	return 0;
}

static int sec_wireless_set_property(struct power_supply *psy,
				enum power_supply_property psp,
				const union power_supply_propval *val)
{
	struct sec_battery_info *battery =
		container_of(psy, struct sec_battery_info, psy_wireless);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		battery->wc_status = val->intval;

		wake_lock(&battery->cable_wake_lock);
		queue_delayed_work_on(0, battery->monitor_wqueue,
			&battery->cable_work, 0);
		break;
#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY)
	case POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL:
		if (battery->wc_status) {
			union power_supply_propval value;
			value.intval = val->intval;
			psy_do_property(battery->pdata->wireless_charger_name, set,
					POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, value);
		}
		break;
#endif
	default:
		return -EINVAL;
	}

	return 0;
}

static int sec_ps_set_property(struct power_supply *psy,
				enum power_supply_property psp,
				const union power_supply_propval *val)
{
	struct sec_battery_info *battery =
		container_of(psy, struct sec_battery_info, psy_ps);
	union power_supply_propval value;

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
	if (val->intval == 0) {
		if (battery->ps_enable == true) {
			battery->ps_enable = val->intval;
				dev_info(battery->dev,
					"%s: power sharing cable set (%d)\n", __func__, battery->ps_enable);
			value.intval = POWER_SUPPLY_TYPE_POWER_SHARING;
			psy_do_property(battery->pdata->charger_name, set,
				POWER_SUPPLY_PROP_ONLINE, value);
		}
	} else if ((val->intval == 1) && (battery->ps_status == true)) {
		battery->ps_enable = val->intval;
			dev_info(battery->dev,
				"%s: power sharing cable set (%d)\n", __func__, battery->ps_enable);
		value.intval = POWER_SUPPLY_TYPE_POWER_SHARING;
		psy_do_property(battery->pdata->charger_name, set,
			POWER_SUPPLY_PROP_ONLINE, value);
	} else {
		dev_err(battery->dev,
			"%s: invalid setting (%d) ps_status (%d)\n",
			__func__, val->intval, battery->ps_status);
	}
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		if (val->intval == POWER_SUPPLY_TYPE_POWER_SHARING) {
			battery->ps_status = true;
			battery->ps_enable = true;
			battery->ps_changed = true;
			dev_info(battery->dev,
				"%s: power sharing cable plugin (%d)\n", __func__, battery->ps_status);
			wake_lock(&battery->monitor_wake_lock);
			queue_delayed_work_on(0, battery->monitor_wqueue, &battery->monitor_work, 0);
		} else {
			battery->ps_status = false;
			battery->ps_enable = false;
			battery->ps_changed = false;
			dev_info(battery->dev,
				"%s: power sharing cable plugout (%d)\n", __func__, battery->ps_status);
			wake_lock(&battery->monitor_wake_lock);
			queue_delayed_work_on(0, battery->monitor_wqueue, &battery->monitor_work, 0);
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int sec_ps_get_property(struct power_supply *psy,
			       enum power_supply_property psp,
			       union power_supply_propval *val)
{
	struct sec_battery_info *battery =
		container_of(psy, struct sec_battery_info, psy_ps);
	union power_supply_propval value;

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		if (battery->ps_enable)
			val->intval = 1;
		else
			val->intval = 0;
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		if (battery->ps_status) {
			if ((battery->ps_enable == true) && (battery->ps_changed == true)) {
				battery->ps_changed = false;

				value.intval = POWER_SUPPLY_TYPE_POWER_SHARING;
				psy_do_property(battery->pdata->charger_name, set,
						POWER_SUPPLY_PROP_ONLINE, value);
			}
			val->intval = 1;
		} else {
			if (battery->ps_enable == true) {
				battery->ps_enable = false;
				dev_info(battery->dev,
					"%s: power sharing cable disconnected! ps disable (%d)\n",
					__func__, battery->ps_enable);

				value.intval = POWER_SUPPLY_TYPE_POWER_SHARING;
				psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_ONLINE, value);
			}
			val->intval = 0;
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/* TODO unused */
#if 0
static irqreturn_t sec_bat_irq_thread(int irq, void *irq_data)
{
	struct sec_battery_info *battery = irq_data;

	dev_info(battery->dev, "%s:(bat_irq occured_start\n", __func__);

	if (battery->pdata->cable_check_type &
		SEC_BATTERY_CABLE_CHECK_INT) {
		if (battery->pdata->is_interrupt_cable_check_possible &&
			!battery->pdata->is_interrupt_cable_check_possible(
			battery->extended_cable_type))
				goto no_cable_check;
		else {
		if (sec_bat_get_cable_type(battery,
			battery->pdata->cable_source_type)) {
			wake_lock(&battery->cable_wake_lock);
			queue_delayed_work(battery->monitor_wqueue, &battery->monitor_work, 0);
	}

	return IRQ_HANDLED;
}

no_cable_check:
	if (battery->pdata->battery_check_type ==
		SEC_BATTERY_CHECK_INT) {
		if (battery_pdata->check_battery_callback)
			battery->present = battery->pdata->check_battery_callback();

		wake_lock(&battery->monitor_wake_lock);
		queue_delayed_work_on(0, battery->monitor_wqueue, &battery->monitor_work, 0);
	}

	return IRQ_HANDLED;
}
#endif

#if defined(CONFIG_MUIC_NOTIFIER)
static int sec_bat_cable_check(struct sec_battery_info *battery,
				muic_attached_dev_t attached_dev)
{
	int current_cable_type = -1;
	union power_supply_propval val;

	pr_info("[%s]ATTACHED(%d)\n", __func__, attached_dev);

	switch (attached_dev)
	{
	case ATTACHED_DEV_JIG_UART_OFF_MUIC:
	case ATTACHED_DEV_JIG_UART_ON_MUIC:
		battery->is_jig_on = true;
	case ATTACHED_DEV_UNDEFINED_RANGE_MUIC:
	case ATTACHED_DEV_SMARTDOCK_MUIC:
	case ATTACHED_DEV_DESKDOCK_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_BATTERY;
		break;
	case ATTACHED_DEV_OTG_MUIC:
	case ATTACHED_DEV_JIG_UART_OFF_VB_OTG_MUIC:
	case ATTACHED_DEV_HMT_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_OTG;
		break;
	case ATTACHED_DEV_USB_MUIC:
	case ATTACHED_DEV_JIG_USB_OFF_MUIC:
	case ATTACHED_DEV_JIG_USB_ON_MUIC:
	case ATTACHED_DEV_SMARTDOCK_USB_MUIC:
	case ATTACHED_DEV_UNOFFICIAL_ID_USB_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_USB;
		break;
	case ATTACHED_DEV_JIG_UART_OFF_VB_MUIC:
	case ATTACHED_DEV_JIG_UART_OFF_VB_FG_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_UARTOFF;
		break;
	case ATTACHED_DEV_TA_MUIC:
	case ATTACHED_DEV_CARDOCK_MUIC:
	case ATTACHED_DEV_DESKDOCK_VB_MUIC:
	case ATTACHED_DEV_SMARTDOCK_TA_MUIC:
	case ATTACHED_DEV_UNOFFICIAL_TA_MUIC:
	case ATTACHED_DEV_UNOFFICIAL_ID_TA_MUIC:
	case ATTACHED_DEV_UNOFFICIAL_ID_ANY_MUIC:
	case ATTACHED_DEV_UNSUPPORTED_ID_VB_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_MAINS;
		break;
	case ATTACHED_DEV_QC_CHARGER_5V_MUIC:
	case ATTACHED_DEV_AFC_CHARGER_5V_MUIC:
		if (is_hv_wire_type(battery->cable_type))
			current_cable_type = POWER_SUPPLY_TYPE_HV_MAINS_CHG_LIMIT;
		else
			current_cable_type = POWER_SUPPLY_TYPE_MAINS;
		break;
	case ATTACHED_DEV_CDP_MUIC:
	case ATTACHED_DEV_UNOFFICIAL_ID_CDP_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_USB_CDP;
		break;
	case ATTACHED_DEV_USB_LANHUB_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_LAN_HUB;
		break;
	case ATTACHED_DEV_CHARGING_CABLE_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_POWER_SHARING;
		break;
	case ATTACHED_DEV_AFC_CHARGER_PREPARE_MUIC:
	case ATTACHED_DEV_QC_CHARGER_PREPARE_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_HV_PREPARE_MAINS;
		break;
	case ATTACHED_DEV_AFC_CHARGER_9V_MUIC:
	case ATTACHED_DEV_QC_CHARGER_9V_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_HV_MAINS;
		break;
	case ATTACHED_DEV_AFC_CHARGER_ERR_V_MUIC:
	case ATTACHED_DEV_QC_CHARGER_ERR_V_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_HV_ERR;
		break;
	case ATTACHED_DEV_UNDEFINED_CHARGING_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_MAINS;
		break;
	case ATTACHED_DEV_HV_ID_ERR_UNDEFINED_MUIC:
	case ATTACHED_DEV_HV_ID_ERR_UNSUPPORTED_MUIC:
	case ATTACHED_DEV_HV_ID_ERR_SUPPORTED_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_HV_UNKNOWN;
		break;
	case ATTACHED_DEV_VZW_INCOMPATIBLE_MUIC:
		current_cable_type = POWER_SUPPLY_TYPE_UNKNOWN;
		break;
	case ATTACHED_DEV_TIMEOUT_OPEN_MUIC: /* D+, D- open status */
		if (battery->wc_status)
			current_cable_type = -1;
		else
			current_cable_type = POWER_SUPPLY_TYPE_USB;
		break;
	default:
		pr_err("%s: invalid type for charger:%d\n",
			__func__, attached_dev);
	}

	if (battery->is_jig_on)
		psy_do_property(battery->pdata->fuelgauge_name, set,
			POWER_SUPPLY_PROP_ENERGY_NOW, val);

	return current_cable_type;

}

static int batt_handle_notification(struct notifier_block *nb,
		unsigned long action, void *data)
{
	muic_attached_dev_t attached_dev = *(muic_attached_dev_t *)data;
	const char *cmd;
	int cable_type;
	struct sec_battery_info *battery =
		container_of(nb, struct sec_battery_info,
			     batt_nb);
	union power_supply_propval value;

	switch (action) {
	case MUIC_NOTIFY_CMD_DETACH:
	case MUIC_NOTIFY_CMD_LOGICALLY_DETACH:
		cmd = "DETACH";
		battery->is_jig_on = false;
		cable_type = POWER_SUPPLY_TYPE_BATTERY;
		battery->muic_cable_type = ATTACHED_DEV_NONE_MUIC;
		battery->usb_3s_nodevice = false;
		break;
	case MUIC_NOTIFY_CMD_ATTACH:
	case MUIC_NOTIFY_CMD_LOGICALLY_ATTACH:
		cmd = "ATTACH";
		cable_type = sec_bat_cable_check(battery, attached_dev);
		battery->muic_cable_type = attached_dev;
		break;
	default:
		cmd = "ERROR";
		cable_type = -1;
		battery->muic_cable_type = ATTACHED_DEV_NONE_MUIC;
		break;
	}

	sec_bat_set_misc_event(battery, BATT_MISC_EVENT_UNDEFINED_RANGE_TYPE,
		(battery->muic_cable_type != ATTACHED_DEV_UNDEFINED_RANGE_MUIC));

	if (attached_dev == ATTACHED_DEV_MHL_MUIC)
		return 0;

	if (cable_type < 0) {
		dev_info(battery->dev, "%s: ignore event(%d)\n",
			__func__, cable_type);
	} else if (cable_type == POWER_SUPPLY_TYPE_POWER_SHARING) {
		battery->ps_status = true;
		battery->ps_enable = true;
		battery->ps_changed = true;
		battery->wire_status = cable_type;
		dev_info(battery->dev,
			"%s: power sharing cable plugin (%d)\n", __func__, battery->ps_status);
	} else if (cable_type == POWER_SUPPLY_TYPE_WIRELESS) {
		battery->wc_status = SEC_WIRELESS_PAD_WPC;
	} else if (cable_type == POWER_SUPPLY_TYPE_HV_WIRELESS) {
		battery->wc_status = SEC_WIRELESS_PAD_WPC_HV;
	} else if (cable_type == POWER_SUPPLY_TYPE_PMA_WIRELESS) {
		battery->wc_status = SEC_WIRELESS_PAD_PMA;
	} else if ((cable_type == POWER_SUPPLY_TYPE_UNKNOWN) &&
		   (battery->status != POWER_SUPPLY_STATUS_DISCHARGING)) {
		battery->cable_type = cable_type;
		wake_lock(&battery->monitor_wake_lock);
		queue_delayed_work_on(0, battery->monitor_wqueue, &battery->monitor_work, 0);
		dev_info(battery->dev,
			"%s: UNKNOWN cable plugin\n", __func__);
		return 0;
	} else {
		battery->wire_status = cable_type;
		if ((battery->wire_status == POWER_SUPPLY_TYPE_BATTERY)
				&& battery->wc_status && !battery->ps_status)
			cable_type = POWER_SUPPLY_TYPE_WIRELESS;
	}
	dev_info(battery->dev,
			"%s: current_cable(%d), wc_status(%d), wire_status(%d)\n",
			__func__, cable_type, battery->wc_status,
			battery->wire_status);

	if (attached_dev == ATTACHED_DEV_USB_LANHUB_MUIC) {
		if (!strcmp(cmd, "ATTACH")) {
			value.intval = true;
			psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_CHARGE_POWERED_OTG_CONTROL,
					value);
			dev_info(battery->dev,
				"%s: Powered OTG cable attached\n", __func__);
		} else {
			value.intval = false;
			psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_CHARGE_POWERED_OTG_CONTROL,
					value);
			dev_info(battery->dev,
				"%s: Powered OTG cable detached\n", __func__);
		}
	}

#if defined(CONFIG_AFC_CHARGER_MODE)
	if (!strcmp(cmd, "ATTACH")) {
		if ((battery->muic_cable_type >= ATTACHED_DEV_QC_CHARGER_PREPARE_MUIC) &&
		    (battery->muic_cable_type <= ATTACHED_DEV_QC_CHARGER_9V_MUIC))
			battery->hv_chg_name = "QC";
		else if ((battery->muic_cable_type >= ATTACHED_DEV_AFC_CHARGER_PREPARE_MUIC) &&
			 (battery->muic_cable_type <= ATTACHED_DEV_AFC_CHARGER_ERR_V_DUPLI_MUIC))
			battery->hv_chg_name = "AFC";
		else
			battery->hv_chg_name = "NONE";
	} else {
			battery->hv_chg_name = "NONE";
	}

	pr_info("%s : HV_CHARGER_NAME(%s)\n",
		__func__, battery->hv_chg_name);
#endif

	if ((cable_type >= 0) &&
	    cable_type <= SEC_SIZEOF_POWER_SUPPLY_TYPE) {
		if((cable_type == POWER_SUPPLY_TYPE_BATTERY) && battery->ps_status) {
			battery->ps_status = false;
			dev_info(battery->dev,
				"%s: power sharing cable plugout (%d)\n", __func__, battery->ps_status);
			wake_lock(&battery->cable_wake_lock);
			queue_delayed_work_on(0, battery->monitor_wqueue, &battery->cable_work, 0);
		} else {
			wake_lock(&battery->cable_wake_lock);
			queue_delayed_work_on(0, battery->monitor_wqueue,
					   &battery->cable_work, 0);
		}
	}

	pr_info("%s: CMD=%s, attached_dev=%d\n", __func__, cmd, attached_dev);

	return 0;
}
#endif /* CONFIG_MUIC_NOTIFIER */

#if defined(CONFIG_VBUS_NOTIFIER)
static int vbus_handle_notification(struct notifier_block *nb,
		unsigned long action, void *data)
{
	vbus_status_t vbus_status = *(vbus_status_t *)data;
	struct sec_battery_info *battery =
		container_of(nb, struct sec_battery_info,
			     vbus_nb);
	union power_supply_propval value;

	if (battery->muic_cable_type == ATTACHED_DEV_HMT_MUIC &&
		battery->muic_vbus_status != vbus_status &&
		battery->muic_vbus_status == STATUS_VBUS_HIGH &&
		vbus_status == STATUS_VBUS_LOW) {
		sec_bat_set_charge(battery, false);
		msleep(500);
		if (!battery->usb_3s_nodevice) {
			value.intval = true;
			psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL,
					value);
			dev_info(battery->dev,
				"%s: changed to OTG cable attached\n", __func__);

			battery->wire_status = POWER_SUPPLY_TYPE_OTG;
			wake_lock(&battery->cable_wake_lock);
			queue_delayed_work_on(0, battery->monitor_wqueue, &battery->cable_work, 0);
		}
	}
	pr_info("%s: action=%d, vbus_status=%d\n", __func__, (int)action, vbus_status);
	battery->muic_vbus_status = vbus_status;

	return 0;
}
#endif

#if defined(CONFIG_USB_EXTERNAL_NOTIFY)
static int usb_handle_notification(struct notifier_block *nb,
		unsigned long action, void *data)
{
	struct sec_battery_info *battery =
		container_of(nb, struct sec_battery_info, usb_nb);

	switch(action) {
	case EXTERNAL_NOTIFY_3S_NODEVICE:		
		if (battery->muic_cable_type == ATTACHED_DEV_HMT_MUIC) {
			pr_info("%s: 3S_NODEVICE(USB HOST Connection timeout)\n", __func__);
			battery->usb_3s_nodevice = true;
		}
		break;
	case EXTERNAL_NOTIFY_DEVICE_CONNECT:
		break;
	default:
		break;
	}
	return 0;
}
#endif

#ifdef CONFIG_OF
static int sec_bat_parse_dt(struct device *dev,
		struct sec_battery_info *battery)
{
	struct device_node *np = dev->of_node;
	sec_battery_platform_data_t *pdata = battery->pdata;
	int ret, len;
	unsigned int i;
	const u32 *p;
	u32 temp;
#if defined(CONFIG_SEC_FACTORY) || defined(CONFIG_WIRELESS_FIRMWARE_UPDATE)
	enum of_gpio_flags irq_gpio_flags;
#endif
	if (!np) {
		pr_info("%s: np NULL\n", __func__);
		return 1;
	}

#if defined(CONFIG_BATTERY_CISD)	
	ret = of_property_read_u32(np, "battery,cisd_cap_high_thr",
		&pdata->cisd_cap_high_thr);
	if (ret) {
		pr_info("%s : cisd_cap_high_thr is Empty\n", __func__);
	}
	ret = of_property_read_u32(np, "battery,cisd_cap_low_thr",
		&pdata->cisd_cap_low_thr);
	if (ret) {
		pr_info("%s : cisd_cap_low_thr is Empty\n", __func__);
	}
	ret = of_property_read_u32(np, "battery,cisd_cap_limit",
		&pdata->cisd_cap_limit);
	if (ret) {
		pr_info("%s : cisd_cap_limit is Empty\n", __func__);
	}
#endif

	pdata->wchg_ctl_en = of_property_read_bool(np,
						   "battery,wchg_ctl_en");
	if (pdata->wchg_ctl_en)
		pdata->wchg_ctl = of_get_named_gpio(np, "battery,wchg_ctl", 0);

	ret = of_property_read_string(np,
		"battery,vendor", (char const **)&pdata->vendor);
	if (ret)
		pr_info("%s: Vendor is Empty\n", __func__);

	ret = of_property_read_string(np,
		"battery,charger_name", (char const **)&pdata->charger_name);
	if (ret)
		pr_info("%s: Charger name is Empty\n", __func__);

	ret = of_property_read_string(np,
		"battery,fuelgauge_name", (char const **)&pdata->fuelgauge_name);
	if (ret)
		pr_info("%s: Fuelgauge name is Empty\n", __func__);

	ret = of_property_read_string(np,
		"battery,wirelss_charger_name", (char const **)&pdata->wireless_charger_name);
	if (ret)
		pr_info("%s: Wirelss charger name is Empty\n", __func__);

	ret = of_property_read_string(np,
		"battery,chip_vendor", (char const **)&pdata->chip_vendor);
	if (ret)
		pr_info("%s: Chip vendor is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,technology",
		&pdata->technology);
	if (ret)
		pr_info("%s : technology is Empty\n", __func__);

	ret = of_property_read_u32(np,
		"battery,wireless_cc_cv", &pdata->wireless_cc_cv);

	pdata->waterproof = of_property_read_bool(np, "battery,waterproof");

	pdata->fake_capacity = of_property_read_bool(np,
						     "battery,fake_capacity");

	pdata->event_check = of_property_read_bool(np,
						   "battery,event_check");

	p = of_get_property(np, "battery,polling_time", &len);
	if (!p)
		return 1;

	len = len / sizeof(u32);
	pdata->polling_time = kzalloc(sizeof(*pdata->polling_time) * len, GFP_KERNEL);
	ret = of_property_read_u32_array(np, "battery,polling_time",
					 pdata->polling_time, len);

	ret = of_property_read_u32(np, "battery,thermal_source",
		&pdata->thermal_source);
	if (ret)
		pr_info("%s : Thermal source is Empty\n", __func__);

	if (pdata->thermal_source == SEC_BATTERY_THERMAL_SOURCE_ADC) {
		p = of_get_property(np, "battery,temp_table_adc", &len);
		if (!p)
			return 1;

		len = len / sizeof(u32);

		pdata->temp_adc_table_size = len;
		pdata->temp_amb_adc_table_size = len;

		pdata->temp_adc_table =
			kzalloc(sizeof(sec_bat_adc_table_data_t) *
				pdata->temp_adc_table_size, GFP_KERNEL);
		pdata->temp_amb_adc_table =
			kzalloc(sizeof(sec_bat_adc_table_data_t) *
				pdata->temp_adc_table_size, GFP_KERNEL);

		for(i = 0; i < pdata->temp_adc_table_size; i++) {
			ret = of_property_read_u32_index(np,
					 "battery,temp_table_adc", i, &temp);
			pdata->temp_adc_table[i].adc = (int)temp;
			if (ret)
				pr_info("%s : Temp_adc_table(adc) is Empty\n",
					__func__);

			ret = of_property_read_u32_index(np,
							 "battery,temp_table_data", i, &temp);
			pdata->temp_adc_table[i].data = (int)temp;
			if (ret)
				pr_info("%s : Temp_adc_table(data) is Empty\n",
					__func__);

			ret = of_property_read_u32_index(np,
							 "battery,temp_table_adc", i, &temp);
			pdata->temp_amb_adc_table[i].adc = (int)temp;
			if (ret)
				pr_info("%s : Temp_amb_adc_table(adc) is Empty\n",
					__func__);

			ret = of_property_read_u32_index(np,
							 "battery,temp_table_data", i, &temp);
			pdata->temp_amb_adc_table[i].data = (int)temp;
			if (ret)
				pr_info("%s : Temp_amb_adc_table(data) is Empty\n",
					__func__);
		}

		/* chg temp adc */
		p = of_get_property(np, "battery,chg_temp_table_adc", &len);
		if (!p)
			return 1;

		len = len / sizeof(u32);

		pdata->chg_temp_adc_table_size = len;

		pdata->chg_temp_adc_table =
			kzalloc(sizeof(sec_bat_adc_table_data_t) *
				pdata->chg_temp_adc_table_size, GFP_KERNEL);

		for(i = 0; i < pdata->chg_temp_adc_table_size; i++) {
			ret = of_property_read_u32_index(np,
							 "battery,chg_temp_table_adc", i, &temp);
			pdata->chg_temp_adc_table[i].adc = (int)temp;
			if (ret)
				pr_info("%s : CHG_Temp_adc_table(adc) is Empty\n",
					__func__);

			ret = of_property_read_u32_index(np,
							 "battery,chg_temp_table_data", i, &temp);
			pdata->chg_temp_adc_table[i].data = (int)temp;
			if (ret)
				pr_info("%s : CHG_Temp_adc_table(data) is Empty\n",
					__func__);
		}

		/* wpc temp adc */
		p = of_get_property(np, "battery,wpc_temp_table_adc", &len);
		if (!p) {
			pr_info("%s : discharge_Temp_adc_table(adc) is Empty\n",__func__);
		} else {
			len = len / sizeof(u32);

			pdata->wpc_temp_adc_table_size = len;

			pdata->wpc_temp_adc_table =
				kzalloc(sizeof(sec_bat_adc_table_data_t) *
					pdata->wpc_temp_adc_table_size, GFP_KERNEL);

			for(i = 0; i < pdata->wpc_temp_adc_table_size; i++) {
				ret = of_property_read_u32_index(np,
								 "battery,wpc_temp_table_adc", i, &temp);
				pdata->wpc_temp_adc_table[i].adc = (int)temp;
				if (ret)
					pr_info("%s : WPC_Temp_adc_table(adc) is Empty\n",
						__func__);

				ret = of_property_read_u32_index(np,
								 "battery,wpc_temp_table_data", i, &temp);
				pdata->wpc_temp_adc_table[i].data = (int)temp;
				if (ret)
					pr_info("%s : WPC_Temp_adc_table(data) is Empty\n",
						__func__);
			}
		}
	}

	pdata->camera_temp_check = of_property_read_bool(np,
							 "battery,camera_temp_check");

	if (pdata->camera_temp_check) {
		p = of_get_property(np, "battery,camera_temp_table_adc", &len);
		if (!p)
			return 1;

		len = len / sizeof(u32);

		pdata->camera_temp_adc_table_size = len;

		pdata->camera_temp_adc_table =
			kzalloc(sizeof(sec_bat_adc_table_data_t) *
				pdata->camera_temp_adc_table_size, GFP_KERNEL);

		for(i = 0; i < pdata->camera_temp_adc_table_size; i++) {
			ret = of_property_read_u32_index(np,
							 "battery,camera_temp_table_adc", i, &temp);
			pdata->camera_temp_adc_table[i].adc = (int)temp;
			if (ret)
				pr_info("%s : CAMERA_Temp_adc_table(adc) is Empty\n",
					__func__);

			ret = of_property_read_u32_index(np,
							 "battery,camera_temp_table_data", i, &temp);
			pdata->camera_temp_adc_table[i].data = (int)temp;
			if (ret)
				pr_info("%s : CAMERA_Temp_adc_table(data) is Empty\n",
					__func__);
		}

		ret = of_property_read_u32(np, "battery,camera_temp_limit",
					   &pdata->camera_temp_limit);
		if (ret)
			pr_info("%s : camera_temp_limit is Empty\n", __func__);
	}

	ret = of_property_read_u32(np, "battery,chg_temp_check",
		&pdata->chg_temp_check);
	if (ret)
		pr_info("%s : chg_temp_check is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,wpc_temp_check",
		&pdata->wpc_temp_check);
	if (ret)
		pr_info("%s : wpc_temp_check is Empty\n", __func__);

	if (pdata->chg_temp_check) {
		ret = of_property_read_u32(np, "battery,chg_high_temp_1st",
					   &temp);
		pdata->chg_high_temp_1st = (int)temp;
		if (ret)
			pr_info("%s : chg_high_temp_threshold is Empty\n", __func__);

		ret = of_property_read_u32(np, "battery,chg_high_temp_2nd",
					   &temp);
		pdata->chg_high_temp_2nd = (int)temp;
		if (ret)
			pr_info("%s : chg_high_temp_threshold is Empty\n", __func__);

		ret = of_property_read_u32(np, "battery,chg_high_temp_recovery",
					   &temp);
		pdata->chg_high_temp_recovery = (int)temp;
		if (ret)
			pr_info("%s : chg_temp_recovery is Empty\n", __func__);

		ret = of_property_read_u32(np, "battery,chg_charging_limit_current",
					   &pdata->chg_charging_limit_current);
		if (ret)
			pr_info("%s : chg_charging_limit_current is Empty\n", __func__);

		ret = of_property_read_u32(np, "battery,chg_charging_limit_current_2nd",
					   &pdata->chg_charging_limit_current_2nd);
		if (ret)
			pr_info("%s : chg_charging_limit_current_2nd is Empty\n", __func__);

		ret = of_property_read_u32(np, "battery,chg_skip_check_time",
					   &pdata->chg_skip_check_time);
		if (ret)
			pr_info("%s : chg_skip_check_time is Empty\n", __func__);

		ret = of_property_read_u32(np, "battery,chg_skip_check_capacity",
					   &pdata->chg_skip_check_capacity);
		if (ret)
			pr_info("%s : chg_skip_check_capacity is Empty\n", __func__);
	}

	ret = of_property_read_u32(np, "battery,sleep_mode_limit_current",
			   &pdata->sleep_mode_limit_current);
	if (ret)
		pr_info("%s : sleep_mode_limit_current is Empty\n", __func__);

	if (pdata->wpc_temp_check) {
		ret = of_property_read_u32(np, "battery,wpc_high_temp", &temp);
		pdata->wpc_high_temp = (int)temp;
		if (ret)
			pr_info("%s : wpc_high_temp is Empty\n", __func__);

		ret = of_property_read_u32(np, "battery,wpc_high_temp_recovery", &temp);
		pdata->wpc_high_temp_recovery = (int)temp;
		if (ret)
			pr_info("%s : wpc_high_temp_recovery is Empty\n", __func__);

		ret = of_property_read_u32(np, "battery,wpc_charging_limit_current",
				   &pdata->wpc_charging_limit_current);
		if (ret)
			pr_info("%s : wpc_charging_limit_current is Empty\n", __func__);

		ret = of_property_read_u32(np, "battery,wpc_lcd_on_high_temp",
					   &pdata->wpc_lcd_on_high_temp);
		if (ret)
			pr_info("%s : wpc_lcd_on_high_temp is Empty\n", __func__);

		ret = of_property_read_u32(np, "battery,wpc_lcd_on_high_temp_rec",
					   &pdata->wpc_lcd_on_high_temp_rec);
		if (ret)
			pr_info("%s : wpc_lcd_on_high_temp_rec is Empty\n", __func__);
	}

	ret = of_property_read_u32(np, "battery,mix_temp_check",
		&pdata->mix_temp_check);
	if (ret)
		pr_info("%s : mix_temp_check is Empty\n", __func__);

	if (pdata->mix_temp_check) {
		ret = of_property_read_u32(np, "battery,mix_high_tbat",
			&pdata->mix_high_tbat);
		if (ret) {
			pdata->mix_high_tbat = 999;
			pr_info("%s : bat_high_temp is Empty\n", __func__);
		}

		ret = of_property_read_u32(np, "battery,mix_high_tchg",
			&pdata->mix_high_tchg);
		if (ret) {
			pdata->mix_high_tchg = 999;
			pr_info("%s : mix_high_tchg is Empty\n", __func__);
		}		

		ret = of_property_read_u32(np, "battery,mix_high_tbat_recov",
			&pdata->mix_high_tbat_recov);
		if (ret) {
			pdata->mix_high_tbat_recov = 999;
			pr_info("%s : mix_high_tbat_recov is Empty\n", __func__);
		}

		ret = of_property_read_u32(np, "battery,mix_input_limit_current",
			&pdata->mix_input_limit_current);
		if (ret) {
			pdata->mix_input_limit_current = 1800;
			pr_info("%s : mix_input_limit_current is Empty\n", __func__);
		}

		ret = of_property_read_u32(np, "battery,mix_high_tbat_hv",
			&pdata->mix_high_tbat_hv);
		if (ret) {
			pdata->mix_high_tbat_hv = 999;
			pr_info("%s : bat_high_temp_hv is Empty\n", __func__);
		}

		ret = of_property_read_u32(np, "battery,mix_high_tchg_hv",
			&pdata->mix_high_tchg_hv);
		if (ret) {
			pdata->mix_high_tchg_hv = 999;
			pr_info("%s : mix_high_tchg_hv is Empty\n", __func__);
		}

		ret = of_property_read_u32(np, "battery,mix_high_tbat_recov_hv",
			&pdata->mix_high_tbat_recov_hv);
		if (ret) {
			pdata->mix_high_tbat_recov_hv = 999;
			pr_info("%s : mix_high_tbat_recov_hv is Empty\n", __func__);
		}

		ret = of_property_read_u32(np, "battery,mix_input_limit_current_hv",
			&pdata->mix_input_limit_current_hv);
		if (ret) {
			pdata->mix_input_limit_current_hv = 1667;
			pr_info("%s : mix_input_limit_current_hv is Empty\n", __func__);
		}
	}

#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY)
	pdata->wpc_delayed_current_en = of_property_read_bool(np,
					   "battery,wpc_delayed_current_en");
	if (pdata->wpc_delayed_current_en) {
		ret = of_property_read_u32(np, "battery,wpc_delayed_current", &temp);
		if (ret) {
			pdata->wpc_delayed_current = 0;
			pr_info("%s : wpc_delayed_current is Empty\n", __func__);
		} else {
			pdata->wpc_delayed_current = temp;
		}
		ret = of_property_read_u32(np, "battery,wpc_delayed_current_time", &temp);
		if (ret) {
			pdata->wpc_delayed_current_time = 0;
			pr_info("%s : wpc_delayed_current_time is Empty\n", __func__);
		} else {
			pdata->wpc_delayed_current_time = temp;
		}
	}
#endif

	ret = of_property_read_u32(np, "battery,inbat_voltage",
			&pdata->inbat_voltage);
	if (ret)
		pr_info("%s : inbat_voltage is Empty\n", __func__);

	if (pdata->inbat_voltage) {
		p = of_get_property(np, "battery,inbat_voltage_table_adc", &len);
		if (!p)
			return 1;

		len = len / sizeof(u32);

		pdata->inbat_adc_table_size = len;

		pdata->inbat_adc_table =
			kzalloc(sizeof(sec_bat_adc_table_data_t) *
					pdata->inbat_adc_table_size, GFP_KERNEL);

		for(i = 0; i < pdata->inbat_adc_table_size; i++) {
			ret = of_property_read_u32_index(np,
							 "battery,inbat_voltage_table_adc", i, &temp);
			pdata->inbat_adc_table[i].adc = (int)temp;
			if (ret)
				pr_info("%s : inbat_adc_table(adc) is Empty\n",
						__func__);

			ret = of_property_read_u32_index(np,
							 "battery,inbat_voltage_table_data", i, &temp);
			pdata->inbat_adc_table[i].data = (int)temp;
			if (ret)
				pr_info("%s : inbat_adc_table(data) is Empty\n",
						__func__);
		}
	}

	p = of_get_property(np, "battery,input_current_limit", &len);
	if (!p)
		return 1;

	len = len / sizeof(u32);

	pdata->charging_current =
		kzalloc(sizeof(sec_charging_current_t) * len,
			GFP_KERNEL);

	for(i = 0; i < len; i++) {
		ret = of_property_read_u32_index(np,
			 "battery,input_current_limit", i,
			 &pdata->charging_current[i].input_current_limit);
		if (ret)
			pr_info("%s : Input_current_limit is Empty\n",
				__func__);

		ret = of_property_read_u32_index(np,
			 "battery,fast_charging_current", i,
			 &pdata->charging_current[i].fast_charging_current);
		if (ret)
			pr_info("%s : Fast charging current is Empty\n",
				__func__);

		ret = of_property_read_u32_index(np,
			 "battery,full_check_current_1st", i,
			 &pdata->charging_current[i].full_check_current_1st);
		if (ret)
			pr_info("%s : Full check current 1st is Empty\n",
				__func__);

		ret = of_property_read_u32_index(np,
			 "battery,full_check_current_2nd", i,
			 &pdata->charging_current[i].full_check_current_2nd);
		if (ret)
			pr_info("%s : Full check current 2nd is Empty\n",
				__func__);
	}

	ret = of_property_read_u32(np, "battery,adc_check_count",
		&pdata->adc_check_count);
	if (ret)
		pr_info("%s : Adc check count is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_adc_type",
		&pdata->temp_adc_type);
	if (ret)
		pr_info("%s : Temp adc type is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,cable_check_type",
		&pdata->cable_check_type);
	if (ret)
		pr_info("%s : Cable check type is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,cable_source_type",
		&pdata->cable_source_type);
#if defined(CONFIG_CHARGING_VZWCONCEPT)
	pdata->cable_check_type &= ~SEC_BATTERY_CABLE_CHECK_NOUSBCHARGE;
	pdata->cable_check_type |= SEC_BATTERY_CABLE_CHECK_NOINCOMPATIBLECHARGE;
#endif
	if (ret)
		pr_info("%s : Cable source type is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,event_waiting_time",
		&pdata->event_waiting_time);
	if (ret)
		pr_info("%s : Event waiting time is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,polling_type",
		&pdata->polling_type);
	if (ret)
		pr_info("%s : Polling type is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,monitor_initial_count",
		&pdata->monitor_initial_count);
	if (ret)
		pr_info("%s : Monitor initial count is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,battery_check_type",
		&pdata->battery_check_type);
	if (ret)
		pr_info("%s : Battery check type is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,check_count",
		&pdata->check_count);
	if (ret)
		pr_info("%s : Check count is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,check_adc_max",
		&pdata->check_adc_max);
	if (ret)
		pr_info("%s : Check adc max is Empty\n", __func__);

        ret = of_property_read_u32(np, "battery,check_adc_min",
                &pdata->check_adc_min);
	if (ret)
		pr_info("%s : Check adc min is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,ovp_uvlo_check_type",
		&pdata->ovp_uvlo_check_type);
	if (ret)
		pr_info("%s : Ovp Uvlo check type is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_check_type",
		&pdata->temp_check_type);
	if (ret)
		pr_info("%s : Temp check type is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_check_count",
		&pdata->temp_check_count);
	if (ret)
		pr_info("%s : Temp check count is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_highlimit_threshold_event",
				   &temp);
	pdata->temp_highlimit_threshold_event = (int)temp;
	if (ret)
		pr_info("%s : Temp highlimit threshold event is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_highlimit_recovery_event",
				   &temp);
	pdata->temp_highlimit_recovery_event = (int)temp;
	if (ret)
		pr_info("%s : Temp highlimit recovery event is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_high_threshold_event",
				   &temp);
	pdata->temp_high_threshold_event =  (int)temp;
	if (ret)
		pr_info("%s : Temp high threshold event is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_high_recovery_event",
				   &temp);
	pdata->temp_high_recovery_event =  (int)temp;
	if (ret)
		pr_info("%s : Temp high recovery event is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_low_threshold_event",
				   &temp);
	pdata->temp_low_threshold_event =  (int)temp;
	if (ret)
		pr_info("%s : Temp low threshold event is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_low_recovery_event",
				   &temp);
	pdata->temp_low_recovery_event =  (int)temp;
	if (ret)
		pr_info("%s : Temp low recovery event is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_highlimit_threshold_normal",
				   &temp);
	pdata->temp_highlimit_threshold_normal =  (int)temp;
	if (ret)
		pr_info("%s : Temp highlimit threshold normal is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_highlimit_recovery_normal",
				   &temp);
	pdata->temp_highlimit_recovery_normal =  (int)temp;
	if (ret)
		pr_info("%s : Temp highlimit recovery normal is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_high_threshold_normal",
				   &temp);
	pdata->temp_high_threshold_normal =  (int)temp;
	if (ret)
		pr_info("%s : Temp high threshold normal is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_high_recovery_normal",
				   &temp);
	pdata->temp_high_recovery_normal =  (int)temp;
	if (ret)
		pr_info("%s : Temp high recovery normal is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_low_threshold_normal",
				   &temp);
	pdata->temp_low_threshold_normal =  (int)temp;
	if (ret)
		pr_info("%s : Temp low threshold normal is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_low_recovery_normal",
				   &temp);
	pdata->temp_low_recovery_normal =  (int)temp;
	if (ret)
		pr_info("%s : Temp low recovery normal is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_highlimit_threshold_lpm",
				   &temp);
	pdata->temp_highlimit_threshold_lpm = (int)temp;
	if (ret)
		pr_info("%s : Temp highlimit threshold lpm is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_highlimit_recovery_lpm",
				   &temp);
	pdata->temp_highlimit_recovery_lpm = (int)temp;
	if (ret)
		pr_info("%s : Temp highlimit recovery lpm is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_high_threshold_lpm",
				   &temp);
	pdata->temp_high_threshold_lpm = (int)temp;
	if (ret)
		pr_info("%s : Temp high threshold lpm is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_high_recovery_lpm",
				   &temp);
	pdata->temp_high_recovery_lpm = (int)temp;
	if (ret)
		pr_info("%s : Temp high recovery lpm is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_low_threshold_lpm",
				   &temp);
	pdata->temp_low_threshold_lpm = (int)temp;
	if (ret)
		pr_info("%s : Temp low threshold lpm is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,temp_low_recovery_lpm",
				   &temp);
	pdata->temp_low_recovery_lpm = (int)temp;
	if (ret)
		pr_info("%s : Temp low recovery lpm is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,wpc_high_threshold_normal",
				   &temp);
	pdata->wpc_high_threshold_normal =  (int)temp;
	if (ret) {
		pr_info("%s : wpc high threshold normal is Empty\n", __func__);
		pdata->wpc_high_threshold_normal = 450;
	}

	ret = of_property_read_u32(np, "battery,wpc_high_recovery_normal",
				   &temp);
	pdata->wpc_high_recovery_normal =  (int)temp;
	if (ret) {
		pr_info("%s : wpc high recovery normal is Empty\n", __func__);
		pdata->wpc_high_recovery_normal =  400;
	}

	ret = of_property_read_u32(np, "battery,wpc_low_threshold_normal",
				   &temp);
	pdata->wpc_low_threshold_normal =  (int)temp;
	if (ret) {
		pr_info("%s : wpc low threshold normal is Empty\n", __func__);
		pdata->wpc_low_threshold_normal = 0;
	}

	ret = of_property_read_u32(np, "battery,wpc_low_recovery_normal",
				   &temp);
	pdata->wpc_low_recovery_normal =  (int)temp;
	if (ret) {
		pr_info("%s : wpc low recovery normal is Empty\n", __func__);
		pdata->wpc_low_recovery_normal =  50;
	}

	pr_info("%s : HIGHLIMIT_THRESHOLD_EVENT(%d), HIGHLIMIT_RECOVERY_EVENT(%d)\n"
		"HIGH_THRESHOLD_EVENT(%d), HIGH_RECOVERY_EVENT(%d) LOW_THRESHOLD_EVENT(%d), LOW_RECOVERY_EVENT(%d)\n"
		"HIGHLIMIT_THRESHOLD_NOLMAL(%d), HIGHLIMIT_RECOVERY_NORMAL(%d)\n"
		"HIGH_THRESHOLD_NORMAL(%d), HIGH_RECOVERY_NORMAL(%d) LOW_THRESHOLD_NORMAL(%d), LOW_RECOVERY_NORMAL(%d)\n"
		"HIGHLIMIT_THRESHOLD_LPM(%d), HIGHLIMIT_RECOVERY_LPM(%d)\n"
		"HIGH_THRESHOLD_LPM(%d), HIGH_RECOVERY_LPM(%d) LOW_THRESHOLD_LPM(%d), LOW_RECOVERY_LPM(%d)\n"
		"HIGH_THRESHOLD_WPC(%d), HIGH_RECOVERY_WPC(%d) LOW_THRESHOLD_WPC(%d), LOW_RECOVERY_WPC(%d)\n",
		__func__, pdata->temp_highlimit_threshold_event, pdata->temp_highlimit_recovery_event,
		pdata->temp_high_threshold_event, pdata->temp_high_recovery_event,
		pdata->temp_low_threshold_event, pdata->temp_low_recovery_event,
		pdata->temp_highlimit_threshold_normal, pdata->temp_highlimit_recovery_normal,
		pdata->temp_high_threshold_normal, pdata->temp_high_recovery_normal,
		pdata->temp_low_threshold_normal, pdata->temp_low_recovery_normal,
		pdata->temp_highlimit_threshold_lpm, pdata->temp_highlimit_recovery_lpm,
		pdata->temp_high_threshold_lpm, pdata->temp_high_recovery_lpm,
		pdata->temp_low_threshold_lpm, pdata->temp_low_recovery_lpm,
		pdata->wpc_high_threshold_normal, pdata->wpc_high_recovery_normal,
		pdata->wpc_low_threshold_normal, pdata->wpc_low_recovery_normal);

	ret = of_property_read_u32(np, "battery,full_check_type",
		&pdata->full_check_type);
	if (ret)
		pr_info("%s : Full check type is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,full_check_type_2nd",
		&pdata->full_check_type_2nd);
	if (ret)
		pr_info("%s : Full check type 2nd is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,full_check_count",
		&pdata->full_check_count);
	if (ret)
		pr_info("%s : Full check count is Empty\n", __func__);

        ret = of_property_read_u32(np, "battery,chg_gpio_full_check",
                &pdata->chg_gpio_full_check);
	if (ret)
		pr_info("%s : Chg gpio full check is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,chg_polarity_full_check",
		&pdata->chg_polarity_full_check);
	if (ret)
		pr_info("%s : Chg polarity full check is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,full_condition_type",
		&pdata->full_condition_type);
	if (ret)
		pr_info("%s : Full condition type is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,full_condition_soc",
		&pdata->full_condition_soc);
	if (ret)
		pr_info("%s : Full condition soc is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,full_condition_vcell",
		&pdata->full_condition_vcell);
	if (ret)
		pr_info("%s : Full condition vcell is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,recharge_check_count",
		&pdata->recharge_check_count);
	if (ret)
		pr_info("%s : Recharge check count is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,recharge_condition_type",
		&pdata->recharge_condition_type);
	if (ret)
		pr_info("%s : Recharge condition type is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,recharge_condition_soc",
		&pdata->recharge_condition_soc);
	if (ret)
		pr_info("%s : Recharge condition soc is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,recharge_condition_vcell",
		&pdata->recharge_condition_vcell);
	if (ret)
		pr_info("%s : Recharge condition vcell is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery, charging_total_time",
		(unsigned int *)&pdata->charging_total_time);
	if (ret)
		pr_info("%s : Charging total time is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,hv_charging_total_time",
				   &pdata->hv_charging_total_time);
	if (ret) {
		pdata->hv_charging_total_time = 3 * 60 * 60;
		pr_info("%s : HV Charging total time is %d\n",
			__func__, pdata->hv_charging_total_time);
	}

	ret = of_property_read_u32(np, "battery,normal_charging_total_time",
				   &pdata->normal_charging_total_time);
	if (ret) {
		pdata->normal_charging_total_time = 5 * 60 * 60;
		pr_info("%s : Normal(WC) Charging total time is %d\n",
			__func__, pdata->normal_charging_total_time);
	}

	ret = of_property_read_u32(np, "battery,usb_charging_total_time",
				   &pdata->usb_charging_total_time);
	if (ret) {
		pdata->usb_charging_total_time = 10 * 60 * 60;
		pr_info("%s : USB Charging total time is %d\n",
			__func__, pdata->usb_charging_total_time);
	}

	ret = of_property_read_u32(np, "battery,recharging_total_time",
		(unsigned int *)&pdata->recharging_total_time);
	if (ret)
		pr_info("%s : Recharging total time is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,charging_reset_time",
		(unsigned int *)&pdata->charging_reset_time);
	if (ret)
		pr_info("%s : Charging reset time is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,charging_reset_time",
		(unsigned int *)&pdata->charging_reset_time);
	if (ret)
		pr_info("%s : Charging reset time is Empty\n", __func__);
#if defined(CONFIG_BATTERY_SWELLING_SELF_DISCHARGING)
#ifdef CONFIG_BATTERY_SWELLING_SELF_DISCHARGING_ZERO_ONLY
	if (zero_sdchg_ic_exist == 0)
		pdata->sdchg_type = kstrdup("sdchg_ap", GFP_KERNEL);
	else
		pdata->sdchg_type = kstrdup("sdchg_ic", GFP_KERNEL);
#else
	ret = of_property_read_string(np,
		"sdchg_type", (char const **)&pdata->sdchg_type);
	if (ret)
		pdata->sdchg_type = kstrdup("sdchg_ic", GFP_KERNEL);
#endif
	{
		struct sdchg_info_t *pinfo;
		// find info from list_head
		list_for_each_entry(pinfo, &sdchg_info_head, info_list) {
			if (!strcmp(pinfo->type, pdata->sdchg_type)) {
				pdata->sdchg_info = pinfo;
				if (pinfo->nochip)
					sdchg_nochip_support = true;
				pr_info("[SDCHG][%s] Battery Self Discharging Type : %s\n",
					__func__, pdata->sdchg_type);
				break;
			}
		}
		if (!pinfo) {
			pr_info("[SDCHG][%s] %s info is not found! \n" \
				"!! Caution : This Model don't support Battery"\
				" Swelling Self Discharging !!\n", __func__, pdata->sdchg_type);
		} else {
			pdata->sdchg_info->sdchg_parse_dt(dev);
		}
	}
#endif

#if defined(CONFIG_BATTERY_SWELLING)
	ret = of_property_read_u32(np, "battery,chg_float_voltage",
		(unsigned int *)&pdata->swelling_normal_float_voltage);
	if (ret)
		pr_info("%s: chg_float_voltage is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,swelling_high_temp_block",
				   &temp);
	pdata->swelling_high_temp_block = (int)temp;
	if (ret)
		pr_info("%s: swelling high temp block is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,swelling_high_temp_recov",
				   &temp);
	pdata->swelling_high_temp_recov = (int)temp;
	if (ret)
		pr_info("%s: swelling high temp recovery is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,swelling_low_temp_block",
				   &temp);
	pdata->swelling_low_temp_block = (int)temp;
	if (ret)
		pr_info("%s: swelling low temp block is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,swelling_low_temp_recov",
				   &temp);
	pdata->swelling_low_temp_recov = (int)temp;
	if (ret)
		pr_info("%s: swelling low temp recovery is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,swelling_chg_current",
				   &temp);
	pdata->swelling_chg_current = (int)temp;
	if (ret)
		pr_info("%s: swelling low temp chg current is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,swelling_wc_chg_current",
				   &temp);
	pdata->swelling_wc_chg_current = (int)temp;
	if (ret) {
		pr_info("%s: swelling wc chg current is Empty. forced set to 600mA.\n", __func__);
		pdata->swelling_wc_chg_current = 600;
	}

	ret = of_property_read_u32(np, "battery,swelling_drop_float_voltage",
		(unsigned int *)&pdata->swelling_drop_float_voltage);
	if (ret)
		pr_info("%s: swelling drop float voltage is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,swelling_high_rechg_voltage",
		(unsigned int *)&pdata->swelling_high_rechg_voltage);
	if (ret)
		pr_info("%s: swelling_high_rechg_voltage is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,swelling_low_rechg_voltage",
		(unsigned int *)&pdata->swelling_low_rechg_voltage);
	if (ret)
		pr_info("%s: swelling_low_rechg_voltage is Empty\n", __func__);

	ret = of_property_read_u32(np, "battery,swelling_block_time",
		(unsigned int *)&pdata->swelling_block_time);
	if (ret)
		pr_info("%s: swelling_block_time is Empty\n", __func__);

	pr_info("%s : SWELLING_HIGH_TEMP(%d) SWELLING_HIGH_TEMP_RECOVERY(%d)\n"
		"SWELLING_LOW_TEMP(%d) SWELLING_LOW_TEMP_RECOVERY(%d) "
		"SWELLING_CHG_CURRENT(%d)\n",
		__func__, pdata->swelling_high_temp_block, pdata->swelling_high_temp_recov,
		pdata->swelling_low_temp_block, pdata->swelling_low_temp_recov,
		pdata->swelling_chg_current);

#endif

#ifdef CONFIG_SEC_FACTORY
	/* Sub PBA Detection for SMD*/
	pdata->sub_pba_detection = of_property_read_bool(np, "battery,sub_pba_detection");

	if(pdata->sub_pba_detection){
		ret = pdata->sub_det = of_get_named_gpio_flags(np, "battery,sub_det",
				0, &irq_gpio_flags);
		if (ret < 0) {
			dev_err(dev, "%s : can't get sub_det\r\n", __FUNCTION__);
		}
		pr_info("%s sub_det = 0x%x\n",__func__, pdata->sub_det);
	}
#endif

#if defined(CONFIG_WIRELESS_FIRMWARE_UPDATE)
		/* wpc_det */
		ret = pdata->wpc_det = of_get_named_gpio_flags(np, "battery,wpc_det",
				0, &irq_gpio_flags);
		if (ret < 0) {
			pr_info("%s : can't get wpc_det\r\n", __func__);
		}
#endif

	ret = of_property_read_u32(np, "battery,chg_float_voltage",
		(unsigned int *)&pdata->chg_float_voltage);
	if (ret)
		pr_info("%s: chg_float_voltage is Empty\n", __func__);

#if defined(CONFIG_BATTERY_AGE_FORECAST)
	p = of_get_property(np, "battery,age_data", &len);
	if (p) {
		battery->pdata->num_age_step = len / sizeof(sec_age_data_t);
		battery->pdata->age_data = kzalloc(len, GFP_KERNEL);
		ret = of_property_read_u32_array(np, "battery,age_data",
				 (u32 *)battery->pdata->age_data, len/sizeof(u32));
		if (ret) {
			pr_err("%s failed to read battery->pdata->age_data: %d\n",
					__func__, ret);
			kfree(battery->pdata->age_data);
			battery->pdata->age_data = NULL;
			battery->pdata->num_age_step = 0;
		}
		pr_err("%s num_age_step : %d\n", __func__, battery->pdata->num_age_step);
		for (len = 0; len < battery->pdata->num_age_step; ++len) {
			pr_err("[%d/%d]cycle:%d, float:%d, full_v:%d, recharge_v:%d, soc:%d\n",
				len, battery->pdata->num_age_step-1,
				battery->pdata->age_data[len].cycle,
				battery->pdata->age_data[len].float_voltage,
				battery->pdata->age_data[len].recharge_condition_vcell,
				battery->pdata->age_data[len].full_condition_soc,
				battery->pdata->age_data[len].full_condition_vcell);
		}
	} else {
		battery->pdata->num_age_step = 0;
		pr_err("%s there is not age_data\n", __func__);
	}
#endif
	return ret;
}
#endif

int charging_night_mode;
static int __init charging_mode(char *str)
{
	int mode;

	/*
	 * Only update loglevel value when a correct setting was passed,
	 * to prevent blind crashes (when loglevel being set to 0) that
	 * are quite hard to debug
	 */
	if (get_option(&str, &mode)) {
		charging_night_mode = mode & 0x000000FF;

		printk(KERN_ERR "charging_mode() : 0x%x(%d)\n", charging_night_mode, charging_night_mode);

		return 0;
	}

	printk(KERN_ERR "charging_mode() : %d\n", -EINVAL);

	return -EINVAL;
}

early_param("charging_mode", charging_mode);

#ifdef CONFIG_OF
extern sec_battery_platform_data_t sec_battery_pdata;
#endif

static void cable_initial_check(struct sec_battery_info *battery)
{
	union power_supply_propval value;

	pr_info("%s : current_cable_type : (%d)\n", __func__, battery->cable_type);

	if (battery->cable_type == POWER_SUPPLY_TYPE_BATTERY) {
		psy_do_property(battery->pdata->charger_name, get,
				POWER_SUPPLY_PROP_ONLINE, value);
		if (value.intval == POWER_SUPPLY_TYPE_WIRELESS) {
			sec_bat_get_battery_info(battery);
		
			value.intval = 1;
			psy_do_property("wireless", set,
				POWER_SUPPLY_PROP_ONLINE, value);
		}
	}
}

static int __devinit sec_battery_probe(struct platform_device *pdev)
{
	sec_battery_platform_data_t *pdata = NULL;
	struct sec_battery_info *battery;
	int ret = 0;
#ifndef CONFIG_OF
	int i;
#endif

	union power_supply_propval value;

	dev_dbg(&pdev->dev,
		"%s: SEC Battery Driver Loading\n", __func__);

	battery = kzalloc(sizeof(*battery), GFP_KERNEL);
	if (!battery)
		return -ENOMEM;

	if (pdev->dev.of_node) {
		pdata = devm_kzalloc(&pdev->dev,
				sizeof(sec_battery_platform_data_t),
				GFP_KERNEL);
		if (!pdata) {
			dev_err(&pdev->dev, "Failed to allocate memory\n");
			ret = -ENOMEM;
			goto err_bat_free;
		}

		battery->pdata = pdata;
		if (sec_bat_parse_dt(&pdev->dev, battery)) {
			dev_err(&pdev->dev,
				"%s: Failed to get battery dt\n", __func__);
			ret = -EINVAL;
			goto err_bat_free;
		}
	} else {
		pdata = dev_get_platdata(&pdev->dev);
		battery->pdata = pdata;
	}

	platform_set_drvdata(pdev, battery);

        battery->dev = &pdev->dev;

	mutex_init(&battery->adclock);
	mutex_init(&battery->misclock);

	dev_dbg(battery->dev, "%s: ADC init\n", __func__);

#ifdef CONFIG_OF
	adc_init(pdev, battery);
#else
	for (i = 0; i < SEC_BAT_ADC_CHANNEL_NUM; i++)
		adc_init(pdev, pdata, i);
#endif
	wake_lock_init(&battery->monitor_wake_lock, WAKE_LOCK_SUSPEND,
		       "sec-battery-monitor");
	wake_lock_init(&battery->cable_wake_lock, WAKE_LOCK_SUSPEND,
		       "sec-battery-cable");
	wake_lock_init(&battery->vbus_wake_lock, WAKE_LOCK_SUSPEND,
		       "sec-battery-vbus");
	wake_lock_init(&battery->siop_wake_lock, WAKE_LOCK_SUSPEND,
		       "sec-battery-siop");
	wake_lock_init(&battery->misc_event_wake_lock, WAKE_LOCK_SUSPEND,
		       "sec-battery-misc-event");

	/* initialization of battery info */
	sec_bat_set_charging_status(battery,
			POWER_SUPPLY_STATUS_DISCHARGING);
	battery->health = POWER_SUPPLY_HEALTH_GOOD;
	battery->present = true;
	battery->is_jig_on = false;
	battery->camera_limit = false;

	battery->polling_count = 1;	/* initial value = 1 */
	battery->polling_time = pdata->polling_time[
		SEC_BATTERY_POLLING_TIME_DISCHARGING];
	battery->polling_in_sleep = false;
	battery->polling_short = false;

	battery->check_count = 0;
	battery->check_adc_count = 0;
	battery->check_adc_value = 0;

	battery->charging_start_time = 0;
	battery->charging_passed_time = 0;
	battery->charging_next_time = 0;
	battery->charging_fullcharged_time = 0;
	battery->siop_level = 100;
	battery->r_siop_level = 100;
	battery->siop_event = 0;
	battery->wc_enable = 1;
	battery->pre_chg_temp = 0;
#if defined(CONFIG_SAMSUNG_BATTERY_ENG_TEST)
	battery->stability_test = 0;
	battery->eng_not_full_status = 0;
#endif
    battery->wc_status = SEC_WIRELESS_PAD_NONE;
	battery->ps_status= 0;
	battery->ps_changed= 0;
	battery->wire_status = POWER_SUPPLY_TYPE_BATTERY;
#if defined(CONFIG_BATTERY_SWELLING)
	battery->swelling_mode = false;
#endif
	battery->charging_block = false;
	battery->chg_limit = SEC_BATTERY_CHG_TEMP_NONE;
	battery->pad_limit = SEC_BATTERY_WPC_TEMP_NONE;
	battery->mix_limit = SEC_BATTERY_MIX_TEMP_NONE;

#if defined(ANDROID_ALARM_ACTIVATED)
	alarm_init(&battery->event_termination_alarm,
			ANDROID_ALARM_ELAPSED_REALTIME_WAKEUP,
			sec_bat_event_expired_timer_func);
#else
	alarm_init(&battery->event_termination_alarm,
			ALARM_BOOTTIME,
			sec_bat_event_expired_timer_func);
#endif
	battery->temp_highlimit_threshold =
		pdata->temp_highlimit_threshold_normal;
	battery->temp_highlimit_recovery =
		pdata->temp_highlimit_recovery_normal;
	battery->temp_high_threshold =
		pdata->temp_high_threshold_normal;
	battery->temp_high_recovery =
		pdata->temp_high_recovery_normal;
	battery->temp_low_recovery =
		pdata->temp_low_recovery_normal;
	battery->temp_low_threshold =
		pdata->temp_low_threshold_normal;

	battery->charging_mode = SEC_BATTERY_CHARGING_NONE;
	battery->is_recharging = false;
	battery->cable_type = POWER_SUPPLY_TYPE_BATTERY;
	battery->test_mode = 0;
	battery->factory_mode = false;
#if defined(CONFIG_STORE_MODE)
 	battery->store_mode = false;
 	value.intval = battery->store_mode;
 	psy_do_property(battery->pdata->charger_name, set,
 	POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX, value);
#else
	battery->store_mode = false;
#endif
	battery->slate_mode = false;
	slate_mode_state = battery->slate_mode;
	battery->is_hc_usb = false;
	battery->ignore_siop = false;

	battery->batt_cycle = -1;
#if defined(CONFIG_BATTERY_AGE_FORECAST)
	battery->pdata->age_step = 0;
#endif

#if defined(CONFIG_WIRELESS_CHARGER_INBATTERY) || defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
	battery->cc_cv_mode = 0;
	battery->full_mode = false;
	battery->cs100_status = false;
#endif

#if defined(CONFIG_BATTERY_SWELLING_SELF_DISCHARGING)
	battery->self_discharging = false;
	battery->force_discharging = false;
	battery->factory_self_discharging_mode_on = false;
#endif

	if(charging_night_mode == 49)
		sleep_mode = true;
	else
		sleep_mode = false;

	battery->current_isys_now = -1;
	battery->current_isys_avg = -1;

	if (battery->pdata->charger_name == NULL)
		battery->pdata->charger_name = "sec-charger";
	if (battery->pdata->fuelgauge_name == NULL)
		battery->pdata->fuelgauge_name = "sec-fuelgauge";

#if defined(CONFIG_CHARGER_MAX77843) || defined(CONFIG_CHARGER_MAX77833)
	pr_info("%s : CHARGER CHIP NAME (%s)\n", __func__, charger_chip_name);
	if (charger_chip_name != NULL && !strcmp(charger_chip_name,"max77843-charger")) {
		battery->pdata->charger_name = "max77843-charger";
		battery->pdata->fuelgauge_name = "max77843-fuelgauge";
	//} else if (!strcmp(charger_chip_name, "max77833-charger")) {
	} else {
		battery->pdata->charger_name = "max77833-charger";
		battery->pdata->fuelgauge_name = "max77833-fuelgauge";
	}
#endif

	battery->psy_bat.name = "battery",
	battery->psy_bat.type = POWER_SUPPLY_TYPE_BATTERY,
	battery->psy_bat.properties = sec_battery_props,
	battery->psy_bat.num_properties = ARRAY_SIZE(sec_battery_props),
	battery->psy_bat.get_property = sec_bat_get_property,
	battery->psy_bat.set_property = sec_bat_set_property,
	battery->psy_usb.name = "usb",
	battery->psy_usb.type = POWER_SUPPLY_TYPE_USB,
	battery->psy_usb.supplied_to = supply_list,
	battery->psy_usb.num_supplicants = ARRAY_SIZE(supply_list),
	battery->psy_usb.properties = sec_power_props,
	battery->psy_usb.num_properties = ARRAY_SIZE(sec_power_props),
	battery->psy_usb.get_property = sec_usb_get_property,
	battery->psy_ac.name = "ac",
	battery->psy_ac.type = POWER_SUPPLY_TYPE_MAINS,
	battery->psy_ac.supplied_to = supply_list,
	battery->psy_ac.num_supplicants = ARRAY_SIZE(supply_list),
	battery->psy_ac.properties = sec_ac_props,
	battery->psy_ac.num_properties = ARRAY_SIZE(sec_ac_props),
	battery->psy_ac.get_property = sec_ac_get_property;
	battery->psy_wireless.name = "wireless",
	battery->psy_wireless.type = POWER_SUPPLY_TYPE_WIRELESS,
	battery->psy_wireless.supplied_to = supply_list,
	battery->psy_wireless.num_supplicants = ARRAY_SIZE(supply_list),
	battery->psy_wireless.properties = sec_power_props,
	battery->psy_wireless.num_properties = ARRAY_SIZE(sec_power_props),
	battery->psy_wireless.get_property = sec_wireless_get_property;
	battery->psy_wireless.set_property = sec_wireless_set_property;
	battery->psy_ps.name = "ps",
	battery->psy_ps.type = POWER_SUPPLY_TYPE_POWER_SHARING,
	battery->psy_ps.supplied_to = supply_list,
	battery->psy_ps.num_supplicants = ARRAY_SIZE(supply_list),
	battery->psy_ps.properties = sec_ps_props,
	battery->psy_ps.num_properties = ARRAY_SIZE(sec_ps_props),
	battery->psy_ps.get_property = sec_ps_get_property;
	battery->psy_ps.set_property = sec_ps_set_property;

	if (battery->pdata->wchg_ctl_en) {
		ret = gpio_request(battery->pdata->wchg_ctl, "WCHG_CTL");
		if (ret) {
			pr_err("failed to request GPIO %u\n", battery->pdata->wchg_ctl);
			goto err_wake_lock;
		}
		gpio_direction_output(battery->pdata->wchg_ctl, 0);
		pr_info("%s : GPIO %d\n", __func__, gpio_get_value(battery->pdata->wchg_ctl));
	}

#if defined(CONFIG_BATTERY_SWELLING_SELF_DISCHARGING)
	if (battery->pdata->sdchg_info) {
		ret = battery->pdata->sdchg_info->sdchg_probe((void *)battery);
		if (ret) {
			pr_err("%s : Error! failed to sdchg_probe(%u)\n", __func__, ret);
			goto err_sdchg_init;
		}
	}
#endif
	/* create work queue */
	battery->monitor_wqueue =
	    alloc_workqueue(dev_name(&pdev->dev), WQ_MEM_RECLAIM, 1);
	if (!battery->monitor_wqueue) {
		dev_err(battery->dev,
			"%s: Fail to Create Workqueue\n", __func__);
		goto err_irq;
	}

	INIT_DELAYED_WORK(&battery->monitor_work, sec_bat_monitor_work);
	INIT_DELAYED_WORK(&battery->cable_work, sec_bat_cable_work);
#if defined(CONFIG_CALC_TIME_TO_FULL)
	INIT_DELAYED_WORK(&battery->timetofull_work, sec_bat_time_to_full_work);
#endif
	INIT_DELAYED_WORK(&battery->siop_work, sec_bat_siop_work);

#if defined(CONFIG_WIRELESS_FIRMWARE_UPDATE)
	INIT_DELAYED_WORK(&battery->fw_init_work, sec_bat_fw_init_work);
#endif
	INIT_DELAYED_WORK(&battery->misc_event_work, sec_bat_misc_event_work);

	switch (pdata->polling_type) {
	case SEC_BATTERY_MONITOR_WORKQUEUE:
		INIT_DELAYED_WORK(&battery->polling_work,
			sec_bat_polling_work);
		break;
	case SEC_BATTERY_MONITOR_ALARM:
#if defined(ANDROID_ALARM_ACTIVATED)
		battery->last_poll_time = alarm_get_elapsed_realtime();
		alarm_init(&battery->polling_alarm,
			ANDROID_ALARM_ELAPSED_REALTIME_WAKEUP,
			sec_bat_alarm);
#else
		battery->last_poll_time = ktime_get_boottime();
		alarm_init(&battery->polling_alarm, ALARM_BOOTTIME,
			sec_bat_alarm);
#endif
		break;
	default:
		break;
	}

#if defined(CONFIG_BATTERY_CISD)
	sec_battery_cisd_init(battery);
#endif

	/* init power supplier framework */
	ret = power_supply_register(&pdev->dev, &battery->psy_ps);
	if (ret) {
		dev_err(battery->dev,
			"%s: Failed to Register psy_ps\n", __func__);
		goto err_workqueue;
	}

	ret = power_supply_register(&pdev->dev, &battery->psy_wireless);
	if (ret) {
		dev_err(battery->dev,
			"%s: Failed to Register psy_wireless\n", __func__);
		goto err_supply_unreg_ps;
	}

	ret = power_supply_register(&pdev->dev, &battery->psy_usb);
	if (ret) {
		dev_err(battery->dev,
			"%s: Failed to Register psy_usb\n", __func__);
		goto err_supply_unreg_wireless;
	}

	ret = power_supply_register(&pdev->dev, &battery->psy_ac);
	if (ret) {
		dev_err(battery->dev,
			"%s: Failed to Register psy_ac\n", __func__);
		goto err_supply_unreg_usb;
	}

	ret = power_supply_register(&pdev->dev, &battery->psy_bat);
	if (ret) {
		dev_err(battery->dev,
			"%s: Failed to Register psy_bat\n", __func__);
		goto err_supply_unreg_ac;
	}

	if (battery->pdata->bat_gpio_init && !battery->pdata->bat_gpio_init()) {
		dev_err(battery->dev,
			"%s: Failed to Initialize GPIO\n", __func__);
		goto err_supply_unreg_bat;
	}
#if 0
	if (battery->pdata->bat_irq) {
		ret = request_threaded_irq(battery->pdata->bat_irq,
				NULL, sec_bat_irq_thread,
				battery->pdata->bat_irq_attr
				| IRQF_ONESHOT,
				"battery-irq", battery);
		if (ret) {
			dev_err(battery->dev,
				"%s: Failed to Request IRQ (bat_int)\n", __func__);
			goto err_supply_unreg_bat;
		}
		if (battery->pdata->bat_irq_gpio) {
			ret = enable_irq_wake(battery->pdata->bat_irq);
			if (ret < 0)
				dev_err(battery->dev,
					"%s: Failed to Enable Wakeup Source(%d)(bat_int)\n",
					__func__, ret);
		}
	}
#endif

	ret = sec_bat_create_attrs(battery->psy_bat.dev);
	if (ret) {
		dev_err(battery->dev,
			"%s : Failed to create_attrs\n", __func__);
		goto err_req_irq;
	}

	/* initialize battery level*/
	value.intval = 0;
	psy_do_property(battery->pdata->fuelgauge_name, get,
			POWER_SUPPLY_PROP_CAPACITY, value);
	battery->capacity = value.intval;

#if defined(CONFIG_WIRELESS_FIRMWARE_UPDATE)
	queue_delayed_work_on(0, battery->monitor_wqueue, &battery->fw_init_work, 0);
#endif

#if defined(CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE)
	value.intval = 0;
	psy_do_property(battery->pdata->wireless_charger_name, set,
					POWER_SUPPLY_PROP_CHARGE_TYPE, value);
#endif

#if defined(CONFIG_MUIC_NOTIFIER)
	muic_notifier_register(&battery->batt_nb,
			       batt_handle_notification,
			       MUIC_NOTIFY_DEV_CHARGER);
#endif
#if defined(CONFIG_VBUS_NOTIFIER)
	vbus_notifier_register(&battery->vbus_nb,
				vbus_handle_notification,
				VBUS_NOTIFY_DEV_CHARGER);
#endif
	battery->usb_3s_nodevice = false;
#if defined(CONFIG_USB_EXTERNAL_NOTIFY)
	usb_external_notify_register(&battery->usb_nb,
				usb_handle_notification,
				EXTERNAL_NOTIFY_DEV_CHARGER);
#endif

#if defined(CONFIG_AFC_CHARGER_MODE)
	value.intval = 1;
	psy_do_property(battery->pdata->charger_name, set,
			POWER_SUPPLY_PROP_AFC_CHARGER_MODE,
			value);
#endif
	if (pdata->initial_check)
		pdata->initial_check();
	else
		cable_initial_check(battery);

	psy_do_property("battery", get,
				POWER_SUPPLY_PROP_ONLINE, value);

#ifdef CONFIG_SEC_FACTORY
	/* Sub PBA Detection for SMD */
	dev_info(battery->dev, "%s: Checking Sub PBA Available\n", __func__);
	if (battery->pdata->sub_pba_detection){
		if(gpio_get_value(battery->pdata->sub_det)){
			dev_info(battery->dev, "%s: sub pba available\n", __func__);
			battery->pdata->sub_pba_available = false;
		} else{
			battery->pdata->sub_pba_available = true;	
			dev_info(battery->dev, "%s: sub pba unavailable\n", __func__);
		}
	}
#endif
	if ((value.intval == POWER_SUPPLY_TYPE_BATTERY) ||
	    (value.intval == POWER_SUPPLY_TYPE_HV_PREPARE_MAINS)) {
		dev_info(&pdev->dev,
			"%s: SEC Battery Driver Monitorwork\n", __func__);
		wake_lock(&battery->monitor_wake_lock);
		queue_delayed_work_on(0, battery->monitor_wqueue, &battery->monitor_work, 0);
	}

	if (battery->pdata->check_battery_callback)
		battery->present = battery->pdata->check_battery_callback();

	dev_info(battery->dev,
		"%s: SEC Battery Driver Loaded\n", __func__);

	return 0;

err_req_irq:
	if (battery->pdata->bat_irq)
		free_irq(battery->pdata->bat_irq, battery);
err_supply_unreg_bat:
	power_supply_unregister(&battery->psy_bat);
err_supply_unreg_ac:
	power_supply_unregister(&battery->psy_ac);
err_supply_unreg_usb:
	power_supply_unregister(&battery->psy_usb);
err_supply_unreg_wireless:
	power_supply_unregister(&battery->psy_wireless);
err_supply_unreg_ps:
	power_supply_unregister(&battery->psy_ps);
err_workqueue:
	destroy_workqueue(battery->monitor_wqueue);
err_irq:
#if defined(CONFIG_BATTERY_SWELLING_SELF_DISCHARGING)
	if (battery->pdata->sdchg_info)
		battery->pdata->sdchg_info->sdchg_remove();
err_sdchg_init:
#endif
	gpio_free(battery->pdata->wchg_ctl);
err_wake_lock:
	wake_lock_destroy(&battery->monitor_wake_lock);
	wake_lock_destroy(&battery->cable_wake_lock);
	wake_lock_destroy(&battery->vbus_wake_lock);
	wake_lock_destroy(&battery->siop_wake_lock);
	wake_lock_destroy(&battery->misc_event_wake_lock);
	mutex_destroy(&battery->adclock);
	mutex_destroy(&battery->misclock);
	kfree(pdata);
err_bat_free:
	kfree(battery);

	return ret;
}

static int __devexit sec_battery_remove(struct platform_device *pdev)
{
	struct sec_battery_info *battery = platform_get_drvdata(pdev);
#ifndef CONFIG_OF
	int i;
#endif

	dev_dbg(battery->dev, "%s: Start\n", __func__);

	switch (battery->pdata->polling_type) {
	case SEC_BATTERY_MONITOR_WORKQUEUE:
		cancel_delayed_work(&battery->polling_work);
		break;
	case SEC_BATTERY_MONITOR_ALARM:
		alarm_cancel(&battery->polling_alarm);
		break;
	default:
		break;
	}

	alarm_cancel(&battery->event_termination_alarm);
	flush_workqueue(battery->monitor_wqueue);
	destroy_workqueue(battery->monitor_wqueue);
	wake_lock_destroy(&battery->monitor_wake_lock);
	wake_lock_destroy(&battery->cable_wake_lock);
	wake_lock_destroy(&battery->vbus_wake_lock);
	wake_lock_destroy(&battery->siop_wake_lock);

	wake_lock_destroy(&battery->misc_event_wake_lock);
	mutex_destroy(&battery->adclock);
	mutex_destroy(&battery->misclock);
#ifdef CONFIG_OF
	adc_exit(battery);
#else
	for (i = 0; i < SEC_BAT_ADC_CHANNEL_NUM; i++)
		adc_exit(battery->pdata, i);
#endif
	power_supply_unregister(&battery->psy_ps);
	power_supply_unregister(&battery->psy_wireless);
	power_supply_unregister(&battery->psy_ac);
	power_supply_unregister(&battery->psy_usb);
	power_supply_unregister(&battery->psy_bat);

#ifdef CONFIG_BATTERY_SWELLING_SELF_DISCHARGING
	if (battery->pdata->sdchg_info)
		battery->pdata->sdchg_info->sdchg_remove();
#endif

	dev_dbg(battery->dev, "%s: End\n", __func__);
	kfree(battery);

	return 0;
}

static int sec_battery_prepare(struct device *dev)
{
	struct sec_battery_info *battery
		= dev_get_drvdata(dev);

	dev_dbg(battery->dev, "%s: Start\n", __func__);

	switch (battery->pdata->polling_type) {
	case SEC_BATTERY_MONITOR_WORKQUEUE:
		cancel_delayed_work(&battery->polling_work);
		break;
	case SEC_BATTERY_MONITOR_ALARM:
		alarm_cancel(&battery->polling_alarm);
		break;
	default:
		break;
	}
	cancel_delayed_work_sync(&battery->monitor_work);

	battery->polling_in_sleep = true;

	sec_bat_set_polling(battery);

	/* cancel work for polling
	 * that is set in sec_bat_set_polling()
	 * no need for polling in sleep
	 */
	if (battery->pdata->polling_type ==
		SEC_BATTERY_MONITOR_WORKQUEUE)
		cancel_delayed_work(&battery->polling_work);

	dev_dbg(battery->dev, "%s: End\n", __func__);

	return 0;
}

static int sec_battery_suspend(struct device *dev)
{
	return 0;
}

static int sec_battery_resume(struct device *dev)
{
	return 0;
}

static void sec_battery_complete(struct device *dev)
{
	struct sec_battery_info *battery
		= dev_get_drvdata(dev);

	dev_dbg(battery->dev, "%s: Start\n", __func__);

	/* cancel current alarm and reset after monitor work */
	if (battery->pdata->polling_type == SEC_BATTERY_MONITOR_ALARM)
		alarm_cancel(&battery->polling_alarm);

	wake_lock(&battery->monitor_wake_lock);
	queue_delayed_work_on(0, battery->monitor_wqueue,
		&battery->monitor_work, 0);

	dev_dbg(battery->dev, "%s: End\n", __func__);

	return;
}

static void sec_battery_shutdown(struct device *dev)
{
#if defined(CONFIG_BATTERY_SWELLING_SELF_DISCHARGING)
	struct sec_battery_info *battery
		= dev_get_drvdata(dev);
	if (battery->force_discharging) {
		if (battery->pdata->sdchg_info) {
			pr_info("SELF DISCHARGING IC DISENABLE\n");
			battery->pdata->sdchg_info->sdchg_force_control(battery, false);
		}
	}
#endif
	return;
}

#ifdef CONFIG_OF
static struct of_device_id sec_battery_dt_ids[] = {
	{ .compatible = "samsung,sec-battery" },
	{ }
};
MODULE_DEVICE_TABLE(of, sec_battery_dt_ids);
#endif /* CONFIG_OF */

static const struct dev_pm_ops sec_battery_pm_ops = {
	.prepare = sec_battery_prepare,
	.suspend = sec_battery_suspend,
	.resume = sec_battery_resume,
	.complete = sec_battery_complete,
};

static struct platform_driver sec_battery_driver = {
	.driver = {
		   .name = "sec-battery",
		   .owner = THIS_MODULE,
		   .pm = &sec_battery_pm_ops,
		   .shutdown = sec_battery_shutdown,
#ifdef CONFIG_OF
		.of_match_table = sec_battery_dt_ids,
#endif
	},
	.probe = sec_battery_probe,
	.remove = __devexit_p(sec_battery_remove),
};

static int __init sec_battery_init(void)
{
	return platform_driver_register(&sec_battery_driver);
}

static void __exit sec_battery_exit(void)
{
	platform_driver_unregister(&sec_battery_driver);
}

late_initcall(sec_battery_init);
module_exit(sec_battery_exit);

MODULE_DESCRIPTION("Samsung Battery Driver");
MODULE_AUTHOR("Samsung Electronics");
MODULE_LICENSE("GPL");
