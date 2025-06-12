{
  description = "PeerSpeak flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-25.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs {
        inherit system;
      };

      mkDevShell = p: p;
    in
    rec {
      packages = {
        peerspeak = pkgs.stdenv.mkDerivation {
          pname = "PeerSpeak"; 
          version = "0.1.0";
          src = ./.;

          nativeBuildInputs = [ pkgs.cmake pkgs.clang ];
          buildInputs = with pkgs; [
            sdl3
            openssl
            gpgme
            sqlite
            sqlitecpp
            asio
            imgui
            xorg.libX11
            xorg.libXcursor
            xorg.libXrandr
            xorg.libXi
          ];

          cmakeFlags = [ "-DCMAKE_BUILD_TYPE=Release" ];

          inherit (pkgs) cmake cmakeBuild;
        };

        default = "PeerSpeak";
      };

      devShells = {
        default = mkDevShell (pkgs.mkShell {
          buildInputs = with pkgs; [
            cmake
            clang
            pkg-config
            sdl3
            openssl
            gpgme
            sqlite
            sqlitecpp
            asio
            imgui
            xorg.libX11
            xorg.libXcursor
            xorg.libXrandr
            xorg.libXi
          ];
        });
      };

      defaultPackage = packages.default;
      defaultDevShell = devShells.default;
    });
}

