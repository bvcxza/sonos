{
  description = "A C++ library and a command line tool for Nostr";

  inputs = {
    # Latest stable Nixpkgs
    nixpkgs.url = "https://flakehub.com/f/NixOS/nixpkgs/0";
  };

  outputs =
    { self, nixpkgs }:
    let
      # Systems supported
      allSystems = [
        "x86_64-linux" # 64-bit Intel/AMD Linux
        "aarch64-linux" # 64-bit ARM Linux
        "x86_64-darwin" # 64-bit Intel macOS
        "aarch64-darwin" # 64-bit ARM macOS
      ];

      # Helper to provide system-specific attributes
      forAllSystems =
        f:
        nixpkgs.lib.genAttrs allSystems (
          system:
          f {
            pkgs = import nixpkgs { inherit system; };
          }
        );
    in
    {
      packages = forAllSystems (
        { pkgs }:
        {
          default =
            let
              binName = "sonos";
              devTools = with pkgs; [
                cmake
                gnumake
                pkg-config
                ninja
                ccache
                gdb
                gtest
              ];
              cppDependencies = with pkgs; [
                boost
                openssl
              ];
            in
            pkgs.stdenv.mkDerivation {
              name = "sonos";
              src = self;
              nativeBuildInputs = devTools;
              buildInputs = cppDependencies;
#              buildPhase = "c++ -std=c++17 -o ${binName} ${./main.cpp} -lPocoFoundation -lboost_system";
#              installPhase = ''
#                mkdir -p $out/bin
#                cp ${binName} $out/bin/
#              '';
            };
        }
      );
    };
}
