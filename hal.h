/* Automatically generated HAL from hal.xml */
/* NEVER EDIT MANUALLY */

/* Generated on: 2019-05-09 14:40:00 */

#ifndef __HAL_H__
#define __HAL_H__

#include <avr/io.h>

#define nop()                                    __asm__ __volatile__("nop")

/* DIRECTION -> PA7 (Output, Initially Inactive) */
#define DIRECTION_BIT                            7
#define DIRECTION_PIN                            PINA
#define DIRECTION_PORT                           PORTA
#define DIRECTION_DDR                            DDRA
#define DIRECTION_ModeOutput()                   DIRECTION_DDR |= _BV(DIRECTION_BIT)
#define DIRECTION_IsOutput()                     ((DIRECTION_DDR & _BV(DIRECTION_BIT)) != 0)
#define DIRECTION_SetHIGH()                      DIRECTION_PORT |= _BV(DIRECTION_BIT)
#define DIRECTION_SetLOW()                       DIRECTION_PORT &= ~_BV(DIRECTION_BIT)
#define DIRECTION_Get()                          (DIRECTION_PIN & _BV(DIRECTION_BIT))
#define DIRECTION_SetInactive()                  DIRECTION_SetLOW()
#define DIRECTION_SetActive()                    DIRECTION_SetHIGH()
#define DIRECTION_Toggle()                       DIRECTION_PORT ^= _BV(DIRECTION_BIT)
#define DIRECTION_SetConditional(condition)      if (condition) DIRECTION_SetActive(); else DIRECTION_SetInactive()
#define DIRECTION_SetConditionalToggle(conditionon, conditionoff, conditiontoggle) if (conditionon) { DIRECTION_SetActive(); } else if (conditionoff) { DIRECTION_SetInactive(); } else if (conditiontoggle) { DIRECTION_Toggle(); }
#define DIRECTION_Pulse()                        { DIRECTION_SetActive(); DIRECTION_SetInactive(); }
#define DIRECTION_PulseNop()                     { DIRECTION_SetActive(); nop(); DIRECTION_SetInactive(); }
#define DIRECTION_IsInactive()                   (DIRECTION_Get() == 0)
#define DIRECTION_IsActive()                     (DIRECTION_Get() != 0)
#define DIRECTION_Init()                         { DIRECTION_SetInactive(); DIRECTION_ModeOutput(); }

/* STEP -> PC3 (Output, Initially Inactive) */
#define STEP_BIT                                 3
#define STEP_PIN                                 PINC
#define STEP_PORT                                PORTC
#define STEP_DDR                                 DDRC
#define STEP_ModeOutput()                        STEP_DDR |= _BV(STEP_BIT)
#define STEP_IsOutput()                          ((STEP_DDR & _BV(STEP_BIT)) != 0)
#define STEP_SetHIGH()                           STEP_PORT |= _BV(STEP_BIT)
#define STEP_SetLOW()                            STEP_PORT &= ~_BV(STEP_BIT)
#define STEP_Get()                               (STEP_PIN & _BV(STEP_BIT))
#define STEP_SetInactive()                       STEP_SetLOW()
#define STEP_SetActive()                         STEP_SetHIGH()
#define STEP_Toggle()                            STEP_PORT ^= _BV(STEP_BIT)
#define STEP_SetConditional(condition)           if (condition) STEP_SetActive(); else STEP_SetInactive()
#define STEP_SetConditionalToggle(conditionon, conditionoff, conditiontoggle) if (conditionon) { STEP_SetActive(); } else if (conditionoff) { STEP_SetInactive(); } else if (conditiontoggle) { STEP_Toggle(); }
#define STEP_Pulse()                             { STEP_SetActive(); STEP_SetInactive(); }
#define STEP_PulseNop()                          { STEP_SetActive(); nop(); STEP_SetInactive(); }
#define STEP_IsInactive()                        (STEP_Get() == 0)
#define STEP_IsActive()                          (STEP_Get() != 0)
#define STEP_Init()                              { STEP_SetInactive(); STEP_ModeOutput(); }

