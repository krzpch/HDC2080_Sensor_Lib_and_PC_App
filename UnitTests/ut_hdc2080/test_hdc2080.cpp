#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <iostream>

extern "C"
{
#include "cli_hdc2080.c"
}

#define HUMIDITY_TO_HEX(x)      (static_cast<int>((x)*65536.0 / 100.0))
#define TEMPERATURE_TO_HEX(x)   (static_cast<int>((x + 40.5) * 65536.0 / 165.0))

#define WRITE16_BIT_REG(address, value) \
    (address)[0] = ((value & 0xFFU));   \
    (address)[1] = ((value >> 8U) & 0xFFU);


void Uart_Test_Send_String(std::string text)
{
    for (size_t i = 0; i < text.length(); i++)
    {
        CLI_Receive_Char(&HDC2080_CLI, text[i]);
    }
}

void Uart_Test_Receive_String(const std::string *text)
{
    for (size_t i = 0; i < text->length(); i++)
    {
        mock().expectOneCall("HAL_UART_Transmit")
              .withMemoryBufferParameter("pData", reinterpret_cast<const uint8_t *>(&(*text)[i]), 1);
    }
}

TEST_GROUP(HDC2080_TestGroup)
{
    void setup()
    {
        /* Reset CLI before test */
        CLI_Init(&HDC2080_CLI);
    }

    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(HDC2080_TestGroup, CLI_Wrong_Command_Process)
{
    const std::string wrong_cmd = "this_is_wrong_command arg1 arg2\n";
    const std::string expected_response_wrong = "Command Not Found\n";
    const std::string get_temperature_cmd = "get_temperature\n";
    const std::string expected_response_temperature_cmd = "TEMP 23.7\n";

    /* Send wrong command from PC */
    Uart_Test_Send_String(wrong_cmd);
    /* Send proper command from PC */
    Uart_Test_Send_String(get_temperature_cmd);

    /* Check if MCU response with cmd not found */
    Uart_Test_Receive_String(&expected_response_wrong);
    /* Check if MCU response with proper temperature */
    Uart_Test_Receive_String(&expected_response_temperature_cmd);

    uint8_t i2c_ddrd_reg = INTERRUPT_DRDY_DRDY_STATUS;
    uint8_t i2c_temperature_reg[2] = {};
    WRITE16_BIT_REG(i2c_temperature_reg, TEMPERATURE_TO_HEX(23.7));

    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", INTERRUPT_DRDY_REG)
        .withOutputParameterReturning("pData", &i2c_ddrd_reg, sizeof(i2c_ddrd_reg))
        .withParameter("Size", 1);

    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", TEMPERATURE_LOW_REG)
        .withOutputParameterReturning("pData", i2c_temperature_reg, sizeof(i2c_temperature_reg))
        .withParameter("Size", 2);

    CHECK_EQUAL(CLI_CMD_NOT_FOUND, CLI_Process_Comand(&HDC2080_CLI));
    CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
    CHECK_EQUAL(CLI_CMD_NOT_PENDING, CLI_Process_Comand(&HDC2080_CLI));
}

TEST(HDC2080_TestGroup, CLI_Too_Many_Arguments_Process)
{
    /* Send "sensor_initalize" command with too many argument bytes (over 64) */
    /* Expectations: CLI should process command and discard arguments if the number of processed bytes is more than 64 */
    const std::string hdc_init_cmd = "sensor_initialize ";
    const std::string temp_hum_cmd = "temp&hum ";
    const std::string expected_response_1 = "softRes OK\n";
    const std::string expected_response_2 = "temp&hum OK\n";

    /* Send init command */
    Uart_Test_Send_String(hdc_init_cmd);
    /* Send arguments - tests for segmeneted messages */
    for (int i = 0; i < 8; i++)
    {
        Uart_Test_Send_String(temp_hum_cmd);
    }
    Uart_Test_Send_String("\n");

    /* Check if the response sent to the UART is correct */
    Uart_Test_Receive_String(&expected_response_1);

    /* soft reset mock */
    uint8_t i2c_soft_reset_reg = RESET_DRDY_INT_CONF_SOFT_RES;
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", RESET_DRDY_INT_CONF_REG)
        .withMemoryBufferParameter("pData", &i2c_soft_reset_reg, sizeof(i2c_soft_reset_reg));

    for (int i = 0; i < 5; i++)
    {
        Uart_Test_Receive_String(&expected_response_2);
        /* MEAS_CONF[2:1] = 11 = NA, other bits set to 1 to make sure they won't be cleared */
        uint8_t i2c_measure_reg = 0b11111111;
        /* MEAS_CONF[2:1] = 00 = Humidity + Temperature */
        uint8_t i2c_expected_measure_reg = 0b11111001;
        mock().expectOneCall("HAL_I2C_Mem_Read")
            .withParameter("MemAddress", MEASUREMENT_CONFIGURATION_REG)
            .withOutputParameterReturning("pData", &i2c_measure_reg, sizeof(i2c_measure_reg))
            .withParameter("Size", 1);
        mock().expectOneCall("HAL_I2C_Mem_Write")
            .withParameter("MemAddress", MEASUREMENT_CONFIGURATION_REG)
            .withMemoryBufferParameter("pData", &i2c_expected_measure_reg, sizeof(i2c_expected_measure_reg));
    }

    CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
    CHECK_EQUAL(CLI_CMD_NOT_PENDING, CLI_Process_Comand(&HDC2080_CLI));
}

TEST(HDC2080_TestGroup, HDC2080_Cmd_Init_Temp_Hum_Auto_Off)
{
    /* Initialize sensor with next settings:
       1. Temperature and Humidity on
       2. AMM Rate mode off */
    const std::string hdc_init_cmd = "sensor_initialize temp&hum autoOff\n";
    const std::string expected_response_1 = "softRes OK\n";
    const std::string expected_response_2 = "temp&hum OK\n";
    const std::string expected_response_3 = "autoOff OK\n";

    /* Send init command */
    Uart_Test_Send_String(hdc_init_cmd);

    /* Check if the response sent to the UART is correct */
    Uart_Test_Receive_String(&expected_response_1);
    Uart_Test_Receive_String(&expected_response_2);
    Uart_Test_Receive_String(&expected_response_3);

    /* soft reset mock */
    uint8_t i2c_soft_reset_reg = RESET_DRDY_INT_CONF_SOFT_RES;
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", RESET_DRDY_INT_CONF_REG)
        .withMemoryBufferParameter("pData", &i2c_soft_reset_reg, sizeof(i2c_soft_reset_reg));

    /* MEAS_CONF[2:1] = 11 = NA, other bits set to 1 to make sure they won't be cleared */
    uint8_t i2c_measure_reg = 0b11111111;
    /* MEAS_CONF[2:1] = 00 = Humidity + Temperature */
    uint8_t i2c_expected_measure_reg = 0b11111001;
    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", MEASUREMENT_CONFIGURATION_REG)
        .withOutputParameterReturning("pData", &i2c_measure_reg, sizeof(i2c_measure_reg))
        .withParameter("Size", 1);
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", MEASUREMENT_CONFIGURATION_REG)
        .withMemoryBufferParameter("pData", &i2c_expected_measure_reg, sizeof(i2c_expected_measure_reg));

    /* AMM[6:4] = 111 = 5Hz, other bits set to 1 to make sure they won't be cleared */
    uint8_t i2c_int_reg = 0b11111111;
    /* AMM[6:4] = 000 = AMM disabled. */
    uint8_t i2c_expected_am_reg = 0b10001111;
    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", RESET_DRDY_INT_CONF_REG)
        .withOutputParameterReturning("pData", &i2c_int_reg, sizeof(i2c_int_reg))
        .withParameter("Size", 1);
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", RESET_DRDY_INT_CONF_REG)
        .withMemoryBufferParameter("pData", &i2c_expected_am_reg, sizeof(i2c_expected_am_reg));

    CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
    CHECK_EQUAL(CLI_CMD_NOT_PENDING, CLI_Process_Comand(&HDC2080_CLI));
}

TEST(HDC2080_TestGroup, HDC2080_Cmd_Sensor_Int_Al_LevelSens_HumMax)
{
    /* Initialize sensor interrupts with next settings:
       1. intAL - pin active low
       2. levelSens - Level_Sensitive
       3. intHumMaxEn 30.0 - Humidity max interrupt enabled (30.0%)
       */
    const std::string hdc_config_interrupts_cmd = "config_interrupts intAL levelSens intHumMaxEn 30.0\n";
    const std::string expected_response_1 = "intAL OK\n";
    const std::string expected_response_2 = "levelSens OK\n";
    const std::string expected_response_3 = "intHumMaxEn OK, float: 30.0\n";
    const std::string expected_response_4 = "HDC2080_Set_INT_Pin_Configuration: 0\n";
    const std::string expected_response_5 = "HDC2080_Set_Interrupt_Configuration: 0\n";
    const std::string expected_response_6 = "HDC2080_Start_Conversion: 0\n";

    /* Send init command */
    Uart_Test_Send_String(hdc_config_interrupts_cmd);

    /* Check if the response sent to the UART is correct */
    Uart_Test_Receive_String(&expected_response_1);
    Uart_Test_Receive_String(&expected_response_2);
    Uart_Test_Receive_String(&expected_response_3);

    /* intHumMaxEn 30.0 mock */
    uint8_t th_thr_high_reg = 0x4C;
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", RH_THR_H_REG)
        .withMemoryBufferParameter("pData", &th_thr_high_reg, sizeof(th_thr_high_reg));

    /* [2:0] = 111, other bits set to 1 to make sure they won't be cleared */
    uint8_t i2c_reset_reg = 0b11111111;
    /* [2:0] = 100 = Level sensitive + Active Low + INT_EN Enable */
    uint8_t i2c_expected_reset_reg = 0b11111100;
    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", RESET_DRDY_INT_CONF_REG)
        .withOutputParameterReturning("pData", &i2c_reset_reg, sizeof(i2c_reset_reg))
        .withParameter("Size", 1);
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", RESET_DRDY_INT_CONF_REG)
        .withMemoryBufferParameter("pData", &i2c_expected_reset_reg, sizeof(i2c_expected_reset_reg));
    Uart_Test_Receive_String(&expected_response_4);

    /* HH_enbale (humidity threshold high interrupt) */
    uint8_t i2c_expected_interrupt_reg = 0b00010000;
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", INTERRUPT_ENABLE_REG)
        .withMemoryBufferParameter("pData", &i2c_expected_interrupt_reg, sizeof(i2c_expected_interrupt_reg));
    Uart_Test_Receive_String(&expected_response_5);

    /* MEAS_TRIG[0] = 0 = no action*/
    uint8_t i2c_measure_reg = 0b10100000;
    /* MEAS_CONF[2:1] = 00 = Humidity + Temperature */
    uint8_t i2c_expected_measure_reg = 0b10100001;
    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", MEASUREMENT_CONFIGURATION_REG)
        .withOutputParameterReturning("pData", &i2c_measure_reg, sizeof(i2c_measure_reg))
        .withParameter("Size", 1);
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", MEASUREMENT_CONFIGURATION_REG)
        .withMemoryBufferParameter("pData", &i2c_expected_measure_reg, sizeof(i2c_expected_measure_reg));
    Uart_Test_Receive_String(&expected_response_6);
    
    CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
    CHECK_EQUAL(CLI_CMD_NOT_PENDING, CLI_Process_Comand(&HDC2080_CLI));
}

TEST(HDC2080_TestGroup, HDC2080_Cmd_DeInit)
{
    /* Send "sensor_deinitialize" command */
    /* Expectations: CLI should process command */
    /* send command with extra spaces */
    const std::string hdc_deinit_cmd = "        sensor_deinitialize       \n";
    /* expected response 0 = OK */
    const std::string expected_response = "HDC2080_Soft_Reset: 0\n";

    /* Send init command */
    Uart_Test_Send_String(hdc_deinit_cmd);

    /* Check if the response sent to the UART is correct */
    Uart_Test_Receive_String(&expected_response);

    /* soft reset mock */
    uint8_t i2c_soft_reset_reg = RESET_DRDY_INT_CONF_SOFT_RES;
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", RESET_DRDY_INT_CONF_REG)
        .withMemoryBufferParameter("pData", &i2c_soft_reset_reg, sizeof(i2c_soft_reset_reg));

    CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
    CHECK_EQUAL(CLI_CMD_NOT_PENDING, CLI_Process_Comand(&HDC2080_CLI));
}

TEST(HDC2080_TestGroup, HDC2080_Cmd_Init_Temp_Hum_Auto_5Hz_Bad_Argument)
{
    /* Initialize sensor with next settings:
       1. Temperature and Humidity on
       2. AMM Rate mode 5Hz
       3. Bad argument */
    /* Expectations: CLI should process command and send ERROR response */
    const std::string hdc_init_cmd = "sensor_initialize temp&hum bad_argument 5Hz\n";
    const std::string expected_response_1 = "softRes OK\n";
    const std::string expected_response_2 = "temp&hum OK\n";
    const std::string expected_response_3 = "Bad argument: bad_argument\n";
    const std::string expected_response_4 = "5Hz OK\n";

    /* Send init command */
    Uart_Test_Send_String(hdc_init_cmd);

    /* Check if the response sent to the UART is correct */
    Uart_Test_Receive_String(&expected_response_1);
    Uart_Test_Receive_String(&expected_response_2);
    Uart_Test_Receive_String(&expected_response_3);
    Uart_Test_Receive_String(&expected_response_4);

    /* soft reset mock */
    uint8_t i2c_soft_reset_reg = RESET_DRDY_INT_CONF_SOFT_RES;
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", RESET_DRDY_INT_CONF_REG)
        .withMemoryBufferParameter("pData", &i2c_soft_reset_reg, sizeof(i2c_soft_reset_reg));

    /* MEAS_CONF[2:1] = 11 = NA, other bits set to 1 to make sure they won't be cleared */
    uint8_t i2c_measure_reg = 0b11111111;
    /* MEAS_CONF[2:1] = 00 = Humidity + Temperature */
    uint8_t i2c_expected_measure_reg = 0b11111001;
    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", MEASUREMENT_CONFIGURATION_REG)
        .withOutputParameterReturning("pData", &i2c_measure_reg, sizeof(i2c_measure_reg))
        .withParameter("Size", 1);
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", MEASUREMENT_CONFIGURATION_REG)
        .withMemoryBufferParameter("pData", &i2c_expected_measure_reg, sizeof(i2c_expected_measure_reg));

    /* AMM[6:4] = 000 = Disabled, other bits set to 1 to make sure they won't be cleared */
    uint8_t i2c_int_reg = 0b10001111;
    /* AMM[6:4] = 111 = AMM 5Hz. */
    uint8_t i2c_expected_am_reg = 0b11111111;
    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", RESET_DRDY_INT_CONF_REG)
        .withOutputParameterReturning("pData", &i2c_int_reg, sizeof(i2c_int_reg))
        .withParameter("Size", 1);
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", RESET_DRDY_INT_CONF_REG)
        .withMemoryBufferParameter("pData", &i2c_expected_am_reg, sizeof(i2c_expected_am_reg));

    CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
    CHECK_EQUAL(CLI_CMD_NOT_PENDING, CLI_Process_Comand(&HDC2080_CLI));
}

TEST(HDC2080_TestGroup, CLI_Buffer_Over_Run)
{
    /* Send random chars to oveflow CLI buffer
       then send valid command "get_humidity" with invalid parameters */
    /* Expectations: CLI should process "get_humidity" command and dont care about parameters */
    const std::string alphanum =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, alphanum.size() - 1);
    std::string random_string;
    for (unsigned int i = 0; i < HDC2080_CLI_BUFFER_SIZE + 10U; i++)
    {
        random_string += alphanum[distribution(generator)];
    }

    /* Send random generated string */
    Uart_Test_Send_String(random_string + "\n");
    /* Check if no valid commend pending */
    const std::string expected_response_wrong = "Command Not Found\n";
    /* Check if MCU response with cmd not found */
    Uart_Test_Receive_String(&expected_response_wrong);

    /* Send a valid command with invalid parameters */
    const std::string get_humidity_cmd = "get_humidity not valid parameters here\n";
    const std::string expected_response_humidity_cmd = "HUM 50.0\n";

    Uart_Test_Send_String(get_humidity_cmd);
    Uart_Test_Receive_String(&expected_response_humidity_cmd);

    uint8_t i2c_ddrd_reg = INTERRUPT_DRDY_DRDY_STATUS;
    uint8_t i2c_humidity_reg[2];
    WRITE16_BIT_REG(i2c_humidity_reg, HUMIDITY_TO_HEX(50.0));

    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", INTERRUPT_DRDY_REG)
        .withOutputParameterReturning("pData", &i2c_ddrd_reg, sizeof(i2c_ddrd_reg))
        .withParameter("Size", 1);

    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", HUMIDITY_LOW_REG)
        .withOutputParameterReturning("pData", i2c_humidity_reg, sizeof(i2c_humidity_reg))
        .withParameter("Size", 2);

    /* Process random generated string - return CLI_CMD_NOT_FOUND */
    CHECK_EQUAL(CLI_CMD_NOT_FOUND, CLI_Process_Comand(&HDC2080_CLI));
    /* Process valid command with invalid parameters - return CLI_CMD_PROCESSED */
    CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
}

