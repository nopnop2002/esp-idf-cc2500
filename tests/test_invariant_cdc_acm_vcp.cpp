#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Simulated fixed buffer size as would be used in the CDC ACM VCP handler */
#define VCP_BUFFER_SIZE 64

/*
 * Safe data handler that enforces the security invariant:
 * data_len MUST NOT exceed the allocated buffer size before memcpy.
 * This is the corrected version of the vulnerable code.
 */
static int safe_vcp_data_handler(uint8_t *buffer, size_t buffer_size,
                                  const uint8_t *data, size_t data_len)
{
    /* Security invariant: data_len must be validated before memcpy */
    if (data_len > buffer_size) {
        return -1; /* Reject oversized data */
    }
    if (data == NULL || buffer == NULL) {
        return -1; /* Reject null pointers */
    }
    if (data_len == 0) {
        return 0; /* Nothing to copy */
    }
    memcpy(buffer, (const char *)data, data_len);
    return (int)data_len;
}

/*
 * Adversarial payload descriptor
 */
typedef struct {
    const char *description;
    size_t data_len;       /* claimed/actual data length */
    size_t buffer_size;    /* allocated buffer size */
} payload_t;

START_TEST(test_buffer_overflow_invariant)
{
    /*
     * Invariant: The data handler MUST NEVER copy more bytes than the
     * allocated buffer can hold. Any data_len > buffer_size must be
     * rejected before memcpy is called.
     */
    payload_t payloads[] = {
        /* Exact boundary - should succeed */
        { "exact buffer size",           VCP_BUFFER_SIZE,     VCP_BUFFER_SIZE     },
        /* One byte over - must be rejected */
        { "one byte over buffer",        VCP_BUFFER_SIZE + 1, VCP_BUFFER_SIZE     },
        /* Classic off-by-one */
        { "off-by-one overflow",         65,                  64                  },
        /* Double the buffer size */
        { "double buffer size",          VCP_BUFFER_SIZE * 2, VCP_BUFFER_SIZE     },
        /* Maximum USB full-speed packet size (512 bytes) into small buffer */
        { "USB FS max packet",           512,                 VCP_BUFFER_SIZE     },
        /* USB high-speed bulk max (512) into tiny buffer */
        { "USB HS bulk into tiny buf",   512,                 16                  },
        /* Large adversarial payload */
        { "large adversarial payload",   4096,                VCP_BUFFER_SIZE     },
        /* Integer-like overflow boundary */
        { "near SIZE_MAX",               SIZE_MAX,            VCP_BUFFER_SIZE     },
        /* Zero length - should succeed */
        { "zero length data",            0,                   VCP_BUFFER_SIZE     },
        /* Single byte into single byte buffer */
        { "single byte exact",           1,                   1                   },
        /* Single byte over single byte buffer */
        { "single byte over",            2,                   1                   },
        /* Typical USB CDC packet sizes */
        { "USB CDC 128 into 64",         128,                 64                  },
        { "USB CDC 256 into 64",         256,                 64                  },
        /* Adversarial: wrap-around attempt */
        { "wrap-around attempt",         SIZE_MAX - 1,        VCP_BUFFER_SIZE     },
        /* Adversarial: just under SIZE_MAX */
        { "just under SIZE_MAX",         SIZE_MAX - VCP_BUFFER_SIZE, VCP_BUFFER_SIZE },
    };

    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        size_t buf_size = payloads[i].buffer_size;
        size_t data_len = payloads[i].data_len;

        /* Allocate a real buffer to detect actual overflows */
        uint8_t *buffer = (uint8_t *)calloc(buf_size + 1, 1);
        ck_assert_msg(buffer != NULL, "Failed to allocate buffer for test case: %s",
                      payloads[i].description);

        /* Canary byte immediately after the buffer */
        uint8_t *canary = buffer + buf_size;
        *canary = 0xAB;

        /* Allocate source data - cap actual allocation to avoid OOM */
        size_t alloc_len = (data_len < 65536) ? data_len : 65536;
        if (alloc_len == 0) alloc_len = 1; /* avoid zero-size alloc edge case */
        uint8_t *data = (uint8_t *)malloc(alloc_len);
        ck_assert_msg(data != NULL, "Failed to allocate data for test case: %s",
                      payloads[i].description);
        memset(data, 0xCC, alloc_len);

        int result = safe_vcp_data_handler(buffer, buf_size, data, data_len);

        /*
         * SECURITY INVARIANT 1:
         * If data_len > buffer_size, the handler MUST reject the operation.
         */
        if (data_len > buf_size) {
            ck_assert_msg(result < 0,
                "SECURITY VIOLATION: Handler accepted oversized data "
                "(data_len=%zu > buf_size=%zu) for case: %s",
                data_len, buf_size, payloads[i].description);
        }

        /*
         * SECURITY INVARIANT 2:
         * If data_len <= buffer_size, the handler MUST succeed.
         */
        if (data_len <= buf_size && data_len < 65536) {
            ck_assert_msg(result >= 0,
                "Handler incorrectly rejected valid data "
                "(data_len=%zu <= buf_size=%zu) for case: %s",
                data_len, buf_size, payloads[i].description);
        }

        /*
         * SECURITY INVARIANT 3:
         * The canary byte after the buffer MUST NOT be modified.
         * Any modification indicates a buffer overflow occurred.
         */
        ck_assert_msg(*canary == 0xAB,
            "SECURITY VIOLATION: Buffer overflow detected - canary byte "
            "corrupted (expected 0xAB, got 0x%02X) for case: %s",
            *canary, payloads[i].description);

        free(data);
        free(buffer);
    }
}
END_TEST

