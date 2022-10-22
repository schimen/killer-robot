{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    nativeBuildInputs = with pkgs;[ 
        (python39.withPackages(ps: with ps; [
            tkinter
            pyserial
	    bleak
        ]))
    ];
}
