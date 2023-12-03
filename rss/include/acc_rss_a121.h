// Copyright (c) Acconeer AB, 2021-2023
// All rights reserved

#ifndef ACC_RSS_A121_H_
#define ACC_RSS_A121_H_

#include <stdbool.h>

#include "acc_config.h"
#include "acc_definitions_common.h"
#include "acc_hal_definitions_a121.h"

/**
 * @brief The minimum buffer size needed for the assembly test
 *
 */
#define ACC_RSS_ASSEMBLY_TEST_MIN_BUFFER_SIZE 2048U


/**
 * @brief The minimum buffer size needed for the diagnostic test
 *
 */
#define ACC_RSS_DIAGNOSTIC_TEST_MIN_BUFFER_SIZE 4096U


/**
 * @brief Return code for rss tests
 *
 */
typedef enum
{
	ACC_RSS_TEST_STATE_ONGOING = 0,
	ACC_RSS_TEST_STATE_TOGGLE_ENABLE_PIN,
	ACC_RSS_TEST_STATE_WAIT_FOR_INTERRUPT,
	ACC_RSS_TEST_STATE_COMPLETE,
} acc_rss_test_state_t;


/**
 * @brief Integration status for rss tests
 *
 */
typedef enum
{
	ACC_RSS_TEST_INTEGRATION_STATUS_OK = 0,
	ACC_RSS_TEST_INTEGRATION_STATUS_TIMEOUT,
} acc_rss_test_integration_status_t;

/**
 * @brief The acc_rss_diagnostics instance
 *
 */
struct acc_rss_diagnostic_test;

typedef struct acc_rss_diagnostic_test acc_rss_diagnostic_test_t;

/**
 * @brief Test identity enum for acc_rss_assembly_test
 *
 */
typedef enum
{
	ACC_RSS_ASSEMBLY_TEST_ID_BASIC_READ,
	ACC_RSS_ASSEMBLY_TEST_ID_COMMUNICATION,
	ACC_RSS_ASSEMBLY_TEST_ID_ENABLE_PIN,
	ACC_RSS_ASSEMBLY_TEST_ID_INTERRUPT,
	ACC_RSS_ASSEMBLY_TEST_ID_CLOCK_AND_SUPPLY,
} acc_rss_assembly_test_test_id_t;


/**
 * @brief The result struct of acc_rss_assembly_test
 *
 */
typedef struct
{
	const char *test_name;
	bool       test_result;
} acc_rss_assembly_test_result_t;


/**
 * @brief The acc_rss_assembly_test instance
 *
 */
struct acc_rss_assembly_test;

typedef struct acc_rss_assembly_test acc_rss_assembly_test_t;


/**
 * @brief Register an integration
 *
 * @param[in] hal A reference to the hal to register
 * @return True if a valid integration is registered, false otherwise
 */
bool acc_rss_hal_register(const acc_hal_a121_t *hal);


/**
 * @brief Get the buffer size needed for the specified config
 *
 * This buffer size can be used to allocate a memory buffer in the
 * application, which is needed for several functions in the RSS library.
 *
 * @param[in] config The config to get the buffer size for
 * @param[out] buffer_size The buffer size
 * @return True if successful, false otherwise
 */
bool acc_rss_get_buffer_size(const acc_config_t *config, uint32_t *buffer_size);


/**
 * @brief Set the log level that determines when the integration HAL logger function is called
 *
 * Shall be called when there is a hal registered in RSS as it has no effect otherwise.
 *
 * @param[in] level The severity level for log output.
 */
void acc_rss_set_log_level(acc_log_level_t level);


/**
 * @brief Create a sensor diagnostic test instance
 *
 * The diagnostic test instance is used to keep track of internal state.
 *
 * The provided buffer start address should be 32-bit aligned.
 * The size of the provided buffer must be at least ACC_RSS_DIAGNOSTIC_TEST_MIN_BUFFER_SIZE bytes.
 * The size of the provided buffer should be a multiple of 8 bytes.
 *
 * @param[in] sensor_id The sensor id to be used to communicate with
 * @param[in] buffer A buffer used for diagnostic test
 * @param[in] buffer_size The size of the buffer
 *
 * @return Diagnostic test instance, NULL if the creation of the instance failed
 */
acc_rss_diagnostic_test_t *acc_rss_diagnostic_test_create(acc_sensor_id_t sensor_id, void *buffer, uint32_t buffer_size);


/**
 * @brief Destroy a sensor diagnosic test instance freeing any resources allocated.
 *
 * @param[in] diagnostic_test The diagnostic test instance to destroy, can be NULL
 */
void acc_rss_diagnostic_test_destroy(acc_rss_diagnostic_test_t *diagnostic_test);


