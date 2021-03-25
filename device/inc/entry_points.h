#ifndef _SRC_ENTRY_POINTS
#define _SRC_ENTRY_POINTS

#include "build_switches.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * UNIT TESTS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if ENABLE_UNIT_TEST_EXT_PWR_SWITCH
void unitTest_enableExternalHwPower();
#endif // ENABLE_UNIT_TEST_EXT_PWR_SWITCH

#if ENABLE_UNIT_TEST_WIRELESS_API
int unitTest_wirelessApi();
#endif // ENABLE_UNIT_TEST_WIRELESS_API

#if ENABLE_UNIT_TEST_NXP_API
int unitTest_nxpApi();
#endif // ENABLE_UNIT_TEST_NXP_API

#if ENABLE_UNIT_TEST_MUX
int unitTest_mux();
#endif // ENABLE_UNIT_TEST_MUX

#if ENABLE_UNIT_TEST_MUX_LP
int unitTest_muxLp();
#endif // ENABLE_UNIT_TEST_MUX_C2000

#if ENABLE_UNIT_TEST_ADC
int unitTest_adc();
#endif // ENABLE_UNIT_TEST_ADC

#if ENABLE_UNIT_TEST_ADC_C2000
int unitTest_adcC2000();
#endif // ENABLE_UNIT_TEST_ADC_C2000

#if ENABLE_UNIT_TEST_ADC_MSP432
int unitTest_adcMsp432();
#endif // ENABLE_UNIT_TEST_ADC_MSP432

#if ENABLE_UNIT_TEST_FLEX_SENSORS
int unitTest_flexSensors();
#endif // ENABLE_UNIT_TEST_FLEX_SENSORS

#if ENABLE_UNIT_TEST_HC05_DEVICE_NAME
int unitTest_hc05DeviceName();
#endif // ENABLE_UNIT_TEST_HC05_DEVICE_NAME

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * MAIN PROGRAMS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if ENABLE_HC05_CONFIG_MSTR
int unitTest_hc05ConfigMaster();
#endif // ENABLE_HC05_CONFIG_MSTR

#endif // _SRC_ENTRY_POINTS
