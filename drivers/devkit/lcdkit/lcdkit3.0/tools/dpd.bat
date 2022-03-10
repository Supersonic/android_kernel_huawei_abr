@rem Copyright ? Huawei Technologies Co., Ltd. 2019-2025. All rights reserved.
@rem Description: lcd dpd script
@echo off
adb remount
adb shell "rm /odm/lcd -rf"
adb push out\ /odm/lcd
adb reboot bootloader
fastboot oem lcd dpd enable
fastboot oem boot
pause
@echo on