/**
 * @brief Execute the diagnostic test
 *
 * The sensor must be powered on and enabled with before this function is called.
 *
 * The function should be called repeatedly until it returns ACC_RSS_TEST_STATE_COMPLETE.
 * If the function returns ACC_RSS_TEST_STATE_TOGGLE_ENABLE_PIN the caller should toggle the
 * enable pin to reset the sensor and then call @ref acc_rss_diagnostic_test_execute() again.
 * If the function returns ACC_RSS_TEST_STATE_WAIT_FOR_INTERRUPT the caller have to wait for
 * the interrupt pin before calling @ref acc_rss_diagnostic_test_execute() again.
 *
 * After assembly test has been run the sensor enable pin should be toggled to reset the sensor.
 *
 * @param[in, out] diagnostic_test The sensor diagnostic test instance
 * @param[in] integration_status Report back to assembly test if 'wait for interrupt' timed out
 * @return ACC_RSS_TEST_STATE_ONGOING if caller should call this function again.
 *         ACC_RSS_TEST_STATE_TOGGLE_ENABLE_PIN if caller should toggle the enable pin.
 *         ACC_RSS_TEST_STATE_WAIT_FOR_INTERRUPT if called should wait for interrupt pin.
 *	       or ACC_RSS_TEST_STATE_COMPLETE if the assembly test is complete.
 */
acc_rss_test_state_t acc_rss_diagnostic_test_execute(acc_rss_diagnostic_test_t         *diagnostic_test,
                                                     acc_rss_test_integration_status_t integration_status);


/**
 * @brief Create a sensor assembly test instance
 *
 * The assembly test instance is used to keep track of internal state and
 * results of the assembly test.
 *
 * The provided buffer start address should be 32-bit aligned.
 * The size of the provided buffer must be at least ACC_RSS_ASSEMBLY_TEST_MIN_BUFFER_SIZE bytes.
 * The size of the provided buffer should be a multiple of 8 bytes.
 * The test will not behave differently if a larger buffer is provided.
 *
 * All assembly tests are enabled by default after creation.
 *
 * @param[in] sensor_id The sensor id to be used to communicate with
 * @param[in] buffer A buffer used for assembly test
 * @param[in] buffer_size The size of the buffer
 *
 * @return Assembly test instance, NULL if the creation of the instance failed
 */
acc_rss_assembly_test_t *acc_rss_assembly_test_create(acc_sensor_id_t sensor_id, void *buffer, uint32_t buffer_size);


/**
 * @brief Destroy a sensor assembly test instance freeing any resources allocated.
 *
 * @param[in] assembly_test The assembly_test instance to destroy, can be NULL
 */
void acc_rss_assembly_test_destroy(acc_rss_assembly_test_t *assembly_test);


/**
 * @brief Enable all assembly tests
 *
 * @param[in] assembly_test The assembly_test instance
 */
void acc_rss_assembly_test_enable_all_tests(acc_rss_assembly_test_t *assembly_test);


/**
 * @brief Disable all assembly tests
 *
 * @param[in] assembly_test The assembly_test instance
 */
void acc_rss_assembly_test_disable_all_tests(acc_rss_assembly_test_t *assembly_test);


/**
 * @brief Enable a test in assembly test
 *
 * @param[in] assembly_test The assembly_test instance
 * @param[in] test_id The id of the test to be enabled
 */
void acc_rss_assembly_test_enable(acc_rss_assembly_test_t *assembly_test, acc_rss_assembly_test_test_id_t test_id);


/**
 * @brief Disable a test in assembly test
 *
 * @param[in] assembly_test The assembly_test instance
 * @param[in] test_id The id of the test to be enabled
 */
void acc_rss_assembly_test_disable(acc_rss_assembly_test_t *assembly_test, acc_rss_assembly_test_test_id_t test_id);


/**
 * @brief Execute the assembly test
 *
 * The sensor must be powered on and enabled before this function is called.
 *
 * The function should be called repeatedly until it returns ACC_RSS_TEST_STATE_COMPLETE.
 * If the function returns ACC_RSS_TEST_STATE_TOGGLE_ENABLE_PIN the caller should toggle the
 * enable pin to reset the sensor and then call @ref acc_rss_assembly_test_execute() again.
 * If the function returns ACC_RSS_TEST_STATE_WAIT_FOR_INTERRUPT the caller have to wait for
 * the interrupt pin before calling @ref acc_rss_assembly_test_execute() again.
 *
 * After assembly test has been run the sensor enable pin should be toggled to reset the sensor.
 *
 * @param[in, out] assembly_test The sensor assembly test instance
 * @param[in] integration_status Report back to assembly test if 'wait for interrupt' timed out
 * @return ACC_RSS_TEST_STATE_ONGOING if caller should call this function again.
 *         ACC_RSS_TEST_STATE_TOGGLE_ENABLE_PIN if caller should toggle the enable pin.
 *         ACC_RSS_TEST_STATE_WAIT_FOR_INTERRUPT if caller should wait for interrupt pin.
 *	       or ACC_RSS_TEST_STATE_COMPLETE if the assembly test is complete.
 */
acc_rss_test_state_t acc_rss_assembly_test_execute(acc_rss_assembly_test_t           *assembly_test,
                                                   acc_rss_test_integration_status_t integration_status);


/**
 * @brief A function to get the results from the sensor assembly test
 *
 * @param[in] assembly_test The sensor assembly test instance
 * @param[out] nbr_of_test_results The number of test results returned
 * @return The assembly test result array
 */
const acc_rss_assembly_test_result_t *acc_rss_assembly_test_get_results(const acc_rss_assembly_test_t *assembly_test,
                                                                        uint16_t                      *nbr_of_test_results);


#endif
