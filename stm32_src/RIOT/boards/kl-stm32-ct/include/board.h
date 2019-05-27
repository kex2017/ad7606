#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "../../kl-stm32-ct/include/periph_conf.h"
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name    Xtimer configuration
 *
 * Tell the xtimer that we use a 16-bit peripheral timer
 * @{
 */
#define XTIMER_WIDTH        (16)
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