/* SLEEP -> PB7 (Output, Initially Inactive, Active-Low) */
#define SLEEP_BIT                                7
#define SLEEP_PIN                                PINB
#define SLEEP_PORT                               PORTB
#define SLEEP_DDR                                DDRB
#define SLEEP_ModeOutput()                       SLEEP_DDR |= _BV(SLEEP_BIT)
#define SLEEP_IsOutput()                         ((SLEEP_DDR & _BV(SLEEP_BIT)) != 0)
#define SLEEP_SetHIGH()                          SLEEP_PORT |= _BV(SLEEP_BIT)
#define SLEEP_SetLOW()                           SLEEP_PORT &= ~_BV(SLEEP_BIT)
#define SLEEP_Get()                              (SLEEP_PIN & _BV(SLEEP_BIT))
#define SLEEP_SetInactive()                      SLEEP_SetHIGH()
#define SLEEP_SetActive()                        SLEEP_SetLOW()
#define SLEEP_Toggle()                           SLEEP_PORT ^= _BV(SLEEP_BIT)
#define SLEEP_SetConditional(condition)          if (condition) SLEEP_SetActive(); else SLEEP_SetInactive()
#define SLEEP_SetConditionalToggle(conditionon, conditionoff, conditiontoggle) if (conditionon) { SLEEP_SetActive(); } else if (conditionoff) { SLEEP_SetInactive(); } else if (conditiontoggle) { SLEEP_Toggle(); }
#define SLEEP_Pulse()                            { SLEEP_SetActive(); SLEEP_SetInactive(); }
#define SLEEP_PulseNop()                         { SLEEP_SetActive(); nop(); SLEEP_SetInactive(); }
#define SLEEP_IsInactive()                       (SLEEP_Get() != 0)
#define SLEEP_IsActive()                         (SLEEP_Get() == 0)
#define SLEEP_Init()                             { SLEEP_SetInactive(); SLEEP_ModeOutput(); }

/* RESET -> PB6 (Output, Initially Inactive, Active-Low) */
#define RESET_BIT                                6
#define RESET_PIN                                PINB
#define RESET_PORT                               PORTB
#define RESET_DDR                                DDRB
#define RESET_ModeOutput()                       RESET_DDR |= _BV(RESET_BIT)
#define RESET_IsOutput()                         ((RESET_DDR & _BV(RESET_BIT)) != 0)
#define RESET_SetHIGH()                          RESET_PORT |= _BV(RESET_BIT)
#define RESET_SetLOW()                           RESET_PORT &= ~_BV(RESET_BIT)
#define RESET_Get()                              (RESET_PIN & _BV(RESET_BIT))
#define RESET_SetInactive()                      RESET_SetHIGH()
#define RESET_SetActive()                        RESET_SetLOW()
#define RESET_Toggle()                           RESET_PORT ^= _BV(RESET_BIT)
#define RESET_SetConditional(condition)          if (condition) RESET_SetActive(); else RESET_SetInactive()
#define RESET_SetConditionalToggle(conditionon, conditionoff, conditiontoggle) if (conditionon) { RESET_SetActive(); } else if (conditionoff) { RESET_SetInactive(); } else if (conditiontoggle) { RESET_Toggle(); }
#define RESET_Pulse()                            { RESET_SetActive(); RESET_SetInactive(); }
#define RESET_PulseNop()                         { RESET_SetActive(); nop(); RESET_SetInactive(); }
#define RESET_IsInactive()                       (RESET_Get() != 0)
#define RESET_IsActive()                         (RESET_Get() == 0)
#define RESET_Init()                             { RESET_SetInactive(); RESET_ModeOutput(); }

/* MS3 -> PF3 (Output, Initially Inactive) */
#define MS3_BIT                                  3
#define MS3_PIN                                  PINF
#define MS3_PORT                                 PORTF
#define MS3_DDR                                  DDRF
#define MS3_ModeOutput()                         MS3_DDR |= _BV(MS3_BIT)
#define MS3_IsOutput()                           ((MS3_DDR & _BV(MS3_BIT)) != 0)
#define MS3_SetHIGH()                            MS3_PORT |= _BV(MS3_BIT)
#define MS3_SetLOW()                             MS3_PORT &= ~_BV(MS3_BIT)
#define MS3_Get()                                (MS3_PIN & _BV(MS3_BIT))
#define MS3_SetInactive()                        MS3_SetLOW()
#define MS3_SetActive()                          MS3_SetHIGH()
#define MS3_Toggle()                             MS3_PORT ^= _BV(MS3_BIT)
#define MS3_SetConditional(condition)            if (condition) MS3_SetActive(); else MS3_SetInactive()
#define MS3_SetConditionalToggle(conditionon, conditionoff, conditiontoggle) if (conditionon) { MS3_SetActive(); } else if (conditionoff) { MS3_SetInactive(); } else if (conditiontoggle) { MS3_Toggle(); }
#define MS3_Pulse()                              { MS3_SetActive(); MS3_SetInactive(); }
#define MS3_PulseNop()                           { MS3_SetActive(); nop(); MS3_SetInactive(); }
#define MS3_IsInactive()                         (MS3_Get() == 0)
#define MS3_IsActive()                           (MS3_Get() != 0)
#define MS3_Init()                               { MS3_SetInactive(); MS3_ModeOutput(); }

