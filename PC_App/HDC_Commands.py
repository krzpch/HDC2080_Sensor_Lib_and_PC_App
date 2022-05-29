##
# @author: Krzysztof Półchłopek
# @date: 03.04.2022
#



## Class containing all funtions, configuration and data that can be used to controll sensor
from setuptools import Command


class HDC2080():
    ## Dictionaries for initialization command
    __config_dict           = {0: "temp&hum", 1: "onlyTemp"}
    __auto_mes_dict         = {0: "autoOff", 1: "1/120Hz", 2: "1/60Hz", 3: "1/10Hz", 4: "1/5Hz", 5: "1Hz", 6: "2Hz", 7: "5Hz"}
    __res_dict              = {0: "res14bit", 1: "res11bit", 2: "res9bit"}
    __int_en_dict           = {0: "intDis", 1: "intAL", 2: "intAH"}
    __int_mode_dict         = {0: "levelSens", 1: "CompMode"}
    __int_select_dict       = {0: "intTempMaxEn", 1: "intTempMinEn", 2: "intHumMaxEn", 3: "intHumMinEn", 4: "intDataReadyEn"}
    
    ## The Constructor
    def __init__(self):
        pass

    ## Method for initialization of sensor
    def init(self, data) -> str:
        command = "sensor_initialize " + self.__config_dict[data[0]] + " " + self.__auto_mes_dict[data[1]] + "\n"
        return command

    def config_temp(self, data) -> str:
        command = "config_temperature " + self.__res_dict[data[0]] + " temp_offset " + data[1] + "\n"
        return command
    
    def config_hum(self, data) -> str:
        command = "config_humidity " + self.__res_dict[data[0]] + " hum_offset " + data[1] + "\n"
        return command
    
    def config_int(self, data) -> str:
        # Check if interrutps are enabled
        if data[0] != 0: 
            int_val = ""
            if data[2] == 0:
                int_val = data[3]
            elif data[2] == 1:
                int_val = data[4]
            elif data[2] == 2:
                int_val = data[5]
            elif data[2] == 3:
                int_val = data[6]
            elif data[2] == 4:
                pass
            
            command = "config_interrupts " + self.__int_en_dict[data[0]] + " " + self.__int_mode_dict[data[1]] + " " + \
                self.__int_select_dict[data[2]] + " " + int_val + "\n"
                
        else:
            command = "config_interrupts " + self.__int_en_dict[data[0]] + "\n"
            
        return command
    
    def deinit(self) -> str:
        command = "sensor_deinitialize\n"
        return command

    def force_measurement(self) -> str:
        command = "force_measurement\n"
        return command

    def recv_temp(self) -> str:
        command = "get_temperature\n"
        return command

    def recv_hum(self) -> str:
        command = "get_humidity\n"
        return command
    
    def recv_mesurements(self) -> str:
        command = "get_measurements\n"
        return command