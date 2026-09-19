{
  description = "A C++ library and a command line tool for Nostr";

  inputs = {
    # Latest stable Nixpkgs
    nixpkgs.url = "github:nixos/nixpkgs";
    self.submodules = true;
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
                bash
                cmake
                pkg-config
                ninja
                ccache
                gdb
                gtest
                which
              ];
              cppDependencies = with pkgs; [
                boost
                openssl
                secp256k1
              ];
            in
            pkgs.stdenv.mkDerivation {
              name = "sonos";
              src = self;
              nativeBuildInputs = devTools;
              buildInputs = cppDependencies;
            };
        }
      );
    };
}
