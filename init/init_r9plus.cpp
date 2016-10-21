/*
   Copyright (c) 2016, The CyanogenMod Project

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <cutils/properties.h>
#include "vendor_init.h"
#include "log.h"
#include "util.h"

static int read_file2(const char *fname, char *data, int max_size)
{
    int fd, rc;

    if (max_size < 1)
        return 0;

    fd = open(fname, O_RDONLY);
    if (fd < 0) {
        ERROR("failed to open '%s'\n", fname);
        return 0;
    }

    rc = read(fd, data, max_size - 1);
    if ((rc > 0) && (rc < max_size))
        data[rc] = '\0';
    else
        data[0] = '\0';
    close(fd);

    return 1;
}

void init_alarm_boot_properties()
{
    char const *alarm_file = "/proc/sys/kernel/boot_reason";
    char buf[64];

    if(read_file2(alarm_file, buf, sizeof(buf))) {

    /*
     * Setup ro.alarm_boot value to true when it is RTC triggered boot up
     * For existing PMIC chips, the following mapping applies
     * for the value of boot_reason:
     *
     * 0 -> unknown
     * 1 -> hard reset
     * 2 -> sudden momentary power loss (SMPL)
     * 3 -> real time clock (RTC)
     * 4 -> DC charger inserted
     * 5 -> USB charger insertd
     * 6 -> PON1 pin toggled (for secondary PMICs)
     * 7 -> CBLPWR_N pin toggled (for external power supply)
     * 8 -> KPDPWR_N pin toggled (power key pressed)
     */
        if(buf[0] == '3')
            property_set("ro.alarm_boot", "true");
        else
            property_set("ro.alarm_boot", "false");
    }
}

void vendor_load_properties() {
    char device[PROP_VALUE_MAX];
    int rc;

    rc = property_get("ro.cm.device", device, NULL);
    if (!rc || strncmp(device, "r9plus", PROP_VALUE_MAX))
        return;

    //char const *prj_file = "/proc/oppoVersion/prjVersion";
    //char const *pcb_file = "/proc/oppoVersion/pcbVersion";
    char const *operator_file = "/proc/oppoVersion/operatorName";
    //char prj_version[64];
    char operator_name[64];
    if(read_file2(operator_file, operator_name, sizeof(operator_name))) {
        if(strstr(operator_name, "8")) {
            property_set("ro.product.model", "OPPO R9 Plusm A");
            property_set("ro.separate.soft", "MSM_15103");
            property_set("ro.product.name", "R9PlusmA");
            property_set("ro.build.product", "R9PlusmA");
            property_set("ro.rf_version", "TDD_FDD_Ch_A_15103");
        } else if(strstr(operator_name, "9")) {
            property_set("ro.product.model", "OPPO R9 Plustm A");
            property_set("ro.separate.soft", "MSM_15102");
            property_set("ro.product.name", "R9PlustmA");
            property_set("ro.build.product", "R9PlustmA");
            property_set("ro.rf_version", "TDD_FDD_Ch_A_15102");
	}
    }

    init_alarm_boot_properties();
}
