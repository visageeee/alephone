# Sprintathon

It's a Marathon, but also a sprint! Sprintathon is a gameplay-focused fork of [Aleph One](https://github.com/Aleph-One-Marathon/alephone) that modernizes the movement, aiming, and first-person feel of the Marathon engine while retaining compatibility with Marathon scenarios.

## Features

All gameplay additions can be configured from the **Sprintathon** preferences panel.

- Adjustable vertical mouselook range. The ability to look further up and down than in the vanilla Marathon experience can create graphical glitches but also makes for a much more modern mouselook "feel" and allow for some more advanced movement combined with the other Sprintathon enhancements.
- Improved first-person camera perspective
- Smoother mouse aiming
- Jumping with coyote time
- Crouching and crouch long-jumps
- Timed sprinting with cooldown
- Lowered weapon and enhanced weapon sway while sprinting
- Wall-running and wall-jumping
- Modernized swimming and surface movement
- Water and dry ledge-grabbing
- Optional global fog

Movement features can be enabled or disabled individually, allowing Sprintathon to range from mostly traditional Marathon movement to the complete modernized movement set.

## Building on Ubuntu

Install the build dependencies:

```bash
sudo apt update
sudo apt install \
  build-essential autoconf automake libtool pkg-config \
  libboost-all-dev libasio-dev \
  libsdl2-dev libsdl2-image-dev libsdl2-net-dev libsdl2-ttf-dev \
  libpng-dev libjpeg-dev libcurl4-openssl-dev \
  libzzip-dev libspeexdsp-dev libsndfile1-dev
````

Configure and compile:

```bash
./autogen.sh
./configure
make -j"$(nproc)"
```

The resulting executable is normally:

```text
Source_Files/alephone
```

## Running

Aleph One requires Marathon scenario data, including files such as `Map`, `Shapes`, `Sounds`, and `Images`.

Pass the scenario directory to the locally compiled executable:

```bash
./Source_Files/alephone ~/Games/Marathon
```

Be sure to run `./Source_Files/alephone`, not an older system installation at `/usr/local/bin/alephone`.

## Controls

Configure bindings from the in-game keyboard preferences. Sprintathon adds or repurposes bindings for:

* Jump / Swim
* Crouch
* Sprint

The exact keys are user-configurable.

## Configuration

Open Preferences and select the **Sprintathon** section. It contains:

* A master movement toggle
* Individual movement-feature toggles
* Several extended-mouselook limits, including nearly full upward and downward aiming

Global fog is configured separately in the OpenGL graphics preferences.

## Project status

Sprintathon is experimental. Gameplay behavior, networking compatibility, saved preferences, and scenario-specific interactions may still require testing.

Bug reports should include:

* The scenario and level
* Relevant Sprintathon options
* Steps to reproduce
* Build output or terminal errors
* Whether the issue occurs in upstream Aleph One

## Upstream project

Sprintathon is based on Aleph One, the open-source continuation of the Marathon engine.

For upstream documentation, licensing, credits, and platform-specific build information, see [README_ALEPHONE.md](README_ALEPHONE.md) and the [Aleph One project](https://github.com/Aleph-One-Marathon/alephone).

## License

Sprintathon retains Aleph One's existing licensing. See the repository's license and copyright files for details.
EOF

git add README.md README_ALEPHONE.md
git commit -m "Add Sprintathon project README"
git push

```

This preserves all upstream documentation and makes the fork’s purpose immediately clear on its GitHub front page.
```