TEST(HDC2080_TestGroup, HDC2080_Humidity_Convert_Random_Test)
{
    /* Send get_humidity command using uart, generate random number (from 0.0 to 100.0)
       check if the answer matches the expectation (HUMIDITY_TO_HEX() macro) */

    const std::string get_humidity_cmd = "get_humidity\n";
    uint8_t i2c_ddrd_reg = INTERRUPT_DRDY_DRDY_STATUS;
    uint8_t i2c_expected_humidity_reg[2];
    std::stringstream ss;

    for (int i = 0; i < 20; i++)
    {
        /* generate random int from 0 to 1000 */
        std::random_device random_device;
        std::mt19937 generator(random_device());
        std::uniform_int_distribution<> distribution(0, 1000);
        int random_number = distribution(generator);

        /* Prepare expected uart response */
        ss.str(std::string());
        ss << std::fixed << std::setprecision(1) << random_number / 10.0;
        std::string expected_response_humidity_cmd = "HUM " +  ss.str() + "\n";

        /* Send get_humidity command */
        Uart_Test_Send_String(get_humidity_cmd);
        /* Check if MCU response with proper value */
        Uart_Test_Receive_String(&expected_response_humidity_cmd);

        /* Calculate the expected value of the register */
        WRITE16_BIT_REG(i2c_expected_humidity_reg, HUMIDITY_TO_HEX(random_number / 10.0));

        mock().expectOneCall("HAL_I2C_Mem_Read")
            .withParameter("MemAddress", INTERRUPT_DRDY_REG)
            .withOutputParameterReturning("pData", &i2c_ddrd_reg, sizeof(i2c_ddrd_reg))
            .withParameter("Size", 1);

        mock().expectOneCall("HAL_I2C_Mem_Read")
            .withParameter("MemAddress", HUMIDITY_LOW_REG)
            .withOutputParameterReturning("pData", i2c_expected_humidity_reg, sizeof(i2c_expected_humidity_reg))
            .withParameter("Size", 2);

        CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
        CHECK_EQUAL(CLI_CMD_NOT_PENDING, CLI_Process_Comand(&HDC2080_CLI));
    }
}

