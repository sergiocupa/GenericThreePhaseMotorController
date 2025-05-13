/*
 * motor_controller_runner.h
 *
 *  Created on: May 12, 2025
 *      Author: sergio
 */

#ifndef INC_THREE_PHASE_DRIVE_RUNNER_H_
#define INC_THREE_PHASE_DRIVE_RUNNER_H_

#include "three_phase_drive_preparer.h"


void OnTransmissionCompleted(uint8_t *Buf, uint32_t *Len, uint8_t epnum);

void three_phase_drive_run(ThreePhaseDriveData *instance);


#endif /* INC_THREE_PHASE_DRIVE_RUNNER_H_ */
