#ifndef __PARAMETER_CONVERSION_H__
#define __PARAMETER_CONVERSION_H__

#include "common_type.h"

/************************ Unit Conversion Macros ************************/
#define Unit_Conversion(u, b) ((int64_t)INT16_MAX * u / b)
#define Voltage_Conversion(v, b) (Unit_Conversion(v, b))
#define Current_Conversion(i, b) (Unit_Conversion(i, b))
#define RPM_Conversion(r, b) (Unit_Conversion(r, b))
#define Hz_Conversion(h, b) (Unit_Conversion((float)h * 60.0f, b))
#define Angle_Conversion(u) (((int32_t)UINT16_MAX * u + 180) / (int32_t)360)

/* h = Hz, pp = Pole Pairs, p = PWM Frequency */
#define HztoPsuedoSpeed_Conversion(h, pp, p) ((uint32_t)UNIT_U16_RESOLUTION * pp * h / p)

/* rpm = RPM, pp = Pole Pairs, p = frequency 
  Converts RPM to how many ticks should occur between 6-step measurements.
  Either from hall sensors or back-EMF crossover detections. 
  Useful for calculating min/max on measured ticks. */
#define RPMto6StepTicks_Conversion(rpm, pp, f) ((uint32_t)f * 10 / (rpm * pp))

#define us_Conversion(us, f) ((uint32_t)(((uint64_t)us * f + 500000) / 1000000))     /* Converts us to ticks. */
#define ms_Conversion(ms, f) ((uint32_t)(((uint64_t)ms * f + 500) / 1000))           /* Converts ms to ticks. */
#define second_Conversion(s, f) ((uint32_t)f * s)                        /* Converts seconds to ticks. */
#define minute_Conversion(m, f) (second_Conversion((uint32_t)60 * m, f)) /* Converts minutes to ticks. */
#define hour_Conversion(h, f) (minute_Conversion((uint32_t)60 * h, f))   /* Converts minutes to ticks. */

#define temperature_Conversion(t) ((int16_t)10 * t) /* Converts termperature in °C to 0.1°C. */

// #define ns_Conversion(ns, f_MHz) (((uint32_t)ns * f_MHz + 500) / 1000) /* Converts ns to ticks. */
#define ns_Conversion(ns, f) ((uint32_t)(((uint64_t)ns * f + 500000000) / (uint64_t)1000000000)) /* Converts ns to ticks. */

/************************************************************************/

/*************** ADC Measurement Conversion Factor Macros ***************/

/* Conversion factors convert from ADCu16 format to one in q1.15 (s16) format for the unit being measured. */
#define ADC_Voltage_Conversion_Factor(v, r1, r2, b) ((uint32_t)UNIT_S16_RESOLUTION * v * ((float)r1 + r2) / ((float)r2 * b))
#define ADC_Current_Conversion_Factor(v, r, g, b) ((uint32_t)UNIT_S16_RESOLUTION * v * 1000 / ((float)r * g * b))

/************************************************************************/

/************************* ADC Sampling Time *************************/

/* fm = MCU frequency (MHz) (int or float)
   fa = ADC frequency (MHz) (int or float)
   m = sampling time in cycles. (float)
   n = ADC bits. (int) 
   d = delay (int or float) (MCU Dependent) */
#define Calc_ADC_Sampling_Ticks(fm, fa, m, n, d) ((uint32_t)(((float)fm / (float)fa) * ((float)m + (float)n + (float)d)))

/*********************************************************************/
/************************* Mechanical Parameter Conversions *************************/

#define KmhToRPM_Conversion(Kmh, WheelDiameter_mm) ((int32_t)((1000.0f * 1000.0f / (60.0f * 3.141592654f * (float)WheelDiameter_mm )) * (float)Kmh))

#define KmhToRPMS16_Conversion(Kmh, WheelDiameter_mm) (RPM_Conversion(KmhToRPM_Conversion(Kmh, WheelDiameter_mm), RPM_MAX))

/*********************************************************************/

#endif
