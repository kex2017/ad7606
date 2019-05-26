
#ifndef DM9000_H_
#define DM9000_H_

#include "mutex.h"
#include "net/netdev.h"

/**
 * @brief   Struct containing the needed peripheral configuration
 */
typedef struct {

} dm9000_params_t;

/**
 * @brief   ENC28J60 device descriptor
 */
typedef struct {
    netdev_t netdev;        /**< pull in the netdev fields */
    mutex_t devlock;        /**< lock the device on access */
} dm9000_t;


/**
 * @brief   Ready the device for initialization through it's netdev interface
 *
 * @param[in] dev           device descriptor
 * @param[in] params        peripheral configuration to use
 */
void dm9000_setup(dm9000_t *dev, const dm9000_params_t *params);


#endif /* DM9000_H_ */
