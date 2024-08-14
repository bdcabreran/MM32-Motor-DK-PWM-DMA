#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>

#define MCU_CLOCK (48000000UL)
#define MCU_CLOCK_MHZ (48U)
#define ADC_CLOCK_MHZ (14U)
#define ADC_SAMPLING_DELAY (0.5f) /* Important: this changes with MCU. e.g. FMD is 0.5, MM is 1.5. */

#define SYSTICK_FREQUENCY ((uint16_t)1000U) /* (Hz) Systick frequency. */
#define SYSTICK_TICK_FREQUENCY (1000U)
#define TICK_INT_PRIORITY (3UL)

#define ADC_VREF (5.0f)         /* ADC reference voltage */
#define ADC_CALIB_VOLTAGE (3.3f) /* ADC calibration voltage. */
#define ADC_CALIB_REGISTER (0x1FFFF7BA) /* ADC calibration value register. */
// #define ADC_INTERNAL_VREF (1.2f) /* ADC internal reference voltage */
#define ADC_RESOLUTION (0x1000) /* 2^n where n = number of bits. */
#define ADC_BITS (12U)           /* ADC bit resolution */
#define ADC_VREF_FILTER_SHIFT (6U)

#define UART_PRIORITY (1UL)

#define NTC_TABLE_SIZE (65U)
/* Get table from here: https://www.sebulli.com/ntc/index.php */
static int16_t NTC_Table[NTC_TABLE_SIZE] = {
    1921, 1607, 1293, 1127, 1016, 933, 866, 811,
    763, 722, 685, 652, 621, 593, 567, 543, 520,
    498, 477, 458, 439, 421, 403, 386, 370, 354,
    338, 323, 308, 293, 278, 264, 250, 236, 222,
    208, 194, 181, 167, 153, 139, 125, 111, 97,
    83, 68, 53, 37, 22, 5, -11, -29, -47, -66,
    -87, -108, -132, -157, -186, -218, -256,
    -302, -364, -460, -556};


#endif