TEST(HDC2080_TestGroup, HDC2080_Temperature_Convert_Random_Test)
{
    /* Send get_temperature command using uart, generate random number (from -40.5 to 124.5)
       check if the answer matches the expectation (TEMPERATURE_TO_HEX() macro) */

    const std::string get_temperature_cmd = "get_temperature\n";
    uint8_t i2c_ddrd_reg = INTERRUPT_DRDY_DRDY_STATUS;
    uint8_t i2c_expected_temperature_reg[2];
    std::stringstream ss;

    for (int i = 0; i < 20; i++)
    {
        /* generate random int from -405 to 1250 */
        std::random_device random_device;
        std::mt19937 generator(random_device());
        std::uniform_int_distribution<> distribution(-405, 1245);
        int random_number = distribution(generator);

        /* Prepare expected uart response */
        ss.str(std::string());
        ss << std::fixed << std::setprecision(1) << random_number / 10.0;
        std::string expected_response_temperature_cmd = "TEMP " +  ss.str() + "\n";

        /* Send get_temperature command */
        Uart_Test_Send_String(get_temperature_cmd);
        /* Check if MCU response with proper value */
        Uart_Test_Receive_String(&expected_response_temperature_cmd);

        /* Calculate the expected value of the register */
        WRITE16_BIT_REG(i2c_expected_temperature_reg, TEMPERATURE_TO_HEX(random_number / 10.0));

        mock().expectOneCall("HAL_I2C_Mem_Read")
            .withParameter("MemAddress", INTERRUPT_DRDY_REG)
            .withOutputParameterReturning("pData", &i2c_ddrd_reg, sizeof(i2c_ddrd_reg))
            .withParameter("Size", 1);

        mock().expectOneCall("HAL_I2C_Mem_Read")
            .withParameter("MemAddress", TEMPERATURE_LOW_REG)
            .withOutputParameterReturning("pData", i2c_expected_temperature_reg, sizeof(i2c_expected_temperature_reg))
            .withParameter("Size", 2);
    
        CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
        CHECK_EQUAL(CLI_CMD_NOT_PENDING, CLI_Process_Comand(&HDC2080_CLI));
    }
}

