{ pkgs ? import <nixpkgs> {} }:
let
  xtensa-esp32-elf = pkgs.callPackage /etc/nixos/nix-config/packages/esp-toolchain {};
in
pkgs.mkShell {
  buildInputs = with pkgs; [
    (python39.withPackages(ps: with ps; [
      bleak
      west
      pyelftools
      pyserial
      pygatt
    ]))
    xtensa-esp32-elf
    openocd
    dtc
    ninja
    dfu-util
    cmake
  ];
  shellHook = ''
    export ZEPHYR_BASE=/home/simen/mikrokontroller/zephyrproject/zephyr

    export ZEPHYR_TOOLCHAIN_VARIANT="espressif"
    export ESPRESSIF_TOOLCHAIN_PATH=${xtensa-esp32-elf}/xtensa-esp32-elf

  '';
}