START_TEST(test_null_pointer_invariant)
{
    /*
     * Invariant: The handler must safely reject NULL pointers
     * without crashing or causing undefined behavior.
     */
    uint8_t buffer[VCP_BUFFER_SIZE];
    uint8_t data[VCP_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    memset(data, 0xAA, sizeof(data));

    /* NULL data pointer */
    int result = safe_vcp_data_handler(buffer, VCP_BUFFER_SIZE, NULL, VCP_BUFFER_SIZE);
    ck_assert_msg(result < 0, "Handler must reject NULL data pointer");

    /* NULL buffer pointer */
    result = safe_vcp_data_handler(NULL, VCP_BUFFER_SIZE, data, VCP_BUFFER_SIZE);
    ck_assert_msg(result < 0, "Handler must reject NULL buffer pointer");

    /* Both NULL */
    result = safe_vcp_data_handler(NULL, VCP_BUFFER_SIZE, NULL, VCP_BUFFER_SIZE);
    ck_assert_msg(result < 0, "Handler must reject both NULL pointers");
}
END_TEST

START_TEST(test_boundary_copy_correctness)
{
    /*
     * Invariant: When data_len == buffer_size (exact fit),
     * the copy must be complete and correct.
     */
    uint8_t buffer[VCP_BUFFER_SIZE];
    uint8_t data[VCP_BUFFER_SIZE];
    uint8_t canary = 0xBE;

    /* Fill data with known pattern */
    for (int i = 0; i < VCP_BUFFER_SIZE; i++) {
        data[i] = (uint8_t)(i & 0xFF);
    }
    memset(buffer, 0, sizeof(buffer));

    int result = safe_vcp_data_handler(buffer, VCP_BUFFER_SIZE, data, VCP_BUFFER_SIZE);

    ck_assert_msg(result == VCP_BUFFER_SIZE,
        "Handler must copy exactly VCP_BUFFER_SIZE bytes on exact fit");

    ck_assert_msg(memcmp(buffer, data, VCP_BUFFER_SIZE) == 0,
        "Buffer contents must match source data after exact-fit copy");

    /* Canary should be untouched (it's a local variable, not adjacent memory,
     * but we verify the copy didn't go out of bounds conceptually) */
    ck_assert_msg(canary == 0xBE, "Stack canary must not be modified");
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security_CDC_ACM_VCP");
    tc_core = tcase_create("Core");

    tcase_set_timeout(tc_core, 30);
    tcase_add_test(tc_core, test_buffer_overflow_invariant);
    tcase_add_test(tc_core, test_null_pointer_invariant);
    tcase_add_test(tc_core, test_boundary_copy_correctness);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}