TEST(HDC2080_TestGroup, HDC2080_I2C_Bus_Error)
{
    /* I2C bus error during read */
    /* Expectations: CLI should process command and send ERROR response */
    const std::string get_measurements_cmd = "get_measurements\n";
    const std::string expected_response_error_cmd = "HDC2080_Get_Temperature_Humidity, ECODE: 1\n";

    /* Send command from PC */
    Uart_Test_Send_String(get_measurements_cmd);

    /* Check if MCU response with error info */
    Uart_Test_Receive_String(&expected_response_error_cmd);

    uint8_t i2c_ddrd_reg = INTERRUPT_DRDY_DRDY_STATUS;
    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", INTERRUPT_DRDY_REG)
        .withOutputParameterReturning("pData", &i2c_ddrd_reg, sizeof(i2c_ddrd_reg))
        .withParameter("Size", 1)
        .andReturnValue(HAL_ERROR);

    CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
    CHECK_EQUAL(CLI_CMD_NOT_PENDING, CLI_Process_Comand(&HDC2080_CLI));
}

TEST(HDC2080_TestGroup, HDC2080_Cmd_Get_Measurements_Temperature_Humidity_Lower_Limits)
{
    /* Send get_measurements command using uart, test lower limits of temperature and humidity
       check if the answer matches the expectation (TEMPERATURE_TO_HEX(), HUMIDITY_TO_HEX() macro) */
    const std::string get_measurements_cmd = "get_measurements\n";
    const std::string expected_response_cmd = "TEMP -40.5\nHUM 0.0\n";

    /* Send command from PC */
    Uart_Test_Send_String(get_measurements_cmd);

    /* Check if MCU response with valid data */
    Uart_Test_Receive_String(&expected_response_cmd);

    uint8_t i2c_ddrd_reg = INTERRUPT_DRDY_DRDY_STATUS;
    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", INTERRUPT_DRDY_REG)
        .withOutputParameterReturning("pData", &i2c_ddrd_reg, sizeof(i2c_ddrd_reg))
        .withParameter("Size", 1);

    uint8_t i2c_expected_response_reg[4] = {0x00, 0x00, 0x00, 0x00};

    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", TEMPERATURE_LOW_REG)
        .withOutputParameterReturning("pData", i2c_expected_response_reg, sizeof(i2c_expected_response_reg))
        .withParameter("Size", 4);

    CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
}

