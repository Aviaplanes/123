"""Cross-build the standalone Windows EXE with: python -m pip install ziglang==0.14.1"""
from pathlib import Path
import hashlib
import subprocess
import sys

source = Path(__file__).resolve().parent
output = source.parent / "MemoryGame.exe"
subprocess.run([
    sys.executable, "-m", "ziglang", "cc", "-target", "x86_64-windows-gnu",
    "-std=c11", "-Os", "-s", "-Wall", "-Wextra", "-Werror", "-municode",
    "-Wl,--subsystem,windows", str(source / "MemoryGame.c"),
    "-o", str(output), "-luser32", "-lgdi32",
], check=True)
digest = hashlib.sha256(output.read_bytes()).hexdigest()
output.with_suffix(".exe.sha256").write_text(f"{digest}  {output.name}\n", encoding="ascii")
print(f"Built {output.name}: {output.stat().st_size} bytes; SHA-256: {digest}")