/* MS2 -> PF2 (Output, Initially Inactive) */
#define MS2_BIT                                  2
#define MS2_PIN                                  PINF
#define MS2_PORT                                 PORTF
#define MS2_DDR                                  DDRF
#define MS2_ModeOutput()                         MS2_DDR |= _BV(MS2_BIT)
#define MS2_IsOutput()                           ((MS2_DDR & _BV(MS2_BIT)) != 0)
#define MS2_SetHIGH()                            MS2_PORT |= _BV(MS2_BIT)
#define MS2_SetLOW()                             MS2_PORT &= ~_BV(MS2_BIT)
#define MS2_Get()                                (MS2_PIN & _BV(MS2_BIT))
#define MS2_SetInactive()                        MS2_SetLOW()
#define MS2_SetActive()                          MS2_SetHIGH()
#define MS2_Toggle()                             MS2_PORT ^= _BV(MS2_BIT)
#define MS2_SetConditional(condition)            if (condition) MS2_SetActive(); else MS2_SetInactive()
#define MS2_SetConditionalToggle(conditionon, conditionoff, conditiontoggle) if (conditionon) { MS2_SetActive(); } else if (conditionoff) { MS2_SetInactive(); } else if (conditiontoggle) { MS2_Toggle(); }
#define MS2_Pulse()                              { MS2_SetActive(); MS2_SetInactive(); }
#define MS2_PulseNop()                           { MS2_SetActive(); nop(); MS2_SetInactive(); }
#define MS2_IsInactive()                         (MS2_Get() == 0)
#define MS2_IsActive()                           (MS2_Get() != 0)
#define MS2_Init()                               { MS2_SetInactive(); MS2_ModeOutput(); }

/* MS1 -> PF1 (Output, Initially Inactive) */
#define MS1_BIT                                  1
#define MS1_PIN                                  PINF
#define MS1_PORT                                 PORTF
#define MS1_DDR                                  DDRF
#define MS1_ModeOutput()                         MS1_DDR |= _BV(MS1_BIT)
#define MS1_IsOutput()                           ((MS1_DDR & _BV(MS1_BIT)) != 0)
#define MS1_SetHIGH()                            MS1_PORT |= _BV(MS1_BIT)
#define MS1_SetLOW()                             MS1_PORT &= ~_BV(MS1_BIT)
#define MS1_Get()                                (MS1_PIN & _BV(MS1_BIT))
#define MS1_SetInactive()                        MS1_SetLOW()
#define MS1_SetActive()                          MS1_SetHIGH()
#define MS1_Toggle()                             MS1_PORT ^= _BV(MS1_BIT)
#define MS1_SetConditional(condition)            if (condition) MS1_SetActive(); else MS1_SetInactive()
#define MS1_SetConditionalToggle(conditionon, conditionoff, conditiontoggle) if (conditionon) { MS1_SetActive(); } else if (conditionoff) { MS1_SetInactive(); } else if (conditiontoggle) { MS1_Toggle(); }
#define MS1_Pulse()                              { MS1_SetActive(); MS1_SetInactive(); }
#define MS1_PulseNop()                           { MS1_SetActive(); nop(); MS1_SetInactive(); }
#define MS1_IsInactive()                         (MS1_Get() == 0)
#define MS1_IsActive()                           (MS1_Get() != 0)
#define MS1_Init()                               { MS1_SetInactive(); MS1_ModeOutput(); }