TEST(HDC2080_TestGroup, HDC2080_Cmd_Get_Measurements_Temperature_Humidity_Upper_Limits)
{
    /* Send get_measurements command using uart, test upper limits of temperature and humidity
       check if the answer matches the expectation (TEMPERATURE_TO_HEX(), HUMIDITY_TO_HEX() macro) */
    const std::string get_measurements_cmd = "get_measurements\n";
    const std::string expected_response_cmd = "TEMP 124.5\nHUM 100.0\n";

    /* Send command from PC */
    Uart_Test_Send_String(get_measurements_cmd);

    /* Check if MCU response with valid data */
    Uart_Test_Receive_String(&expected_response_cmd);

    uint8_t i2c_ddrd_reg = INTERRUPT_DRDY_DRDY_STATUS;
    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", INTERRUPT_DRDY_REG)
        .withOutputParameterReturning("pData", &i2c_ddrd_reg, sizeof(i2c_ddrd_reg))
        .withParameter("Size", 1);

    uint8_t i2c_expected_response_reg[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", TEMPERATURE_LOW_REG)
        .withOutputParameterReturning("pData", i2c_expected_response_reg, sizeof(i2c_expected_response_reg))
        .withParameter("Size", 4);

    CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
}

TEST(HDC2080_TestGroup, HDC_CLI_Cmd_Config_Temperature_res14bit_offset_seg_frame)
{
    /* Config temperature with next settings:
       1. res14bit
       2. temp_offset 0b1101 = +2,08C */
    const std::string hdc_config_cmd_1 = "conf";
    const std::string hdc_config_cmd_2 = "ig_temperat";
    const std::string hdc_config_cmd_3 = "ure res14bit temp_";
    const std::string hdc_config_cmd_4 = "offset 0x0d\n";
    const std::string expected_response_1 = "res14bit OK\n";
    const std::string expected_response_2 = "temp_offset OK, offset: d\n";

    /* Send commands */
    Uart_Test_Send_String(hdc_config_cmd_1);
    Uart_Test_Send_String(hdc_config_cmd_2);
    Uart_Test_Send_String(hdc_config_cmd_3);
    Uart_Test_Send_String(hdc_config_cmd_4);

    /* Check if the response sent to the UART is correct */
    Uart_Test_Receive_String(&expected_response_1);
    Uart_Test_Receive_String(&expected_response_2);

    /* TRES[7:6] = 11 = NA */
    uint8_t i2c_measure_reg = 0b11000001;
    /* TRES[7:6] = 00 = 14bit */
    uint8_t i2c_expected_measure_reg = 0b00000001;
    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", MEASUREMENT_CONFIGURATION_REG)
        .withOutputParameterReturning("pData", &i2c_measure_reg, sizeof(i2c_measure_reg))
        .withParameter("Size", 1);
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", MEASUREMENT_CONFIGURATION_REG)
        .withMemoryBufferParameter("pData", &i2c_expected_measure_reg, sizeof(i2c_expected_measure_reg));

    /* emp_offset 0b1101 = +2,08C */
    uint8_t i2c_expected_temp_offset_reg = 0b00001101;
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", TEMP_OFFSET_ADJUST_REG)
        .withMemoryBufferParameter("pData", &i2c_expected_temp_offset_reg, sizeof(i2c_expected_temp_offset_reg));

    CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
    CHECK_EQUAL(CLI_CMD_NOT_PENDING, CLI_Process_Comand(&HDC2080_CLI));
}

TEST(HDC2080_TestGroup, HDC_CLI_Cmd_Config_Humidity_res14bit_offset_extra_arg)
{
    /* Config temperature with next settings:
       1. res14bit
       2. temp_offset 0b1 = +0,4% */
    const std::string hdc_config_cmd = "config_humidity res14bit hum_offset 0x01 ";
    const std::string uart_extra_arg = "extra_arg\n";
    const std::string expected_response_1 = "res14bit OK\n";
    const std::string expected_response_2 = "hum_offset OK, offset: 1\n";
    const std::string expected_response_3 = "Bad argument: extra_arg\n";

    /* Send command */
    Uart_Test_Send_String(hdc_config_cmd);
    Uart_Test_Send_String(uart_extra_arg);

    /* Check if the response sent to the UART is correct */
    Uart_Test_Receive_String(&expected_response_1);
    Uart_Test_Receive_String(&expected_response_2);
    Uart_Test_Receive_String(&expected_response_3);

    /* HRES[4:5] = 11 = NA */
    uint8_t i2c_measure_reg = 0b00110001;
    /* HRES[4:5] = 00 = 14bit */
    uint8_t i2c_expected_measure_reg = 0b00000001;
    mock().expectOneCall("HAL_I2C_Mem_Read")
        .withParameter("MemAddress", MEASUREMENT_CONFIGURATION_REG)
        .withOutputParameterReturning("pData", &i2c_measure_reg, sizeof(i2c_measure_reg))
        .withParameter("Size", 1);
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", MEASUREMENT_CONFIGURATION_REG)
        .withMemoryBufferParameter("pData", &i2c_expected_measure_reg, sizeof(i2c_expected_measure_reg));

    /* emp_offset 0b1 =+0,4% */
    uint8_t i2c_expected_hum_offset_reg = 0b00000001;
    mock().expectOneCall("HAL_I2C_Mem_Write")
        .withParameter("MemAddress", HUM_OFFSET_ADJUST_REG)
        .withMemoryBufferParameter("pData", &i2c_expected_hum_offset_reg, sizeof(i2c_expected_hum_offset_reg));

    CHECK_EQUAL(CLI_CMD_PROCESSED, CLI_Process_Comand(&HDC2080_CLI));
    CHECK_EQUAL(CLI_CMD_NOT_PENDING, CLI_Process_Comand(&HDC2080_CLI));
}