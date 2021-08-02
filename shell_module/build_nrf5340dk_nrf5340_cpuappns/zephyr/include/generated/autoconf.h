#define CONFIG_SHELL_FOREGROUND_CMDS 1
#define CONFIG_SHELL_DYNAMIC_CMDS 1
#define CONFIG_GPIO 1
#define CONFIG_NET_CONFIG_IEEE802154_DEV_NAME ""
#define CONFIG_UART_INTERRUPT_DRIVEN 1
#define CONFIG_GPIO_NRF_P1 1
#define CONFIG_BOARD "nrf5340dk_nrf5340_cpuapp"
#define CONFIG_FLASH_LOAD_SIZE 0x30000
#define CONFIG_SRAM_SIZE 192
#define CONFIG_FLASH_LOAD_OFFSET 0x50000
#define CONFIG_SOC "nRF5340_CPUAPP_QKAA"
#define CONFIG_SOC_SERIES "nrf53"
#define CONFIG_NUM_IRQS 69
#define CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC 32768
#define CONFIG_HEAP_MEM_POOL_SIZE 49152
#define CONFIG_PM 1
#define CONFIG_SOC_HAS_TIMING_FUNCTIONS 1
#define CONFIG_ARCH_HAS_CUSTOM_BUSY_WAIT 1
#define CONFIG_CLOCK_CONTROL 1
#define CONFIG_NRF_RTC_TIMER 1
#define CONFIG_SYS_CLOCK_TICKS_PER_SEC 32768
#define CONFIG_BUILD_OUTPUT_HEX 1
#define CONFIG_ROM_START_OFFSET 0x0
#define CONFIG_FLASH_SIZE 1024
#define CONFIG_FLASH_BASE_ADDRESS 0x0
#define CONFIG_SERIAL 1
#define CONFIG_MAIN_STACK_SIZE 1024
#define CONFIG_PRIVILEGED_STACK_SIZE 1024
#define CONFIG_ENTROPY_GENERATOR 1
#define CONFIG_LOG_DEFAULT_LEVEL 3
#define CONFIG_PM_PARTITION_SIZE_PROVISION 0x280
#define CONFIG_PM_PARTITION_SIZE_B0_IMAGE 0x8000
#define CONFIG_SB_VALIDATION_INFO_MAGIC 0x86518483
#define CONFIG_SB_VALIDATION_POINTER_MAGIC 0x6919b47e
#define CONFIG_SB_VALIDATION_INFO_CRYPTO_ID 1
#define CONFIG_SB_VALIDATION_INFO_VERSION 2
#define CONFIG_SB_VALIDATION_METADATA_OFFSET 0
#define CONFIG_SB_VALIDATE_FW_SIGNATURE 1
#define CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE 1024
#define CONFIG_SPM 1
#define CONFIG_SPM_BUILD_STRATEGY_SKIP_BUILD 1
#define CONFIG_SPM_SECURE_SERVICES 1
#define CONFIG_SPM_SERVICE_RNG 1
#define CONFIG_SPM_SERVICE_READ 1
#define CONFIG_SPM_SERVICE_FIND_FIRMWARE_INFO 1
#define CONFIG_ARM_ENTRY_VENEERS_LIB_NAME "spm/libspmsecureentries.a"
#define CONFIG_FW_INFO 1
#define CONFIG_FW_INFO_OFFSET 0x200
#define CONFIG_FW_INFO_FIRMWARE_VERSION 1
#define CONFIG_FW_INFO_MAGIC_COMMON 0x281ee6de
#define CONFIG_FW_INFO_MAGIC_FIRMWARE_INFO 0x8fcebb4c
#define CONFIG_FW_INFO_MAGIC_EXT_API 0xb845acea
#define CONFIG_FW_INFO_HARDWARE_ID 53
#define CONFIG_FW_INFO_VERSION 2
#define CONFIG_FW_INFO_CRYPTO_ID 0
#define CONFIG_FW_INFO_MAGIC_COMPATIBILITY_ID 0
#define CONFIG_FW_INFO_MAGIC_LEN 12
#define CONFIG_FW_INFO_VALID_VAL 0x9102FFFF
#define CONFIG_EXT_API_PROVIDE_EXT_API_UNUSED 1
#define CONFIG_MPSL_THREAD_COOP_PRIO 8
#define CONFIG_MPSL_SIGNAL_STACK_SIZE 1024
#define CONFIG_MPSL_TIMESLOT_SESSION_COUNT 0
#define CONFIG_MPSL_LOG_LEVEL_INF 1
#define CONFIG_MPSL_LOG_LEVEL 3
#define CONFIG_PARTITION_MANAGER_ENABLED 1
#define CONFIG_FLASH_MAP_CUSTOM 1
#define CONFIG_SRAM_BASE_ADDRESS 0x20040000
#define CONFIG_PM_IMAGE_NOT_BUILT_FROM_SOURCE 1
#define CONFIG_PM_SRAM_BASE 0x20000000
#define CONFIG_PM_SRAM_SIZE 0x80000
#define CONFIG_MGMT_FMFU_LOG_LEVEL_INF 1
#define CONFIG_MGMT_FMFU_LOG_LEVEL 3
#define CONFIG_BOOT_SIGNATURE_KEY_FILE ""
#define CONFIG_TFM_BOARD "C:/ncs/v1.6.0/nrf/modules/tfm/tfm/boards/nrf5340_cpuapp"
#define CONFIG_AGPS_LOG_LEVEL_INF 1
#define CONFIG_AGPS_LOG_LEVEL 3
#define CONFIG_NRF_SPU_FLASH_REGION_SIZE 0x4000
#define CONFIG_FPROTECT_BLOCK_SIZE 0x4000
#define CONFIG_RESET_ON_FATAL_ERROR 1
#define CONFIG_FATAL_ERROR_LOG_LEVEL_INF 1
#define CONFIG_FATAL_ERROR_LOG_LEVEL 3
#define CONFIG_HW_UNIQUE_KEY_PARTITION_SIZE 0x0
#define CONFIG_ENTROPY_CC3XX 1
#define CONFIG_ZEPHYR_NRF_MODULE 1
#define CONFIG_DT_FLASH_WRITE_BLOCK_SIZE 4
#define CONFIG_ZEPHYR_MCUBOOT_MODULE 1
#define CONFIG_ZEPHYR_TRUSTED_FIRMWARE_M_MODULE 1
#define CONFIG_ZEPHYR_CJSON_MODULE 1
#define CONFIG_ZEPHYR_PELION_DM_MODULE 1
#define CONFIG_ZEPHYR_CDDL_GEN_MODULE 1
#define CONFIG_ZEPHYR_MEMFAULT_FIRMWARE_SDK_MODULE 1
#define CONFIG_ZEPHYR_HAL_NORDIC_MODULE 1
#define CONFIG_HAS_NORDIC_DRIVERS 1
#define CONFIG_HAS_NRFX 1
#define CONFIG_NRFX_CLOCK 1
#define CONFIG_NRFX_CLOCK_LFXO_TWO_STAGE_ENABLED 1
#define CONFIG_NRFX_DPPI 1
#define CONFIG_NRFX_GPIOTE 1
#define CONFIG_NRFX_NVMC 1
#define CONFIG_ZEPHYR_MBEDTLS_MODULE 1
#define CONFIG_MBEDTLS_BUILTIN 1
#define CONFIG_ZEPHYR_TRACERECORDER_MODULE 1
#define CONFIG_NRF_MODEM_SHMEM_CTRL_SIZE 0x4e8
#define CONFIG_NRF_802154_SOURCE_NRFXLIB 1
#define CONFIG_ZEPHYR_NRFXLIB_MODULE 1
#define CONFIG_ZEPHYR_CONNECTEDHOMEIP_MODULE 1
#define CONFIG_HAS_CMSIS_CORE 1
#define CONFIG_HAS_CMSIS_CORE_M 1
#define CONFIG_BOARD_NRF5340DK_NRF5340_CPUAPPNS 1
#define CONFIG_BOARD_ENABLE_DCDC_APP 1
#define CONFIG_BOARD_ENABLE_DCDC_NET 1
#define CONFIG_BOARD_ENABLE_DCDC_HV 1
#define CONFIG_SOC_SERIES_NRF53X 1
#define CONFIG_CPU_HAS_ARM_MPU 1
#define CONFIG_CPU_HAS_NRF_IDAU 1
#define CONFIG_NRF_SPU_RAM_REGION_SIZE 0x2000
#define CONFIG_HAS_SWO 1
#define CONFIG_SOC_FAMILY "nordic_nrf"
#define CONFIG_SOC_FAMILY_NRF 1
#define CONFIG_HAS_HW_NRF_CC312 1
#define CONFIG_HAS_HW_NRF_CLOCK 1
#define CONFIG_HAS_HW_NRF_DPPIC 1
#define CONFIG_HAS_HW_NRF_EGU0 1
#define CONFIG_HAS_HW_NRF_EGU1 1
#define CONFIG_HAS_HW_NRF_EGU2 1
#define CONFIG_HAS_HW_NRF_EGU3 1
#define CONFIG_HAS_HW_NRF_EGU4 1
#define CONFIG_HAS_HW_NRF_EGU5 1
#define CONFIG_HAS_HW_NRF_GPIO0 1
#define CONFIG_HAS_HW_NRF_GPIO1 1
#define CONFIG_HAS_HW_NRF_GPIOTE 1
#define CONFIG_HAS_HW_NRF_I2S 1
#define CONFIG_HAS_HW_NRF_IPC 1
#define CONFIG_HAS_HW_NRF_NFCT 1
#define CONFIG_HAS_HW_NRF_NVMC_PE 1
#define CONFIG_HAS_HW_NRF_PDM 1
#define CONFIG_HAS_HW_NRF_POWER 1
#define CONFIG_HAS_HW_NRF_PWM0 1
#define CONFIG_HAS_HW_NRF_PWM1 1
#define CONFIG_HAS_HW_NRF_PWM2 1
#define CONFIG_HAS_HW_NRF_PWM3 1
#define CONFIG_HAS_HW_NRF_QDEC0 1
#define CONFIG_HAS_HW_NRF_QDEC1 1
#define CONFIG_HAS_HW_NRF_QSPI 1
#define CONFIG_HAS_HW_NRF_RTC0 1
#define CONFIG_HAS_HW_NRF_RTC1 1
#define CONFIG_HAS_HW_NRF_SAADC 1
#define CONFIG_HAS_HW_NRF_SPIM0 1
#define CONFIG_HAS_HW_NRF_SPIM1 1
#define CONFIG_HAS_HW_NRF_SPIM2 1
#define CONFIG_HAS_HW_NRF_SPIM3 1
#define CONFIG_HAS_HW_NRF_SPIM4 1
#define CONFIG_HAS_HW_NRF_SPIS0 1
#define CONFIG_HAS_HW_NRF_SPIS1 1
#define CONFIG_HAS_HW_NRF_SPIS2 1
#define CONFIG_HAS_HW_NRF_SPIS3 1
#define CONFIG_HAS_HW_NRF_SPU 1
#define CONFIG_HAS_HW_NRF_TIMER0 1
#define CONFIG_HAS_HW_NRF_TIMER1 1
#define CONFIG_HAS_HW_NRF_TIMER2 1
#define CONFIG_HAS_HW_NRF_TWIM0 1
#define CONFIG_HAS_HW_NRF_TWIM1 1
#define CONFIG_HAS_HW_NRF_TWIM2 1
#define CONFIG_HAS_HW_NRF_TWIM3 1
#define CONFIG_HAS_HW_NRF_TWIS0 1
#define CONFIG_HAS_HW_NRF_TWIS1 1
#define CONFIG_HAS_HW_NRF_TWIS2 1
#define CONFIG_HAS_HW_NRF_TWIS3 1
#define CONFIG_HAS_HW_NRF_UARTE0 1
#define CONFIG_HAS_HW_NRF_UARTE1 1
#define CONFIG_HAS_HW_NRF_UARTE2 1
#define CONFIG_HAS_HW_NRF_UARTE3 1
#define CONFIG_HAS_HW_NRF_USBD 1
#define CONFIG_HAS_HW_NRF_USBREG 1
#define CONFIG_HAS_HW_NRF_WDT0 1
#define CONFIG_HAS_HW_NRF_WDT1 1
#define CONFIG_NRF_HW_RTC1_RESERVED 1
#define CONFIG_SOC_NRF5340_CPUAPP 1
#define CONFIG_SOC_NRF5340_CPUAPP_QKAA 1
#define CONFIG_SOC_DCDC_NRF53X_APP 1
#define CONFIG_SOC_DCDC_NRF53X_NET 1
#define CONFIG_SOC_DCDC_NRF53X_HV 1
#define CONFIG_SOC_LOG_LEVEL_INF 1
#define CONFIG_SOC_LOG_LEVEL 3
#define CONFIG_SOC_COMPATIBLE_NRF 1
#define CONFIG_ARCH "arm"
#define CONFIG_CPU_CORTEX 1
#define CONFIG_CPU_CORTEX_M 1
#define CONFIG_ISA_THUMB2 1
#define CONFIG_ASSEMBLER_ISA_THUMB2 1
#define CONFIG_COMPILER_ISA_THUMB2 1
#define CONFIG_STACK_ALIGN_DOUBLE_WORD 1
#define CONFIG_PLATFORM_SPECIFIC_INIT 1
#define CONFIG_FAULT_DUMP 2
#define CONFIG_BUILTIN_STACK_GUARD 1
#define CONFIG_ARM_STACK_PROTECTION 1
#define CONFIG_ARM_NONSECURE_FIRMWARE 1
#define CONFIG_CPU_CORTEX_M33 1
#define CONFIG_CPU_CORTEX_M_HAS_SYSTICK 1
#define CONFIG_CPU_CORTEX_M_HAS_DWT 1
#define CONFIG_CPU_CORTEX_M_HAS_BASEPRI 1
#define CONFIG_CPU_CORTEX_M_HAS_VTOR 1
#define CONFIG_CPU_CORTEX_M_HAS_SPLIM 1
#define CONFIG_CPU_CORTEX_M_HAS_PROGRAMMABLE_FAULT_PRIOS 1
#define CONFIG_CPU_CORTEX_M_HAS_CMSE 1
#define CONFIG_ARMV7_M_ARMV8_M_MAINLINE 1
#define CONFIG_ARMV8_M_MAINLINE 1
#define CONFIG_ARMV8_M_SE 1
#define CONFIG_ARMV7_M_ARMV8_M_FP 1
#define CONFIG_ARMV8_M_DSP 1
#define CONFIG_GEN_ISR_TABLES 1
#define CONFIG_TEST_EXTRA_STACKSIZE 0
#define CONFIG_CORTEX_M_DEBUG_NULL_POINTER_EXCEPTION_DETECTION_NONE 1
#define CONFIG_ARM_TRUSTZONE_M 1
#define CONFIG_ARM_FIRMWARE_USES_SECURE_ENTRY_FUNCS 1
#define CONFIG_GEN_IRQ_VECTOR_TABLE 1
#define CONFIG_ARM_MPU 1
#define CONFIG_ARM_MPU_REGION_MIN_ALIGN_AND_SIZE 32
#define CONFIG_CUSTOM_SECTION_MIN_ALIGN_SIZE 32
#define CONFIG_ARM 1
#define CONFIG_ARCH_IS_SET 1
#define CONFIG_ARCH_LOG_LEVEL_INF 1
#define CONFIG_ARCH_LOG_LEVEL 3
#define CONFIG_MPU_LOG_LEVEL_INF 1
#define CONFIG_MPU_LOG_LEVEL 3
#define CONFIG_TRUSTED_EXECUTION_NONSECURE 1
#define CONFIG_HW_STACK_PROTECTION 1
#define CONFIG_KOBJECT_TEXT_AREA 256
#define CONFIG_KOBJECT_DATA_AREA_RESERVE_EXTRA_PERCENT 100
#define CONFIG_KOBJECT_RODATA_AREA_EXTRA_BYTES 16
#define CONFIG_GEN_PRIV_STACKS 1
#define CONFIG_GEN_SW_ISR_TABLE 1
#define CONFIG_ARCH_SW_ISR_TABLE_ALIGN 0
#define CONFIG_GEN_IRQ_START_VECTOR 0
#define CONFIG_ARCH_HAS_TIMING_FUNCTIONS 1
#define CONFIG_ARCH_HAS_TRUSTED_EXECUTION 1
#define CONFIG_ARCH_HAS_STACK_PROTECTION 1
#define CONFIG_ARCH_HAS_USERSPACE 1
#define CONFIG_ARCH_HAS_EXECUTABLE_PAGE_BIT 1
#define CONFIG_ARCH_HAS_RAMFUNC_SUPPORT 1
#define CONFIG_ARCH_HAS_NESTED_EXCEPTION_DETECTION 1
#define CONFIG_ARCH_SUPPORTS_COREDUMP 1
#define CONFIG_ARCH_SUPPORTS_ARCH_HW_INIT 1
#define CONFIG_ARCH_HAS_EXTRA_EXCEPTION_INFO 1
#define CONFIG_ARCH_HAS_THREAD_LOCAL_STORAGE 1
#define CONFIG_ARCH_HAS_THREAD_ABORT 1
#define CONFIG_CPU_HAS_TEE 1
#define CONFIG_CPU_HAS_FPU 1
#define CONFIG_CPU_HAS_MPU 1
#define CONFIG_MPU 1
#define CONFIG_MPU_REQUIRES_NON_OVERLAPPING_REGIONS 1
#define CONFIG_MPU_GAP_FILLING 1
#define CONFIG_SRAM_REGION_PERMISSIONS 1
#define CONFIG_TOOLCHAIN_HAS_BUILTIN_FFS 1
#define CONFIG_KERNEL_LOG_LEVEL_INF 1
#define CONFIG_KERNEL_LOG_LEVEL 3
#define CONFIG_MULTITHREADING 1
#define CONFIG_NUM_COOP_PRIORITIES 16
#define CONFIG_NUM_PREEMPT_PRIORITIES 15
#define CONFIG_MAIN_THREAD_PRIORITY 0
#define CONFIG_COOP_ENABLED 1
#define CONFIG_PREEMPT_ENABLED 1
#define CONFIG_PRIORITY_CEILING 0
#define CONFIG_NUM_METAIRQ_PRIORITIES 0
#define CONFIG_IDLE_STACK_SIZE 320
#define CONFIG_ISR_STACK_SIZE 2048
#define CONFIG_THREAD_STACK_INFO 1
#define CONFIG_ERRNO 1
#define CONFIG_SCHED_DUMB 1
#define CONFIG_WAITQ_DUMB 1
#define CONFIG_INIT_STACKS 1
#define CONFIG_BOOT_DELAY 0
#define CONFIG_THREAD_MONITOR 1
#define CONFIG_THREAD_NAME 1
#define CONFIG_THREAD_MAX_NAME_LEN 32
#define CONFIG_SYSTEM_WORKQUEUE_PRIORITY -1
#define CONFIG_ATOMIC_OPERATIONS_BUILTIN 1
#define CONFIG_TIMESLICING 1
#define CONFIG_TIMESLICE_SIZE 0
#define CONFIG_TIMESLICE_PRIORITY 0
#define CONFIG_POLL 1
#define CONFIG_NUM_MBOX_ASYNC_MSGS 10
#define CONFIG_NUM_PIPE_ASYNC_MSGS 10
#define CONFIG_KERNEL_MEM_POOL 1
#define CONFIG_ARCH_HAS_CUSTOM_SWAP_TO_MAIN 1
#define CONFIG_SWAP_NONATOMIC 1
#define CONFIG_SYS_CLOCK_EXISTS 1
#define CONFIG_TIMEOUT_64BIT 1
#define CONFIG_XIP 1
#define CONFIG_KERNEL_INIT_PRIORITY_OBJECTS 30
#define CONFIG_KERNEL_INIT_PRIORITY_DEFAULT 40
#define CONFIG_KERNEL_INIT_PRIORITY_DEVICE 50
#define CONFIG_APPLICATION_INIT_PRIORITY 90
#define CONFIG_STACK_POINTER_RANDOM 0
#define CONFIG_MP_NUM_CPUS 1
#define CONFIG_TICKLESS_KERNEL 1
#define CONFIG_HAS_DTS 1
#define CONFIG_HAS_DTS_GPIO 1
#define CONFIG_UART_CONSOLE_ON_DEV_NAME "UART_0"
#define CONFIG_CONSOLE 1
#define CONFIG_CONSOLE_INPUT_MAX_LINE_LEN 128
#define CONFIG_CONSOLE_HAS_DRIVER 1
#define CONFIG_UART_CONSOLE 1
#define CONFIG_UART_CONSOLE_INIT_PRIORITY 60
#define CONFIG_UART_CONSOLE_INPUT_EXPIRED 1
#define CONFIG_UART_CONSOLE_INPUT_EXPIRED_TIMEOUT 15000
#define CONFIG_UART_CONSOLE_LOG_LEVEL_INF 1
#define CONFIG_UART_CONSOLE_LOG_LEVEL 3
#define CONFIG_HAS_SEGGER_RTT 1
#define CONFIG_SERIAL_HAS_DRIVER 1
#define CONFIG_SERIAL_SUPPORT_ASYNC 1
#define CONFIG_SERIAL_SUPPORT_INTERRUPT 1
#define CONFIG_UART_NRFX 1
#define CONFIG_UART_0_NRF_UARTE 1
#define CONFIG_UART_0_ENHANCED_POLL_OUT 1
#define CONFIG_UART_0_INTERRUPT_DRIVEN 1
#define CONFIG_UART_0_NRF_TX_BUFFER_SIZE 32
#define CONFIG_UART_ENHANCED_POLL_OUT 1
#define CONFIG_NRF_UARTE_PERIPHERAL 1
#define CONFIG_APIC_TIMER_IRQ_PRIORITY 4
#define CONFIG_NRF_RTC_TIMER_USER_CHAN_COUNT 0
#define CONFIG_SYSTEM_CLOCK_WAIT_FOR_STABILITY 1
#define CONFIG_SYSTEM_CLOCK_DISABLE 1
#define CONFIG_SYSTEM_CLOCK_INIT_PRIORITY 0
#define CONFIG_TICKLESS_CAPABLE 1
#define CONFIG_ENTROPY_HAS_DRIVER 1
#define CONFIG_GPIO_LOG_LEVEL_INF 1
#define CONFIG_GPIO_LOG_LEVEL 3
#define CONFIG_GPIO_NRFX 1
#define CONFIG_GPIO_NRF_INIT_PRIORITY 40
#define CONFIG_GPIO_NRF_P0 1
#define CONFIG_GPIO_NRF_INT_EDGE_USING_GPIOTE 1
#define CONFIG_CLOCK_CONTROL_LOG_LEVEL_INF 1
#define CONFIG_CLOCK_CONTROL_LOG_LEVEL 3
#define CONFIG_CLOCK_CONTROL_NRF 1
#define CONFIG_CLOCK_CONTROL_NRF_SHELL 1
#define CONFIG_CLOCK_CONTROL_NRF_K32SRC_XTAL 1
#define CONFIG_CLOCK_CONTROL_NRF_K32SRC_50PPM 1
#define CONFIG_CLOCK_CONTROL_NRF_ACCURACY 50
#define CONFIG_MINIMAL_LIBC 1
#define CONFIG_HAS_NEWLIB_LIBC_NANO 1
#define CONFIG_MINIMAL_LIBC_MALLOC 1
#define CONFIG_MINIMAL_LIBC_MALLOC_ARENA_SIZE 0
#define CONFIG_MINIMAL_LIBC_CALLOC 1
#define CONFIG_MINIMAL_LIBC_REALLOCARRAY 1
#define CONFIG_STDOUT_CONSOLE 1
#define CONFIG_RING_BUFFER 1
#define CONFIG_SYS_HEAP_ALLOC_LOOPS 3
#define CONFIG_MPSC_PBUF 1
#define CONFIG_REBOOT 1
#define CONFIG_CBPRINTF_COMPLETE 1
#define CONFIG_CBPRINTF_FULL_INTEGRAL 1
#define CONFIG_CBPRINTF_N_SPECIFIER 1
#define CONFIG_POSIX_MAX_FDS 4
#define CONFIG_POSIX_CLOCK 1
#define CONFIG_MAX_TIMER_COUNT 5
#define CONFIG_FNMATCH 1
#define CONFIG_PRINTK 1
#define CONFIG_ASSERT_VERBOSE 1
#define CONFIG_LOG 1
#define CONFIG_LOG_MODE_DEFERRED 1
#define CONFIG_LOG_RUNTIME_FILTERING 1
#define CONFIG_LOG_OVERRIDE_LEVEL 0
#define CONFIG_LOG_MAX_LEVEL 4
#define CONFIG_LOG_FUNC_NAME_PREFIX_DBG 1
#define CONFIG_LOG_MODE_OVERFLOW 1
#define CONFIG_LOG_PROCESS_TRIGGER_THRESHOLD 10
#define CONFIG_LOG_PROCESS_THREAD 1
#define CONFIG_LOG_PROCESS_THREAD_SLEEP_MS 1000
#define CONFIG_LOG_PROCESS_THREAD_STACK_SIZE 768
#define CONFIG_LOG_BUFFER_SIZE 1024
#define CONFIG_LOG_DETECT_MISSED_STRDUP 1
#define CONFIG_LOG_STRDUP_MAX_STRING 32
#define CONFIG_LOG_STRDUP_BUF_COUNT 4
#define CONFIG_LOG_DOMAIN_ID 0
#define CONFIG_LOG_CMDS 1
#define CONFIG_LOG2_USE_VLA 1
#define CONFIG_PM_POLICY_RESIDENCY 1
#define CONFIG_PM_POLICY_RESIDENCY_DEFAULT 1
#define CONFIG_PM_LOG_LEVEL_INF 1
#define CONFIG_PM_LOG_LEVEL 3
#define CONFIG_SHELL 1
#define CONFIG_SHELL_LOG_LEVEL_INF 1
#define CONFIG_SHELL_LOG_LEVEL 3
#define CONFIG_SHELL_BACKENDS 1
#define CONFIG_SHELL_BACKEND_SERIAL 1
#define CONFIG_SHELL_BACKEND_SERIAL_INIT_PRIORITY 0
#define CONFIG_SHELL_PROMPT_UART "uart:~$ "
#define CONFIG_UART_SHELL_ON_DEV_NAME "UART_0"
#define CONFIG_SHELL_BACKEND_SERIAL_INTERRUPT_DRIVEN 1
#define CONFIG_SHELL_BACKEND_SERIAL_TX_RING_BUFFER_SIZE 8
#define CONFIG_SHELL_BACKEND_SERIAL_RX_RING_BUFFER_SIZE 64
#define CONFIG_SHELL_BACKEND_SERIAL_LOG_MESSAGE_QUEUE_TIMEOUT 100
#define CONFIG_SHELL_BACKEND_SERIAL_LOG_MESSAGE_QUEUE_SIZE 10
#define CONFIG_SHELL_BACKEND_SERIAL_LOG_LEVEL_DEFAULT 1
#define CONFIG_SHELL_BACKEND_SERIAL_LOG_LEVEL 5
#define CONFIG_SHELL_STACK_SIZE 49152
#define CONFIG_SHELL_BACKSPACE_MODE_DELETE 1
#define CONFIG_SHELL_CMD_BUFF_SIZE 256
#define CONFIG_SHELL_PRINTF_BUFF_SIZE 30
#define CONFIG_SHELL_DEFAULT_TERMINAL_WIDTH 80
#define CONFIG_SHELL_DEFAULT_TERMINAL_HEIGHT 24
#define CONFIG_SHELL_ARGC_MAX 12
#define CONFIG_SHELL_TAB 1
#define CONFIG_SHELL_TAB_AUTOCOMPLETION 1
#define CONFIG_SHELL_WILDCARD 1
#define CONFIG_SHELL_ECHO_STATUS 1
#define CONFIG_SHELL_VT100_COLORS 1
#define CONFIG_SHELL_METAKEYS 1
#define CONFIG_SHELL_HELP 1
#define CONFIG_SHELL_HELP_OPT_PARSE 1
#define CONFIG_SHELL_HELP_ON_WRONG_ARGUMENT_COUNT 1
#define CONFIG_SHELL_HISTORY 1
#define CONFIG_SHELL_HISTORY_BUFFER 512
#define CONFIG_SHELL_STATS 1
#define CONFIG_SHELL_CMDS 1
#define CONFIG_SHELL_CMDS_RESIZE 1
#define CONFIG_SHELL_LOG_BACKEND 1
#define CONFIG_KERNEL_SHELL 1
#define CONFIG_DEVICE_SHELL 1
#define CONFIG_DATE_SHELL 1
#define CONFIG_ENTROPY_DEVICE_RANDOM_GENERATOR 1
#define CONFIG_CSPRING_ENABLED 1
#define CONFIG_HARDWARE_DEVICE_CS_GENERATOR 1
#define CONFIG_TOOLCHAIN_GNUARMEMB 1
#define CONFIG_LINKER_ORPHAN_SECTION_WARN 1
#define CONFIG_HAS_FLASH_LOAD_OFFSET 1
#define CONFIG_KERNEL_ENTRY "__start"
#define CONFIG_LINKER_SORT_BY_ALIGNMENT 1
#define CONFIG_SRAM_OFFSET 0x0
#define CONFIG_SIZE_OPTIMIZATIONS 1
#define CONFIG_COMPILER_COLOR_DIAGNOSTICS 1
#define CONFIG_COMPILER_OPT ""
#define CONFIG_RUNTIME_ERROR_CHECKS 1
#define CONFIG_KERNEL_BIN_NAME "zephyr"
#define CONFIG_OUTPUT_STAT 1
#define CONFIG_OUTPUT_DISASSEMBLY 1
#define CONFIG_OUTPUT_PRINT_MEMORY_USAGE 1
#define CONFIG_BUILD_OUTPUT_BIN 1
#define CONFIG_COMPAT_INCLUDES 1