/* ENABLE -> PF0 (Output, Initially Inactive, Active-Low) */
#define ENABLE_BIT                               0
#define ENABLE_PIN                               PINF
#define ENABLE_PORT                              PORTF
#define ENABLE_DDR                               DDRF
#define ENABLE_ModeOutput()                      ENABLE_DDR |= _BV(ENABLE_BIT)
#define ENABLE_IsOutput()                        ((ENABLE_DDR & _BV(ENABLE_BIT)) != 0)
#define ENABLE_SetHIGH()                         ENABLE_PORT |= _BV(ENABLE_BIT)
#define ENABLE_SetLOW()                          ENABLE_PORT &= ~_BV(ENABLE_BIT)
#define ENABLE_Get()                             (ENABLE_PIN & _BV(ENABLE_BIT))
#define ENABLE_SetInactive()                     ENABLE_SetHIGH()
#define ENABLE_SetActive()                       ENABLE_SetLOW()
#define ENABLE_Toggle()                          ENABLE_PORT ^= _BV(ENABLE_BIT)
#define ENABLE_SetConditional(condition)         if (condition) ENABLE_SetActive(); else ENABLE_SetInactive()
#define ENABLE_SetConditionalToggle(conditionon, conditionoff, conditiontoggle) if (conditionon) { ENABLE_SetActive(); } else if (conditionoff) { ENABLE_SetInactive(); } else if (conditiontoggle) { ENABLE_Toggle(); }
#define ENABLE_Pulse()                           { ENABLE_SetActive(); ENABLE_SetInactive(); }
#define ENABLE_PulseNop()                        { ENABLE_SetActive(); nop(); ENABLE_SetInactive(); }
#define ENABLE_IsInactive()                      (ENABLE_Get() != 0)
#define ENABLE_IsActive()                        (ENABLE_Get() == 0)
#define ENABLE_Init()                            { ENABLE_SetInactive(); ENABLE_ModeOutput(); }

/* SWITCH_UP -> PA0 (Input, Initially Pullup On, Active-Low) */
#define SWITCH_UP_BIT                            0
#define SWITCH_UP_PIN                            PINA
#define SWITCH_UP_PORT                           PORTA
#define SWITCH_UP_DDR                            DDRA
#define SWITCH_UP_SetPullupActive()              SWITCH_UP_PORT |= _BV(SWITCH_UP_BIT)
#define SWITCH_UP_SetPullupInactive()            SWITCH_UP_PORT &= ~_BV(SWITCH_UP_BIT)
#define SWITCH_UP_ModeInput()                    SWITCH_UP_DDR &= ~_BV(SWITCH_UP_BIT)
#define SWITCH_UP_IsInput()                      ((SWITCH_UP_DDR & _BV(SWITCH_UP_BIT)) == 0)
#define SWITCH_UP_Get()                          (SWITCH_UP_PIN & _BV(SWITCH_UP_BIT))
#define SWITCH_UP_GetBit()                       (SWITCH_UP_Get() >> SWITCH_UP_BIT)
#define SWITCH_UP_IsInactive()                   (SWITCH_UP_Get() != 0)
#define SWITCH_UP_IsActive()                     (SWITCH_UP_Get() == 0)
#define SWITCH_UP_Init()                         { SWITCH_UP_SetPullupActive(); SWITCH_UP_ModeInput(); }

/* SWITCH_DOWN -> PA1 (Input, Initially Pullup On, Active-Low) */
#define SWITCH_DOWN_BIT                          1
#define SWITCH_DOWN_PIN                          PINA
#define SWITCH_DOWN_PORT                         PORTA
#define SWITCH_DOWN_DDR                          DDRA
#define SWITCH_DOWN_SetPullupActive()            SWITCH_DOWN_PORT |= _BV(SWITCH_DOWN_BIT)
#define SWITCH_DOWN_SetPullupInactive()          SWITCH_DOWN_PORT &= ~_BV(SWITCH_DOWN_BIT)
#define SWITCH_DOWN_ModeInput()                  SWITCH_DOWN_DDR &= ~_BV(SWITCH_DOWN_BIT)
#define SWITCH_DOWN_IsInput()                    ((SWITCH_DOWN_DDR & _BV(SWITCH_DOWN_BIT)) == 0)
#define SWITCH_DOWN_Get()                        (SWITCH_DOWN_PIN & _BV(SWITCH_DOWN_BIT))
#define SWITCH_DOWN_GetBit()                     (SWITCH_DOWN_Get() >> SWITCH_DOWN_BIT)
#define SWITCH_DOWN_IsInactive()                 (SWITCH_DOWN_Get() != 0)
#define SWITCH_DOWN_IsActive()                   (SWITCH_DOWN_Get() == 0)
#define SWITCH_DOWN_Init()                       { SWITCH_DOWN_SetPullupActive(); SWITCH_DOWN_ModeInput(); }

