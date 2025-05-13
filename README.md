# GenericThreePhaseMotorController

The software is generic and can be used for both synchronous and asynchronous motors. An example of the use of synchronous motors is the BLDC type (used in drones). In this case, since precise positioning is not required, it is possible to configure control without an encoder.

For synchronous mode, it is also possible to use servomotors with encoders (common in robotics), which require torque for large movement impulses and precise positioning.

The software will also support use with large asynchronous induction motors, which can be configured with FOC, V/f control, or a combination of both. The IGBT activation dead time can be configured; this configuration is necessary to ensure that there is no unwanted short circuit in the activation cycle, caused by a delay in the IGBT deactivation times.
