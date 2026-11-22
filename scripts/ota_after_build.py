"""
PlatformIO post-action: OTA-flash firmware.bin via the monorepo fleet tool.

See scripts/status/fleet_ota_hook.py in ServerBootShutdownManagemement.
SKIP_FLEET_OTA=1 leaves the .bin without calling the fleet tool.
"""

Import("env")  # type: ignore[name-defined]  # noqa: F821 — PlatformIO injects this

import sys
from pathlib import Path

DEVICE_ID = "PlatformIO_ESP8266_Main_Entry"


def _standalone(source, target, env):  # noqa: ARG001
    print(
        "ota_after_build: fleet hook not found (not a monorepo submodule); "
        "leaving {}".format(target[0])
    )


project_dir = Path(env["PROJECT_DIR"]).resolve()  # type: ignore[name-defined]
hook_dir = project_dir.parent.parent / "scripts" / "status"
hook = hook_dir / "fleet_ota_hook.py"
if hook.is_file():
    sys.path.insert(0, str(hook_dir))
    from fleet_ota_hook import register

    register(env, DEVICE_ID)  # type: ignore[name-defined]
else:
    env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", _standalone)  # type: ignore[name-defined]