/* SWITCH_MIDDLE -> PA2 (Input, Initially Pullup On, Active-Low) */
#define SWITCH_MIDDLE_BIT                        2
#define SWITCH_MIDDLE_PIN                        PINA
#define SWITCH_MIDDLE_PORT                       PORTA
#define SWITCH_MIDDLE_DDR                        DDRA
#define SWITCH_MIDDLE_SetPullupActive()          SWITCH_MIDDLE_PORT |= _BV(SWITCH_MIDDLE_BIT)
#define SWITCH_MIDDLE_SetPullupInactive()        SWITCH_MIDDLE_PORT &= ~_BV(SWITCH_MIDDLE_BIT)
#define SWITCH_MIDDLE_ModeInput()                SWITCH_MIDDLE_DDR &= ~_BV(SWITCH_MIDDLE_BIT)
#define SWITCH_MIDDLE_IsInput()                  ((SWITCH_MIDDLE_DDR & _BV(SWITCH_MIDDLE_BIT)) == 0)
#define SWITCH_MIDDLE_Get()                      (SWITCH_MIDDLE_PIN & _BV(SWITCH_MIDDLE_BIT))
#define SWITCH_MIDDLE_GetBit()                   (SWITCH_MIDDLE_Get() >> SWITCH_MIDDLE_BIT)
#define SWITCH_MIDDLE_IsInactive()               (SWITCH_MIDDLE_Get() != 0)
#define SWITCH_MIDDLE_IsActive()                 (SWITCH_MIDDLE_Get() == 0)
#define SWITCH_MIDDLE_Init()                     { SWITCH_MIDDLE_SetPullupActive(); SWITCH_MIDDLE_ModeInput(); }

/* RELAY -> PA6 (Output, Initially Inactive) */
#define RELAY_BIT                                6
#define RELAY_PIN                                PINA
#define RELAY_PORT                               PORTA
#define RELAY_DDR                                DDRA
#define RELAY_ModeOutput()                       RELAY_DDR |= _BV(RELAY_BIT)
#define RELAY_IsOutput()                         ((RELAY_DDR & _BV(RELAY_BIT)) != 0)
#define RELAY_SetHIGH()                          RELAY_PORT |= _BV(RELAY_BIT)
#define RELAY_SetLOW()                           RELAY_PORT &= ~_BV(RELAY_BIT)
#define RELAY_Get()                              (RELAY_PIN & _BV(RELAY_BIT))
#define RELAY_SetInactive()                      RELAY_SetLOW()
#define RELAY_SetActive()                        RELAY_SetHIGH()
#define RELAY_Toggle()                           RELAY_PORT ^= _BV(RELAY_BIT)
#define RELAY_SetConditional(condition)          if (condition) RELAY_SetActive(); else RELAY_SetInactive()
#define RELAY_SetConditionalToggle(conditionon, conditionoff, conditiontoggle) if (conditionon) { RELAY_SetActive(); } else if (conditionoff) { RELAY_SetInactive(); } else if (conditiontoggle) { RELAY_Toggle(); }
#define RELAY_Pulse()                            { RELAY_SetActive(); RELAY_SetInactive(); }
#define RELAY_PulseNop()                         { RELAY_SetActive(); nop(); RELAY_SetInactive(); }
#define RELAY_IsInactive()                       (RELAY_Get() == 0)
#define RELAY_IsActive()                         (RELAY_Get() != 0)
#define RELAY_Init()                             { RELAY_SetInactive(); RELAY_ModeOutput(); }

#define initHAL() {\
		DIRECTION_Init();\
		STEP_Init();\
		SLEEP_Init();\
		RESET_Init();\
		MS3_Init();\
		MS2_Init();\
		MS1_Init();\
		ENABLE_Init();\
		SWITCH_UP_Init();\
		SWITCH_DOWN_Init();\
		SWITCH_MIDDLE_Init();\
		RELAY_Init();\
}

#endif
