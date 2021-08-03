/*
 * Copyright (c) 2018 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <spm.h>
#include <aarch32/cortex_m/tz.h>
#include <stdio.h>



__TZ_NONSECURE_ENTRY_FUNC
int spm_secure_function(void)
{
    // DO SOMETHING
    return 5;
}

void main(void)
{
	spm_config();
	spm_jump();
}
