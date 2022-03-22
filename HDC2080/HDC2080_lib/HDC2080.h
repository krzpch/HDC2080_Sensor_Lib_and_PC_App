#ifndef _HDC2080_H_
#define _HDC2080_H_

#define TEMPERATURE_LOW_REG                 0x00
#define TEMPERATURE_HIGH_REG                0x01
#define HUMIDITY_LOW_REG                    0x02
#define HUMIDITY_HIGH_REG                   0x03
#define INTERRUPT_DRDY_REG                  0x04
#define TEMPERATURE_MAX_REG                 0x05
#define HUMIDITY_MAX_REG                    0x06
#define INTERRUPT_ENABLE_REG                0x07
#define TEMP_OFFSET_ADJUST_REG              0x08
#define HUM_OFFSET_ADJUST_REG               0x09
#define TEMP_THR_L_REG                      0x0A
#define TEMP_THR_H_REG                      0x0B
#define RH_THR_L_REG                        0x0C
#define RH_THR_H_REG                        0x0D
#define RESET_DRDY_INT_CONF_REG             0x0E
#define MEASUREMENT_CONFIGURATION_REG       0x0F
#define MANUFACTURER_ID_LOW_REG             0xFC
#define MANUFACTURER_ID_HIGH_REG            0xFD
#define DEVICE_ID_LOW_REG                   0xFE
#define DEVICE_ID_HIGH_REG                  0xFF

#define INTERRUPT_DRDY_DRDY_STATUS          0x80
#define INTERRUPT_DRDY_TH_STATUS            0x40
#define INTERRUPT_DRDY_TL_STATUS            0x20  
#define INTERRUPT_DRDY_HH_STATUS            0x10   
#define INTERRUPT_DRDY_HL_STATUS            0x08

#define INTERRUPT_ENABLE_DRDY_ENABLE        0x80
#define INTERRUPT_ENABLE_TH_ENABLE          0x40
#define INTERRUPT_ENABLE_TL_ENABLE          0x20
#define INTERRUPT_ENABLE_HH_ENABLE          0x10
#define INTERRUPT_ENABLE_HL_ENABLE          0x08

#define TEMP_OFFSET_ADJUST_7        0x80
#define TEMP_OFFSET_ADJUST_6        0x40
#define TEMP_OFFSET_ADJUST_5        0x20 
#define TEMP_OFFSET_ADJUST_4        0x10
#define TEMP_OFFSET_ADJUST_3        0x08
#define TEMP_OFFSET_ADJUST_2        0x04
#define TEMP_OFFSET_ADJUST_1        0x02
#define TEMP_OFFSET_ADJUST_0        0x01
 
#define HUM_OFFSET_ADJUST_7         0x80       
#define HUM_OFFSET_ADJUST_6         0x40  
#define HUM_OFFSET_ADJUST_5         0x20  
#define HUM_OFFSET_ADJUST_4         0x10  
#define HUM_OFFSET_ADJUST_3         0x08  
#define HUM_OFFSET_ADJUST_2         0x04
#define HUM_OFFSET_ADJUST_1         0x02
#define HUM_OFFSET_ADJUST_0         0x01  

#define RESET_DRDY_INT_CONF_SOFT_RES        0x80
#define RESET_DRDY_INT_CONF_AMM_2           0x40
#define RESET_DRDY_INT_CONF_AMM_1           0x20
#define RESET_DRDY_INT_CONF_AMM_0           0x10
#define RESET_DRDY_INT_CONF_HEAT_EN         0x08
#define RESET_DRDY_INT_CONF_DRDY_INT_EN     0x04
#define RESET_DRDY_INT_CONF_INT_POL         0x02
#define RESET_DRDY_INT_CONF_INT_MODE        0x01

#define MEASUREMENT_CONFIGURATION_TRES_1         0x80
#define MEASUREMENT_CONFIGURATION_TRES_0         0x40
#define MEASUREMENT_CONFIGURATION_HRES_1         0x20
#define MEASUREMENT_CONFIGURATION_HRES_0         0x10
#define MEASUREMENT_CONFIGURATION_RES           0x08
#define MEASUREMENT_CONFIGURATION_MEAS_CONF_1    0x04
#define MEASUREMENT_CONFIGURATION_MEAS_CONF_0    0x02
#define MEASUREMENT_CONFIGURATION_MEAS_TRIG     0x01



#endif