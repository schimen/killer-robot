{ pkgs ? import <nixpkgs> {} }:

(pkgs.buildFHSUserEnv {
  name = "zephyr-env";
  targetPkgs = pkgs: (with pkgs; [
    (python39.withPackages(ps: with ps; [
      bleak
      west
      requests
      pyelftools
      pyserial
      pygatt
    ]))
    openocd
    dtc
    ninja
    dfu-util
    cmake
  ]);
  runScript = ''
    bash -c '\
    export ZEPHYR_BASE=$HOME/mikrokontroller/zephyrproject/zephyr; \
    export ZEPHYR_TOOLCHAIN_VARIANT="espressif"; \
    export ESPRESSIF_TOOLCHAIN_PATH="$HOME/.espressif/tools/xtensa-esp32-elf/esp-2022r1-11.2.0/xtensa-esp32-elf"; \
    export PATH=$PATH:$ESPRESSIF_TOOLCHAIN_PATH/bin; \
    bash'
  '';
}).env

