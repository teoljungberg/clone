{pkgs ? import <nixpkgs> {}}: let
  stdenv = pkgs.stdenv;
in
  stdenv.mkDerivation rec {
    pname = "clone";
    version = "0.0.1";
    name = "${pname}-${version}";
    src = ./.;

    buildInputs = [pkgs.clang pkgs.mandoc];

    installPhase = ''
      make install PREFIX=$out
    '';
  }
