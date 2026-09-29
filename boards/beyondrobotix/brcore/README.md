# Beyond Robotix BR Core

PX4 board support for the BR Core flight controller: STM32H743 with a 24 MHz
crystal, three IMUs (2x IIM-42652, BMI088), two MS5611 barometers, BMM350
compass, microSD, two CAN buses and Ethernet (LAN8742A).

This firmware lives on the `BR-Core` branch of
[BeyondRobotix/BR-PX4](https://github.com/BeyondRobotix/BR-PX4), based on PX4
v1.17.0.

## Build

```sh
make beyondrobotix_brcore_bootloader   # PX4 bootloader
make beyondrobotix_brcore_default      # firmware
```

PX4 builds NuttX inside the source tree. After changing a NuttX defconfig,
switching NuttX (below) or building the bootloader, clean it before the next
build, or stale objects get linked:

```sh
git -C platforms/nuttx/NuttX/nuttx clean -dXfq
git -C platforms/nuttx/NuttX/apps clean -dXfq
rm -rf build/beyondrobotix_brcore_*
```

## Flash

Firmware, over USB through the bootloader:

```sh
python3 Tools/px_uploader.py --port '/dev/serial/by-id/*BR_Core*' \
    build/beyondrobotix_brcore_default/beyondrobotix_brcore_default.px4
```

The bootloader is flashed once over SWD at `0x08000000` (sector 0 only).
A prebuilt copy is in `extras/`. With OpenOCD and an ST-Link:

```sh
openocd -f interface/stlink.cfg -f target/stm32h7x.cfg \
    -c "init" -c "reset halt" \
    -c "flash write_image erase build/beyondrobotix_brcore_bootloader/beyondrobotix_brcore_bootloader.bin 0x08000000" \
    -c "reset run" -c "shutdown"
```

The firmware sleeps the CPU when idle, so the first SWD attach after power-up
can fail with `Cortex-M PARTNO 0x0 is unrecognized`. Run the command again.

## NuttX version

BR Core builds against
[BeyondRobotix/NuttX](https://github.com/BeyondRobotix/NuttX), a fork of
PX4/NuttX with fixes this board needs. `nuttx.sh` switches between it and the
unmodified PX4/NuttX commit that PX4 v1.17.0 pins, and cleans the build:

```sh
boards/beyondrobotix/brcore/nuttx.sh status     # which NuttX is checked out
boards/beyondrobotix/brcore/nuttx.sh upstream   # PX4/NuttX, as PX4 v1.17.0 ships it
boards/beyondrobotix/brcore/nuttx.sh br         # back to BeyondRobotix/NuttX
```

`br` is the commit this branch records, so a fresh clone with
`git submodule update --init --recursive` already has it.

While on `upstream`:

- Build with `GIT_SUBMODULES_ARE_EVIL=1 make beyondrobotix_brcore_default`.
  Otherwise PX4's submodule check stops the build because NuttX is not at the
  recorded commit (with `CI=true` it resets NuttX instead).
- `git status` shows the NuttX submodule as modified. Don't commit that;
  switch back with `br`.

The board builds with either. With upstream NuttX the BR-only options are
ignored; see the note below on large file support.

### BR NuttX changes

Branch `br/sdmmc1-idma-bounce`, on top of the PX4 v1.17.0 NuttX commit:

- **SDMMC1 bounce buffer** (`CONFIG_STM32H7_SDMMC1_BOUNCE_SIZE`). The SD card
  is on SDMMC1, whose DMA can only reach AXI SRAM. Buffers the heap places in
  SRAM1-4 were rejected, and FAT fell back to one sector per transfer
  (about 150 KB/s). The board now copies them through a 16 KB AXI SRAM buffer
  (`CONFIG_MMCSD_MULTIBLOCK_LIMIT=32`). On the same card: writes 1.1 MB/s
  (4 KB blocks) to 2.7 MB/s (16 KB blocks), reads 5-7 MB/s.
- **Large file support**: cherry-pick of PX4/NuttX#388, which PX4 merged after
  v1.17.0. With it, `CONFIG_FS_LARGEFILE=y` gives 64-bit file sizes, as PX4
  `main` enables for boards with SD cards (PX4-Autopilot#28043). Without it,
  cards over 4 GB show the wrong size in `df` (a 64 GB card showed 3.5 GB).
  PX4's logger and MAVLink storage reports were already correct.

`upstream` lacks both, so the SD card is slow and large file support is
unreliable there (PX4/NuttX#388 explains why). Use it to compare against stock
NuttX, not for flight firmware.

When moving BR-Core to a newer PX4 release, rebase this branch onto the NuttX
commit that release pins, update `UPSTREAM_COMMIT` in `nuttx.sh`, and record
the new commit in the submodule.

## Board notes

- **Clock:** 400 MHz, the `cubepilot/cubeorange` clock tree.
- **Idle:** `src/idle.c` replaces NuttX's spinning idle loop with `WFI`
  (`CONFIG_ARCH_IDLE_CUSTOM`). USB init clears the unused ULPI sleep clock,
  without which USB stops while the CPU sleeps. Together with 400 MHz this
  brings the idle IMU temperature from about 60 C to 55 C.
- **Parameters:** stored in internal flash sector 15 (`FLASH_BASED_PARAMS`),
  as `holybro/kakuteh7`. The app gets 1792 KB.
- **Ethernet:** DHCP only. The board has no FRAM or QSPI to hold network
  settings, so `netman` and runtime IP changes are not available. MAVLink runs
  on UDP port 14550.
- **Heater:** regulates IMU1 (`SENS_TEMP_ID`) to 45 C (`SENS_IMU_TEMP`).

## Before release

- Board ID 8000 is provisional. Register an ID through a pull request to
  ArduPilot's `Tools/AP_Bootloader/board_types.txt`, then update
  `src/hw_config.h` and `firmware.prototype`.
- USB VID/PID `1209:5741` is ArduPilot's single-CDC ID, used until Beyond
  Robotix has its own (for example from pid.codes). Do not use `1209:5740`:
  Mission Planner's Windows driver binds it as ArduPilot's composite device,
  which splits PX4's single CDC ACM port so it will not open. Set the new ID
  in both defconfigs.
